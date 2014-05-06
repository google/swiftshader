//===---- llvm/Support/IRBuilder.h - Builder for LLVM Instrs ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the IRBuilder class, which is used as a convenient way
// to create LLVM instructions with a consistent and simplified interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_IRBUILDER_H
#define LLVM_SUPPORT_IRBUILDER_H

#include "llvm/Instructions.h"
#include "llvm/BasicBlock.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/ConstantFolder.h"

namespace llvm {
  class MDNode;

/// IRBuilderDefaultInserter - This provides the default implementation of the
/// IRBuilder 'InsertHelper' method that is called whenever an instruction is
/// created by IRBuilder and needs to be inserted.  By default, this inserts the
/// instruction at the insertion point.
class IRBuilderDefaultInserter {
protected:
  void InsertHelper(Instruction *I,
                    BasicBlock *BB, BasicBlock::iterator InsertPt) const {
    if (BB) BB->getInstList().insert(InsertPt, I);
  }
};

/// IRBuilderBase - Common base class shared among various IRBuilders.
class IRBuilderBase {
  DebugLoc CurDbgLocation;
protected:
  BasicBlock *BB;
  BasicBlock::iterator InsertPt;
  LLVMContext &Context;
public:
  
  IRBuilderBase(LLVMContext &context)
    : Context(context) {
    ClearInsertionPoint();
  }
  
  //===--------------------------------------------------------------------===//
  // Builder configuration methods
  //===--------------------------------------------------------------------===//
  
  /// ClearInsertionPoint - Clear the insertion point: created instructions will
  /// not be inserted into a block.
  void ClearInsertionPoint() {
    BB = 0;
  }
  
  BasicBlock *GetInsertBlock() const { return BB; }
  BasicBlock::iterator GetInsertPoint() const { return InsertPt; }
  LLVMContext &getContext() const { return Context; }
  
  /// SetInsertPoint - This specifies that created instructions should be
  /// appended to the end of the specified block.
  void SetInsertPoint(BasicBlock *TheBB) {
    BB = TheBB;
    InsertPt = BB->end();
  }
  
  /// SetInsertPoint - This specifies that created instructions should be
  /// inserted at the specified point.
  void SetInsertPoint(BasicBlock *TheBB, BasicBlock::iterator IP) {
    BB = TheBB;
    InsertPt = IP;
  }
  
  /// SetCurrentDebugLocation - Set location information used by debugging
  /// information.
  void SetCurrentDebugLocation(const DebugLoc &L) {
    CurDbgLocation = L;
  }
  
  /// getCurrentDebugLocation - Get location information used by debugging
  /// information.
  const DebugLoc &getCurrentDebugLocation() const { return CurDbgLocation; }
  
  /// SetInstDebugLocation - If this builder has a current debug location, set
  /// it on the specified instruction.
  void SetInstDebugLocation(Instruction *I) const {
    if (!CurDbgLocation.isUnknown())
      I->setDebugLoc(CurDbgLocation);
  }

  /// InsertPoint - A saved insertion point.
  class InsertPoint {
    BasicBlock *Block;
    BasicBlock::iterator Point;

  public:
    /// Creates a new insertion point which doesn't point to anything.
    InsertPoint() : Block(0) {}

    /// Creates a new insertion point at the given location.
    InsertPoint(BasicBlock *InsertBlock, BasicBlock::iterator InsertPoint)
      : Block(InsertBlock), Point(InsertPoint) {}

    /// isSet - Returns true if this insert point is set.
    bool isSet() const { return (Block != 0); }

    llvm::BasicBlock *getBlock() const { return Block; }
    llvm::BasicBlock::iterator getPoint() const { return Point; }
  };

  /// saveIP - Returns the current insert point.
  InsertPoint saveIP() const {
    return InsertPoint(GetInsertBlock(), GetInsertPoint());
  }

  /// saveAndClearIP - Returns the current insert point, clearing it
  /// in the process.
  InsertPoint saveAndClearIP() {
    InsertPoint IP(GetInsertBlock(), GetInsertPoint());
    ClearInsertionPoint();
    return IP;
  }

  /// restoreIP - Sets the current insert point to a previously-saved
  /// location.
  void restoreIP(InsertPoint IP) {
    if (IP.isSet())
      SetInsertPoint(IP.getBlock(), IP.getPoint());
    else
      ClearInsertionPoint();
  }

  //===--------------------------------------------------------------------===//
  // Miscellaneous creation methods.
  //===--------------------------------------------------------------------===//
  
  /// CreateGlobalString - Make a new global variable with an initializer that
  /// has array of i8 type filled in with the nul terminated string value
  /// specified.  If Name is specified, it is the name of the global variable
  /// created.
  Value *CreateGlobalString(const char *Str = "", const Twine &Name = "");

  /// getInt1 - Get a constant value representing either true or false.
  ConstantInt *getInt1(bool V) {
    return ConstantInt::get(getInt1Ty(), V);
  }

