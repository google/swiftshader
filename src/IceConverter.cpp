//===- subzero/src/IceConverter.cpp - Converts LLVM to Ice  ---------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LLVM to ICE converter.
//
//===----------------------------------------------------------------------===//

#include "IceConverter.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"
#include "IceTypes.h"
#include "IceTypeConverter.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <iostream>

using namespace llvm;

namespace {

// Debugging helper
template <typename T> static std::string LLVMObjectAsString(const T *O) {
  std::string Dump;
  raw_string_ostream Stream(Dump);
  O->print(Stream);
  return Stream.str();
}

// Converter from LLVM to ICE. The entry point is the convertFunction method.
//
// Note: this currently assumes that the given IR was verified to be valid PNaCl
// bitcode:
// https://developers.google.com/native-client/dev/reference/pnacl-bitcode-abi
// If not, all kinds of assertions may fire.
//
class LLVM2ICEConverter {
public:
  LLVM2ICEConverter(Ice::GlobalContext *Ctx, LLVMContext &LLVMContext)
      : Ctx(Ctx), Func(NULL), CurrentNode(NULL), TypeConverter(LLVMContext) {}

  // Caller is expected to delete the returned Ice::Cfg object.
  Ice::Cfg *convertFunction(const Function *F) {
    VarMap.clear();
    NodeMap.clear();
    Func = new Ice::Cfg(Ctx);
    Func->setFunctionName(F->getName());
    Func->setReturnType(convertToIceType(F->getReturnType()));
    Func->setInternal(F->hasInternalLinkage());

    // The initial definition/use of each arg is the entry node.
    CurrentNode = mapBasicBlockToNode(&F->getEntryBlock());
    for (Function::const_arg_iterator ArgI = F->arg_begin(),
                                      ArgE = F->arg_end();
         ArgI != ArgE; ++ArgI) {
      Func->addArg(mapValueToIceVar(ArgI));
    }

    // Make an initial pass through the block list just to resolve the
    // blocks in the original linearized order.  Otherwise the ICE
    // linearized order will be affected by branch targets in
    // terminator instructions.
    for (Function::const_iterator BBI = F->begin(), BBE = F->end(); BBI != BBE;
         ++BBI) {
      mapBasicBlockToNode(BBI);
    }
    for (Function::const_iterator BBI = F->begin(), BBE = F->end(); BBI != BBE;
         ++BBI) {
      CurrentNode = mapBasicBlockToNode(BBI);
      convertBasicBlock(BBI);
    }
    Func->setEntryNode(mapBasicBlockToNode(&F->getEntryBlock()));
    Func->computePredecessors();

    return Func;
  }

  // convertConstant() does not use Func or require it to be a valid
  // Ice::Cfg pointer.  As such, it's suitable for e.g. constructing
  // global initializers.
  Ice::Constant *convertConstant(const Constant *Const) {
    if (const GlobalValue *GV = dyn_cast<GlobalValue>(Const)) {
      return Ctx->getConstantSym(convertToIceType(GV->getType()), 0,
                                 GV->getName());
    } else if (const ConstantInt *CI = dyn_cast<ConstantInt>(Const)) {
      Ice::Type Ty = convertToIceType(CI->getType());
      if (Ty == Ice::IceType_i64) {
        return Ctx->getConstantInt64(Ty, CI->getSExtValue());
      } else {
        return Ctx->getConstantInt32(Ty, CI->getSExtValue());
      }
    } else if (const ConstantFP *CFP = dyn_cast<ConstantFP>(Const)) {
      Ice::Type Type = convertToIceType(CFP->getType());
      if (Type == Ice::IceType_f32)
        return Ctx->getConstantFloat(CFP->getValueAPF().convertToFloat());
      else if (Type == Ice::IceType_f64)
        return Ctx->getConstantDouble(CFP->getValueAPF().convertToDouble());
      llvm_unreachable("Unexpected floating point type");
      return NULL;
    } else if (const UndefValue *CU = dyn_cast<UndefValue>(Const)) {
      return Ctx->getConstantUndef(convertToIceType(CU->getType()));
    } else {
      llvm_unreachable("Unhandled constant type");
      return NULL;
    }
  }

private:
  // LLVM values (instructions, etc.) are mapped directly to ICE variables.
  // mapValueToIceVar has a version that forces an ICE type on the variable,
  // and a version that just uses convertToIceType on V.
  Ice::Variable *mapValueToIceVar(const Value *V, Ice::Type IceTy) {
    if (IceTy == Ice::IceType_void)
      return NULL;
    if (VarMap.find(V) == VarMap.end()) {
      assert(CurrentNode);
      VarMap[V] = Func->makeVariable(IceTy, CurrentNode, V->getName());
    }
    return VarMap[V];
  }

