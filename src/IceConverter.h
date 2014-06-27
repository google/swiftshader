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

#include "IceGlobalContext.h"

namespace llvm {
class Module;
}

namespace Ice {

class Converter {
public:
  Converter(Ice::GlobalContext *Ctx,
            bool DisableInternal,
            bool SubzeroTimingEnabled,
            bool DisableTranslation)
      : Ctx(Ctx),
        DisableInternal(DisableInternal),
        SubzeroTimingEnabled(SubzeroTimingEnabled),
        DisableTranslation(DisableTranslation)
  {}
  /// Converts the LLVM Module to ICE. Returns exit status 0 if successful,
  /// Nonzero otherwise.
  int convertToIce(llvm::Module *Mod);
private:
  Ice::GlobalContext *Ctx;
  bool DisableInternal;
  bool SubzeroTimingEnabled;
  bool DisableTranslation;
};

}

#endif  // SUBZERO_SRC_ICECONVERTER_H