  /// getTrue - Get the constant value for i1 true.
  ConstantInt *getTrue() {
    return ConstantInt::getTrue(Context);
  }

  /// getFalse - Get the constant value for i1 false.
  ConstantInt *getFalse() {
    return ConstantInt::getFalse(Context);
  }

  /// getInt8 - Get a constant 8-bit value.
  ConstantInt *getInt8(uint8_t C) {
    return ConstantInt::get(getInt8Ty(), C);
  }

  /// getInt16 - Get a constant 16-bit value.
  ConstantInt *getInt16(uint16_t C) {
    return ConstantInt::get(getInt16Ty(), C);
  }

  /// getInt32 - Get a constant 32-bit value.
  ConstantInt *getInt32(uint32_t C) {
    return ConstantInt::get(getInt32Ty(), C);
  }
  
  /// getInt64 - Get a constant 64-bit value.
  ConstantInt *getInt64(uint64_t C) {
    return ConstantInt::get(getInt64Ty(), C);
  }
  
  //===--------------------------------------------------------------------===//
  // Type creation methods
  //===--------------------------------------------------------------------===//
  
  /// getInt1Ty - Fetch the type representing a single bit
  const IntegerType *getInt1Ty() {
    return Type::getInt1Ty(Context);
  }
  
  /// getInt8Ty - Fetch the type representing an 8-bit integer.
  const IntegerType *getInt8Ty() {
    return Type::getInt8Ty(Context);
  }
  
  /// getInt16Ty - Fetch the type representing a 16-bit integer.
  const IntegerType *getInt16Ty() {
    return Type::getInt16Ty(Context);
  }
  
  /// getInt32Ty - Fetch the type resepresenting a 32-bit integer.
  const IntegerType *getInt32Ty() {
    return Type::getInt32Ty(Context);
  }
  
  /// getInt64Ty - Fetch the type representing a 64-bit integer.
  const IntegerType *getInt64Ty() {
    return Type::getInt64Ty(Context);
  }
  
  /// getFloatTy - Fetch the type representing a 32-bit floating point value.
  const Type *getFloatTy() {
    return Type::getFloatTy(Context);
  }
  
  /// getDoubleTy - Fetch the type representing a 64-bit floating point value.
  const Type *getDoubleTy() {
    return Type::getDoubleTy(Context);
  }
  
  /// getVoidTy - Fetch the type representing void.
  const Type *getVoidTy() {
    return Type::getVoidTy(Context);
  }
  
  const PointerType *getInt8PtrTy() {
    return Type::getInt8PtrTy(Context);
  }
  
  /// getCurrentFunctionReturnType - Get the return type of the current function
  /// that we're emitting into.
  const Type *getCurrentFunctionReturnType() const;
};
  
/// IRBuilder - This provides a uniform API for creating instructions and
/// inserting them into a basic block: either at the end of a BasicBlock, or
/// at a specific iterator location in a block.
///
/// Note that the builder does not expose the full generality of LLVM
/// instructions.  For access to extra instruction properties, use the mutators
/// (e.g. setVolatile) on the instructions after they have been created.
/// The first template argument handles whether or not to preserve names in the
/// final instruction output. This defaults to on.  The second template argument
/// specifies a class to use for creating constants.  This defaults to creating
/// minimally folded constants.  The fourth template argument allows clients to
/// specify custom insertion hooks that are called on every newly created
/// insertion.
template<typename T = ConstantFolder,
         typename Inserter = IRBuilderDefaultInserter >
class IRBuilder : public IRBuilderBase, public Inserter {
  T Folder;
public:
  IRBuilder(LLVMContext &C, const T &F, const Inserter &I = Inserter())
    : IRBuilderBase(C), Inserter(I), Folder(F) {
  }
  
  explicit IRBuilder(LLVMContext &C) : IRBuilderBase(C), Folder(C) {
  }
  
  explicit IRBuilder(BasicBlock *TheBB, const T &F)
    : IRBuilderBase(TheBB->getContext()), Folder(F) {
    SetInsertPoint(TheBB);
  }
  
  explicit IRBuilder(BasicBlock *TheBB)
    : IRBuilderBase(TheBB->getContext()), Folder(Context) {
    SetInsertPoint(TheBB);
  }
  
  IRBuilder(BasicBlock *TheBB, BasicBlock::iterator IP, const T& F)
    : IRBuilderBase(TheBB->getContext()), Folder(F) {
    SetInsertPoint(TheBB, IP);
  }
  
  IRBuilder(BasicBlock *TheBB, BasicBlock::iterator IP)
    : IRBuilderBase(TheBB->getContext()), Folder(Context) {
    SetInsertPoint(TheBB, IP);
  }

  /// getFolder - Get the constant folder being used.
  const T &getFolder() { return Folder; }
    
