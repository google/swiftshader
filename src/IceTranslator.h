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

#include <memory>

namespace llvm {
class Module;
}

namespace Ice {

class ClFlags;
class Cfg;
class GlobalAddress;
class GlobalContext;

// Base class for translating ICE to machine code.
// Derived classes convert other intermediate representations down to ICE,
// and then call the appropriate (inherited) methods to convert ICE into
// machine instructions.
class Translator {
public:
  typedef std::vector<Ice::GlobalAddress *> GlobalAddressList;

  Translator(GlobalContext *Ctx, const ClFlags &Flags)
      : Ctx(Ctx), Flags(Flags), ErrorStatus(0) {}

  ~Translator();
  bool getErrorStatus() const { return ErrorStatus; }

  GlobalContext *getContext() const { return Ctx; }

  const ClFlags &getFlags() const { return Flags; }

  /// Translates the constructed ICE function Fcn to machine code.
  /// Takes ownership of Fcn. Note: As a side effect, Field Func is
  /// set to Fcn.
  void translateFcn(Cfg *Fcn);

  /// Emits the constant pool.
  void emitConstants();

  /// Lowers the given list of global addresses to target.
  void lowerGlobals(const GlobalAddressList &GlobalAddresses);

  /// Creates a name using the given prefix and corresponding index.
  std::string createUnnamedName(const IceString &Prefix, SizeT Index);

  /// Reports if there is a (potential) conflict between Name, and using
  /// Prefix to name unnamed names. Errors are put on Ostream.
  /// Returns true if there isn't a potential conflict.
  bool checkIfUnnamedNameSafe(const IceString &Name, const char *Kind,
                              const IceString &Prefix, Ostream &Stream);

  // Walks module and generates names for unnamed globals using prefix
  // getFlags().DefaultGlobalPrefix, if the prefix is non-empty.
  void nameUnnamedGlobalAddresses(llvm::Module *Mod);

  // Walks module and generates names for unnamed functions using
  // prefix getFlags().DefaultFunctionPrefix, if the prefix is
  // non-empty.
  void nameUnnamedFunctions(llvm::Module *Mod);

protected:
  GlobalContext *Ctx;
  const ClFlags &Flags;
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
  std::unique_ptr<Cfg> Func;

private:
  Translator(const Translator &) = delete;
  Translator &operator=(const Translator &) = delete;
};
}

#endif // SUBZERO_SRC_ICETRANSLATOR_H
