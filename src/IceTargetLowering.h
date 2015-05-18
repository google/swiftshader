//===- subzero/src/IceTargetLowering.h - Lowering interface -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLowering, LoweringContext, and
// TargetDataLowering classes.  TargetLowering is an abstract class
// used to drive the translation/lowering process.  LoweringContext
// maintains a context for lowering each instruction, offering
// conveniences such as iterating over non-deleted instructions.
// TargetDataLowering is an abstract class used to drive the
// lowering/emission of global initializers, external global
// declarations, and internal constant pools.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERING_H
#define SUBZERO_SRC_ICETARGETLOWERING_H

#include "IceDefs.h"
#include "IceInst.h" // for the names of the Inst subtypes
#include "IceOperand.h"
#include "IceTypes.h"

namespace Ice {

// LoweringContext makes it easy to iterate through non-deleted
// instructions in a node, and insert new (lowered) instructions at
// the current point.  Along with the instruction list container and
// associated iterators, it holds the current node, which is needed
// when inserting new instructions in order to track whether variables
// are used as single-block or multi-block.
class LoweringContext {
  LoweringContext(const LoweringContext &) = delete;
  LoweringContext &operator=(const LoweringContext &) = delete;

public:
  LoweringContext() : Node(nullptr), LastInserted(nullptr) {}
  ~LoweringContext() {}
  void init(CfgNode *Node);
  Inst *getNextInst() const {
    if (Next == End)
      return nullptr;
    return Next;
  }
  Inst *getNextInst(InstList::iterator &Iter) const {
    advanceForward(Iter);
    if (Iter == End)
      return nullptr;
    return Iter;
  }
  CfgNode *getNode() const { return Node; }
  bool atEnd() const { return Cur == End; }
  InstList::iterator getCur() const { return Cur; }
  InstList::iterator getNext() const { return Next; }
  InstList::iterator getEnd() const { return End; }
  void insert(Inst *Inst);
  Inst *getLastInserted() const;
  void advanceCur() { Cur = Next; }
  void advanceNext() { advanceForward(Next); }
  void rewind();
  void setInsertPoint(const InstList::iterator &Position) { Next = Position; }

private:
  // Node is the argument to Inst::updateVars().
  CfgNode *Node;
  Inst *LastInserted;
  // Cur points to the current instruction being considered.  It is
  // guaranteed to point to a non-deleted instruction, or to be End.
  InstList::iterator Cur;
  // Next doubles as a pointer to the next valid instruction (if any),
  // and the new-instruction insertion point.  It is also updated for
  // the caller in case the lowering consumes more than one high-level
  // instruction.  It is guaranteed to point to a non-deleted
  // instruction after Cur, or to be End.  TODO: Consider separating
  // the notion of "next valid instruction" and "new instruction
  // insertion point", to avoid confusion when previously-deleted
  // instructions come between the two points.
  InstList::iterator Next;
  // Begin is a copy of Insts.begin(), used if iterators are moved backward.
  InstList::iterator Begin;
  // End is a copy of Insts.end(), used if Next needs to be advanced.
  InstList::iterator End;

  void skipDeleted(InstList::iterator &I) const;
  void advanceForward(InstList::iterator &I) const;
};

class TargetLowering {
  TargetLowering() = delete;
  TargetLowering(const TargetLowering &) = delete;
  TargetLowering &operator=(const TargetLowering &) = delete;

public:
  // TODO(jvoung): return a unique_ptr like the other factory functions.
  static TargetLowering *createLowering(TargetArch Target, Cfg *Func);
  static std::unique_ptr<Assembler> createAssembler(TargetArch Target,
                                                    Cfg *Func);
  void translate() {
    switch (Ctx->getFlags().getOptLevel()) {
    case Opt_m1:
      translateOm1();
      break;
    case Opt_0:
      translateO0();
      break;
    case Opt_1:
      translateO1();
      break;
    case Opt_2:
      translateO2();
      break;
    }
  }
  virtual void translateOm1() {
    Func->setError("Target doesn't specify Om1 lowering steps.");
  }
  virtual void translateO0() {
    Func->setError("Target doesn't specify O0 lowering steps.");
  }
  virtual void translateO1() {
    Func->setError("Target doesn't specify O1 lowering steps.");
  }
  virtual void translateO2() {
    Func->setError("Target doesn't specify O2 lowering steps.");
  }

  // Tries to do address mode optimization on a single instruction.
  void doAddressOpt();
  // Randomly insert NOPs.
  void doNopInsertion();
  // Lowers a single non-Phi instruction.
  void lower();
  // Does preliminary lowering of the set of Phi instructions in the
  // current node.  The main intention is to do what's needed to keep
  // the unlowered Phi instructions consistent with the lowered
  // non-Phi instructions, e.g. to lower 64-bit operands on a 32-bit
  // target.
  virtual void prelowerPhis() {}
  // Lowers a list of "parallel" assignment instructions representing
  // a topological sort of the Phi instructions.
  virtual void lowerPhiAssignments(CfgNode *Node,
                                   const AssignList &Assignments) = 0;
  // Tries to do branch optimization on a single instruction.  Returns
  // true if some optimization was done.
  virtual bool doBranchOpt(Inst * /*I*/, const CfgNode * /*NextNode*/) {
    return false;
  }