  /// Insert - Insert and return the specified instruction.
  template<typename InstTy>
  InstTy *Insert(InstTy *I) const {
    this->InsertHelper(I, BB, InsertPt);
    if (!getCurrentDebugLocation().isUnknown())
      this->SetInstDebugLocation(I);
    return I;
  }

  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Terminators
  //===--------------------------------------------------------------------===//

  /// CreateRetVoid - Create a 'ret void' instruction.
  ReturnInst *CreateRetVoid() {
    return Insert(ReturnInst::Create(Context));
  }

  /// @verbatim
  /// CreateRet - Create a 'ret <val>' instruction.
  /// @endverbatim
  ReturnInst *CreateRet(Value *V) {
    return Insert(ReturnInst::Create(Context, V));
  }
  
  /// CreateAggregateRet - Create a sequence of N insertvalue instructions,
  /// with one Value from the retVals array each, that build a aggregate
  /// return value one value at a time, and a ret instruction to return
  /// the resulting aggregate value. This is a convenience function for
  /// code that uses aggregate return values as a vehicle for having
  /// multiple return values.
  ///
  ReturnInst *CreateAggregateRet(Value *const *retVals, unsigned N) {
    Value *V = UndefValue::get(getCurrentFunctionReturnType());
    for (unsigned i = 0; i != N; ++i)
      V = CreateInsertValue(V, retVals[i], i, "mrv");
    return Insert(ReturnInst::Create(Context, V));
  }

  /// CreateBr - Create an unconditional 'br label X' instruction.
  BranchInst *CreateBr(BasicBlock *Dest) {
    return Insert(BranchInst::Create(Dest));
  }

  /// CreateCondBr - Create a conditional 'br Cond, TrueDest, FalseDest'
  /// instruction.
  BranchInst *CreateCondBr(Value *Cond, BasicBlock *True, BasicBlock *False) {
    return Insert(BranchInst::Create(True, False, Cond));
  }

  /// CreateSwitch - Create a switch instruction with the specified value,
  /// default dest, and with a hint for the number of cases that will be added
  /// (for efficient allocation).
  SwitchInst *CreateSwitch(Value *V, BasicBlock *Dest, unsigned NumCases = 10) {
    return Insert(SwitchInst::Create(V, Dest, NumCases));
  }

  /// CreateIndirectBr - Create an indirect branch instruction with the
  /// specified address operand, with an optional hint for the number of
  /// destinations that will be added (for efficient allocation).
  IndirectBrInst *CreateIndirectBr(Value *Addr, unsigned NumDests = 10) {
    return Insert(IndirectBrInst::Create(Addr, NumDests));
  }

  UnreachableInst *CreateUnreachable() {
    return Insert(new UnreachableInst(Context));
  }

  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Binary Operators
  //===--------------------------------------------------------------------===//

  Value *CreateAdd(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateAdd(LC, RC);
    return Insert(BinaryOperator::CreateAdd(LHS, RHS));
  }
  Value *CreateNSWAdd(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNSWAdd(LC, RC);
    return Insert(BinaryOperator::CreateNSWAdd(LHS, RHS));
  }
  Value *CreateNUWAdd(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNUWAdd(LC, RC);
    return Insert(BinaryOperator::CreateNUWAdd(LHS, RHS));
  }
  Value *CreateFAdd(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFAdd(LC, RC);
    return Insert(BinaryOperator::CreateFAdd(LHS, RHS));
  }
  Value *CreateSub(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateSub(LC, RC);
    return Insert(BinaryOperator::CreateSub(LHS, RHS));
  }
  Value *CreateNSWSub(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNSWSub(LC, RC);
    return Insert(BinaryOperator::CreateNSWSub(LHS, RHS));
  }
  Value *CreateNUWSub(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNUWSub(LC, RC);
    return Insert(BinaryOperator::CreateNUWSub(LHS, RHS));
  }
  Value *CreateFSub(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFSub(LC, RC);
    return Insert(BinaryOperator::CreateFSub(LHS, RHS));
  }
  Value *CreateMul(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateMul(LC, RC);
    return Insert(BinaryOperator::CreateMul(LHS, RHS));
  }
  Value *CreateNSWMul(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNSWMul(LC, RC);
    return Insert(BinaryOperator::CreateNSWMul(LHS, RHS));
  }
  Value *CreateNUWMul(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateNUWMul(LC, RC);
    return Insert(BinaryOperator::CreateNUWMul(LHS, RHS));
  }
  Value *CreateFMul(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFMul(LC, RC);
    return Insert(BinaryOperator::CreateFMul(LHS, RHS));
  }
  Value *CreateUDiv(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateUDiv(LC, RC);
    return Insert(BinaryOperator::CreateUDiv(LHS, RHS));
  }
  Value *CreateSDiv(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateSDiv(LC, RC);
    return Insert(BinaryOperator::CreateSDiv(LHS, RHS));
  }
  Value *CreateExactSDiv(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateExactSDiv(LC, RC);
    return Insert(BinaryOperator::CreateExactSDiv(LHS, RHS));
  }
  Value *CreateFDiv(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFDiv(LC, RC);
    return Insert(BinaryOperator::CreateFDiv(LHS, RHS));
  }
  Value *CreateURem(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateURem(LC, RC);
    return Insert(BinaryOperator::CreateURem(LHS, RHS));
  }
  Value *CreateSRem(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateSRem(LC, RC);
    return Insert(BinaryOperator::CreateSRem(LHS, RHS));
  }
  Value *CreateFRem(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFRem(LC, RC);
    return Insert(BinaryOperator::CreateFRem(LHS, RHS));
  }

