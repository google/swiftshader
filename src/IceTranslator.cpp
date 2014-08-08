//===- subzero/src/IceTranslator.cpp - ICE to machine code ------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the general driver class for translating ICE to
// machine code.
//
//===----------------------------------------------------------------------===//

#include "IceTranslator.h"

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceTargetLowering.h"

#include <iostream>

using namespace Ice;

Translator::~Translator() {}

void Translator::translateFcn(Ice::Cfg *Fcn) {
  Func.reset(Fcn);
  if (Ctx->getFlags().DisableInternal)
    Func->setInternal(false);
  if (Ctx->getFlags().DisableTranslation) {
    Func->dump();
  } else {
    Ice::Timer TTranslate;
    Func->translate();
    if (Ctx->getFlags().SubzeroTimingEnabled) {
      std::cerr << "[Subzero timing] Translate function "
                << Func->getFunctionName() << ": " << TTranslate.getElapsedSec()
                << " sec\n";
    }
    if (Func->hasError()) {
      std::cerr << "ICE translation error: " << Func->getError() << "\n";
      ErrorStatus = true;
    }

    Ice::Timer TEmit;
    Func->emit();
    if (Ctx->getFlags().SubzeroTimingEnabled) {
      std::cerr << "[Subzero timing] Emit function " << Func->getFunctionName()
                << ": " << TEmit.getElapsedSec() << " sec\n";
    }
  }
}

void Translator::emitConstants() {
  if (!Ctx->getFlags().DisableTranslation && Func)
    Func->getTarget()->emitConstants();
}
