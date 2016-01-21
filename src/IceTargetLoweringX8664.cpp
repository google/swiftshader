//===- subzero/src/IceTargetLoweringX8664.cpp - x86-64 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the TargetLoweringX8664 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//
#include "IceTargetLoweringX8664.h"

#include "IceDefs.h"
#include "IceTargetLoweringX8664Traits.h"

namespace X8664 {
std::unique_ptr<::Ice::TargetLowering> createTargetLowering(::Ice::Cfg *Func) {
  return ::Ice::X8664::TargetX8664::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8664::TargetDataX8664::create(Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8664::TargetHeaderX8664::create(Ctx);
}

void staticInit(::Ice::GlobalContext *Ctx) {
  ::Ice::X8664::TargetX8664::staticInit(Ctx);
}
} // end of namespace X8664

namespace Ice {
namespace X8664 {

//------------------------------------------------------------------------------
//      ______   ______     ______     __     ______   ______
//     /\__  _\ /\  == \   /\  __ \   /\ \   /\__  _\ /\  ___\
//     \/_/\ \/ \ \  __<   \ \  __ \  \ \ \  \/_/\ \/ \ \___  \
//        \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_\    \ \_\  \/\_____\
//         \/_/   \/_/ /_/   \/_/\/_/   \/_/     \/_/   \/_____/
//
//------------------------------------------------------------------------------
const TargetX8664Traits::TableFcmpType TargetX8664Traits::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8664::Traits::Cond::C1, X8664::Traits::Cond::C2, swapV,      \
        X8664::Traits::Cond::pred                                              \
  }                                                                            \
  ,
    FCMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableFcmpSize = llvm::array_lengthof(TableFcmp);

const TargetX8664Traits::TableIcmp32Type TargetX8664Traits::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8664::Traits::Cond::C_32 }                                                \
  ,
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const TargetX8664Traits::TableIcmp64Type TargetX8664Traits::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8664::Traits::Cond::C1_64, X8664::Traits::Cond::C2_64,                    \
        X8664::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const TargetX8664Traits::TableTypeX8664AttributesType
    TargetX8664Traits::TableTypeX8664Attributes[] = {
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
  { IceType_##elementty }                                                      \
  ,
        ICETYPEX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableTypeX8664AttributesSize =
    llvm::array_lengthof(TableTypeX8664Attributes);

const uint32_t TargetX8664Traits::X86_STACK_ALIGNMENT_BYTES = 16;
const char *TargetX8664Traits::TargetName = "X8664";

template <>
std::array<llvm::SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSet = {{}};

template <>
std::array<llvm::SmallBitVector,
           TargetX86Base<X8664::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8664::Traits>::RegisterAliases = {{}};

template <>
FixupKind TargetX86Base<X8664::Traits>::PcRelFixup =
    TargetX86Base<X8664::Traits>::Traits::FK_PcRel;

template <>
FixupKind TargetX86Base<X8664::Traits>::AbsFixup =
    TargetX86Base<X8664::Traits>::Traits::FK_Abs;

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
void TargetX8664::_add_sp(Operand *Adjustment) {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  if (!NeedSandboxing) {
    _add(rsp, Adjustment);
    return;
  }

  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  // When incrementing rsp, NaCl sandboxing requires the following sequence
  //
  // .bundle_start
  // add Adjustment, %esp
  // add %r15, %rsp
  // .bundle_end
  //
  // In Subzero, even though rsp and esp alias each other, defining one does not
  // define the other. Therefore, we must emit
  //
  // .bundle_start
  // %esp = fake-def %rsp
  // add Adjustment, %esp
  // %rsp = fake-def %esp
  // add %r15, %rsp
  // .bundle_end
  //
  // The fake-defs ensure that the
  //
  // add Adjustment, %esp
  //
  // instruction is not DCE'd.
  AutoBundle _(this);
  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _add(esp, Adjustment);
  _redefined(Context.insert<InstFakeDef>(rsp, esp));
  _add(rsp, r15);
}

void TargetX8664::_mov_sp(Operand *NewValue) {
  assert(NewValue->getType() == IceType_i32);

  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);

  AutoBundle _(this);

  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _redefined(_mov(esp, NewValue));
  _redefined(Context.insert<InstFakeDef>(rsp, esp));

  if (!NeedSandboxing) {
    return;
  }

  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  _add(rsp, r15);
}

void TargetX8664::_push_rbp() {
  assert(NeedSandboxing);

  Constant *_0 = Ctx->getConstantZero(IceType_i32);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  auto *TopOfStack = llvm::cast<X86OperandMem>(
      legalize(X86OperandMem::create(Func, IceType_i32, rsp, _0),
               Legal_Reg | Legal_Mem));

  // Emits a sequence:
  //
  //   .bundle_start
  //   push 0
  //   mov %ebp, %(rsp)
  //   .bundle_end
  //
  // to avoid leaking the upper 32-bits (i.e., the sandbox address.)
  AutoBundle _(this);
  _push(_0);
  Context.insert<typename Traits::Insts::Store>(ebp, TopOfStack);
}

void TargetX8664::_link_bp() {
  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, Traits::WordType);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, Traits::WordType);

