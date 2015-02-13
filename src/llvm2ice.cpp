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

#include <fstream>
#include <iostream>

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"

#include "IceCfg.h"
#include "IceClFlags.h"
#include "IceConverter.h"
#include "IceELFObjectWriter.h"
#include "IceELFStreamer.h"
#include "PNaClTranslator.h"

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
        clEnumValN(Ice::IceV_RegOrigins, "orig", "Physical register origins"),
        clEnumValN(Ice::IceV_LinearScan, "regalloc", "Linear scan details"),
        clEnumValN(Ice::IceV_Frame, "frame", "Stack frame layout details"),
        clEnumValN(Ice::IceV_AddrOpt, "addropt", "Address mode optimization"),
        clEnumValN(Ice::IceV_Random, "random", "Randomization details"),
        clEnumValN(Ice::IceV_All, "all", "Use all verbose options"),
        clEnumValN(Ice::IceV_Most, "most",
                   "Use all verbose options except 'regalloc' and 'time'"),
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
static cl::opt<bool> UseSandboxing("sandbox", cl::desc("Use sandboxing"));
static cl::opt<bool>
    FunctionSections("ffunction-sections",
                     cl::desc("Emit functions into separate sections"));
static cl::opt<bool>
    DataSections("fdata-sections",
                 cl::desc("Emit (global) data into separate sections"));
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
    TestPrefix("prefix",
               cl::desc("Prepend a prefix to symbol names for testing"),
               cl::init(""), cl::value_desc("prefix"));
static cl::opt<bool> DisableInternal("externalize",
                                     cl::desc("Externalize all symbols"));
static cl::opt<bool>
    DisableTranslation("notranslate", cl::desc("Disable Subzero translation"));
// Note: Modifiable only if ALLOW_DISABLE_IR_GEN.
static cl::opt<bool>
    DisableIRGeneration("no-ir-gen",
                        cl::desc("Disable generating Subzero IR."));
static cl::opt<std::string>
    TranslateOnly("translate-only",
                  cl::desc("Translate only the given function"), cl::init(""));

static cl::opt<bool> SubzeroTimingEnabled(
    "timing", cl::desc("Enable breakdown timing of Subzero translation"));

static cl::opt<bool> TimeEachFunction(
    "timing-funcs", cl::desc("Print total translation time for each function"));

static cl::opt<std::string> TimingFocusOn(
    "timing-focus",
    cl::desc("Break down timing for a specific function (use '*' for all)"),
    cl::init(""));

static cl::opt<std::string> VerboseFocusOn(
    "verbose-focus",
    cl::desc("Temporarily enable full verbosity for a specific function"),
    cl::init(""));

static cl::opt<bool>
    EnablePhiEdgeSplit("phi-edge-split",
                       cl::desc("Enable edge splitting for Phi lowering"),
                       cl::init(true));

static cl::opt<bool> DecorateAsm(
    "asm-verbose",
    cl::desc("Decorate textual asm output with register liveness info"));

static cl::opt<bool>
    DumpStats("szstats",
              cl::desc("Print statistics after translating each function"));

// This is currently needed by crosstest.py.
static cl::opt<bool> AllowUninitializedGlobals(
    "allow-uninitialized-globals",
    cl::desc("Allow global variables to be uninitialized"));

static cl::opt<NaClFileFormat> InputFileFormat(
    "bitcode-format", cl::desc("Define format of input file:"),
    cl::values(clEnumValN(LLVMFormat, "llvm", "LLVM file (default)"),
               clEnumValN(PNaClFormat, "pnacl", "PNaCl bitcode file"),
               clEnumValEnd),
    cl::init(LLVMFormat));

static cl::opt<std::string>
    DefaultGlobalPrefix("default-global-prefix",
                        cl::desc("Define default global prefix for naming "
                                 "unnamed globals"),
                        cl::init("Global"));

static cl::opt<std::string>
    DefaultFunctionPrefix("default-function-prefix",
                          cl::desc("Define default function prefix for naming "
                                   "unnamed functions"),
                          cl::init("Function"));

// Note: While this flag isn't used in the minimal build, we keep this
// flag so that tests can set this command-line flag without concern
// to the type of build. We double check that this flag at runtime
// to make sure the consistency is maintained.
static cl::opt<bool>
    BuildOnRead("build-on-read",
                cl::desc("Build ICE instructions when reading bitcode"),
                cl::init(true));

static cl::opt<bool> AllowErrorRecovery(
    "allow-pnacl-reader-error-recovery",
    cl::desc("Allow error recovery when reading PNaCl bitcode."),
    cl::init(false));

// TODO(kschimpf) Remove once the emitter handles these cases.
static cl::opt<bool>
    StubConstantCalls("stub-const-calls",
                      cl::desc("Stub indirect calls to constants."),
                      cl::init(false));

static cl::opt<bool> LLVMVerboseErrors(
    "verbose-llvm-parse-errors",
    cl::desc("Print out more descriptive PNaCl bitcode parse errors when "
             "building LLVM IR first"),
    cl::init(false));