  Value *CreateShl(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateShl(LC, RC);
    return Insert(BinaryOperator::CreateShl(LHS, RHS));
  }
  Value *CreateShl(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateShl(LC, RHSC);
    return Insert(BinaryOperator::CreateShl(LHS, RHSC));
  }
  Value *CreateShl(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateShl(LC, RHSC);
    return Insert(BinaryOperator::CreateShl(LHS, RHSC));
  }

  Value *CreateLShr(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateLShr(LC, RC);
    return Insert(BinaryOperator::CreateLShr(LHS, RHS));
  }
  Value *CreateLShr(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateLShr(LC, RHSC);
    return Insert(BinaryOperator::CreateLShr(LHS, RHSC));
  }
  Value *CreateLShr(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateLShr(LC, RHSC);
    return Insert(BinaryOperator::CreateLShr(LHS, RHSC));
  }

  Value *CreateAShr(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateAShr(LC, RC);
    return Insert(BinaryOperator::CreateAShr(LHS, RHS));
  }
  Value *CreateAShr(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateAShr(LC, RHSC);
    return Insert(BinaryOperator::CreateAShr(LHS, RHSC));
  }
  Value *CreateAShr(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateAShr(LC, RHSC);
    return Insert(BinaryOperator::CreateAShr(LHS, RHSC));
  }

  Value *CreateAnd(Value *LHS, Value *RHS) {
    if (Constant *RC = dyn_cast<Constant>(RHS)) {
      if (isa<ConstantInt>(RC) && cast<ConstantInt>(RC)->isAllOnesValue())
        return LHS;  // LHS & -1 -> LHS
      if (Constant *LC = dyn_cast<Constant>(LHS))
        return Folder.CreateAnd(LC, RC);
    }
    return Insert(BinaryOperator::CreateAnd(LHS, RHS));
  }
  Value *CreateAnd(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateAnd(LC, RHSC);
    return Insert(BinaryOperator::CreateAnd(LHS, RHSC));
  }
  Value *CreateAnd(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateAnd(LC, RHSC);
    return Insert(BinaryOperator::CreateAnd(LHS, RHSC));
  }

  Value *CreateOr(Value *LHS, Value *RHS) {
    if (Constant *RC = dyn_cast<Constant>(RHS)) {
      if (RC->isNullValue())
        return LHS;  // LHS | 0 -> LHS
      if (Constant *LC = dyn_cast<Constant>(LHS))
        return Folder.CreateOr(LC, RC);
    }
    return Insert(BinaryOperator::CreateOr(LHS, RHS));
  }
  Value *CreateOr(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateOr(LC, RHSC);
    return Insert(BinaryOperator::CreateOr(LHS, RHSC));
  }
  Value *CreateOr(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateOr(LC, RHSC);
    return Insert(BinaryOperator::CreateOr(LHS, RHSC));
  }

  Value *CreateXor(Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateXor(LC, RC);
    return Insert(BinaryOperator::CreateXor(LHS, RHS));
  }
  Value *CreateXor(Value *LHS, const APInt &RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateXor(LC, RHSC);
    return Insert(BinaryOperator::CreateXor(LHS, RHSC));
  }
  Value *CreateXor(Value *LHS, uint64_t RHS) {
    Constant *RHSC = ConstantInt::get(LHS->getType(), RHS);
    if (Constant *LC = dyn_cast<Constant>(LHS))
      return Folder.CreateXor(LC, RHSC);
    return Insert(BinaryOperator::CreateXor(LHS, RHSC));
  }

  Value *CreateBinOp(Instruction::BinaryOps Opc,
                     Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateBinOp(Opc, LC, RC);
    return Insert(BinaryOperator::Create(Opc, LHS, RHS));
  }

  Value *CreateNeg(Value *V) {
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateNeg(VC);
    return Insert(BinaryOperator::CreateNeg(V));
  }
  Value *CreateNSWNeg(Value *V) {
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateNSWNeg(VC);
    return Insert(BinaryOperator::CreateNSWNeg(V));
  }
  Value *CreateNUWNeg(Value *V) {
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateNUWNeg(VC);
    return Insert(BinaryOperator::CreateNUWNeg(V));
  }
  Value *CreateFNeg(Value *V) {
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateFNeg(VC);
    return Insert(BinaryOperator::CreateFNeg(V));
  }
  Value *CreateNot(Value *V) {
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateNot(VC);
    return Insert(BinaryOperator::CreateNot(V));
  }

  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Memory Instructions
  //===--------------------------------------------------------------------===//

