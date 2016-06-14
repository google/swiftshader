//===- subzero/src/IceASanInstrumentation.cpp - ASan ------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the AddressSanitizer instrumentation class.
///
//===----------------------------------------------------------------------===//

#include "IceASanInstrumentation.h"

#include "IceBuildDefs.h"
#include "IceGlobalInits.h"

#include <sstream>

namespace Ice {

namespace {
constexpr SizeT RzSize = 32;
const std::string RzPrefix = "__$rz";
const llvm::NaClBitcodeRecord::RecordVector RzContents =
    llvm::NaClBitcodeRecord::RecordVector(RzSize, 'R');
} // end of anonymous namespace

// Create redzones around all global variables, ensuring that the initializer
// types of the redzones and their associated globals match so that they are
// laid out together in memory.
void ASanInstrumentation::instrumentGlobals(VariableDeclarationList &Globals) {
  if (DidInsertRedZones)
    return;

  VariableDeclarationList NewGlobals;
  // Global holding pointers to all redzones
  auto *RzArray = VariableDeclaration::create(&NewGlobals);
  // Global holding the size of RzArray
  auto *RzArraySizeVar = VariableDeclaration::create(&NewGlobals);
  SizeT RzArraySize = 0;

  RzArray->setName(Ctx, nextRzName());
  RzArraySizeVar->setName(Ctx, nextRzName());
  RzArray->setIsConstant(true);
  RzArraySizeVar->setIsConstant(true);
  NewGlobals.push_back(RzArray);
  NewGlobals.push_back(RzArraySizeVar);

  for (VariableDeclaration *Global : Globals) {
    VariableDeclaration *RzLeft =
        createRz(&NewGlobals, RzArray, RzArraySize, Global);
    VariableDeclaration *RzRight =
        createRz(&NewGlobals, RzArray, RzArraySize, Global);
    NewGlobals.push_back(RzLeft);
    NewGlobals.push_back(Global);
    NewGlobals.push_back(RzRight);
  }

  // update the contents of the RzArraySize global
  llvm::NaClBitcodeRecord::RecordVector SizeContents;
  for (unsigned i = 0; i < sizeof(RzArraySize); i++) {
    SizeContents.emplace_back(RzArraySize % (1 << CHAR_BIT));
    RzArraySize >>= CHAR_BIT;
  }
  RzArraySizeVar->addInitializer(
      VariableDeclaration::DataInitializer::create(&NewGlobals, SizeContents));

  // Replace old list of globals, without messing up arena allocators
  Globals.clear();
  Globals.merge(&NewGlobals);
  DidInsertRedZones = true;

  // Log the new set of globals
  if (BuildDefs::dump() && (getFlags().getVerbose() & IceV_GlobalInit)) {
    OstreamLocker _(Ctx);
    Ctx->getStrDump() << "========= Instrumented Globals =========\n";
    for (VariableDeclaration *Global : Globals) {
      Global->dump(Ctx->getStrDump());
    }
  }
}

std::string ASanInstrumentation::nextRzName() {
  std::stringstream Name;
  Name << RzPrefix << RzNum++;
  return Name.str();
}

VariableDeclaration *
ASanInstrumentation::createRz(VariableDeclarationList *List,
                              VariableDeclaration *RzArray, SizeT &RzArraySize,
                              VariableDeclaration *Global) {
  auto *Rz = VariableDeclaration::create(List);
  Rz->setName(Ctx, nextRzName());
  if (Global->hasNonzeroInitializer()) {
    Rz->addInitializer(
        VariableDeclaration::DataInitializer::create(List, RzContents));
  } else {
    Rz->addInitializer(
        VariableDeclaration::ZeroInitializer::create(List, RzSize));
  }
  Rz->setIsConstant(Global->getIsConstant());
  RzArray->addInitializer(VariableDeclaration::RelocInitializer::create(
      List, Rz, RelocOffsetArray(0)));
  ++RzArraySize;
  return Rz;
}

} // end of namespace Ice
