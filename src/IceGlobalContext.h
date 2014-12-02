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

#include <memory>

#include "llvm/Support/Allocator.h"

#include "IceDefs.h"
#include "IceClFlags.h"
#include "IceELFObjectWriter.h"
#include "IceIntrinsics.h"
#include "IceRNG.h"
#include "IceTimerTree.h"
#include "IceTypes.h"

namespace Ice {

class ClFlags;
class FuncSigType;

// This class collects rudimentary statistics during translation.
class CodeStats {
  CodeStats(const CodeStats &) = delete;
  // CodeStats &operator=(const CodeStats &) = delete;

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
  void dump(const IceString &Name, Ostream &Str) {
    if (!ALLOW_DUMP)
      return;
    Str << "|" << Name << "|Inst Count  |" << InstructionsEmitted << "\n";
    Str << "|" << Name << "|Regs Saved  |" << RegistersSaved << "\n";
    Str << "|" << Name << "|Frame Bytes |" << FrameBytes << "\n";
    Str << "|" << Name << "|Spills      |" << Spills << "\n";
    Str << "|" << Name << "|Fills       |" << Fills << "\n";
    Str << "|" << Name << "|Spills+Fills|" << Spills + Fills << "\n";
  }

private:
  uint32_t InstructionsEmitted;
  uint32_t RegistersSaved;
  uint32_t FrameBytes;
  uint32_t Spills;
  uint32_t Fills;
};

// TODO: Accesses to all non-const fields of GlobalContext need to
// be synchronized, especially the constant pool, the allocator, and
// the output streams.
class GlobalContext {
  GlobalContext(const GlobalContext &) = delete;
  GlobalContext &operator=(const GlobalContext &) = delete;

public:
  GlobalContext(Ostream *OsDump, Ostream *OsEmit, ELFStreamer *ELFStreamer,
                VerboseMask Mask, TargetArch Arch, OptLevel Opt,
                IceString TestPrefix, const ClFlags &Flags);
  ~GlobalContext();

  // Returns true if any of the specified options in the verbose mask
  // are set.  If the argument is omitted, it checks if any verbose
  // options at all are set.
  VerboseMask getVerbose() const { return VMask; }
  bool isVerbose(VerboseMask Mask = IceV_All) const { return VMask & Mask; }
  void setVerbose(VerboseMask Mask) { VMask = Mask; }
  void addVerbose(VerboseMask Mask) { VMask |= Mask; }
  void subVerbose(VerboseMask Mask) { VMask &= ~Mask; }

  Ostream &getStrDump() { return *StrDump; }
  Ostream &getStrEmit() { return *StrEmit; }

  TargetArch getTargetArch() const { return Arch; }
  OptLevel getOptLevel() const { return Opt; }

  // When emitting assembly, we allow a string to be prepended to
  // names of translated functions.  This makes it easier to create an
  // execution test against a reference translator like llc, with both
  // translators using the same bitcode as input.
  IceString getTestPrefix() const { return TestPrefix; }
  IceString mangleName(const IceString &Name) const;

  // The purpose of HasEmitted is to add a header comment at the
  // beginning of assembly code emission, doing it once per file
  // rather than once per function.
  bool testAndSetHasEmittedFirstMethod() {
    bool HasEmitted = HasEmittedFirstMethod;
    HasEmittedFirstMethod = true;
    return HasEmitted;
  }

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
  ConstantList getConstantPool(Type Ty) const;
  // Returns a new function declaration, allocated in an internal
  // memory pool.  Ownership of the function is maintained by this
  // class instance.
  FunctionDeclaration *newFunctionDeclaration(const FuncSigType *Signature,
                                              unsigned CallingConv,
                                              unsigned Linkage, bool IsProto);

  // Returns a new global variable declaration, allocated in an
  // internal memory pool.  Ownership of the function is maintained by
  // this class instance.
  VariableDeclaration *newVariableDeclaration();

  const ClFlags &getFlags() const { return Flags; }

  bool isIRGenerationDisabled() const {
    return ALLOW_DISABLE_IR_GEN ? getFlags().DisableIRGeneration : false;
  }

