//===- subzero/src/IceGlobalContext.h - Global context defs -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares aspects of the compilation that persist across
// multiple functions.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEGLOBALCONTEXT_H
#define SUBZERO_SRC_ICEGLOBALCONTEXT_H

#include <mutex>
#include <thread>

#include "IceDefs.h"
#include "IceClFlags.h"
#include "IceIntrinsics.h"
#include "IceRNG.h"
#include "IceTimerTree.h"
#include "IceTypes.h"
#include "IceUtils.h"

namespace Ice {

class ClFlags;
class ConstantPool;
class FuncSigType;

// LockedPtr is a way to provide automatically locked access to some object.
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

private:
  T *Value;
  GlobalLockType *Lock;
};

class GlobalContext {
  GlobalContext(const GlobalContext &) = delete;
  GlobalContext &operator=(const GlobalContext &) = delete;

  // CodeStats collects rudimentary statistics during translation.
  class CodeStats {
    CodeStats(const CodeStats &) = delete;
    CodeStats &operator=(const CodeStats &) = default;

  public:
    CodeStats()
        : InstructionsEmitted(0), RegistersSaved(0), FrameBytes(0), Spills(0),
          Fills(0) {}
    void reset() { *this = CodeStats(); }
    void updateEmitted(uint32_t InstCount) { InstructionsEmitted += InstCount; }
    void updateRegistersSaved(uint32_t Num) { RegistersSaved += Num; }
    void updateFrameBytes(uint32_t Bytes) { FrameBytes += Bytes; }
    void updateSpills() { ++Spills; }
    void updateFills() { ++Fills; }
    void dump(const IceString &Name, Ostream &Str);

  private:
    uint32_t InstructionsEmitted;
    uint32_t RegistersSaved;
    uint32_t FrameBytes;
    uint32_t Spills;
    uint32_t Fills;
  };

  // ThreadContext contains thread-local data.  This data can be
  // combined/reduced as needed after all threads complete.
  class ThreadContext {
    ThreadContext(const ThreadContext &) = delete;
    ThreadContext &operator=(const ThreadContext &) = delete;

  public:
    ThreadContext() {}
    CodeStats StatsFunction;
    std::vector<TimerStack> Timers;
  };

public:
  GlobalContext(Ostream *OsDump, Ostream *OsEmit, ELFStreamer *ELFStreamer,
                VerboseMask Mask, TargetArch Arch, OptLevel Opt,
                IceString TestPrefix, const ClFlags &Flags);
  ~GlobalContext();

  VerboseMask getVerbose() const { return VMask; }

  // The dump and emit streams need to be used by only one thread at a
  // time.  This is done by exclusively reserving the streams via
  // lockStr() and unlockStr().  The OstreamLocker class can be used
  // to conveniently manage this.
  //
  // The model is that a thread grabs the stream lock, then does an
  // arbitrary amount of work during which far-away callees may grab
  // the stream and do something with it, and finally the thread
  // releases the stream lock.  This allows large chunks of output to
  // be dumped or emitted without risking interleaving from multiple
  // threads.
  void lockStr() { StrLock.lock(); }
  void unlockStr() { StrLock.unlock(); }
  Ostream &getStrDump() { return *StrDump; }
  Ostream &getStrEmit() { return *StrEmit; }

  TargetArch getTargetArch() const { return Arch; }
  OptLevel getOptLevel() const { return Opt; }
  LockedPtr<ErrorCode> getErrorStatus() {
    return LockedPtr<ErrorCode>(&ErrorStatus, &ErrorStatusLock);
  }

  // When emitting assembly, we allow a string to be prepended to
  // names of translated functions.  This makes it easier to create an
  // execution test against a reference translator like llc, with both
  // translators using the same bitcode as input.
  IceString getTestPrefix() const { return TestPrefix; }
  IceString mangleName(const IceString &Name) const;

  // Manage Constants.
  // getConstant*() functions are not const because they might add
  // something to the constant pool.
  Constant *getConstantInt(Type Ty, int64_t Value);
  Constant *getConstantInt1(int8_t ConstantInt1);
  Constant *getConstantInt8(int8_t ConstantInt8);
  Constant *getConstantInt16(int16_t ConstantInt16);
  Constant *getConstantInt32(int32_t ConstantInt32);
  Constant *getConstantInt64(int64_t ConstantInt64);
  Constant *getConstantFloat(float Value);
  Constant *getConstantDouble(double Value);
  // Returns a symbolic constant.
  Constant *getConstantSym(RelocOffsetT Offset, const IceString &Name,
                           bool SuppressMangling);
  // Returns an undef.
  Constant *getConstantUndef(Type Ty);
  // Returns a zero value.
  Constant *getConstantZero(Type Ty);
  // getConstantPool() returns a copy of the constant pool for
  // constants of a given type.
  ConstantList getConstantPool(Type Ty);

