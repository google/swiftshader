//===- subzero/src/WasmTranslator.cpp - WASM to Subzero Translation -------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines a driver for translating Wasm bitcode into PNaCl bitcode.
///
/// The translator uses V8's WebAssembly decoder to handle the binary Wasm
/// format but replaces the usual TurboFan builder with a new PNaCl builder.
///
//===----------------------------------------------------------------------===//

#if ALLOW_WASM

#include "llvm/Support/StreamingMemoryObject.h"

#include "WasmTranslator.h"

#include "src/wasm/module-decoder.h"
#include "src/wasm/wasm-opcodes.h"
#include "src/zone.h"

#include "IceCfgNode.h"
#include "IceGlobalInits.h"

using namespace std;
using namespace Ice;
using namespace v8;
using namespace v8::internal;
using namespace v8::internal::wasm;
using v8::internal::wasm::DecodeWasmModule;

#include "src/wasm/ast-decoder-impl.h"

#define LOG(Expr) log([&](Ostream & out) { Expr; })

namespace {

Ice::Type toIceType(v8::internal::MachineType) {
  // TODO(eholk): actually convert this.
  return IceType_i32;
}

Ice::Type toIceType(wasm::LocalType Type) {
  switch (Type) {
  default:
    llvm::report_fatal_error("unexpected enum value");
  case MachineRepresentation::kNone:
    llvm::report_fatal_error("kNone type not supported");
  case MachineRepresentation::kBit:
    return IceType_i1;
  case MachineRepresentation::kWord8:
    return IceType_i8;
  case MachineRepresentation::kWord16:
    return IceType_i16;
  case MachineRepresentation::kWord32:
    return IceType_i32;
  case MachineRepresentation::kWord64:
    return IceType_i64;
  case MachineRepresentation::kFloat32:
    return IceType_f32;
  case MachineRepresentation::kFloat64:
    return IceType_f64;
  case MachineRepresentation::kSimd128:
    llvm::report_fatal_error("ambiguous SIMD type");
  case MachineRepresentation::kTagged:
    llvm::report_fatal_error("kTagged type not supported");
  }
}

} // end of anonymous namespace

/// This class wraps either an Operand or a CfgNode.
///
/// Turbofan's sea of nodes representation only has nodes for values, control
/// flow, etc. In Subzero these concepts are all separate. This class lets V8's
/// Wasm decoder treat Subzero objects as though they are all the same.
class OperandNode {
  static constexpr uintptr_t NODE_FLAG = 1;
  static constexpr uintptr_t UNDEF_PTR = (uintptr_t)-1;

  uintptr_t Data = UNDEF_PTR;

public:
  OperandNode() = default;
  explicit OperandNode(Operand *Operand)
      : Data(reinterpret_cast<uintptr_t>(Operand)) {}
  explicit OperandNode(CfgNode *Node)
      : Data(reinterpret_cast<uintptr_t>(Node) | NODE_FLAG) {}
  explicit OperandNode(nullptr_t) : Data(UNDEF_PTR) {}

  operator Operand *() const {
    if (UNDEF_PTR == Data) {
      return nullptr;
    }
    if (!isOperand()) {
      llvm::report_fatal_error("This OperandNode is not an Operand");
    }
    return reinterpret_cast<Operand *>(Data);
  }

  operator CfgNode *() const {
    if (UNDEF_PTR == Data) {
      return nullptr;
    }
    if (!isCfgNode()) {
      llvm::report_fatal_error("This OperandNode is not a CfgNode");
    }
    return reinterpret_cast<CfgNode *>(Data & ~NODE_FLAG);
  }

  explicit operator bool() const { return (Data != UNDEF_PTR) && Data; }
  bool operator==(const OperandNode &Rhs) const {
    return (Data == Rhs.Data) ||
           (UNDEF_PTR == Data && (Rhs.Data == 0 || Rhs.Data == NODE_FLAG)) ||
           (UNDEF_PTR == Rhs.Data && (Data == 0 || Data == NODE_FLAG));
  }
  bool operator!=(const OperandNode &Rhs) const { return !(*this == Rhs); }

  bool isOperand() const { return (Data != UNDEF_PTR) && !(Data & NODE_FLAG); }
  bool isCfgNode() const { return (Data != UNDEF_PTR) && (Data & NODE_FLAG); }

