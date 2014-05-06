//===-- LLVMTargetMachine.cpp - Implement the LLVMTargetMachine class -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LLVMTargetMachine class.
//
//===----------------------------------------------------------------------===//

#include "llvm/Target/TargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunctionAnalysis.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/GCStrategy.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

static cl::opt<bool> DisablePostRA("disable-post-ra", cl::Hidden,
    cl::desc("Disable Post Regalloc"));
static cl::opt<bool> DisableBranchFold("disable-branch-fold", cl::Hidden,
    cl::desc("Disable branch folding"));
static cl::opt<bool> DisableTailDuplicate("disable-tail-duplicate", cl::Hidden,
    cl::desc("Disable tail duplication"));
static cl::opt<bool> DisableEarlyTailDup("disable-early-taildup", cl::Hidden,
    cl::desc("Disable pre-register allocation tail duplication"));
static cl::opt<bool> DisableCodePlace("disable-code-place", cl::Hidden,
    cl::desc("Disable code placement"));
static cl::opt<bool> DisableSSC("disable-ssc", cl::Hidden,
    cl::desc("Disable Stack Slot Coloring"));
static cl::opt<bool> DisableMachineLICM("disable-machine-licm", cl::Hidden,
    cl::desc("Disable Machine LICM"));
static cl::opt<bool> DisablePostRAMachineLICM("disable-postra-machine-licm",
    cl::Hidden,
    cl::desc("Disable Machine LICM"));
static cl::opt<bool> DisableMachineSink("disable-machine-sink", cl::Hidden,
    cl::desc("Disable Machine Sinking"));
static cl::opt<bool> DisableLSR("disable-lsr", cl::Hidden,
    cl::desc("Disable Loop Strength Reduction Pass"));
static cl::opt<bool> DisableCGP("disable-cgp", cl::Hidden,
    cl::desc("Disable Codegen Prepare"));
static cl::opt<bool> ShowMCEncoding("show-mc-encoding", cl::Hidden,
    cl::desc("Show encoding in .s output"));
static cl::opt<bool> ShowMCInst("show-mc-inst", cl::Hidden,
    cl::desc("Show instruction structure in .s output"));
static cl::opt<bool> EnableMCLogging("enable-mc-api-logging", cl::Hidden,
    cl::desc("Enable MC API logging"));

static cl::opt<cl::boolOrDefault>
AsmVerbose("asm-verbose", cl::desc("Add comments to directives."),
           cl::init(cl::BOU_UNSET));

static bool getVerboseAsm() {
  switch (AsmVerbose) {
  default:
  case cl::BOU_UNSET: return TargetMachine::getAsmVerbosityDefault();
  case cl::BOU_TRUE:  return true;
  case cl::BOU_FALSE: return false;
  }      
}

// Enable or disable an experimental optimization to split GEPs
// and run a special GVN pass which does not examine loads, in
// an effort to factor out redundancy implicit in complex GEPs.
static cl::opt<bool> EnableSplitGEPGVN("split-gep-gvn", cl::Hidden,
    cl::desc("Split GEPs and run no-load GVN"));

LLVMTargetMachine::LLVMTargetMachine(const Target &T,
                                     const std::string &Triple)
  : TargetMachine(T), TargetTriple(Triple) {
  AsmInfo = T.createAsmInfo(TargetTriple);
}

// Set the default code model for the JIT for a generic target.
// FIXME: Is small right here? or .is64Bit() ? Large : Small?
void LLVMTargetMachine::setCodeModelForJIT() {
  setCodeModel(CodeModel::Small);
}

// Set the default code model for static compilation for a generic target.
void LLVMTargetMachine::setCodeModelForStatic() {
  setCodeModel(CodeModel::Small);
}

/// addPassesToEmitMachineCode - Add passes to the specified pass manager to
/// get machine code emitted.  This uses a JITCodeEmitter object to handle
/// actually outputting the machine code and resolving things like the address
/// of functions.  This method should returns true if machine code emission is
/// not supported.
///
bool LLVMTargetMachine::addPassesToEmitMachineCode(PassManagerBase &PM,
                                                   JITCodeEmitter &JCE,
                                                   CodeGenOpt::Level OptLevel) {
  // Make sure the code model is set.
  setCodeModelForJIT();
  
  // Add common CodeGen passes.
  MCContext *Ctx = 0;
  if (addCommonCodeGenPasses(PM, OptLevel, Ctx))
    return true;

  addCodeEmitter(PM, OptLevel, JCE);

  return false; // success!
}

