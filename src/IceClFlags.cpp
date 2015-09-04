//===- subzero/src/IceClFlags.cpp - Command line flags and parsing --------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines commandline flags parsing.
/// This currently relies on llvm::cl to parse.  In the future, the minimal
/// build can have a simpler parser.
///
//===----------------------------------------------------------------------===//

#include "IceClFlags.h"

#include "IceClFlagsExtra.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "llvm/Support/CommandLine.h"
#pragma clang diagnostic pop

namespace cl = llvm::cl;

// Options which are captured in Ice::ClFlags and propagated.

namespace {

cl::opt<bool> AllowErrorRecovery(
    "allow-pnacl-reader-error-recovery",
    cl::desc("Allow error recovery when reading PNaCl bitcode."),
    cl::init(false));

cl::opt<bool> AllowIacaMarks(
    "allow-iaca-marks",
    cl::desc("Allow IACA (Intel Architecture Code Analyzer) marks to be "
             "inserted. These binaries are not executable."),
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

cl::opt<bool> EnableBlockProfile(
    "enable-block-profile",
    cl::desc("If true, instrument basic blocks, and output profiling "
             "information to stdout at the end of program execution."),
    cl::init(false));

cl::opt<bool>
    ForceMemIntrinOpt("fmem-intrin-opt",
                      cl::desc("Force optimization of memory intrinsics."));

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
               cl::init(1));

cl::opt<bool> ShouldDoNopInsertion("nop-insertion",
                                   cl::desc("Randomly insert NOPs"),
                                   cl::init(false));

cl::opt<bool>
    RandomizeRegisterAllocation("randomize-regalloc",
                                cl::desc("Randomize register allocation"),
                                cl::init(false));

cl::opt<bool> SkipUnimplemented(
    "skip-unimplemented",
    cl::desc("Skip through unimplemented lowering code instead of aborting."),
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
        clEnumValN(Ice::Target_ARM64, "arm64", "arm64"),
        clEnumValN(Ice::Target_MIPS32, "mips", "mips32"),
        clEnumValN(Ice::Target_MIPS32, "mips32", "mips32 (same as mips)"),
        clEnumValEnd));

cl::opt<uint32_t> TestStackExtra(
    "test-stack-extra",
    cl::desc(
        "Extra amount of stack to add to the frame in bytes (for testing)."),
    cl::init(0));

