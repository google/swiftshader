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
  Converter(GlobalContext *Ctx, Ice::ClFlags &Flags) : Translator(Ctx, Flags) {}
  /// Converts the LLVM Module to ICE. Returns exit status 0 if successful,
  /// Nonzero otherwise.
  int convertToIce(llvm::Module *Mod);

private:
  Converter(const Converter &) LLVM_DELETED_FUNCTION;
  Converter &operator=(const Converter &) LLVM_DELETED_FUNCTION;
};
}

#endif // SUBZERO_SRC_ICECONVERTER_H
