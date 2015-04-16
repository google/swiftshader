//===- subzero/src/IceClFlags.cpp - Command line flags and parsing --------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines commandline flags parsing.
// This currently relies on llvm::cl to parse.  In the future, the minimal
// build can have a simpler parser.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/CommandLine.h"

#include "IceClFlags.h"
#include "IceClFlagsExtra.h"

namespace cl = llvm::cl;

// Options which are captured in Ice::ClFlags and propagated.

namespace {

cl::opt<bool> AllowErrorRecovery(
    "allow-pnacl-reader-error-recovery",
    cl::desc("Allow error recovery when reading PNaCl bitcode."),
    cl::init(false));

// This is currently needed by crosstest.py.
cl::opt<bool> AllowUninitializedGlobals(
    "allow-uninitialized-globals",
    cl::desc("Allow global variables to be uninitialized"));

cl::opt<bool>
    DataSections("fdata-sections",
                 cl::desc("Emit (global) data into separate sections"));

cl::opt<bool> DecorateAsm(
    "asm-verbose",
    cl::desc("Decorate textual asm output with register liveness info"));

cl::opt<std::string>
    DefaultFunctionPrefix("default-function-prefix",
                          cl::desc("Define default function prefix for naming "
                                   "unnamed functions"),
                          cl::init("Function"));

cl::opt<std::string>
    DefaultGlobalPrefix("default-global-prefix",
                        cl::desc("Define default global prefix for naming "
                                 "unnamed globals"),
                        cl::init("Global"));
cl::opt<bool> DisableInternal("externalize",
                              cl::desc("Externalize all symbols"));
// Note: Modifiable only if ALLOW_DISABLE_IR_GEN.
cl::opt<bool> DisableIRGeneration("no-ir-gen",
                                  cl::desc("Disable generating Subzero IR."));
cl::opt<bool> DisableTranslation("notranslate",
                                 cl::desc("Disable Subzero translation"));

cl::opt<bool>
    DumpStats("szstats",
              cl::desc("Print statistics after translating each function"));

cl::opt<bool>
    FunctionSections("ffunction-sections",
                     cl::desc("Emit functions into separate sections"));

// Number of translation threads (in addition to the parser thread and
// the emitter thread).  The special case of 0 means purely
// sequential, i.e. parser, translator, and emitter all within the
// same single thread.  (This may need a slight rework if we expand to
// multiple parser or emitter threads.)
cl::opt<uint32_t> NumThreads(
    "threads",
    cl::desc("Number of translation threads (0 for purely sequential)"),
    // TODO(stichnot): Settle on a good default.  Consider
    // something related to std::thread::hardware_concurrency().
    cl::init(2));

cl::opt<Ice::OptLevel> OLevel(cl::desc("Optimization level"),
                              cl::init(Ice::Opt_m1), cl::value_desc("level"),
                              cl::values(clEnumValN(Ice::Opt_m1, "Om1", "-1"),
                                         clEnumValN(Ice::Opt_m1, "O-1", "-1"),
                                         clEnumValN(Ice::Opt_0, "O0", "0"),
                                         clEnumValN(Ice::Opt_1, "O1", "1"),
                                         clEnumValN(Ice::Opt_2, "O2", "2"),
                                         clEnumValEnd));

cl::opt<bool>
    EnablePhiEdgeSplit("phi-edge-split",
                       cl::desc("Enable edge splitting for Phi lowering"),
                       cl::init(true));

// TODO(stichnot): See if we can easily use LLVM's -rng-seed option
// and implementation.  I expect the implementation is different and
// therefore the tests would need to be changed.
cl::opt<unsigned long long>
    RandomSeed("sz-seed", cl::desc("Seed the random number generator"),
               cl::init(time(0)));

cl::opt<bool> ShouldDoNopInsertion("nop-insertion",
                                   cl::desc("Randomly insert NOPs"),
                                   cl::init(false));

cl::opt<bool>
    RandomizeRegisterAllocation("randomize-regalloc",
                                cl::desc("Randomize register allocation"),
                                cl::init(false));

cl::opt<bool> SubzeroTimingEnabled(
    "timing", cl::desc("Enable breakdown timing of Subzero translation"));

cl::opt<Ice::TargetArch> TargetArch(
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
cl::opt<Ice::TargetInstructionSet> TargetInstructionSet(
    "mattr", cl::desc("Target architecture attributes"),
    cl::init(Ice::X86InstructionSet_SSE2),
    cl::values(clEnumValN(Ice::X86InstructionSet_SSE2, "sse2",
                          "Enable SSE2 instructions (default)"),
               clEnumValN(Ice::X86InstructionSet_SSE4_1, "sse4.1",
                          "Enable SSE 4.1 instructions"),
               clEnumValEnd));
cl::opt<std::string>
    TestPrefix("prefix",
               cl::desc("Prepend a prefix to symbol names for testing"),
               cl::init(""), cl::value_desc("prefix"));

cl::opt<bool> TimeEachFunction(
    "timing-funcs", cl::desc("Print total translation time for each function"));

cl::opt<std::string> TimingFocusOn(
    "timing-focus",
    cl::desc("Break down timing for a specific function (use '*' for all)"),
    cl::init(""));

cl::opt<std::string>
    TranslateOnly("translate-only",
                  cl::desc("Translate only the given function"), cl::init(""));

cl::opt<bool> UseSandboxing("sandbox", cl::desc("Use sandboxing"));

cl::opt<std::string> VerboseFocusOn(
    "verbose-focus",
    cl::desc("Temporarily enable full verbosity for a specific function"),
    cl::init(""));

cl::opt<Ice::FileType> OutFileType(
    "filetype", cl::desc("Output file type"), cl::init(Ice::FT_Iasm),
    cl::values(clEnumValN(Ice::FT_Elf, "obj", "Native ELF object ('.o') file"),
               clEnumValN(Ice::FT_Asm, "asm", "Assembly ('.s') file"),
               clEnumValN(Ice::FT_Iasm, "iasm",
                          "Low-level integrated assembly ('.s') file"),
               clEnumValEnd));

cl::opt<int> MaxNopsPerInstruction(
    "max-nops-per-instruction",
    cl::desc("Max number of nops to insert per instruction"), cl::init(1));

cl::opt<int> NopProbabilityAsPercentage(
    "nop-insertion-percentage",
    cl::desc("Nop insertion probability as percentage"), cl::init(10));

cl::list<Ice::VerboseItem> VerboseList(
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
                   "Use all verbose options except 'regalloc'"),
        clEnumValN(Ice::IceV_None, "none", "No verbosity"), clEnumValEnd));