  Operand *toOperand() const { return static_cast<Operand *>(*this); }

  CfgNode *toCfgNode() const { return static_cast<CfgNode *>(*this); }
};

Ostream &operator<<(Ostream &Out, const OperandNode &Op) {
  if (Op.isOperand()) {
    Out << "(Operand*)" << Op.toOperand();
  } else if (Op.isCfgNode()) {
    Out << "(CfgNode*)" << Op.toCfgNode();
  } else {
    Out << "nullptr";
  }
  return Out;
}

constexpr bool isComparison(wasm::WasmOpcode Opcode) {
  switch (Opcode) {
  case kExprI32Ne:
  case kExprI64Ne:
  case kExprI32Eq:
  case kExprI64Eq:
  case kExprI32LtS:
  case kExprI64LtS:
  case kExprI32LtU:
  case kExprI64LtU:
  case kExprI32GeS:
  case kExprI64GeS:
  case kExprI32GtS:
  case kExprI64GtS:
  case kExprI32GtU:
  case kExprI64GtU:
    return true;
  default:
    return false;
  }
}

class IceBuilder {
  using Node = OperandNode;

  IceBuilder() = delete;
  IceBuilder(const IceBuilder &) = delete;
  IceBuilder &operator=(const IceBuilder &) = delete;

public:
  explicit IceBuilder(class Cfg *Func)
      : Func(Func), Ctx(Func->getContext()), ControlPtr(nullptr) {}

  /// Allocates a buffer of Nodes for use by V8.
  Node *Buffer(size_t Count) {
    LOG(out << "Buffer(" << Count << ")\n");
    return Func->allocateArrayOf<Node>(Count);
  }

