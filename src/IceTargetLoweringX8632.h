//===- subzero/src/IceTargetLoweringX8632.h - x86-32 lowering ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLoweringX8632 class, which
// implements the TargetLowering interface for the x86-32
// architecture.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8632_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8632_H

#include "IceDefs.h"
#include "IceTargetLowering.h"
#include "IceInstX8632.h"

namespace Ice {

class TargetX8632 : public TargetLowering {
public:
  static TargetX8632 *create(Cfg *Func) { return new TargetX8632(Func); }

  virtual void translateOm1();

  virtual Variable *getPhysicalRegister(SizeT RegNum);
  virtual IceString getRegName(SizeT RegNum, Type Ty) const;
  virtual llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                              RegSetMask Exclude) const;
  virtual const llvm::SmallBitVector &getRegisterSetForType(Type Ty) const {
    return TypeToRegisterSet[Ty];
  }
  virtual bool hasFramePointer() const { return IsEbpBasedFrame; }
  virtual SizeT getFrameOrStackReg() const {
    return IsEbpBasedFrame ? Reg_ebp : Reg_esp;
  }
  virtual size_t typeWidthInBytesOnStack(Type Ty) {
    // Round up to the next multiple of 4 bytes.  In particular, i1,
    // i8, and i16 are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }
  virtual void emitVariable(const Variable *Var, const Cfg *Func) const;
  virtual void addProlog(CfgNode *Node);
  virtual void addEpilog(CfgNode *Node);
  SizeT makeNextLabelNumber() { return NextLabelNumber++; }
  // Ensure that a 64-bit Variable has been split into 2 32-bit
  // Variables, creating them if necessary.  This is needed for all
  // I64 operations, and it is needed for pushing F64 arguments for
  // function calls using the 32-bit push instruction (though the
  // latter could be done by directly writing to the stack).
  void split64(Variable *Var);
  void setArgOffsetAndCopy(Variable *Arg, Variable *FramePtr,
                           int32_t BasicFrameOffset, int32_t &InArgsSizeBytes);
  Operand *loOperand(Operand *Operand);
  Operand *hiOperand(Operand *Operand);

  enum Registers {
#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
          frameptr, isI8, isInt, isFP)                                         \
  val init,
    REGX8632_TABLE
#undef X
        Reg_NUM
  };

protected:
  TargetX8632(Cfg *Func);

  virtual void postLower();

  virtual void lowerAlloca(const InstAlloca *Inst);
  virtual void lowerArithmetic(const InstArithmetic *Inst);
  virtual void lowerAssign(const InstAssign *Inst);
  virtual void lowerBr(const InstBr *Inst);
  virtual void lowerCall(const InstCall *Inst);
  virtual void lowerCast(const InstCast *Inst);
  virtual void lowerFcmp(const InstFcmp *Inst);
  virtual void lowerIcmp(const InstIcmp *Inst);
  virtual void lowerLoad(const InstLoad *Inst);
  virtual void lowerPhi(const InstPhi *Inst);
  virtual void lowerRet(const InstRet *Inst);
  virtual void lowerSelect(const InstSelect *Inst);
  virtual void lowerStore(const InstStore *Inst);
  virtual void lowerSwitch(const InstSwitch *Inst);
  virtual void lowerUnreachable(const InstUnreachable *Inst);

  // Operand legalization helpers.  To deal with address mode
  // constraints, the helpers will create a new Operand and emit
  // instructions that guarantee that the Operand kind is one of those
  // indicated by the LegalMask (a bitmask of allowed kinds).  If the
  // input Operand is known to already meet the constraints, it may be
  // simply returned as the result, without creating any new
  // instructions or operands.
  enum OperandLegalization {
    Legal_None = 0,
    Legal_Reg = 1 << 0, // physical register, not stack location
    Legal_Imm = 1 << 1,
    Legal_Mem = 1 << 2, // includes [eax+4*ecx] as well as [esp+12]
    Legal_All = ~Legal_None
  };
  typedef uint32_t LegalMask;
  Operand *legalize(Operand *From, LegalMask Allowed = Legal_All,
                    bool AllowOverlap = false,
                    int32_t RegNum = Variable::NoRegister);
  Variable *legalizeToVar(Operand *From, bool AllowOverlap = false,
                          int32_t RegNum = Variable::NoRegister);

  Variable *makeReg(Type Ty, int32_t RegNum = Variable::NoRegister);
  InstCall *makeHelperCall(const IceString &Name, Variable *Dest,
                           SizeT MaxSrcs) {
    bool SuppressMangling = true;
    Type Ty = Dest ? Dest->getType() : IceType_void;
    Constant *CallTarget = Ctx->getConstantSym(Ty, 0, Name, SuppressMangling);
    InstCall *Call = InstCall::create(Func, MaxSrcs, Dest, CallTarget);
    return Call;
  }