// Options not captured in Ice::ClFlags and propagated.

cl::opt<bool> AlwaysExitSuccess(
    "exit-success", cl::desc("Exit with success status, even if errors found"),
    cl::init(false));

// Note: While this flag isn't used in the minimal build, we keep this
// flag so that tests can set this command-line flag without concern
// to the type of build. We double check that this flag at runtime
// to make sure the consistency is maintained.
cl::opt<bool>
    BuildOnRead("build-on-read",
                cl::desc("Build ICE instructions when reading bitcode"),
                cl::init(true));

cl::opt<llvm::NaClFileFormat> InputFileFormat(
    "bitcode-format", cl::desc("Define format of input file:"),
    cl::values(clEnumValN(llvm::LLVMFormat, "llvm", "LLVM file (default)"),
               clEnumValN(llvm::PNaClFormat, "pnacl", "PNaCl bitcode file"),
               clEnumValEnd),
    cl::init(llvm::LLVMFormat));

cl::opt<bool> GenerateBuildAtts(
    "build-atts", cl::desc("Generate list of build attributes associated with "
                           "this executable."),
    cl::init(false));

cl::opt<std::string> IRFilename(cl::Positional, cl::desc("<IR file>"),
                                cl::init("-"));
cl::opt<std::string> LogFilename("log", cl::desc("Set log filename"),
                                 cl::init("-"), cl::value_desc("filename"));