  Node Error() { llvm::report_fatal_error("Error"); }
  Node Start(unsigned Params) {
    LOG(out << "Start(" << Params << ") = ");
    auto *Entry = Func->makeNode();
    Func->setEntryNode(Entry);
    LOG(out << Node(Entry) << "\n");
    return OperandNode(Entry);
  }
  Node Param(unsigned Index, wasm::LocalType Type) {
    LOG(out << "Param(" << Index << ") = ");
    auto *Arg = makeVariable(toIceType(Type));
    assert(Index == NextArg);
    Func->addArg(Arg);
    ++NextArg;
    LOG(out << Node(Arg) << "\n");
    return OperandNode(Arg);
  }
  Node Loop(CfgNode *Entry) {
    auto *Loop = Func->makeNode();
    LOG(out << "Loop(" << Entry << ") = " << Loop << "\n");
    Entry->appendInst(InstBr::create(Func, Loop));
    return OperandNode(Loop);
  }
  void Terminate(Node Effect, Node Control) {
    // TODO(eholk): this is almost certainly wrong
    LOG(out << "Terminate(" << Effect << ", " << Control << ")"
            << "\n");
  }
  Node Merge(unsigned Count, Node *Controls) {
    LOG(out << "Merge(" << Count);
    for (unsigned i = 0; i < Count; ++i) {
      LOG(out << ", " << Controls[i]);
    }
    LOG(out << ") = ");

    auto *MergedNode = Func->makeNode();

    for (unsigned i = 0; i < Count; ++i) {
      CfgNode *Control = Controls[i];
      Control->appendInst(InstBr::create(Func, MergedNode));
    }
    LOG(out << (OperandNode)MergedNode << "\n");
    return OperandNode(MergedNode);
  }
  Node Phi(wasm::LocalType Type, unsigned Count, Node *Vals, Node Control) {
    LOG(out << "Phi(" << Count << ", " << Control);
    for (int i = 0; i < Count; ++i) {
      LOG(out << ", " << Vals[i]);
    }
    LOG(out << ") = ");

    const auto &InEdges = Control.toCfgNode()->getInEdges();
    assert(Count == InEdges.size());

    assert(Count > 0);

    auto *Dest = makeVariable(Vals[0].toOperand()->getType(), Control);

    // Multiply by 10 in case more things get added later.

    // TODO(eholk): find a better way besides multiplying by some arbitrary
    // constant.
    auto *Phi = InstPhi::create(Func, Count * 10, Dest);
    for (int i = 0; i < Count; ++i) {
      auto *Op = Vals[i].toOperand();
      assert(Op);
      Phi->addArgument(Op, InEdges[i]);
    }
    setDefiningInst(Dest, Phi);
    Control.toCfgNode()->appendInst(Phi);
    LOG(out << Node(Dest) << "\n");
    return OperandNode(Dest);
  }
  Node EffectPhi(unsigned Count, Node *Effects, Node Control) {
    // TODO(eholk): this function is almost certainly wrong.
    LOG(out << "EffectPhi(" << Count << ", " << Control << "):\n");
    for (unsigned i = 0; i < Count; ++i) {
      LOG(out << "  " << Effects[i] << "\n");
    }
    return OperandNode(nullptr);
  }
  Node Int32Constant(int32_t Value) {
    LOG(out << "Int32Constant(" << Value << ") = ");
    auto *Const = Ctx->getConstantInt32(Value);
    assert(Const);
    assert(Control());
    LOG(out << Node(Const) << "\n");
    return OperandNode(Const);
  }
  Node Int64Constant(int64_t Value) {
    LOG(out << "Int64Constant(" << Value << ") = ");
    auto *Const = Ctx->getConstantInt64(Value);
    assert(Const);
    LOG(out << Node(Const) << "\n");
    return OperandNode(Const);
  }
  Node Float32Constant(float Value) {
    LOG(out << "Float32Constant(" << Value << ") = ");
    auto *Const = Ctx->getConstantFloat(Value);
    assert(Const);
    LOG(out << Node(Const) << "\n");
    return OperandNode(Const);
  }
  Node Float64Constant(double Value) {
    LOG(out << "Float64Constant(" << Value << ") = ");
    auto *Const = Ctx->getConstantDouble(Value);
    assert(Const);
    LOG(out << Node(Const) << "\n");
    return OperandNode(Const);
  }
  Node Binop(wasm::WasmOpcode Opcode, Node Left, Node Right) {
    LOG(out << "Binop(" << WasmOpcodes::OpcodeName(Opcode) << ", " << Left
            << ", " << Right << ") = ");
    auto *Dest = makeVariable(
        isComparison(Opcode) ? IceType_i1 : Left.toOperand()->getType());
    switch (Opcode) {
    case kExprI32Add:
    case kExprI64Add:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Add, Dest, Left, Right));
      break;
    case kExprI32Sub:
    case kExprI64Sub:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Sub, Dest, Left, Right));
      break;
    case kExprI32Mul:
    case kExprI64Mul:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Mul, Dest, Left, Right));
      break;
    case kExprI32DivU:
    case kExprI64DivU:
      Control()->appendInst(InstArithmetic::create(Func, InstArithmetic::Udiv,
                                                   Dest, Left, Right));
      break;
    case kExprI32RemU:
    case kExprI64RemU:
      Control()->appendInst(InstArithmetic::create(Func, InstArithmetic::Urem,
                                                   Dest, Left, Right));
      break;
    case kExprI32Ior:
    case kExprI64Ior:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Or, Dest, Left, Right));
      break;
    case kExprI32Xor:
    case kExprI64Xor:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Xor, Dest, Left, Right));
      break;
    case kExprI32Shl:
    case kExprI64Shl:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::Shl, Dest, Left, Right));
      break;
    case kExprI32ShrU:
    case kExprI64ShrU:
    case kExprI32ShrS:
    case kExprI64ShrS:
      Control()->appendInst(InstArithmetic::create(Func, InstArithmetic::Ashr,
                                                   Dest, Left, Right));
      break;
    case kExprI32And:
    case kExprI64And:
      Control()->appendInst(
          InstArithmetic::create(Func, InstArithmetic::And, Dest, Left, Right));
      break;
    case kExprI32Ne:
    case kExprI64Ne:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Ne, Dest, Left, Right));
      break;
    case kExprI32Eq:
    case kExprI64Eq:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Eq, Dest, Left, Right));
      break;
    case kExprI32LtS:
    case kExprI64LtS:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Slt, Dest, Left, Right));
      break;
    case kExprI32LtU:
    case kExprI64LtU:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Ult, Dest, Left, Right));
      break;
    case kExprI32GeS:
    case kExprI64GeS:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Sge, Dest, Left, Right));
    case kExprI32GtS:
    case kExprI64GtS:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Sgt, Dest, Left, Right));
      break;
    case kExprI32GtU:
    case kExprI64GtU:
      Control()->appendInst(
          InstIcmp::create(Func, InstIcmp::Ugt, Dest, Left, Right));
      break;
    default:
      LOG(out << "Unknown binop: " << WasmOpcodes::OpcodeName(Opcode) << "\n");
      llvm::report_fatal_error("Uncovered or invalid binop.");
      return OperandNode(nullptr);
    }
    LOG(out << Dest << "\n");
    return OperandNode(Dest);
  }
  Node Unop(wasm::WasmOpcode Opcode, Node Input) {
    LOG(out << "Unop(" << WasmOpcodes::OpcodeName(Opcode) << ", " << Input
            << ") = ");
    Ice::Variable *Dest = nullptr;
    switch (Opcode) {
    case kExprF32Neg: {
      Dest = makeVariable(IceType_f32);
      Control()->appendInst(InstArithmetic::create(
          Func, InstArithmetic::Fsub, Dest, Ctx->getConstantFloat(0), Input));
      break;
    }
    case kExprF64Neg: {
      Dest = makeVariable(IceType_f64);
      Control()->appendInst(InstArithmetic::create(
          Func, InstArithmetic::Fsub, Dest, Ctx->getConstantDouble(0), Input));
      break;
    }
    case kExprI64UConvertI32:
      Dest = makeVariable(IceType_i64);
      Control()->appendInst(
          InstCast::create(Func, InstCast::Zext, Dest, Input));
      break;
    default:
      LOG(out << "Unknown unop: " << WasmOpcodes::OpcodeName(Opcode) << "\n");
      llvm::report_fatal_error("Uncovered or invalid unop.");
      return OperandNode(nullptr);
    }
    LOG(out << Dest << "\n");
    return OperandNode(Dest);
  }
  unsigned InputCount(CfgNode *Node) const { return Node->getInEdges().size(); }
  bool IsPhiWithMerge(Node Phi, Node Merge) const {
    LOG(out << "IsPhiWithMerge(" << Phi << ", " << Merge << ")"
            << "\n");
    if (Phi && Phi.isOperand()) {
      LOG(out << "  ...is operand"
              << "\n");
      if (auto *Inst = getDefiningInst(Phi)) {
        LOG(out << "  ...has defining instruction"
                << "\n");
        LOG(out << getDefNode(Phi) << "\n");
        LOG(out << "  ..." << (getDefNode(Phi) == Merge) << "\n");
        return getDefNode(Phi) == Merge;
      }
    }
    return false;
  }
  void AppendToMerge(CfgNode *Merge, CfgNode *From) const {
    From->appendInst(InstBr::create(Func, Merge));
  }
  void AppendToPhi(Node Merge, Node Phi, Node From) {
    LOG(out << "AppendToPhi(" << Merge << ", " << Phi << ", " << From << ")"
            << "\n");
    auto *Inst = getDefiningInst(Phi);
    Inst->addArgument(From, getDefNode(From));
  }

  //-----------------------------------------------------------------------
  // Operations that read and/or write {control} and {effect}.
  //-----------------------------------------------------------------------
  Node Branch(Node Cond, Node *TrueNode, Node *FalseNode) {
    // true_node and false_node appear to be out parameters.
    LOG(out << "Branch(" << Cond << ", ");

    // save control here because true_node appears to alias control.
    auto *Ctrl = Control();

    *TrueNode = OperandNode(Func->makeNode());
    *FalseNode = OperandNode(Func->makeNode());

    LOG(out << *TrueNode << ", " << *FalseNode << ")"
            << "\n");

    Ctrl->appendInst(InstBr::create(Func, Cond, *TrueNode, *FalseNode));
    return OperandNode(nullptr);
  }
  Node Switch(unsigned Count, Node Key) { llvm::report_fatal_error("Switch"); }
  Node IfValue(int32_t Value, Node Sw) { llvm::report_fatal_error("IfValue"); }
  Node IfDefault(Node Sw) { llvm::report_fatal_error("IfDefault"); }
  Node Return(unsigned Count, Node *Vals) {
    assert(1 >= Count);
    LOG(out << "Return(");
    if (Count > 0)
      LOG(out << Vals[0]);
    LOG(out << ")"
            << "\n");
    auto *Instr =
        1 == Count ? InstRet::create(Func, Vals[0]) : InstRet::create(Func);
    Control()->appendInst(Instr);
    Control()->setHasReturn();
    LOG(out << Node(nullptr) << "\n");
    return OperandNode(nullptr);
  }
  Node ReturnVoid() {
    LOG(out << "ReturnVoid() = ");
    auto *Instr = InstRet::create(Func);
    Control()->appendInst(Instr);
    Control()->setHasReturn();
    LOG(out << Node(nullptr) << "\n");
    return OperandNode(nullptr);
  }
  Node Unreachable() {
    LOG(out << "Unreachable() = ");
    auto *Instr = InstUnreachable::create(Func);
    Control()->appendInst(Instr);
    LOG(out << Node(nullptr) << "\n");
    return OperandNode(nullptr);
  }

  Node CallDirect(uint32_t Index, Node *Args) {
    LOG(out << "CallDirect(" << Index << ")"
            << "\n");
    assert(Module->IsValidFunction(Index));
    const auto *Module = this->Module->module;
    assert(Module);
    const auto &Target = Module->functions[Index];
    const auto *Sig = Target.sig;
    assert(Sig);
    const auto NumArgs = Sig->parameter_count();
    LOG(out << "  number of args: " << NumArgs << "\n");

    const auto TargetName =
        Ctx->getGlobalString(Module->GetName(Target.name_offset));
    LOG(out << "  target name: " << TargetName << "\n");

    assert(Sig->return_count() <= 1);

    auto *TargetOperand = Ctx->getConstantSym(0, TargetName);

    auto *Dest = Sig->return_count() > 0
                     ? makeVariable(toIceType(Sig->GetReturn()))
                     : nullptr;
    auto *Call = InstCall::create(Func, NumArgs, Dest, TargetOperand,
                                  false /* HasTailCall */);
    for (int i = 0; i < NumArgs; ++i) {
      // The builder reserves the first argument for the code object.
      LOG(out << "  args[" << i << "] = " << Args[i + 1] << "\n");
      Call->addArg(Args[i + 1]);
    }

    Control()->appendInst(Call);
    LOG(out << "Call Result = " << Node(Dest) << "\n");
    return OperandNode(Dest);
  }
  Node CallImport(uint32_t Index, Node *Args) {
    LOG(out << "CallImport(" << Index << ")"
            << "\n");
    const auto *Module = this->Module->module;
    assert(Module);
    const auto *Sig = this->Module->GetImportSignature(Index);
    assert(Sig);
    const auto NumArgs = Sig->parameter_count();
    LOG(out << "  number of args: " << NumArgs << "\n");

    const auto &Target = Module->import_table[Index];
    const auto TargetName =
        Ctx->getGlobalString(Module->GetName(Target.function_name_offset));
    LOG(out << "  target name: " << TargetName << "\n");

    assert(Sig->return_count() <= 1);

    auto *TargetOperand = Ctx->getConstantSym(0, TargetName);

    auto *Dest = Sig->return_count() > 0
                     ? makeVariable(toIceType(Sig->GetReturn()))
                     : nullptr;
    constexpr bool NoTailCall = false;
    auto *Call =
        InstCall::create(Func, NumArgs, Dest, TargetOperand, NoTailCall);
    for (int i = 0; i < NumArgs; ++i) {
      // The builder reserves the first argument for the code object.
      LOG(out << "  args[" << i << "] = " << Args[i + 1] << "\n");
      Call->addArg(Args[i + 1]);
    }

    Control()->appendInst(Call);
    LOG(out << "Call Result = " << Node(Dest) << "\n");
    return OperandNode(Dest);
  }
  Node CallIndirect(uint32_t Index, Node *Args) {
    llvm::report_fatal_error("CallIndirect");
  }
  Node Invert(Node Node) { llvm::report_fatal_error("Invert"); }
  Node FunctionTable() { llvm::report_fatal_error("FunctionTable"); }

  //-----------------------------------------------------------------------
  // Operations that concern the linear memory.
  //-----------------------------------------------------------------------
  Node MemSize(uint32_t Offset) { llvm::report_fatal_error("MemSize"); }
  Node LoadGlobal(uint32_t Index) { llvm::report_fatal_error("LoadGlobal"); }
  Node StoreGlobal(uint32_t Index, Node Val) {
    llvm::report_fatal_error("StoreGlobal");
  }
  Node LoadMem(wasm::LocalType Type, MachineType MemType, Node Index,
               uint32_t Offset) {
    LOG(out << "LoadMem(" << Index << "[" << Offset << "]) = ");

    // first, add the index and the offset together.
    auto *OffsetConstant = Ctx->getConstantInt32(Offset);
    auto *Addr = makeVariable(IceType_i32);
    Control()->appendInst(InstArithmetic::create(Func, InstArithmetic::Add,
                                                 Addr, Index, OffsetConstant));

    // then load the memory
    auto *LoadResult = makeVariable(toIceType(MemType));
    Control()->appendInst(InstLoad::create(Func, LoadResult, Addr));

    // and cast, if needed
    Ice::Variable *Result = nullptr;
    if (toIceType(Type) != toIceType(MemType)) {
      Result = makeVariable(toIceType(Type));
      // TODO(eholk): handle signs correctly.
      Control()->appendInst(
          InstCast::create(Func, InstCast::Sext, Result, LoadResult));
    } else {
      Result = LoadResult;
    }

    LOG(out << Result << "\n");
    return OperandNode(Result);
  }
  void StoreMem(MachineType Type, Node Index, uint32_t Offset, Node Val) {
    LOG(out << "StoreMem(" << Index << "[" << Offset << "] = " << Val << ")"
            << "\n");

    // TODO(eholk): surely there is a better way to do this.

    // first, add the index and the offset together.
    auto *OffsetConstant = Ctx->getConstantInt32(Offset);
    auto *Addr = makeVariable(IceType_i32);
    Control()->appendInst(InstArithmetic::create(Func, InstArithmetic::Add,
                                                 Addr, Index, OffsetConstant));

    // cast the value to the right type, if needed
    Operand *StoreVal = nullptr;
    if (toIceType(Type) != Val.toOperand()->getType()) {
      auto *LocalStoreVal = makeVariable(toIceType(Type));
      Control()->appendInst(
          InstCast::create(Func, InstCast::Trunc, LocalStoreVal, Val));
      StoreVal = LocalStoreVal;
    } else {
      StoreVal = Val;
    }

    // then store the memory
    Control()->appendInst(InstStore::create(Func, StoreVal, Addr));
  }

  static void PrintDebugName(Node node) {
    llvm::report_fatal_error("PrintDebugName");
  }

  CfgNode *Control() {
    return ControlPtr ? ControlPtr->toCfgNode() : Func->getEntryNode();
  }
  Node Effect() { return *EffectPtr; }

  void set_module(wasm::ModuleEnv *Module) { this->Module = Module; }

  void set_control_ptr(Node *Control) { this->ControlPtr = Control; }

  void set_effect_ptr(Node *Effect) { this->EffectPtr = Effect; }

