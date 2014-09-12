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
#include "IceDefs.h"
#include "IceTargetLowering.h"
#include "llvm/IR/Module.h"

#include <iostream>

using namespace Ice;

Translator::~Translator() {}

namespace {
void setValueName(llvm::Value *V, const char *Kind, const IceString &Prefix,
                  uint32_t &NameIndex, Ostream &errs) {
  if (V->hasName()) {
    const std::string &Name(V->getName());
    if (Name.find(Prefix) == 0) {
      errs << "Warning: Default " << Kind << " prefix '" << Prefix
           << "' conflicts with name '" << Name << "'.\n";
    }
    return;
  }
  if (NameIndex == 0) {
    V->setName(Prefix);
    ++NameIndex;
    return;
  }
  std::string Buffer;
  llvm::raw_string_ostream StrBuf(Buffer);
  StrBuf << Prefix << NameIndex;
  V->setName(StrBuf.str());
  ++NameIndex;
}
} // end of anonymous namespace

void Translator::nameUnnamedGlobalAddresses(llvm::Module *Mod) {
  const IceString &GlobalPrefix = Flags.DefaultGlobalPrefix;
  Ostream &errs = Ctx->getStrDump();
  if (!GlobalPrefix.empty()) {
    uint32_t NameIndex = 0;
    for (llvm::Module::global_iterator I = Mod->global_begin(),
                                       E = Mod->global_end();
         I != E; ++I) {
      setValueName(I, "global", GlobalPrefix, NameIndex, errs);
    }
  }
  const IceString &FunctionPrefix = Flags.DefaultFunctionPrefix;
  if (FunctionPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  for (llvm::Module::iterator I = Mod->begin(), E = Mod->end(); I != E; ++I) {
    setValueName(I, "function", FunctionPrefix, NameIndex, errs);
  }
}

void Translator::translateFcn(Cfg *Fcn) {
  Func.reset(Fcn);
  if (Ctx->getFlags().DisableInternal)
    Func->setInternal(false);
  if (Ctx->getFlags().DisableTranslation) {
    Func->dump();
  } else {
    Timer TTranslate;
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

    Timer TEmit;
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
