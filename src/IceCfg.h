//===- subzero/src/IceCfg.h - Control flow graph ----------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the Cfg class, which represents the control flow graph
/// and the overall per-function compilation context.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECFG_H
#define SUBZERO_SRC_ICECFG_H

#include "IceAssembler.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceTypes.h"

namespace Ice {

class Cfg {
  Cfg() = delete;
  Cfg(const Cfg &) = delete;
  Cfg &operator=(const Cfg &) = delete;

public:
  ~Cfg();

  static std::unique_ptr<Cfg> create(GlobalContext *Ctx,
                                     uint32_t SequenceNumber) {
    return std::unique_ptr<Cfg>(new Cfg(Ctx, SequenceNumber));
  }
  /// Gets a pointer to the current thread's Cfg.
  static const Cfg *getCurrentCfg() { return ICE_TLS_GET_FIELD(CurrentCfg); }
  static void setCurrentCfg(const Cfg *Func) {
    ICE_TLS_SET_FIELD(CurrentCfg, Func);
  }
  /// Gets a pointer to the current thread's Cfg's allocator.
  static ArenaAllocator<> *getCurrentCfgAllocator() {
    assert(ICE_TLS_GET_FIELD(CurrentCfg));
    return ICE_TLS_GET_FIELD(CurrentCfg)->Allocator.get();
  }

  GlobalContext *getContext() const { return Ctx; }
  uint32_t getSequenceNumber() const { return SequenceNumber; }

  /// Returns true if any of the specified options in the verbose mask are set.
  /// If the argument is omitted, it checks if any verbose options at all are
  /// set.
  bool isVerbose(VerboseMask Mask = IceV_All) const { return VMask & Mask; }
  void setVerbose(VerboseMask Mask) { VMask = Mask; }

  /// \name Manage the name and return type of the function being translated.
  /// @{
  void setFunctionName(const IceString &Name) { FunctionName = Name; }
  IceString getFunctionName() const { return FunctionName; }
  void setReturnType(Type Ty) { ReturnType = Ty; }
  /// @}

  /// \name Manage the "internal" attribute of the function.
  /// @{
  void setInternal(bool Internal) { IsInternalLinkage = Internal; }
  bool getInternal() const { return IsInternalLinkage; }
  /// @}

  /// \name Manage errors.
  /// @{

  /// Translation error flagging. If support for some construct is known to be
  /// missing, instead of an assertion failure, setError() should be called and
  /// the error should be propagated back up. This way, we can gracefully fail
  /// to translate and let a fallback translator handle the function.
  void setError(const IceString &Message);
  bool hasError() const { return HasError; }
  IceString getError() const { return ErrorMessage; }
  /// @}

  /// \name Manage nodes (a.k.a. basic blocks, CfgNodes).
  /// @{
  void setEntryNode(CfgNode *EntryNode) { Entry = EntryNode; }
  CfgNode *getEntryNode() const { return Entry; }
  /// Create a node and append it to the end of the linearized list. The loop
  /// nest depth of the new node may not be valid if it is created after
  /// computeLoopNestDepth.
  CfgNode *makeNode();
  SizeT getNumNodes() const { return Nodes.size(); }
  const NodeList &getNodes() const { return Nodes; }
  /// Swap nodes of Cfg with given list of nodes.  The number of nodes must
  /// remain unchanged.
  void swapNodes(NodeList &NewNodes);
  /// @}

  using IdentifierIndexType = int32_t;
  /// Adds a name to the list and returns its index, suitable for the argument
  /// to getIdentifierName(). No checking for duplicates is done. This is
  /// generally used for node names and variable names to avoid embedding a
  /// std::string inside an arena-allocated object.
  IdentifierIndexType addIdentifierName(const IceString &Name) {
    IdentifierIndexType Index = IdentifierNames.size();
    IdentifierNames.push_back(Name);
    return Index;
  }
  const IceString &getIdentifierName(IdentifierIndexType Index) const {
    return IdentifierNames[Index];
  }
  enum { IdentifierIndexInvalid = -1 };

  /// \name Manage instruction numbering.
  /// @{
  InstNumberT newInstNumber() { return NextInstNumber++; }
  InstNumberT getNextInstNumber() const { return NextInstNumber; }
  /// @}

  /// \name Manage Variables.
  /// @{

  /// Create a new Variable with a particular type and an optional name. The
  /// Node argument is the node where the variable is defined.
  // TODO(jpp): untemplate this with separate methods: makeVariable,
  // makeSpillVariable, and makeStackVariable.
  template <typename T = Variable> T *makeVariable(Type Ty) {
    SizeT Index = Variables.size();
    T *Var = T::create(this, Ty, Index);
    Variables.push_back(Var);
    return Var;
  }
  SizeT getNumVariables() const { return Variables.size(); }
  const VarList &getVariables() const { return Variables; }
  /// @}

  /// \name Manage arguments to the function.
  /// @{
  void addArg(Variable *Arg);
  const VarList &getArgs() const { return Args; }
  VarList &getArgs() { return Args; }
  void addImplicitArg(Variable *Arg);
  const VarList &getImplicitArgs() const { return ImplicitArgs; }
  /// @}

  /// \name Manage the jump tables.
  /// @{
  void addJumpTable(InstJumpTable *JumpTable) {
    JumpTables.emplace_back(JumpTable);
  }
  /// @}