static cl::opt<Ice::FileType> OutFileType(
    "filetype", cl::desc("Output file type"), cl::init(Ice::FT_Iasm),
    cl::values(clEnumValN(Ice::FT_Elf, "obj", "Native ELF object ('.o') file"),
               clEnumValN(Ice::FT_Asm, "asm", "Assembly ('.s') file"),
               clEnumValN(Ice::FT_Iasm, "iasm",
                          "Low-level integrated assembly ('.s') file"),
               clEnumValEnd));

static cl::opt<bool> AlwaysExitSuccess(
    "exit-success", cl::desc("Exit with success status, even if errors found"),
    cl::init(false));

static cl::opt<bool> GenerateBuildAtts(
    "build-atts", cl::desc("Generate list of build attributes associated with "
                           "this executable."),
    cl::init(false));

// Number of translation threads (in addition to the parser thread and
// the emitter thread).  The special case of 0 means purely
// sequential, i.e. parser, translator, and emitter all within the
// same single thread.  (This may need a slight rework if we expand to
// multiple parser or emitter threads.)
static cl::opt<uint32_t> NumThreads(
    "threads",
    cl::desc("Number of translation threads (0 for purely sequential)"),
    // TODO(stichnot): Settle on a good default.  Consider
    // something related to std::thread::hardware_concurrency().
    cl::init(2));

static int GetReturnValue(int Val) {
  if (AlwaysExitSuccess)
    return 0;
  return Val;
}

static struct {
  const char *FlagName;
  int FlagValue;
} ConditionalBuildAttributes[] = {{"dump", ALLOW_DUMP},
                                  {"disable_ir_gen", ALLOW_DISABLE_IR_GEN},
                                  {"llvm_cl", ALLOW_LLVM_CL},
                                  {"llvm_ir", ALLOW_LLVM_IR},
                                  {"llvm_ir_as_input", ALLOW_LLVM_IR_AS_INPUT},
                                  {"minimal_build", ALLOW_MINIMAL_BUILD}};

// Validates values of build attributes. Prints them to Stream if
// Stream is non-null.
static void ValidateAndGenerateBuildAttributes(Ice::Ostream *Stream) {

  if (Stream)
    *Stream << TargetArch << "\n";

  for (size_t i = 0; i < array_lengthof(ConditionalBuildAttributes); ++i) {
    switch (ConditionalBuildAttributes[i].FlagValue) {
    case 0:
      if (Stream)
        *Stream << "no_" << ConditionalBuildAttributes[i].FlagName << "\n";
      break;
    case 1:
      if (Stream)
        *Stream << "allow_" << ConditionalBuildAttributes[i].FlagName << "\n";
      break;
    default: {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Flag " << ConditionalBuildAttributes[i].FlagName
             << " must be defined as 0/1. Found: "
             << ConditionalBuildAttributes[i].FlagValue;
      report_fatal_error(StrBuf.str());
    }
    }
  }
}

