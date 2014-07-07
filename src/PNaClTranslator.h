//===- subzero/src/PNaClTranslator.h - ICE from bitcode ---------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the PNaCl bitcode file to ICE, to machine code
// translator.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_PNACLTRANSLATOR_H
#define SUBZERO_SRC_PNACLTRANSLATOR_H

#include "IceTranslator.h"
#include <string>

namespace Ice {

class PNaClTranslator : public Translator {
public:
  PNaClTranslator(GlobalContext *Ctx, ClFlags &Flags)
      : Translator(Ctx, Flags) {}
  // Reads the PNaCl bitcode file and translates to ICE, which is then
  // converted to machine code. Sets ExitStatus to non-zero if any
  // errors occurred.
  void translate(const std::string &IRFilename);

private:
  PNaClTranslator(const PNaClTranslator &) LLVM_DELETED_FUNCTION;
  PNaClTranslator &operator=(const PNaClTranslator &) LLVM_DELETED_FUNCTION;
};
}

#endif // SUBZERO_SRC_PNACLTRANSLATOR_H
