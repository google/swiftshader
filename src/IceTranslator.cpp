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

namespace {

// Match a symbol name against a match string.  An empty match string
// means match everything.  Returns true if there is a match.
bool matchSymbolName(const IceString &SymbolName, const IceString &Match) {
  return Match.empty() || Match == SymbolName;
}

} // end of anonymous namespace

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
  VerboseMask OldVerboseMask = Ctx->getVerbose();
  if (!matchSymbolName(Func->getFunctionName(), Ctx->getFlags().VerboseFocusOn))
    Ctx->setVerbose(IceV_None);

  if (Ctx->getFlags().DisableTranslation ||
      !matchSymbolName(Func->getFunctionName(),
                       Ctx->getFlags().TranslateOnly)) {
    Func->dump();
  } else {
    Func->translate();
    if (Func->hasError()) {
      std::cerr << "ICE translation error: " << Func->getError() << "\n";
      ErrorStatus = true;
    }

    if (!ErrorStatus) {
      if (Ctx->getFlags().UseIntegratedAssembler) {
        Func->emitIAS();
      } else {
        Func->emit();
      }
    }
    Ctx->dumpStats(Func->getFunctionName());
  }

  Ctx->setVerbose(OldVerboseMask);
}

void Translator::emitConstants() {
  if (!Ctx->getFlags().DisableTranslation && Func)
    Func->getTarget()->emitConstants();
}

void Translator::lowerGlobals(
    const VariableDeclarationListType &VariableDeclarations) {
  std::unique_ptr<TargetGlobalInitLowering> GlobalLowering(
      TargetGlobalInitLowering::createLowering(Ctx->getTargetArch(), Ctx));
  bool DisableTranslation = Ctx->getFlags().DisableTranslation;
  const bool DumpGlobalVariables =
      ALLOW_DUMP && Ctx->isVerbose() && Ctx->getFlags().VerboseFocusOn.empty();
  Ostream &Stream = Ctx->getStrDump();
  const IceString &TranslateOnly = Ctx->getFlags().TranslateOnly;
  for (const Ice::VariableDeclaration *Global : VariableDeclarations) {
    if (DumpGlobalVariables)
      Global->dump(getContext(), Stream);
    if (!DisableTranslation &&
        matchSymbolName(Global->getName(), TranslateOnly))
      GlobalLowering->lower(*Global);
  }
  GlobalLowering.reset();
}