  virtual SizeT getNumRegisters() const = 0;
  // Returns a variable pre-colored to the specified physical
  // register.  This is generally used to get very direct access to
  // the register such as in the prolog or epilog or for marking
  // scratch registers as killed by a call.  If a Type is not
  // provided, a target-specific default type is used.
  virtual Variable *getPhysicalRegister(SizeT RegNum,
                                        Type Ty = IceType_void) = 0;
  // Returns a printable name for the register.
  virtual IceString getRegName(SizeT RegNum, Type Ty) const = 0;

  virtual bool hasFramePointer() const { return false; }
  virtual SizeT getFrameOrStackReg() const = 0;
  virtual size_t typeWidthInBytesOnStack(Type Ty) const = 0;
  bool hasComputedFrame() const { return HasComputedFrame; }
  // Returns true if this function calls a function that has the
  // "returns twice" attribute.
  bool callsReturnsTwice() const { return CallsReturnsTwice; }
  void setCallsReturnsTwice(bool RetTwice) { CallsReturnsTwice = RetTwice; }
  int32_t getStackAdjustment() const { return StackAdjustment; }
  void updateStackAdjustment(int32_t Offset) { StackAdjustment += Offset; }
  void resetStackAdjustment() { StackAdjustment = 0; }
  SizeT makeNextLabelNumber() { return NextLabelNumber++; }
  LoweringContext &getContext() { return Context; }

  enum RegSet {
    RegSet_None = 0,
    RegSet_CallerSave = 1 << 0,
    RegSet_CalleeSave = 1 << 1,
    RegSet_StackPointer = 1 << 2,
    RegSet_FramePointer = 1 << 3,
    RegSet_All = ~RegSet_None
  };
  typedef uint32_t RegSetMask;

  virtual llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                              RegSetMask Exclude) const = 0;
  virtual const llvm::SmallBitVector &getRegisterSetForType(Type Ty) const = 0;
  void regAlloc(RegAllocKind Kind);

  virtual void makeRandomRegisterPermutation(
      llvm::SmallVectorImpl<int32_t> &Permutation,
      const llvm::SmallBitVector &ExcludeRegisters) const = 0;

  // Save/restore any mutable state for the situation where code
  // emission needs multiple passes, such as sandboxing or relaxation.
  // Subclasses may provide their own implementation, but should be
  // sure to also call the parent class's methods.
  virtual void snapshotEmitState() {
    SnapshotStackAdjustment = StackAdjustment;
  }
  virtual void rollbackEmitState() {
    StackAdjustment = SnapshotStackAdjustment;
  }

  virtual void emitVariable(const Variable *Var) const = 0;

  void emitWithoutPrefix(const ConstantRelocatable *CR) const;
  void emit(const ConstantRelocatable *CR) const;
  virtual const char *getConstantPrefix() const = 0;

  virtual void emit(const ConstantUndef *C) const = 0;
  virtual void emit(const ConstantInteger32 *C) const = 0;
  virtual void emit(const ConstantInteger64 *C) const = 0;
  virtual void emit(const ConstantFloat *C) const = 0;
  virtual void emit(const ConstantDouble *C) const = 0;

  // Performs target-specific argument lowering.
  virtual void lowerArguments() = 0;

  virtual void initNodeForLowering(CfgNode *) {}
  virtual void addProlog(CfgNode *Node) = 0;
  virtual void addEpilog(CfgNode *Node) = 0;

  virtual ~TargetLowering() {}

protected:
  explicit TargetLowering(Cfg *Func);
  virtual void lowerAlloca(const InstAlloca *Inst) = 0;
  virtual void lowerArithmetic(const InstArithmetic *Inst) = 0;
  virtual void lowerAssign(const InstAssign *Inst) = 0;
  virtual void lowerBr(const InstBr *Inst) = 0;
  virtual void lowerCall(const InstCall *Inst) = 0;
  virtual void lowerCast(const InstCast *Inst) = 0;
  virtual void lowerFcmp(const InstFcmp *Inst) = 0;
  virtual void lowerExtractElement(const InstExtractElement *Inst) = 0;
  virtual void lowerIcmp(const InstIcmp *Inst) = 0;
  virtual void lowerInsertElement(const InstInsertElement *Inst) = 0;
  virtual void lowerIntrinsicCall(const InstIntrinsicCall *Inst) = 0;
  virtual void lowerLoad(const InstLoad *Inst) = 0;
  virtual void lowerPhi(const InstPhi *Inst) = 0;
  virtual void lowerRet(const InstRet *Inst) = 0;
  virtual void lowerSelect(const InstSelect *Inst) = 0;
  virtual void lowerStore(const InstStore *Inst) = 0;
  virtual void lowerSwitch(const InstSwitch *Inst) = 0;
  virtual void lowerUnreachable(const InstUnreachable *Inst) = 0;

