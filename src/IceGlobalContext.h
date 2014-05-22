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

#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/raw_ostream.h"

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

// TODO: Accesses to all non-const fields of GlobalContext need to
// be synchronized, especially the constant pool, the allocator, and
// the output streams.
class GlobalContext {
public:
  GlobalContext(llvm::raw_ostream *OsDump, llvm::raw_ostream *OsEmit,
                VerboseMask Mask, TargetArch Arch, OptLevel Opt,
                IceString TestPrefix);
  ~GlobalContext();

  // Returns true if any of the specified options in the verbose mask
  // are set.  If the argument is omitted, it checks if any verbose
  // options at all are set.  IceV_Timing is treated specially, so
  // that running with just IceV_Timing verbosity doesn't trigger an
  // avalanche of extra output.
  bool isVerbose(VerboseMask Mask = (IceV_All & ~IceV_Timing)) const {
    return VMask & Mask;
  }
  void setVerbose(VerboseMask Mask) { VMask = Mask; }
  void addVerbose(VerboseMask Mask) { VMask |= Mask; }
  void subVerbose(VerboseMask Mask) { VMask &= ~Mask; }

  Ostream &getStrDump() { return StrDump; }
  Ostream &getStrEmit() { return StrEmit; }

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
  Constant *getConstantInt(Type Ty, uint64_t ConstantInt64);
  Constant *getConstantFloat(float Value);
  Constant *getConstantDouble(double Value);
  // Returns a symbolic constant.
  Constant *getConstantSym(Type Ty, int64_t Offset, const IceString &Name = "",
                           bool SuppressMangling = false);

  // Allocate data of type T using the global allocator.
  template <typename T> T *allocate() { return Allocator.Allocate<T>(); }

private:
  Ostream StrDump; // Stream for dumping / diagnostics
  Ostream StrEmit; // Stream for code emission

  llvm::BumpPtrAllocator Allocator;
  VerboseMask VMask;
  llvm::OwningPtr<class ConstantPool> ConstPool;
  const TargetArch Arch;
  const OptLevel Opt;
  const IceString TestPrefix;
  bool HasEmittedFirstMethod;
  GlobalContext(const GlobalContext &) LLVM_DELETED_FUNCTION;
  GlobalContext &operator=(const GlobalContext &) LLVM_DELETED_FUNCTION;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEGLOBALCONTEXT_H
