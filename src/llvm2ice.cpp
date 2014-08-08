//===- subzero/src/llvm2ice.cpp - Driver for testing ----------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a driver that uses LLVM capabilities to parse a
// bitcode file and build the LLVM IR, and then convert the LLVM basic
// blocks, instructions, and operands into their Subzero equivalents.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceConverter.h"
#include "PNaClTranslator.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"

#include <fstream>
#include <iostream>

using namespace llvm;

static cl::list<Ice::VerboseItem> VerboseList(
    "verbose", cl::CommaSeparated,
    cl::desc("Verbose options (can be comma-separated):"),
    cl::values(
        clEnumValN(Ice::IceV_Instructions, "inst", "Print basic instructions"),
        clEnumValN(Ice::IceV_Deleted, "del", "Include deleted instructions"),
        clEnumValN(Ice::IceV_InstNumbers, "instnum",
                   "Print instruction numbers"),
        clEnumValN(Ice::IceV_Preds, "pred", "Show predecessors"),
        clEnumValN(Ice::IceV_Succs, "succ", "Show successors"),
        clEnumValN(Ice::IceV_Liveness, "live", "Liveness information"),
        clEnumValN(Ice::IceV_RegManager, "rmgr", "Register manager status"),
        clEnumValN(Ice::IceV_RegOrigins, "orig", "Physical register origins"),
        clEnumValN(Ice::IceV_LinearScan, "regalloc", "Linear scan details"),
        clEnumValN(Ice::IceV_Frame, "frame", "Stack frame layout details"),
        clEnumValN(Ice::IceV_Timing, "time", "Pass timing details"),
        clEnumValN(Ice::IceV_All, "all", "Use all verbose options"),
        clEnumValN(Ice::IceV_None, "none", "No verbosity"), clEnumValEnd));
static cl::opt<Ice::TargetArch> TargetArch(
    "target", cl::desc("Target architecture:"), cl::init(Ice::Target_X8632),
    cl::values(
        clEnumValN(Ice::Target_X8632, "x8632", "x86-32"),
        clEnumValN(Ice::Target_X8632, "x86-32", "x86-32 (same as x8632)"),
        clEnumValN(Ice::Target_X8632, "x86_32", "x86-32 (same as x8632)"),
        clEnumValN(Ice::Target_X8664, "x8664", "x86-64"),
        clEnumValN(Ice::Target_X8664, "x86-64", "x86-64 (same as x8664)"),
        clEnumValN(Ice::Target_X8664, "x86_64", "x86-64 (same as x8664)"),
        clEnumValN(Ice::Target_ARM32, "arm", "arm32"),
        clEnumValN(Ice::Target_ARM32, "arm32", "arm32 (same as arm)"),
        clEnumValN(Ice::Target_ARM64, "arm64", "arm64"), clEnumValEnd));
static cl::opt<bool>
    FunctionSections("ffunction-sections",
                     cl::desc("Emit functions into separate sections"));
static cl::opt<Ice::OptLevel>
OptLevel(cl::desc("Optimization level"), cl::init(Ice::Opt_m1),
         cl::value_desc("level"),
         cl::values(clEnumValN(Ice::Opt_m1, "Om1", "-1"),
                    clEnumValN(Ice::Opt_m1, "O-1", "-1"),
                    clEnumValN(Ice::Opt_0, "O0", "0"),
                    clEnumValN(Ice::Opt_1, "O1", "1"),
                    clEnumValN(Ice::Opt_2, "O2", "2"), clEnumValEnd));
static cl::opt<std::string> IRFilename(cl::Positional, cl::desc("<IR file>"),
                                       cl::init("-"));
static cl::opt<std::string> OutputFilename("o",
                                           cl::desc("Override output filename"),
                                           cl::init("-"),
                                           cl::value_desc("filename"));
static cl::opt<std::string> LogFilename("log", cl::desc("Set log filename"),
                                        cl::init("-"),
                                        cl::value_desc("filename"));
static cl::opt<std::string>
TestPrefix("prefix", cl::desc("Prepend a prefix to symbol names for testing"),
           cl::init(""), cl::value_desc("prefix"));
static cl::opt<bool>
DisableInternal("external",
                cl::desc("Disable 'internal' linkage type for testing"));
static cl::opt<bool>
DisableTranslation("notranslate", cl::desc("Disable Subzero translation"));

static cl::opt<bool> SubzeroTimingEnabled(
    "timing", cl::desc("Enable breakdown timing of Subzero translation"));

static cl::opt<bool>
    DisableGlobals("disable-globals",
                   cl::desc("Disable global initializer translation"));

static cl::opt<NaClFileFormat> InputFileFormat(
    "bitcode-format", cl::desc("Define format of input file:"),
    cl::values(clEnumValN(LLVMFormat, "llvm", "LLVM file (default)"),
               clEnumValN(PNaClFormat, "pnacl", "PNaCl bitcode file"),
               clEnumValEnd),
    cl::init(LLVMFormat));

static cl::opt<bool>
BuildOnRead("build-on-read",
            cl::desc("Build ICE instructions when reading bitcode"),
            cl::init(false));

int main(int argc, char **argv) {

  cl::ParseCommandLineOptions(argc, argv);

  Ice::VerboseMask VMask = Ice::IceV_None;
  for (unsigned i = 0; i != VerboseList.size(); ++i)
    VMask |= VerboseList[i];

  std::ofstream Ofs;
  if (OutputFilename != "-") {
    Ofs.open(OutputFilename.c_str(), std::ofstream::out);
  }
  raw_os_ostream *Os =
      new raw_os_ostream(OutputFilename == "-" ? std::cout : Ofs);
  Os->SetUnbuffered();
  std::ofstream Lfs;
  if (LogFilename != "-") {
    Lfs.open(LogFilename.c_str(), std::ofstream::out);
  }
  raw_os_ostream *Ls = new raw_os_ostream(LogFilename == "-" ? std::cout : Lfs);
  Ls->SetUnbuffered();

  Ice::ClFlags Flags;
  Flags.DisableInternal = DisableInternal;
  Flags.SubzeroTimingEnabled = SubzeroTimingEnabled;
  Flags.DisableTranslation = DisableTranslation;
  Flags.DisableGlobals = DisableGlobals;
  Flags.FunctionSections = FunctionSections;

  Ice::GlobalContext Ctx(Ls, Os, VMask, TargetArch, OptLevel, TestPrefix,
                         Flags);

  if (BuildOnRead) {
    Ice::PNaClTranslator Translator(&Ctx);
    Translator.translate(IRFilename);
    return Translator.getErrorStatus();
  } else {
    // Parse the input LLVM IR file into a module.
    SMDiagnostic Err;
    Ice::Timer T;
    Module *Mod =
        NaClParseIRFile(IRFilename, InputFileFormat, Err, getGlobalContext());

    if (SubzeroTimingEnabled) {
      std::cerr << "[Subzero timing] IR Parsing: " << T.getElapsedSec()
                << " sec\n";
    }

    if (!Mod) {
      Err.print(argv[0], errs());
      return 1;
    }

    Ice::Converter Converter(&Ctx);
    Converter.convertToIce(Mod);
    return Converter.getErrorStatus();
  }
}
