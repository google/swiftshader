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

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceGlobalInits.h"
#include "IceTargetLowering.h"
#include "IceTranslator.h"

using namespace Ice;

Translator::Translator(GlobalContext *Ctx, const ClFlags &Flags)
    : Ctx(Ctx), Flags(Flags),
      GlobalLowering(TargetGlobalLowering::createLowering(Ctx)), ErrorStatus() {
}

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
                                        const IceString &Prefix) {
  if (Name.find(Prefix) == 0) {
    for (size_t i = Prefix.size(); i < Name.size(); ++i) {
      if (!isdigit(Name[i])) {
        return false;
      }
    }
    OstreamLocker L(Ctx);
    Ostream &Stream = Ctx->getStrDump();
    Stream << "Warning : Default " << Kind << " prefix '" << Prefix
           << "' potentially conflicts with name '" << Name << "'.\n";
    return true;
  }
  return false;
}

void Translator::translateFcn(Cfg *Func) {
  Ctx->cfgQueueBlockingPush(Func);
  if (Ctx->getFlags().NumTranslationThreads == 0) {
    Ctx->translateFunctions();
  }
}

void Translator::emitConstants() {
  if (!getErrorStatus())
    GlobalLowering->lowerConstants(Ctx);
}

void Translator::transferErrorCode() const {
  if (getErrorStatus())
    Ctx->getErrorStatus()->assign(getErrorStatus().value());
}

void Translator::lowerGlobals(
    const VariableDeclarationListType &VariableDeclarations) {
  bool DisableTranslation = Ctx->getFlags().DisableTranslation;
  const bool DumpGlobalVariables =
      ALLOW_DUMP && Ctx->getVerbose() && Ctx->getFlags().VerboseFocusOn.empty();
  OstreamLocker L(Ctx);
  Ostream &Stream = Ctx->getStrDump();
  const IceString &TranslateOnly = Ctx->getFlags().TranslateOnly;
  for (const Ice::VariableDeclaration *Global : VariableDeclarations) {
    if (DumpGlobalVariables)
      Global->dump(getContext(), Stream);
    if (!DisableTranslation &&
        GlobalContext::matchSymbolName(Global->getName(), TranslateOnly))
      GlobalLowering->lowerInit(*Global);
  }
}
