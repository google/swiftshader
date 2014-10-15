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

#include <string>

#include "IceTranslator.h"

namespace Ice {

class PNaClTranslator : public Translator {
  PNaClTranslator(const PNaClTranslator &) = delete;
  PNaClTranslator &operator=(const PNaClTranslator &) = delete;

public:
  PNaClTranslator(GlobalContext *Ctx, const ClFlags &Flags)
      : Translator(Ctx, Flags) {}
  // Reads the PNaCl bitcode file and translates to ICE, which is then
  // converted to machine code. Sets ErrorStatus to true if any
  // errors occurred.
  void translate(const std::string &IRFilename);
};

} // end of namespace Ice

#endif // SUBZERO_SRC_PNACLTRANSLATOR_H