  // The following are helpers that insert lowered x86 instructions
  // with minimal syntactic overhead, so that the lowering code can
  // look as close to assembly as practical.
  void _adc(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Adc::create(Func, Dest, Src0));
  }
  void _add(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Add::create(Func, Dest, Src0));
  }
  void _addss(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Addss::create(Func, Dest, Src0));
  }
  void _and(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632And::create(Func, Dest, Src0));
  }
  void _br(InstX8632Br::BrCond Condition, CfgNode *TargetTrue,
           CfgNode *TargetFalse) {
    Context.insert(
        InstX8632Br::create(Func, TargetTrue, TargetFalse, Condition));
  }
  void _br(CfgNode *Target) {
    Context.insert(InstX8632Br::create(Func, Target));
  }
  void _br(InstX8632Br::BrCond Condition, CfgNode *Target) {
    Context.insert(InstX8632Br::create(Func, Target, Condition));
  }
  void _br(InstX8632Br::BrCond Condition, InstX8632Label *Label) {
    Context.insert(InstX8632Br::create(Func, Label, Condition));
  }
  void _cdq(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Cdq::create(Func, Dest, Src0));
  }
  void _cmp(Operand *Src0, Operand *Src1) {
    Context.insert(InstX8632Icmp::create(Func, Src0, Src1));
  }
  void _cvt(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Cvt::create(Func, Dest, Src0));
  }
  void _div(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(InstX8632Div::create(Func, Dest, Src0, Src1));
  }
  void _divss(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Divss::create(Func, Dest, Src0));
  }
  void _fld(Operand *Src0) { Context.insert(InstX8632Fld::create(Func, Src0)); }
  void _fstp(Variable *Dest) {
    Context.insert(InstX8632Fstp::create(Func, Dest));
  }
  void _idiv(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(InstX8632Idiv::create(Func, Dest, Src0, Src1));
  }
  void _imul(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Imul::create(Func, Dest, Src0));
  }
  // If Dest=NULL is passed in, then a new variable is created, marked
  // as infinite register allocation weight, and returned through the
  // in/out Dest argument.
  void _mov(Variable *&Dest, Operand *Src0,
            int32_t RegNum = Variable::NoRegister) {
    if (Dest == NULL) {
      Dest = legalizeToVar(Src0, false, RegNum);
    } else {
      Context.insert(InstX8632Mov::create(Func, Dest, Src0));
    }
  }
  void _movsx(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Movsx::create(Func, Dest, Src0));
  }
  void _movzx(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Movzx::create(Func, Dest, Src0));
  }
  void _mul(Variable *Dest, Variable *Src0, Operand *Src1) {
    Context.insert(InstX8632Mul::create(Func, Dest, Src0, Src1));
  }
  void _mulss(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Mulss::create(Func, Dest, Src0));
  }
  void _or(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Or::create(Func, Dest, Src0));
  }
  void _pop(Variable *Dest) {
    Context.insert(InstX8632Pop::create(Func, Dest));
  }
  void _push(Operand *Src0, bool SuppressStackAdjustment = false) {
    Context.insert(InstX8632Push::create(Func, Src0, SuppressStackAdjustment));
  }
  void _ret(Variable *Src0 = NULL) {
    Context.insert(InstX8632Ret::create(Func, Src0));
  }
  void _sar(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Sar::create(Func, Dest, Src0));
  }
  void _sbb(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Sbb::create(Func, Dest, Src0));
  }
  void _shl(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Shl::create(Func, Dest, Src0));
  }
  void _shld(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstX8632Shld::create(Func, Dest, Src0, Src1));
  }
  void _shr(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Shr::create(Func, Dest, Src0));
  }
  void _shrd(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstX8632Shrd::create(Func, Dest, Src0, Src1));
  }
  void _store(Operand *Value, OperandX8632 *Mem) {
    Context.insert(InstX8632Store::create(Func, Value, Mem));
  }
  void _sub(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Sub::create(Func, Dest, Src0));
  }
  void _subss(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Subss::create(Func, Dest, Src0));
  }
  void _test(Operand *Src0, Operand *Src1) {
    Context.insert(InstX8632Test::create(Func, Src0, Src1));
  }
  void _ucomiss(Operand *Src0, Operand *Src1) {
    Context.insert(InstX8632Ucomiss::create(Func, Src0, Src1));
  }
  void _xor(Variable *Dest, Operand *Src0) {
    Context.insert(InstX8632Xor::create(Func, Dest, Src0));
  }

  bool IsEbpBasedFrame;
  int32_t FrameSizeLocals;
  int32_t LocalsSizeBytes;
  llvm::SmallBitVector TypeToRegisterSet[IceType_NUM];
  llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  SizeT NextLabelNumber;
  bool ComputedLiveRanges;
  VarList PhysicalRegisters;
  static IceString RegNames[];

private:
  TargetX8632(const TargetX8632 &) LLVM_DELETED_FUNCTION;
  TargetX8632 &operator=(const TargetX8632 &) LLVM_DELETED_FUNCTION;
  virtual ~TargetX8632() {}
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8632_H
