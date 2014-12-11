//===- subzero/src/IceCfg.h - Control flow graph ----------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Cfg class, which represents the control flow
// graph and the overall per-function compilation context.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECFG_H
#define SUBZERO_SRC_ICECFG_H

#include <memory>

#include "llvm/Support/Allocator.h"

#include "assembler.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceTypes.h"

namespace Ice {

class Cfg {
  Cfg(const Cfg &) = delete;
  Cfg &operator=(const Cfg &) = delete;

public:
  Cfg(GlobalContext *Ctx);
  ~Cfg();

  GlobalContext *getContext() const { return Ctx; }

  // Manage the name and return type of the function being translated.
  void setFunctionName(const IceString &Name) { FunctionName = Name; }
  IceString getFunctionName() const { return FunctionName; }
  void setReturnType(Type Ty) { ReturnType = Ty; }

  // Manage the "internal" attribute of the function.
  void setInternal(bool Internal) { IsInternalLinkage = Internal; }
  bool getInternal() const { return IsInternalLinkage; }

  // Translation error flagging.  If support for some construct is
  // known to be missing, instead of an assertion failure, setError()
  // should be called and the error should be propagated back up.
  // This way, we can gracefully fail to translate and let a fallback
  // translator handle the function.
  void setError(const IceString &Message);
  bool hasError() const { return HasError; }
  IceString getError() const { return ErrorMessage; }

  // Manage nodes (a.k.a. basic blocks, CfgNodes).
  void setEntryNode(CfgNode *EntryNode) { Entry = EntryNode; }
  CfgNode *getEntryNode() const { return Entry; }
  // Create a node and append it to the end of the linearized list.
  CfgNode *makeNode();
  SizeT getNumNodes() const { return Nodes.size(); }
  const NodeList &getNodes() const { return Nodes; }

  typedef int32_t IdentifierIndexType;
  // Adds a name to the list and returns its index, suitable for the
  // argument to getIdentifierName().  No checking for duplicates is
  // done.  This is generally used for node names and variable names
  // to avoid embedding a std::string inside an arena-allocated
  // object.
  IdentifierIndexType addIdentifierName(const IceString &Name) {
    IdentifierIndexType Index = IdentifierNames.size();
    IdentifierNames.push_back(Name);
    return Index;
  }
  const IceString &getIdentifierName(IdentifierIndexType Index) const {
    return IdentifierNames[Index];
  }
  enum {
    IdentifierIndexInvalid = -1
  };

  // Manage instruction numbering.
  InstNumberT newInstNumber() { return NextInstNumber++; }
  InstNumberT getNextInstNumber() const { return NextInstNumber; }

  // Manage Variables.
  // Create a new Variable with a particular type and an optional
  // name.  The Node argument is the node where the variable is defined.
  template <typename T = Variable> T *makeVariable(Type Ty) {
    SizeT Index = Variables.size();
    T *Var = T::create(this, Ty, Index);
    Variables.push_back(Var);
    return Var;
  }
  SizeT getNumVariables() const { return Variables.size(); }
  const VarList &getVariables() const { return Variables; }

  // Manage arguments to the function.
  void addArg(Variable *Arg);
  const VarList &getArgs() const { return Args; }
  VarList &getArgs() { return Args; }
  void addImplicitArg(Variable *Arg);
  const VarList &getImplicitArgs() const { return ImplicitArgs; }

  // Miscellaneous accessors.
  TargetLowering *getTarget() const { return Target.get(); }
  VariablesMetadata *getVMetadata() const { return VMetadata.get(); }
  Liveness *getLiveness() const { return Live.get(); }
  template <typename T> T *getAssembler() const {
    return static_cast<T *>(TargetAssembler.get());
  }
  bool hasComputedFrame() const;
  bool getFocusedTiming() const { return FocusedTiming; }
  void setFocusedTiming() { FocusedTiming = true; }

  // Passes over the CFG.
  void translate();
  // After the CFG is fully constructed, iterate over the nodes and
  // compute the predecessor edges, in the form of
  // CfgNode::InEdges[].
  void computePredecessors();
  void renumberInstructions();
  void placePhiLoads();
  void placePhiStores();
  void deletePhis();
  void advancedPhiLowering();
  void reorderNodes();
  void doAddressOpt();
  void doArgLowering();
  void doNopInsertion();
  void genCode();
  void genFrame();
  void livenessLightweight();
  void liveness(LivenessMode Mode);
  bool validateLiveness() const;
  void contractEmptyNodes();
  void doBranchOpt();

  // Manage the CurrentNode field, which is used for validating the
  // Variable::DefNode field during dumping/emitting.
  void setCurrentNode(const CfgNode *Node) { CurrentNode = Node; }
  void resetCurrentNode() { setCurrentNode(NULL); }
  const CfgNode *getCurrentNode() const { return CurrentNode; }

  void emit();
  void emitIAS();
  void emitTextHeader(const IceString &MangledName);
  void dump(const IceString &Message = "");

  // Allocate data of type T using the per-Cfg allocator.
  template <typename T> T *allocate() { return Allocator.Allocate<T>(); }

  // Allocate an instruction of type T using the per-Cfg instruction allocator.
  template <typename T> T *allocateInst() { return Allocator.Allocate<T>(); }

  // Allocate an array of data of type T using the per-Cfg allocator.
  template <typename T> T *allocateArrayOf(size_t NumElems) {
    return Allocator.Allocate<T>(NumElems);
  }

  // Deallocate data that was allocated via allocate<T>().
  template <typename T> void deallocate(T *Object) {
    Allocator.Deallocate(Object);
  }

  // Deallocate data that was allocated via allocateInst<T>().
  template <typename T> void deallocateInst(T *Instr) {
    Allocator.Deallocate(Instr);
  }

  // Deallocate data that was allocated via allocateArrayOf<T>().
  template <typename T> void deallocateArrayOf(T *Array) {
    Allocator.Deallocate(Array);
  }

private:
  // TODO: for now, everything is allocated from the same allocator. In the
  // future we may want to split this to several allocators, for example in
  // order to use a "Recycler" to preserve memory. If we keep all allocation
  // requests from the Cfg exposed via methods, we can always switch the
  // implementation over at a later point.
  llvm::BumpPtrAllocatorImpl<llvm::MallocAllocator, 1024 * 1024> Allocator;

  GlobalContext *Ctx;
  IceString FunctionName;
  Type ReturnType;
  bool IsInternalLinkage;
  bool HasError;
  bool FocusedTiming;
  IceString ErrorMessage;
  CfgNode *Entry; // entry basic block
  NodeList Nodes; // linearized node list; Entry should be first
  std::vector<IceString> IdentifierNames;
  InstNumberT NextInstNumber;
  VarList Variables;
  VarList Args; // subset of Variables, in argument order
  VarList ImplicitArgs; // subset of Variables
  std::unique_ptr<Liveness> Live;
  std::unique_ptr<TargetLowering> Target;
  std::unique_ptr<VariablesMetadata> VMetadata;
  std::unique_ptr<Assembler> TargetAssembler;

  // CurrentNode is maintained during dumping/emitting just for
  // validating Variable::DefNode.  Normally, a traversal over
  // CfgNodes maintains this, but before global operations like
  // register allocation, resetCurrentNode() should be called to avoid
  // spurious validation failures.
  const CfgNode *CurrentNode;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECFG_H
