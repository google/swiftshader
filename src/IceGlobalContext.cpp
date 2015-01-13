//===- subzero/src/IceGlobalContext.cpp - Global context defs -------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines aspects of the compilation that persist across
// multiple functions.
//
//===----------------------------------------------------------------------===//

#include <ctype.h> // isdigit(), isupper()
#include <locale>  // locale
#include <unordered_map>

#include "llvm/Support/Timer.h"

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalContext.h"
#include "IceGlobalInits.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"
#include "IceTimerTree.h"
#include "IceTypes.h"

namespace std {
template <> struct hash<Ice::RelocatableTuple> {
  size_t operator()(const Ice::RelocatableTuple &Key) const {
    return hash<Ice::IceString>()(Key.Name) +
           hash<Ice::RelocOffsetT>()(Key.Offset);
  }
};
} // end of namespace std

namespace Ice {

// TypePool maps constants of type KeyType (e.g. float) to pointers to
// type ValueType (e.g. ConstantFloat).
template <Type Ty, typename KeyType, typename ValueType> class TypePool {
  TypePool(const TypePool &) = delete;
  TypePool &operator=(const TypePool &) = delete;

public:
  TypePool() : NextPoolID(0) {}
  ValueType *getOrAdd(GlobalContext *Ctx, KeyType Key) {
    auto Iter = Pool.find(Key);
    if (Iter != Pool.end())
      return Iter->second;
    ValueType *Result = ValueType::create(Ctx, Ty, Key, NextPoolID++);
    Pool[Key] = Result;
    return Result;
  }
  ConstantList getConstantPool() const {
    ConstantList Constants;
    Constants.reserve(Pool.size());
    for (auto &I : Pool)
      Constants.push_back(I.second);
    return Constants;
  }

private:
  typedef std::unordered_map<KeyType, ValueType *> ContainerType;
  ContainerType Pool;
  uint32_t NextPoolID;
};

// UndefPool maps ICE types to the corresponding ConstantUndef values.
class UndefPool {
  UndefPool(const UndefPool &) = delete;
  UndefPool &operator=(const UndefPool &) = delete;

public:
  UndefPool() : NextPoolID(0), Pool(IceType_NUM) {}

