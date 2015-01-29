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

#include <iostream>

#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceConverter.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceGlobalInits.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"
#include "IceTypes.h"
#include "IceTypeConverter.h"

// TODO(kschimpf): Remove two namespaces being visible at once.
using namespace llvm;

namespace {

// Debugging helper
template <typename T> static std::string LLVMObjectAsString(const T *O) {
  std::string Dump;
  raw_string_ostream Stream(Dump);
  O->print(Stream);
  return Stream.str();
}

// Base class for converting LLVM to ICE.
class LLVM2ICEConverter {
  LLVM2ICEConverter(const LLVM2ICEConverter &) = delete;
  LLVM2ICEConverter &operator=(const LLVM2ICEConverter &) = delete;

public:
  LLVM2ICEConverter(Ice::Converter &Converter)
      : Converter(Converter), Ctx(Converter.getContext()),
        TypeConverter(Converter.getModule()->getContext()) {}

  Ice::Converter &getConverter() const { return Converter; }

protected:
  Ice::Converter &Converter;
  Ice::GlobalContext *Ctx;
  const Ice::TypeConverter TypeConverter;
};

// Converter from LLVM functions to ICE. The entry point is the
// convertFunction method.
//
// Note: this currently assumes that the given IR was verified to be
// valid PNaCl bitcode. Otherwise, the behavior is undefined.
class LLVM2ICEFunctionConverter : LLVM2ICEConverter {
  LLVM2ICEFunctionConverter(const LLVM2ICEFunctionConverter &) = delete;
  LLVM2ICEFunctionConverter &
  operator=(const LLVM2ICEFunctionConverter &) = delete;

public:
  LLVM2ICEFunctionConverter(Ice::Converter &Converter)
      : LLVM2ICEConverter(Converter), Func(nullptr) {}

  // Caller is expected to delete the returned Ice::Cfg object.
  Ice::Cfg *convertFunction(const Function *F) {
    VarMap.clear();
    NodeMap.clear();
    Func = Ice::Cfg::create(Ctx);
    Func->setFunctionName(F->getName());
    Func->setReturnType(convertToIceType(F->getReturnType()));
    Func->setInternal(F->hasInternalLinkage());
    Ice::TimerMarker T(Ice::TimerStack::TT_llvmConvert, Func);

    // The initial definition/use of each arg is the entry node.
    for (auto ArgI = F->arg_begin(), ArgE = F->arg_end(); ArgI != ArgE;
         ++ArgI) {
      Func->addArg(mapValueToIceVar(ArgI));
    }

    // Make an initial pass through the block list just to resolve the
    // blocks in the original linearized order.  Otherwise the ICE
    // linearized order will be affected by branch targets in
    // terminator instructions.
    for (const BasicBlock &BBI : *F)
      mapBasicBlockToNode(&BBI);
    for (const BasicBlock &BBI : *F)
      convertBasicBlock(&BBI);
    Func->setEntryNode(mapBasicBlockToNode(&F->getEntryBlock()));
    Func->computePredecessors();

    return Func;
  }

  // convertConstant() does not use Func or require it to be a valid
  // Ice::Cfg pointer.  As such, it's suitable for e.g. constructing
  // global initializers.
  Ice::Constant *convertConstant(const Constant *Const) {
    if (const auto GV = dyn_cast<GlobalValue>(Const)) {
      Ice::GlobalDeclaration *Decl = getConverter().getGlobalDeclaration(GV);
      const Ice::RelocOffsetT Offset = 0;
      return Ctx->getConstantSym(Offset, Decl->getName(),
                                 Decl->getSuppressMangling());
    } else if (const auto CI = dyn_cast<ConstantInt>(Const)) {
      Ice::Type Ty = convertToIceType(CI->getType());
      return Ctx->getConstantInt(Ty, CI->getSExtValue());
    } else if (const auto CFP = dyn_cast<ConstantFP>(Const)) {
      Ice::Type Type = convertToIceType(CFP->getType());
      if (Type == Ice::IceType_f32)
        return Ctx->getConstantFloat(CFP->getValueAPF().convertToFloat());
      else if (Type == Ice::IceType_f64)
        return Ctx->getConstantDouble(CFP->getValueAPF().convertToDouble());
      llvm_unreachable("Unexpected floating point type");
      return nullptr;
    } else if (const auto CU = dyn_cast<UndefValue>(Const)) {
      return Ctx->getConstantUndef(convertToIceType(CU->getType()));
    } else {
      llvm_unreachable("Unhandled constant type");
      return nullptr;
    }
  }

private:
  // LLVM values (instructions, etc.) are mapped directly to ICE variables.
  // mapValueToIceVar has a version that forces an ICE type on the variable,
  // and a version that just uses convertToIceType on V.
  Ice::Variable *mapValueToIceVar(const Value *V, Ice::Type IceTy) {
    if (IceTy == Ice::IceType_void)
      return nullptr;
    if (VarMap.find(V) == VarMap.end()) {
      VarMap[V] = Func->makeVariable(IceTy);
      if (ALLOW_DUMP)
        VarMap[V]->setName(Func, V->getName());
    }
    return VarMap[V];
  }