cl::opt<Ice::TargetInstructionSet> TargetInstructionSet(
    "mattr", cl::desc("Target architecture attributes"),
    cl::init(Ice::BaseInstructionSet),
    cl::values(clEnumValN(Ice::BaseInstructionSet, "base",
                          "Target chooses baseline instruction set (default)"),
               clEnumValN(Ice::X86InstructionSet_SSE2, "sse2",
                          "Enable X86 SSE2 instructions"),
               clEnumValN(Ice::X86InstructionSet_SSE4_1, "sse4.1",
                          "Enable X86 SSE 4.1 instructions"),
               clEnumValN(Ice::ARM32InstructionSet_Neon, "neon",
                          "Enable ARM Neon instructions"),
               clEnumValN(Ice::ARM32InstructionSet_HWDivArm, "hwdiv-arm",
                          "Enable ARM integer divide instructions in ARM mode"),
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
        clEnumValN(Ice::IceV_Folding, "fold", "Instruction folding details"),
        clEnumValN(Ice::IceV_RMW, "rmw", "ReadModifyWrite optimization"),
        clEnumValN(Ice::IceV_Loop, "loop", "Loop nest depth analysis"),
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

// Define the command line options for immediates pooling and randomization
cl::opt<Ice::RandomizeAndPoolImmediatesEnum> RandomizeAndPoolImmediatesOption(
    "randomize-pool-immediates",
    cl::desc("Randomize or pooling the representation of immediates"),
    cl::init(Ice::RPI_None),
    cl::values(clEnumValN(Ice::RPI_None, "none",
                          "Do not randomize or pooling immediates (default)"),
               clEnumValN(Ice::RPI_Randomize, "randomize",
                          "Turn on immediate constants blinding"),
               clEnumValN(Ice::RPI_Pool, "pool",
                          "Turn on immediate constants pooling"),
               clEnumValEnd));
// Command line option for x86 immediate integer randomization/pooling
// threshold. Immediates whose representation are between:
// -RandomizeAndPoolImmediatesThreshold/2 and
// +RandomizeAndPoolImmediatesThreshold/2 will be randomized or pooled.
cl::opt<uint32_t> RandomizeAndPoolImmediatesThreshold(
    "randomize-pool-threshold",
    cl::desc("The threshold for immediates randomization and pooling"),
    cl::init(0xffff));

// Command line option for turning on basic block shuffling.
cl::opt<bool> ReorderBasicBlocks(
    "reorder-basic-blocks",
    cl::desc("Shuffle the layout of basic blocks in each functions"),
    cl::init(false));

// Command line option for turning on function layout reordering.
cl::opt<bool> ReorderFunctions(
    "reorder-functions",
    cl::desc("Reorder the layout of functions in TEXT section"),
    cl::init(false));

// Command line option for the shuffling window size for function reordering.
// The default size is 8.
cl::opt<uint32_t> ReorderFunctionsWindowSize(
    "reorder-functions-window-size",
    cl::desc("The shuffling window size for function reordering. 1 or 0 means "
             "no effective shuffling."),
    cl::init(8));

// Command line option for turning on global variable layout reordering.
cl::opt<bool> ReorderGlobalVariables(
    "reorder-global-variables",
    cl::desc("Reorder the layout of global variables in NON TEXT section"),
    cl::init(false));

// Command line option for turning on layout reordering in constant pools.
cl::opt<bool> ReorderPooledConstants(
    "reorder-pooled-constants",
    cl::desc("Reorder the layout of constants in constant pools"),
    cl::init(false));

// Command line option for accepting textual bitcode.
cl::opt<bool> BitcodeAsText(
    "bitcode-as-text",
    cl::desc(
        "Accept textual form of PNaCl bitcode records (i.e. not .ll assembly)"),
    cl::init(false));
} // end of anonymous namespace

namespace Ice {

void ClFlags::parseFlags(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);
  AppName = IceString(argv[0]);
}

void ClFlags::resetClFlags(ClFlags &OutFlags) {
  // bool fields
  OutFlags.AllowErrorRecovery = false;
  OutFlags.AllowIacaMarks = false;
  OutFlags.AllowUninitializedGlobals = false;
  OutFlags.DataSections = false;
  OutFlags.DecorateAsm = false;
  OutFlags.DisableInternal = false;
  OutFlags.DisableIRGeneration = false;
  OutFlags.DisableTranslation = false;
  OutFlags.DumpStats = false;
  OutFlags.EnableBlockProfile = false;
  OutFlags.ForceMemIntrinOpt = false;
  OutFlags.FunctionSections = false;
  OutFlags.GenerateUnitTestMessages = false;
  OutFlags.PhiEdgeSplit = false;
  OutFlags.RandomNopInsertion = false;
  OutFlags.RandomRegAlloc = false;
  OutFlags.ReorderBasicBlocks = false;
  OutFlags.ReorderFunctions = false;
  OutFlags.ReorderGlobalVariables = false;
  OutFlags.ReorderPooledConstants = false;
  OutFlags.SkipUnimplemented = false;
  OutFlags.SubzeroTimingEnabled = false;
  OutFlags.TimeEachFunction = false;
  OutFlags.UseSandboxing = false;
  // Enum and integer fields.
  OutFlags.Opt = Opt_m1;
  OutFlags.OutFileType = FT_Iasm;
  OutFlags.RandomMaxNopsPerInstruction = 0;
  OutFlags.RandomNopProbabilityAsPercentage = 0;
  OutFlags.RandomizeAndPoolImmediatesOption = RPI_None;
  OutFlags.RandomizeAndPoolImmediatesThreshold = 0xffff;
  OutFlags.ReorderFunctionsWindowSize = 8;
  OutFlags.TArch = TargetArch_NUM;
  OutFlags.TestStackExtra = 0;
  OutFlags.VMask = IceV_None;
  // IceString fields.
  OutFlags.DefaultFunctionPrefix = "";
  OutFlags.DefaultGlobalPrefix = "";
  OutFlags.TestPrefix = "";
  OutFlags.TimingFocusOn = "";
  OutFlags.TranslateOnly = "";
  OutFlags.VerboseFocusOn = "";
  // size_t and 64-bit fields.
  OutFlags.NumTranslationThreads = 0;
  OutFlags.RandomSeed = 0;
}

void ClFlags::getParsedClFlags(ClFlags &OutFlags) {
  if (::DisableIRGeneration)
    ::DisableTranslation = true;

  Ice::VerboseMask VMask = Ice::IceV_None;
  // Don't generate verbose messages if routines
  // to dump messages are not available.
  if (BuildDefs::dump()) {
    for (unsigned i = 0; i != VerboseList.size(); ++i)
      VMask |= VerboseList[i];
  }

  OutFlags.setAllowErrorRecovery(::AllowErrorRecovery);
  OutFlags.setAllowIacaMarks(::AllowIacaMarks);
  OutFlags.setAllowUninitializedGlobals(::AllowUninitializedGlobals);
  OutFlags.setDataSections(::DataSections);
  OutFlags.setDecorateAsm(::DecorateAsm);
  OutFlags.setDefaultFunctionPrefix(::DefaultFunctionPrefix);
  OutFlags.setDefaultGlobalPrefix(::DefaultGlobalPrefix);
  OutFlags.setDisableInternal(::DisableInternal);
  OutFlags.setDisableIRGeneration(::DisableIRGeneration);
  OutFlags.setDisableTranslation(::DisableTranslation);
  OutFlags.setDumpStats(::DumpStats);
  OutFlags.setEnableBlockProfile(::EnableBlockProfile);
  OutFlags.setForceMemIntrinOpt(::ForceMemIntrinOpt);
  OutFlags.setFunctionSections(::FunctionSections);
  OutFlags.setNumTranslationThreads(::NumThreads);
  OutFlags.setOptLevel(::OLevel);
  OutFlags.setPhiEdgeSplit(::EnablePhiEdgeSplit);
  OutFlags.setRandomSeed(::RandomSeed);
  OutFlags.setRandomizeAndPoolImmediatesOption(
      ::RandomizeAndPoolImmediatesOption);
  OutFlags.setRandomizeAndPoolImmediatesThreshold(
      ::RandomizeAndPoolImmediatesThreshold);
  OutFlags.setReorderFunctionsWindowSize(::ReorderFunctionsWindowSize);
  OutFlags.setShouldReorderBasicBlocks(::ReorderBasicBlocks);
  OutFlags.setShouldDoNopInsertion(::ShouldDoNopInsertion);
  OutFlags.setShouldRandomizeRegAlloc(::RandomizeRegisterAllocation);
  OutFlags.setShouldReorderFunctions(::ReorderFunctions);
  OutFlags.setShouldReorderGlobalVariables(::ReorderGlobalVariables);
  OutFlags.setShouldReorderPooledConstants(::ReorderPooledConstants);
  OutFlags.setSkipUnimplemented(::SkipUnimplemented);
  OutFlags.setSubzeroTimingEnabled(::SubzeroTimingEnabled);
  OutFlags.setTargetArch(::TargetArch);
  OutFlags.setTargetInstructionSet(::TargetInstructionSet);
  OutFlags.setTestPrefix(::TestPrefix);
  OutFlags.setTestStackExtra(::TestStackExtra);
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
  OutFlagsExtra.setBitcodeAsText(BitcodeAsText);
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
