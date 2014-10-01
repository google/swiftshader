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
    for (auto I = Mod->global_begin(), E = Mod->global_end(); I != E; ++I)
      setValueName(I, "global", GlobalPrefix, NameIndex, errs);
  }
  const IceString &FunctionPrefix = Flags.DefaultFunctionPrefix;
  if (FunctionPrefix.empty())
    return;
  uint32_t NameIndex = 0;
  for (llvm::Function &I : *Mod)
    setValueName(&I, "function", FunctionPrefix, NameIndex, errs);
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

void Translator::convertGlobals(llvm::Module *Mod) {
  std::unique_ptr<TargetGlobalInitLowering> GlobalLowering(
      TargetGlobalInitLowering::createLowering(Ctx->getTargetArch(), Ctx));
  for (auto I = Mod->global_begin(), E = Mod->global_end(); I != E; ++I) {
    if (!I->hasInitializer())
      continue;
    const llvm::Constant *Initializer = I->getInitializer();
    IceString Name = I->getName();
    unsigned Align = I->getAlignment();
    uint64_t NumElements = 0;
    const char *Data = NULL;
    bool IsInternal = I->hasInternalLinkage();
    bool IsConst = I->isConstant();
    bool IsZeroInitializer = false;

    if (const llvm::ConstantDataArray *CDA =
            llvm::dyn_cast<llvm::ConstantDataArray>(Initializer)) {
      NumElements = CDA->getNumElements();
      assert(llvm::isa<llvm::IntegerType>(CDA->getElementType()) &&
             (llvm::cast<llvm::IntegerType>(CDA->getElementType())
                  ->getBitWidth() == 8));
      Data = CDA->getRawDataValues().data();
    } else if (llvm::isa<llvm::ConstantAggregateZero>(Initializer)) {
      if (const llvm::ArrayType *AT =
              llvm::dyn_cast<llvm::ArrayType>(Initializer->getType())) {
        assert(llvm::isa<llvm::IntegerType>(AT->getElementType()) &&
               (llvm::cast<llvm::IntegerType>(AT->getElementType())
                    ->getBitWidth() == 8));
        NumElements = AT->getNumElements();
        IsZeroInitializer = true;
      } else {
        llvm_unreachable("Unhandled constant aggregate zero type");
      }
    } else {
      llvm_unreachable("Unhandled global initializer");
    }

    GlobalLowering->lower(Name, Align, IsInternal, IsConst, IsZeroInitializer,
                          NumElements, Data,
                          Ctx->getFlags().DisableTranslation);
  }
  GlobalLowering.reset();
}