  Ice::Variable *mapValueToIceVar(const Value *V) {
    return mapValueToIceVar(V, convertToIceType(V->getType()));
  }

  Ice::CfgNode *mapBasicBlockToNode(const BasicBlock *BB) {
    if (NodeMap.find(BB) == NodeMap.end()) {
      NodeMap[BB] = Func->makeNode();
      if (ALLOW_DUMP)
        NodeMap[BB]->setName(BB->getName());
    }
    return NodeMap[BB];
  }

  Ice::Type convertToIceType(Type *LLVMTy) const {
    Ice::Type IceTy = TypeConverter.convertToIceType(LLVMTy);
    if (IceTy == Ice::IceType_NUM)
      report_fatal_error(std::string("Invalid PNaCl type ") +
                         LLVMObjectAsString(LLVMTy));
    return IceTy;
  }

  // Given an LLVM instruction and an operand number, produce the
  // Ice::Operand this refers to. If there's no such operand, return
  // nullptr.
  Ice::Operand *convertOperand(const Instruction *Inst, unsigned OpNum) {
    if (OpNum >= Inst->getNumOperands()) {
      return nullptr;
    }
    const Value *Op = Inst->getOperand(OpNum);
    return convertValue(Op);
  }

  Ice::Operand *convertValue(const Value *Op) {
    if (const auto Const = dyn_cast<Constant>(Op)) {
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
    return nullptr;
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
    const auto BinOp = cast<BinaryOperator>(Inst);
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
    Ice::Variable *Dest = mapValueToIceVar(Inst, Ice::getPointerType());
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
    Ice::InstCall *NewInst = nullptr;
    const Ice::Intrinsics::FullIntrinsicInfo *Info = nullptr;

    if (const auto Target = dyn_cast<Ice::ConstantRelocatable>(CallTarget)) {
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
    if (NewInst == nullptr) {
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
    Ice::Variable *Dest = mapValueToIceVar(Inst, Ice::getPointerType());

    return Ice::InstAlloca::create(Func, ByteCount, Align, Dest);
  }

  Ice::Inst *convertUnreachableInstruction(const UnreachableInst * /*Inst*/) {
    return Ice::InstUnreachable::create(Func);
  }

  Ice::CfgNode *convertBasicBlock(const BasicBlock *BB) {
    Ice::CfgNode *Node = mapBasicBlockToNode(BB);
    for (const Instruction &II : *BB) {
      Ice::Inst *Inst = convertInstruction(&II);
      Node->appendInst(Inst);
    }
    return Node;
  }

  void validateIntrinsicCall(const Ice::InstCall *Call,
                             const Ice::Intrinsics::FullIntrinsicInfo *I) {
    Ice::SizeT ArgIndex = 0;
    switch (I->validateCall(Call, ArgIndex)) {
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
  Ice::Cfg *Func;
  std::map<const Value *, Ice::Variable *> VarMap;
  std::map<const BasicBlock *, Ice::CfgNode *> NodeMap;
};

// Converter from LLVM global variables to ICE. The entry point is the
// convertGlobalsToIce method.
//
// Note: this currently assumes that the given IR was verified to be
// valid PNaCl bitcode. Othewise, the behavior is undefined.
class LLVM2ICEGlobalsConverter : public LLVM2ICEConverter {
  LLVM2ICEGlobalsConverter(const LLVM2ICEGlobalsConverter &) = delete;
  LLVM2ICEGlobalsConverter &
  operator-(const LLVM2ICEGlobalsConverter &) = delete;

public:
  LLVM2ICEGlobalsConverter(Ice::Converter &Converter)
      : LLVM2ICEConverter(Converter) {}

  /// Converts global variables, and their initializers into ICE
  /// global variable declarations, for module Mod. Puts corresponding
  /// converted declarations into VariableDeclarations.
  void convertGlobalsToIce(Module *Mod,
                           Ice::VariableDeclarationList &VariableDeclarations);

private:
  // Adds the Initializer to the list of initializers for the Global
  // variable declaraation.
  void addGlobalInitializer(Ice::VariableDeclaration &Global,
                            const Constant *Initializer) {
    const bool HasOffset = false;
    const Ice::RelocOffsetT Offset = 0;
    addGlobalInitializer(Global, Initializer, HasOffset, Offset);
  }

  // Adds Initializer to the list of initializers for Global variable
  // declaration.  HasOffset is true only if Initializer is a
  // relocation initializer and Offset should be added to the
  // relocation.
  void addGlobalInitializer(Ice::VariableDeclaration &Global,
                            const Constant *Initializer, bool HasOffset,
                            Ice::RelocOffsetT Offset);

  // Converts the given constant C to the corresponding integer
  // literal it contains.
  Ice::RelocOffsetT getIntegerLiteralConstant(const Value *C) {
    const auto CI = dyn_cast<ConstantInt>(C);
    if (CI && CI->getType()->isIntegerTy(32))
      return CI->getSExtValue();

    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "Constant not i32 literal: " << *C;
    report_fatal_error(StrBuf.str());
    return 0;
  }
};

void LLVM2ICEGlobalsConverter::convertGlobalsToIce(
    Module *Mod, Ice::VariableDeclarationList &VariableDeclarations) {
  for (Module::const_global_iterator I = Mod->global_begin(),
                                     E = Mod->global_end();
       I != E; ++I) {

    const GlobalVariable *GV = I;

    Ice::GlobalDeclaration *Var = getConverter().getGlobalDeclaration(GV);
    Ice::VariableDeclaration *VarDecl = cast<Ice::VariableDeclaration>(Var);
    VariableDeclarations.push_back(VarDecl);

    if (!GV->hasInternalLinkage() && GV->hasInitializer()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Can't define external global declaration: " << GV->getName();
      report_fatal_error(StrBuf.str());
    }

    if (!GV->hasInitializer()) {
      if (Ctx->getFlags().AllowUninitializedGlobals)
        continue;
      else {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Global declaration missing initializer: " << GV->getName();
        report_fatal_error(StrBuf.str());
      }
    }

    const Constant *Initializer = GV->getInitializer();
    if (const auto CompoundInit = dyn_cast<ConstantStruct>(Initializer)) {
      for (ConstantStruct::const_op_iterator I = CompoundInit->op_begin(),
                                             E = CompoundInit->op_end();
           I != E; ++I) {
        if (const auto Init = dyn_cast<Constant>(I)) {
          addGlobalInitializer(*VarDecl, Init);
        }
      }
    } else {
      addGlobalInitializer(*VarDecl, Initializer);
    }
  }
}

void LLVM2ICEGlobalsConverter::addGlobalInitializer(
    Ice::VariableDeclaration &Global, const Constant *Initializer,
    bool HasOffset, Ice::RelocOffsetT Offset) {
  (void)HasOffset;
  assert(HasOffset || Offset == 0);

  if (const auto CDA = dyn_cast<ConstantDataArray>(Initializer)) {
    assert(!HasOffset && isa<IntegerType>(CDA->getElementType()) &&
           (cast<IntegerType>(CDA->getElementType())->getBitWidth() == 8));
    Global.addInitializer(new Ice::VariableDeclaration::DataInitializer(
        CDA->getRawDataValues().data(), CDA->getNumElements()));
    return;
  }

  if (isa<ConstantAggregateZero>(Initializer)) {
    if (const auto AT = dyn_cast<ArrayType>(Initializer->getType())) {
      assert(!HasOffset && isa<IntegerType>(AT->getElementType()) &&
             (cast<IntegerType>(AT->getElementType())->getBitWidth() == 8));
      Global.addInitializer(
          new Ice::VariableDeclaration::ZeroInitializer(AT->getNumElements()));
    } else {
      llvm_unreachable("Unhandled constant aggregate zero type");
    }
    return;
  }

  if (const auto Exp = dyn_cast<ConstantExpr>(Initializer)) {
    switch (Exp->getOpcode()) {
    case Instruction::Add:
      assert(!HasOffset);
      addGlobalInitializer(Global, Exp->getOperand(0), true,
                           getIntegerLiteralConstant(Exp->getOperand(1)));
      return;
    case Instruction::PtrToInt: {
      assert(TypeConverter.convertToIceType(Exp->getType()) ==
             Ice::getPointerType());
      const auto GV = dyn_cast<GlobalValue>(Exp->getOperand(0));
      assert(GV);
      const Ice::GlobalDeclaration *Addr =
          getConverter().getGlobalDeclaration(GV);
      Global.addInitializer(
          new Ice::VariableDeclaration::RelocInitializer(Addr, Offset));
      return;
    }
    default:
      break;
    }
  }

  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Unhandled global initializer: " << Initializer;
  report_fatal_error(StrBuf.str());
}

} // end of anonymous namespace

namespace Ice {

void Converter::nameUnnamedGlobalVariables(Module *Mod) {
  const IceString &GlobalPrefix = Flags.DefaultGlobalPrefix;
  if (GlobalPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  for (auto V = Mod->global_begin(), E = Mod->global_end(); V != E; ++V) {
    if (!V->hasName()) {
      V->setName(createUnnamedName(GlobalPrefix, NameIndex));
      ++NameIndex;
    } else {
      checkIfUnnamedNameSafe(V->getName(), "global", GlobalPrefix);
    }
  }
}

void Converter::nameUnnamedFunctions(Module *Mod) {
  const IceString &FunctionPrefix = Flags.DefaultFunctionPrefix;
  if (FunctionPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  for (Function &F : *Mod) {
    if (!F.hasName()) {
      F.setName(createUnnamedName(FunctionPrefix, NameIndex));
      ++NameIndex;
    } else {
      checkIfUnnamedNameSafe(F.getName(), "function", FunctionPrefix);
    }
  }
}

void Converter::convertToIce() {
  TimerMarker T(TimerStack::TT_convertToIce, Ctx);
  nameUnnamedGlobalVariables(Mod);
  nameUnnamedFunctions(Mod);
  installGlobalDeclarations(Mod);
  convertGlobals(Mod);
  convertFunctions();
}

GlobalDeclaration *Converter::getGlobalDeclaration(const GlobalValue *V) {
  GlobalDeclarationMapType::const_iterator Pos = GlobalDeclarationMap.find(V);
  if (Pos == GlobalDeclarationMap.end()) {
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "Can't find global declaration for: " << V->getName();
    report_fatal_error(StrBuf.str());
  }
  return Pos->second;
}

void Converter::installGlobalDeclarations(Module *Mod) {
  const TypeConverter Converter(Mod->getContext());
  // Install function declarations.
  for (const Function &Func : *Mod) {
    FuncSigType Signature;
    FunctionType *FuncType = Func.getFunctionType();
    Signature.setReturnType(
        Converter.convertToIceType(FuncType->getReturnType()));
    for (size_t I = 0; I < FuncType->getNumParams(); ++I) {
      Signature.appendArgType(
          Converter.convertToIceType(FuncType->getParamType(I)));
    }
    FunctionDeclaration *IceFunc = FunctionDeclaration::create(
        Signature, Func.getCallingConv(), Func.getLinkage(), Func.empty());
    IceFunc->setName(Func.getName());
    GlobalDeclarationMap[&Func] = IceFunc;
  }
  // Install global variable declarations.
  for (Module::const_global_iterator I = Mod->global_begin(),
                                     E = Mod->global_end();
       I != E; ++I) {
    const GlobalVariable *GV = I;
    VariableDeclaration *Var = VariableDeclaration::create();
    Var->setName(GV->getName());
    Var->setAlignment(GV->getAlignment());
    Var->setIsConstant(GV->isConstant());
    Var->setLinkage(GV->getLinkage());
    GlobalDeclarationMap[GV] = Var;
  }
}

void Converter::convertGlobals(Module *Mod) {
  LLVM2ICEGlobalsConverter GlobalsConverter(*this);
  VariableDeclarationList VariableDeclarations;
  GlobalsConverter.convertGlobalsToIce(Mod, VariableDeclarations);
  lowerGlobals(VariableDeclarations);
}

void Converter::convertFunctions() {
  TimerStackIdT StackID = GlobalContext::TSK_Funcs;
  for (const Function &I : *Mod) {
    if (I.empty())
      continue;

    TimerIdT TimerID = 0;
    if (ALLOW_DUMP && Ctx->getFlags().TimeEachFunction) {
      TimerID = Ctx->getTimerID(StackID, I.getName());
      Ctx->pushTimer(TimerID, StackID);
    }
    LLVM2ICEFunctionConverter FunctionConverter(*this);

    Cfg *Fcn = FunctionConverter.convertFunction(&I);
    translateFcn(Fcn);
    if (ALLOW_DUMP && Ctx->getFlags().TimeEachFunction)
      Ctx->popTimer(TimerID, StackID);
  }
}

} // end of namespace Ice
