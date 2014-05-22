//===- subzero/src/IceOperand.h - High-level operands -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Operand class and its target-independent
// subclasses.  The main classes are Variable, which represents an
// LLVM variable that is either register- or stack-allocated, and the
// Constant hierarchy, which represents integer, floating-point,
// and/or symbolic constants.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEOPERAND_H
#define SUBZERO_SRC_ICEOPERAND_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class Operand {
public:
  enum OperandKind {
    kConst_Base,
    kConstInteger,
    kConstFloat,
    kConstDouble,
    kConstRelocatable,
    kConst_Num,
    kVariable,
    // Target-specific operand classes use kTarget as the starting
    // point for their Kind enum space.
    kTarget
  };
  OperandKind getKind() const { return Kind; }
  Type getType() const { return Ty; }

  // Every Operand keeps an array of the Variables referenced in
  // the operand.  This is so that the liveness operations can get
  // quick access to the variables of interest, without having to dig
  // so far into the operand.
  SizeT getNumVars() const { return NumVars; }
  Variable *getVar(SizeT I) const {
    assert(I < getNumVars());
    return Vars[I];
  }
  virtual void emit(const Cfg *Func) const = 0;
  virtual void dump(const Cfg *Func) const = 0;

  // Query whether this object was allocated in isolation, or added to
  // some higher-level pool.  This determines whether a containing
  // object's destructor should delete this object.  Generally,
  // constants are pooled globally, variables are pooled per-CFG, and
  // target-specific operands are not pooled.
  virtual bool isPooled() const { return false; }

  virtual ~Operand() {}

protected:
  Operand(OperandKind Kind, Type Ty)
      : Ty(Ty), Kind(Kind), NumVars(0), Vars(NULL) {}

  const Type Ty;
  const OperandKind Kind;
  // Vars and NumVars are initialized by the derived class.
  SizeT NumVars;
  Variable **Vars;

private:
  Operand(const Operand &) LLVM_DELETED_FUNCTION;
  Operand &operator=(const Operand &) LLVM_DELETED_FUNCTION;
};

// Constant is the abstract base class for constants.  All
// constants are allocated from a global arena and are pooled.
class Constant : public Operand {
public:
  virtual void emit(const Cfg *Func) const = 0;
  virtual void dump(const Cfg *Func) const = 0;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind >= kConst_Base && Kind <= kConst_Num;
  }

protected:
  Constant(OperandKind Kind, Type Ty) : Operand(Kind, Ty) {
    Vars = NULL;
    NumVars = 0;
  }
  virtual ~Constant() {}

private:
  Constant(const Constant &) LLVM_DELETED_FUNCTION;
  Constant &operator=(const Constant &) LLVM_DELETED_FUNCTION;
};

// ConstantPrimitive<> wraps a primitive type.
template <typename T, Operand::OperandKind K>
class ConstantPrimitive : public Constant {
public:
  static ConstantPrimitive *create(GlobalContext *Ctx, Type Ty, T Value) {
    return new (Ctx->allocate<ConstantPrimitive>())
        ConstantPrimitive(Ty, Value);
  }
  T getValue() const { return Value; }
  virtual void emit(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrEmit();
    Str << getValue();
  }
  virtual void dump(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrDump();
    Str << getValue();
  }

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == K;
  }

private:
  ConstantPrimitive(Type Ty, T Value) : Constant(K, Ty), Value(Value) {}
  ConstantPrimitive(const ConstantPrimitive &) LLVM_DELETED_FUNCTION;
  ConstantPrimitive &operator=(const ConstantPrimitive &) LLVM_DELETED_FUNCTION;
  virtual ~ConstantPrimitive() {}
  const T Value;
};

typedef ConstantPrimitive<uint64_t, Operand::kConstInteger> ConstantInteger;
typedef ConstantPrimitive<float, Operand::kConstFloat> ConstantFloat;
typedef ConstantPrimitive<double, Operand::kConstDouble> ConstantDouble;

