//===- subzero/src/IceConverter.h - Converts LLVM to ICE --------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the LLVM to ICE converter.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECONVERTER_H
#define SUBZERO_SRC_ICECONVERTER_H

#include "IceTranslator.h"

namespace llvm {
class Module;
}

namespace Ice {

class Converter : public Translator {
public:
  Converter(llvm::Module *Mod, GlobalContext *Ctx, const Ice::ClFlags &Flags)
      : Translator(Ctx, Flags), Mod(Mod) {}

  /// Converts the LLVM Module to ICE. Sets exit status to false if successful,
  /// true otherwise.
  void convertToIce();

private:
  llvm::Module *Mod;
  // Converts functions to ICE, and then machine code.
  void convertFunctions();

  // Converts globals to ICE, and then machine code.
  void convertGlobals(llvm::Module *Mod);

  Converter(const Converter &) = delete;
  Converter &operator=(const Converter &) = delete;
};

} // end of namespace ICE.

#endif // SUBZERO_SRC_ICECONVERTER_H
