//===- subzero/src/IceGlobalContext.h - Global context defs -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares aspects of the compilation that persist across multiple
/// functions.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEGLOBALCONTEXT_H
#define SUBZERO_SRC_ICEGLOBALCONTEXT_H

#include "IceDefs.h"
#include "IceClFlags.h"
#include "IceIntrinsics.h"
#include "IceRNG.h"
#include "IceSwitchLowering.h"
#include "IceTargetLowering.def"
#include "IceThreading.h"
#include "IceTimerTree.h"
#include "IceTypes.h"
#include "IceUtils.h"

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Ice {

class ClFlags;
class ConstantPool;
class EmitterWorkItem;
class FuncSigType;

// Runtime helper function IDs

enum class RuntimeHelper {
#define X(Tag, Name) H_##Tag,
  RUNTIME_HELPER_FUNCTIONS_TABLE
#undef X
      H_Num
};

/// LockedPtr is a way to provide automatically locked access to some object.
template <typename T> class LockedPtr {
  LockedPtr() = delete;
  LockedPtr(const LockedPtr &) = delete;
  LockedPtr &operator=(const LockedPtr &) = delete;

public:
  LockedPtr(T *Value, GlobalLockType *Lock) : Value(Value), Lock(Lock) {
    Lock->lock();
  }
  LockedPtr(LockedPtr &&Other) : Value(Other.Value), Lock(Other.Lock) {
    Other.Value = nullptr;
    Other.Lock = nullptr;
  }
  ~LockedPtr() { Lock->unlock(); }
  T *operator->() const { return Value; }
  T &operator*() const { return *Value; }
  T *get() { return Value; }

private:
  T *Value;
  GlobalLockType *Lock;
};

class GlobalContext {
  GlobalContext() = delete;
  GlobalContext(const GlobalContext &) = delete;
  GlobalContext &operator=(const GlobalContext &) = delete;

  /// CodeStats collects rudimentary statistics during translation.
  class CodeStats {
    CodeStats(const CodeStats &) = delete;
    CodeStats &operator=(const CodeStats &) = default;
#define CODESTATS_TABLE                                                        \
  /* dump string, enum value */                                                \
  X("Inst Count  ", InstCount)                                                 \
  X("Regs Saved  ", RegsSaved)                                                 \
  X("Frame Bytes ", FrameByte)                                                 \
  X("Spills      ", NumSpills)                                                 \
  X("Fills       ", NumFills)                                                  \
  X("R/P Imms    ", NumRPImms)
    //#define X(str, tag)

  public:
    enum CSTag {
#define X(str, tag) CS_##tag,
      CODESTATS_TABLE
#undef X
          CS_NUM
    };
    CodeStats() { reset(); }
    void reset() { Stats.fill(0); }
    void update(CSTag Tag, uint32_t Count = 1) {
      assert(Tag < Stats.size());
      Stats[Tag] += Count;
    }
    void add(const CodeStats &Other) {
      for (uint32_t i = 0; i < Stats.size(); ++i)
        Stats[i] += Other.Stats[i];
    }
    void dump(const IceString &Name, GlobalContext *Ctx);

  private:
    std::array<uint32_t, CS_NUM> Stats;
  };

  /// TimerList is a vector of TimerStack objects, with extra methods
  /// to initialize and merge these vectors.
  class TimerList : public std::vector<TimerStack> {
    TimerList(const TimerList &) = delete;
    TimerList &operator=(const TimerList &) = delete;

  public:
    TimerList() = default;
    /// initInto() initializes a target list of timers based on the
    /// current list.  In particular, it creates the same number of
    /// timers, in the same order, with the same names, but initially
    /// empty of timing data.
    void initInto(TimerList &Dest) const {
      if (!BuildDefs::timers())
        return;
      Dest.clear();
      for (const TimerStack &Stack : *this) {
        Dest.push_back(TimerStack(Stack.getName()));
      }
    }
    void mergeFrom(TimerList &Src) {
      if (!BuildDefs::timers())
        return;
      assert(size() == Src.size());
      size_type i = 0;
      for (TimerStack &Stack : *this) {
        assert(Stack.getName() == Src[i].getName());
        Stack.mergeFrom(Src[i]);
        ++i;
      }
    }
  };

  /// ThreadContext contains thread-local data.  This data can be
  /// combined/reduced as needed after all threads complete.
  class ThreadContext {
    ThreadContext(const ThreadContext &) = delete;
    ThreadContext &operator=(const ThreadContext &) = delete;

  public:
    ThreadContext() = default;
    CodeStats StatsFunction;
    CodeStats StatsCumulative;
    TimerList Timers;
  };

public:
  /// The dump stream is a log stream while emit is the stream code
  /// is emitted to. The error stream is strictly for logging errors.
  GlobalContext(Ostream *OsDump, Ostream *OsEmit, Ostream *OsError,
                ELFStreamer *ELFStreamer);
  ~GlobalContext();

  ///
  /// The dump, error, and emit streams need to be used by only one
  /// thread at a time.  This is done by exclusively reserving the
  /// streams via lockStr() and unlockStr().  The OstreamLocker class
  /// can be used to conveniently manage this.
  ///
  /// The model is that a thread grabs the stream lock, then does an
  /// arbitrary amount of work during which far-away callees may grab
  /// the stream and do something with it, and finally the thread
  /// releases the stream lock.  This allows large chunks of output to
  /// be dumped or emitted without risking interleaving from multiple
  /// threads.
  void lockStr() { StrLock.lock(); }
  void unlockStr() { StrLock.unlock(); }
  Ostream &getStrDump() { return *StrDump; }
  Ostream &getStrError() { return *StrError; }
  Ostream &getStrEmit() { return *StrEmit; }
  void setStrEmit(Ostream &NewStrEmit) { StrEmit = &NewStrEmit; }

  LockedPtr<ErrorCode> getErrorStatus() {
    return LockedPtr<ErrorCode>(&ErrorStatus, &ErrorStatusLock);
  }

  /// \name Manage Constants.
  /// @{
  // getConstant*() functions are not const because they might add something to
  // the constant pool.
  Constant *getConstantInt(Type Ty, int64_t Value);
  Constant *getConstantInt1(int8_t ConstantInt1) {
    ConstantInt1 &= INT8_C(1);
    switch (ConstantInt1) {
    case 0:
      return getConstantZero(IceType_i1);
    case 1:
      return ConstantTrue;
    default:
      assert(false && "getConstantInt1 not on true/false");
      return getConstantInt1Internal(ConstantInt1);
    }
  }
  Constant *getConstantInt8(int8_t ConstantInt8) {
    switch (ConstantInt8) {
    case 0:
      return getConstantZero(IceType_i8);
    default:
      return getConstantInt8Internal(ConstantInt8);
    }
  }
  Constant *getConstantInt16(int16_t ConstantInt16) {
    switch (ConstantInt16) {
    case 0:
      return getConstantZero(IceType_i16);
    default:
      return getConstantInt16Internal(ConstantInt16);
    }
  }
  Constant *getConstantInt32(int32_t ConstantInt32) {
    switch (ConstantInt32) {
    case 0:
      return getConstantZero(IceType_i32);
    default:
      return getConstantInt32Internal(ConstantInt32);
    }
  }
  Constant *getConstantInt64(int64_t ConstantInt64) {
    switch (ConstantInt64) {
    case 0:
      return getConstantZero(IceType_i64);
    default:
      return getConstantInt64Internal(ConstantInt64);
    }
  }
  Constant *getConstantFloat(float Value);
  Constant *getConstantDouble(double Value);
  /// Returns a symbolic constant.
  Constant *getConstantSym(const RelocOffsetT Offset,
                           const RelocOffsetArray &OffsetExpr,
                           const IceString &Name, const IceString &EmitString);
  Constant *getConstantSym(RelocOffsetT Offset, const IceString &Name);
  Constant *getConstantExternSym(const IceString &Name);
  /// Returns an undef.
  Constant *getConstantUndef(Type Ty);
  /// Returns a zero value.
  Constant *getConstantZero(Type Ty);
  /// getConstantPool() returns a copy of the constant pool for constants of a
  /// given type.
  ConstantList getConstantPool(Type Ty);
  /// Returns a copy of the list of external symbols.
  ConstantList getConstantExternSyms();
  /// @}
  Constant *getRuntimeHelperFunc(RuntimeHelper FuncID) const {
    assert(FuncID < RuntimeHelper::H_Num);
    Constant *Result = RuntimeHelperFunc[static_cast<size_t>(FuncID)];
    assert(Result != nullptr && "No such runtime helper function");
    return Result;
  }

  /// Return a locked pointer to the registered jump tables.
  JumpTableDataList getJumpTables();
  /// Create a new jump table entry and return a reference to it.
  JumpTableData &addJumpTable(const IceString &FuncName, SizeT Id,
                              const JumpTableData::TargetList &TargetList);

  static const ClFlags &getFlags() { return Flags; }

  /// Allocate data of type T using the global allocator. We allow entities
  /// allocated from this global allocator to be either trivially or
  /// non-trivially destructible. We optimize the case when T is trivially
  /// destructible by not registering a destructor. Destructors will be invoked
  /// during GlobalContext destruction in the reverse object creation order.
  template <typename T>
  typename std::enable_if<std::is_trivially_destructible<T>::value, T>::type *
  allocate() {
    return getAllocator()->Allocate<T>();
  }

  template <typename T>
  typename std::enable_if<!std::is_trivially_destructible<T>::value, T>::type *
  allocate() {
    T *Ret = getAllocator()->Allocate<T>();
    getDestructors()->emplace_back([Ret]() { Ret->~T(); });
    return Ret;
  }

  const Intrinsics &getIntrinsicsInfo() const { return IntrinsicsInfo; }

  ELFObjectWriter *getObjectWriter() const { return ObjectWriter.get(); }

  /// Reset stats at the beginning of a function.
  void resetStats() {
    if (BuildDefs::dump())
      ICE_TLS_GET_FIELD(TLS)->StatsFunction.reset();
  }
  void dumpStats(const IceString &Name, bool Final = false);
  void statsUpdateEmitted(uint32_t InstCount) {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_InstCount, InstCount);
    Tls->StatsCumulative.update(CodeStats::CS_InstCount, InstCount);
  }
  void statsUpdateRegistersSaved(uint32_t Num) {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_RegsSaved, Num);
    Tls->StatsCumulative.update(CodeStats::CS_RegsSaved, Num);
  }
  void statsUpdateFrameBytes(uint32_t Bytes) {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_FrameByte, Bytes);
    Tls->StatsCumulative.update(CodeStats::CS_FrameByte, Bytes);
  }
  void statsUpdateSpills() {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_NumSpills);
    Tls->StatsCumulative.update(CodeStats::CS_NumSpills);
  }
  void statsUpdateFills() {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_NumFills);
    Tls->StatsCumulative.update(CodeStats::CS_NumFills);
  }

  /// Number of Randomized or Pooled Immediates
  void statsUpdateRPImms() {
    if (!getFlags().getDumpStats())
      return;
    ThreadContext *Tls = ICE_TLS_GET_FIELD(TLS);
    Tls->StatsFunction.update(CodeStats::CS_NumRPImms);
    Tls->StatsCumulative.update(CodeStats::CS_NumRPImms);
  }

  /// These are predefined TimerStackIdT values.
  enum TimerStackKind { TSK_Default = 0, TSK_Funcs, TSK_Num };

  /// newTimerStackID() creates a new TimerStack in the global space. It does
  /// not affect any TimerStack objects in TLS.
  TimerStackIdT newTimerStackID(const IceString &Name);
  /// dumpTimers() dumps the global timer data. As such, one probably wants to
  /// call mergeTimerStacks() as a prerequisite.
  void dumpTimers(TimerStackIdT StackID = TSK_Default,
                  bool DumpCumulative = true);
  /// The following methods affect only the calling thread's TLS timer data.
  TimerIdT getTimerID(TimerStackIdT StackID, const IceString &Name);
  void pushTimer(TimerIdT ID, TimerStackIdT StackID);
  void popTimer(TimerIdT ID, TimerStackIdT StackID);
  void resetTimer(TimerStackIdT StackID);
  void setTimerName(TimerStackIdT StackID, const IceString &NewName);

  /// This is the first work item sequence number that the parser produces, and
  /// correspondingly the first sequence number that the emitter thread will
  /// wait for. Start numbering at 1 to leave room for a sentinel, in case e.g.
  /// we wish to inject items with a special sequence number that may be
  /// executed out of order.
  static uint32_t getFirstSequenceNumber() { return 1; }
  /// Adds a newly parsed and constructed function to the Cfg work queue.
  /// Notifies any idle workers that a new function is available for
  /// translating. May block if the work queue is too large, in order to control
  /// memory footprint.
  void optQueueBlockingPush(std::unique_ptr<Cfg> Func);
  /// Takes a Cfg from the work queue for translating. May block if the work
  /// queue is currently empty. Returns nullptr if there is no more work - the
  /// queue is empty and either end() has been called or the Sequential flag was
  /// set.
  std::unique_ptr<Cfg> optQueueBlockingPop();
  /// Notifies that no more work will be added to the work queue.
  void optQueueNotifyEnd() { OptQ.notifyEnd(); }

  /// Emit file header for output file.
  void emitFileHeader();

  void lowerConstants();

  void lowerJumpTables();

  void emitQueueBlockingPush(std::unique_ptr<EmitterWorkItem> Item);
  std::unique_ptr<EmitterWorkItem> emitQueueBlockingPop();
  void emitQueueNotifyEnd() { EmitQ.notifyEnd(); }

  void initParserThread() {
    ThreadContext *Tls = new ThreadContext();
    auto Timers = getTimers();
    Timers->initInto(Tls->Timers);
    AllThreadContexts.push_back(Tls);
    ICE_TLS_SET_FIELD(TLS, Tls);
  }

  void startWorkerThreads() {
    size_t NumWorkers = getFlags().getNumTranslationThreads();
    auto Timers = getTimers();
    for (size_t i = 0; i < NumWorkers; ++i) {
      ThreadContext *WorkerTLS = new ThreadContext();
      Timers->initInto(WorkerTLS->Timers);
      AllThreadContexts.push_back(WorkerTLS);
      TranslationThreads.push_back(std::thread(
          &GlobalContext::translateFunctionsWrapper, this, WorkerTLS));
    }
    if (NumWorkers) {
      ThreadContext *WorkerTLS = new ThreadContext();
      Timers->initInto(WorkerTLS->Timers);
      AllThreadContexts.push_back(WorkerTLS);
      EmitterThreads.push_back(
          std::thread(&GlobalContext::emitterWrapper, this, WorkerTLS));
    }
  }

  void waitForWorkerThreads() {
    optQueueNotifyEnd();
    for (std::thread &Worker : TranslationThreads) {
      Worker.join();
    }
    TranslationThreads.clear();

    // Only notify the emit queue to end after all the translation threads have
    // ended.
    emitQueueNotifyEnd();
    for (std::thread &Worker : EmitterThreads) {
      Worker.join();
    }
    EmitterThreads.clear();

    if (BuildDefs::timers()) {
      auto Timers = getTimers();
      for (ThreadContext *TLS : AllThreadContexts)
        Timers->mergeFrom(TLS->Timers);
    }
    if (BuildDefs::dump()) {
      // Do a separate loop over AllThreadContexts to avoid holding two locks
      // at once.
      auto Stats = getStatsCumulative();
      for (ThreadContext *TLS : AllThreadContexts)
        Stats->add(TLS->StatsCumulative);
    }
  }

  /// Translation thread startup routine.
  void translateFunctionsWrapper(ThreadContext *MyTLS) {
    ICE_TLS_SET_FIELD(TLS, MyTLS);
    translateFunctions();
  }
  /// Translate functions from the Cfg queue until the queue is empty.
  void translateFunctions();

  /// Emitter thread startup routine.
  void emitterWrapper(ThreadContext *MyTLS) {
    ICE_TLS_SET_FIELD(TLS, MyTLS);
    emitItems();
  }
  /// Emit functions and global initializers from the emitter queue until the
  /// queue is empty.
  void emitItems();

  /// Uses DataLowering to lower Globals. Side effects:
  ///  - discards the initializer list for the global variable in Globals.
  ///  - clears the Globals array.
  void lowerGlobals(const IceString &SectionSuffix);

  /// Lowers the profile information.
  void lowerProfileData();

  void dumpConstantLookupCounts();

  /// Utility function to match a symbol name against a match string. This is
  /// used in a few cases where we want to take some action on a particular
  /// function or symbol based on a command-line argument, such as changing the
  /// verbose level for a particular function. An empty Match argument means
  /// match everything. Returns true if there is a match.
  static bool matchSymbolName(const IceString &SymbolName,
                              const IceString &Match) {
    return Match.empty() || Match == SymbolName;
  }

  static ClFlags Flags;
  static ClFlagsExtra ExtraFlags;

  /// DisposeGlobalVariablesAfterLowering controls whether the memory used by
  /// GlobaleVariables can be reclaimed right after they have been lowered.
  /// @{
  bool getDisposeGlobalVariablesAfterLowering() const {
    return DisposeGlobalVariablesAfterLowering;
  }

  void setDisposeGlobalVariablesAfterLowering(bool Value) {
    DisposeGlobalVariablesAfterLowering = Value;
  }
  /// @}