  if (!NeedSandboxing) {
    _push(rbp);
    _mov(rbp, rsp);
  } else {
    _push_rbp();

    AutoBundle _(this);
    _redefined(Context.insert<InstFakeDef>(ebp, rbp));
    _redefined(Context.insert<InstFakeDef>(esp, rsp));
    _mov(ebp, esp);
    _redefined(Context.insert<InstFakeDef>(rsp, esp));
    _add(rbp, r15);
  }
  // Keep ebp live for late-stage liveness analysis (e.g. asm-verbose mode).
  Context.insert<InstFakeUse>(rbp);
}

void TargetX8664::_unlink_bp() {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, IceType_i64);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
  // use of rsp before the assignment of rsp=rbp keeps previous rsp
  // adjustments from being dead-code eliminated.
  Context.insert<InstFakeUse>(rsp);
  if (!NeedSandboxing) {
    _mov(rsp, rbp);
    _pop(rbp);
  } else {
    _mov_sp(ebp);

    Variable *r15 =
        getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
    Variable *rcx =
        getPhysicalRegister(Traits::RegisterSet::Reg_rcx, IceType_i64);
    Variable *ecx =
        getPhysicalRegister(Traits::RegisterSet::Reg_ecx, IceType_i32);

    _pop(rcx);
    Context.insert<InstFakeDef>(ecx, rcx);
    AutoBundle _(this);
    _mov(ebp, ecx);

    _redefined(Context.insert<InstFakeDef>(rbp, ebp));
    _add(rbp, r15);
  }
}

void TargetX8664::_push_reg(Variable *Reg) {
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, Traits::WordType);
  if (Reg != rbp || !NeedSandboxing) {
    _push(Reg);
  } else {
    _push_rbp();
  }
}

void TargetX8664::emitGetIP(CfgNode *Node) {
  // No IP base register is needed on X86-64.
  (void)Node;
}

