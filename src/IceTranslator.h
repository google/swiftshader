//===- subzero/src/IceTranslator.h - ICE to machine code --------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the general driver class for translating ICE to machine
/// code.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETRANSLATOR_H
#define SUBZERO_SRC_ICETRANSLATOR_H

#include "IceDefs.h"
#include "IceGlobalContext.h"

namespace llvm {
class Module;
} // end of namespace llvm

namespace Ice {

class ClFlags;
class Cfg;
class VariableDeclaration;
class GlobalContext;

/// Base class for translating ICE to machine code. Derived classes convert
/// other intermediate representations down to ICE, and then call the
/// appropriate (inherited) methods to convert ICE into machine instructions.
class Translator {
  Translator() = delete;
  Translator(const Translator &) = delete;
  Translator &operator=(const Translator &) = delete;

public:
  explicit Translator(GlobalContext *Ctx);

  virtual ~Translator() = default;
  const ErrorCode &getErrorStatus() const { return ErrorStatus; }

  GlobalContext *getContext() const { return Ctx; }

  const ClFlags &getFlags() const { return Ctx->getFlags(); }

  /// Translates the constructed ICE function Fcn to machine code. Takes
  /// ownership of Func.
  void translateFcn(std::unique_ptr<Cfg> Func);

  /// Lowers the given list of global addresses to target. Generates list of
  /// corresponding variable declarations.
  void
  lowerGlobals(std::unique_ptr<VariableDeclarationList> VariableDeclarations);

  /// Creates a name using the given prefix and corresponding index.
  std::string createUnnamedName(const IceString &Prefix, SizeT Index);

  /// Reports if there is a (potential) conflict between Name, and using Prefix
  /// to name unnamed names. Errors are put on Ostream. Returns true if there
  /// isn't a potential conflict.
  bool checkIfUnnamedNameSafe(const IceString &Name, const char *Kind,
                              const IceString &Prefix);

  uint32_t getNextSequenceNumber() { return NextSequenceNumber++; }

protected:
  GlobalContext *Ctx;
  uint32_t NextSequenceNumber;
  /// ErrorCode of the translation.
  ErrorCode ErrorStatus;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETRANSLATOR_H