  AllocaInst *CreateAlloca(const Type *Ty, Value *ArraySize = 0) {
    return Insert(new AllocaInst(Ty, ArraySize));
  }
  // Provided to resolve 'CreateLoad(Ptr, "...")' correctly, instead of
  // converting the string to 'bool' for the isVolatile parameter.
  LoadInst *CreateLoad(Value *Ptr) {
    return Insert(new LoadInst(Ptr));
  }
  LoadInst *CreateLoad(Value *Ptr, bool isVolatile) {
    return Insert(new LoadInst(Ptr, 0, isVolatile));
  }
  StoreInst *CreateStore(Value *Val, Value *Ptr, bool isVolatile = false) {
    return Insert(new StoreInst(Val, Ptr, isVolatile));
  }
  template<typename InputIterator>
  Value *CreateGEP(Value *Ptr, InputIterator IdxBegin, InputIterator IdxEnd) {
    if (Constant *PC = dyn_cast<Constant>(Ptr)) {
      // Every index must be constant.
      InputIterator i;
      for (i = IdxBegin; i < IdxEnd; ++i)
        if (!isa<Constant>(*i))
          break;
      if (i == IdxEnd)
        return Folder.CreateGetElementPtr(PC, &IdxBegin[0], IdxEnd - IdxBegin);
    }
    return Insert(GetElementPtrInst::Create(Ptr, IdxBegin, IdxEnd));
  }
  template<typename InputIterator>
  Value *CreateInBoundsGEP(Value *Ptr, InputIterator IdxBegin,
                           InputIterator IdxEnd) {
    if (Constant *PC = dyn_cast<Constant>(Ptr)) {
      // Every index must be constant.
      InputIterator i;
      for (i = IdxBegin; i < IdxEnd; ++i)
        if (!isa<Constant>(*i))
          break;
      if (i == IdxEnd)
        return Folder.CreateInBoundsGetElementPtr(PC,
                                                  &IdxBegin[0],
                                                  IdxEnd - IdxBegin);
    }
    return Insert(GetElementPtrInst::CreateInBounds(Ptr, IdxBegin, IdxEnd));
  }
  Value *CreateGEP(Value *Ptr, Value *Idx) {
    if (Constant *PC = dyn_cast<Constant>(Ptr))
      if (Constant *IC = dyn_cast<Constant>(Idx))
        return Folder.CreateGetElementPtr(PC, &IC, 1);
    return Insert(GetElementPtrInst::Create(Ptr, Idx));
  }
  Value *CreateInBoundsGEP(Value *Ptr, Value *Idx) {
    if (Constant *PC = dyn_cast<Constant>(Ptr))
      if (Constant *IC = dyn_cast<Constant>(Idx))
        return Folder.CreateInBoundsGetElementPtr(PC, &IC, 1);
    return Insert(GetElementPtrInst::CreateInBounds(Ptr, Idx));
  }
  Value *CreateConstGEP1_32(Value *Ptr, unsigned Idx0) {
    Value *Idx = ConstantInt::get(Type::getInt32Ty(Context), Idx0);

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateGetElementPtr(PC, &Idx, 1);

    return Insert(GetElementPtrInst::Create(Ptr, &Idx, &Idx+1));    
  }
  Value *CreateConstInBoundsGEP1_32(Value *Ptr, unsigned Idx0) {
    Value *Idx = ConstantInt::get(Type::getInt32Ty(Context), Idx0);

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateInBoundsGetElementPtr(PC, &Idx, 1);

    return Insert(GetElementPtrInst::CreateInBounds(Ptr, &Idx, &Idx+1));
  }
  Value *CreateConstGEP2_32(Value *Ptr, unsigned Idx0, unsigned Idx1) {
    Value *Idxs[] = {
      ConstantInt::get(Type::getInt32Ty(Context), Idx0),
      ConstantInt::get(Type::getInt32Ty(Context), Idx1)
    };

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateGetElementPtr(PC, Idxs, 2);

    return Insert(GetElementPtrInst::Create(Ptr, Idxs, Idxs+2));    
  }
  Value *CreateConstInBoundsGEP2_32(Value *Ptr, unsigned Idx0, unsigned Idx1) {
    Value *Idxs[] = {
      ConstantInt::get(Type::getInt32Ty(Context), Idx0),
      ConstantInt::get(Type::getInt32Ty(Context), Idx1)
    };

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateInBoundsGetElementPtr(PC, Idxs, 2);

    return Insert(GetElementPtrInst::CreateInBounds(Ptr, Idxs, Idxs+2));
  }
  Value *CreateConstGEP1_64(Value *Ptr, uint64_t Idx0) {
    Value *Idx = ConstantInt::get(Type::getInt64Ty(Context), Idx0);

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateGetElementPtr(PC, &Idx, 1);

    return Insert(GetElementPtrInst::Create(Ptr, &Idx, &Idx+1));    
  }
  Value *CreateConstInBoundsGEP1_64(Value *Ptr, uint64_t Idx0) {
    Value *Idx = ConstantInt::get(Type::getInt64Ty(Context), Idx0);

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateInBoundsGetElementPtr(PC, &Idx, 1);

    return Insert(GetElementPtrInst::CreateInBounds(Ptr, &Idx, &Idx+1));
  }
  Value *CreateConstGEP2_64(Value *Ptr, uint64_t Idx0, uint64_t Idx1) {
    Value *Idxs[] = {
      ConstantInt::get(Type::getInt64Ty(Context), Idx0),
      ConstantInt::get(Type::getInt64Ty(Context), Idx1)
    };

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateGetElementPtr(PC, Idxs, 2);

    return Insert(GetElementPtrInst::Create(Ptr, Idxs, Idxs+2));    
  }
  Value *CreateConstInBoundsGEP2_64(Value *Ptr, uint64_t Idx0, uint64_t Idx1) {
    Value *Idxs[] = {
      ConstantInt::get(Type::getInt64Ty(Context), Idx0),
      ConstantInt::get(Type::getInt64Ty(Context), Idx1)
    };

    if (Constant *PC = dyn_cast<Constant>(Ptr))
      return Folder.CreateInBoundsGetElementPtr(PC, Idxs, 2);

    return Insert(GetElementPtrInst::CreateInBounds(Ptr, Idxs, Idxs+2));
  }
  Value *CreateStructGEP(Value *Ptr, unsigned Idx) {
    return CreateConstInBoundsGEP2_32(Ptr, 0, Idx);
  }
  