Traits::X86OperandMem *TargetX8664::_sandbox_mem_reference(X86OperandMem *Mem) {
  // In x86_64-nacl, all memory references are relative to %r15 (i.e., %rzp.)
  // NaCl sandboxing also requires that any registers that are not %rsp and
  // %rbp to be 'truncated' to 32-bit before memory access.
  assert(NeedSandboxing);
  Variable *Base = Mem->getBase();
  Variable *Index = Mem->getIndex();
  uint16_t Shift = 0;
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  Constant *Offset = Mem->getOffset();
  Variable *T = nullptr;

  if (Mem->getIsRebased()) {
    // If Mem.IsRebased, then we don't need to update Mem to contain a reference
    // to %r15, but we still need to truncate Mem.Index (if any) to 32-bit.
    assert(r15 == Base);
    T = Index;
    Shift = Mem->getShift();
  } else if (Base != nullptr && Index != nullptr) {
    // Another approach could be to emit an
    //
    //   lea Mem, %T
    //
    // And then update Mem.Base = r15, Mem.Index = T, Mem.Shift = 0
    llvm::report_fatal_error("memory reference contains base and index.");
  } else if (Base != nullptr) {
    T = Base;
  } else if (Index != nullptr) {
    T = Index;
    Shift = Mem->getShift();
  }

  // NeedsLea is a flags indicating whether Mem needs to be materialized to a
  // GPR prior to being used. A LEA is needed if Mem.Offset is a constant
  // relocatable, or if Mem.Offset is negative. In both these cases, the LEA is
  // needed to ensure the sandboxed memory operand will only use the lower
  // 32-bits of T+Offset.
  bool NeedsLea = false;
  if (const auto *Offset = Mem->getOffset()) {
    if (llvm::isa<ConstantRelocatable>(Offset)) {
      NeedsLea = true;
    } else if (const auto *Imm = llvm::cast<ConstantInteger32>(Offset)) {
      NeedsLea = Imm->getValue() < 0;
    }
  }

  int32_t RegNum = Variable::NoRegister;
  int32_t RegNum32 = Variable::NoRegister;
  if (T != nullptr) {
    if (T->hasReg()) {
      RegNum = Traits::getGprForType(IceType_i64, T->getRegNum());
      RegNum32 = Traits::getGprForType(IceType_i32, RegNum);
      switch (RegNum) {
      case Traits::RegisterSet::Reg_rsp:
      case Traits::RegisterSet::Reg_rbp:
        // Memory operands referencing rsp/rbp do not need to be sandboxed.
        return Mem;
      }
    }

    switch (T->getType()) {
    default:
    case IceType_i64:
      // Even though "default:" would also catch T.Type == IceType_i64, an
      // explicit 'case IceType_i64' shows that memory operands are always
      // supposed to be 32-bits.
      llvm::report_fatal_error("Mem pointer should be 32-bit.");
    case IceType_i32: {
      Variable *T64 = makeReg(IceType_i64, RegNum);
      auto *Movzx = _movzx(T64, T);
      if (!NeedsLea) {
        // This movzx is only needed when Mem does not need to be lea'd into a
        // temporary. If an lea is going to be emitted, then eliding this movzx
        // is safe because the emitted lea will write a 32-bit result --
        // implicitly zero-extended to 64-bit.
        Movzx->setMustKeep();
      }
      T = T64;
    } break;
    }
  }

  if (NeedsLea) {
    Variable *NewT = makeReg(IceType_i32, RegNum32);
    Variable *Base = T;
    Variable *Index = T;
    static constexpr bool NotRebased = false;
    if (Shift == 0) {
      Index = nullptr;
    } else {
      Base = nullptr;
    }
    _lea(NewT, Traits::X86OperandMem::create(
                   Func, Mem->getType(), Base, Offset, Index, Shift,
                   Traits::X86OperandMem::DefaultSegment, NotRebased));

    T = makeReg(IceType_i64, RegNum);
    _movzx(T, NewT);
    Shift = 0;
    Offset = nullptr;
  }

  static constexpr bool IsRebased = true;
  return Traits::X86OperandMem::create(
      Func, Mem->getType(), r15, Offset, T, Shift,
      Traits::X86OperandMem::DefaultSegment, IsRebased);
}

void TargetX8664::_sub_sp(Operand *Adjustment) {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
  if (!NeedSandboxing) {
    _sub(rsp, Adjustment);
    return;
  }

  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  // .bundle_start
  // sub Adjustment, %esp
  // add %r15, %rsp
  // .bundle_end
  AutoBundle _(this);
  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _sub(esp, Adjustment);
  _redefined(Context.insert<InstFakeDef>(rsp, esp));
  _add(rsp, r15);
}

void TargetX8664::initSandbox() {
  assert(NeedSandboxing);
  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  Context.insert<InstFakeDef>(r15);
  Context.insert<InstFakeUse>(r15);
}

void TargetX8664::lowerIndirectJump(Variable *JumpTarget) {
  std::unique_ptr<AutoBundle> Bundler;

  if (!NeedSandboxing) {
    Variable *T = makeReg(IceType_i64);
    _movzx(T, JumpTarget);
    JumpTarget = T;
  } else {
    Variable *T = makeReg(IceType_i32);
    Variable *T64 = makeReg(IceType_i64);
    Variable *r15 =
        getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

    _mov(T, JumpTarget);
    Bundler = makeUnique<AutoBundle>(this);
    const SizeT BundleSize =
        1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
    _and(T, Ctx->getConstantInt32(~(BundleSize - 1)));
    _movzx(T64, T);
    _add(T64, r15);
    JumpTarget = T64;
  }

  _jmp(JumpTarget);
}