private:
  wasm::ModuleEnv *Module;
  Node *ControlPtr;
  Node *EffectPtr;

  class Cfg *Func;
  GlobalContext *Ctx;

  SizeT NextArg = 0;

  CfgUnorderedMap<Operand *, InstPhi *> PhiMap;
  CfgUnorderedMap<Operand *, CfgNode *> DefNodeMap;

  InstPhi *getDefiningInst(Operand *Op) const {
    const auto &Iter = PhiMap.find(Op);
    if (Iter == PhiMap.end()) {
      return nullptr;
    }
    return Iter->second;
  }

  void setDefiningInst(Operand *Op, InstPhi *Phi) {
    LOG(out << "\n== setDefiningInst(" << Op << ", " << Phi << ") ==\n");
    PhiMap.emplace(Op, Phi);
  }

  Ice::Variable *makeVariable(Ice::Type Type) {
    return makeVariable(Type, Control());
  }

  Ice::Variable *makeVariable(Ice::Type Type, CfgNode *DefNode) {
    auto *Var = Func->makeVariable(Type);
    DefNodeMap.emplace(Var, DefNode);
    return Var;
  }

  CfgNode *getDefNode(Operand *Op) const {
    const auto &Iter = DefNodeMap.find(Op);
    if (Iter == DefNodeMap.end()) {
      return nullptr;
    }
    return Iter->second;
  }

  template <typename F = std::function<void(Ostream &)>> void log(F Fn) const {
    if (BuildDefs::dump() && (getFlags().getVerbose() & IceV_Wasm)) {
      Fn(Ctx->getStrDump());
      Ctx->getStrDump().flush();
    }
  }
};