  Ice::Variable *mapValueToIceVar(const Value *V) {
    return mapValueToIceVar(V, convertToIceType(V->getType()));
  }

  Ice::CfgNode *mapBasicBlockToNode(const BasicBlock *BB) {
    if (NodeMap.find(BB) == NodeMap.end()) {
      NodeMap[BB] = Func->makeNode(BB->getName());
    }
    return NodeMap[BB];
  }

  Ice::Type convertToIceType(Type *LLVMTy) const {
    Ice::Type IceTy = TypeConverter.convertToIceType(LLVMTy);
    if (IceTy == Ice::IceType_NUM)
      llvm::report_fatal_error(std::string("Invalid PNaCl type ") +
                               LLVMObjectAsString(LLVMTy));
    return IceTy;
  }

  // Given an LLVM instruction and an operand number, produce the
  // Ice::Operand this refers to. If there's no such operand, return
  // NULL.
  Ice::Operand *convertOperand(const Instruction *Inst, unsigned OpNum) {
    if (OpNum >= Inst->getNumOperands()) {
      return NULL;
    }
    const Value *Op = Inst->getOperand(OpNum);
    return convertValue(Op);
  }

  Ice::Operand *convertValue(const Value *Op) {
    if (const Constant *Const = dyn_cast<Constant>(Op)) {
      return convertConstant(Const);
    } else {
      return mapValueToIceVar(Op);
    }
  }