Inst *TargetX8664::emitCallToTarget(Operand *CallTarget, Variable *ReturnReg) {
  Inst *NewCall = nullptr;
  auto *CallTargetR = llvm::dyn_cast<Variable>(CallTarget);
  if (NeedSandboxing) {
    InstX86Label *ReturnAddress = InstX86Label::create(Func, this);
    ReturnAddress->setIsReturnLocation(true);
    constexpr bool SuppressMangling = true;
    /* AutoBundle scoping */ {
      std::unique_ptr<AutoBundle> Bundler;
      if (CallTargetR == nullptr) {
        Bundler = makeUnique<AutoBundle>(this, InstBundleLock::Opt_PadToEnd);
        _push(Ctx->getConstantSym(0, ReturnAddress->getName(Func),
                                  SuppressMangling));
      } else {
        Variable *T = makeReg(IceType_i32);
        Variable *T64 = makeReg(IceType_i64);
        Variable *r15 =
            getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

        _mov(T, CallTargetR);
        Bundler = makeUnique<AutoBundle>(this, InstBundleLock::Opt_PadToEnd);
        _push(Ctx->getConstantSym(0, ReturnAddress->getName(Func),
                                  SuppressMangling));
        const SizeT BundleSize =
            1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
        _and(T, Ctx->getConstantInt32(~(BundleSize - 1)));
        _movzx(T64, T);
        _add(T64, r15);
        CallTarget = T64;
      }

      NewCall = Context.insert<Traits::Insts::Jmp>(CallTarget);
    }
    if (ReturnReg != nullptr) {
      Context.insert<InstFakeDef>(ReturnReg);
    }

    Context.insert(ReturnAddress);
  } else {
    if (CallTargetR != nullptr) {
      // x86-64 in Subzero is ILP32. Therefore, CallTarget is i32, but the
      // emitted call needs a i64 register (for textual asm.)
      Variable *T = makeReg(IceType_i64);
      _movzx(T, CallTargetR);
      CallTarget = T;
    }
    NewCall = Context.insert<Traits::Insts::Call>(ReturnReg, CallTarget);
  }
  return NewCall;
}

Variable *TargetX8664::moveReturnValueToRegister(Operand *Value,
                                                 Type ReturnType) {
  if (isVectorType(ReturnType) || isScalarFloatingType(ReturnType)) {
    return legalizeToReg(Value, Traits::RegisterSet::Reg_xmm0);
  } else {
    assert(ReturnType == IceType_i32 || ReturnType == IceType_i64);
    Variable *Reg = nullptr;
    _mov(Reg, Value,
         Traits::getGprForType(ReturnType, Traits::RegisterSet::Reg_rax));
    return Reg;
  }
}

void TargetX8664::emitSandboxedReturn() {
  Variable *T_rcx = makeReg(IceType_i64, Traits::RegisterSet::Reg_rcx);
  Variable *T_ecx = makeReg(IceType_i32, Traits::RegisterSet::Reg_ecx);
  _pop(T_rcx);
  _mov(T_ecx, T_rcx);
  // lowerIndirectJump(T_ecx);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  /* AutoBundle scoping */ {
    AutoBundle _(this);
    const SizeT BundleSize =
        1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
    _and(T_ecx, Ctx->getConstantInt32(~(BundleSize - 1)));
    Context.insert<InstFakeDef>(T_rcx, T_ecx);
    _add(T_rcx, r15);

    _jmp(T_rcx);
  }
}

void TargetX8664::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  IceString MangledName = Ctx->mangleName(Func->getFunctionName());
  Str << "\t.section\t.rodata." << MangledName
      << "$jumptable,\"a\",@progbits\n";
  Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
  Str << InstJumpTable::makeName(MangledName, JumpTable->getId()) << ":";

  // On X8664 ILP32 pointers are 32-bit hence the use of .long
  for (SizeT I = 0; I < JumpTable->getNumTargets(); ++I)
    Str << "\n\t.long\t" << JumpTable->getTarget(I)->getAsmName();
  Str << "\n";
}

namespace {
template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantFloat;
  static const Type Ty = IceType_f32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<float>::TypeName = "float";
const char *PoolTypeConverter<float>::AsmTag = ".long";
const char *PoolTypeConverter<float>::PrintfString = "0x%x";

template <> struct PoolTypeConverter<double> {
  using PrimitiveIntType = uint64_t;
  using IceType = ConstantDouble;
  static const Type Ty = IceType_f64;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<double>::TypeName = "double";
const char *PoolTypeConverter<double>::AsmTag = ".quad";
const char *PoolTypeConverter<double>::PrintfString = "0x%llx";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint32_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint32_t>::TypeName = "i32";
const char *PoolTypeConverter<uint32_t>::AsmTag = ".long";
const char *PoolTypeConverter<uint32_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint16_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i16;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint16_t>::TypeName = "i16";
const char *PoolTypeConverter<uint16_t>::AsmTag = ".short";
const char *PoolTypeConverter<uint16_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint8_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i8;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint8_t>::TypeName = "i8";
const char *PoolTypeConverter<uint8_t>::AsmTag = ".byte";
const char *PoolTypeConverter<uint8_t>::PrintfString = "0x%x";
} // end of anonymous namespace

template <typename T>
void TargetDataX8664::emitConstantPool(GlobalContext *Ctx) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Type Ty = T::Ty;
  SizeT Align = typeAlignInBytes(Ty);
  ConstantList Pool = Ctx->getConstantPool(Ty);