  const ClFlags &getFlags() const { return Flags; }

  bool isIRGenerationDisabled() const {
    return ALLOW_DISABLE_IR_GEN ? getFlags().DisableIRGeneration : false;
  }

  // Allocate data of type T using the global allocator.
  template <typename T> T *allocate() { return getAllocator()->Allocate<T>(); }

  const Intrinsics &getIntrinsicsInfo() const { return IntrinsicsInfo; }

  // TODO(wala,stichnot): Make the RNG play nicely with multithreaded
  // translation.
  RandomNumberGenerator &getRNG() { return RNG; }

  ELFObjectWriter *getObjectWriter() const { return ObjectWriter.get(); }

  // Reset stats at the beginning of a function.
  void resetStats() {
    if (ALLOW_DUMP)
      ICE_TLS_GET_FIELD(TLS)->StatsFunction.reset();
  }
  void dumpStats(const IceString &Name, bool Final = false);
  void statsUpdateEmitted(uint32_t InstCount) {
    if (!ALLOW_DUMP || !getFlags().DumpStats)
      return;
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.updateEmitted(InstCount);
    getStatsCumulative()->updateEmitted(InstCount);
  }
  void statsUpdateRegistersSaved(uint32_t Num) {
    if (!ALLOW_DUMP || !getFlags().DumpStats)
      return;
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.updateRegistersSaved(Num);
    getStatsCumulative()->updateRegistersSaved(Num);
  }
  void statsUpdateFrameBytes(uint32_t Bytes) {
    if (!ALLOW_DUMP || !getFlags().DumpStats)
      return;
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.updateFrameBytes(Bytes);
    getStatsCumulative()->updateFrameBytes(Bytes);
  }
  void statsUpdateSpills() {
    if (!ALLOW_DUMP || !getFlags().DumpStats)
      return;
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.updateSpills();
    getStatsCumulative()->updateSpills();
  }
  void statsUpdateFills() {
    if (!ALLOW_DUMP || !getFlags().DumpStats)
      return;
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.updateFills();
    getStatsCumulative()->updateFills();
  }

  // These are predefined TimerStackIdT values.
  enum TimerStackKind { TSK_Default = 0, TSK_Funcs, TSK_Num };

  TimerStackIdT newTimerStackID(const IceString &Name);
  TimerIdT getTimerID(TimerStackIdT StackID, const IceString &Name);
  void pushTimer(TimerIdT ID, TimerStackIdT StackID = TSK_Default);
  void popTimer(TimerIdT ID, TimerStackIdT StackID = TSK_Default);
  void resetTimer(TimerStackIdT StackID);
  void setTimerName(TimerStackIdT StackID, const IceString &NewName);
  void dumpTimers(TimerStackIdT StackID = TSK_Default,
                  bool DumpCumulative = true);

  // Adds a newly parsed and constructed function to the Cfg work
  // queue.  Notifies any idle workers that a new function is
  // available for translating.  May block if the work queue is too
  // large, in order to control memory footprint.
  void cfgQueueBlockingPush(Cfg *Func) { CfgQ.blockingPush(Func); }
  // Takes a Cfg from the work queue for translating.  May block if
  // the work queue is currently empty.  Returns nullptr if there is
  // no more work - the queue is empty and either end() has been
  // called or the Sequential flag was set.
  Cfg *cfgQueueBlockingPop() { return CfgQ.blockingPop(); }
  // Notifies that no more work will be added to the work queue.
  void cfgQueueNotifyEnd() { CfgQ.notifyEnd(); }

  void startWorkerThreads() {
    size_t NumWorkers = getFlags().NumTranslationThreads;
    for (size_t i = 0; i < NumWorkers; ++i) {
      ThreadContext *WorkerTLS = new ThreadContext();
      AllThreadContexts.push_back(WorkerTLS);
      TranslationThreads.push_back(std::thread(
          &GlobalContext::translateFunctionsWrapper, this, WorkerTLS));
    }
    if (NumWorkers) {
      // TODO(stichnot): start a new thread for the emitter queue worker.
    }
  }

