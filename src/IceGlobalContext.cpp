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

#include "IceDefs.h"
#include "IceTypes.h"
#include "IceCfg.h"
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
  TypePool() {}
  ValueType *getOrAdd(GlobalContext *Ctx, Type Ty, KeyType Key) {
    TupleType TupleKey = std::make_pair(Ty, Key);
    typename ContainerType::const_iterator Iter = Pool.find(TupleKey);
    if (Iter != Pool.end())
      return Iter->second;
    ValueType *Result = ValueType::create(Ctx, Ty, Key);
    Pool[TupleKey] = Result;
    return Result;
  }

private:
  typedef std::pair<Type, KeyType> TupleType;
  struct TupleCompare {
    bool operator()(const TupleType &A, const TupleType &B) {
      if (A.first != B.first)
        return A.first < B.first;
      if (KeyTypeHasFP)
        return memcmp(&A.second, &B.second, sizeof(KeyType)) < 0;
      return A.second < B.second;
    }
  };
  typedef std::map<const TupleType, ValueType *, TupleCompare> ContainerType;
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
};

GlobalContext::GlobalContext(llvm::raw_ostream *OsDump,
                             llvm::raw_ostream *OsEmit, VerboseMask Mask,
                             TargetArch Arch, OptLevel Opt,
                             IceString TestPrefix)
    : StrDump(OsDump), StrEmit(OsEmit), VMask(Mask),
      ConstPool(new ConstantPool()), Arch(Arch), Opt(Opt),
      TestPrefix(TestPrefix), HasEmittedFirstMethod(false) {}

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
  char NameBase[1 + Name.length()];
  const size_t BufLen = 30 + Name.length() + PrefixLength;
  char NewName[BufLen];
  uint32_t BaseLength = 0; // using uint32_t due to sscanf format string

  int ItemsParsed = sscanf(Name.c_str(), "_ZN%s", NameBase);
  if (ItemsParsed == 1) {
    // Transform _ZN3foo3barExyz ==> _ZN6Prefix3foo3barExyz
    //   (splice in "6Prefix")          ^^^^^^^
    snprintf(NewName, BufLen, "_ZN%u%s%s", PrefixLength,
             getTestPrefix().c_str(), NameBase);
    // We ignore the snprintf return value (here and below).  If we
    // somehow miscalculated the output buffer length, the output will
    // be truncated, but it will be truncated consistently for all
    // mangleName() calls on the same input string.
    return NewName;
  }

  ItemsParsed = sscanf(Name.c_str(), "_Z%u%s", &BaseLength, NameBase);
  if (ItemsParsed == 2 && BaseLength <= strlen(NameBase)) {
    // Transform _Z3barxyz ==> _ZN6Prefix3barExyz
    //                           ^^^^^^^^    ^
    // (splice in "N6Prefix", and insert "E" after "3bar")
    // But an "I" after the identifier indicates a template argument
    // list terminated with "E"; insert the new "E" before/after the
    // old "E".  E.g.:
    // Transform _Z3barIabcExyz ==> _ZN6Prefix3barIabcEExyz
    //                                ^^^^^^^^         ^
    // (splice in "N6Prefix", and insert "E" after "3barIabcE")
    char OrigName[Name.length()];
    char OrigSuffix[Name.length()];
    uint32_t ActualBaseLength = BaseLength;
    if (NameBase[ActualBaseLength] == 'I') {
      ++ActualBaseLength;
      while (NameBase[ActualBaseLength] != 'E' &&
             NameBase[ActualBaseLength] != '\0')
        ++ActualBaseLength;
    }
    strncpy(OrigName, NameBase, ActualBaseLength);
    OrigName[ActualBaseLength] = '\0';
    strcpy(OrigSuffix, NameBase + ActualBaseLength);
    snprintf(NewName, BufLen, "_ZN%u%s%u%sE%s", PrefixLength,
             getTestPrefix().c_str(), BaseLength, OrigName, OrigSuffix);
    return NewName;
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

void Timer::printElapsedUs(GlobalContext *Ctx, const IceString &Tag) const {
  if (Ctx->isVerbose(IceV_Timing)) {
    // Prefixing with '#' allows timing strings to be included
    // without error in textual assembly output.
    Ctx->getStrDump() << "# " << getElapsedUs() << " usec " << Tag << "\n";
  }
}

} // end of namespace Ice