  Str << "\t.section\t.rodata.cst" << Align << ",\"aM\",@progbits," << Align
      << "\n";
  Str << "\t.align\t" << Align << "\n";

  // If reorder-pooled-constants option is set to true, we need to shuffle the
  // constant pool before emitting it.
  if (Ctx->getFlags().shouldReorderPooledConstants()) {
    // Use the constant's kind value as the salt for creating random number
    // generator.
    Operand::OperandKind K = (*Pool.begin())->getKind();
    RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                              RPE_PooledConstantReordering, K);
    RandomShuffle(Pool.begin(), Pool.end(),
                  [&RNG](uint64_t N) { return (uint32_t)RNG.next(N); });
  }

  for (Constant *C : Pool) {
    if (!C->getShouldBePooled())
      continue;
    auto *Const = llvm::cast<typename T::IceType>(C);
    typename T::IceType::PrimType Value = Const->getValue();
    // Use memcpy() to copy bits from Value into RawValue in a way that avoids
    // breaking strict-aliasing rules.
    typename T::PrimitiveIntType RawValue;
    memcpy(&RawValue, &Value, sizeof(Value));
    char buf[30];
    int CharsPrinted =
        snprintf(buf, llvm::array_lengthof(buf), T::PrintfString, RawValue);
    assert(CharsPrinted >= 0 &&
           (size_t)CharsPrinted < llvm::array_lengthof(buf));
    (void)CharsPrinted; // avoid warnings if asserts are disabled
    Const->emitPoolLabel(Str, Ctx);
    Str << ":\n\t" << T::AsmTag << "\t" << buf << "\t/* " << T::TypeName << " "
        << Value << " */\n";
  }
}

void TargetDataX8664::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  // No need to emit constants from the int pool since (for x86) they are
  // embedded as immediates in the instructions, just emit float/double.
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();

    Writer->writeConstantPool<ConstantInteger32>(IceType_i8);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i16);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i32);

    Writer->writeConstantPool<ConstantFloat>(IceType_f32);
    Writer->writeConstantPool<ConstantDouble>(IceType_f64);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    OstreamLocker L(Ctx);

    emitConstantPool<PoolTypeConverter<uint8_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint16_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint32_t>>(Ctx);

    emitConstantPool<PoolTypeConverter<float>>(Ctx);
    emitConstantPool<PoolTypeConverter<double>>(Ctx);
  } break;
  }
}

void TargetDataX8664::lowerJumpTables() {
  const bool IsPIC = Ctx->getFlags().getUseNonsfi();
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    for (const JumpTableData &JumpTable : Ctx->getJumpTables())
      Writer->writeJumpTable(JumpTable, TargetX8664::Traits::FK_Abs, IsPIC);
  } break;
  case FT_Asm:
    // Already emitted from Cfg
    break;
  case FT_Iasm: {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Ctx->getStrEmit();
    for (const JumpTableData &JT : Ctx->getJumpTables()) {
      Str << "\t.section\t.rodata." << JT.getFunctionName()
          << "$jumptable,\"a\",@progbits\n";
      Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
      Str << InstJumpTable::makeName(JT.getFunctionName(), JT.getId()) << ":";

      // On X8664 ILP32 pointers are 32-bit hence the use of .long
      for (intptr_t TargetOffset : JT.getTargetOffsets())
        Str << "\n\t.long\t" << JT.getFunctionName() << "+" << TargetOffset;
      Str << "\n";
    }
  } break;
  }
}

void TargetDataX8664::lowerGlobals(const VariableDeclarationList &Vars,
                                   const IceString &SectionSuffix) {
  const bool IsPIC = Ctx->getFlags().getUseNonsfi();
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, TargetX8664::Traits::FK_Abs, SectionSuffix,
                             IsPIC);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    const IceString &TranslateOnly = Ctx->getFlags().getTranslateOnly();
    OstreamLocker L(Ctx);
    for (const VariableDeclaration *Var : Vars) {
      if (GlobalContext::matchSymbolName(Var->getName(), TranslateOnly)) {
        emitGlobal(*Var, SectionSuffix);
      }
    }
  } break;
  }
}

// In some cases, there are x-macros tables for both high-level and low-level
// instructions/operands that use the same enum key value. The tables are kept
// separate to maintain a proper separation between abstraction layers. There
// is a risk that the tables could get out of sync if enum values are reordered
// or if entries are added or deleted. The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

namespace {
// Validate the enum values in FCMPX8664_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
ICEINSTFCMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
FCMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8664_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8664_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld) _tmp_##tag,
  ICETYPEX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static const int _table1_##tag = IceType_##tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPEX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace X8664
} // end of namespace Ice