/// addPassesToEmitMC - Add passes to the specified pass manager to get
/// machine code emitted with the MCJIT. This method returns true if machine
/// code is not supported. It fills the MCContext Ctx pointer which can be
/// used to build custom MCStreamer.
///
bool LLVMTargetMachine::addPassesToEmitMC(PassManagerBase &PM,
                                          MCContext *&Ctx,
                                          CodeGenOpt::Level OptLevel) {
  // Add common CodeGen passes.
  if (addCommonCodeGenPasses(PM, OptLevel, Ctx))
    return true;
  // Make sure the code model is set.
  setCodeModelForJIT();

  return false; // success!
}

/// addCommonCodeGenPasses - Add standard LLVM codegen passes used for both
/// emitting to assembly files or machine code output.
///
bool LLVMTargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
                                               CodeGenOpt::Level OptLevel,
                                               MCContext *&OutContext) {
  // Standard LLVM-Level Passes.

  // Optionally, tun split-GEPs and no-load GVN.
  if (EnableSplitGEPGVN) {
    PM.add(createGEPSplitterPass());
    PM.add(createGVNPass(/*NoLoads=*/true));
  }

  // Run loop strength reduction before anything else.
  if (OptLevel != CodeGenOpt::None && !DisableLSR) {
    PM.add(createLoopStrengthReducePass(getTargetLowering()));
  }

  // Make sure that no unreachable blocks are instruction selected.
  PM.add(createUnreachableBlockEliminationPass());

  if (OptLevel != CodeGenOpt::None && !DisableCGP)
    PM.add(createCodeGenPreparePass(getTargetLowering()));

  PM.add(createStackProtectorPass(getTargetLowering()));

  addPreISel(PM, OptLevel);

  // Standard Lower-Level Passes.
  
  // Install a MachineModuleInfo class, which is an immutable pass that holds
  // all the per-module stuff we're generating, including MCContext.
  MachineModuleInfo *MMI = new MachineModuleInfo(*getMCAsmInfo());
  PM.add(MMI);
  OutContext = &MMI->getContext(); // Return the MCContext specifically by-ref.
  

  // Set up a MachineFunction for the rest of CodeGen to work on.
  PM.add(new MachineFunctionAnalysis(*this, OptLevel));

  // Ask the target for an isel.
  if (addInstSelector(PM, OptLevel))
    return true;

  // Optimize PHIs before DCE: removing dead PHI cycles may make more
  // instructions dead.
  if (OptLevel != CodeGenOpt::None)
    PM.add(createOptimizePHIsPass());

  if (OptLevel != CodeGenOpt::None) {
    // With optimization, dead code should already be eliminated. However
    // there is one known exception: lowered code for arguments that are only
    // used by tail calls, where the tail calls reuse the incoming stack
    // arguments directly (see t11 in test/CodeGen/X86/sibcall.ll).
    PM.add(createDeadMachineInstructionElimPass());

    PM.add(createPeepholeOptimizerPass());
    if (!DisableMachineLICM)
      PM.add(createMachineLICMPass());
    PM.add(createMachineCSEPass());
    if (!DisableMachineSink)
      PM.add(createMachineSinkingPass());
  }

  // Pre-ra tail duplication.
  if (OptLevel != CodeGenOpt::None && !DisableEarlyTailDup) {
    PM.add(createTailDuplicatePass(true));
  }

  // Run pre-ra passes.
  addPreRegAlloc(PM, OptLevel);

  // Perform register allocation.
  PM.add(createRegisterAllocator(OptLevel));

  // Perform stack slot coloring and post-ra machine LICM.
  if (OptLevel != CodeGenOpt::None) {
    // FIXME: Re-enable coloring with register when it's capable of adding
    // kill markers.
    if (!DisableSSC)
      PM.add(createStackSlotColoringPass(false));

    // Run post-ra machine LICM to hoist reloads / remats.
    if (!DisablePostRAMachineLICM)
      PM.add(createMachineLICMPass(false));
  }

  // Run post-ra passes.
  addPostRegAlloc(PM, OptLevel);

  PM.add(createLowerSubregsPass());

  // Insert prolog/epilog code.  Eliminate abstract frame index references...
  PM.add(createPrologEpilogCodeInserter());

  // Run pre-sched2 passes.
  addPreSched2(PM, OptLevel);
  
  // Second pass scheduler.
  if (OptLevel != CodeGenOpt::None && !DisablePostRA) {
    PM.add(createPostRAScheduler(OptLevel));
  }

  // Branch folding must be run after regalloc and prolog/epilog insertion.
  if (OptLevel != CodeGenOpt::None && !DisableBranchFold) {
    PM.add(createBranchFoldingPass(getEnableTailMergeDefault()));
  }

  // Tail duplication.
  if (OptLevel != CodeGenOpt::None && !DisableTailDuplicate) {
    PM.add(createTailDuplicatePass(false));
  }

  if (OptLevel != CodeGenOpt::None && !DisableCodePlace) {
    PM.add(createCodePlacementOptPass());
  }

  addPreEmitPass(PM, OptLevel);

  return false;
}