// RelocatableTuple bundles the parameters that are used to
// construct an ConstantRelocatable.  It is done this way so that
// ConstantRelocatable can fit into the global constant pool
// template mechanism.
class RelocatableTuple {
  RelocatableTuple &operator=(const RelocatableTuple &) LLVM_DELETED_FUNCTION;

public:
  RelocatableTuple(const int64_t Offset, const IceString &Name,
                   bool SuppressMangling)
      : Offset(Offset), Name(Name), SuppressMangling(SuppressMangling) {}
  RelocatableTuple(const RelocatableTuple &Other)
      : Offset(Other.Offset), Name(Other.Name),
        SuppressMangling(Other.SuppressMangling) {}

  const int64_t Offset;
  const IceString Name;
  bool SuppressMangling;
};

bool operator<(const RelocatableTuple &A, const RelocatableTuple &B);

// ConstantRelocatable represents a symbolic constant combined with
// a fixed offset.
class ConstantRelocatable : public Constant {
public:
  static ConstantRelocatable *create(GlobalContext *Ctx, Type Ty,
                                     const RelocatableTuple &Tuple) {
    return new (Ctx->allocate<ConstantRelocatable>()) ConstantRelocatable(
        Ty, Tuple.Offset, Tuple.Name, Tuple.SuppressMangling);
  }
  int64_t getOffset() const { return Offset; }
  IceString getName() const { return Name; }
  void setSuppressMangling(bool Value) { SuppressMangling = Value; }
  bool getSuppressMangling() const { return SuppressMangling; }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind == kConstRelocatable;
  }

private:
  ConstantRelocatable(Type Ty, int64_t Offset, const IceString &Name,
                      bool SuppressMangling)
      : Constant(kConstRelocatable, Ty), Offset(Offset), Name(Name),
        SuppressMangling(SuppressMangling) {}
  ConstantRelocatable(const ConstantRelocatable &) LLVM_DELETED_FUNCTION;
  ConstantRelocatable &
  operator=(const ConstantRelocatable &) LLVM_DELETED_FUNCTION;
  virtual ~ConstantRelocatable() {}
  const int64_t Offset; // fixed offset to add
  const IceString Name; // optional for debug/dump
  bool SuppressMangling;
};

// RegWeight is a wrapper for a uint32_t weight value, with a
// special value that represents infinite weight, and an addWeight()
// method that ensures that W+infinity=infinity.
class RegWeight {
public:
  RegWeight() : Weight(0) {}
  RegWeight(uint32_t Weight) : Weight(Weight) {}
  const static uint32_t Inf = ~0; // Force regalloc to give a register
  const static uint32_t Zero = 0; // Force regalloc NOT to give a register
  void addWeight(uint32_t Delta) {
    if (Delta == Inf)
      Weight = Inf;
    else if (Weight != Inf)
      Weight += Delta;
  }
  void addWeight(const RegWeight &Other) { addWeight(Other.Weight); }
  void setWeight(uint32_t Val) { Weight = Val; }
  uint32_t getWeight() const { return Weight; }
  bool isInf() const { return Weight == Inf; }

private:
  uint32_t Weight;
};
Ostream &operator<<(Ostream &Str, const RegWeight &W);
bool operator<(const RegWeight &A, const RegWeight &B);
bool operator<=(const RegWeight &A, const RegWeight &B);
bool operator==(const RegWeight &A, const RegWeight &B);

// Variable represents an operand that is register-allocated or
// stack-allocated.  If it is register-allocated, it will ultimately
// have a non-negative RegNum field.
class Variable : public Operand {
public:
  static Variable *create(Cfg *Func, Type Ty, const CfgNode *Node, SizeT Index,
                          const IceString &Name) {
    return new (Func->allocate<Variable>()) Variable(Ty, Node, Index, Name);
  }

  SizeT getIndex() const { return Number; }
  IceString getName() const;

  Inst *getDefinition() const { return DefInst; }
  void setDefinition(Inst *Inst, const CfgNode *Node);
  void replaceDefinition(Inst *Inst, const CfgNode *Node);

  const CfgNode *getLocalUseNode() const { return DefNode; }
  bool isMultiblockLife() const { return (DefNode == NULL); }
  void setUse(const Inst *Inst, const CfgNode *Node);

  bool getIsArg() const { return IsArgument; }
  void setIsArg(Cfg *Func);

  int32_t getStackOffset() const { return StackOffset; }
  void setStackOffset(int32_t Offset) { StackOffset = Offset; }