  // Note: this currently assumes a 1x1 mapping between LLVM IR and Ice
  // instructions.
  Ice::Inst *convertInstruction(const Instruction *Inst) {
    switch (Inst->getOpcode()) {
    case Instruction::PHI:
      return convertPHINodeInstruction(cast<PHINode>(Inst));
    case Instruction::Br:
      return convertBrInstruction(cast<BranchInst>(Inst));
    case Instruction::Ret:
      return convertRetInstruction(cast<ReturnInst>(Inst));
    case Instruction::IntToPtr:
      return convertIntToPtrInstruction(cast<IntToPtrInst>(Inst));
    case Instruction::PtrToInt:
      return convertPtrToIntInstruction(cast<PtrToIntInst>(Inst));
    case Instruction::ICmp:
      return convertICmpInstruction(cast<ICmpInst>(Inst));
    case Instruction::FCmp:
      return convertFCmpInstruction(cast<FCmpInst>(Inst));
    case Instruction::Select:
      return convertSelectInstruction(cast<SelectInst>(Inst));
    case Instruction::Switch:
      return convertSwitchInstruction(cast<SwitchInst>(Inst));
    case Instruction::Load:
      return convertLoadInstruction(cast<LoadInst>(Inst));
    case Instruction::Store:
      return convertStoreInstruction(cast<StoreInst>(Inst));
    case Instruction::ZExt:
      return convertCastInstruction(cast<ZExtInst>(Inst), Ice::InstCast::Zext);
    case Instruction::SExt:
      return convertCastInstruction(cast<SExtInst>(Inst), Ice::InstCast::Sext);
    case Instruction::Trunc:
      return convertCastInstruction(cast<TruncInst>(Inst),
                                    Ice::InstCast::Trunc);
    case Instruction::FPTrunc:
      return convertCastInstruction(cast<FPTruncInst>(Inst),
                                    Ice::InstCast::Fptrunc);
    case Instruction::FPExt:
      return convertCastInstruction(cast<FPExtInst>(Inst),
                                    Ice::InstCast::Fpext);
    case Instruction::FPToSI:
      return convertCastInstruction(cast<FPToSIInst>(Inst),
                                    Ice::InstCast::Fptosi);
    case Instruction::FPToUI:
      return convertCastInstruction(cast<FPToUIInst>(Inst),
                                    Ice::InstCast::Fptoui);
    case Instruction::SIToFP:
      return convertCastInstruction(cast<SIToFPInst>(Inst),
                                    Ice::InstCast::Sitofp);
    case Instruction::UIToFP:
      return convertCastInstruction(cast<UIToFPInst>(Inst),
                                    Ice::InstCast::Uitofp);
    case Instruction::BitCast:
      return convertCastInstruction(cast<BitCastInst>(Inst),
                                    Ice::InstCast::Bitcast);
    case Instruction::Add:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Add);
    case Instruction::Sub:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Sub);
    case Instruction::Mul:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Mul);
    case Instruction::UDiv:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Udiv);
    case Instruction::SDiv:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Sdiv);
    case Instruction::URem:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Urem);
    case Instruction::SRem:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Srem);
    case Instruction::Shl:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Shl);
    case Instruction::LShr:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Lshr);
    case Instruction::AShr:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Ashr);
    case Instruction::FAdd:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Fadd);
    case Instruction::FSub:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Fsub);
    case Instruction::FMul:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Fmul);
    case Instruction::FDiv:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Fdiv);
    case Instruction::FRem:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Frem);
    case Instruction::And:
      return convertArithInstruction(Inst, Ice::InstArithmetic::And);
    case Instruction::Or:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Or);
    case Instruction::Xor:
      return convertArithInstruction(Inst, Ice::InstArithmetic::Xor);
    case Instruction::ExtractElement:
      return convertExtractElementInstruction(cast<ExtractElementInst>(Inst));
    case Instruction::InsertElement:
      return convertInsertElementInstruction(cast<InsertElementInst>(Inst));
    case Instruction::Call:
      return convertCallInstruction(cast<CallInst>(Inst));
    case Instruction::Alloca:
      return convertAllocaInstruction(cast<AllocaInst>(Inst));
    case Instruction::Unreachable:
      return convertUnreachableInstruction(cast<UnreachableInst>(Inst));
    default:
      report_fatal_error(std::string("Invalid PNaCl instruction: ") +
                         LLVMObjectAsString(Inst));
    }

    llvm_unreachable("convertInstruction");
    return NULL;
  }

  Ice::Inst *convertLoadInstruction(const LoadInst *Inst) {
    Ice::Operand *Src = convertOperand(Inst, 0);
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    return Ice::InstLoad::create(Func, Dest, Src);
  }

  Ice::Inst *convertStoreInstruction(const StoreInst *Inst) {
    Ice::Operand *Addr = convertOperand(Inst, 1);
    Ice::Operand *Val = convertOperand(Inst, 0);
    return Ice::InstStore::create(Func, Val, Addr);
  }

  Ice::Inst *convertArithInstruction(const Instruction *Inst,
                                     Ice::InstArithmetic::OpKind Opcode) {
    const BinaryOperator *BinOp = cast<BinaryOperator>(Inst);
    Ice::Operand *Src0 = convertOperand(Inst, 0);
    Ice::Operand *Src1 = convertOperand(Inst, 1);
    Ice::Variable *Dest = mapValueToIceVar(BinOp);
    return Ice::InstArithmetic::create(Func, Opcode, Dest, Src0, Src1);
  }

  Ice::Inst *convertPHINodeInstruction(const PHINode *Inst) {
    unsigned NumValues = Inst->getNumIncomingValues();
    Ice::InstPhi *IcePhi =
        Ice::InstPhi::create(Func, NumValues, mapValueToIceVar(Inst));
    for (unsigned N = 0, E = NumValues; N != E; ++N) {
      IcePhi->addArgument(convertOperand(Inst, N),
                          mapBasicBlockToNode(Inst->getIncomingBlock(N)));
    }
    return IcePhi;
  }

  Ice::Inst *convertBrInstruction(const BranchInst *Inst) {
    if (Inst->isConditional()) {
      Ice::Operand *Src = convertOperand(Inst, 0);
      BasicBlock *BBThen = Inst->getSuccessor(0);
      BasicBlock *BBElse = Inst->getSuccessor(1);
      Ice::CfgNode *NodeThen = mapBasicBlockToNode(BBThen);
      Ice::CfgNode *NodeElse = mapBasicBlockToNode(BBElse);
      return Ice::InstBr::create(Func, Src, NodeThen, NodeElse);
    } else {
      BasicBlock *BBSucc = Inst->getSuccessor(0);
      return Ice::InstBr::create(Func, mapBasicBlockToNode(BBSucc));
    }
  }

  Ice::Inst *convertIntToPtrInstruction(const IntToPtrInst *Inst) {
    Ice::Operand *Src = convertOperand(Inst, 0);
    Ice::Variable *Dest =
        mapValueToIceVar(Inst, TypeConverter.getIcePointerType());
    return Ice::InstAssign::create(Func, Dest, Src);
  }

  Ice::Inst *convertPtrToIntInstruction(const PtrToIntInst *Inst) {
    Ice::Operand *Src = convertOperand(Inst, 0);
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    return Ice::InstAssign::create(Func, Dest, Src);
  }

  Ice::Inst *convertRetInstruction(const ReturnInst *Inst) {
    Ice::Operand *RetOperand = convertOperand(Inst, 0);
    if (RetOperand) {
      return Ice::InstRet::create(Func, RetOperand);
    } else {
      return Ice::InstRet::create(Func);
    }
  }

  Ice::Inst *convertCastInstruction(const Instruction *Inst,
                                    Ice::InstCast::OpKind CastKind) {
    Ice::Operand *Src = convertOperand(Inst, 0);
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    return Ice::InstCast::create(Func, CastKind, Dest, Src);
  }

  Ice::Inst *convertICmpInstruction(const ICmpInst *Inst) {
    Ice::Operand *Src0 = convertOperand(Inst, 0);
    Ice::Operand *Src1 = convertOperand(Inst, 1);
    Ice::Variable *Dest = mapValueToIceVar(Inst);

    Ice::InstIcmp::ICond Cond;
    switch (Inst->getPredicate()) {
    default:
      llvm_unreachable("ICmpInst predicate");
    case CmpInst::ICMP_EQ:
      Cond = Ice::InstIcmp::Eq;
      break;
    case CmpInst::ICMP_NE:
      Cond = Ice::InstIcmp::Ne;
      break;
    case CmpInst::ICMP_UGT:
      Cond = Ice::InstIcmp::Ugt;
      break;
    case CmpInst::ICMP_UGE:
      Cond = Ice::InstIcmp::Uge;
      break;
    case CmpInst::ICMP_ULT:
      Cond = Ice::InstIcmp::Ult;
      break;
    case CmpInst::ICMP_ULE:
      Cond = Ice::InstIcmp::Ule;
      break;
    case CmpInst::ICMP_SGT:
      Cond = Ice::InstIcmp::Sgt;
      break;
    case CmpInst::ICMP_SGE:
      Cond = Ice::InstIcmp::Sge;
      break;
    case CmpInst::ICMP_SLT:
      Cond = Ice::InstIcmp::Slt;
      break;
    case CmpInst::ICMP_SLE:
      Cond = Ice::InstIcmp::Sle;
      break;
    }

    return Ice::InstIcmp::create(Func, Cond, Dest, Src0, Src1);
  }

  Ice::Inst *convertFCmpInstruction(const FCmpInst *Inst) {
    Ice::Operand *Src0 = convertOperand(Inst, 0);
    Ice::Operand *Src1 = convertOperand(Inst, 1);
    Ice::Variable *Dest = mapValueToIceVar(Inst);

    Ice::InstFcmp::FCond Cond;
    switch (Inst->getPredicate()) {

    default:
      llvm_unreachable("FCmpInst predicate");

    case CmpInst::FCMP_FALSE:
      Cond = Ice::InstFcmp::False;
      break;
    case CmpInst::FCMP_OEQ:
      Cond = Ice::InstFcmp::Oeq;
      break;
    case CmpInst::FCMP_OGT:
      Cond = Ice::InstFcmp::Ogt;
      break;
    case CmpInst::FCMP_OGE:
      Cond = Ice::InstFcmp::Oge;
      break;
    case CmpInst::FCMP_OLT:
      Cond = Ice::InstFcmp::Olt;
      break;
    case CmpInst::FCMP_OLE:
      Cond = Ice::InstFcmp::Ole;
      break;
    case CmpInst::FCMP_ONE:
      Cond = Ice::InstFcmp::One;
      break;
    case CmpInst::FCMP_ORD:
      Cond = Ice::InstFcmp::Ord;
      break;
    case CmpInst::FCMP_UEQ:
      Cond = Ice::InstFcmp::Ueq;
      break;
    case CmpInst::FCMP_UGT:
      Cond = Ice::InstFcmp::Ugt;
      break;
    case CmpInst::FCMP_UGE:
      Cond = Ice::InstFcmp::Uge;
      break;
    case CmpInst::FCMP_ULT:
      Cond = Ice::InstFcmp::Ult;
      break;
    case CmpInst::FCMP_ULE:
      Cond = Ice::InstFcmp::Ule;
      break;
    case CmpInst::FCMP_UNE:
      Cond = Ice::InstFcmp::Une;
      break;
    case CmpInst::FCMP_UNO:
      Cond = Ice::InstFcmp::Uno;
      break;
    case CmpInst::FCMP_TRUE:
      Cond = Ice::InstFcmp::True;
      break;
    }

    return Ice::InstFcmp::create(Func, Cond, Dest, Src0, Src1);
  }

  Ice::Inst *convertExtractElementInstruction(const ExtractElementInst *Inst) {
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    Ice::Operand *Source1 = convertValue(Inst->getOperand(0));
    Ice::Operand *Source2 = convertValue(Inst->getOperand(1));
    return Ice::InstExtractElement::create(Func, Dest, Source1, Source2);
  }

  Ice::Inst *convertInsertElementInstruction(const InsertElementInst *Inst) {
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    Ice::Operand *Source1 = convertValue(Inst->getOperand(0));
    Ice::Operand *Source2 = convertValue(Inst->getOperand(1));
    Ice::Operand *Source3 = convertValue(Inst->getOperand(2));
    return Ice::InstInsertElement::create(Func, Dest, Source1, Source2,
                                          Source3);
  }

  Ice::Inst *convertSelectInstruction(const SelectInst *Inst) {
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    Ice::Operand *Cond = convertValue(Inst->getCondition());
    Ice::Operand *Source1 = convertValue(Inst->getTrueValue());
    Ice::Operand *Source2 = convertValue(Inst->getFalseValue());
    return Ice::InstSelect::create(Func, Dest, Cond, Source1, Source2);
  }

  Ice::Inst *convertSwitchInstruction(const SwitchInst *Inst) {
    Ice::Operand *Source = convertValue(Inst->getCondition());
    Ice::CfgNode *LabelDefault = mapBasicBlockToNode(Inst->getDefaultDest());
    unsigned NumCases = Inst->getNumCases();
    Ice::InstSwitch *Switch =
        Ice::InstSwitch::create(Func, NumCases, Source, LabelDefault);
    unsigned CurrentCase = 0;
    for (SwitchInst::ConstCaseIt I = Inst->case_begin(), E = Inst->case_end();
         I != E; ++I, ++CurrentCase) {
      uint64_t CaseValue = I.getCaseValue()->getSExtValue();
      Ice::CfgNode *CaseSuccessor = mapBasicBlockToNode(I.getCaseSuccessor());
      Switch->addBranch(CurrentCase, CaseValue, CaseSuccessor);
    }
    return Switch;
  }

  Ice::Inst *convertCallInstruction(const CallInst *Inst) {
    Ice::Variable *Dest = mapValueToIceVar(Inst);
    Ice::Operand *CallTarget = convertValue(Inst->getCalledValue());
    unsigned NumArgs = Inst->getNumArgOperands();
    // Note: Subzero doesn't (yet) do anything special with the Tail
    // flag in the bitcode, i.e. CallInst::isTailCall().
    Ice::InstCall *NewInst = NULL;
    const Ice::Intrinsics::FullIntrinsicInfo *Info = NULL;

    if (Ice::ConstantRelocatable *Target =
            llvm::dyn_cast<Ice::ConstantRelocatable>(CallTarget)) {
      // Check if this direct call is to an Intrinsic (starts with "llvm.")
      static const char LLVMPrefix[] = "llvm.";
      const size_t LLVMPrefixLen = strlen(LLVMPrefix);
      Ice::IceString Name = Target->getName();
      if (Name.substr(0, LLVMPrefixLen) == LLVMPrefix) {
        Ice::IceString NameSuffix = Name.substr(LLVMPrefixLen);
        Info = Ctx->getIntrinsicsInfo().find(NameSuffix);
        if (!Info) {
          report_fatal_error(std::string("Invalid PNaCl intrinsic call: ") +
                             LLVMObjectAsString(Inst));
        }
        NewInst = Ice::InstIntrinsicCall::create(Func, NumArgs, Dest,
                                                 CallTarget, Info->Info);
      }
    }

    // Not an intrinsic call.
    if (NewInst == NULL) {
      NewInst = Ice::InstCall::create(Func, NumArgs, Dest, CallTarget,
                                      Inst->isTailCall());
    }
    for (unsigned i = 0; i < NumArgs; ++i) {
      NewInst->addArg(convertOperand(Inst, i));
    }
    if (Info) {
      validateIntrinsicCall(NewInst, Info);
    }
    return NewInst;
  }

  Ice::Inst *convertAllocaInstruction(const AllocaInst *Inst) {
    // PNaCl bitcode only contains allocas of byte-granular objects.
    Ice::Operand *ByteCount = convertValue(Inst->getArraySize());
    uint32_t Align = Inst->getAlignment();
    Ice::Variable *Dest =
        mapValueToIceVar(Inst, TypeConverter.getIcePointerType());

    return Ice::InstAlloca::create(Func, ByteCount, Align, Dest);
  }

  Ice::Inst *convertUnreachableInstruction(const UnreachableInst * /*Inst*/) {
    return Ice::InstUnreachable::create(Func);
  }

  Ice::CfgNode *convertBasicBlock(const BasicBlock *BB) {
    Ice::CfgNode *Node = mapBasicBlockToNode(BB);
    for (BasicBlock::const_iterator II = BB->begin(), II_e = BB->end();
         II != II_e; ++II) {
      Ice::Inst *Inst = convertInstruction(II);
      Node->appendInst(Inst);
    }
    return Node;
  }

  void validateIntrinsicCall(const Ice::InstCall *Call,
                             const Ice::Intrinsics::FullIntrinsicInfo *I) {
    Ice::SizeT ArgIndex = 0;
    switch (I->validateCall(Call, ArgIndex)) {
    default:
      report_fatal_error("Unknown validation error for intrinsic call");
      break;
    case Ice::Intrinsics::IsValidCall:
      break;
    case Ice::Intrinsics::BadReturnType: {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Intrinsic call expects return type " << I->getReturnType()
             << ". Found: " << Call->getReturnType();
      report_fatal_error(StrBuf.str());
      break;
    }
    case Ice::Intrinsics::WrongNumOfArgs: {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Intrinsic call expects " << I->getNumArgs()
             << ". Found: " << Call->getNumArgs();
      report_fatal_error(StrBuf.str());
      break;
    }
    case Ice::Intrinsics::WrongCallArgType: {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Intrinsic call argument " << ArgIndex << " expects type "
             << I->getArgType(ArgIndex)
             << ". Found: " << Call->getArg(ArgIndex)->getType();
      report_fatal_error(StrBuf.str());
      break;
    }
    }
  }

private:
  // Data
  Ice::GlobalContext *Ctx;
  Ice::Cfg *Func;
  Ice::CfgNode *CurrentNode;
  std::map<const Value *, Ice::Variable *> VarMap;
  std::map<const BasicBlock *, Ice::CfgNode *> NodeMap;
  Ice::TypeConverter TypeConverter;
};

} // end of anonymous namespace

namespace Ice {

void Converter::convertToIce() {
  nameUnnamedGlobalAddresses(Mod);
  if (!Ctx->getFlags().DisableGlobals)
    convertGlobals(Mod);
  convertFunctions();
}

void Converter::convertFunctions() {
  for (Module::const_iterator I = Mod->begin(), E = Mod->end(); I != E; ++I) {
    if (I->empty())
      continue;
    LLVM2ICEConverter FunctionConverter(Ctx, Mod->getContext());

    Timer TConvert;
    Cfg *Fcn = FunctionConverter.convertFunction(I);
    if (Ctx->getFlags().SubzeroTimingEnabled) {
      std::cerr << "[Subzero timing] Convert function "
                << Fcn->getFunctionName() << ": " << TConvert.getElapsedSec()
                << " sec\n";
    }
    translateFcn(Fcn);
  }

  emitConstants();
}

} // end of namespace Ice