  ConstantUndef *getOrAdd(GlobalContext *Ctx, Type Ty) {
    if (Pool[Ty] == nullptr)
      Pool[Ty] = ConstantUndef::create(Ctx, Ty, NextPoolID++);
    return Pool[Ty];
  }

private:
  uint32_t NextPoolID;
  std::vector<ConstantUndef *> Pool;
};

// The global constant pool bundles individual pools of each type of
// interest.
class ConstantPool {
  ConstantPool(const ConstantPool &) = delete;
  ConstantPool &operator=(const ConstantPool &) = delete;

public:
  ConstantPool() {}
  TypePool<IceType_f32, float, ConstantFloat> Floats;
  TypePool<IceType_f64, double, ConstantDouble> Doubles;
  TypePool<IceType_i1, int8_t, ConstantInteger32> Integers1;
  TypePool<IceType_i8, int8_t, ConstantInteger32> Integers8;
  TypePool<IceType_i16, int16_t, ConstantInteger32> Integers16;
  TypePool<IceType_i32, int32_t, ConstantInteger32> Integers32;
  TypePool<IceType_i64, int64_t, ConstantInteger64> Integers64;
  TypePool<IceType_i32, RelocatableTuple, ConstantRelocatable> Relocatables;
  UndefPool Undefs;
};

void CodeStats::dump(const IceString &Name, Ostream &Str) {
  if (!ALLOW_DUMP)
    return;
  Str << "|" << Name << "|Inst Count  |" << InstructionsEmitted << "\n";
  Str << "|" << Name << "|Regs Saved  |" << RegistersSaved << "\n";
  Str << "|" << Name << "|Frame Bytes |" << FrameBytes << "\n";
  Str << "|" << Name << "|Spills      |" << Spills << "\n";
  Str << "|" << Name << "|Fills       |" << Fills << "\n";
  Str << "|" << Name << "|Spills+Fills|" << Spills + Fills << "\n";
  Str << "|" << Name << "|Memory Usage|";
  if (ssize_t MemUsed = llvm::TimeRecord::getCurrentTime(false).getMemUsed())
    Str << MemUsed;
  else
    Str << "(requires '-track-memory')";
  Str << "\n";
}

GlobalContext::GlobalContext(Ostream *OsDump, Ostream *OsEmit,
                             ELFStreamer *ELFStr, VerboseMask Mask,
                             TargetArch Arch, OptLevel Opt,
                             IceString TestPrefix, const ClFlags &Flags)
    : StrDump(OsDump), StrEmit(OsEmit), VMask(Mask),
      ConstPool(new ConstantPool()), Arch(Arch), Opt(Opt),
      TestPrefix(TestPrefix), Flags(Flags), RNG(""), ObjectWriter() {
  // Pre-register built-in stack names.
  if (ALLOW_DUMP) {
    newTimerStackID("Total across all functions");
    newTimerStackID("Per-function summary");
  }
  if (Flags.UseELFWriter) {
    ObjectWriter.reset(new ELFObjectWriter(*this, *ELFStr));
  }
}

// Scan a string for S[0-9A-Z]*_ patterns and replace them with
// S<num>_ where <num> is the next base-36 value.  If a type name
// legitimately contains that pattern, then the substitution will be
// made in error and most likely the link will fail.  In this case,
// the test classes can be rewritten not to use that pattern, which is
// much simpler and more reliable than implementing a full demangling
// parser.  Another substitution-in-error may occur if a type
// identifier ends with the pattern S[0-9A-Z]*, because an immediately
// following substitution string like "S1_" or "PS1_" may be combined
// with the previous type.
void GlobalContext::incrementSubstitutions(ManglerVector &OldName) const {
  const std::locale CLocale("C");
  // Provide extra space in case the length of <num> increases.
  ManglerVector NewName(OldName.size() * 2);
  size_t OldPos = 0;
  size_t NewPos = 0;
  size_t OldLen = OldName.size();
  for (; OldPos < OldLen; ++OldPos, ++NewPos) {
    if (OldName[OldPos] == '\0')
      break;
    if (OldName[OldPos] == 'S') {
      // Search forward until we find _ or invalid character (including \0).
      bool AllZs = true;
      bool Found = false;
      size_t Last;
      for (Last = OldPos + 1; Last < OldLen; ++Last) {
        char Ch = OldName[Last];
        if (Ch == '_') {
          Found = true;
          break;
        } else if (std::isdigit(Ch) || std::isupper(Ch, CLocale)) {
          if (Ch != 'Z')
            AllZs = false;
        } else {
          // Invalid character, stop searching.
          break;
        }
      }
      if (Found) {
        NewName[NewPos++] = OldName[OldPos++]; // 'S'
        size_t Length = Last - OldPos;
        // NewPos and OldPos point just past the 'S'.
        assert(NewName[NewPos - 1] == 'S');
        assert(OldName[OldPos - 1] == 'S');
        assert(OldName[OldPos + Length] == '_');
        if (AllZs) {
          // Replace N 'Z' characters with a '0' (if N=0) or '1' (if
          // N>0) followed by N '0' characters.
          NewName[NewPos++] = (Length ? '1' : '0');
          for (size_t i = 0; i < Length; ++i) {
            NewName[NewPos++] = '0';
          }
        } else {
          // Iterate right-to-left and increment the base-36 number.
          bool Carry = true;
          for (size_t i = 0; i < Length; ++i) {
            size_t Offset = Length - 1 - i;
            char Ch = OldName[OldPos + Offset];
            if (Carry) {
              Carry = false;
              switch (Ch) {
              case '9':
                Ch = 'A';
                break;
              case 'Z':
                Ch = '0';
                Carry = true;
                break;
              default:
                ++Ch;
                break;
              }
            }
            NewName[NewPos + Offset] = Ch;
          }
          NewPos += Length;
        }
        OldPos = Last;
        // Fall through and let the '_' be copied across.
      }
    }
    NewName[NewPos] = OldName[OldPos];
  }
  assert(NewName[NewPos] == '\0');
  OldName = NewName;
}

// In this context, name mangling means to rewrite a symbol using a
// given prefix.  For a C++ symbol, nest the original symbol inside
// the "prefix" namespace.  For other symbols, just prepend the
// prefix.
IceString GlobalContext::mangleName(const IceString &Name) const {
  // An already-nested name like foo::bar() gets pushed down one
  // level, making it equivalent to Prefix::foo::bar().
  //   _ZN3foo3barExyz ==> _ZN6Prefix3foo3barExyz
  // A non-nested but mangled name like bar() gets nested, making it
  // equivalent to Prefix::bar().
  //   _Z3barxyz ==> ZN6Prefix3barExyz
  // An unmangled, extern "C" style name, gets a simple prefix:
  //   bar ==> Prefixbar
  if (!ALLOW_DUMP || getTestPrefix().empty())
    return Name;

  unsigned PrefixLength = getTestPrefix().length();
  ManglerVector NameBase(1 + Name.length());
  const size_t BufLen = 30 + Name.length() + PrefixLength;
  ManglerVector NewName(BufLen);
  uint32_t BaseLength = 0; // using uint32_t due to sscanf format string

  int ItemsParsed = sscanf(Name.c_str(), "_ZN%s", NameBase.data());
  if (ItemsParsed == 1) {
    // Transform _ZN3foo3barExyz ==> _ZN6Prefix3foo3barExyz
    //   (splice in "6Prefix")          ^^^^^^^
    snprintf(NewName.data(), BufLen, "_ZN%u%s%s", PrefixLength,
             getTestPrefix().c_str(), NameBase.data());
    // We ignore the snprintf return value (here and below).  If we
    // somehow miscalculated the output buffer length, the output will
    // be truncated, but it will be truncated consistently for all
    // mangleName() calls on the same input string.
    incrementSubstitutions(NewName);
    return NewName.data();
  }

  // Artificially limit BaseLength to 9 digits (less than 1 billion)
  // because sscanf behavior is undefined on integer overflow.  If
  // there are more than 9 digits (which we test by looking at the
  // beginning of NameBase), then we consider this a failure to parse
  // a namespace mangling, and fall back to the simple prefixing.
  ItemsParsed = sscanf(Name.c_str(), "_Z%9u%s", &BaseLength, NameBase.data());
  if (ItemsParsed == 2 && BaseLength <= strlen(NameBase.data()) &&
      !isdigit(NameBase[0])) {
    // Transform _Z3barxyz ==> _ZN6Prefix3barExyz
    //                           ^^^^^^^^    ^
    // (splice in "N6Prefix", and insert "E" after "3bar")
    // But an "I" after the identifier indicates a template argument
    // list terminated with "E"; insert the new "E" before/after the
    // old "E".  E.g.:
    // Transform _Z3barIabcExyz ==> _ZN6Prefix3barIabcEExyz
    //                                ^^^^^^^^         ^
    // (splice in "N6Prefix", and insert "E" after "3barIabcE")
    ManglerVector OrigName(Name.length());
    ManglerVector OrigSuffix(Name.length());
    uint32_t ActualBaseLength = BaseLength;
    if (NameBase[ActualBaseLength] == 'I') {
      ++ActualBaseLength;
      while (NameBase[ActualBaseLength] != 'E' &&
             NameBase[ActualBaseLength] != '\0')
        ++ActualBaseLength;
    }
    strncpy(OrigName.data(), NameBase.data(), ActualBaseLength);
    OrigName[ActualBaseLength] = '\0';
    strcpy(OrigSuffix.data(), NameBase.data() + ActualBaseLength);
    snprintf(NewName.data(), BufLen, "_ZN%u%s%u%sE%s", PrefixLength,
             getTestPrefix().c_str(), BaseLength, OrigName.data(),
             OrigSuffix.data());
    incrementSubstitutions(NewName);
    return NewName.data();
  }

  // Transform bar ==> Prefixbar
  //                   ^^^^^^
  return getTestPrefix() + Name;
}

GlobalContext::~GlobalContext() {
  llvm::DeleteContainerPointers(GlobalDeclarations);
}

Constant *GlobalContext::getConstantInt(Type Ty, int64_t Value) {
  switch (Ty) {
  case IceType_i1:
    return getConstantInt1(Value);
  case IceType_i8:
    return getConstantInt8(Value);
  case IceType_i16:
    return getConstantInt16(Value);
  case IceType_i32:
    return getConstantInt32(Value);
  case IceType_i64:
    return getConstantInt64(Value);
  default:
    llvm_unreachable("Bad integer type for getConstant");
  }
  return nullptr;
}

Constant *GlobalContext::getConstantInt1(int8_t ConstantInt1) {
  ConstantInt1 &= INT8_C(1);
  return ConstPool->Integers1.getOrAdd(this, ConstantInt1);
}

Constant *GlobalContext::getConstantInt8(int8_t ConstantInt8) {
  return ConstPool->Integers8.getOrAdd(this, ConstantInt8);
}

Constant *GlobalContext::getConstantInt16(int16_t ConstantInt16) {
  return ConstPool->Integers16.getOrAdd(this, ConstantInt16);
}

Constant *GlobalContext::getConstantInt32(int32_t ConstantInt32) {
  return ConstPool->Integers32.getOrAdd(this, ConstantInt32);
}

Constant *GlobalContext::getConstantInt64(int64_t ConstantInt64) {
  return ConstPool->Integers64.getOrAdd(this, ConstantInt64);
}

Constant *GlobalContext::getConstantFloat(float ConstantFloat) {
  return ConstPool->Floats.getOrAdd(this, ConstantFloat);
}

Constant *GlobalContext::getConstantDouble(double ConstantDouble) {
  return ConstPool->Doubles.getOrAdd(this, ConstantDouble);
}

Constant *GlobalContext::getConstantSym(RelocOffsetT Offset,
                                        const IceString &Name,
                                        bool SuppressMangling) {
  return ConstPool->Relocatables.getOrAdd(
      this, RelocatableTuple(Offset, Name, SuppressMangling));
}

Constant *GlobalContext::getConstantUndef(Type Ty) {
  return ConstPool->Undefs.getOrAdd(this, Ty);
}

Constant *GlobalContext::getConstantZero(Type Ty) {
  switch (Ty) {
  case IceType_i1:
    return getConstantInt1(0);
  case IceType_i8:
    return getConstantInt8(0);
  case IceType_i16:
    return getConstantInt16(0);
  case IceType_i32:
    return getConstantInt32(0);
  case IceType_i64:
    return getConstantInt64(0);
  case IceType_f32:
    return getConstantFloat(0);
  case IceType_f64:
    return getConstantDouble(0);
  case IceType_v4i1:
  case IceType_v8i1:
  case IceType_v16i1:
  case IceType_v16i8:
  case IceType_v8i16:
  case IceType_v4i32:
  case IceType_v4f32: {
    IceString Str;
    llvm::raw_string_ostream BaseOS(Str);
    BaseOS << "Unsupported constant type: " << Ty;
    llvm_unreachable(BaseOS.str().c_str());
  } break;
  case IceType_void:
  case IceType_NUM:
    break;
  }
  llvm_unreachable("Unknown type");
}

ConstantList GlobalContext::getConstantPool(Type Ty) const {
  switch (Ty) {
  case IceType_i1:
  case IceType_i8:
  case IceType_i16:
  case IceType_i32:
    return ConstPool->Integers32.getConstantPool();
  case IceType_i64:
    return ConstPool->Integers64.getConstantPool();
  case IceType_f32:
    return ConstPool->Floats.getConstantPool();
  case IceType_f64:
    return ConstPool->Doubles.getConstantPool();
  case IceType_v4i1:
  case IceType_v8i1:
  case IceType_v16i1:
  case IceType_v16i8:
  case IceType_v8i16:
  case IceType_v4i32:
  case IceType_v4f32: {
    IceString Str;
    llvm::raw_string_ostream BaseOS(Str);
    BaseOS << "Unsupported constant type: " << Ty;
    llvm_unreachable(BaseOS.str().c_str());
  } break;
  case IceType_void:
  case IceType_NUM:
    break;
  }
  llvm_unreachable("Unknown type");
}

FunctionDeclaration *
GlobalContext::newFunctionDeclaration(const FuncSigType *Signature,
                                      unsigned CallingConv, unsigned Linkage,
                                      bool IsProto) {
  FunctionDeclaration *Func = new FunctionDeclaration(
      *Signature, static_cast<llvm::CallingConv::ID>(CallingConv),
      static_cast<llvm::GlobalValue::LinkageTypes>(Linkage), IsProto);
  GlobalDeclarations.push_back(Func);
  return Func;
}

VariableDeclaration *GlobalContext::newVariableDeclaration() {
  VariableDeclaration *Var = new VariableDeclaration();
  GlobalDeclarations.push_back(Var);
  return Var;
}

TimerIdT GlobalContext::getTimerID(TimerStackIdT StackID,
                                   const IceString &Name) {
  assert(StackID < Timers.size());
  return Timers[StackID].getTimerID(Name);
}

TimerStackIdT GlobalContext::newTimerStackID(const IceString &Name) {
  if (!ALLOW_DUMP)
    return 0;
  TimerStackIdT NewID = Timers.size();
  Timers.push_back(TimerStack(Name));
  return NewID;
}

void GlobalContext::pushTimer(TimerIdT ID, TimerStackIdT StackID) {
  assert(StackID < Timers.size());
  Timers[StackID].push(ID);
}

void GlobalContext::popTimer(TimerIdT ID, TimerStackIdT StackID) {
  assert(StackID < Timers.size());
  Timers[StackID].pop(ID);
}

void GlobalContext::resetTimer(TimerStackIdT StackID) {
  assert(StackID < Timers.size());
  Timers[StackID].reset();
}

void GlobalContext::setTimerName(TimerStackIdT StackID,
                                 const IceString &NewName) {
  assert(StackID < Timers.size());
  Timers[StackID].setName(NewName);
}

void GlobalContext::dumpStats(const IceString &Name, bool Final) {
  if (!ALLOW_DUMP)
    return;
  if (Flags.DumpStats) {
    if (Final) {
      StatsCumulative.dump(Name, getStrDump());
    } else {
      StatsFunction.dump(Name, getStrDump());
      StatsCumulative.dump("_TOTAL_", getStrDump());
    }
  }
}

void GlobalContext::dumpTimers(TimerStackIdT StackID, bool DumpCumulative) {
  if (!ALLOW_DUMP)
    return;
  assert(Timers.size() > StackID);
  Timers[StackID].dump(getStrDump(), DumpCumulative);
}

TimerMarker::TimerMarker(TimerIdT ID, const Cfg *Func)
    : ID(ID), Ctx(Func->getContext()), Active(false) {
  if (ALLOW_DUMP) {
    Active = Func->getFocusedTiming() || Ctx->getFlags().SubzeroTimingEnabled;
    if (Active)
      Ctx->pushTimer(ID);
  }
}

} // end of namespace Ice