  static const int32_t NoRegister = -1;
  bool hasReg() const { return getRegNum() != NoRegister; }
  int32_t getRegNum() const { return RegNum; }
  void setRegNum(int32_t NewRegNum) {
    // Regnum shouldn't be set more than once.
    assert(!hasReg() || RegNum == NewRegNum);
    RegNum = NewRegNum;
  }

  RegWeight getWeight() const { return Weight; }
  void setWeight(uint32_t NewWeight) { Weight = NewWeight; }
  void setWeightInfinite() { Weight = RegWeight::Inf; }

  Variable *getPreferredRegister() const { return RegisterPreference; }
  bool getRegisterOverlap() const { return AllowRegisterOverlap; }
  void setPreferredRegister(Variable *Prefer, bool Overlap) {
    RegisterPreference = Prefer;
    AllowRegisterOverlap = Overlap;
  }

  Variable *getLo() const { return LoVar; }
  Variable *getHi() const { return HiVar; }
  void setLoHi(Variable *Lo, Variable *Hi) {
    assert(LoVar == NULL);
    assert(HiVar == NULL);
    LoVar = Lo;
    HiVar = Hi;
  }
  // Creates a temporary copy of the variable with a different type.
  // Used primarily for syntactic correctness of textual assembly
  // emission.  Note that only basic information is copied, in
  // particular not DefInst, IsArgument, Weight, RegisterPreference,
  // AllowRegisterOverlap, LoVar, HiVar, VarsReal.
  Variable asType(Type Ty);

  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == kVariable;
  }

  // The destructor is public because of the asType() method.
  virtual ~Variable() {}

private:
  Variable(Type Ty, const CfgNode *Node, SizeT Index, const IceString &Name)
      : Operand(kVariable, Ty), Number(Index), Name(Name), DefInst(NULL),
        DefNode(Node), IsArgument(false), StackOffset(0), RegNum(NoRegister),
        Weight(1), RegisterPreference(NULL), AllowRegisterOverlap(false),
        LoVar(NULL), HiVar(NULL) {
    Vars = VarsReal;
    Vars[0] = this;
    NumVars = 1;
  }
  Variable(const Variable &) LLVM_DELETED_FUNCTION;
  Variable &operator=(const Variable &) LLVM_DELETED_FUNCTION;
  // Number is unique across all variables, and is used as a
  // (bit)vector index for liveness analysis.
  const SizeT Number;
  // Name is optional.
  const IceString Name;
  // DefInst is the instruction that produces this variable as its
  // dest.
  Inst *DefInst;
  // DefNode is the node where this variable was produced, and is
  // reset to NULL if it is used outside that node.  This is used for
  // detecting isMultiblockLife().  TODO: Collapse this to a single
  // bit and use a separate pass to calculate the values across the
  // Cfg.  This saves space in the Variable, and removes the fragility
  // of incrementally computing and maintaining the information.
  const CfgNode *DefNode;
  bool IsArgument;
  // StackOffset is the canonical location on stack (only if
  // RegNum<0 || IsArgument).
  int32_t StackOffset;
  // RegNum is the allocated register, or NoRegister if it isn't
  // register-allocated.
  int32_t RegNum;
  RegWeight Weight; // Register allocation priority
  // RegisterPreference says that if possible, the register allocator
  // should prefer the register that was assigned to this linked
  // variable.  It also allows a spill slot to share its stack
  // location with another variable, if that variable does not get
  // register-allocated and therefore has a stack location.
  Variable *RegisterPreference;
  // AllowRegisterOverlap says that it is OK to honor
  // RegisterPreference and "share" a register even if the two live
  // ranges overlap.
  bool AllowRegisterOverlap;
  // LoVar and HiVar are needed for lowering from 64 to 32 bits.  When
  // lowering from I64 to I32 on a 32-bit architecture, we split the
  // variable into two machine-size pieces.  LoVar is the low-order
  // machine-size portion, and HiVar is the remaining high-order
  // portion.  TODO: It's wasteful to penalize all variables on all
  // targets this way; use a sparser representation.  It's also
  // wasteful for a 64-bit target.
  Variable *LoVar;
  Variable *HiVar;
  // VarsReal (and Operand::Vars) are set up such that Vars[0] ==
  // this.
  Variable *VarsReal[1];
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEOPERAND_H