  /// CreateGlobalStringPtr - Same as CreateGlobalString, but return a pointer
  /// with "i8*" type instead of a pointer to array of i8.
  Value *CreateGlobalStringPtr(const char *Str = "") {
    Value *gv = CreateGlobalString(Str);
    Value *zero = ConstantInt::get(Type::getInt32Ty(Context), 0);
    Value *Args[] = { zero, zero };
    return CreateInBoundsGEP(gv, Args, Args+2);
  }
  
  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Cast/Conversion Operators
  //===--------------------------------------------------------------------===//

  Value *CreateTrunc(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::Trunc, V, DestTy);
  }
  Value *CreateZExt(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::ZExt, V, DestTy);
  }
  Value *CreateSExt(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::SExt, V, DestTy);
  }
  Value *CreateFPToUI(Value *V, const Type *DestTy){
    return CreateCast(Instruction::FPToUI, V, DestTy);
  }
  Value *CreateFPToSI(Value *V, const Type *DestTy){
    return CreateCast(Instruction::FPToSI, V, DestTy);
  }
  Value *CreateUIToFP(Value *V, const Type *DestTy){
    return CreateCast(Instruction::UIToFP, V, DestTy);
  }
  Value *CreateSIToFP(Value *V, const Type *DestTy){
    return CreateCast(Instruction::SIToFP, V, DestTy);
  }
  Value *CreateFPTrunc(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::FPTrunc, V, DestTy);
  }
  Value *CreateFPExt(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::FPExt, V, DestTy);
  }
  Value *CreatePtrToInt(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::PtrToInt, V, DestTy);
  }
  Value *CreateIntToPtr(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::IntToPtr, V, DestTy);
  }
  Value *CreateBitCast(Value *V, const Type *DestTy) {
    return CreateCast(Instruction::BitCast, V, DestTy);
  }
  Value *CreateZExtOrBitCast(Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateZExtOrBitCast(VC, DestTy);
    return Insert(CastInst::CreateZExtOrBitCast(V, DestTy));
  }
  Value *CreateSExtOrBitCast(Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateSExtOrBitCast(VC, DestTy);
    return Insert(CastInst::CreateSExtOrBitCast(V, DestTy));
  }
  Value *CreateTruncOrBitCast(Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateTruncOrBitCast(VC, DestTy);
    return Insert(CastInst::CreateTruncOrBitCast(V, DestTy));
  }
  Value *CreateCast(Instruction::CastOps Op, Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateCast(Op, VC, DestTy);
    return Insert(CastInst::Create(Op, V, DestTy));
  }
  Value *CreatePointerCast(Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreatePointerCast(VC, DestTy);
    return Insert(CastInst::CreatePointerCast(V, DestTy));
  }
  Value *CreateIntCast(Value *V, const Type *DestTy, bool isSigned) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateIntCast(VC, DestTy, isSigned);
    return Insert(CastInst::CreateIntegerCast(V, DestTy, isSigned));
  }
