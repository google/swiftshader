//===- subzero/src/IceTranslator.h - ICE to machine code --------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the general driver class for translating ICE to
// machine code.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETRANSLATOR_H
#define SUBZERO_SRC_ICETRANSLATOR_H

#include "llvm/ADT/OwningPtr.h"

namespace Ice {

class ClFlags;
class Cfg;
class GlobalContext;

// Base class for translating ICE to machine code.
// Derived classes convert other intermediate representations down to ICE,
// and then call the appropriate (inherited) methods to convert ICE into
// machine instructions.
class Translator {
public:
  Translator(GlobalContext *Ctx) : Ctx(Ctx), ErrorStatus(0) {}

  ~Translator();
  bool getErrorStatus() const { return ErrorStatus; }

protected:
  GlobalContext *Ctx;
  // The exit status of the translation. False is successful. True
  // otherwise.
  bool ErrorStatus;
  // Ideally, Func would be inside the methods that converts IR to
  // functions.  However, emitting the constant pool requires a valid
  // Cfg object, so we need to defer deleting the last non-empty Cfg
  // object to emit the constant pool (via emitConstants). TODO:
  // Since all constants are globally pooled in the GlobalContext
  // object, change all Constant related functions to use
  // GlobalContext instead of Cfg, and then make emitConstantPool use
  // that.
  llvm::OwningPtr<Cfg> Func;

  /// Translates the constructed ICE function Fcn to machine code.
  /// Note: As a side effect, Field Func is set to Fcn.
  void translateFcn(Cfg *Fcn);

  /// Emits the constant pool.
  void emitConstants();

private:
  Translator(const Translator &) LLVM_DELETED_FUNCTION;
  Translator &operator=(const Translator &) LLVM_DELETED_FUNCTION;
};
}

#endif // SUBZERO_SRC_ICETRANSLATOR_H
