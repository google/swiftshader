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

#include <iostream>
#include <memory>

#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceGlobalInits.h"
#include "IceTargetLowering.h"
#include "IceTranslator.h"

using namespace Ice;

Translator::~Translator() {}

IceString Translator::createUnnamedName(const IceString &Prefix, SizeT Index) {
  if (Index == 0)
    return Prefix;
  std::string Buffer;
  llvm::raw_string_ostream StrBuf(Buffer);
  StrBuf << Prefix << Index;
  return StrBuf.str();
}

bool Translator::checkIfUnnamedNameSafe(const IceString &Name, const char *Kind,
                                        const IceString &Prefix,
                                        Ostream &Stream) {
  if (Name.find(Prefix) == 0) {
    for (size_t i = Prefix.size(); i < Name.size(); ++i) {
      if (!isdigit(Name[i])) {
        return false;
      }
    }
    Stream << "Warning : Default " << Kind << " prefix '" << Prefix
           << "' potentially conflicts with name '" << Name << "'.\n";
    return true;
  }
  return false;
}

void Translator::translateFcn(Cfg *Fcn) {
  Ctx->resetStats();
  Func.reset(Fcn);
  if (Ctx->getFlags().DisableInternal)
    Func->setInternal(false);
  if (Ctx->getFlags().DisableTranslation) {
    Func->dump();
  } else {
    Func->translate();
    if (Func->hasError()) {
      std::cerr << "ICE translation error: " << Func->getError() << "\n";
      ErrorStatus = true;
    }

    Func->emit();
    Ctx->dumpStats(Func->getFunctionName());
  }
}

void Translator::emitConstants() {
  if (!Ctx->getFlags().DisableTranslation && Func)
    Func->getTarget()->emitConstants();
}

void Translator::lowerGlobals(
    const VariableDeclarationListType &VariableDeclarations) {
  llvm::OwningPtr<TargetGlobalInitLowering> GlobalLowering(
      TargetGlobalInitLowering::createLowering(Ctx->getTargetArch(), Ctx));
  bool DisableTranslation = Ctx->getFlags().DisableTranslation;
  bool DumpGlobalVariables = Ctx->isVerbose();
  Ostream &Stream = Ctx->getStrDump();
  for (const Ice::VariableDeclaration *Global : VariableDeclarations) {
    if (DumpGlobalVariables)
      Global->dump(getContext(), Stream);
    if(!DisableTranslation)
      GlobalLowering->lower(*Global);
  }
  GlobalLowering.reset();
}