  /// \name Miscellaneous accessors.
  /// @{
  TargetLowering *getTarget() const { return Target.get(); }
  VariablesMetadata *getVMetadata() const { return VMetadata.get(); }
  Liveness *getLiveness() const { return Live.get(); }
  template <typename T = Assembler> T *getAssembler() const {
    return llvm::dyn_cast<T>(TargetAssembler.get());
  }
  Assembler *releaseAssembler() { return TargetAssembler.release(); }
  std::unique_ptr<VariableDeclarationList> getGlobalInits() {
    return std::move(GlobalInits);
  }
  bool hasComputedFrame() const;
  bool getFocusedTiming() const { return FocusedTiming; }
  void setFocusedTiming() { FocusedTiming = true; }
  uint32_t getConstantBlindingCookie() const { return ConstantBlindingCookie; }
  /// @}

  /// Returns true if Var is a global variable that is used by the profiling
  /// code.
  static bool isProfileGlobal(const VariableDeclaration &Var);

  /// Passes over the CFG.
  void translate();
  /// After the CFG is fully constructed, iterate over the nodes and compute the
  /// predecessor and successor edges, in the form of CfgNode::InEdges[] and
  /// CfgNode::OutEdges[].
  void computeInOutEdges();
  void renumberInstructions();
  void placePhiLoads();
  void placePhiStores();
  void deletePhis();
  void advancedPhiLowering();
  void reorderNodes();
  void shuffleNodes();
  void doAddressOpt();
  void doArgLowering();
  void doNopInsertion();
  void genCode();
  void genFrame();
  void computeLoopNestDepth();
  void livenessLightweight();
  void liveness(LivenessMode Mode);
  bool validateLiveness() const;
  void contractEmptyNodes();
  void doBranchOpt();
  void markNodesForSandboxing();

  /// \name  Manage the CurrentNode field.
  /// CurrentNode is used for validating the Variable::DefNode field during
  /// dumping/emitting.
  /// @{
  void setCurrentNode(const CfgNode *Node) { CurrentNode = Node; }
  void resetCurrentNode() { setCurrentNode(nullptr); }
  const CfgNode *getCurrentNode() const { return CurrentNode; }
  /// @}

  void emit();
  void emitIAS();
  static void emitTextHeader(const IceString &MangledName, GlobalContext *Ctx,
                             const Assembler *Asm);
  void dump(const IceString &Message = "");

  /// Allocate data of type T using the per-Cfg allocator.
  template <typename T> T *allocate() { return Allocator->Allocate<T>(); }

  /// Allocate an array of data of type T using the per-Cfg allocator.
  template <typename T> T *allocateArrayOf(size_t NumElems) {
    return Allocator->Allocate<T>(NumElems);
  }

  /// Deallocate data that was allocated via allocate<T>().
  template <typename T> void deallocate(T *Object) {
    Allocator->Deallocate(Object);
  }

  /// Deallocate data that was allocated via allocateArrayOf<T>().
  template <typename T> void deallocateArrayOf(T *Array) {
    Allocator->Deallocate(Array);
  }

private:
  Cfg(GlobalContext *Ctx, uint32_t SequenceNumber);

  /// Adds a call to the ProfileSummary runtime function as the first
  /// instruction in this CFG's entry block.
  void addCallToProfileSummary();

  /// Iterates over the basic blocks in this CFG, adding profiling code to each
  /// one of them. It returns a list with all the globals that the profiling
  /// code needs to be defined.
  void profileBlocks();

  /// Delete registered jump table placeholder instructions. This should only be
  /// called once all repointing has taken place.
  void deleteJumpTableInsts();
  /// Iterate through the registered jump tables and emit them.
  void emitJumpTables();

  GlobalContext *Ctx;
  uint32_t SequenceNumber;             /// output order for emission
  uint32_t ConstantBlindingCookie = 0; /// cookie for constant blinding
  VerboseMask VMask;
  IceString FunctionName = "";
  Type ReturnType = IceType_void;
  bool IsInternalLinkage = false;
  bool HasError = false;
  bool FocusedTiming = false;
  IceString ErrorMessage = "";
  CfgNode *Entry = nullptr; /// entry basic block
  NodeList Nodes;           /// linearized node list; Entry should be first
  std::vector<IceString> IdentifierNames;
  InstNumberT NextInstNumber;
  VarList Variables;
  VarList Args;         /// subset of Variables, in argument order
  VarList ImplicitArgs; /// subset of Variables
  std::unique_ptr<ArenaAllocator<>> Allocator;
  std::unique_ptr<Liveness> Live;
  std::unique_ptr<TargetLowering> Target;
  std::unique_ptr<VariablesMetadata> VMetadata;
  std::unique_ptr<Assembler> TargetAssembler;
  /// Globals required by this CFG. Mostly used for the profiler's globals.
  std::unique_ptr<VariableDeclarationList> GlobalInits;
  std::vector<InstJumpTable *> JumpTables;

  /// CurrentNode is maintained during dumping/emitting just for validating
  /// Variable::DefNode. Normally, a traversal over CfgNodes maintains this, but
  /// before global operations like register allocation, resetCurrentNode()
  /// should be called to avoid spurious validation failures.
  const CfgNode *CurrentNode = nullptr;

  /// Maintain a pointer in TLS to the current Cfg being translated. This is
  /// primarily for accessing its allocator statelessly, but other uses are
  /// possible.
  ICE_TLS_DECLARE_FIELD(const Cfg *, CurrentCfg);

public:
  static void TlsInit() { ICE_TLS_INIT_FIELD(CurrentCfg); }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECFG_H