int main(int argc, char **argv) {

  cl::ParseCommandLineOptions(argc, argv);

  if (DisableIRGeneration)
    DisableTranslation = true;

  Ice::VerboseMask VMask = Ice::IceV_None;
  // Don't generate verbose messages if routines
  // to dump messages are not available.
  if (ALLOW_DUMP) {
    for (unsigned i = 0; i != VerboseList.size(); ++i)
      VMask |= VerboseList[i];
  }

  std::ofstream Lfs;
  std::unique_ptr<Ice::Ostream> Ls;
  if (LogFilename != "-") {
    Lfs.open(LogFilename.c_str(), std::ofstream::out);
    Ls.reset(new raw_os_ostream(Lfs));
  } else {
    Ls.reset(new raw_os_ostream(std::cout));
  }
  Ls->SetUnbuffered();

  ValidateAndGenerateBuildAttributes(GenerateBuildAtts ? Ls.get() : nullptr);
  if (GenerateBuildAtts)
    return GetReturnValue(Ice::EC_None);

  if (!ALLOW_DISABLE_IR_GEN && DisableIRGeneration) {
    *Ls << "Error: Build doesn't allow --no-ir-gen when not "
        << "ALLOW_DISABLE_IR_GEN!\n";
    return GetReturnValue(Ice::EC_Args);
  }

  Ice::ClFlags Flags;
  Flags.setAllowErrorRecovery(AllowErrorRecovery);
  Flags.setAllowUninitializedGlobals(AllowUninitializedGlobals);
  Flags.setDataSections(DataSections);
  Flags.setDecorateAsm(DecorateAsm);
  Flags.setDefaultFunctionPrefix(DefaultFunctionPrefix);
  Flags.setDefaultGlobalPrefix(DefaultGlobalPrefix);
  Flags.setDisableInternal(DisableInternal);
  Flags.setDisableIRGeneration(DisableIRGeneration);
  Flags.setDisableTranslation(DisableTranslation);
  Flags.setDumpStats(DumpStats);
  Flags.setFunctionSections(FunctionSections);
  Flags.setNumTranslationThreads(NumThreads);
  Flags.setPhiEdgeSplit(EnablePhiEdgeSplit);
  Flags.setStubConstantCalls(StubConstantCalls);
  Flags.setSubzeroTimingEnabled(SubzeroTimingEnabled);
  Flags.setTimeEachFunction(TimeEachFunction);
  Flags.setTimingFocusOn(TimingFocusOn);
  Flags.setTranslateOnly(TranslateOnly);
  Flags.setUseSandboxing(UseSandboxing);
  Flags.setVerboseFocusOn(VerboseFocusOn);
  Flags.setOutFileType(OutFileType);

  // Force -build-on-read=0 for .ll files.
  const std::string LLSuffix = ".ll";
  if (IRFilename.length() >= LLSuffix.length() &&
      IRFilename.compare(IRFilename.length() - LLSuffix.length(),
                         LLSuffix.length(), LLSuffix) == 0)
    BuildOnRead = false;

  // With the ELF writer, use a raw_fd_ostream to allow seeking.
  // Also don't buffer, otherwise it gets pretty slow.
  std::unique_ptr<Ice::Ostream> Os;
  std::unique_ptr<Ice::ELFStreamer> ELFStr;
  std::ofstream Ofs;
  switch (OutFileType) {
  case Ice::FT_Elf: {
    if (OutputFilename == "-") {
      *Ls << "Error: writing binary ELF to stdout is unsupported\n";
      return GetReturnValue(Ice::EC_Args);
    }
    std::string ErrorInfo;
    raw_fd_ostream *FdOs =
        new raw_fd_ostream(OutputFilename.c_str(), ErrorInfo, sys::fs::F_None);
    Os.reset(FdOs);
    if (!ErrorInfo.empty()) {
      *Ls << "Failed to open output file: " << OutputFilename << ":\n"
          << ErrorInfo << "\n";
      return GetReturnValue(Ice::EC_Args);
    }
    ELFStr.reset(new Ice::ELFStreamer(*FdOs));
  } break;
  case Ice::FT_Asm:
  case Ice::FT_Iasm: {
    if (OutputFilename != "-") {
      Ofs.open(OutputFilename.c_str(), std::ofstream::out);
      Os.reset(new raw_os_ostream(Ofs));
    } else {
      Os.reset(new raw_os_ostream(std::cout));
    }
    Os->SetUnbuffered();
  } break;
  }

  Ice::GlobalContext Ctx(Ls.get(), Os.get(), ELFStr.get(), VMask, TargetArch,
                         OptLevel, TestPrefix, Flags);

  Ice::TimerMarker T(Ice::TimerStack::TT_szmain, &Ctx);

  if (OutFileType == Ice::FT_Elf) {
    Ice::TimerMarker T1(Ice::TimerStack::TT_emit, &Ctx);
    Ctx.getObjectWriter()->writeInitialELFHeader();
  }

  Ctx.startWorkerThreads();

  std::unique_ptr<Ice::Translator> Translator;
  if (BuildOnRead) {
    std::unique_ptr<Ice::PNaClTranslator> PTranslator(
        new Ice::PNaClTranslator(&Ctx));
    PTranslator->translate(IRFilename);
    Translator.reset(PTranslator.release());
  } else if (ALLOW_LLVM_IR) {
    // Parse the input LLVM IR file into a module.
    SMDiagnostic Err;
    Ice::TimerMarker T1(Ice::TimerStack::TT_parse, &Ctx);
    raw_ostream *Verbose = LLVMVerboseErrors ? &errs() : nullptr;
    Module *Mod = NaClParseIRFile(IRFilename, InputFileFormat, Err, Verbose,
                                  getGlobalContext());

    if (!Mod) {
      Err.print(argv[0], errs());
      return GetReturnValue(Ice::EC_Bitcode);
    }

    std::unique_ptr<Ice::Converter> Converter(new Ice::Converter(Mod, &Ctx));
    Converter->convertToIce();
    Translator.reset(Converter.release());
  } else {
    *Ls << "Error: Build doesn't allow LLVM IR, "
        << "--build-on-read=0 not allowed\n";
    return GetReturnValue(Ice::EC_Args);
  }

  Ctx.waitForWorkerThreads();
  Translator->transferErrorCode();
  Translator->emitConstants();

  if (OutFileType == Ice::FT_Elf) {
    Ice::TimerMarker T1(Ice::TimerStack::TT_emit, &Ctx);
    Ctx.getObjectWriter()->setUndefinedSyms(Ctx.getConstantExternSyms());
    Ctx.getObjectWriter()->writeNonUserSections();
  }
  if (SubzeroTimingEnabled)
    Ctx.dumpTimers();
  if (TimeEachFunction) {
    const bool DumpCumulative = false;
    Ctx.dumpTimers(Ice::GlobalContext::TSK_Funcs, DumpCumulative);
  }
  const bool FinalStats = true;
  Ctx.dumpStats("_FINAL_", FinalStats);
  return GetReturnValue(Ctx.getErrorStatus()->value());
}