  virtual void doAddressOptLoad() {}
  virtual void doAddressOptStore() {}
  virtual void randomlyInsertNop(float Probability) = 0;
  // This gives the target an opportunity to post-process the lowered
  // expansion before returning.
  virtual void postLower() {}

  // Find two-address non-SSA instructions and set the DestNonKillable flag
  // to keep liveness analysis consistent.
  void inferTwoAddress();

  // Make a call to an external helper function.
  InstCall *makeHelperCall(const IceString &Name, Variable *Dest,
                           SizeT MaxSrcs);

  Cfg *Func;
  GlobalContext *Ctx;
  bool HasComputedFrame;
  bool CallsReturnsTwice;
  // StackAdjustment keeps track of the current stack offset from its
  // natural location, as arguments are pushed for a function call.
  int32_t StackAdjustment;
  SizeT NextLabelNumber;
  LoweringContext Context;

  // Runtime helper function names
  const static constexpr char *H_bitcast_16xi1_i16 = "__Sz_bitcast_16xi1_i16";
  const static constexpr char *H_bitcast_8xi1_i8 = "__Sz_bitcast_8xi1_i8";
  const static constexpr char *H_bitcast_i16_16xi1 = "__Sz_bitcast_i16_16xi1";
  const static constexpr char *H_bitcast_i8_8xi1 = "__Sz_bitcast_i8_8xi1";
  const static constexpr char *H_call_ctpop_i32 = "__popcountsi2";
  const static constexpr char *H_call_ctpop_i64 = "__popcountdi2";
  const static constexpr char *H_call_longjmp = "longjmp";
  const static constexpr char *H_call_memcpy = "memcpy";
  const static constexpr char *H_call_memmove = "memmove";
  const static constexpr char *H_call_memset = "memset";
  const static constexpr char *H_call_read_tp = "__nacl_read_tp";
  const static constexpr char *H_call_setjmp = "setjmp";
  const static constexpr char *H_fptosi_f32_i64 = "__Sz_fptosi_f32_i64";
  const static constexpr char *H_fptosi_f64_i64 = "__Sz_fptosi_f64_i64";
  const static constexpr char *H_fptoui_4xi32_f32 = "__Sz_fptoui_4xi32_f32";
  const static constexpr char *H_fptoui_f32_i32 = "__Sz_fptoui_f32_i32";
  const static constexpr char *H_fptoui_f32_i64 = "__Sz_fptoui_f32_i64";
  const static constexpr char *H_fptoui_f64_i32 = "__Sz_fptoui_f64_i32";
  const static constexpr char *H_fptoui_f64_i64 = "__Sz_fptoui_f64_i64";
  const static constexpr char *H_frem_f32 = "fmodf";
  const static constexpr char *H_frem_f64 = "fmod";
  const static constexpr char *H_sdiv_i64 = "__divdi3";
  const static constexpr char *H_sitofp_i64_f32 = "__Sz_sitofp_i64_f32";
  const static constexpr char *H_sitofp_i64_f64 = "__Sz_sitofp_i64_f64";
  const static constexpr char *H_srem_i64 = "__moddi3";
  const static constexpr char *H_udiv_i64 = "__udivdi3";
  const static constexpr char *H_uitofp_4xi32_4xf32 = "__Sz_uitofp_4xi32_4xf32";
  const static constexpr char *H_uitofp_i32_f32 = "__Sz_uitofp_i32_f32";
  const static constexpr char *H_uitofp_i32_f64 = "__Sz_uitofp_i32_f64";
  const static constexpr char *H_uitofp_i64_f32 = "__Sz_uitofp_i64_f32";
  const static constexpr char *H_uitofp_i64_f64 = "__Sz_uitofp_i64_f64";
  const static constexpr char *H_urem_i64 = "__umoddi3";

private:
  int32_t SnapshotStackAdjustment;
};

// TargetDataLowering is used for "lowering" data including initializers
// for global variables, and the internal constant pools.  It is separated
// out from TargetLowering because it does not require a Cfg.
class TargetDataLowering {
  TargetDataLowering() = delete;
  TargetDataLowering(const TargetDataLowering &) = delete;
  TargetDataLowering &operator=(const TargetDataLowering &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> createLowering(GlobalContext *Ctx);
  virtual ~TargetDataLowering();

  virtual void
  lowerGlobals(std::unique_ptr<VariableDeclarationList> Vars) const = 0;
  virtual void lowerConstants() const = 0;

protected:
  explicit TargetDataLowering(GlobalContext *Ctx) : Ctx(Ctx) {}
  GlobalContext *Ctx;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERING_H
