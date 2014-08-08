//===- subzero/src/IceGlobalContext.cpp - Global context defs ---*- C++ -*-===//
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

#include "IceDefs.h"
#include "IceTypes.h"
#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceGlobalContext.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

// TypePool maps constants of type KeyType (e.g. float) to pointers to
// type ValueType (e.g. ConstantFloat).  KeyType values are compared
// using memcmp() because of potential NaN values in KeyType values.
// KeyTypeHasFP indicates whether KeyType is a floating-point type
// whose values need to be compared using memcmp() for NaN
// correctness.  TODO: use std::is_floating_point<KeyType> instead of
// KeyTypeHasFP with C++11.
template <typename KeyType, typename ValueType, bool KeyTypeHasFP = false>
class TypePool {
  TypePool(const TypePool &) LLVM_DELETED_FUNCTION;
  TypePool &operator=(const TypePool &) LLVM_DELETED_FUNCTION;

public:
  TypePool() : NextPoolID(0) {}
  ValueType *getOrAdd(GlobalContext *Ctx, Type Ty, KeyType Key) {
    TupleType TupleKey = std::make_pair(Ty, Key);
    typename ContainerType::const_iterator Iter = Pool.find(TupleKey);
    if (Iter != Pool.end())
      return Iter->second;
    ValueType *Result = ValueType::create(Ctx, Ty, Key, NextPoolID++);
    Pool[TupleKey] = Result;
    return Result;
  }
  ConstantList getConstantPool() const {
    ConstantList Constants;
    Constants.reserve(Pool.size());
    // TODO: replace the loop with std::transform + lambdas.
    for (typename ContainerType::const_iterator I = Pool.begin(),
                                                E = Pool.end();
         I != E; ++I) {
      Constants.push_back(I->second);
    }
    return Constants;
  }

private:
  typedef std::pair<Type, KeyType> TupleType;
  struct TupleCompare {
    bool operator()(const TupleType &A, const TupleType &B) const {
      if (A.first != B.first)
        return A.first < B.first;
      if (KeyTypeHasFP)
        return memcmp(&A.second, &B.second, sizeof(KeyType)) < 0;
      return A.second < B.second;
    }
  };
  typedef std::map<const TupleType, ValueType *, TupleCompare> ContainerType;
  ContainerType Pool;
  uint32_t NextPoolID;
};

// UndefPool maps ICE types to the corresponding ConstantUndef values.
class UndefPool {
  UndefPool(const UndefPool &) LLVM_DELETED_FUNCTION;
  UndefPool &operator=(const UndefPool &) LLVM_DELETED_FUNCTION;

public:
  UndefPool() : NextPoolID(0) {}

  ConstantUndef *getOrAdd(GlobalContext *Ctx, Type Ty) {
    ContainerType::iterator I = Pool.find(Ty);
    if (I != Pool.end())
      return I->second;
    ConstantUndef *Undef = ConstantUndef::create(Ctx, Ty, NextPoolID++);
    Pool[Ty] = Undef;
    return Undef;
  }

private:
  uint32_t NextPoolID;
  typedef std::map<Type, ConstantUndef *> ContainerType;
  ContainerType Pool;
};

// The global constant pool bundles individual pools of each type of
// interest.
class ConstantPool {
  ConstantPool(const ConstantPool &) LLVM_DELETED_FUNCTION;
  ConstantPool &operator=(const ConstantPool &) LLVM_DELETED_FUNCTION;

public:
  ConstantPool() {}
  TypePool<float, ConstantFloat, true> Floats;
  TypePool<double, ConstantDouble, true> Doubles;
  TypePool<uint64_t, ConstantInteger> Integers;
  TypePool<RelocatableTuple, ConstantRelocatable> Relocatables;
  UndefPool Undefs;
};

GlobalContext::GlobalContext(llvm::raw_ostream *OsDump,
                             llvm::raw_ostream *OsEmit, VerboseMask Mask,
                             TargetArch Arch, OptLevel Opt,
                             IceString TestPrefix, const ClFlags &Flags)
    : StrDump(OsDump), StrEmit(OsEmit), VMask(Mask),
      ConstPool(new ConstantPool()), Arch(Arch), Opt(Opt),
      TestPrefix(TestPrefix), Flags(Flags), HasEmittedFirstMethod(false) {}

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
  if (getTestPrefix().empty())
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

GlobalContext::~GlobalContext() {}

Constant *GlobalContext::getConstantInt(Type Ty, uint64_t ConstantInt64) {
  return ConstPool->Integers.getOrAdd(this, Ty, ConstantInt64);
}

Constant *GlobalContext::getConstantFloat(float ConstantFloat) {
  return ConstPool->Floats.getOrAdd(this, IceType_f32, ConstantFloat);
}

Constant *GlobalContext::getConstantDouble(double ConstantDouble) {
  return ConstPool->Doubles.getOrAdd(this, IceType_f64, ConstantDouble);
}

Constant *GlobalContext::getConstantSym(Type Ty, int64_t Offset,
                                        const IceString &Name,
                                        bool SuppressMangling) {
  return ConstPool->Relocatables.getOrAdd(
      this, Ty, RelocatableTuple(Offset, Name, SuppressMangling));
}

Constant *GlobalContext::getConstantUndef(Type Ty) {
  return ConstPool->Undefs.getOrAdd(this, Ty);
}

Constant *GlobalContext::getConstantZero(Type Ty) {
  switch (Ty) {
  case IceType_i1:
  case IceType_i8:
  case IceType_i16:
  case IceType_i32:
  case IceType_i64:
    return getConstantInt(Ty, 0);
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
  case IceType_i64:
    return ConstPool->Integers.getConstantPool();
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

void Timer::printElapsedUs(GlobalContext *Ctx, const IceString &Tag) const {
  if (Ctx->isVerbose(IceV_Timing)) {
    // Prefixing with '#' allows timing strings to be included
    // without error in textual assembly output.
    Ctx->getStrDump() << "# " << getElapsedUs() << " usec " << Tag << "\n";
  }
}

} // end of namespace Ice
