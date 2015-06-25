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
#include "IceGlobalInits.h"
#include "IceTargetLowering.h"

using namespace Ice;

Translator::Translator(GlobalContext *Ctx)
    : Ctx(Ctx), NextSequenceNumber(GlobalContext::getFirstSequenceNumber()),
      ErrorStatus() {}

Translator::~Translator() = default;

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
  Ctx->optQueueBlockingPush(std::move(Func));
}

void Translator::lowerGlobals(
    std::unique_ptr<VariableDeclarationList> VariableDeclarations) {
  EmitterWorkItem *Item = new EmitterWorkItem(getNextSequenceNumber(),
                                              VariableDeclarations.release());
  Ctx->emitQueueBlockingPush(Item);
}