cl::opt<bool> LLVMVerboseErrors(
    "verbose-llvm-parse-errors",
    cl::desc("Print out more descriptive PNaCl bitcode parse errors when "
             "building LLVM IR first"),
    cl::init(false));
cl::opt<std::string> OutputFilename("o", cl::desc("Override output filename"),
                                    cl::init("-"), cl::value_desc("filename"));

Ice::IceString AppName;

} // end of anonymous namespace

namespace Ice {

void ClFlags::parseFlags(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);
  AppName = IceString(argv[0]);
}

void ClFlags::getParsedClFlags(ClFlags &OutFlags) {
  if (::DisableIRGeneration)
    ::DisableTranslation = true;

  Ice::VerboseMask VMask = Ice::IceV_None;
  // Don't generate verbose messages if routines
  // to dump messages are not available.
  if (ALLOW_DUMP) {
    for (unsigned i = 0; i != VerboseList.size(); ++i)
      VMask |= VerboseList[i];
  }

  OutFlags.setAllowErrorRecovery(::AllowErrorRecovery);
  OutFlags.setAllowUninitializedGlobals(::AllowUninitializedGlobals);
  OutFlags.setDataSections(::DataSections);
  OutFlags.setDecorateAsm(::DecorateAsm);
  OutFlags.setDefaultFunctionPrefix(::DefaultFunctionPrefix);
  OutFlags.setDefaultGlobalPrefix(::DefaultGlobalPrefix);
  OutFlags.setDisableInternal(::DisableInternal);
  OutFlags.setDisableIRGeneration(::DisableIRGeneration);
  OutFlags.setDisableTranslation(::DisableTranslation);
  OutFlags.setDumpStats(::DumpStats);
  OutFlags.setFunctionSections(::FunctionSections);
  OutFlags.setNumTranslationThreads(::NumThreads);
  OutFlags.setOptLevel(::OLevel);
  OutFlags.setPhiEdgeSplit(::EnablePhiEdgeSplit);
  OutFlags.setRandomSeed(::RandomSeed);
  OutFlags.setShouldDoNopInsertion(::ShouldDoNopInsertion);
  OutFlags.setShouldRandomizeRegAlloc(::RandomizeRegisterAllocation);
  OutFlags.setSubzeroTimingEnabled(::SubzeroTimingEnabled);
  OutFlags.setTargetArch(::TargetArch);
  OutFlags.setTargetInstructionSet(::TargetInstructionSet);
  OutFlags.setTestPrefix(::TestPrefix);
  OutFlags.setTimeEachFunction(::TimeEachFunction);
  OutFlags.setTimingFocusOn(::TimingFocusOn);
  OutFlags.setTranslateOnly(::TranslateOnly);
  OutFlags.setUseSandboxing(::UseSandboxing);
  OutFlags.setVerboseFocusOn(::VerboseFocusOn);
  OutFlags.setOutFileType(::OutFileType);
  OutFlags.setMaxNopsPerInstruction(::MaxNopsPerInstruction);
  OutFlags.setNopProbabilityAsPercentage(::NopProbabilityAsPercentage);
  OutFlags.setVerbose(VMask);
}

void ClFlags::getParsedClFlagsExtra(ClFlagsExtra &OutFlagsExtra) {
  OutFlagsExtra.setAlwaysExitSuccess(AlwaysExitSuccess);
  OutFlagsExtra.setBuildOnRead(BuildOnRead);
  OutFlagsExtra.setGenerateBuildAtts(GenerateBuildAtts);
  OutFlagsExtra.setLLVMVerboseErrors(LLVMVerboseErrors);
  OutFlagsExtra.setAppName(AppName);
  OutFlagsExtra.setInputFileFormat(InputFileFormat);
  OutFlagsExtra.setIRFilename(IRFilename);
  OutFlagsExtra.setLogFilename(LogFilename);
  OutFlagsExtra.setOutputFilename(OutputFilename);
}

} // end of namespace Ice