std::string fnNameFromId(uint32_t Id) {
  return std::string("fn") + to_string(Id);
}

std::unique_ptr<Cfg> WasmTranslator::translateFunction(Zone *Zone,
                                                       FunctionEnv *Env,
                                                       const byte *Base,
                                                       const byte *Start,
                                                       const byte *End) {
  OstreamLocker L1(Ctx);
  auto Func = Cfg::create(Ctx, getNextSequenceNumber());
  Ice::CfgLocalAllocatorScope L2(Func.get());

  // TODO: parse the function signature...

  IceBuilder Builder(Func.get());
  LR_WasmDecoder<OperandNode, IceBuilder> Decoder(Zone, &Builder);

  LOG(out << getFlags().getDefaultGlobalPrefix() << "\n");
  Decoder.Decode(Env, Base, Start, End);

  // We don't always know where the incoming branches are in phi nodes, so this
  // function finds them.
  Func->fixPhiNodes();

  return Func;
}

WasmTranslator::WasmTranslator(GlobalContext *Ctx)
    : Translator(Ctx), BufferSize(24 << 10), Buffer(new uint8_t[24 << 10]) {
  // TODO(eholk): compute the correct buffer size. This uses 24k by default,
  // which has been big enough for testing but is not a general solution.
}

void WasmTranslator::translate(
    const std::string &IRFilename,
    std::unique_ptr<llvm::DataStreamer> InputStream) {
  LOG(out << "Initializing v8/wasm stuff..."
          << "\n");
  Zone Zone;
  ZoneScope _(&Zone);

  SizeT BytesRead = InputStream->GetBytes(Buffer.get(), BufferSize);
  LOG(out << "Read " << BytesRead << " bytes"
          << "\n");

  LOG(out << "Decoding module " << IRFilename << "\n");

  constexpr v8::internal::Isolate *NoIsolate = nullptr;
  auto Result = DecodeWasmModule(NoIsolate, &Zone, Buffer.get(),
                                 Buffer.get() + BytesRead, false, kWasmOrigin);

  auto Module = Result.val;

  LOG(out << "Module info:"
          << "\n");
  LOG(out << "  number of globals:       " << Module->globals.size() << "\n");
  LOG(out << "  number of signatures:    " << Module->signatures.size()
          << "\n");
  LOG(out << "  number of functions:     " << Module->functions.size() << "\n");
  LOG(out << "  number of data_segments: " << Module->data_segments.size()
          << "\n");
  LOG(out << "  function table size:     " << Module->function_table.size()
          << "\n");

  ModuleEnv ModuleEnv;
  ModuleEnv.module = Module;

  LOG(out << "\n"
          << "Function information:"
          << "\n");
  for (const auto F : Module->functions) {
    LOG(out << "  " << F.name_offset << ": " << Module->GetName(F.name_offset));
    if (F.exported)
      LOG(out << " export");
    if (F.external)
      LOG(out << " extern");
    LOG(out << "\n");
  }

  FunctionEnv Fenv;
  Fenv.module = &ModuleEnv;

  LOG(out << "Translating " << IRFilename << "\n");

  // Translate each function.
  uint32_t Id = 0;
  for (const auto Fn : Module->functions) {
    std::string NewName = fnNameFromId(Id++);
    LOG(out << "  " << Fn.name_offset << ": " << Module->GetName(Fn.name_offset)
            << " -> " << NewName << "...");

    Fenv.sig = Fn.sig;
    Fenv.local_i32_count = Fn.local_i32_count;
    Fenv.local_i64_count = Fn.local_i64_count;
    Fenv.local_f32_count = Fn.local_f32_count;
    Fenv.local_f64_count = Fn.local_f64_count;
    Fenv.SumLocals();

    auto Func = translateFunction(&Zone, &Fenv, Buffer.get(),
                                  Buffer.get() + Fn.code_start_offset,
                                  Buffer.get() + Fn.code_end_offset);
    Func->setFunctionName(Ctx->getGlobalString(NewName));

    Ctx->optQueueBlockingPush(makeUnique<CfgOptWorkItem>(std::move(Func)));
    LOG(out << "done.\n");
  }

  return;
}

#endif // ALLOW_WASM
