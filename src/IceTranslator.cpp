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

void Translator::nameUnnamedGlobalAddresses(llvm::Module *Mod) {
  const IceString &GlobalPrefix = Flags.DefaultGlobalPrefix;
  if (GlobalPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  Ostream &errs = Ctx->getStrDump();
  for (auto V = Mod->global_begin(), E = Mod->global_end(); V != E; ++V) {
    if (!V->hasName()) {
      V->setName(createUnnamedName(GlobalPrefix, NameIndex));
      ++NameIndex;
    } else {
      checkIfUnnamedNameSafe(V->getName(), "global", GlobalPrefix, errs);
    }
  }
}

void Translator::nameUnnamedFunctions(llvm::Module *Mod) {
  const IceString &FunctionPrefix = Flags.DefaultFunctionPrefix;
  if (FunctionPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  Ostream &errs = Ctx->getStrDump();
  for (llvm::Function &F : *Mod) {
    if (!F.hasName()) {
      F.setName(createUnnamedName(FunctionPrefix, NameIndex));
      ++NameIndex;
    } else {
      checkIfUnnamedNameSafe(F.getName(), "function", FunctionPrefix, errs);
    }
  }
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

void Translator::lowerGlobals(const GlobalAddressList &GlobalAddresses) {
  llvm::OwningPtr<Ice::TargetGlobalInitLowering> GlobalLowering(
      Ice::TargetGlobalInitLowering::createLowering(Ctx->getTargetArch(), Ctx));
  bool DisableTranslation = Ctx->getFlags().DisableTranslation;
  for (const Ice::GlobalAddress *Addr : GlobalAddresses) {
    GlobalLowering->lower(*Addr, DisableTranslation);
  }
  GlobalLowering.reset();
}