private:
  // Provided to resolve 'CreateIntCast(Ptr, Ptr, "...")', giving a compile time
  // error, instead of converting the string to bool for the isSigned parameter.
  Value *CreateIntCast(Value *, const Type *, const char *); // DO NOT IMPLEMENT
public:
  Value *CreateFPCast(Value *V, const Type *DestTy) {
    if (V->getType() == DestTy)
      return V;
    if (Constant *VC = dyn_cast<Constant>(V))
      return Folder.CreateFPCast(VC, DestTy);
    return Insert(CastInst::CreateFPCast(V, DestTy));
  }

  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Compare Instructions
  //===--------------------------------------------------------------------===//

  Value *CreateICmpEQ(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_EQ, LHS, RHS);
  }
  Value *CreateICmpNE(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_NE, LHS, RHS);
  }
  Value *CreateICmpUGT(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_UGT, LHS, RHS);
  }
  Value *CreateICmpUGE(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_UGE, LHS, RHS);
  }
  Value *CreateICmpULT(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_ULT, LHS, RHS);
  }
  Value *CreateICmpULE(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_ULE, LHS, RHS);
  }
  Value *CreateICmpSGT(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_SGT, LHS, RHS);
  }
  Value *CreateICmpSGE(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_SGE, LHS, RHS);
  }
  Value *CreateICmpSLT(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_SLT, LHS, RHS);
  }
  Value *CreateICmpSLE(Value *LHS, Value *RHS) {
    return CreateICmp(ICmpInst::ICMP_SLE, LHS, RHS);
  }

  Value *CreateFCmpOEQ(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_OEQ, LHS, RHS);
  }
  Value *CreateFCmpOGT(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_OGT, LHS, RHS);
  }
  Value *CreateFCmpOGE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_OGE, LHS, RHS);
  }
  Value *CreateFCmpOLT(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_OLT, LHS, RHS);
  }
  Value *CreateFCmpOLE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_OLE, LHS, RHS);
  }
  Value *CreateFCmpONE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_ONE, LHS, RHS);
  }
  Value *CreateFCmpORD(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_ORD, LHS, RHS);
  }
  Value *CreateFCmpUNO(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_UNO, LHS, RHS);
  }
  Value *CreateFCmpUEQ(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_UEQ, LHS, RHS);
  }
  Value *CreateFCmpUGT(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_UGT, LHS, RHS);
  }
  Value *CreateFCmpUGE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_UGE, LHS, RHS);
  }
  Value *CreateFCmpULT(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_ULT, LHS, RHS);
  }
  Value *CreateFCmpULE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_ULE, LHS, RHS);
  }
  Value *CreateFCmpUNE(Value *LHS, Value *RHS) {
    return CreateFCmp(FCmpInst::FCMP_UNE, LHS, RHS);
  }

  Value *CreateICmp(CmpInst::Predicate P, Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateICmp(P, LC, RC);
    return Insert(new ICmpInst(P, LHS, RHS));
  }
  Value *CreateFCmp(CmpInst::Predicate P, Value *LHS, Value *RHS) {
    if (Constant *LC = dyn_cast<Constant>(LHS))
      if (Constant *RC = dyn_cast<Constant>(RHS))
        return Folder.CreateFCmp(P, LC, RC);
    return Insert(new FCmpInst(P, LHS, RHS));
  }

  //===--------------------------------------------------------------------===//
  // Instruction creation methods: Other Instructions
  //===--------------------------------------------------------------------===//

  PHINode *CreatePHI(const Type *Ty) {
    return Insert(PHINode::Create(Ty));
  }

  CallInst *CreateCall(Value *Callee) {
    return Insert(CallInst::Create(Callee));
  }
  CallInst *CreateCall(Value *Callee, Value *Arg) {
    return Insert(CallInst::Create(Callee, Arg));
  }
  CallInst *CreateCall2(Value *Callee, Value *Arg1, Value *Arg2) {
    Value *Args[] = { Arg1, Arg2 };
    return Insert(CallInst::Create(Callee, Args, Args+2));
  }
  CallInst *CreateCall3(Value *Callee, Value *Arg1, Value *Arg2, Value *Arg3) {
    Value *Args[] = { Arg1, Arg2, Arg3 };
    return Insert(CallInst::Create(Callee, Args, Args+3));
  }
  CallInst *CreateCall4(Value *Callee, Value *Arg1, Value *Arg2, Value *Arg3,
                        Value *Arg4) {
    Value *Args[] = { Arg1, Arg2, Arg3, Arg4 };
    return Insert(CallInst::Create(Callee, Args, Args+4));
  }
  CallInst *CreateCall5(Value *Callee, Value *Arg1, Value *Arg2, Value *Arg3,
                        Value *Arg4, Value *Arg5) {
    Value *Args[] = { Arg1, Arg2, Arg3, Arg4, Arg5 };
    return Insert(CallInst::Create(Callee, Args, Args+5));
  }

  template<typename InputIterator>
  CallInst *CreateCall(Value *Callee, InputIterator ArgBegin,
                       InputIterator ArgEnd) {
    return Insert(CallInst::Create(Callee, ArgBegin, ArgEnd));
  }

  Value *CreateSelect(Value *C, Value *True, Value *False) {
    if (Constant *CC = dyn_cast<Constant>(C))
      if (Constant *TC = dyn_cast<Constant>(True))
        if (Constant *FC = dyn_cast<Constant>(False))
          return Folder.CreateSelect(CC, TC, FC);
    return Insert(SelectInst::Create(C, True, False));
  }

  Value *CreateExtractElement(Value *Vec, Value *Idx) {
    if (Constant *VC = dyn_cast<Constant>(Vec))
      if (Constant *IC = dyn_cast<Constant>(Idx))
        return Folder.CreateExtractElement(VC, IC);
    return Insert(ExtractElementInst::Create(Vec, Idx));
  }

  Value *CreateInsertElement(Value *Vec, Value *NewElt, Value *Idx) {
    if (Constant *VC = dyn_cast<Constant>(Vec))
      if (Constant *NC = dyn_cast<Constant>(NewElt))
        if (Constant *IC = dyn_cast<Constant>(Idx))
          return Folder.CreateInsertElement(VC, NC, IC);
    return Insert(InsertElementInst::Create(Vec, NewElt, Idx));
  }

  Value *CreateShuffleVector(Value *V1, Value *V2, Value *Mask) {
    if (Constant *V1C = dyn_cast<Constant>(V1))
      if (Constant *V2C = dyn_cast<Constant>(V2))
        if (Constant *MC = dyn_cast<Constant>(Mask))
          return Folder.CreateShuffleVector(V1C, V2C, MC);
    return Insert(new ShuffleVectorInst(V1, V2, Mask));
  }

  Value *CreateExtractValue(Value *Agg, unsigned Idx) {
    if (Constant *AggC = dyn_cast<Constant>(Agg))
      return Folder.CreateExtractValue(AggC, &Idx, 1);
    return Insert(ExtractValueInst::Create(Agg, Idx));
  }

  template<typename InputIterator>
  Value *CreateExtractValue(Value *Agg,
                            InputIterator IdxBegin,
                            InputIterator IdxEnd) {
    if (Constant *AggC = dyn_cast<Constant>(Agg))
      return Folder.CreateExtractValue(AggC, IdxBegin, IdxEnd - IdxBegin);
    return Insert(ExtractValueInst::Create(Agg, IdxBegin, IdxEnd));
  }

  Value *CreateInsertValue(Value *Agg, Value *Val, unsigned Idx) {
    if (Constant *AggC = dyn_cast<Constant>(Agg))
      if (Constant *ValC = dyn_cast<Constant>(Val))
        return Folder.CreateInsertValue(AggC, ValC, &Idx, 1);
    return Insert(InsertValueInst::Create(Agg, Val, Idx));
  }

  template<typename InputIterator>
  Value *CreateInsertValue(Value *Agg, Value *Val,
                           InputIterator IdxBegin,
                           InputIterator IdxEnd) {
    if (Constant *AggC = dyn_cast<Constant>(Agg))
      if (Constant *ValC = dyn_cast<Constant>(Val))
        return Folder.CreateInsertValue(AggC, ValC, IdxBegin, IdxEnd-IdxBegin);
    return Insert(InsertValueInst::Create(Agg, Val, IdxBegin, IdxEnd));
  }

  //===--------------------------------------------------------------------===//
  // Utility creation methods
  //===--------------------------------------------------------------------===//

  /// CreateIsNull - Return an i1 value testing if \arg Arg is null.
  Value *CreateIsNull(Value *Arg) {
    return CreateICmpEQ(Arg, Constant::getNullValue(Arg->getType()));
  }

  /// CreateIsNotNull - Return an i1 value testing if \arg Arg is not null.
  Value *CreateIsNotNull(Value *Arg) {
    return CreateICmpNE(Arg, Constant::getNullValue(Arg->getType()));
  }

  /// CreatePtrDiff - Return the i64 difference between two pointer values,
  /// dividing out the size of the pointed-to objects.  This is intended to
  /// implement C-style pointer subtraction. As such, the pointers must be
  /// appropriately aligned for their element types and pointing into the
  /// same object.
  Value *CreatePtrDiff(Value *LHS, Value *RHS) {
    assert(LHS->getType() == RHS->getType() &&
           "Pointer subtraction operand types must match!");
    const PointerType *ArgType = cast<PointerType>(LHS->getType());
    Value *LHS_int = CreatePtrToInt(LHS, Type::getInt64Ty(Context));
    Value *RHS_int = CreatePtrToInt(RHS, Type::getInt64Ty(Context));
    Value *Difference = CreateSub(LHS_int, RHS_int);
    return CreateExactSDiv(Difference,
                           ConstantExpr::getSizeOf(ArgType->getElementType()));
  }
};

}

#endif
