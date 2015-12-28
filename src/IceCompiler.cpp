//===- subzero/src/IceCompiler.cpp - Driver for bitcode translation -------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines a driver for translating PNaCl bitcode into native code.
///
/// The driver can either directly parse the binary bitcode file, or use LLVM
/// routines to parse a textual bitcode file into LLVM IR and then convert LLVM
/// IR into ICE. In either case, the high-level ICE is then compiled down to
/// native code, as either an ELF object file or a textual asm file.
///
//===----------------------------------------------------------------------===//

#include "IceCompiler.h"

#include "IceBuildDefs.h"
#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceClFlagsExtra.h"
#include "IceConverter.h"
#include "IceELFObjectWriter.h"
#include "PNaClTranslator.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Bitcode/NaCl/NaClReaderWriter.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/StreamingMemoryObject.h"

#include <regex>

#pragma clang diagnostic pop

namespace Ice {

namespace {

struct {
  const char *FlagName;
  bool FlagValue;
} ConditionalBuildAttributes[] = {
    {"dump", BuildDefs::dump()},
    {"llvm_cl", BuildDefs::llvmCl()},
    {"llvm_ir", BuildDefs::llvmIr()},
    {"llvm_ir_as_input", BuildDefs::llvmIrAsInput()},
    {"minimal_build", BuildDefs::minimal()},
    {"browser_mode", BuildDefs::browser()}};

/// Dumps values of build attributes to Stream if Stream is non-null.
void dumpBuildAttributes(Ostream &Str) {
// List the supported targets.
#define SUBZERO_TARGET(TARGET) Str << "target_" #TARGET << "\n";
#include "llvm/Config/SZTargets.def"
  const char *Prefix[2] = {"no", "allow"};
  for (size_t i = 0; i < llvm::array_lengthof(ConditionalBuildAttributes);
       ++i) {
    const auto &A = ConditionalBuildAttributes[i];
    Str << Prefix[A.FlagValue] << "_" << A.FlagName << "\n";
  }
}
bool llvmIRInput(const IceString &Filename) {
  return BuildDefs::llvmIrAsInput() &&
         std::regex_match(Filename, std::regex(".*\\.ll"));
}

} // end of anonymous namespace

void Compiler::run(const Ice::ClFlagsExtra &ExtraFlags, GlobalContext &Ctx,
                   std::unique_ptr<llvm::DataStreamer> &&InputStream) {
  if (ExtraFlags.getGenerateBuildAtts()) {
    dumpBuildAttributes(Ctx.getStrDump());
    Ctx.getErrorStatus()->assign(EC_None);
    return;
  }
  // The Minimal build (specifically, when dump()/emit() are not implemented)
  // allows only --filetype=obj. Check here to avoid cryptic error messages
  // downstream.
  if (!BuildDefs::dump() && Ctx.getFlags().getOutFileType() != FT_Elf) {
    // TODO(stichnot): Access the actual command-line argument via
    // llvm::Option.ArgStr and .ValueStr .
    Ctx.getStrError()
        << "Error: only --filetype=obj is supported in this build.\n";
    Ctx.getErrorStatus()->assign(EC_Args);
    return;
  }

  TimerMarker T(Ice::TimerStack::TT_szmain, &Ctx);

  Ctx.emitFileHeader();
  Ctx.startWorkerThreads();

  std::unique_ptr<Translator> Translator;
  const IceString &IRFilename = ExtraFlags.getIRFilename();
  bool BuildOnRead = ExtraFlags.getBuildOnRead() && !llvmIRInput(IRFilename);
  if (BuildOnRead) {
    std::unique_ptr<PNaClTranslator> PTranslator(new PNaClTranslator(&Ctx));
    std::unique_ptr<llvm::StreamingMemoryObject> MemObj(
        new llvm::StreamingMemoryObjectImpl(InputStream.release()));
    PTranslator->translate(IRFilename, std::move(MemObj));
    Translator.reset(PTranslator.release());
  } else if (BuildDefs::llvmIr()) {
    if (BuildDefs::browser()) {
      Ctx.getStrError()
          << "non BuildOnRead is not supported w/ PNACL_BROWSER_TRANSLATOR\n";
      Ctx.getErrorStatus()->assign(EC_Args);
      return;
    }
    // Parse the input LLVM IR file into a module.
    llvm::SMDiagnostic Err;
    TimerMarker T1(Ice::TimerStack::TT_parse, &Ctx);
    llvm::DiagnosticHandlerFunction DiagnosticHandler =
        ExtraFlags.getLLVMVerboseErrors()
            ? redirectNaClDiagnosticToStream(llvm::errs())
            : nullptr;
    std::unique_ptr<llvm::Module> Mod =
        NaClParseIRFile(IRFilename, ExtraFlags.getInputFileFormat(), Err,
                        llvm::getGlobalContext(), DiagnosticHandler);
    if (!Mod) {
      Err.print(ExtraFlags.getAppName().c_str(), llvm::errs());
      Ctx.getErrorStatus()->assign(EC_Bitcode);
      return;
    }

    std::unique_ptr<Converter> Converter(new class Converter(Mod.get(), &Ctx));
    Converter->convertToIce();
    Translator.reset(Converter.release());
  } else {
    Ctx.getStrError() << "Error: Build doesn't allow LLVM IR, "
                      << "--build-on-read=0 not allowed\n";
    Ctx.getErrorStatus()->assign(EC_Args);
    return;
  }

  Ctx.waitForWorkerThreads();
  if (Translator->getErrorStatus()) {
    Ctx.getErrorStatus()->assign(Translator->getErrorStatus().value());
  } else {
    Ctx.lowerGlobals("last");
    Ctx.lowerProfileData();
    Ctx.lowerConstants();
    Ctx.lowerJumpTables();

    if (Ctx.getFlags().getOutFileType() == FT_Elf) {
      TimerMarker T1(Ice::TimerStack::TT_emit, &Ctx);
      Ctx.getObjectWriter()->setUndefinedSyms(Ctx.getConstantExternSyms());
      Ctx.getObjectWriter()->writeNonUserSections();
    }
  }

  if (Ctx.getFlags().getSubzeroTimingEnabled())
    Ctx.dumpTimers();

  if (Ctx.getFlags().getTimeEachFunction()) {
    constexpr bool DumpCumulative = false;
    Ctx.dumpTimers(GlobalContext::TSK_Funcs, DumpCumulative);
  }
  constexpr bool FinalStats = true;
  Ctx.dumpStats("_FINAL_", FinalStats);
}

} // end of namespace Ice
