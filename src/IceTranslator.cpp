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
      DataLowering(TargetDataLowering::createLowering(Ctx)), ErrorStatus() {}

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

void Translator::translateFcn(std::unique_ptr<Cfg> Func) {
  Ctx->cfgQueueBlockingPush(std::move(Func));
  if (Ctx->getFlags().getNumTranslationThreads() == 0) {
    Ctx->translateFunctions();
  }
}

void Translator::emitConstants() {
  if (!getErrorStatus())
    DataLowering->lowerConstants(Ctx);
}

void Translator::transferErrorCode() const {
  if (getErrorStatus())
    Ctx->getErrorStatus()->assign(getErrorStatus().value());
}

void
Translator::lowerGlobals(const VariableDeclarationList &VariableDeclarations) {
  TimerMarker T(TimerStack::TT_emitGlobalInitializers, Ctx);
  bool DisableTranslation = Ctx->getFlags().getDisableTranslation();
  const bool DumpGlobalVariables = ALLOW_DUMP && Ctx->getVerbose() &&
                                   Ctx->getFlags().getVerboseFocusOn().empty();
  if (Ctx->getFlags().getUseELFWriter()) {
    // Dump all globals if requested, but don't interleave w/ emission.
    if (DumpGlobalVariables) {
      OstreamLocker L(Ctx);
      Ostream &Stream = Ctx->getStrDump();
      for (const Ice::VariableDeclaration *Global : VariableDeclarations) {
        Global->dump(getContext(), Stream);
      }
    }
    DataLowering->lowerGlobalsELF(VariableDeclarations);
  } else {
    const IceString &TranslateOnly = Ctx->getFlags().getTranslateOnly();
    OstreamLocker L(Ctx);
    Ostream &Stream = Ctx->getStrDump();
    for (const Ice::VariableDeclaration *Global : VariableDeclarations) {
      // Interleave dump output w/ emit output.
      if (DumpGlobalVariables)
        Global->dump(getContext(), Stream);
      if (!DisableTranslation &&
          GlobalContext::matchSymbolName(Global->getName(), TranslateOnly))
        DataLowering->lowerGlobal(*Global);
    }
  }
}