  // Allocate data of type T using the global allocator.
  template <typename T> T *allocate() { return Allocator.Allocate<T>(); }

  const Intrinsics &getIntrinsicsInfo() const { return IntrinsicsInfo; }

  // TODO(wala,stichnot): Make the RNG play nicely with multithreaded
  // translation.
  RandomNumberGenerator &getRNG() { return RNG; }

  ELFObjectWriter *getObjectWriter() const { return ObjectWriter.get(); }

  // Reset stats at the beginning of a function.
  void resetStats() { StatsFunction.reset(); }
  void dumpStats(const IceString &Name, bool Final = false);
  void statsUpdateEmitted(uint32_t InstCount) {
    StatsFunction.updateEmitted(InstCount);
    StatsCumulative.updateEmitted(InstCount);
  }
  void statsUpdateRegistersSaved(uint32_t Num) {
    StatsFunction.updateRegistersSaved(Num);
    StatsCumulative.updateRegistersSaved(Num);
  }
  void statsUpdateFrameBytes(uint32_t Bytes) {
    StatsFunction.updateFrameBytes(Bytes);
    StatsCumulative.updateFrameBytes(Bytes);
  }
  void statsUpdateSpills() {
    StatsFunction.updateSpills();
    StatsCumulative.updateSpills();
  }
  void statsUpdateFills() {
    StatsFunction.updateFills();
    StatsCumulative.updateFills();
  }

  // These are predefined TimerStackIdT values.
  enum TimerStackKind {
    TSK_Default = 0,
    TSK_Funcs,
    TSK_Num
  };

  TimerIdT getTimerID(TimerStackIdT StackID, const IceString &Name);
  TimerStackIdT newTimerStackID(const IceString &Name);
  void pushTimer(TimerIdT ID, TimerStackIdT StackID = TSK_Default);
  void popTimer(TimerIdT ID, TimerStackIdT StackID = TSK_Default);
  void resetTimer(TimerStackIdT StackID);
  void setTimerName(TimerStackIdT StackID, const IceString &NewName);
  void dumpTimers(TimerStackIdT StackID = TSK_Default,
                  bool DumpCumulative = true);

private:
  Ostream *StrDump; // Stream for dumping / diagnostics
  Ostream *StrEmit; // Stream for code emission

  llvm::BumpPtrAllocatorImpl<llvm::MallocAllocator, 1024 * 1024> Allocator;
  VerboseMask VMask;
  std::unique_ptr<class ConstantPool> ConstPool;
  Intrinsics IntrinsicsInfo;
  const TargetArch Arch;
  const OptLevel Opt;
  const IceString TestPrefix;
  const ClFlags &Flags;
  bool HasEmittedFirstMethod;
  RandomNumberGenerator RNG;
  std::unique_ptr<ELFObjectWriter> ObjectWriter;
  CodeStats StatsFunction;
  CodeStats StatsCumulative;
  std::vector<TimerStack> Timers;
  std::vector<GlobalDeclaration *> GlobalDeclarations;

  // Private helpers for mangleName()
  typedef llvm::SmallVector<char, 32> ManglerVector;
  void incrementSubstitutions(ManglerVector &OldName) const;
};

// Helper class to push and pop a timer marker.  The constructor
// pushes a marker, and the destructor pops it.  This is for
// convenient timing of regions of code.
class TimerMarker {
  TimerMarker(const TimerMarker &) = delete;
  TimerMarker &operator=(const TimerMarker &) = delete;

public:
  TimerMarker(TimerIdT ID, GlobalContext *Ctx)
      : ID(ID), Ctx(Ctx), Active(Ctx->getFlags().SubzeroTimingEnabled) {
    if (Active)
      Ctx->pushTimer(ID);
  }
  TimerMarker(TimerIdT ID, const Cfg *Func);

  ~TimerMarker() {
    if (Active)
      Ctx->popTimer(ID);
  }

private:
  TimerIdT ID;
  GlobalContext *const Ctx;
  const bool Active;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEGLOBALCONTEXT_H