private:
  // Try to ensure mutexes are allocated on separate cache lines.

  // Destructors collaborate with Allocator
  ICE_CACHELINE_BOUNDARY;
  // Managed by getAllocator()
  GlobalLockType AllocLock;
  ArenaAllocator Allocator;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getInitializerAllocator()
  GlobalLockType InitAllocLock;
  VariableDeclarationList Globals;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getDestructors()
  using DestructorArray = std::vector<std::function<void()>>;
  GlobalLockType DestructorsLock;
  DestructorArray Destructors;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getConstantPool()
  GlobalLockType ConstPoolLock;
  std::unique_ptr<ConstantPool> ConstPool;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getJumpTableList()
  GlobalLockType JumpTablesLock;
  JumpTableDataList JumpTableList;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getErrorStatus()
  GlobalLockType ErrorStatusLock;
  ErrorCode ErrorStatus;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getStatsCumulative()
  GlobalLockType StatsLock;
  CodeStats StatsCumulative;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getTimers()
  GlobalLockType TimerLock;
  TimerList Timers;

  ICE_CACHELINE_BOUNDARY;
  /// StrLock is a global lock on the dump and emit output streams.
  using StrLockType = std::mutex;
  StrLockType StrLock;
  Ostream *StrDump;  /// Stream for dumping / diagnostics
  Ostream *StrEmit;  /// Stream for code emission
  Ostream *StrError; /// Stream for logging errors.

  ICE_CACHELINE_BOUNDARY;

  Intrinsics IntrinsicsInfo;
  // TODO(jpp): move to EmitterContext.
  std::unique_ptr<ELFObjectWriter> ObjectWriter;
  BoundedProducerConsumerQueue<Cfg> OptQ;
  BoundedProducerConsumerQueue<EmitterWorkItem> EmitQ;
  // DataLowering is only ever used by a single thread at a time (either in
  // emitItems(), or in IceCompiler::run before the compilation is over.)
  // TODO(jpp): move to EmitterContext.
  std::unique_ptr<TargetDataLowering> DataLowering;
  /// If !HasEmittedCode, SubZero will accumulate all Globals (which are "true"
  /// program global variables) until the first code WorkItem is seen.
  // TODO(jpp): move to EmitterContext.
  bool HasSeenCode = false;
  // TODO(jpp): move to EmitterContext.
  VariableDeclaration *ProfileBlockInfoVarDecl = nullptr;
  std::vector<VariableDeclaration *> ProfileBlockInfos;
  /// Indicates if global variable declarations can be disposed of right after
  /// lowering.
  bool DisposeGlobalVariablesAfterLowering = true;
  Constant *ConstZeroForType[IceType_NUM];
  Constant *ConstantTrue;
  // Holds the constants representing each runtime helper function.
  Constant *RuntimeHelperFunc[static_cast<size_t>(RuntimeHelper::H_Num)];

  Constant *getConstantZeroInternal(Type Ty);
  Constant *getConstantIntInternal(Type Ty, int64_t Value);
  Constant *getConstantInt1Internal(int8_t ConstantInt1);
  Constant *getConstantInt8Internal(int8_t ConstantInt8);
  Constant *getConstantInt16Internal(int16_t ConstantInt16);
  Constant *getConstantInt32Internal(int32_t ConstantInt32);
  Constant *getConstantInt64Internal(int64_t ConstantInt64);
  LockedPtr<ArenaAllocator> getAllocator() {
    return LockedPtr<ArenaAllocator>(&Allocator, &AllocLock);
  }
  LockedPtr<VariableDeclarationList> getInitializerAllocator() {
    return LockedPtr<VariableDeclarationList>(&Globals, &InitAllocLock);
  }
  LockedPtr<ConstantPool> getConstPool() {
    return LockedPtr<ConstantPool>(ConstPool.get(), &ConstPoolLock);
  }
  LockedPtr<JumpTableDataList> getJumpTableList() {
    return LockedPtr<JumpTableDataList>(&JumpTableList, &JumpTablesLock);
  }
  LockedPtr<CodeStats> getStatsCumulative() {
    return LockedPtr<CodeStats>(&StatsCumulative, &StatsLock);
  }
  LockedPtr<TimerList> getTimers() {
    return LockedPtr<TimerList>(&Timers, &TimerLock);
  }
  LockedPtr<DestructorArray> getDestructors() {
    return LockedPtr<DestructorArray>(&Destructors, &DestructorsLock);
  }

  void accumulateGlobals(std::unique_ptr<VariableDeclarationList> Globls) {
    LockedPtr<VariableDeclarationList> _(&Globals, &InitAllocLock);
    if (Globls != nullptr) {
      Globals.merge(Globls.get());
    }
  }

  void lowerGlobalsIfNoCodeHasBeenSeen() {
    if (HasSeenCode)
      return;
    constexpr char NoSuffix[] = "";
    lowerGlobals(NoSuffix);
    HasSeenCode = true;
  }

  void saveBlockInfoPtrs();

  llvm::SmallVector<ThreadContext *, 128> AllThreadContexts;
  llvm::SmallVector<std::thread, 128> TranslationThreads;
  llvm::SmallVector<std::thread, 128> EmitterThreads;
  // Each thread has its own TLS pointer which is also held in
  // AllThreadContexts.
  ICE_TLS_DECLARE_FIELD(ThreadContext *, TLS);