  void waitForWorkerThreads() {
    cfgQueueNotifyEnd();
    // TODO(stichnot): call end() on the emitter work queue.
    for (std::thread &Worker : TranslationThreads) {
      Worker.join();
    }
    TranslationThreads.clear();
    // TODO(stichnot): join the emitter thread.
  }

  // Translation thread startup routine.
  void translateFunctionsWrapper(ThreadContext *MyTLS) {
    ICE_TLS_SET_FIELD(TLS, MyTLS);
    translateFunctions();
  }
  // Translate functions from the Cfg queue until the queue is empty.
  void translateFunctions();

  // Utility function to match a symbol name against a match string.
  // This is used in a few cases where we want to take some action on
  // a particular function or symbol based on a command-line argument,
  // such as changing the verbose level for a particular function.  An
  // empty Match argument means match everything.  Returns true if
  // there is a match.
  static bool matchSymbolName(const IceString &SymbolName,
                              const IceString &Match) {
    return Match.empty() || Match == SymbolName;
  }

private:
  // Try to ensure mutexes are allocated on separate cache lines.

  ICE_CACHELINE_BOUNDARY;
  // Managed by getAllocator()
  GlobalLockType AllocLock;
  ArenaAllocator<> Allocator;

  ICE_CACHELINE_BOUNDARY;
  // Managed by getConstantPool()
  GlobalLockType ConstPoolLock;
  std::unique_ptr<ConstantPool> ConstPool;

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
  std::vector<TimerStack> Timers;

  ICE_CACHELINE_BOUNDARY;
  // StrLock is a global lock on the dump and emit output streams.
  typedef std::mutex StrLockType;
  StrLockType StrLock;
  Ostream *StrDump; // Stream for dumping / diagnostics
  Ostream *StrEmit; // Stream for code emission

  ICE_CACHELINE_BOUNDARY;

  const VerboseMask VMask;
  Intrinsics IntrinsicsInfo;
  const TargetArch Arch;
  const OptLevel Opt;
  const IceString TestPrefix;
  const ClFlags &Flags;
  RandomNumberGenerator RNG; // TODO(stichnot): Move into Cfg.
  std::unique_ptr<ELFObjectWriter> ObjectWriter;
  BoundedProducerConsumerQueue<Cfg> CfgQ;

  LockedPtr<ArenaAllocator<>> getAllocator() {
    return LockedPtr<ArenaAllocator<>>(&Allocator, &AllocLock);
  }
  LockedPtr<ConstantPool> getConstPool() {
    return LockedPtr<ConstantPool>(ConstPool.get(), &ConstPoolLock);
  }
  LockedPtr<CodeStats> getStatsCumulative() {
    return LockedPtr<CodeStats>(&StatsCumulative, &StatsLock);
  }
  LockedPtr<std::vector<TimerStack>> getTimers() {
    return LockedPtr<std::vector<TimerStack>>(&Timers, &TimerLock);
  }

  std::vector<ThreadContext *> AllThreadContexts;
  std::vector<std::thread> TranslationThreads;
  // Each thread has its own TLS pointer which is also held in
  // AllThreadContexts.
  ICE_TLS_DECLARE_FIELD(ThreadContext *, TLS);

  // Private helpers for mangleName()
  typedef llvm::SmallVector<char, 32> ManglerVector;
  void incrementSubstitutions(ManglerVector &OldName) const;

public:
  static void TlsInit() { ICE_TLS_INIT_FIELD(TLS); }
};

// Helper class to push and pop a timer marker.  The constructor
// pushes a marker, and the destructor pops it.  This is for
// convenient timing of regions of code.
class TimerMarker {
  TimerMarker(const TimerMarker &) = delete;
  TimerMarker &operator=(const TimerMarker &) = delete;

public:
  TimerMarker(TimerIdT ID, GlobalContext *Ctx)
      : ID(ID), Ctx(Ctx), Active(false) {
    if (ALLOW_DUMP) {
      Active = Ctx->getFlags().SubzeroTimingEnabled;
      if (Active)
        Ctx->pushTimer(ID);
    }
  }
  TimerMarker(TimerIdT ID, const Cfg *Func);

  ~TimerMarker() {
    if (ALLOW_DUMP && Active)
      Ctx->popTimer(ID);
  }

private:
  TimerIdT ID;
  GlobalContext *const Ctx;
  bool Active;
};

// Helper class for locking the streams and then automatically
// unlocking them.
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