public:
  static void TlsInit() { ICE_TLS_INIT_FIELD(TLS); }
};

/// Helper class to push and pop a timer marker. The constructor pushes a
/// marker, and the destructor pops it. This is for convenient timing of regions
/// of code.
class TimerMarker {
  TimerMarker() = delete;
  TimerMarker(const TimerMarker &) = delete;
  TimerMarker &operator=(const TimerMarker &) = delete;

public:
  TimerMarker(TimerIdT ID, GlobalContext *Ctx,
              TimerStackIdT StackID = GlobalContext::TSK_Default)
      : ID(ID), Ctx(Ctx), StackID(StackID) {
    if (BuildDefs::timers())
      push();
  }
  TimerMarker(TimerIdT ID, const Cfg *Func,
              TimerStackIdT StackID = GlobalContext::TSK_Default)
      : ID(ID), Ctx(nullptr), StackID(StackID) {
    // Ctx gets set at the beginning of pushCfg().
    if (BuildDefs::timers())
      pushCfg(Func);
  }
  TimerMarker(GlobalContext *Ctx, const IceString &FuncName)
      : ID(getTimerIdFromFuncName(Ctx, FuncName)), Ctx(Ctx),
        StackID(GlobalContext::TSK_Funcs) {
    if (BuildDefs::timers())
      push();
  }

  ~TimerMarker() {
    if (BuildDefs::timers() && Active)
      Ctx->popTimer(ID, StackID);
  }

private:
  void push();
  void pushCfg(const Cfg *Func);
  static TimerIdT getTimerIdFromFuncName(GlobalContext *Ctx,
                                         const IceString &FuncName);
  const TimerIdT ID;
  GlobalContext *Ctx;
  const TimerStackIdT StackID;
  bool Active = false;
};

/// Helper class for locking the streams and then automatically unlocking them.
class OstreamLocker {
private:
  OstreamLocker() = delete;
  OstreamLocker(const OstreamLocker &) = delete;
  OstreamLocker &operator=(const OstreamLocker &) = delete;

public:
  explicit OstreamLocker(GlobalContext *Ctx) : Ctx(Ctx) { Ctx->lockStr(); }
  ~OstreamLocker() { Ctx->unlockStr(); }

private:
  GlobalContext *const Ctx;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEGLOBALCONTEXT_H
