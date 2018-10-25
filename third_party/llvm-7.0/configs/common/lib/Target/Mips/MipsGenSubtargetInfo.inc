/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Subtarget Enumeration Source Fragment                                      *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_SUBTARGETINFO_ENUM
#undef GET_SUBTARGETINFO_ENUM

namespace llvm {
namespace Mips {
enum {
  FeatureCRC = 0,
  FeatureCnMips = 1,
  FeatureDSP = 2,
  FeatureDSPR2 = 3,
  FeatureDSPR3 = 4,
  FeatureEVA = 5,
  FeatureFP64Bit = 6,
  FeatureFPXX = 7,
  FeatureGINV = 8,
  FeatureGP64Bit = 9,
  FeatureLongCalls = 10,
  FeatureMSA = 11,
  FeatureMT = 12,
  FeatureMadd4 = 13,
  FeatureMicroMips = 14,
  FeatureMips1 = 15,
  FeatureMips2 = 16,
  FeatureMips3 = 17,
  FeatureMips3_32 = 18,
  FeatureMips3_32r2 = 19,
  FeatureMips4 = 20,
  FeatureMips4_32 = 21,
  FeatureMips4_32r2 = 22,
  FeatureMips5 = 23,
  FeatureMips5_32r2 = 24,
  FeatureMips16 = 25,
  FeatureMips32 = 26,
  FeatureMips32r2 = 27,
  FeatureMips32r3 = 28,
  FeatureMips32r5 = 29,
  FeatureMips32r6 = 30,
  FeatureMips64 = 31,
  FeatureMips64r2 = 32,
  FeatureMips64r3 = 33,
  FeatureMips64r5 = 34,
  FeatureMips64r6 = 35,
  FeatureNaN2008 = 36,
  FeatureNoABICalls = 37,
  FeatureNoOddSPReg = 38,
  FeaturePTR64Bit = 39,
  FeatureSingleFloat = 40,
  FeatureSoftFloat = 41,
  FeatureSym32 = 42,
  FeatureUseIndirectJumpsHazard = 43,
  FeatureUseTCCInDIV = 44,
  FeatureVFPU = 45,
  FeatureVirt = 46,
  ImplP5600 = 47,
};
} // end namespace Mips
} // end namespace llvm

#endif // GET_SUBTARGETINFO_ENUM


#ifdef GET_SUBTARGETINFO_MC_DESC
#undef GET_SUBTARGETINFO_MC_DESC

namespace llvm {
// Sorted (by key) array of values for CPU features.
extern const llvm::SubtargetFeatureKV MipsFeatureKV[] = {
  { "cnmips", "Octeon cnMIPS Support", { Mips::FeatureCnMips }, { Mips::FeatureMips64r2 } },
  { "crc", "Mips R6 CRC ASE", { Mips::FeatureCRC }, { } },
  { "dsp", "Mips DSP ASE", { Mips::FeatureDSP }, { } },
  { "dspr2", "Mips DSP-R2 ASE", { Mips::FeatureDSPR2 }, { Mips::FeatureDSP } },
  { "dspr3", "Mips DSP-R3 ASE", { Mips::FeatureDSPR3 }, { Mips::FeatureDSP, Mips::FeatureDSPR2 } },
  { "eva", "Mips EVA ASE", { Mips::FeatureEVA }, { } },
  { "fp64", "Support 64-bit FP registers", { Mips::FeatureFP64Bit }, { } },
  { "fpxx", "Support for FPXX", { Mips::FeatureFPXX }, { } },
  { "ginv", "Mips Global Invalidate ASE", { Mips::FeatureGINV }, { } },
  { "gp64", "General Purpose Registers are 64-bit wide", { Mips::FeatureGP64Bit }, { } },
  { "long-calls", "Disable use of the jal instruction", { Mips::FeatureLongCalls }, { } },
  { "micromips", "microMips mode", { Mips::FeatureMicroMips }, { } },
  { "mips1", "Mips I ISA Support [highly experimental]", { Mips::FeatureMips1 }, { } },
  { "mips16", "Mips16 mode", { Mips::FeatureMips16 }, { } },
  { "mips2", "Mips II ISA Support [highly experimental]", { Mips::FeatureMips2 }, { Mips::FeatureMips1 } },
  { "mips3", "MIPS III ISA Support [highly experimental]", { Mips::FeatureMips3 }, { Mips::FeatureMips2, Mips::FeatureMips3_32, Mips::FeatureMips3_32r2, Mips::FeatureGP64Bit, Mips::FeatureFP64Bit } },
  { "mips32", "Mips32 ISA Support", { Mips::FeatureMips32 }, { Mips::FeatureMips2, Mips::FeatureMips3_32, Mips::FeatureMips4_32 } },
  { "mips32r2", "Mips32r2 ISA Support", { Mips::FeatureMips32r2 }, { Mips::FeatureMips3_32r2, Mips::FeatureMips4_32r2, Mips::FeatureMips5_32r2, Mips::FeatureMips32 } },
  { "mips32r3", "Mips32r3 ISA Support", { Mips::FeatureMips32r3 }, { Mips::FeatureMips32r2 } },
  { "mips32r5", "Mips32r5 ISA Support", { Mips::FeatureMips32r5 }, { Mips::FeatureMips32r3 } },
  { "mips32r6", "Mips32r6 ISA Support [experimental]", { Mips::FeatureMips32r6 }, { Mips::FeatureMips32r5, Mips::FeatureFP64Bit, Mips::FeatureNaN2008 } },
  { "mips3_32", "Subset of MIPS-III that is also in MIPS32 [highly experimental]", { Mips::FeatureMips3_32 }, { } },
  { "mips3_32r2", "Subset of MIPS-III that is also in MIPS32r2 [highly experimental]", { Mips::FeatureMips3_32r2 }, { } },
  { "mips4", "MIPS IV ISA Support", { Mips::FeatureMips4 }, { Mips::FeatureMips3, Mips::FeatureMips4_32, Mips::FeatureMips4_32r2 } },
  { "mips4_32", "Subset of MIPS-IV that is also in MIPS32 [highly experimental]", { Mips::FeatureMips4_32 }, { } },
  { "mips4_32r2", "Subset of MIPS-IV that is also in MIPS32r2 [highly experimental]", { Mips::FeatureMips4_32r2 }, { } },
  { "mips5", "MIPS V ISA Support [highly experimental]", { Mips::FeatureMips5 }, { Mips::FeatureMips4, Mips::FeatureMips5_32r2 } },
  { "mips5_32r2", "Subset of MIPS-V that is also in MIPS32r2 [highly experimental]", { Mips::FeatureMips5_32r2 }, { } },
  { "mips64", "Mips64 ISA Support", { Mips::FeatureMips64 }, { Mips::FeatureMips5, Mips::FeatureMips32 } },
  { "mips64r2", "Mips64r2 ISA Support", { Mips::FeatureMips64r2 }, { Mips::FeatureMips64, Mips::FeatureMips32r2 } },
  { "mips64r3", "Mips64r3 ISA Support", { Mips::FeatureMips64r3 }, { Mips::FeatureMips64r2, Mips::FeatureMips32r3 } },
  { "mips64r5", "Mips64r5 ISA Support", { Mips::FeatureMips64r5 }, { Mips::FeatureMips64r3, Mips::FeatureMips32r5 } },
  { "mips64r6", "Mips64r6 ISA Support [experimental]", { Mips::FeatureMips64r6 }, { Mips::FeatureMips32r6, Mips::FeatureMips64r5, Mips::FeatureNaN2008 } },
  { "msa", "Mips MSA ASE", { Mips::FeatureMSA }, { } },
  { "mt", "Mips MT ASE", { Mips::FeatureMT }, { } },
  { "nan2008", "IEEE 754-2008 NaN encoding", { Mips::FeatureNaN2008 }, { } },
  { "noabicalls", "Disable SVR4-style position-independent code", { Mips::FeatureNoABICalls }, { } },
  { "nomadd4", "Disable 4-operand madd.fmt and related instructions", { Mips::FeatureMadd4 }, { } },
  { "nooddspreg", "Disable odd numbered single-precision registers", { Mips::FeatureNoOddSPReg }, { } },
  { "p5600", "The P5600 Processor", { Mips::ImplP5600 }, { Mips::FeatureMips32r5 } },
  { "ptr64", "Pointers are 64-bit wide", { Mips::FeaturePTR64Bit }, { } },
  { "single-float", "Only supports single precision float", { Mips::FeatureSingleFloat }, { } },
  { "soft-float", "Does not support floating point instructions", { Mips::FeatureSoftFloat }, { } },
  { "sym32", "Symbols are 32 bit on Mips64", { Mips::FeatureSym32 }, { } },
  { "use-indirect-jump-hazard", "Use indirect jump guards to prevent certain speculation based attacks", { Mips::FeatureUseIndirectJumpsHazard }, { } },
  { "use-tcc-in-div", "Force the assembler to use trapping", { Mips::FeatureUseTCCInDIV }, { } },
  { "vfpu", "Enable vector FPU instructions", { Mips::FeatureVFPU }, { } },
  { "virt", "Mips Virtualization ASE", { Mips::FeatureVirt }, { } },
};

// Sorted (by key) array of values for CPU subtype.
extern const llvm::SubtargetFeatureKV MipsSubTypeKV[] = {
  { "mips1", "Select the mips1 processor", { Mips::FeatureMips1 }, { } },
  { "mips2", "Select the mips2 processor", { Mips::FeatureMips2 }, { } },
  { "mips3", "Select the mips3 processor", { Mips::FeatureMips3 }, { } },
  { "mips32", "Select the mips32 processor", { Mips::FeatureMips32 }, { } },
  { "mips32r2", "Select the mips32r2 processor", { Mips::FeatureMips32r2 }, { } },
  { "mips32r3", "Select the mips32r3 processor", { Mips::FeatureMips32r3 }, { } },
  { "mips32r5", "Select the mips32r5 processor", { Mips::FeatureMips32r5 }, { } },
  { "mips32r6", "Select the mips32r6 processor", { Mips::FeatureMips32r6 }, { } },
  { "mips4", "Select the mips4 processor", { Mips::FeatureMips4 }, { } },
  { "mips5", "Select the mips5 processor", { Mips::FeatureMips5 }, { } },
  { "mips64", "Select the mips64 processor", { Mips::FeatureMips64 }, { } },
  { "mips64r2", "Select the mips64r2 processor", { Mips::FeatureMips64r2 }, { } },
  { "mips64r3", "Select the mips64r3 processor", { Mips::FeatureMips64r3 }, { } },
  { "mips64r5", "Select the mips64r5 processor", { Mips::FeatureMips64r5 }, { } },
  { "mips64r6", "Select the mips64r6 processor", { Mips::FeatureMips64r6 }, { } },
  { "octeon", "Select the octeon processor", { Mips::FeatureMips64r2, Mips::FeatureCnMips }, { } },
  { "p5600", "Select the p5600 processor", { Mips::ImplP5600 }, { } },
};

#ifdef DBGFIELD
#error "<target>GenSubtargetInfo.inc requires a DBGFIELD macro"
#endif
#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
#define DBGFIELD(x) x,
#else
#define DBGFIELD(x)
#endif

// ===============================================================
// Data tables for the new per-operand machine model.

// {ProcResourceIdx, Cycles}
extern const llvm::MCWriteProcResEntry MipsWriteProcResTable[] = {
  { 0,  0}, // Invalid
  { 1,  1}, // #1
  { 7,  1}, // #2
  { 1,  1}, // #3
  { 9,  1}, // #4
  { 5,  1}, // #5
  {13,  1}, // #6
  { 5,  1}, // #7
  {11,  1}, // #8
  { 1,  1}, // #9
  {17,  1}, // #10
  {10, 33}, // #11
  {19, 33}, // #12
  {10, 31}, // #13
  {19, 31}, // #14
  { 5,  1}, // #15
  {14,  1}, // #16
  {16,  1}, // #17
  {18,  1}, // #18
  { 2,  1}, // #19
  { 8,  1}, // #20
  { 3,  1}, // #21
  { 5, 29}, // #22
  { 6, 29}, // #23
  { 5, 14}, // #24
  { 6, 14}, // #25
  { 5,  1}, // #26
  {12,  1}, // #27
  { 5, 21}, // #28
  { 6, 21}, // #29
  { 5, 10}, // #30
  { 6, 10}, // #31
  { 5,  1}, // #32
  {15,  1}, // #33
  { 5, 33}, // #34
  { 4,  1}, // #35
  { 3,  1}, // #36
  { 8,  1}, // #37
  { 1,  1}, // #38
  { 4,  1}, // #39
  { 9,  1}, // #40
  { 1,  1}, // #41
  {13,  1}, // #42
  { 1,  1}, // #43
  { 4,  1}, // #44
  {13,  1}, // #45
  { 1,  1}, // #46
  { 4,  3}, // #47
  {13,  1}, // #48
  { 1,  1}, // #49
  { 4,  1}, // #50
  { 7,  1}, // #51
  { 1,  1}, // #52
  { 2, 34}, // #53
  { 7,  1}, // #54
  { 5,  1}, // #55
  { 6, 23}, // #56
  {10,  1}, // #57
  { 5,  1}, // #58
  {10,  1}, // #59
  { 5,  1}, // #60
  { 6, 31}, // #61
  {10,  1}, // #62
  { 5,  1}, // #63
  { 6, 27}, // #64
  {10,  1}, // #65
  { 5,  1}, // #66
  { 6, 35}, // #67
  {10,  1}, // #68
  { 1,  1}, // #69
  { 5,  1}, // #70
  {12,  1}, // #71
  {13,  1}, // #72
  { 1,  1}, // #73
  { 5,  1}, // #74
  {10,  1}, // #75
  {13,  1}, // #76
  { 1,  1}, // #77
  { 5,  1}, // #78
  {11,  1}, // #79
  {13,  1} // #80
}; // MipsWriteProcResTable

// {Cycles, WriteResourceID}
extern const llvm::MCWriteLatencyEntry MipsWriteLatencyTable[] = {
  { 0,  0}, // Invalid
  { 1,  0}, // #1 GenericWriteALU_GenericWriteJump_GenericWriteHILO_GenericWriteTrap_GenericWriteCOPOther_GenericWriteCOP0Short_GenericWritePref_GenericWriteStore_GenericWriteFPUStore_GenericWriteSync_GenericWriteMSAShortLogic_P5600WriteALU_P5600WriteJump_P5600COP0_P5600COP2_P5600WriteStore_P5600WriteCache_P5600WriteAL2_P5600WriteMSAShortLogic_P5600WriteFPUS_P5600WriteMoveOtherUnitsToFPU_P5600WriteStoreFromOtherUnits
  { 2,  0}, // #2 GenericWriteFPUMoveGPRFPU_GenericWriteJumpAndLink_GenericWriteMove_GenericWriteFPUCmp_GenericReadCOP0_GenericWriteLoad_GenericWriteFPULoad_GenericWriteStoreSC_GenericWriteMSAShortInt_GenericDSPShort_P5600WriteJumpAndLink_P5600WriteAL2ShadowMov_P5600WriteAL2CondMov_P5600WriteAL2BitExt_P5600WriteMSAShortInt_P5600WriteGPRFromBypass_P5600WriteStoreFromOtherUnits
  { 5,  0}, // #3 GenericWriteFPUL_GenericWriteALULong_GenericWriteCache_GenericWriteMDUtoGPR_GenericReadWriteCOP0Long_GenericWriteMSALongInt_GenericDSPMTHLIP_GenericDSPMTHILO_P5600WriteAL2Mult_P5600WriteAL2MAdd_P5600WriteMSALongInt
  {33,  0}, // #4 GenericWriteDIV_GenericWriteFPUDivI
  {31,  0}, // #5 GenericWriteDIVU_P5600WriteFPUDivD
  { 4,  0}, // #6 GenericWriteFPUS_GenericWriteFPUMoveFP_GenericWriteMul_GenericWriteCOP0TLB_P5600WriteLoad_P5600WriteLoadShifted_P5600WriteFPUL_P5600WriteLoadOtherUnitsToFPU
  { 3,  0}, // #7 GenericWriteCOP0_GenericWriteMSAOther3_P5600WriteAL2Mul_P5600WriteMSAOther3
  {29,  0}, // #8 GenericWriteFPUSqrtD
  {32,  0}, // #9 GenericWriteFPUDivD
  {17,  0}, // #10 GenericWriteFPUDivS_GenericWriteFPUSqrtS
  {25,  0}, // #11 GenericWriteFPURcpD
  {13,  0}, // #12 GenericWriteFPURcpS
  { 6,  0}, // #13 GenericDSPLong_P5600WriteFPUL_MADDSUB
  {34,  0}, // #14 P5600WriteAL2Div_P5600WriteAL2DivU
  {23,  0}, // #15 P5600WriteFPUDivI_P5600WriteFPUDivS
  {27,  0}, // #16 P5600WriteFPUSqrtS_P5600WriteFPURsqrtD_P5600WriteFPURsqrtS
  {35,  0} // #17 P5600WriteFPUSqrtD
}; // MipsWriteLatencyTable

// {UseIdx, WriteResourceID, Cycles}
extern const llvm::MCReadAdvanceEntry MipsReadAdvanceTable[] = {
  {0,  0,  0}, // Invalid
  {0,  0,  5} // #1
}; // MipsReadAdvanceTable

// {Name, NumMicroOps, BeginGroup, EndGroup, WriteProcResIdx,#, WriteLatencyIdx,#, ReadAdvanceIdx,#}
static const llvm::MCSchedClassDesc MipsGenericModelSchedClasses[] = {
  {DBGFIELD("InvalidSchedClass")  16383, false, false,  0, 0,  0, 0,  0, 0},
  {DBGFIELD("IIPseudo")           1, false, false,  1, 2,  1, 1,  0, 0}, // #1
  {DBGFIELD("II_B")               1, false, false,  3, 2,  1, 1,  0, 0}, // #2
  {DBGFIELD("II_BCCZAL")          1, false, false,  3, 2,  1, 1,  0, 0}, // #3
  {DBGFIELD("II_MTC1")            1, false, false,  5, 2,  2, 1,  0, 0}, // #4
  {DBGFIELD("II_MFC1")            1, false, false,  5, 2,  2, 1,  0, 0}, // #5
  {DBGFIELD("II_JALR")            1, false, false,  3, 2,  2, 1,  0, 0}, // #6
  {DBGFIELD("II_CVT")             1, false, false,  7, 2,  3, 1,  0, 0}, // #7
  {DBGFIELD("II_DMULT")           1, false, false,  9, 2,  1, 1,  0, 0}, // #8
  {DBGFIELD("II_DMULTU")          1, false, false,  9, 2,  1, 1,  0, 0}, // #9
  {DBGFIELD("II_DDIV")            1, false, false, 11, 2,  4, 1,  0, 0}, // #10
  {DBGFIELD("II_DDIVU")           1, false, false, 13, 2,  5, 1,  0, 0}, // #11
  {DBGFIELD("II_IndirectBranchPseudo") 0, false, false,  0, 0,  0, 0,  0, 0}, // #12
  {DBGFIELD("II_MADD")            1, false, false,  9, 2,  1, 1,  0, 0}, // #13
  {DBGFIELD("II_MADDU")           1, false, false,  9, 2,  1, 1,  0, 0}, // #14
  {DBGFIELD("II_MFHI_MFLO")       1, false, false,  1, 2,  3, 1,  0, 0}, // #15
  {DBGFIELD("II_MSUB")            1, false, false,  9, 2,  1, 1,  0, 0}, // #16
  {DBGFIELD("II_MSUBU")           1, false, false,  9, 2,  1, 1,  0, 0}, // #17
  {DBGFIELD("II_MTHI_MTLO")       1, false, false,  1, 2,  2, 1,  0, 0}, // #18
  {DBGFIELD("II_MULT")            1, false, false,  1, 2,  1, 1,  0, 0}, // #19
  {DBGFIELD("II_MULTU")           1, false, false,  1, 2,  1, 1,  0, 0}, // #20
  {DBGFIELD("II_ReturnPseudo")    0, false, false,  0, 0,  0, 0,  0, 0}, // #21
  {DBGFIELD("II_DIV")             1, false, false, 11, 2,  4, 1,  0, 0}, // #22
  {DBGFIELD("II_DIVU")            1, false, false, 13, 2,  5, 1,  0, 0}, // #23
  {DBGFIELD("II_J")               1, false, false,  3, 2,  1, 1,  0, 0}, // #24
  {DBGFIELD("II_JR")              1, false, false,  3, 2,  1, 1,  0, 0}, // #25
  {DBGFIELD("II_TRAP")            1, false, false,  3, 2,  1, 1,  0, 0}, // #26
  {DBGFIELD("II_ADD")             1, false, false,  1, 2,  1, 1,  0, 0}, // #27
  {DBGFIELD("II_ADDIUPC")         1, false, false,  1, 2,  1, 1,  0, 0}, // #28
  {DBGFIELD("II_ADDIU")           1, false, false,  1, 2,  1, 1,  0, 0}, // #29
  {DBGFIELD("II_ADDU")            1, false, false,  1, 2,  1, 1,  0, 0}, // #30
  {DBGFIELD("II_ADDI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #31
  {DBGFIELD("II_ALIGN")           1, false, false,  1, 2,  1, 1,  0, 0}, // #32
  {DBGFIELD("II_ALUIPC")          1, false, false,  1, 2,  1, 1,  0, 0}, // #33
  {DBGFIELD("II_AND")             1, false, false,  1, 2,  1, 1,  0, 0}, // #34
  {DBGFIELD("II_ANDI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #35
  {DBGFIELD("II_AUI")             1, false, false,  1, 2,  1, 1,  0, 0}, // #36
  {DBGFIELD("II_AUIPC")           1, false, false,  1, 2,  1, 1,  0, 0}, // #37
  {DBGFIELD("IIM16Alu")           1, false, false,  1, 2,  1, 1,  0, 0}, // #38
  {DBGFIELD("II_BADDU")           1, false, false,  1, 2,  1, 1,  0, 0}, // #39
  {DBGFIELD("II_BC")              1, false, false,  3, 2,  1, 1,  0, 0}, // #40
  {DBGFIELD("II_BALC")            1, false, false,  3, 2,  2, 1,  0, 0}, // #41
  {DBGFIELD("II_BBIT")            1, false, false,  1, 2,  1, 1,  0, 0}, // #42
  {DBGFIELD("II_BC1CCZ")          1, false, false, 15, 2,  2, 1,  0, 0}, // #43
  {DBGFIELD("II_BC1F")            1, false, false, 15, 2,  2, 1,  0, 0}, // #44
  {DBGFIELD("II_BC1FL")           1, false, false, 15, 2,  2, 1,  0, 0}, // #45
  {DBGFIELD("II_BC1T")            1, false, false, 15, 2,  2, 1,  0, 0}, // #46
  {DBGFIELD("II_BC1TL")           1, false, false, 15, 2,  2, 1,  0, 0}, // #47
  {DBGFIELD("II_BC2CCZ")          1, false, false,  3, 2,  2, 1,  0, 0}, // #48
  {DBGFIELD("II_BCC")             1, false, false,  3, 2,  1, 1,  0, 0}, // #49
  {DBGFIELD("II_BCCC")            1, false, false,  3, 2,  1, 1,  0, 0}, // #50
  {DBGFIELD("II_BCCZ")            1, false, false,  3, 2,  1, 1,  0, 0}, // #51
  {DBGFIELD("II_BCCZC")           1, false, false,  3, 2,  1, 1,  0, 0}, // #52
  {DBGFIELD("II_BCCZALS")         1, false, false,  3, 2,  2, 1,  0, 0}, // #53
  {DBGFIELD("II_BITSWAP")         1, false, false,  1, 2,  1, 1,  0, 0}, // #54
  {DBGFIELD("II_BREAK")           1, false, false,  3, 2,  1, 1,  0, 0}, // #55
  {DBGFIELD("II_CACHE")           1, false, false, 17, 2,  3, 1,  0, 0}, // #56
  {DBGFIELD("II_CACHEE")          1, false, false, 17, 2,  3, 1,  0, 0}, // #57
  {DBGFIELD("II_CEIL")            1, false, false,  7, 2,  3, 1,  0, 0}, // #58
  {DBGFIELD("II_CFC1")            1, false, false,  5, 2,  2, 1,  0, 0}, // #59
  {DBGFIELD("II_CFC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #60
  {DBGFIELD("II_INS")             1, false, false,  1, 2,  1, 1,  0, 0}, // #61
  {DBGFIELD("II_CLASS_D")         1, false, false, 15, 2,  6, 1,  0, 0}, // #62
  {DBGFIELD("II_CLASS_S")         1, false, false, 15, 2,  6, 1,  0, 0}, // #63
  {DBGFIELD("II_CLO")             1, false, false,  1, 2,  1, 1,  0, 0}, // #64
  {DBGFIELD("II_CLZ")             1, false, false,  1, 2,  1, 1,  0, 0}, // #65
  {DBGFIELD("II_CMP_CC_D")        1, false, false, 15, 2,  2, 1,  0, 0}, // #66
  {DBGFIELD("II_CMP_CC_S")        1, false, false, 15, 2,  2, 1,  0, 0}, // #67
  {DBGFIELD("II_CRC32B")          0, false, false,  0, 0,  0, 0,  0, 0}, // #68
  {DBGFIELD("II_CRC32CB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #69
  {DBGFIELD("II_CRC32CD")         0, false, false,  0, 0,  0, 0,  0, 0}, // #70
  {DBGFIELD("II_CRC32CH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #71
  {DBGFIELD("II_CRC32CW")         0, false, false,  0, 0,  0, 0,  0, 0}, // #72
  {DBGFIELD("II_CRC32D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #73
  {DBGFIELD("II_CRC32H")          0, false, false,  0, 0,  0, 0,  0, 0}, // #74
  {DBGFIELD("II_CRC32W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #75
  {DBGFIELD("II_CTC1")            1, false, false,  5, 2,  2, 1,  0, 0}, // #76
  {DBGFIELD("II_CTC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #77
  {DBGFIELD("II_C_CC_D")          1, false, false, 15, 2,  2, 1,  0, 0}, // #78
  {DBGFIELD("II_C_CC_S")          1, false, false, 15, 2,  2, 1,  0, 0}, // #79
  {DBGFIELD("II_DADD")            1, false, false,  1, 2,  1, 1,  0, 0}, // #80
  {DBGFIELD("II_DADDI")           1, false, false,  1, 2,  1, 1,  0, 0}, // #81
  {DBGFIELD("II_DADDIU")          1, false, false,  1, 2,  1, 1,  0, 0}, // #82
  {DBGFIELD("II_DADDU")           1, false, false,  1, 2,  1, 1,  0, 0}, // #83
  {DBGFIELD("II_DAHI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #84
  {DBGFIELD("II_DALIGN")          1, false, false,  1, 2,  1, 1,  0, 0}, // #85
  {DBGFIELD("II_DATI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #86
  {DBGFIELD("II_DAUI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #87
  {DBGFIELD("II_DBITSWAP")        1, false, false,  1, 2,  1, 1,  0, 0}, // #88
  {DBGFIELD("II_DCLO")            1, false, false,  1, 2,  1, 1,  0, 0}, // #89
  {DBGFIELD("II_DCLZ")            1, false, false,  1, 2,  1, 1,  0, 0}, // #90
  {DBGFIELD("II_DERET")           1, false, false,  3, 2,  1, 1,  0, 0}, // #91
  {DBGFIELD("II_EXT")             1, false, false,  1, 2,  1, 1,  0, 0}, // #92
  {DBGFIELD("II_DI")              1, false, false, 19, 2,  7, 1,  0, 0}, // #93
  {DBGFIELD("II_DLSA")            1, false, false,  1, 2,  1, 1,  0, 0}, // #94
  {DBGFIELD("II_DMFC0")           1, false, false, 19, 2,  2, 1,  0, 0}, // #95
  {DBGFIELD("II_DMFC1")           1, false, false,  5, 2,  2, 1,  0, 0}, // #96
  {DBGFIELD("II_DMFC2")           1, false, false, 21, 1,  1, 1,  0, 0}, // #97
  {DBGFIELD("II_DMFGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #98
  {DBGFIELD("II_DMOD")            1, false, false, 11, 2,  4, 1,  0, 0}, // #99
  {DBGFIELD("II_DMODU")           1, false, false, 11, 2,  4, 1,  0, 0}, // #100
  {DBGFIELD("II_DMT")             1, false, false,  1, 2,  2, 1,  0, 0}, // #101
  {DBGFIELD("II_DMTC0")           1, false, false, 19, 2,  7, 1,  0, 0}, // #102
  {DBGFIELD("II_DMTC1")           1, false, false,  5, 2,  2, 1,  0, 0}, // #103
  {DBGFIELD("II_DMTC2")           1, false, false, 21, 1,  1, 1,  0, 0}, // #104
  {DBGFIELD("II_DMTGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #105
  {DBGFIELD("II_DMUH")            1, false, false,  9, 2,  3, 1,  0, 0}, // #106
  {DBGFIELD("II_DMUHU")           1, false, false,  9, 2,  3, 1,  0, 0}, // #107
  {DBGFIELD("II_DMUL")            1, false, false,  9, 2,  3, 1,  0, 0}, // #108
  {DBGFIELD("II_POP")             1, false, false,  1, 2,  1, 1,  0, 0}, // #109
  {DBGFIELD("II_DROTR")           1, false, false,  1, 2,  1, 1,  0, 0}, // #110
  {DBGFIELD("II_DROTR32")         1, false, false,  1, 2,  1, 1,  0, 0}, // #111
  {DBGFIELD("II_DROTRV")          1, false, false,  1, 2,  1, 1,  0, 0}, // #112
  {DBGFIELD("II_DSBH")            1, false, false,  1, 2,  1, 1,  0, 0}, // #113
  {DBGFIELD("II_DSHD")            1, false, false,  1, 2,  1, 1,  0, 0}, // #114
  {DBGFIELD("II_DSLL")            1, false, false,  1, 2,  1, 1,  0, 0}, // #115
  {DBGFIELD("II_DSLL32")          1, false, false,  1, 2,  1, 1,  0, 0}, // #116
  {DBGFIELD("II_DSLLV")           1, false, false,  1, 2,  1, 1,  0, 0}, // #117
  {DBGFIELD("II_DSRA")            1, false, false,  1, 2,  1, 1,  0, 0}, // #118
  {DBGFIELD("II_DSRA32")          1, false, false,  1, 2,  1, 1,  0, 0}, // #119
  {DBGFIELD("II_DSRAV")           1, false, false,  1, 2,  1, 1,  0, 0}, // #120
  {DBGFIELD("II_DSRL")            1, false, false,  1, 2,  1, 1,  0, 0}, // #121
  {DBGFIELD("II_DSRL32")          1, false, false,  1, 2,  1, 1,  0, 0}, // #122
  {DBGFIELD("II_DSRLV")           1, false, false,  1, 2,  1, 1,  0, 0}, // #123
  {DBGFIELD("II_DSUB")            1, false, false,  1, 2,  1, 1,  0, 0}, // #124
  {DBGFIELD("II_DSUBU")           1, false, false,  1, 2,  1, 1,  0, 0}, // #125
  {DBGFIELD("II_DVP")             1, false, false, 19, 2,  7, 1,  0, 0}, // #126
  {DBGFIELD("II_DVPE")            1, false, false,  1, 2,  2, 1,  0, 0}, // #127
  {DBGFIELD("II_EHB")             1, false, false, 19, 2,  7, 1,  0, 0}, // #128
  {DBGFIELD("II_EI")              1, false, false, 19, 2,  7, 1,  0, 0}, // #129
  {DBGFIELD("II_EMT")             1, false, false,  1, 2,  2, 1,  0, 0}, // #130
  {DBGFIELD("II_ERET")            1, false, false,  3, 2,  1, 1,  0, 0}, // #131
  {DBGFIELD("II_ERETNC")          1, false, false,  3, 2,  1, 1,  0, 0}, // #132
  {DBGFIELD("II_EVP")             1, false, false, 19, 2,  7, 1,  0, 0}, // #133
  {DBGFIELD("II_EVPE")            1, false, false,  1, 2,  2, 1,  0, 0}, // #134
  {DBGFIELD("II_ABS")             1, false, false, 15, 2,  6, 1,  0, 0}, // #135
  {DBGFIELD("II_SQRT_D")          1, false, false, 22, 2,  8, 1,  0, 0}, // #136
  {DBGFIELD("II_ADD_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #137
  {DBGFIELD("II_ADD_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #138
  {DBGFIELD("II_DIV_D")           1, false, false, 22, 2,  9, 1,  0, 0}, // #139
  {DBGFIELD("II_DIV_S")           1, false, false, 24, 2, 10, 1,  0, 0}, // #140
  {DBGFIELD("II_FLOOR")           1, false, false,  7, 2,  3, 1,  0, 0}, // #141
  {DBGFIELD("II_MOV_D")           1, false, false,  5, 2,  6, 1,  0, 0}, // #142
  {DBGFIELD("II_MOV_S")           1, false, false,  5, 2,  6, 1,  0, 0}, // #143
  {DBGFIELD("II_MUL_D")           1, false, false,  7, 2,  3, 1,  0, 0}, // #144
  {DBGFIELD("II_MUL_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #145
  {DBGFIELD("II_NEG")             1, false, false, 15, 2,  6, 1,  0, 0}, // #146
  {DBGFIELD("II_FORK")            1, false, false, 19, 2,  1, 1,  0, 0}, // #147
  {DBGFIELD("II_SQRT_S")          1, false, false, 24, 2, 10, 1,  0, 0}, // #148
  {DBGFIELD("II_SUB_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #149
  {DBGFIELD("II_SUB_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #150
  {DBGFIELD("II_GINVI")           0, false, false,  0, 0,  0, 0,  0, 0}, // #151
  {DBGFIELD("II_GINVT")           0, false, false,  0, 0,  0, 0,  0, 0}, // #152
  {DBGFIELD("II_HYPCALL")         0, false, false,  0, 0,  0, 0,  0, 0}, // #153
  {DBGFIELD("II_JAL")             1, false, false,  3, 2,  2, 1,  0, 0}, // #154
  {DBGFIELD("II_JALR_HB")         1, false, false,  3, 2,  2, 1,  0, 0}, // #155
  {DBGFIELD("II_JALRC")           1, false, false,  3, 2,  2, 1,  0, 0}, // #156
  {DBGFIELD("II_JALRS")           1, false, false,  3, 2,  2, 1,  0, 0}, // #157
  {DBGFIELD("II_JALS")            1, false, false,  3, 2,  2, 1,  0, 0}, // #158
  {DBGFIELD("II_JIALC")           1, false, false,  3, 2,  2, 1,  0, 0}, // #159
  {DBGFIELD("II_JIC")             1, false, false,  3, 2,  1, 1,  0, 0}, // #160
  {DBGFIELD("II_JRADDIUSP")       1, false, false,  3, 2,  1, 1,  0, 0}, // #161
  {DBGFIELD("II_JRC")             1, false, false,  3, 2,  1, 1,  0, 0}, // #162
  {DBGFIELD("II_JR_HB")           1, false, false,  3, 2,  1, 1,  0, 0}, // #163
  {DBGFIELD("II_LB")              1, false, false, 17, 2,  2, 1,  0, 0}, // #164
  {DBGFIELD("II_LBE")             1, false, false, 17, 2,  2, 1,  0, 0}, // #165
  {DBGFIELD("II_LBU")             1, false, false, 17, 2,  2, 1,  0, 0}, // #166
  {DBGFIELD("II_LBUE")            1, false, false, 17, 2,  2, 1,  0, 0}, // #167
  {DBGFIELD("II_LD")              1, false, false, 17, 2,  2, 1,  0, 0}, // #168
  {DBGFIELD("II_LDC1")            1, false, false, 26, 2,  2, 1,  0, 0}, // #169
  {DBGFIELD("II_LDC2")            1, false, false, 17, 2,  2, 1,  0, 0}, // #170
  {DBGFIELD("II_LDC3")            1, false, false, 17, 2,  2, 1,  0, 0}, // #171
  {DBGFIELD("II_LDL")             1, false, false, 17, 2,  2, 1,  0, 0}, // #172
  {DBGFIELD("II_LDPC")            1, false, false, 17, 2,  2, 1,  0, 0}, // #173
  {DBGFIELD("II_LDR")             1, false, false, 17, 2,  2, 1,  0, 0}, // #174
  {DBGFIELD("II_LDXC1")           1, false, false, 26, 2,  2, 1,  0, 0}, // #175
  {DBGFIELD("II_LH")              1, false, false, 17, 2,  2, 1,  0, 0}, // #176
  {DBGFIELD("II_LHE")             1, false, false, 17, 2,  2, 1,  0, 0}, // #177
  {DBGFIELD("II_LHU")             1, false, false, 17, 2,  2, 1,  0, 0}, // #178
  {DBGFIELD("II_LHUE")            1, false, false, 17, 2,  2, 1,  0, 0}, // #179
  {DBGFIELD("II_LI")              1, false, false,  1, 2,  1, 1,  0, 0}, // #180
  {DBGFIELD("II_LL")              1, false, false, 17, 2,  2, 1,  0, 0}, // #181
  {DBGFIELD("II_LLD")             1, false, false, 17, 2,  2, 1,  0, 0}, // #182
  {DBGFIELD("II_LLE")             1, false, false, 17, 2,  2, 1,  0, 0}, // #183
  {DBGFIELD("II_LSA")             1, false, false,  1, 2,  1, 1,  0, 0}, // #184
  {DBGFIELD("II_LUI")             1, false, false,  1, 2,  1, 1,  0, 0}, // #185
  {DBGFIELD("II_LUXC1")           1, false, false, 26, 2,  2, 1,  0, 0}, // #186
  {DBGFIELD("II_LW")              1, false, false, 17, 2,  2, 1,  0, 0}, // #187
  {DBGFIELD("II_LWC1")            1, false, false, 26, 2,  2, 1,  0, 0}, // #188
  {DBGFIELD("II_LWC2")            1, false, false, 17, 2,  2, 1,  0, 0}, // #189
  {DBGFIELD("II_LWC3")            1, false, false, 17, 2,  2, 1,  0, 0}, // #190
  {DBGFIELD("II_LWE")             1, false, false, 17, 2,  2, 1,  0, 0}, // #191
  {DBGFIELD("II_LWL")             1, false, false, 17, 2,  2, 1,  0, 0}, // #192
  {DBGFIELD("II_LWLE")            1, false, false, 17, 2,  2, 1,  0, 0}, // #193
  {DBGFIELD("II_LWM")             1, false, false, 17, 2,  2, 1,  0, 0}, // #194
  {DBGFIELD("II_LWPC")            1, false, false, 17, 2,  2, 1,  0, 0}, // #195
  {DBGFIELD("II_LWP")             1, false, false, 17, 2,  2, 1,  0, 0}, // #196
  {DBGFIELD("II_LWR")             1, false, false, 17, 2,  2, 1,  0, 0}, // #197
  {DBGFIELD("II_LWRE")            1, false, false, 17, 2,  2, 1,  0, 0}, // #198
  {DBGFIELD("II_LWUPC")           1, false, false, 17, 2,  2, 1,  0, 0}, // #199
  {DBGFIELD("II_LWU")             1, false, false, 17, 2,  2, 1,  0, 0}, // #200
  {DBGFIELD("II_LWXC1")           1, false, false, 26, 2,  2, 1,  0, 0}, // #201
  {DBGFIELD("II_LWXS")            1, false, false, 17, 2,  2, 1,  0, 0}, // #202
  {DBGFIELD("II_MADDF_D")         1, false, false, 15, 2,  6, 1,  0, 0}, // #203
  {DBGFIELD("II_MADDF_S")         1, false, false, 15, 2,  6, 1,  0, 0}, // #204
  {DBGFIELD("II_MADD_D")          1, false, false,  7, 2,  3, 1,  0, 0}, // #205
  {DBGFIELD("II_MADD_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #206
  {DBGFIELD("II_MAX_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #207
  {DBGFIELD("II_MAXA_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #208
  {DBGFIELD("II_MAX_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #209
  {DBGFIELD("II_MAXA_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #210
  {DBGFIELD("II_MFC0")            1, false, false, 19, 2,  2, 1,  0, 0}, // #211
  {DBGFIELD("II_MFC2")            1, false, false, 21, 1,  1, 1,  0, 0}, // #212
  {DBGFIELD("II_MFGC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #213
  {DBGFIELD("II_MFHC0")           1, false, false, 19, 2,  2, 1,  0, 0}, // #214
  {DBGFIELD("II_MFHC1")           1, false, false,  5, 2,  2, 1,  0, 0}, // #215
  {DBGFIELD("II_MFHGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #216
  {DBGFIELD("II_MFTR")            1, false, false,  1, 2,  2, 1,  0, 0}, // #217
  {DBGFIELD("II_MIN_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #218
  {DBGFIELD("II_MINA_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #219
  {DBGFIELD("II_MIN_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #220
  {DBGFIELD("II_MINA_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #221
  {DBGFIELD("II_MOD")             1, false, false, 11, 2,  4, 1,  0, 0}, // #222
  {DBGFIELD("II_MODU")            1, false, false, 11, 2,  4, 1,  0, 0}, // #223
  {DBGFIELD("II_MOVE")            1, false, false,  1, 2,  1, 1,  0, 0}, // #224
  {DBGFIELD("II_MOVF_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #225
  {DBGFIELD("II_MOVF")            1, false, false,  5, 2,  6, 1,  0, 0}, // #226
  {DBGFIELD("II_MOVF_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #227
  {DBGFIELD("II_MOVN_D")          1, false, false,  5, 2,  6, 1,  0, 0}, // #228
  {DBGFIELD("II_MOVN")            1, false, false,  1, 2,  1, 1,  0, 0}, // #229
  {DBGFIELD("II_MOVN_S")          1, false, false,  5, 2,  6, 1,  0, 0}, // #230
  {DBGFIELD("II_MOVT_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #231
  {DBGFIELD("II_MOVT")            1, false, false,  5, 2,  6, 1,  0, 0}, // #232
  {DBGFIELD("II_MOVT_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #233
  {DBGFIELD("II_MOVZ_D")          1, false, false,  5, 2,  6, 1,  0, 0}, // #234
  {DBGFIELD("II_MOVZ")            1, false, false,  1, 2,  1, 1,  0, 0}, // #235
  {DBGFIELD("II_MOVZ_S")          1, false, false,  5, 2,  6, 1,  0, 0}, // #236
  {DBGFIELD("II_MSUBF_D")         1, false, false, 15, 2,  6, 1,  0, 0}, // #237
  {DBGFIELD("II_MSUBF_S")         1, false, false, 15, 2,  6, 1,  0, 0}, // #238
  {DBGFIELD("II_MSUB_D")          1, false, false,  7, 2,  3, 1,  0, 0}, // #239
  {DBGFIELD("II_MSUB_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #240
  {DBGFIELD("II_MTC0")            1, false, false, 19, 2,  7, 1,  0, 0}, // #241
  {DBGFIELD("II_MTC2")            1, false, false, 21, 1,  1, 1,  0, 0}, // #242
  {DBGFIELD("II_MTGC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #243
  {DBGFIELD("II_MTHC0")           1, false, false, 19, 2,  7, 1,  0, 0}, // #244
  {DBGFIELD("II_MTHC1")           1, false, false,  5, 2,  2, 1,  0, 0}, // #245
  {DBGFIELD("II_MTHGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #246
  {DBGFIELD("II_MTTR")            1, false, false,  1, 2,  2, 1,  0, 0}, // #247
  {DBGFIELD("II_MUH")             1, false, false,  9, 2,  6, 1,  0, 0}, // #248
  {DBGFIELD("II_MUHU")            1, false, false,  9, 2,  6, 1,  0, 0}, // #249
  {DBGFIELD("II_MUL")             1, false, false,  9, 2,  3, 1,  0, 0}, // #250
  {DBGFIELD("II_MULU")            1, false, false,  9, 2,  6, 1,  0, 0}, // #251
  {DBGFIELD("II_NMADD_D")         1, false, false,  7, 2,  3, 1,  0, 0}, // #252
  {DBGFIELD("II_NMADD_S")         1, false, false, 15, 2,  6, 1,  0, 0}, // #253
  {DBGFIELD("II_NMSUB_D")         1, false, false,  7, 2,  3, 1,  0, 0}, // #254
  {DBGFIELD("II_NMSUB_S")         1, false, false, 15, 2,  6, 1,  0, 0}, // #255
  {DBGFIELD("II_NOR")             1, false, false,  1, 2,  1, 1,  0, 0}, // #256
  {DBGFIELD("II_NOT")             1, false, false,  1, 2,  1, 1,  0, 0}, // #257
  {DBGFIELD("II_OR")              1, false, false,  1, 2,  1, 1,  0, 0}, // #258
  {DBGFIELD("II_ORI")             1, false, false,  1, 2,  1, 1,  0, 0}, // #259
  {DBGFIELD("II_PAUSE")           1, false, false, 19, 2,  7, 1,  0, 0}, // #260
  {DBGFIELD("II_PREF")            1, false, false, 17, 2,  1, 1,  0, 0}, // #261
  {DBGFIELD("II_PREFE")           1, false, false, 17, 2,  1, 1,  0, 0}, // #262
  {DBGFIELD("II_RDHWR")           1, false, false,  1, 2,  2, 1,  0, 0}, // #263
  {DBGFIELD("II_RDPGPR")          1, false, false, 19, 2,  7, 1,  0, 0}, // #264
  {DBGFIELD("II_RECIP_D")         1, false, false, 28, 2, 11, 1,  0, 0}, // #265
  {DBGFIELD("II_RECIP_S")         1, false, false, 30, 2, 12, 1,  0, 0}, // #266
  {DBGFIELD("II_RINT_D")          1, false, false,  7, 2,  3, 1,  0, 0}, // #267
  {DBGFIELD("II_RINT_S")          1, false, false,  7, 2,  3, 1,  0, 0}, // #268
  {DBGFIELD("II_ROTR")            1, false, false,  1, 2,  1, 1,  0, 0}, // #269
  {DBGFIELD("II_ROTRV")           1, false, false,  1, 2,  1, 1,  0, 0}, // #270
  {DBGFIELD("II_ROUND")           1, false, false,  7, 2,  3, 1,  0, 0}, // #271
  {DBGFIELD("II_RSQRT_D")         1, false, false, 28, 2, 11, 1,  0, 0}, // #272
  {DBGFIELD("II_RSQRT_S")         1, false, false, 30, 2, 12, 1,  0, 0}, // #273
  {DBGFIELD("II_RESTORE")         1, false, false, 17, 2,  2, 1,  0, 0}, // #274
  {DBGFIELD("II_SB")              1, false, false, 17, 2,  1, 1,  0, 0}, // #275
  {DBGFIELD("II_SBE")             1, false, false, 17, 2,  1, 1,  0, 0}, // #276
  {DBGFIELD("II_SC")              1, false, false, 17, 2,  2, 1,  0, 0}, // #277
  {DBGFIELD("II_SCD")             1, false, false, 17, 2,  1, 1,  0, 0}, // #278
  {DBGFIELD("II_SCE")             1, false, false, 17, 2,  1, 1,  0, 0}, // #279
  {DBGFIELD("II_SD")              1, false, false, 17, 2,  1, 1,  0, 0}, // #280
  {DBGFIELD("II_SDBBP")           1, false, false,  3, 2,  1, 1,  0, 0}, // #281
  {DBGFIELD("II_SDC1")            1, false, false, 32, 2,  1, 1,  0, 0}, // #282
  {DBGFIELD("II_SDC2")            1, false, false, 17, 2,  1, 1,  0, 0}, // #283
  {DBGFIELD("II_SDC3")            1, false, false, 17, 2,  1, 1,  0, 0}, // #284
  {DBGFIELD("II_SDL")             1, false, false, 17, 2,  1, 1,  0, 0}, // #285
  {DBGFIELD("II_SDR")             1, false, false, 17, 2,  1, 1,  0, 0}, // #286
  {DBGFIELD("II_SDXC1")           1, false, false, 32, 2,  1, 1,  0, 0}, // #287
  {DBGFIELD("II_SEB")             1, false, false,  1, 2,  1, 1,  0, 0}, // #288
  {DBGFIELD("II_SEH")             1, false, false,  1, 2,  1, 1,  0, 0}, // #289
  {DBGFIELD("II_SELCCZ")          1, false, false,  1, 2,  1, 1,  0, 0}, // #290
  {DBGFIELD("II_SELCCZ_D")        1, false, false, 15, 2,  6, 1,  0, 0}, // #291
  {DBGFIELD("II_SELCCZ_S")        1, false, false, 15, 2,  6, 1,  0, 0}, // #292
  {DBGFIELD("II_SEL_D")           1, false, false, 15, 2,  2, 1,  0, 0}, // #293
  {DBGFIELD("II_SEL_S")           1, false, false, 15, 2,  2, 1,  0, 0}, // #294
  {DBGFIELD("II_SEQ_SNE")         1, false, false,  1, 2,  1, 1,  0, 0}, // #295
  {DBGFIELD("II_SEQI_SNEI")       1, false, false,  1, 2,  1, 1,  0, 0}, // #296
  {DBGFIELD("II_SH")              1, false, false, 17, 2,  1, 1,  0, 0}, // #297
  {DBGFIELD("II_SHE")             1, false, false, 17, 2,  1, 1,  0, 0}, // #298
  {DBGFIELD("II_SLL")             1, false, false,  1, 2,  1, 1,  0, 0}, // #299
  {DBGFIELD("II_SLLV")            1, false, false,  1, 2,  1, 1,  0, 0}, // #300
  {DBGFIELD("II_SLT_SLTU")        1, false, false,  1, 2,  1, 1,  0, 0}, // #301
  {DBGFIELD("II_SLTI_SLTIU")      1, false, false,  1, 2,  1, 1,  0, 0}, // #302
  {DBGFIELD("II_SRA")             1, false, false,  1, 2,  1, 1,  0, 0}, // #303
  {DBGFIELD("II_SRAV")            1, false, false,  1, 2,  1, 1,  0, 0}, // #304
  {DBGFIELD("II_SRL")             1, false, false,  1, 2,  1, 1,  0, 0}, // #305
  {DBGFIELD("II_SRLV")            1, false, false,  1, 2,  1, 1,  0, 0}, // #306
  {DBGFIELD("II_SSNOP")           1, false, false,  1, 2,  1, 1,  0, 0}, // #307
  {DBGFIELD("II_SUB")             1, false, false,  1, 2,  1, 1,  0, 0}, // #308
  {DBGFIELD("II_SUBU")            1, false, false,  1, 2,  1, 1,  0, 0}, // #309
  {DBGFIELD("II_SUXC1")           1, false, false, 32, 2,  1, 1,  0, 0}, // #310
  {DBGFIELD("II_SW")              1, false, false, 17, 2,  1, 1,  0, 0}, // #311
  {DBGFIELD("II_SWC1")            1, false, false, 32, 2,  1, 1,  0, 0}, // #312
  {DBGFIELD("II_SWC2")            1, false, false, 17, 2,  1, 1,  0, 0}, // #313
  {DBGFIELD("II_SWC3")            1, false, false, 17, 2,  1, 1,  0, 0}, // #314
  {DBGFIELD("II_SWE")             1, false, false, 17, 2,  1, 1,  0, 0}, // #315
  {DBGFIELD("II_SWL")             1, false, false, 17, 2,  1, 1,  0, 0}, // #316
  {DBGFIELD("II_SWLE")            1, false, false, 17, 2,  1, 1,  0, 0}, // #317
  {DBGFIELD("II_SWM")             1, false, false, 17, 2,  1, 1,  0, 0}, // #318
  {DBGFIELD("II_SWP")             1, false, false, 17, 2,  1, 1,  0, 0}, // #319
  {DBGFIELD("II_SWR")             1, false, false, 17, 2,  1, 1,  0, 0}, // #320
  {DBGFIELD("II_SWRE")            1, false, false, 17, 2,  1, 1,  0, 0}, // #321
  {DBGFIELD("II_SWXC1")           1, false, false, 32, 2,  1, 1,  0, 0}, // #322
  {DBGFIELD("II_SYNC")            1, false, false, 17, 2,  1, 1,  0, 0}, // #323
  {DBGFIELD("II_SYNCI")           1, false, false, 17, 2,  1, 1,  0, 0}, // #324
  {DBGFIELD("II_SYSCALL")         1, false, false,  3, 2,  1, 1,  0, 0}, // #325
  {DBGFIELD("II_SAVE")            1, false, false, 17, 2,  1, 1,  0, 0}, // #326
  {DBGFIELD("II_TEQ")             1, false, false,  3, 2,  1, 1,  0, 0}, // #327
  {DBGFIELD("II_TEQI")            1, false, false,  3, 2,  1, 1,  0, 0}, // #328
  {DBGFIELD("II_TGE")             1, false, false,  3, 2,  1, 1,  0, 0}, // #329
  {DBGFIELD("II_TGEI")            1, false, false,  3, 2,  1, 1,  0, 0}, // #330
  {DBGFIELD("II_TGEIU")           1, false, false,  3, 2,  1, 1,  0, 0}, // #331
  {DBGFIELD("II_TGEU")            1, false, false,  3, 2,  1, 1,  0, 0}, // #332
  {DBGFIELD("II_TLBGINV")         0, false, false,  0, 0,  0, 0,  0, 0}, // #333
  {DBGFIELD("II_TLBGINVF")        0, false, false,  0, 0,  0, 0,  0, 0}, // #334
  {DBGFIELD("II_TLBGP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #335
  {DBGFIELD("II_TLBGR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #336
  {DBGFIELD("II_TLBGWI")          0, false, false,  0, 0,  0, 0,  0, 0}, // #337
  {DBGFIELD("II_TLBGWR")          0, false, false,  0, 0,  0, 0,  0, 0}, // #338
  {DBGFIELD("II_TLBINV")          1, false, false, 19, 2,  6, 1,  0, 0}, // #339
  {DBGFIELD("II_TLBINVF")         1, false, false, 19, 2,  6, 1,  0, 0}, // #340
  {DBGFIELD("II_TLBP")            1, false, false, 19, 2,  6, 1,  0, 0}, // #341
  {DBGFIELD("II_TLBR")            1, false, false, 19, 2,  6, 1,  0, 0}, // #342
  {DBGFIELD("II_TLBWI")           1, false, false, 19, 2,  6, 1,  0, 0}, // #343
  {DBGFIELD("II_TLBWR")           1, false, false, 19, 2,  6, 1,  0, 0}, // #344
  {DBGFIELD("II_TLT")             1, false, false,  3, 2,  1, 1,  0, 0}, // #345
  {DBGFIELD("II_TLTI")            1, false, false,  3, 2,  1, 1,  0, 0}, // #346
  {DBGFIELD("II_TTLTIU")          1, false, false,  3, 2,  1, 1,  0, 0}, // #347
  {DBGFIELD("II_TLTU")            1, false, false,  3, 2,  1, 1,  0, 0}, // #348
  {DBGFIELD("II_TNE")             1, false, false,  3, 2,  1, 1,  0, 0}, // #349
  {DBGFIELD("II_TNEI")            1, false, false,  3, 2,  1, 1,  0, 0}, // #350
  {DBGFIELD("II_TRUNC")           1, false, false,  7, 2,  3, 1,  0, 0}, // #351
  {DBGFIELD("II_WAIT")            1, false, false, 19, 2,  7, 1,  0, 0}, // #352
  {DBGFIELD("II_WRPGPR")          1, false, false, 19, 2,  7, 1,  0, 0}, // #353
  {DBGFIELD("II_WSBH")            1, false, false,  1, 2,  1, 1,  0, 0}, // #354
  {DBGFIELD("II_XOR")             1, false, false,  1, 2,  1, 1,  0, 0}, // #355
  {DBGFIELD("II_XORI")            1, false, false,  1, 2,  1, 1,  0, 0}, // #356
  {DBGFIELD("II_YIELD")           1, false, false, 19, 2,  3, 1,  0, 0}, // #357
  {DBGFIELD("AND")                1, false, false,  1, 2,  1, 1,  0, 0}, // #358
  {DBGFIELD("LUi")                1, false, false,  1, 2,  1, 1,  0, 0}, // #359
  {DBGFIELD("NOR")                1, false, false,  1, 2,  1, 1,  0, 0}, // #360
  {DBGFIELD("OR")                 1, false, false,  1, 2,  1, 1,  0, 0}, // #361
  {DBGFIELD("SLTi_SLTiu")         1, false, false,  1, 2,  1, 1,  0, 0}, // #362
  {DBGFIELD("SUB")                1, false, false,  1, 2,  1, 1,  0, 0}, // #363
  {DBGFIELD("SUBu")               1, false, false,  1, 2,  1, 1,  0, 0}, // #364
  {DBGFIELD("XOR")                1, false, false,  1, 2,  1, 1,  0, 0}, // #365
  {DBGFIELD("B")                  1, false, false,  3, 2,  1, 1,  0, 0}, // #366
  {DBGFIELD("BAL")                1, false, false,  3, 2,  1, 1,  0, 0}, // #367
  {DBGFIELD("BAL_BR_BGEZAL_BGEZALL_BLTZAL_BLTZALL") 1, false, false,  3, 2,  1, 1,  0, 0}, // #368
  {DBGFIELD("BEQ_BEQL_BNE_BNEL")  1, false, false,  3, 2,  1, 1,  0, 0}, // #369
  {DBGFIELD("BGEZ_BGEZL_BGTZ_BGTZL_BLEZ_BLEZL_BLTZ_BLTZL") 1, false, false,  3, 2,  1, 1,  0, 0}, // #370
  {DBGFIELD("BREAK")              1, false, false,  3, 2,  1, 1,  0, 0}, // #371
  {DBGFIELD("DERET")              1, false, false,  3, 2,  1, 1,  0, 0}, // #372
  {DBGFIELD("ERET")               1, false, false,  3, 2,  1, 1,  0, 0}, // #373
  {DBGFIELD("ERETNC")             1, false, false,  3, 2,  1, 1,  0, 0}, // #374
  {DBGFIELD("J_TAILCALL")         1, false, false,  3, 2,  1, 1,  0, 0}, // #375
  {DBGFIELD("JR_TAILCALLREG_TAILCALLREGHB") 1, false, false,  3, 2,  1, 1,  0, 0}, // #376
  {DBGFIELD("JR_HB")              1, false, false,  3, 2,  1, 1,  0, 0}, // #377
  {DBGFIELD("PseudoIndirectBranch_PseudoIndirectHazardBranch") 0, false, false,  0, 0,  0, 0,  0, 0}, // #378
  {DBGFIELD("PseudoReturn")       0, false, false,  0, 0,  0, 0,  0, 0}, // #379
  {DBGFIELD("SDBBP")              1, false, false,  3, 2,  1, 1,  0, 0}, // #380
  {DBGFIELD("SSNOP")              1, false, false,  1, 2,  1, 1,  0, 0}, // #381
  {DBGFIELD("SYSCALL")            1, false, false,  3, 2,  1, 1,  0, 0}, // #382
  {DBGFIELD("TEQ")                1, false, false,  3, 2,  1, 1,  0, 0}, // #383
  {DBGFIELD("TEQI")               1, false, false,  3, 2,  1, 1,  0, 0}, // #384
  {DBGFIELD("TGE")                1, false, false,  3, 2,  1, 1,  0, 0}, // #385
  {DBGFIELD("TGEI")               1, false, false,  3, 2,  1, 1,  0, 0}, // #386
  {DBGFIELD("TGEIU")              1, false, false,  3, 2,  1, 1,  0, 0}, // #387
  {DBGFIELD("TGEU")               1, false, false,  3, 2,  1, 1,  0, 0}, // #388
  {DBGFIELD("TLT")                1, false, false,  3, 2,  1, 1,  0, 0}, // #389
  {DBGFIELD("TLTI")               1, false, false,  3, 2,  1, 1,  0, 0}, // #390
  {DBGFIELD("TLTU")               1, false, false,  3, 2,  1, 1,  0, 0}, // #391
  {DBGFIELD("TNE")                1, false, false,  3, 2,  1, 1,  0, 0}, // #392
  {DBGFIELD("TNEI")               1, false, false,  3, 2,  1, 1,  0, 0}, // #393
  {DBGFIELD("TRAP")               1, false, false,  3, 2,  1, 1,  0, 0}, // #394
  {DBGFIELD("TTLTIU")             1, false, false,  3, 2,  1, 1,  0, 0}, // #395
  {DBGFIELD("WAIT")               1, false, false, 19, 2,  7, 1,  0, 0}, // #396
  {DBGFIELD("PAUSE")              1, false, false, 19, 2,  7, 1,  0, 0}, // #397
  {DBGFIELD("JAL")                1, false, false,  3, 2,  2, 1,  0, 0}, // #398
  {DBGFIELD("JALR_JALRHBPseudo_JALRPseudo") 1, false, false,  3, 2,  2, 1,  0, 0}, // #399
  {DBGFIELD("JALR_HB")            1, false, false,  3, 2,  2, 1,  0, 0}, // #400
  {DBGFIELD("JALX")               1, false, false,  3, 2,  2, 1,  0, 0}, // #401
  {DBGFIELD("TLBINV")             1, false, false, 19, 2,  6, 1,  0, 0}, // #402
  {DBGFIELD("TLBINVF")            1, false, false, 19, 2,  6, 1,  0, 0}, // #403
  {DBGFIELD("TLBP")               1, false, false, 19, 2,  6, 1,  0, 0}, // #404
  {DBGFIELD("TLBR")               1, false, false, 19, 2,  6, 1,  0, 0}, // #405
  {DBGFIELD("TLBWI")              1, false, false, 19, 2,  6, 1,  0, 0}, // #406
  {DBGFIELD("TLBWR")              1, false, false, 19, 2,  6, 1,  0, 0}, // #407
  {DBGFIELD("MFC0")               1, false, false, 19, 2,  2, 1,  0, 0}, // #408
  {DBGFIELD("MTC0")               1, false, false, 19, 2,  7, 1,  0, 0}, // #409
  {DBGFIELD("MFC2")               1, false, false, 21, 1,  1, 1,  0, 0}, // #410
  {DBGFIELD("MTC2")               1, false, false, 21, 1,  1, 1,  0, 0}, // #411
  {DBGFIELD("LB")                 1, false, false, 17, 2,  2, 1,  0, 0}, // #412
  {DBGFIELD("LBu")                1, false, false, 17, 2,  2, 1,  0, 0}, // #413
  {DBGFIELD("LH")                 1, false, false, 17, 2,  2, 1,  0, 0}, // #414
  {DBGFIELD("LHu")                1, false, false, 17, 2,  2, 1,  0, 0}, // #415
  {DBGFIELD("LW")                 1, false, false, 17, 2,  2, 1,  0, 0}, // #416
  {DBGFIELD("LL")                 1, false, false, 17, 2,  2, 1,  0, 0}, // #417
  {DBGFIELD("LWC2")               1, false, false, 17, 2,  2, 1,  0, 0}, // #418
  {DBGFIELD("LWC3")               1, false, false, 17, 2,  2, 1,  0, 0}, // #419
  {DBGFIELD("LDC2")               1, false, false, 17, 2,  2, 1,  0, 0}, // #420
  {DBGFIELD("LDC3")               1, false, false, 17, 2,  2, 1,  0, 0}, // #421
  {DBGFIELD("LBE")                1, false, false, 17, 2,  2, 1,  0, 0}, // #422
  {DBGFIELD("LBuE")               1, false, false, 17, 2,  2, 1,  0, 0}, // #423
  {DBGFIELD("LHE")                1, false, false, 17, 2,  2, 1,  0, 0}, // #424
  {DBGFIELD("LHuE")               1, false, false, 17, 2,  2, 1,  0, 0}, // #425
  {DBGFIELD("LWE")                1, false, false, 17, 2,  2, 1,  0, 0}, // #426
  {DBGFIELD("LLE")                1, false, false, 17, 2,  2, 1,  0, 0}, // #427
  {DBGFIELD("LWPC")               1, false, false, 17, 2,  2, 1,  0, 0}, // #428
  {DBGFIELD("LWL")                1, false, false, 17, 2,  2, 1,  0, 0}, // #429
  {DBGFIELD("LWR")                1, false, false, 17, 2,  2, 1,  0, 0}, // #430
  {DBGFIELD("LWLE")               1, false, false, 17, 2,  2, 1,  0, 0}, // #431
  {DBGFIELD("LWRE")               1, false, false, 17, 2,  2, 1,  0, 0}, // #432
  {DBGFIELD("SB")                 1, false, false, 17, 2,  1, 1,  0, 0}, // #433
  {DBGFIELD("SH")                 1, false, false, 17, 2,  1, 1,  0, 0}, // #434
  {DBGFIELD("SW")                 1, false, false, 17, 2,  1, 1,  0, 0}, // #435
  {DBGFIELD("SWC2")               1, false, false, 17, 2,  1, 1,  0, 0}, // #436
  {DBGFIELD("SWC3")               1, false, false, 17, 2,  1, 1,  0, 0}, // #437
  {DBGFIELD("SDC2")               1, false, false, 17, 2,  1, 1,  0, 0}, // #438
  {DBGFIELD("SDC3")               1, false, false, 17, 2,  1, 1,  0, 0}, // #439
  {DBGFIELD("SC")                 1, false, false, 17, 2,  2, 1,  0, 0}, // #440
  {DBGFIELD("SBE")                1, false, false, 17, 2,  1, 1,  0, 0}, // #441
  {DBGFIELD("SHE")                1, false, false, 17, 2,  1, 1,  0, 0}, // #442
  {DBGFIELD("SWE")                1, false, false, 17, 2,  1, 1,  0, 0}, // #443
  {DBGFIELD("SCE")                1, false, false, 17, 2,  1, 1,  0, 0}, // #444
  {DBGFIELD("SWL")                1, false, false, 17, 2,  1, 1,  0, 0}, // #445
  {DBGFIELD("SWR")                1, false, false, 17, 2,  1, 1,  0, 0}, // #446
  {DBGFIELD("SWLE")               1, false, false, 17, 2,  1, 1,  0, 0}, // #447
  {DBGFIELD("SWRE")               1, false, false, 17, 2,  1, 1,  0, 0}, // #448
  {DBGFIELD("PREF")               1, false, false, 17, 2,  1, 1,  0, 0}, // #449
  {DBGFIELD("PREFE")              1, false, false, 17, 2,  1, 1,  0, 0}, // #450
  {DBGFIELD("CACHE")              1, false, false, 17, 2,  3, 1,  0, 0}, // #451
  {DBGFIELD("CACHEE")             1, false, false, 17, 2,  3, 1,  0, 0}, // #452
  {DBGFIELD("SYNC")               1, false, false, 17, 2,  1, 1,  0, 0}, // #453
  {DBGFIELD("SYNCI")              1, false, false, 17, 2,  1, 1,  0, 0}, // #454
  {DBGFIELD("CLO")                1, false, false,  1, 2,  1, 1,  0, 0}, // #455
  {DBGFIELD("CLZ")                1, false, false,  1, 2,  1, 1,  0, 0}, // #456
  {DBGFIELD("DI")                 1, false, false, 19, 2,  7, 1,  0, 0}, // #457
  {DBGFIELD("EI")                 1, false, false, 19, 2,  7, 1,  0, 0}, // #458
  {DBGFIELD("MFHI_MFLO_PseudoMFHI_PseudoMFLO") 1, false, false,  1, 2,  3, 1,  0, 0}, // #459
  {DBGFIELD("EHB")                1, false, false, 19, 2,  7, 1,  0, 0}, // #460
  {DBGFIELD("RDHWR")              1, false, false,  1, 2,  2, 1,  0, 0}, // #461
  {DBGFIELD("WSBH")               1, false, false,  1, 2,  1, 1,  0, 0}, // #462
  {DBGFIELD("MOVN_I_I")           1, false, false,  1, 2,  1, 1,  0, 0}, // #463
  {DBGFIELD("MOVZ_I_I")           1, false, false,  1, 2,  1, 1,  0, 0}, // #464
  {DBGFIELD("DIV_PseudoSDIV_SDIV") 1, false, false, 11, 2,  4, 1,  0, 0}, // #465
  {DBGFIELD("DIVU_PseudoUDIV_UDIV") 1, false, false, 13, 2,  5, 1,  0, 0}, // #466
  {DBGFIELD("MUL")                1, false, false,  9, 2,  3, 1,  0, 0}, // #467
  {DBGFIELD("MULT_PseudoMULT")    1, false, false,  1, 2,  1, 1,  0, 0}, // #468
  {DBGFIELD("MULTu_PseudoMULTu")  1, false, false,  1, 2,  1, 1,  0, 0}, // #469
  {DBGFIELD("MADD_PseudoMADD")    1, false, false,  9, 2,  1, 1,  0, 0}, // #470
  {DBGFIELD("MADDU_PseudoMADDU")  1, false, false,  9, 2,  1, 1,  0, 0}, // #471
  {DBGFIELD("MSUB_PseudoMSUB")    1, false, false,  9, 2,  1, 1,  0, 0}, // #472
  {DBGFIELD("MSUBU_PseudoMSUBU")  1, false, false,  9, 2,  1, 1,  0, 0}, // #473
  {DBGFIELD("MTHI_MTLO_PseudoMTLOHI") 1, false, false,  1, 2,  2, 1,  0, 0}, // #474
  {DBGFIELD("EXT")                1, false, false,  1, 2,  1, 1,  0, 0}, // #475
  {DBGFIELD("INS")                1, false, false,  1, 2,  1, 1,  0, 0}, // #476
  {DBGFIELD("ADD")                1, false, false,  1, 2,  1, 1,  0, 0}, // #477
  {DBGFIELD("ADDi")               1, false, false,  1, 2,  1, 1,  0, 0}, // #478
  {DBGFIELD("ADDiu")              1, false, false,  1, 2,  1, 1,  0, 0}, // #479
  {DBGFIELD("ANDi")               1, false, false,  1, 2,  1, 1,  0, 0}, // #480
  {DBGFIELD("ORi")                1, false, false,  1, 2,  1, 1,  0, 0}, // #481
  {DBGFIELD("ROTR")               1, false, false,  1, 2,  1, 1,  0, 0}, // #482
  {DBGFIELD("SEB")                1, false, false,  1, 2,  1, 1,  0, 0}, // #483
  {DBGFIELD("SEH")                1, false, false,  1, 2,  1, 1,  0, 0}, // #484
  {DBGFIELD("SLT_SLTu")           1, false, false,  1, 2,  1, 1,  0, 0}, // #485
  {DBGFIELD("SLL")                1, false, false,  1, 2,  1, 1,  0, 0}, // #486
  {DBGFIELD("SRA")                1, false, false,  1, 2,  1, 1,  0, 0}, // #487
  {DBGFIELD("SRL")                1, false, false,  1, 2,  1, 1,  0, 0}, // #488
  {DBGFIELD("XORi")               1, false, false,  1, 2,  1, 1,  0, 0}, // #489
  {DBGFIELD("ADDu")               1, false, false,  1, 2,  1, 1,  0, 0}, // #490
  {DBGFIELD("SLLV")               1, false, false,  1, 2,  1, 1,  0, 0}, // #491
  {DBGFIELD("SRAV")               1, false, false,  1, 2,  1, 1,  0, 0}, // #492
  {DBGFIELD("SRLV")               1, false, false,  1, 2,  1, 1,  0, 0}, // #493
  {DBGFIELD("LSA")                1, false, false,  1, 2,  1, 1,  0, 0}, // #494
  {DBGFIELD("COPY")               1, false, false,  1, 2,  1, 1,  0, 0}, // #495
  {DBGFIELD("VSHF_B_VSHF_D_VSHF_H_VSHF_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #496
  {DBGFIELD("BINSLI_B_BINSLI_D_BINSLI_H_BINSLI_W_BINSL_B_BINSL_D_BINSL_H_BINSL_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #497
  {DBGFIELD("BINSRI_B_BINSRI_D_BINSRI_H_BINSRI_W_BINSR_B_BINSR_D_BINSR_H_BINSR_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #498
  {DBGFIELD("INSERT_B_INSERT_D_INSERT_H_INSERT_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #499
  {DBGFIELD("SLDI_B_SLDI_D_SLDI_H_SLDI_W_SLD_B_SLD_D_SLD_H_SLD_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #500
  {DBGFIELD("BSETI_B_BSETI_D_BSETI_H_BSETI_W_BSET_B_BSET_D_BSET_H_BSET_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #501
  {DBGFIELD("BCLRI_B_BCLRI_D_BCLRI_H_BCLRI_W_BCLR_B_BCLR_D_BCLR_H_BCLR_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #502
  {DBGFIELD("BNEGI_B_BNEGI_D_BNEGI_H_BNEGI_W_BNEG_B_BNEG_D_BNEG_H_BNEG_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #503
  {DBGFIELD("BSELI_B_BSEL_V")     1, false, false, 15, 2,  2, 1,  0, 0}, // #504
  {DBGFIELD("BMNZI_B_BMNZ_V_BMZI_B_BMZ_V") 1, false, false, 15, 2,  2, 1,  0, 0}, // #505
  {DBGFIELD("PCNT_B_PCNT_D_PCNT_H_PCNT_W") 1, false, false, 15, 2,  7, 1,  0, 0}, // #506
  {DBGFIELD("SAT_S_B_SAT_S_D_SAT_S_H_SAT_S_W_SAT_U_B_SAT_U_D_SAT_U_H_SAT_U_W") 1, false, false, 15, 2,  7, 1,  0, 0}, // #507
  {DBGFIELD("BNZ_B_BNZ_D_BNZ_H_BNZ_V_BNZ_W_BZ_B_BZ_D_BZ_H_BZ_V_BZ_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #508
  {DBGFIELD("CFCMSA_CTCMSA")      1, false, false, 15, 2,  1, 1,  0, 0}, // #509
  {DBGFIELD("FABS_S_FABS_D32_FABS_D64") 1, false, false, 15, 2,  6, 1,  0, 0}, // #510
  {DBGFIELD("MOVF_D32_MOVF_D64")  1, false, false, 15, 2,  6, 1,  0, 0}, // #511
  {DBGFIELD("MOVF_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #512
  {DBGFIELD("MOVT_D32_MOVT_D64")  1, false, false, 15, 2,  6, 1,  0, 0}, // #513
  {DBGFIELD("MOVT_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #514
  {DBGFIELD("FMOV_D32_FMOV_D64")  1, false, false,  5, 2,  6, 1,  0, 0}, // #515
  {DBGFIELD("FMOV_S")             1, false, false,  5, 2,  6, 1,  0, 0}, // #516
  {DBGFIELD("FNEG_S_FNEG_D32_FNEG_D64") 1, false, false, 15, 2,  6, 1,  0, 0}, // #517
  {DBGFIELD("ADD_A_B_ADD_A_D_ADD_A_H_ADD_A_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #518
  {DBGFIELD("ADDS_A_B_ADDS_A_D_ADDS_A_H_ADDS_A_W_ADDS_S_B_ADDS_S_D_ADDS_S_H_ADDS_S_W_ADDS_U_B_ADDS_U_D_ADDS_U_H_ADDS_U_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #519
  {DBGFIELD("ADDVI_B_ADDVI_D_ADDVI_H_ADDVI_W_ADDV_B_ADDV_D_ADDV_H_ADDV_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #520
  {DBGFIELD("ASUB_S_B_ASUB_S_D_ASUB_S_H_ASUB_S_W_ASUB_U_B_ASUB_U_D_ASUB_U_H_ASUB_U_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #521
  {DBGFIELD("AVER_S_B_AVER_S_D_AVER_S_H_AVER_S_W_AVER_U_B_AVER_U_D_AVER_U_H_AVER_U_W_AVE_S_B_AVE_S_D_AVE_S_H_AVE_S_W_AVE_U_B_AVE_U_D_AVE_U_H_AVE_U_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #522
  {DBGFIELD("SHF_B_SHF_H_SHF_W")  1, false, false, 15, 2,  2, 1,  0, 0}, // #523
  {DBGFIELD("FILL_B_FILL_D_FILL_H_FILL_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #524
  {DBGFIELD("SPLATI_B_SPLATI_D_SPLATI_H_SPLATI_W_SPLAT_B_SPLAT_D_SPLAT_H_SPLAT_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #525
  {DBGFIELD("MOVE_V")             1, false, false, 15, 2,  1, 1,  0, 0}, // #526
  {DBGFIELD("LDI_B_LDI_D_LDI_H_LDI_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #527
  {DBGFIELD("AND_V_NOR_V_OR_V_XOR_V") 1, false, false, 15, 2,  1, 1,  0, 0}, // #528
  {DBGFIELD("ANDI_B_NORI_B_ORI_B_XORI_B") 1, false, false, 15, 2,  1, 1,  0, 0}, // #529
  {DBGFIELD("FEXP2_D_FEXP2_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #530
  {DBGFIELD("CLTI_S_B_CLTI_S_D_CLTI_S_H_CLTI_S_W_CLTI_U_B_CLTI_U_D_CLTI_U_H_CLTI_U_W_CLT_S_B_CLT_S_D_CLT_S_H_CLT_S_W_CLT_U_B_CLT_U_D_CLT_U_H_CLT_U_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #531
  {DBGFIELD("CLEI_S_B_CLEI_S_D_CLEI_S_H_CLEI_S_W_CLEI_U_B_CLEI_U_D_CLEI_U_H_CLEI_U_W_CLE_S_B_CLE_S_D_CLE_S_H_CLE_S_W_CLE_U_B_CLE_U_D_CLE_U_H_CLE_U_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #532
  {DBGFIELD("CEQI_B_CEQI_D_CEQI_H_CEQI_W_CEQ_B_CEQ_D_CEQ_H_CEQ_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #533
  {DBGFIELD("CMP_UN_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #534
  {DBGFIELD("CMP_UN_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #535
  {DBGFIELD("CMP_UEQ_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #536
  {DBGFIELD("CMP_UEQ_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #537
  {DBGFIELD("CMP_EQ_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #538
  {DBGFIELD("CMP_EQ_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #539
  {DBGFIELD("CMP_LT_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #540
  {DBGFIELD("CMP_LT_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #541
  {DBGFIELD("CMP_ULT_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #542
  {DBGFIELD("CMP_ULT_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #543
  {DBGFIELD("CMP_LE_D")           1, false, false, 15, 2,  6, 1,  0, 0}, // #544
  {DBGFIELD("CMP_LE_S")           1, false, false, 15, 2,  6, 1,  0, 0}, // #545
  {DBGFIELD("CMP_ULE_D")          1, false, false, 15, 2,  6, 1,  0, 0}, // #546
  {DBGFIELD("CMP_ULE_S")          1, false, false, 15, 2,  6, 1,  0, 0}, // #547
  {DBGFIELD("FSAF_D_FSAF_W_FSEQ_D_FSEQ_W_FSLE_D_FSLE_W_FSLT_D_FSLT_W_FSNE_D_FSNE_W_FSOR_D_FSOR_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #548
  {DBGFIELD("FSUEQ_D_FSUEQ_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #549
  {DBGFIELD("FSULE_D_FSULE_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #550
  {DBGFIELD("FSULT_D_FSULT_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #551
  {DBGFIELD("FSUNE_D_FSUNE_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #552
  {DBGFIELD("FSUN_D_FSUN_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #553
  {DBGFIELD("FCAF_D_FCAF_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #554
  {DBGFIELD("FCEQ_D_FCEQ_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #555
  {DBGFIELD("FCLE_D_FCLE_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #556
  {DBGFIELD("FCLT_D_FCLT_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #557
  {DBGFIELD("FCNE_D_FCNE_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #558
  {DBGFIELD("FCOR_D_FCOR_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #559
  {DBGFIELD("FCUEQ_D_FCUEQ_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #560
  {DBGFIELD("FCULE_D_FCULE_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #561
  {DBGFIELD("FCULT_D_FCULT_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #562
  {DBGFIELD("FCUNE_D_FCUNE_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #563
  {DBGFIELD("FCUN_D_FCUN_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #564
  {DBGFIELD("FABS_D_FABS_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #565
  {DBGFIELD("FFINT_S_D_FFINT_S_W_FFINT_U_D_FFINT_U_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #566
  {DBGFIELD("FFQL_D_FFQL_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #567
  {DBGFIELD("FFQR_D_FFQR_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #568
  {DBGFIELD("FTINT_S_D_FTINT_S_W_FTINT_U_D_FTINT_U_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #569
  {DBGFIELD("FRINT_D_FRINT_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #570
  {DBGFIELD("FTQ_H_FTQ_W")        1, false, false, 15, 2,  6, 1,  0, 0}, // #571
  {DBGFIELD("FTRUNC_S_D_FTRUNC_S_W_FTRUNC_U_D_FTRUNC_U_W") 1, false, false, 15, 2,  6, 1,  0, 0}, // #572
  {DBGFIELD("FEXDO_H_FEXDO_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #573
  {DBGFIELD("FEXUPL_D_FEXUPL_W")  1, false, false, 15, 2,  6, 1,  0, 0}, // #574
  {DBGFIELD("FEXUPR_D_FEXUPR_W")  1, false, false, 15, 2,  6, 1,  0, 0}, // #575
  {DBGFIELD("FCLASS_D_FCLASS_W")  1, false, false, 15, 2,  6, 1,  0, 0}, // #576
  {DBGFIELD("FMAX_A_D_FMAX_A_W")  1, false, false, 15, 2,  6, 1,  0, 0}, // #577
  {DBGFIELD("FMAX_D_FMAX_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #578
  {DBGFIELD("FMIN_A_D_FMIN_A_W")  1, false, false, 15, 2,  6, 1,  0, 0}, // #579
  {DBGFIELD("FMIN_D_FMIN_W")      1, false, false, 15, 2,  6, 1,  0, 0}, // #580
  {DBGFIELD("FLOG2_D_FLOG2_W")    1, false, false, 15, 2,  6, 1,  0, 0}, // #581
  {DBGFIELD("ILVL_B_ILVL_D_ILVL_H_ILVL_W_ILVR_B_ILVR_D_ILVR_H_ILVR_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #582
  {DBGFIELD("ILVEV_B_ILVEV_D_ILVEV_H_ILVEV_W_ILVOD_B_ILVOD_D_ILVOD_H_ILVOD_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #583
  {DBGFIELD("INSVE_B_INSVE_D_INSVE_H_INSVE_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #584
  {DBGFIELD("SUBS_S_B_SUBS_S_D_SUBS_S_H_SUBS_S_W_SUBS_U_B_SUBS_U_D_SUBS_U_H_SUBS_U_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #585
  {DBGFIELD("SUBSUS_U_B_SUBSUS_U_D_SUBSUS_U_H_SUBSUS_U_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #586
  {DBGFIELD("SUBSUU_S_B_SUBSUU_S_D_SUBSUU_S_H_SUBSUU_S_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #587
  {DBGFIELD("SUBVI_B_SUBVI_D_SUBVI_H_SUBVI_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #588
  {DBGFIELD("SUBV_B_SUBV_D_SUBV_H_SUBV_W") 1, false, false, 15, 2,  2, 1,  0, 0}, // #589
  {DBGFIELD("MOD_S_B_MOD_S_D_MOD_S_H_MOD_S_W_MOD_U_B_MOD_U_D_MOD_U_H_MOD_U_W") 1, false, false, 34, 1,  4, 1,  0, 0}, // #590
  {DBGFIELD("DIV_S_B_DIV_S_D_DIV_S_H_DIV_S_W_DIV_U_B_DIV_U_D_DIV_U_H_DIV_U_W") 1, false, false, 34, 1,  4, 1,  0, 0}, // #591
  {DBGFIELD("HADD_S_D_HADD_S_H_HADD_S_W_HADD_U_D_HADD_U_H_HADD_U_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #592
  {DBGFIELD("HSUB_S_D_HSUB_S_H_HSUB_S_W_HSUB_U_D_HSUB_U_H_HSUB_U_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #593
  {DBGFIELD("MAX_S_B_MAX_S_D_MAX_S_H_MAX_S_W_MIN_S_B_MIN_S_D_MIN_S_H_MIN_S_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #594
  {DBGFIELD("MAX_U_B_MAX_U_D_MAX_U_H_MAX_U_W_MIN_U_B_MIN_U_D_MIN_U_H_MIN_U_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #595
  {DBGFIELD("MAX_A_B_MAX_A_D_MAX_A_H_MAX_A_W_MIN_A_B_MIN_A_D_MIN_A_H_MIN_A_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #596
  {DBGFIELD("MAXI_S_B_MAXI_S_D_MAXI_S_H_MAXI_S_W_MAXI_U_B_MAXI_U_D_MAXI_U_H_MAXI_U_W_MINI_S_B_MINI_S_D_MINI_S_H_MINI_S_W_MINI_U_B_MINI_U_D_MINI_U_H_MINI_U_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #597
  {DBGFIELD("SRAI_B_SRAI_D_SRAI_H_SRAI_W_SRA_B_SRA_D_SRA_H_SRA_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #598
  {DBGFIELD("SRLI_B_SRLI_D_SRLI_H_SRLI_W_SRL_B_SRL_D_SRL_H_SRL_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #599
  {DBGFIELD("SRARI_B_SRARI_D_SRARI_H_SRARI_W_SRAR_B_SRAR_D_SRAR_H_SRAR_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #600
  {DBGFIELD("SRLRI_B_SRLRI_D_SRLRI_H_SRLRI_W_SRLR_B_SRLR_D_SRLR_H_SRLR_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #601
  {DBGFIELD("SLLI_B_SLLI_D_SLLI_H_SLLI_W_SLL_B_SLL_D_SLL_H_SLL_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #602
  {DBGFIELD("PCKEV_B_PCKEV_D_PCKEV_H_PCKEV_W_PCKOD_B_PCKOD_D_PCKOD_H_PCKOD_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #603
  {DBGFIELD("NLOC_B_NLOC_D_NLOC_H_NLOC_W_NLZC_B_NLZC_D_NLZC_H_NLZC_W") 1, false, false, 15, 2,  1, 1,  0, 0}, // #604
  {DBGFIELD("FADD_D32_FADD_D64")  1, false, false, 15, 2,  6, 1,  0, 0}, // #605
  {DBGFIELD("FADD_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #606
  {DBGFIELD("FMUL_D32_FMUL_D64")  1, false, false,  7, 2,  3, 1,  0, 0}, // #607
  {DBGFIELD("FMUL_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #608
  {DBGFIELD("FSUB_D32_FSUB_D64")  1, false, false, 15, 2,  6, 1,  0, 0}, // #609
  {DBGFIELD("FSUB_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #610
  {DBGFIELD("TRUNC_L_D64_TRUNC_L_S_TRUNC_W_D32_TRUNC_W_D64_TRUNC_W_S") 1, false, false,  7, 2,  3, 1,  0, 0}, // #611
  {DBGFIELD("CVT_D32_S_CVT_D32_W_CVT_D64_L_CVT_D64_S_CVT_D64_W_CVT_L_D64_CVT_L_S_CVT_S_D32_CVT_S_D64_CVT_S_L_CVT_S_W_CVT_W_D32_CVT_W_D64_CVT_W_S") 1, false, false,  7, 2,  3, 1,  0, 0}, // #612
  {DBGFIELD("C_EQ_D32_C_EQ_D64_C_F_D32_C_F_D64_C_LE_D32_C_LE_D64_C_LT_D32_C_LT_D64_C_NGE_D32_C_NGE_D64_C_NGLE_D32_C_NGLE_D64_C_NGL_D32_C_NGL_D64_C_NGT_D32_C_NGT_D64_C_OLE_D32_C_OLE_D64_C_OLT_D32_C_OLT_D64_C_SEQ_D32_C_SEQ_D64_C_SF_D32_C_SF_D64_C_UEQ_D32_C_UEQ_D64_C_ULE_D32_C_ULE_D64_C_ULT_D32_C_ULT_D64_C_UN_D32_C_UN_D64") 1, false, false, 15, 2,  2, 1,  0, 0}, // #613
  {DBGFIELD("C_EQ_S_C_F_S_C_LE_S_C_LT_S_C_NGE_S_C_NGLE_S_C_NGL_S_C_NGT_S_C_OLE_S_C_OLT_S_C_SEQ_S_C_SF_S_C_UEQ_S_C_ULE_S_C_ULT_S_C_UN_S") 1, false, false, 15, 2,  2, 1,  0, 0}, // #614
  {DBGFIELD("FCMP_D32_FCMP_D64")  1, false, false, 15, 2,  2, 1,  0, 0}, // #615
  {DBGFIELD("FCMP_S32")           1, false, false, 15, 2,  2, 1,  0, 0}, // #616
  {DBGFIELD("PseudoCVT_D32_W_PseudoCVT_D64_L_PseudoCVT_D64_W_PseudoCVT_S_L_PseudoCVT_S_W") 1, false, false,  7, 2,  3, 1,  0, 0}, // #617
  {DBGFIELD("FDIV_S")             1, false, false, 24, 2, 10, 1,  0, 0}, // #618
  {DBGFIELD("FDIV_D32_FDIV_D64")  1, false, false, 22, 2,  9, 1,  0, 0}, // #619
  {DBGFIELD("FSQRT_S")            1, false, false, 24, 2, 10, 1,  0, 0}, // #620
  {DBGFIELD("FSQRT_D32_FSQRT_D64") 1, false, false, 22, 2,  8, 1,  0, 0}, // #621
  {DBGFIELD("FRCP_D_FRCP_W")      1, false, false,  7, 2,  3, 1,  0, 0}, // #622
  {DBGFIELD("FRSQRT_D_FRSQRT_W")  1, false, false,  7, 2,  3, 1,  0, 0}, // #623
  {DBGFIELD("RECIP_D32_RECIP_D64") 1, false, false, 28, 2, 11, 1,  0, 0}, // #624
  {DBGFIELD("RSQRT_D32_RSQRT_D64") 1, false, false, 28, 2, 11, 1,  0, 0}, // #625
  {DBGFIELD("RECIP_S")            1, false, false, 30, 2, 12, 1,  0, 0}, // #626
  {DBGFIELD("RSQRT_S")            1, false, false, 30, 2, 12, 1,  0, 0}, // #627
  {DBGFIELD("FMADD_D_FMADD_W")    1, false, false,  7, 2,  3, 1,  0, 0}, // #628
  {DBGFIELD("FMSUB_D_FMSUB_W")    1, false, false,  7, 2,  3, 1,  0, 0}, // #629
  {DBGFIELD("FDIV_W")             1, false, false,  7, 2,  3, 1,  0, 0}, // #630
  {DBGFIELD("FDIV_D")             1, false, false,  7, 2,  3, 1,  0, 0}, // #631
  {DBGFIELD("FSQRT_W")            1, false, false,  7, 2,  3, 1,  0, 0}, // #632
  {DBGFIELD("FSQRT_D")            1, false, false,  7, 2,  3, 1,  0, 0}, // #633
  {DBGFIELD("FMUL_D_FMUL_W")      1, false, false,  7, 2,  3, 1,  0, 0}, // #634
  {DBGFIELD("FADD_D_FADD_W")      1, false, false,  7, 2,  3, 1,  0, 0}, // #635
  {DBGFIELD("FSUB_D_FSUB_W")      1, false, false,  7, 2,  3, 1,  0, 0}, // #636
  {DBGFIELD("DPADD_S_D_DPADD_S_H_DPADD_S_W_DPADD_U_D_DPADD_U_H_DPADD_U_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #637
  {DBGFIELD("DPSUB_S_D_DPSUB_S_H_DPSUB_S_W_DPSUB_U_D_DPSUB_U_H_DPSUB_U_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #638
  {DBGFIELD("DOTP_S_D_DOTP_S_H_DOTP_S_W_DOTP_U_D_DOTP_U_H_DOTP_U_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #639
  {DBGFIELD("MSUBV_B_MSUBV_D_MSUBV_H_MSUBV_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #640
  {DBGFIELD("MADDV_B_MADDV_D_MADDV_H_MADDV_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #641
  {DBGFIELD("MULV_B_MULV_D_MULV_H_MULV_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #642
  {DBGFIELD("MADDR_Q_H_MADDR_Q_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #643
  {DBGFIELD("MADD_Q_H_MADD_Q_W")  1, false, false, 15, 2,  3, 1,  0, 0}, // #644
  {DBGFIELD("MSUBR_Q_H_MSUBR_Q_W") 1, false, false, 15, 2,  3, 1,  0, 0}, // #645
  {DBGFIELD("MSUB_Q_H_MSUB_Q_W")  1, false, false, 15, 2,  3, 1,  0, 0}, // #646
  {DBGFIELD("MULR_Q_H_MULR_Q_W")  1, false, false, 15, 2,  3, 1,  0, 0}, // #647
  {DBGFIELD("MUL_Q_H_MUL_Q_W")    1, false, false, 15, 2,  3, 1,  0, 0}, // #648
  {DBGFIELD("MADD_D32_MADD_D64")  1, false, false,  7, 2,  3, 1,  0, 0}, // #649
  {DBGFIELD("MADD_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #650
  {DBGFIELD("MSUB_D32_MSUB_D64")  1, false, false,  7, 2,  3, 1,  0, 0}, // #651
  {DBGFIELD("MSUB_S")             1, false, false, 15, 2,  6, 1,  0, 0}, // #652
  {DBGFIELD("NMADD_D32_NMADD_D64") 1, false, false,  7, 2,  3, 1,  0, 0}, // #653
  {DBGFIELD("NMADD_S")            1, false, false, 15, 2,  6, 1,  0, 0}, // #654
  {DBGFIELD("NMSUB_D32_NMSUB_D64") 1, false, false,  7, 2,  3, 1,  0, 0}, // #655
  {DBGFIELD("NMSUB_S")            1, false, false, 15, 2,  6, 1,  0, 0}, // #656
  {DBGFIELD("CTC1")               1, false, false,  5, 2,  2, 1,  0, 0}, // #657
  {DBGFIELD("MTC1_MTC1_D64_BuildPairF64_BuildPairF64_64") 1, false, false,  5, 2,  2, 1,  0, 0}, // #658
  {DBGFIELD("MTHC1_D32_MTHC1_D64") 1, false, false,  5, 2,  2, 1,  0, 0}, // #659
  {DBGFIELD("COPY_U_B_COPY_U_H_COPY_U_W") 1, false, false,  5, 2,  2, 1,  0, 0}, // #660
  {DBGFIELD("COPY_S_B_COPY_S_D_COPY_S_H_COPY_S_W") 1, false, false,  5, 2,  2, 1,  0, 0}, // #661
  {DBGFIELD("BC1F")               1, false, false, 15, 2,  2, 1,  0, 0}, // #662
  {DBGFIELD("BC1FL")              1, false, false, 15, 2,  2, 1,  0, 0}, // #663
  {DBGFIELD("BC1T")               1, false, false, 15, 2,  2, 1,  0, 0}, // #664
  {DBGFIELD("BC1TL")              1, false, false, 15, 2,  2, 1,  0, 0}, // #665
  {DBGFIELD("CFC1")               1, false, false,  5, 2,  2, 1,  0, 0}, // #666
  {DBGFIELD("MFC1_MFC1_D64_ExtractElementF64_ExtractElementF64_64") 1, false, false,  5, 2,  2, 1,  0, 0}, // #667
  {DBGFIELD("MFHC1_D32_MFHC1_D64") 1, false, false,  5, 2,  2, 1,  0, 0}, // #668
  {DBGFIELD("MOVF_I")             1, false, false,  5, 2,  6, 1,  0, 0}, // #669
  {DBGFIELD("MOVT_I")             1, false, false,  5, 2,  6, 1,  0, 0}, // #670
  {DBGFIELD("SDC1")               1, false, false, 32, 2,  1, 1,  0, 0}, // #671
  {DBGFIELD("SDXC1")              1, false, false, 32, 2,  1, 1,  0, 0}, // #672
  {DBGFIELD("SUXC1")              1, false, false, 32, 2,  1, 1,  0, 0}, // #673
  {DBGFIELD("SWC1")               1, false, false, 32, 2,  1, 1,  0, 0}, // #674
  {DBGFIELD("SWXC1")              1, false, false, 32, 2,  1, 1,  0, 0}, // #675
  {DBGFIELD("ST_B_ST_D_ST_H_ST_W") 1, false, false, 32, 2,  1, 1,  0, 0}, // #676
  {DBGFIELD("MOVN_I_D32_MOVN_I_D64") 1, false, false,  5, 2,  6, 1,  0, 0}, // #677
  {DBGFIELD("MOVN_I_S")           1, false, false,  5, 2,  6, 1,  0, 0}, // #678
  {DBGFIELD("MOVZ_I_D32_MOVZ_I_D64") 1, false, false,  5, 2,  6, 1,  0, 0}, // #679
  {DBGFIELD("MOVZ_I_S")           1, false, false,  5, 2,  6, 1,  0, 0}, // #680
  {DBGFIELD("LDC1")               1, false, false, 26, 2,  2, 1,  0, 0}, // #681
  {DBGFIELD("LDXC1")              1, false, false, 26, 2,  2, 1,  0, 0}, // #682
  {DBGFIELD("LWC1")               1, false, false, 26, 2,  2, 1,  0, 0}, // #683
  {DBGFIELD("LWXC1")              1, false, false, 26, 2,  2, 1,  0, 0}, // #684
  {DBGFIELD("LUXC1")              1, false, false, 26, 2,  2, 1,  0, 0}, // #685
  {DBGFIELD("LD_B_LD_D_LD_H_LD_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #686
  {DBGFIELD("CEIL_L_D64_CEIL_L_S_CEIL_W_D32_CEIL_W_D64_CEIL_W_S") 1, false, false,  7, 2,  3, 1,  0, 0}, // #687
  {DBGFIELD("FLOOR_L_D64_FLOOR_L_S_FLOOR_W_D32_FLOOR_W_D64_FLOOR_W_S") 1, false, false,  7, 2,  3, 1,  0, 0}, // #688
  {DBGFIELD("ROUND_L_D64_ROUND_L_S_ROUND_W_D32_ROUND_W_D64_ROUND_W_S") 1, false, false,  7, 2,  3, 1,  0, 0}, // #689
  {DBGFIELD("ROTRV")              1, false, false,  1, 2,  1, 1,  0, 0}, // #690
  {DBGFIELD("EXTRV_RS_W")         1, false, false, 35, 1, 13, 1,  0, 0}, // #691
  {DBGFIELD("EXTRV_R_W")          1, false, false, 35, 1, 13, 1,  0, 0}, // #692
  {DBGFIELD("EXTRV_S_H")          1, false, false, 35, 1, 13, 1,  0, 0}, // #693
  {DBGFIELD("EXTRV_W")            1, false, false, 35, 1, 13, 1,  0, 0}, // #694
  {DBGFIELD("EXTR_RS_W")          1, false, false, 35, 1, 13, 1,  0, 0}, // #695
  {DBGFIELD("EXTR_R_W")           1, false, false, 35, 1, 13, 1,  0, 0}, // #696
  {DBGFIELD("EXTR_S_H")           1, false, false, 35, 1, 13, 1,  0, 0}, // #697
  {DBGFIELD("EXTR_W")             1, false, false, 35, 1, 13, 1,  0, 0}, // #698
  {DBGFIELD("INSV")               1, false, false, 35, 1, 13, 1,  0, 0}, // #699
  {DBGFIELD("MTHLIP")             1, false, false, 35, 1,  3, 1,  0, 0}, // #700
  {DBGFIELD("MTHI_DSP")           1, false, false, 35, 1,  3, 1,  0, 0}, // #701
  {DBGFIELD("MTLO_DSP")           1, false, false, 35, 1,  3, 1,  0, 0}, // #702
  {DBGFIELD("ABSQ_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #703
  {DBGFIELD("ABSQ_S_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #704
  {DBGFIELD("ADDQ_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #705
  {DBGFIELD("ADDQ_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #706
  {DBGFIELD("ADDQ_S_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #707
  {DBGFIELD("ADDSC")              1, false, false, 35, 1,  2, 1,  0, 0}, // #708
  {DBGFIELD("ADDU_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #709
  {DBGFIELD("ADDU_S_QB")          1, false, false, 35, 1,  2, 1,  0, 0}, // #710
  {DBGFIELD("ADDWC")              1, false, false, 35, 1,  2, 1,  0, 0}, // #711
  {DBGFIELD("BITREV")             1, false, false, 35, 1,  2, 1,  0, 0}, // #712
  {DBGFIELD("BPOSGE32")           1, false, false, 35, 1,  2, 1,  0, 0}, // #713
  {DBGFIELD("CMPGU_EQ_QB")        1, false, false, 35, 1,  2, 1,  0, 0}, // #714
  {DBGFIELD("CMPGU_LE_QB")        1, false, false, 35, 1,  2, 1,  0, 0}, // #715
  {DBGFIELD("CMPGU_LT_QB")        1, false, false, 35, 1,  2, 1,  0, 0}, // #716
  {DBGFIELD("CMPU_EQ_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #717
  {DBGFIELD("CMPU_LE_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #718
  {DBGFIELD("CMPU_LT_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #719
  {DBGFIELD("CMP_EQ_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #720
  {DBGFIELD("CMP_LE_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #721
  {DBGFIELD("CMP_LT_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #722
  {DBGFIELD("DPAQ_SA_L_W")        1, false, false, 35, 1,  2, 1,  0, 0}, // #723
  {DBGFIELD("DPAQ_S_W_PH")        1, false, false, 35, 1,  2, 1,  0, 0}, // #724
  {DBGFIELD("DPAU_H_QBL")         1, false, false, 35, 1,  2, 1,  0, 0}, // #725
  {DBGFIELD("DPAU_H_QBR")         1, false, false, 35, 1,  2, 1,  0, 0}, // #726
  {DBGFIELD("DPSQ_SA_L_W")        1, false, false, 35, 1,  2, 1,  0, 0}, // #727
  {DBGFIELD("DPSQ_S_W_PH")        1, false, false, 35, 1,  2, 1,  0, 0}, // #728
  {DBGFIELD("DPSU_H_QBL")         1, false, false, 35, 1,  2, 1,  0, 0}, // #729
  {DBGFIELD("DPSU_H_QBR")         1, false, false, 35, 1,  2, 1,  0, 0}, // #730
  {DBGFIELD("EXTPDPV")            1, false, false, 35, 1,  2, 1,  0, 0}, // #731
  {DBGFIELD("EXTPDP")             1, false, false, 35, 1,  2, 1,  0, 0}, // #732
  {DBGFIELD("EXTPV")              1, false, false, 35, 1,  2, 1,  0, 0}, // #733
  {DBGFIELD("EXTP")               1, false, false, 35, 1,  2, 1,  0, 0}, // #734
  {DBGFIELD("LBUX")               1, false, false, 35, 1,  2, 1,  0, 0}, // #735
  {DBGFIELD("LHX")                1, false, false, 35, 1,  2, 1,  0, 0}, // #736
  {DBGFIELD("LWX")                1, false, false, 35, 1,  2, 1,  0, 0}, // #737
  {DBGFIELD("MADDU_DSP")          1, false, false, 35, 1,  2, 1,  0, 0}, // #738
  {DBGFIELD("MADD_DSP")           1, false, false, 35, 1,  2, 1,  0, 0}, // #739
  {DBGFIELD("MAQ_SA_W_PHL")       1, false, false, 35, 1,  2, 1,  0, 0}, // #740
  {DBGFIELD("MAQ_SA_W_PHR")       1, false, false, 35, 1,  2, 1,  0, 0}, // #741
  {DBGFIELD("MAQ_S_W_PHL")        1, false, false, 35, 1,  2, 1,  0, 0}, // #742
  {DBGFIELD("MAQ_S_W_PHR")        1, false, false, 35, 1,  2, 1,  0, 0}, // #743
  {DBGFIELD("MFHI_DSP")           1, false, false, 35, 1,  2, 1,  0, 0}, // #744
  {DBGFIELD("MFLO_DSP")           1, false, false, 35, 1,  2, 1,  0, 0}, // #745
  {DBGFIELD("MODSUB")             1, false, false, 35, 1,  2, 1,  0, 0}, // #746
  {DBGFIELD("MSUBU_DSP")          1, false, false, 35, 1,  2, 1,  0, 0}, // #747
  {DBGFIELD("MSUB_DSP")           1, false, false, 35, 1,  2, 1,  0, 0}, // #748
  {DBGFIELD("MULEQ_S_W_PHL")      1, false, false, 35, 1,  2, 1,  0, 0}, // #749
  {DBGFIELD("MULEQ_S_W_PHR")      1, false, false, 35, 1,  2, 1,  0, 0}, // #750
  {DBGFIELD("MULEU_S_PH_QBL")     1, false, false, 35, 1,  2, 1,  0, 0}, // #751
  {DBGFIELD("MULEU_S_PH_QBR")     1, false, false, 35, 1,  2, 1,  0, 0}, // #752
  {DBGFIELD("MULQ_RS_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #753
  {DBGFIELD("MULSAQ_S_W_PH")      1, false, false, 35, 1,  2, 1,  0, 0}, // #754
  {DBGFIELD("MULTU_DSP")          1, false, false, 35, 1,  2, 1,  0, 0}, // #755
  {DBGFIELD("MULT_DSP")           1, false, false, 35, 1,  2, 1,  0, 0}, // #756
  {DBGFIELD("PACKRL_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #757
  {DBGFIELD("PICK_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #758
  {DBGFIELD("PICK_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #759
  {DBGFIELD("PRECEQU_PH_QBLA")    1, false, false, 35, 1,  2, 1,  0, 0}, // #760
  {DBGFIELD("PRECEQU_PH_QBL")     1, false, false, 35, 1,  2, 1,  0, 0}, // #761
  {DBGFIELD("PRECEQU_PH_QBRA")    1, false, false, 35, 1,  2, 1,  0, 0}, // #762
  {DBGFIELD("PRECEQU_PH_QBR")     1, false, false, 35, 1,  2, 1,  0, 0}, // #763
  {DBGFIELD("PRECEQ_W_PHL")       1, false, false, 35, 1,  2, 1,  0, 0}, // #764
  {DBGFIELD("PRECEQ_W_PHR")       1, false, false, 35, 1,  2, 1,  0, 0}, // #765
  {DBGFIELD("PRECEU_PH_QBLA")     1, false, false, 35, 1,  2, 1,  0, 0}, // #766
  {DBGFIELD("PRECEU_PH_QBL")      1, false, false, 35, 1,  2, 1,  0, 0}, // #767
  {DBGFIELD("PRECEU_PH_QBRA")     1, false, false, 35, 1,  2, 1,  0, 0}, // #768
  {DBGFIELD("PRECEU_PH_QBR")      1, false, false, 35, 1,  2, 1,  0, 0}, // #769
  {DBGFIELD("PRECRQU_S_QB_PH")    1, false, false, 35, 1,  2, 1,  0, 0}, // #770
  {DBGFIELD("PRECRQ_PH_W")        1, false, false, 35, 1,  2, 1,  0, 0}, // #771
  {DBGFIELD("PRECRQ_QB_PH")       1, false, false, 35, 1,  2, 1,  0, 0}, // #772
  {DBGFIELD("PRECRQ_RS_PH_W")     1, false, false, 35, 1,  2, 1,  0, 0}, // #773
  {DBGFIELD("RADDU_W_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #774
  {DBGFIELD("RDDSP")              1, false, false, 35, 1,  2, 1,  0, 0}, // #775
  {DBGFIELD("REPLV_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #776
  {DBGFIELD("REPLV_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #777
  {DBGFIELD("REPL_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #778
  {DBGFIELD("REPL_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #779
  {DBGFIELD("SHILOV")             1, false, false, 35, 1,  2, 1,  0, 0}, // #780
  {DBGFIELD("SHILO")              1, false, false, 35, 1,  2, 1,  0, 0}, // #781
  {DBGFIELD("SHLLV_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #782
  {DBGFIELD("SHLLV_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #783
  {DBGFIELD("SHLLV_S_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #784
  {DBGFIELD("SHLLV_S_W")          1, false, false, 35, 1,  2, 1,  0, 0}, // #785
  {DBGFIELD("SHLL_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #786
  {DBGFIELD("SHLL_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #787
  {DBGFIELD("SHLL_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #788
  {DBGFIELD("SHLL_S_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #789
  {DBGFIELD("SHRAV_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #790
  {DBGFIELD("SHRAV_R_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #791
  {DBGFIELD("SHRAV_R_W")          1, false, false, 35, 1,  2, 1,  0, 0}, // #792
  {DBGFIELD("SHRA_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #793
  {DBGFIELD("SHRA_R_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #794
  {DBGFIELD("SHRA_R_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #795
  {DBGFIELD("SHRLV_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #796
  {DBGFIELD("SHRL_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #797
  {DBGFIELD("SUBQ_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #798
  {DBGFIELD("SUBQ_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #799
  {DBGFIELD("SUBQ_S_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #800
  {DBGFIELD("SUBU_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #801
  {DBGFIELD("SUBU_S_QB")          1, false, false, 35, 1,  2, 1,  0, 0}, // #802
  {DBGFIELD("WRDSP")              1, false, false, 35, 1,  2, 1,  0, 0}, // #803
  {DBGFIELD("ABSQ_S_QB")          1, false, false, 35, 1,  2, 1,  0, 0}, // #804
  {DBGFIELD("ADDQH_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #805
  {DBGFIELD("ADDQH_R_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #806
  {DBGFIELD("ADDQH_R_W")          1, false, false, 35, 1,  2, 1,  0, 0}, // #807
  {DBGFIELD("ADDQH_W")            1, false, false, 35, 1,  2, 1,  0, 0}, // #808
  {DBGFIELD("ADDUH_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #809
  {DBGFIELD("ADDUH_R_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #810
  {DBGFIELD("ADDU_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #811
  {DBGFIELD("ADDU_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #812
  {DBGFIELD("APPEND")             1, false, false, 35, 1,  2, 1,  0, 0}, // #813
  {DBGFIELD("BALIGN")             1, false, false, 35, 1,  2, 1,  0, 0}, // #814
  {DBGFIELD("CMPGDU_EQ_QB")       1, false, false, 35, 1,  2, 1,  0, 0}, // #815
  {DBGFIELD("CMPGDU_LE_QB")       1, false, false, 35, 1,  2, 1,  0, 0}, // #816
  {DBGFIELD("CMPGDU_LT_QB")       1, false, false, 35, 1,  2, 1,  0, 0}, // #817
  {DBGFIELD("DPA_W_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #818
  {DBGFIELD("DPAQX_SA_W_PH")      1, false, false, 35, 1,  2, 1,  0, 0}, // #819
  {DBGFIELD("DPAQX_S_W_PH")       1, false, false, 35, 1,  2, 1,  0, 0}, // #820
  {DBGFIELD("DPAX_W_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #821
  {DBGFIELD("DPS_W_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #822
  {DBGFIELD("DPSQX_S_W_PH")       1, false, false, 35, 1,  2, 1,  0, 0}, // #823
  {DBGFIELD("DPSQX_SA_W_PH")      1, false, false, 35, 1,  2, 1,  0, 0}, // #824
  {DBGFIELD("DPSX_W_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #825
  {DBGFIELD("MUL_PH")             1, false, false, 35, 1,  2, 1,  0, 0}, // #826
  {DBGFIELD("MUL_S_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #827
  {DBGFIELD("MULQ_RS_W")          1, false, false, 35, 1,  2, 1,  0, 0}, // #828
  {DBGFIELD("MULQ_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #829
  {DBGFIELD("MULQ_S_W")           1, false, false, 35, 1,  2, 1,  0, 0}, // #830
  {DBGFIELD("MULSA_W_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #831
  {DBGFIELD("PRECR_QB_PH")        1, false, false, 35, 1,  2, 1,  0, 0}, // #832
  {DBGFIELD("PRECR_SRA_PH_W")     1, false, false, 35, 1,  2, 1,  0, 0}, // #833
  {DBGFIELD("PRECR_SRA_R_PH_W")   1, false, false, 35, 1,  2, 1,  0, 0}, // #834
  {DBGFIELD("PREPEND")            1, false, false, 35, 1,  2, 1,  0, 0}, // #835
  {DBGFIELD("SHRA_QB")            1, false, false, 35, 1,  2, 1,  0, 0}, // #836
  {DBGFIELD("SHRA_R_QB")          1, false, false, 35, 1,  2, 1,  0, 0}, // #837
  {DBGFIELD("SHRAV_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #838
  {DBGFIELD("SHRAV_R_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #839
  {DBGFIELD("SHRL_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #840
  {DBGFIELD("SHRLV_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #841
  {DBGFIELD("SUBQH_PH")           1, false, false, 35, 1,  2, 1,  0, 0}, // #842
  {DBGFIELD("SUBQH_R_PH")         1, false, false, 35, 1,  2, 1,  0, 0}, // #843
  {DBGFIELD("SUBQH_W")            1, false, false, 35, 1,  2, 1,  0, 0}, // #844
  {DBGFIELD("SUBQH_R_W")          1, false, false, 35, 1,  2, 1,  0, 0}, // #845
  {DBGFIELD("SUBU_PH")            1, false, false, 35, 1,  2, 1,  0, 0}, // #846
  {DBGFIELD("SUBU_S_PH")          1, false, false, 35, 1,  2, 1,  0, 0}, // #847
  {DBGFIELD("SUBUH_QB")           1, false, false, 35, 1,  2, 1,  0, 0}, // #848
  {DBGFIELD("SUBUH_R_QB")         1, false, false, 35, 1,  2, 1,  0, 0}, // #849
  {DBGFIELD("ABSQ_S_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #850
  {DBGFIELD("ABSQ_S_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #851
  {DBGFIELD("ADDQ_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #852
  {DBGFIELD("ADDQ_S_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #853
  {DBGFIELD("ADDQ_S_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #854
  {DBGFIELD("ADDSC_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #855
  {DBGFIELD("ADDU_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #856
  {DBGFIELD("ADDU_S_QB_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #857
  {DBGFIELD("ADDWC_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #858
  {DBGFIELD("BITREV_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #859
  {DBGFIELD("BPOSGE32_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #860
  {DBGFIELD("CMPGU_EQ_QB_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #861
  {DBGFIELD("CMPGU_LE_QB_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #862
  {DBGFIELD("CMPGU_LT_QB_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #863
  {DBGFIELD("CMPU_EQ_QB_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #864
  {DBGFIELD("CMPU_LE_QB_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #865
  {DBGFIELD("CMPU_LT_QB_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #866
  {DBGFIELD("CMP_EQ_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #867
  {DBGFIELD("CMP_LE_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #868
  {DBGFIELD("CMP_LT_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #869
  {DBGFIELD("DPAQ_SA_L_W_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #870
  {DBGFIELD("DPAQ_S_W_PH_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #871
  {DBGFIELD("DPAU_H_QBL_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #872
  {DBGFIELD("DPAU_H_QBR_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #873
  {DBGFIELD("DPSQ_SA_L_W_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #874
  {DBGFIELD("DPSQ_S_W_PH_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #875
  {DBGFIELD("DPSU_H_QBL_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #876
  {DBGFIELD("DPSU_H_QBR_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #877
  {DBGFIELD("EXTPDPV_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #878
  {DBGFIELD("EXTPDP_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #879
  {DBGFIELD("EXTPV_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #880
  {DBGFIELD("EXTP_MM")            1, false, false, 35, 1,  2, 1,  0, 0}, // #881
  {DBGFIELD("EXTRV_RS_W_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #882
  {DBGFIELD("EXTRV_R_W_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #883
  {DBGFIELD("EXTRV_S_H_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #884
  {DBGFIELD("EXTRV_W_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #885
  {DBGFIELD("EXTR_RS_W_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #886
  {DBGFIELD("EXTR_R_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #887
  {DBGFIELD("EXTR_S_H_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #888
  {DBGFIELD("EXTR_W_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #889
  {DBGFIELD("INSV_MM")            1, false, false, 35, 1,  2, 1,  0, 0}, // #890
  {DBGFIELD("LBUX_MM")            1, false, false, 35, 1,  2, 1,  0, 0}, // #891
  {DBGFIELD("LHX_MM")             1, false, false, 35, 1,  2, 1,  0, 0}, // #892
  {DBGFIELD("LWX_MM")             1, false, false, 35, 1,  2, 1,  0, 0}, // #893
  {DBGFIELD("MADDU_DSP_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #894
  {DBGFIELD("MADD_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #895
  {DBGFIELD("MAQ_SA_W_PHL_MM")    1, false, false, 35, 1,  2, 1,  0, 0}, // #896
  {DBGFIELD("MAQ_SA_W_PHR_MM")    1, false, false, 35, 1,  2, 1,  0, 0}, // #897
  {DBGFIELD("MAQ_S_W_PHL_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #898
  {DBGFIELD("MAQ_S_W_PHR_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #899
  {DBGFIELD("MFHI_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #900
  {DBGFIELD("MFLO_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #901
  {DBGFIELD("MODSUB_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #902
  {DBGFIELD("MOVEP_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #903
  {DBGFIELD("MOVEP_MMR6")         1, false, false, 35, 1,  2, 1,  0, 0}, // #904
  {DBGFIELD("MOVN_I_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #905
  {DBGFIELD("MOVZ_I_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #906
  {DBGFIELD("MSUBU_DSP_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #907
  {DBGFIELD("MSUB_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #908
  {DBGFIELD("MTHI_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #909
  {DBGFIELD("MTHLIP_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #910
  {DBGFIELD("MTLO_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #911
  {DBGFIELD("MULEQ_S_W_PHL_MM")   1, false, false, 35, 1,  2, 1,  0, 0}, // #912
  {DBGFIELD("MULEQ_S_W_PHR_MM")   1, false, false, 35, 1,  2, 1,  0, 0}, // #913
  {DBGFIELD("MULEU_S_PH_QBL_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #914
  {DBGFIELD("MULEU_S_PH_QBR_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #915
  {DBGFIELD("MULQ_RS_PH_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #916
  {DBGFIELD("MULSAQ_S_W_PH_MM")   1, false, false, 35, 1,  2, 1,  0, 0}, // #917
  {DBGFIELD("MULTU_DSP_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #918
  {DBGFIELD("MULT_DSP_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #919
  {DBGFIELD("PACKRL_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #920
  {DBGFIELD("PICK_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #921
  {DBGFIELD("PICK_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #922
  {DBGFIELD("PRECEQU_PH_QBLA_MM") 1, false, false, 35, 1,  2, 1,  0, 0}, // #923
  {DBGFIELD("PRECEQU_PH_QBL_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #924
  {DBGFIELD("PRECEQU_PH_QBRA_MM") 1, false, false, 35, 1,  2, 1,  0, 0}, // #925
  {DBGFIELD("PRECEQU_PH_QBR_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #926
  {DBGFIELD("PRECEQ_W_PHL_MM")    1, false, false, 35, 1,  2, 1,  0, 0}, // #927
  {DBGFIELD("PRECEQ_W_PHR_MM")    1, false, false, 35, 1,  2, 1,  0, 0}, // #928
  {DBGFIELD("PRECEU_PH_QBLA_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #929
  {DBGFIELD("PRECEU_PH_QBL_MM")   1, false, false, 35, 1,  2, 1,  0, 0}, // #930
  {DBGFIELD("PRECEU_PH_QBRA_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #931
  {DBGFIELD("PRECEU_PH_QBR_MM")   1, false, false, 35, 1,  2, 1,  0, 0}, // #932
  {DBGFIELD("PRECRQU_S_QB_PH_MM") 1, false, false, 35, 1,  2, 1,  0, 0}, // #933
  {DBGFIELD("PRECRQ_PH_W_MM")     1, false, false, 35, 1,  2, 1,  0, 0}, // #934
  {DBGFIELD("PRECRQ_QB_PH_MM")    1, false, false, 35, 1,  2, 1,  0, 0}, // #935
  {DBGFIELD("PRECRQ_RS_PH_W_MM")  1, false, false, 35, 1,  2, 1,  0, 0}, // #936
  {DBGFIELD("RADDU_W_QB_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #937
  {DBGFIELD("RDDSP_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #938
  {DBGFIELD("REPLV_PH_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #939
  {DBGFIELD("REPLV_QB_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #940
  {DBGFIELD("REPL_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #941
  {DBGFIELD("REPL_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #942
  {DBGFIELD("SHILOV_MM")          1, false, false, 35, 1,  2, 1,  0, 0}, // #943
  {DBGFIELD("SHILO_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #944
  {DBGFIELD("SHLLV_PH_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #945
  {DBGFIELD("SHLLV_QB_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #946
  {DBGFIELD("SHLLV_S_PH_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #947
  {DBGFIELD("SHLLV_S_W_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #948
  {DBGFIELD("SHLL_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #949
  {DBGFIELD("SHLL_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #950
  {DBGFIELD("SHLL_S_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #951
  {DBGFIELD("SHLL_S_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #952
  {DBGFIELD("SHRAV_PH_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #953
  {DBGFIELD("SHRAV_R_PH_MM")      1, false, false, 35, 1,  2, 1,  0, 0}, // #954
  {DBGFIELD("SHRAV_R_W_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #955
  {DBGFIELD("SHRA_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #956
  {DBGFIELD("SHRA_R_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #957
  {DBGFIELD("SHRA_R_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #958
  {DBGFIELD("SHRLV_QB_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #959
  {DBGFIELD("SHRL_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #960
  {DBGFIELD("SUBQ_PH_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #961
  {DBGFIELD("SUBQ_S_PH_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #962
  {DBGFIELD("SUBQ_S_W_MM")        1, false, false, 35, 1,  2, 1,  0, 0}, // #963
  {DBGFIELD("SUBU_QB_MM")         1, false, false, 35, 1,  2, 1,  0, 0}, // #964
  {DBGFIELD("SUBU_S_QB_MM")       1, false, false, 35, 1,  2, 1,  0, 0}, // #965
  {DBGFIELD("WRDSP_MM")           1, false, false, 35, 1,  2, 1,  0, 0}, // #966
  {DBGFIELD("ABSQ_S_QB_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #967
  {DBGFIELD("ADDQH_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #968
  {DBGFIELD("ADDQH_R_PH_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #969
  {DBGFIELD("ADDQH_R_W_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #970
  {DBGFIELD("ADDQH_W_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #971
  {DBGFIELD("ADDUH_QB_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #972
  {DBGFIELD("ADDUH_R_QB_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #973
  {DBGFIELD("ADDU_PH_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #974
  {DBGFIELD("ADDU_S_PH_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #975
  {DBGFIELD("APPEND_MMR2")        1, false, false, 35, 1,  2, 1,  0, 0}, // #976
  {DBGFIELD("BALIGN_MMR2")        1, false, false, 35, 1,  2, 1,  0, 0}, // #977
  {DBGFIELD("CMPGDU_EQ_QB_MMR2")  1, false, false, 35, 1,  2, 1,  0, 0}, // #978
  {DBGFIELD("CMPGDU_LE_QB_MMR2")  1, false, false, 35, 1,  2, 1,  0, 0}, // #979
  {DBGFIELD("CMPGDU_LT_QB_MMR2")  1, false, false, 35, 1,  2, 1,  0, 0}, // #980
  {DBGFIELD("DPA_W_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #981
  {DBGFIELD("DPAQX_SA_W_PH_MMR2") 1, false, false, 35, 1,  2, 1,  0, 0}, // #982
  {DBGFIELD("DPAQX_S_W_PH_MMR2")  1, false, false, 35, 1,  2, 1,  0, 0}, // #983
  {DBGFIELD("DPAX_W_PH_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #984
  {DBGFIELD("DPS_W_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #985
  {DBGFIELD("DPSQX_S_W_PH_MMR2")  1, false, false, 35, 1,  2, 1,  0, 0}, // #986
  {DBGFIELD("DPSQX_SA_W_PH_MMR2") 1, false, false, 35, 1,  2, 1,  0, 0}, // #987
  {DBGFIELD("DPSX_W_PH_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #988
  {DBGFIELD("MUL_PH_MMR2")        1, false, false, 35, 1,  2, 1,  0, 0}, // #989
  {DBGFIELD("MUL_S_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #990
  {DBGFIELD("MULQ_RS_W_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #991
  {DBGFIELD("MULQ_S_PH_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #992
  {DBGFIELD("MULQ_S_W_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #993
  {DBGFIELD("MULSA_W_PH_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #994
  {DBGFIELD("PRECR_QB_PH_MMR2")   1, false, false, 35, 1,  2, 1,  0, 0}, // #995
  {DBGFIELD("PRECR_SRA_PH_W_MMR2") 1, false, false, 35, 1,  2, 1,  0, 0}, // #996
  {DBGFIELD("PRECR_SRA_R_PH_W_MMR2") 1, false, false, 35, 1,  2, 1,  0, 0}, // #997
  {DBGFIELD("PREPEND_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #998
  {DBGFIELD("SHRA_QB_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #999
  {DBGFIELD("SHRA_R_QB_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #1000
  {DBGFIELD("SHRAV_QB_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #1001
  {DBGFIELD("SHRAV_R_QB_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #1002
  {DBGFIELD("SHRL_PH_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #1003
  {DBGFIELD("SHRLV_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #1004
  {DBGFIELD("SUBQH_PH_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #1005
  {DBGFIELD("SUBQH_R_PH_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #1006
  {DBGFIELD("SUBQH_W_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #1007
  {DBGFIELD("SUBQH_R_W_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #1008
  {DBGFIELD("SUBU_PH_MMR2")       1, false, false, 35, 1,  2, 1,  0, 0}, // #1009
  {DBGFIELD("SUBU_S_PH_MMR2")     1, false, false, 35, 1,  2, 1,  0, 0}, // #1010
  {DBGFIELD("SUBUH_QB_MMR2")      1, false, false, 35, 1,  2, 1,  0, 0}, // #1011
  {DBGFIELD("SUBUH_R_QB_MMR2")    1, false, false, 35, 1,  2, 1,  0, 0}, // #1012
  {DBGFIELD("BPOSGE32C_MMR3")     1, false, false, 35, 1,  2, 1,  0, 0}, // #1013
  {DBGFIELD("P5600WriteALU")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1014
  {DBGFIELD("P5600WriteAL2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1015
}; // MipsGenericModelSchedClasses

// {Name, NumMicroOps, BeginGroup, EndGroup, WriteProcResIdx,#, WriteLatencyIdx,#, ReadAdvanceIdx,#}
static const llvm::MCSchedClassDesc MipsP5600ModelSchedClasses[] = {
  {DBGFIELD("InvalidSchedClass")  16383, false, false,  0, 0,  0, 0,  0, 0},
  {DBGFIELD("IIPseudo")           0, false, false,  0, 0,  0, 0,  0, 0}, // #1
  {DBGFIELD("II_B")               0, false, false,  0, 0,  0, 0,  0, 0}, // #2
  {DBGFIELD("II_BCCZAL")          0, false, false,  0, 0,  0, 0,  0, 0}, // #3
  {DBGFIELD("II_MTC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #4
  {DBGFIELD("II_MFC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #5
  {DBGFIELD("II_JALR")            0, false, false,  0, 0,  0, 0,  0, 0}, // #6
  {DBGFIELD("II_CVT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #7
  {DBGFIELD("II_DMULT")           0, false, false,  0, 0,  0, 0,  0, 0}, // #8
  {DBGFIELD("II_DMULTU")          0, false, false,  0, 0,  0, 0,  0, 0}, // #9
  {DBGFIELD("II_DDIV")            0, false, false,  0, 0,  0, 0,  0, 0}, // #10
  {DBGFIELD("II_DDIVU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #11
  {DBGFIELD("II_IndirectBranchPseudo") 0, false, false,  0, 0,  0, 0,  0, 0}, // #12
  {DBGFIELD("II_MADD")            0, false, false,  0, 0,  0, 0,  0, 0}, // #13
  {DBGFIELD("II_MADDU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #14
  {DBGFIELD("II_MFHI_MFLO")       0, false, false,  0, 0,  0, 0,  0, 0}, // #15
  {DBGFIELD("II_MSUB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #16
  {DBGFIELD("II_MSUBU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #17
  {DBGFIELD("II_MTHI_MTLO")       0, false, false,  0, 0,  0, 0,  0, 0}, // #18
  {DBGFIELD("II_MULT")            0, false, false,  0, 0,  0, 0,  0, 0}, // #19
  {DBGFIELD("II_MULTU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #20
  {DBGFIELD("II_ReturnPseudo")    0, false, false,  0, 0,  0, 0,  0, 0}, // #21
  {DBGFIELD("II_DIV")             0, false, false,  0, 0,  0, 0,  0, 0}, // #22
  {DBGFIELD("II_DIVU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #23
  {DBGFIELD("II_J")               0, false, false,  0, 0,  0, 0,  0, 0}, // #24
  {DBGFIELD("II_JR")              0, false, false,  0, 0,  0, 0,  0, 0}, // #25
  {DBGFIELD("II_TRAP")            0, false, false,  0, 0,  0, 0,  0, 0}, // #26
  {DBGFIELD("II_ADD")             0, false, false,  0, 0,  0, 0,  0, 0}, // #27
  {DBGFIELD("II_ADDIUPC")         0, false, false,  0, 0,  0, 0,  0, 0}, // #28
  {DBGFIELD("II_ADDIU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #29
  {DBGFIELD("II_ADDU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #30
  {DBGFIELD("II_ADDI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #31
  {DBGFIELD("II_ALIGN")           0, false, false,  0, 0,  0, 0,  0, 0}, // #32
  {DBGFIELD("II_ALUIPC")          0, false, false,  0, 0,  0, 0,  0, 0}, // #33
  {DBGFIELD("II_AND")             0, false, false,  0, 0,  0, 0,  0, 0}, // #34
  {DBGFIELD("II_ANDI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #35
  {DBGFIELD("II_AUI")             0, false, false,  0, 0,  0, 0,  0, 0}, // #36
  {DBGFIELD("II_AUIPC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #37
  {DBGFIELD("IIM16Alu")           0, false, false,  0, 0,  0, 0,  0, 0}, // #38
  {DBGFIELD("II_BADDU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #39
  {DBGFIELD("II_BC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #40
  {DBGFIELD("II_BALC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #41
  {DBGFIELD("II_BBIT")            0, false, false,  0, 0,  0, 0,  0, 0}, // #42
  {DBGFIELD("II_BC1CCZ")          0, false, false,  0, 0,  0, 0,  0, 0}, // #43
  {DBGFIELD("II_BC1F")            0, false, false,  0, 0,  0, 0,  0, 0}, // #44
  {DBGFIELD("II_BC1FL")           0, false, false,  0, 0,  0, 0,  0, 0}, // #45
  {DBGFIELD("II_BC1T")            0, false, false,  0, 0,  0, 0,  0, 0}, // #46
  {DBGFIELD("II_BC1TL")           0, false, false,  0, 0,  0, 0,  0, 0}, // #47
  {DBGFIELD("II_BC2CCZ")          0, false, false,  0, 0,  0, 0,  0, 0}, // #48
  {DBGFIELD("II_BCC")             0, false, false,  0, 0,  0, 0,  0, 0}, // #49
  {DBGFIELD("II_BCCC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #50
  {DBGFIELD("II_BCCZ")            0, false, false,  0, 0,  0, 0,  0, 0}, // #51
  {DBGFIELD("II_BCCZC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #52
  {DBGFIELD("II_BCCZALS")         0, false, false,  0, 0,  0, 0,  0, 0}, // #53
  {DBGFIELD("II_BITSWAP")         0, false, false,  0, 0,  0, 0,  0, 0}, // #54
  {DBGFIELD("II_BREAK")           0, false, false,  0, 0,  0, 0,  0, 0}, // #55
  {DBGFIELD("II_CACHE")           0, false, false,  0, 0,  0, 0,  0, 0}, // #56
  {DBGFIELD("II_CACHEE")          0, false, false,  0, 0,  0, 0,  0, 0}, // #57
  {DBGFIELD("II_CEIL")            0, false, false,  0, 0,  0, 0,  0, 0}, // #58
  {DBGFIELD("II_CFC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #59
  {DBGFIELD("II_CFC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #60
  {DBGFIELD("II_INS")             0, false, false,  0, 0,  0, 0,  0, 0}, // #61
  {DBGFIELD("II_CLASS_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #62
  {DBGFIELD("II_CLASS_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #63
  {DBGFIELD("II_CLO")             0, false, false,  0, 0,  0, 0,  0, 0}, // #64
  {DBGFIELD("II_CLZ")             0, false, false,  0, 0,  0, 0,  0, 0}, // #65
  {DBGFIELD("II_CMP_CC_D")        0, false, false,  0, 0,  0, 0,  0, 0}, // #66
  {DBGFIELD("II_CMP_CC_S")        0, false, false,  0, 0,  0, 0,  0, 0}, // #67
  {DBGFIELD("II_CRC32B")          0, false, false,  0, 0,  0, 0,  0, 0}, // #68
  {DBGFIELD("II_CRC32CB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #69
  {DBGFIELD("II_CRC32CD")         0, false, false,  0, 0,  0, 0,  0, 0}, // #70
  {DBGFIELD("II_CRC32CH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #71
  {DBGFIELD("II_CRC32CW")         0, false, false,  0, 0,  0, 0,  0, 0}, // #72
  {DBGFIELD("II_CRC32D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #73
  {DBGFIELD("II_CRC32H")          0, false, false,  0, 0,  0, 0,  0, 0}, // #74
  {DBGFIELD("II_CRC32W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #75
  {DBGFIELD("II_CTC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #76
  {DBGFIELD("II_CTC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #77
  {DBGFIELD("II_C_CC_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #78
  {DBGFIELD("II_C_CC_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #79
  {DBGFIELD("II_DADD")            0, false, false,  0, 0,  0, 0,  0, 0}, // #80
  {DBGFIELD("II_DADDI")           0, false, false,  0, 0,  0, 0,  0, 0}, // #81
  {DBGFIELD("II_DADDIU")          0, false, false,  0, 0,  0, 0,  0, 0}, // #82
  {DBGFIELD("II_DADDU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #83
  {DBGFIELD("II_DAHI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #84
  {DBGFIELD("II_DALIGN")          0, false, false,  0, 0,  0, 0,  0, 0}, // #85
  {DBGFIELD("II_DATI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #86
  {DBGFIELD("II_DAUI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #87
  {DBGFIELD("II_DBITSWAP")        0, false, false,  0, 0,  0, 0,  0, 0}, // #88
  {DBGFIELD("II_DCLO")            0, false, false,  0, 0,  0, 0,  0, 0}, // #89
  {DBGFIELD("II_DCLZ")            0, false, false,  0, 0,  0, 0,  0, 0}, // #90
  {DBGFIELD("II_DERET")           0, false, false,  0, 0,  0, 0,  0, 0}, // #91
  {DBGFIELD("II_EXT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #92
  {DBGFIELD("II_DI")              0, false, false,  0, 0,  0, 0,  0, 0}, // #93
  {DBGFIELD("II_DLSA")            0, false, false,  0, 0,  0, 0,  0, 0}, // #94
  {DBGFIELD("II_DMFC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #95
  {DBGFIELD("II_DMFC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #96
  {DBGFIELD("II_DMFC2")           0, false, false,  0, 0,  0, 0,  0, 0}, // #97
  {DBGFIELD("II_DMFGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #98
  {DBGFIELD("II_DMOD")            0, false, false,  0, 0,  0, 0,  0, 0}, // #99
  {DBGFIELD("II_DMODU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #100
  {DBGFIELD("II_DMT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #101
  {DBGFIELD("II_DMTC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #102
  {DBGFIELD("II_DMTC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #103
  {DBGFIELD("II_DMTC2")           0, false, false,  0, 0,  0, 0,  0, 0}, // #104
  {DBGFIELD("II_DMTGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #105
  {DBGFIELD("II_DMUH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #106
  {DBGFIELD("II_DMUHU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #107
  {DBGFIELD("II_DMUL")            0, false, false,  0, 0,  0, 0,  0, 0}, // #108
  {DBGFIELD("II_POP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #109
  {DBGFIELD("II_DROTR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #110
  {DBGFIELD("II_DROTR32")         0, false, false,  0, 0,  0, 0,  0, 0}, // #111
  {DBGFIELD("II_DROTRV")          0, false, false,  0, 0,  0, 0,  0, 0}, // #112
  {DBGFIELD("II_DSBH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #113
  {DBGFIELD("II_DSHD")            0, false, false,  0, 0,  0, 0,  0, 0}, // #114
  {DBGFIELD("II_DSLL")            0, false, false,  0, 0,  0, 0,  0, 0}, // #115
  {DBGFIELD("II_DSLL32")          0, false, false,  0, 0,  0, 0,  0, 0}, // #116
  {DBGFIELD("II_DSLLV")           0, false, false,  0, 0,  0, 0,  0, 0}, // #117
  {DBGFIELD("II_DSRA")            0, false, false,  0, 0,  0, 0,  0, 0}, // #118
  {DBGFIELD("II_DSRA32")          0, false, false,  0, 0,  0, 0,  0, 0}, // #119
  {DBGFIELD("II_DSRAV")           0, false, false,  0, 0,  0, 0,  0, 0}, // #120
  {DBGFIELD("II_DSRL")            0, false, false,  0, 0,  0, 0,  0, 0}, // #121
  {DBGFIELD("II_DSRL32")          0, false, false,  0, 0,  0, 0,  0, 0}, // #122
  {DBGFIELD("II_DSRLV")           0, false, false,  0, 0,  0, 0,  0, 0}, // #123
  {DBGFIELD("II_DSUB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #124
  {DBGFIELD("II_DSUBU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #125
  {DBGFIELD("II_DVP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #126
  {DBGFIELD("II_DVPE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #127
  {DBGFIELD("II_EHB")             0, false, false,  0, 0,  0, 0,  0, 0}, // #128
  {DBGFIELD("II_EI")              0, false, false,  0, 0,  0, 0,  0, 0}, // #129
  {DBGFIELD("II_EMT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #130
  {DBGFIELD("II_ERET")            0, false, false,  0, 0,  0, 0,  0, 0}, // #131
  {DBGFIELD("II_ERETNC")          0, false, false,  0, 0,  0, 0,  0, 0}, // #132
  {DBGFIELD("II_EVP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #133
  {DBGFIELD("II_EVPE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #134
  {DBGFIELD("II_ABS")             0, false, false,  0, 0,  0, 0,  0, 0}, // #135
  {DBGFIELD("II_SQRT_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #136
  {DBGFIELD("II_ADD_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #137
  {DBGFIELD("II_ADD_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #138
  {DBGFIELD("II_DIV_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #139
  {DBGFIELD("II_DIV_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #140
  {DBGFIELD("II_FLOOR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #141
  {DBGFIELD("II_MOV_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #142
  {DBGFIELD("II_MOV_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #143
  {DBGFIELD("II_MUL_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #144
  {DBGFIELD("II_MUL_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #145
  {DBGFIELD("II_NEG")             0, false, false,  0, 0,  0, 0,  0, 0}, // #146
  {DBGFIELD("II_FORK")            0, false, false,  0, 0,  0, 0,  0, 0}, // #147
  {DBGFIELD("II_SQRT_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #148
  {DBGFIELD("II_SUB_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #149
  {DBGFIELD("II_SUB_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #150
  {DBGFIELD("II_GINVI")           0, false, false,  0, 0,  0, 0,  0, 0}, // #151
  {DBGFIELD("II_GINVT")           0, false, false,  0, 0,  0, 0,  0, 0}, // #152
  {DBGFIELD("II_HYPCALL")         0, false, false,  0, 0,  0, 0,  0, 0}, // #153
  {DBGFIELD("II_JAL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #154
  {DBGFIELD("II_JALR_HB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #155
  {DBGFIELD("II_JALRC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #156
  {DBGFIELD("II_JALRS")           0, false, false,  0, 0,  0, 0,  0, 0}, // #157
  {DBGFIELD("II_JALS")            0, false, false,  0, 0,  0, 0,  0, 0}, // #158
  {DBGFIELD("II_JIALC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #159
  {DBGFIELD("II_JIC")             0, false, false,  0, 0,  0, 0,  0, 0}, // #160
  {DBGFIELD("II_JRADDIUSP")       0, false, false,  0, 0,  0, 0,  0, 0}, // #161
  {DBGFIELD("II_JRC")             0, false, false,  0, 0,  0, 0,  0, 0}, // #162
  {DBGFIELD("II_JR_HB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #163
  {DBGFIELD("II_LB")              0, false, false,  0, 0,  0, 0,  0, 0}, // #164
  {DBGFIELD("II_LBE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #165
  {DBGFIELD("II_LBU")             0, false, false,  0, 0,  0, 0,  0, 0}, // #166
  {DBGFIELD("II_LBUE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #167
  {DBGFIELD("II_LD")              0, false, false,  0, 0,  0, 0,  0, 0}, // #168
  {DBGFIELD("II_LDC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #169
  {DBGFIELD("II_LDC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #170
  {DBGFIELD("II_LDC3")            0, false, false,  0, 0,  0, 0,  0, 0}, // #171
  {DBGFIELD("II_LDL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #172
  {DBGFIELD("II_LDPC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #173
  {DBGFIELD("II_LDR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #174
  {DBGFIELD("II_LDXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #175
  {DBGFIELD("II_LH")              0, false, false,  0, 0,  0, 0,  0, 0}, // #176
  {DBGFIELD("II_LHE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #177
  {DBGFIELD("II_LHU")             0, false, false,  0, 0,  0, 0,  0, 0}, // #178
  {DBGFIELD("II_LHUE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #179
  {DBGFIELD("II_LI")              0, false, false,  0, 0,  0, 0,  0, 0}, // #180
  {DBGFIELD("II_LL")              0, false, false,  0, 0,  0, 0,  0, 0}, // #181
  {DBGFIELD("II_LLD")             0, false, false,  0, 0,  0, 0,  0, 0}, // #182
  {DBGFIELD("II_LLE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #183
  {DBGFIELD("II_LSA")             0, false, false,  0, 0,  0, 0,  0, 0}, // #184
  {DBGFIELD("II_LUI")             0, false, false,  0, 0,  0, 0,  0, 0}, // #185
  {DBGFIELD("II_LUXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #186
  {DBGFIELD("II_LW")              0, false, false,  0, 0,  0, 0,  0, 0}, // #187
  {DBGFIELD("II_LWC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #188
  {DBGFIELD("II_LWC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #189
  {DBGFIELD("II_LWC3")            0, false, false,  0, 0,  0, 0,  0, 0}, // #190
  {DBGFIELD("II_LWE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #191
  {DBGFIELD("II_LWL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #192
  {DBGFIELD("II_LWLE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #193
  {DBGFIELD("II_LWM")             0, false, false,  0, 0,  0, 0,  0, 0}, // #194
  {DBGFIELD("II_LWPC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #195
  {DBGFIELD("II_LWP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #196
  {DBGFIELD("II_LWR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #197
  {DBGFIELD("II_LWRE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #198
  {DBGFIELD("II_LWUPC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #199
  {DBGFIELD("II_LWU")             0, false, false,  0, 0,  0, 0,  0, 0}, // #200
  {DBGFIELD("II_LWXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #201
  {DBGFIELD("II_LWXS")            0, false, false,  0, 0,  0, 0,  0, 0}, // #202
  {DBGFIELD("II_MADDF_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #203
  {DBGFIELD("II_MADDF_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #204
  {DBGFIELD("II_MADD_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #205
  {DBGFIELD("II_MADD_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #206
  {DBGFIELD("II_MAX_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #207
  {DBGFIELD("II_MAXA_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #208
  {DBGFIELD("II_MAX_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #209
  {DBGFIELD("II_MAXA_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #210
  {DBGFIELD("II_MFC0")            0, false, false,  0, 0,  0, 0,  0, 0}, // #211
  {DBGFIELD("II_MFC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #212
  {DBGFIELD("II_MFGC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #213
  {DBGFIELD("II_MFHC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #214
  {DBGFIELD("II_MFHC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #215
  {DBGFIELD("II_MFHGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #216
  {DBGFIELD("II_MFTR")            0, false, false,  0, 0,  0, 0,  0, 0}, // #217
  {DBGFIELD("II_MIN_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #218
  {DBGFIELD("II_MINA_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #219
  {DBGFIELD("II_MIN_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #220
  {DBGFIELD("II_MINA_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #221
  {DBGFIELD("II_MOD")             0, false, false,  0, 0,  0, 0,  0, 0}, // #222
  {DBGFIELD("II_MODU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #223
  {DBGFIELD("II_MOVE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #224
  {DBGFIELD("II_MOVF_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #225
  {DBGFIELD("II_MOVF")            0, false, false,  0, 0,  0, 0,  0, 0}, // #226
  {DBGFIELD("II_MOVF_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #227
  {DBGFIELD("II_MOVN_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #228
  {DBGFIELD("II_MOVN")            0, false, false,  0, 0,  0, 0,  0, 0}, // #229
  {DBGFIELD("II_MOVN_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #230
  {DBGFIELD("II_MOVT_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #231
  {DBGFIELD("II_MOVT")            0, false, false,  0, 0,  0, 0,  0, 0}, // #232
  {DBGFIELD("II_MOVT_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #233
  {DBGFIELD("II_MOVZ_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #234
  {DBGFIELD("II_MOVZ")            0, false, false,  0, 0,  0, 0,  0, 0}, // #235
  {DBGFIELD("II_MOVZ_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #236
  {DBGFIELD("II_MSUBF_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #237
  {DBGFIELD("II_MSUBF_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #238
  {DBGFIELD("II_MSUB_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #239
  {DBGFIELD("II_MSUB_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #240
  {DBGFIELD("II_MTC0")            0, false, false,  0, 0,  0, 0,  0, 0}, // #241
  {DBGFIELD("II_MTC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #242
  {DBGFIELD("II_MTGC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #243
  {DBGFIELD("II_MTHC0")           0, false, false,  0, 0,  0, 0,  0, 0}, // #244
  {DBGFIELD("II_MTHC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #245
  {DBGFIELD("II_MTHGC0")          0, false, false,  0, 0,  0, 0,  0, 0}, // #246
  {DBGFIELD("II_MTTR")            0, false, false,  0, 0,  0, 0,  0, 0}, // #247
  {DBGFIELD("II_MUH")             0, false, false,  0, 0,  0, 0,  0, 0}, // #248
  {DBGFIELD("II_MUHU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #249
  {DBGFIELD("II_MUL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #250
  {DBGFIELD("II_MULU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #251
  {DBGFIELD("II_NMADD_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #252
  {DBGFIELD("II_NMADD_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #253
  {DBGFIELD("II_NMSUB_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #254
  {DBGFIELD("II_NMSUB_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #255
  {DBGFIELD("II_NOR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #256
  {DBGFIELD("II_NOT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #257
  {DBGFIELD("II_OR")              0, false, false,  0, 0,  0, 0,  0, 0}, // #258
  {DBGFIELD("II_ORI")             0, false, false,  0, 0,  0, 0,  0, 0}, // #259
  {DBGFIELD("II_PAUSE")           0, false, false,  0, 0,  0, 0,  0, 0}, // #260
  {DBGFIELD("II_PREF")            0, false, false,  0, 0,  0, 0,  0, 0}, // #261
  {DBGFIELD("II_PREFE")           0, false, false,  0, 0,  0, 0,  0, 0}, // #262
  {DBGFIELD("II_RDHWR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #263
  {DBGFIELD("II_RDPGPR")          0, false, false,  0, 0,  0, 0,  0, 0}, // #264
  {DBGFIELD("II_RECIP_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #265
  {DBGFIELD("II_RECIP_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #266
  {DBGFIELD("II_RINT_D")          0, false, false,  0, 0,  0, 0,  0, 0}, // #267
  {DBGFIELD("II_RINT_S")          0, false, false,  0, 0,  0, 0,  0, 0}, // #268
  {DBGFIELD("II_ROTR")            0, false, false,  0, 0,  0, 0,  0, 0}, // #269
  {DBGFIELD("II_ROTRV")           0, false, false,  0, 0,  0, 0,  0, 0}, // #270
  {DBGFIELD("II_ROUND")           0, false, false,  0, 0,  0, 0,  0, 0}, // #271
  {DBGFIELD("II_RSQRT_D")         0, false, false,  0, 0,  0, 0,  0, 0}, // #272
  {DBGFIELD("II_RSQRT_S")         0, false, false,  0, 0,  0, 0,  0, 0}, // #273
  {DBGFIELD("II_RESTORE")         0, false, false,  0, 0,  0, 0,  0, 0}, // #274
  {DBGFIELD("II_SB")              0, false, false,  0, 0,  0, 0,  0, 0}, // #275
  {DBGFIELD("II_SBE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #276
  {DBGFIELD("II_SC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #277
  {DBGFIELD("II_SCD")             0, false, false,  0, 0,  0, 0,  0, 0}, // #278
  {DBGFIELD("II_SCE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #279
  {DBGFIELD("II_SD")              0, false, false,  0, 0,  0, 0,  0, 0}, // #280
  {DBGFIELD("II_SDBBP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #281
  {DBGFIELD("II_SDC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #282
  {DBGFIELD("II_SDC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #283
  {DBGFIELD("II_SDC3")            0, false, false,  0, 0,  0, 0,  0, 0}, // #284
  {DBGFIELD("II_SDL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #285
  {DBGFIELD("II_SDR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #286
  {DBGFIELD("II_SDXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #287
  {DBGFIELD("II_SEB")             0, false, false,  0, 0,  0, 0,  0, 0}, // #288
  {DBGFIELD("II_SEH")             0, false, false,  0, 0,  0, 0,  0, 0}, // #289
  {DBGFIELD("II_SELCCZ")          0, false, false,  0, 0,  0, 0,  0, 0}, // #290
  {DBGFIELD("II_SELCCZ_D")        0, false, false,  0, 0,  0, 0,  0, 0}, // #291
  {DBGFIELD("II_SELCCZ_S")        0, false, false,  0, 0,  0, 0,  0, 0}, // #292
  {DBGFIELD("II_SEL_D")           0, false, false,  0, 0,  0, 0,  0, 0}, // #293
  {DBGFIELD("II_SEL_S")           0, false, false,  0, 0,  0, 0,  0, 0}, // #294
  {DBGFIELD("II_SEQ_SNE")         0, false, false,  0, 0,  0, 0,  0, 0}, // #295
  {DBGFIELD("II_SEQI_SNEI")       0, false, false,  0, 0,  0, 0,  0, 0}, // #296
  {DBGFIELD("II_SH")              0, false, false,  0, 0,  0, 0,  0, 0}, // #297
  {DBGFIELD("II_SHE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #298
  {DBGFIELD("II_SLL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #299
  {DBGFIELD("II_SLLV")            0, false, false,  0, 0,  0, 0,  0, 0}, // #300
  {DBGFIELD("II_SLT_SLTU")        0, false, false,  0, 0,  0, 0,  0, 0}, // #301
  {DBGFIELD("II_SLTI_SLTIU")      0, false, false,  0, 0,  0, 0,  0, 0}, // #302
  {DBGFIELD("II_SRA")             0, false, false,  0, 0,  0, 0,  0, 0}, // #303
  {DBGFIELD("II_SRAV")            0, false, false,  0, 0,  0, 0,  0, 0}, // #304
  {DBGFIELD("II_SRL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #305
  {DBGFIELD("II_SRLV")            0, false, false,  0, 0,  0, 0,  0, 0}, // #306
  {DBGFIELD("II_SSNOP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #307
  {DBGFIELD("II_SUB")             0, false, false,  0, 0,  0, 0,  0, 0}, // #308
  {DBGFIELD("II_SUBU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #309
  {DBGFIELD("II_SUXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #310
  {DBGFIELD("II_SW")              0, false, false,  0, 0,  0, 0,  0, 0}, // #311
  {DBGFIELD("II_SWC1")            0, false, false,  0, 0,  0, 0,  0, 0}, // #312
  {DBGFIELD("II_SWC2")            0, false, false,  0, 0,  0, 0,  0, 0}, // #313
  {DBGFIELD("II_SWC3")            0, false, false,  0, 0,  0, 0,  0, 0}, // #314
  {DBGFIELD("II_SWE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #315
  {DBGFIELD("II_SWL")             0, false, false,  0, 0,  0, 0,  0, 0}, // #316
  {DBGFIELD("II_SWLE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #317
  {DBGFIELD("II_SWM")             0, false, false,  0, 0,  0, 0,  0, 0}, // #318
  {DBGFIELD("II_SWP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #319
  {DBGFIELD("II_SWR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #320
  {DBGFIELD("II_SWRE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #321
  {DBGFIELD("II_SWXC1")           0, false, false,  0, 0,  0, 0,  0, 0}, // #322
  {DBGFIELD("II_SYNC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #323
  {DBGFIELD("II_SYNCI")           0, false, false,  0, 0,  0, 0,  0, 0}, // #324
  {DBGFIELD("II_SYSCALL")         0, false, false,  0, 0,  0, 0,  0, 0}, // #325
  {DBGFIELD("II_SAVE")            0, false, false,  0, 0,  0, 0,  0, 0}, // #326
  {DBGFIELD("II_TEQ")             0, false, false,  0, 0,  0, 0,  0, 0}, // #327
  {DBGFIELD("II_TEQI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #328
  {DBGFIELD("II_TGE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #329
  {DBGFIELD("II_TGEI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #330
  {DBGFIELD("II_TGEIU")           0, false, false,  0, 0,  0, 0,  0, 0}, // #331
  {DBGFIELD("II_TGEU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #332
  {DBGFIELD("II_TLBGINV")         0, false, false,  0, 0,  0, 0,  0, 0}, // #333
  {DBGFIELD("II_TLBGINVF")        0, false, false,  0, 0,  0, 0,  0, 0}, // #334
  {DBGFIELD("II_TLBGP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #335
  {DBGFIELD("II_TLBGR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #336
  {DBGFIELD("II_TLBGWI")          0, false, false,  0, 0,  0, 0,  0, 0}, // #337
  {DBGFIELD("II_TLBGWR")          0, false, false,  0, 0,  0, 0,  0, 0}, // #338
  {DBGFIELD("II_TLBINV")          0, false, false,  0, 0,  0, 0,  0, 0}, // #339
  {DBGFIELD("II_TLBINVF")         0, false, false,  0, 0,  0, 0,  0, 0}, // #340
  {DBGFIELD("II_TLBP")            0, false, false,  0, 0,  0, 0,  0, 0}, // #341
  {DBGFIELD("II_TLBR")            0, false, false,  0, 0,  0, 0,  0, 0}, // #342
  {DBGFIELD("II_TLBWI")           0, false, false,  0, 0,  0, 0,  0, 0}, // #343
  {DBGFIELD("II_TLBWR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #344
  {DBGFIELD("II_TLT")             0, false, false,  0, 0,  0, 0,  0, 0}, // #345
  {DBGFIELD("II_TLTI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #346
  {DBGFIELD("II_TTLTIU")          0, false, false,  0, 0,  0, 0,  0, 0}, // #347
  {DBGFIELD("II_TLTU")            0, false, false,  0, 0,  0, 0,  0, 0}, // #348
  {DBGFIELD("II_TNE")             0, false, false,  0, 0,  0, 0,  0, 0}, // #349
  {DBGFIELD("II_TNEI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #350
  {DBGFIELD("II_TRUNC")           0, false, false,  0, 0,  0, 0,  0, 0}, // #351
  {DBGFIELD("II_WAIT")            0, false, false,  0, 0,  0, 0,  0, 0}, // #352
  {DBGFIELD("II_WRPGPR")          0, false, false,  0, 0,  0, 0,  0, 0}, // #353
  {DBGFIELD("II_WSBH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #354
  {DBGFIELD("II_XOR")             0, false, false,  0, 0,  0, 0,  0, 0}, // #355
  {DBGFIELD("II_XORI")            0, false, false,  0, 0,  0, 0,  0, 0}, // #356
  {DBGFIELD("II_YIELD")           0, false, false,  0, 0,  0, 0,  0, 0}, // #357
  {DBGFIELD("AND")                1, false, false, 36, 2,  1, 1,  0, 0}, // #358
  {DBGFIELD("LUi")                1, false, false, 36, 2,  1, 1,  0, 0}, // #359
  {DBGFIELD("NOR")                1, false, false, 36, 2,  1, 1,  0, 0}, // #360
  {DBGFIELD("OR")                 1, false, false, 36, 2,  1, 1,  0, 0}, // #361
  {DBGFIELD("SLTi_SLTiu")         1, false, false, 36, 2,  1, 1,  0, 0}, // #362
  {DBGFIELD("SUB")                1, false, false, 36, 2,  1, 1,  0, 0}, // #363
  {DBGFIELD("SUBu")               1, false, false, 36, 2,  1, 1,  0, 0}, // #364
  {DBGFIELD("XOR")                1, false, false, 36, 2,  1, 1,  0, 0}, // #365
  {DBGFIELD("B")                  1, false, false, 38, 3,  1, 1,  0, 0}, // #366
  {DBGFIELD("BAL")                1, false, false, 38, 3,  1, 1,  0, 0}, // #367
  {DBGFIELD("BAL_BR_BGEZAL_BGEZALL_BLTZAL_BLTZALL") 1, false, false, 38, 3,  1, 1,  0, 0}, // #368
  {DBGFIELD("BEQ_BEQL_BNE_BNEL")  1, false, false, 38, 3,  1, 1,  0, 0}, // #369
  {DBGFIELD("BGEZ_BGEZL_BGTZ_BGTZL_BLEZ_BLEZL_BLTZ_BLTZL") 1, false, false, 38, 3,  1, 1,  0, 0}, // #370
  {DBGFIELD("BREAK")              1, false, false, 38, 3,  1, 1,  0, 0}, // #371
  {DBGFIELD("DERET")              1, false, false, 38, 3,  1, 1,  0, 0}, // #372
  {DBGFIELD("ERET")               1, false, false, 38, 3,  1, 1,  0, 0}, // #373
  {DBGFIELD("ERETNC")             1, false, false, 38, 3,  1, 1,  0, 0}, // #374
  {DBGFIELD("J_TAILCALL")         1, false, false, 38, 3,  1, 1,  0, 0}, // #375
  {DBGFIELD("JR_TAILCALLREG_TAILCALLREGHB") 1, false, false, 38, 3,  1, 1,  0, 0}, // #376
  {DBGFIELD("JR_HB")              1, false, false, 38, 3,  1, 1,  0, 0}, // #377
  {DBGFIELD("PseudoIndirectBranch_PseudoIndirectHazardBranch") 1, false, false, 38, 3,  1, 1,  0, 0}, // #378
  {DBGFIELD("PseudoReturn")       1, false, false, 38, 3,  1, 1,  0, 0}, // #379
  {DBGFIELD("SDBBP")              1, false, false, 38, 3,  1, 1,  0, 0}, // #380
  {DBGFIELD("SSNOP")              1, false, false, 38, 3,  1, 1,  0, 0}, // #381
  {DBGFIELD("SYSCALL")            1, false, false, 38, 3,  1, 1,  0, 0}, // #382
  {DBGFIELD("TEQ")                1, false, false, 38, 3,  1, 1,  0, 0}, // #383
  {DBGFIELD("TEQI")               1, false, false, 38, 3,  1, 1,  0, 0}, // #384
  {DBGFIELD("TGE")                1, false, false, 38, 3,  1, 1,  0, 0}, // #385
  {DBGFIELD("TGEI")               1, false, false, 38, 3,  1, 1,  0, 0}, // #386
  {DBGFIELD("TGEIU")              1, false, false, 38, 3,  1, 1,  0, 0}, // #387
  {DBGFIELD("TGEU")               1, false, false, 38, 3,  1, 1,  0, 0}, // #388
  {DBGFIELD("TLT")                1, false, false, 38, 3,  1, 1,  0, 0}, // #389
  {DBGFIELD("TLTI")               1, false, false, 38, 3,  1, 1,  0, 0}, // #390
  {DBGFIELD("TLTU")               1, false, false, 38, 3,  1, 1,  0, 0}, // #391
  {DBGFIELD("TNE")                1, false, false, 38, 3,  1, 1,  0, 0}, // #392
  {DBGFIELD("TNEI")               1, false, false, 38, 3,  1, 1,  0, 0}, // #393
  {DBGFIELD("TRAP")               1, false, false, 38, 3,  1, 1,  0, 0}, // #394
  {DBGFIELD("TTLTIU")             1, false, false, 38, 3,  1, 1,  0, 0}, // #395
  {DBGFIELD("WAIT")               1, false, false, 38, 3,  1, 1,  0, 0}, // #396
  {DBGFIELD("PAUSE")              1, false, false, 38, 3,  1, 1,  0, 0}, // #397
  {DBGFIELD("JAL")                1, false, false, 38, 3,  2, 1,  0, 0}, // #398
  {DBGFIELD("JALR_JALRHBPseudo_JALRPseudo") 1, false, false, 38, 3,  2, 1,  0, 0}, // #399
  {DBGFIELD("JALR_HB")            1, false, false, 38, 3,  2, 1,  0, 0}, // #400
  {DBGFIELD("JALX")               1, false, false, 38, 3,  2, 1,  0, 0}, // #401
  {DBGFIELD("TLBINV")             1, false, false, 38, 3,  1, 1,  0, 0}, // #402
  {DBGFIELD("TLBINVF")            1, false, false, 38, 3,  1, 1,  0, 0}, // #403
  {DBGFIELD("TLBP")               1, false, false, 38, 3,  1, 1,  0, 0}, // #404
  {DBGFIELD("TLBR")               1, false, false, 38, 3,  1, 1,  0, 0}, // #405
  {DBGFIELD("TLBWI")              1, false, false, 38, 3,  1, 1,  0, 0}, // #406
  {DBGFIELD("TLBWR")              1, false, false, 38, 3,  1, 1,  0, 0}, // #407
  {DBGFIELD("MFC0")               1, false, false, 38, 3,  1, 1,  0, 0}, // #408
  {DBGFIELD("MTC0")               1, false, false, 38, 3,  1, 1,  0, 0}, // #409
  {DBGFIELD("MFC2")               1, false, false, 38, 3,  1, 1,  0, 0}, // #410
  {DBGFIELD("MTC2")               1, false, false, 38, 3,  1, 1,  0, 0}, // #411
  {DBGFIELD("LB")                 1, false, false, 41, 2,  6, 1,  0, 0}, // #412
  {DBGFIELD("LBu")                1, false, false, 41, 2,  6, 1,  0, 0}, // #413
  {DBGFIELD("LH")                 1, false, false, 41, 2,  6, 1,  0, 0}, // #414
  {DBGFIELD("LHu")                1, false, false, 41, 2,  6, 1,  0, 0}, // #415
  {DBGFIELD("LW")                 1, false, false, 41, 2,  6, 1,  0, 0}, // #416
  {DBGFIELD("LL")                 1, false, false, 41, 2,  6, 1,  0, 0}, // #417
  {DBGFIELD("LWC2")               1, false, false, 41, 2,  6, 1,  0, 0}, // #418
  {DBGFIELD("LWC3")               1, false, false, 41, 2,  6, 1,  0, 0}, // #419
  {DBGFIELD("LDC2")               1, false, false, 41, 2,  6, 1,  0, 0}, // #420
  {DBGFIELD("LDC3")               1, false, false, 41, 2,  6, 1,  0, 0}, // #421
  {DBGFIELD("LBE")                1, false, false, 41, 2,  6, 1,  0, 0}, // #422
  {DBGFIELD("LBuE")               1, false, false, 41, 2,  6, 1,  0, 0}, // #423
  {DBGFIELD("LHE")                1, false, false, 41, 2,  6, 1,  0, 0}, // #424
  {DBGFIELD("LHuE")               1, false, false, 41, 2,  6, 1,  0, 0}, // #425
  {DBGFIELD("LWE")                1, false, false, 41, 2,  6, 1,  0, 0}, // #426
  {DBGFIELD("LLE")                1, false, false, 41, 2,  6, 1,  0, 0}, // #427
  {DBGFIELD("LWPC")               1, false, false, 41, 2,  6, 1,  0, 0}, // #428
  {DBGFIELD("LWL")                1, false, false, 43, 3,  6, 1,  0, 0}, // #429
  {DBGFIELD("LWR")                1, false, false, 43, 3,  6, 1,  0, 0}, // #430
  {DBGFIELD("LWLE")               1, false, false, 43, 3,  6, 1,  0, 0}, // #431
  {DBGFIELD("LWRE")               1, false, false, 43, 3,  6, 1,  0, 0}, // #432
  {DBGFIELD("SB")                 1, false, false, 46, 3,  1, 1,  0, 0}, // #433
  {DBGFIELD("SH")                 1, false, false, 46, 3,  1, 1,  0, 0}, // #434
  {DBGFIELD("SW")                 1, false, false, 46, 3,  1, 1,  0, 0}, // #435
  {DBGFIELD("SWC2")               1, false, false, 46, 3,  1, 1,  0, 0}, // #436
  {DBGFIELD("SWC3")               1, false, false, 46, 3,  1, 1,  0, 0}, // #437
  {DBGFIELD("SDC2")               1, false, false, 46, 3,  1, 1,  0, 0}, // #438
  {DBGFIELD("SDC3")               1, false, false, 46, 3,  1, 1,  0, 0}, // #439
  {DBGFIELD("SC")                 1, false, false, 46, 3,  1, 1,  0, 0}, // #440
  {DBGFIELD("SBE")                1, false, false, 46, 3,  1, 1,  0, 0}, // #441
  {DBGFIELD("SHE")                1, false, false, 46, 3,  1, 1,  0, 0}, // #442
  {DBGFIELD("SWE")                1, false, false, 46, 3,  1, 1,  0, 0}, // #443
  {DBGFIELD("SCE")                1, false, false, 46, 3,  1, 1,  0, 0}, // #444
  {DBGFIELD("SWL")                1, false, false, 46, 3,  1, 1,  0, 0}, // #445
  {DBGFIELD("SWR")                1, false, false, 46, 3,  1, 1,  0, 0}, // #446
  {DBGFIELD("SWLE")               1, false, false, 46, 3,  1, 1,  0, 0}, // #447
  {DBGFIELD("SWRE")               1, false, false, 46, 3,  1, 1,  0, 0}, // #448
  {DBGFIELD("PREF")               1, false, false, 41, 2,  1, 1,  0, 0}, // #449
  {DBGFIELD("PREFE")              1, false, false, 41, 2,  1, 1,  0, 0}, // #450
  {DBGFIELD("CACHE")              1, false, false, 41, 2,  1, 1,  0, 0}, // #451
  {DBGFIELD("CACHEE")             1, false, false, 41, 2,  1, 1,  0, 0}, // #452
  {DBGFIELD("SYNC")               1, false, false, 41, 2,  1, 1,  0, 0}, // #453
  {DBGFIELD("SYNCI")              1, false, false, 41, 2,  1, 1,  0, 0}, // #454
  {DBGFIELD("CLO")                1, false, false,  1, 2,  1, 1,  0, 0}, // #455
  {DBGFIELD("CLZ")                1, false, false,  1, 2,  1, 1,  0, 0}, // #456
  {DBGFIELD("DI")                 1, false, false,  1, 2,  1, 1,  0, 0}, // #457
  {DBGFIELD("EI")                 1, false, false,  1, 2,  1, 1,  0, 0}, // #458
  {DBGFIELD("MFHI_MFLO_PseudoMFHI_PseudoMFLO") 1, false, false,  1, 2,  1, 1,  0, 0}, // #459
  {DBGFIELD("EHB")                1, false, false,  1, 2,  2, 1,  0, 0}, // #460
  {DBGFIELD("RDHWR")              1, false, false,  1, 2,  2, 1,  0, 0}, // #461
  {DBGFIELD("WSBH")               1, false, false,  1, 2,  2, 1,  0, 0}, // #462
  {DBGFIELD("MOVN_I_I")           1, false, false, 49, 3,  2, 1,  0, 0}, // #463
  {DBGFIELD("MOVZ_I_I")           1, false, false, 49, 3,  2, 1,  0, 0}, // #464
  {DBGFIELD("DIV_PseudoSDIV_SDIV") 1, false, false, 52, 3, 14, 1,  0, 0}, // #465
  {DBGFIELD("DIVU_PseudoUDIV_UDIV") 1, false, false, 52, 3, 14, 1,  0, 0}, // #466
  {DBGFIELD("MUL")                1, false, false,  1, 2,  7, 1,  0, 0}, // #467
  {DBGFIELD("MULT_PseudoMULT")    1, false, false,  1, 2,  3, 1,  0, 0}, // #468
  {DBGFIELD("MULTu_PseudoMULTu")  1, false, false,  1, 2,  3, 1,  0, 0}, // #469
  {DBGFIELD("MADD_PseudoMADD")    1, false, false, 49, 3,  3, 1,  0, 0}, // #470
  {DBGFIELD("MADDU_PseudoMADDU")  1, false, false, 49, 3,  3, 1,  0, 0}, // #471
  {DBGFIELD("MSUB_PseudoMSUB")    1, false, false, 49, 3,  3, 1,  0, 0}, // #472
  {DBGFIELD("MSUBU_PseudoMSUBU")  1, false, false, 49, 3,  3, 1,  0, 0}, // #473
  {DBGFIELD("MTHI_MTLO_PseudoMTLOHI") 1, false, false, 49, 3,  3, 1,  0, 0}, // #474
  {DBGFIELD("EXT")                1, false, false,  1, 2,  2, 1,  0, 0}, // #475
  {DBGFIELD("INS")                1, false, false,  1, 2,  2, 1,  0, 0}, // #476
  {DBGFIELD("ADD")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #477
  {DBGFIELD("ADDi")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #478
  {DBGFIELD("ADDiu")              16382, false, false,  0, 0,  0, 0,  0, 0}, // #479
  {DBGFIELD("ANDi")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #480
  {DBGFIELD("ORi")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #481
  {DBGFIELD("ROTR")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #482
  {DBGFIELD("SEB")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #483
  {DBGFIELD("SEH")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #484
  {DBGFIELD("SLT_SLTu")           16382, false, false,  0, 0,  0, 0,  0, 0}, // #485
  {DBGFIELD("SLL")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #486
  {DBGFIELD("SRA")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #487
  {DBGFIELD("SRL")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #488
  {DBGFIELD("XORi")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #489
  {DBGFIELD("ADDu")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #490
  {DBGFIELD("SLLV")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #491
  {DBGFIELD("SRAV")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #492
  {DBGFIELD("SRLV")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #493
  {DBGFIELD("LSA")                16382, false, false,  0, 0,  0, 0,  0, 0}, // #494
  {DBGFIELD("COPY")               16382, false, false,  0, 0,  0, 0,  0, 0}, // #495
  {DBGFIELD("VSHF_B_VSHF_D_VSHF_H_VSHF_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #496
  {DBGFIELD("BINSLI_B_BINSLI_D_BINSLI_H_BINSLI_W_BINSL_B_BINSL_D_BINSL_H_BINSL_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #497
  {DBGFIELD("BINSRI_B_BINSRI_D_BINSRI_H_BINSRI_W_BINSR_B_BINSR_D_BINSR_H_BINSR_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #498
  {DBGFIELD("INSERT_B_INSERT_D_INSERT_H_INSERT_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #499
  {DBGFIELD("SLDI_B_SLDI_D_SLDI_H_SLDI_W_SLD_B_SLD_D_SLD_H_SLD_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #500
  {DBGFIELD("BSETI_B_BSETI_D_BSETI_H_BSETI_W_BSET_B_BSET_D_BSET_H_BSET_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #501
  {DBGFIELD("BCLRI_B_BCLRI_D_BCLRI_H_BCLRI_W_BCLR_B_BCLR_D_BCLR_H_BCLR_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #502
  {DBGFIELD("BNEGI_B_BNEGI_D_BNEGI_H_BNEGI_W_BNEG_B_BNEG_D_BNEG_H_BNEG_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #503
  {DBGFIELD("BSELI_B_BSEL_V")     1, false, false, 26, 2,  2, 1,  0, 0}, // #504
  {DBGFIELD("BMNZI_B_BMNZ_V_BMZI_B_BMZ_V") 1, false, false, 26, 2,  2, 1,  0, 0}, // #505
  {DBGFIELD("PCNT_B_PCNT_D_PCNT_H_PCNT_W") 1, false, false, 26, 2,  7, 1,  0, 0}, // #506
  {DBGFIELD("SAT_S_B_SAT_S_D_SAT_S_H_SAT_S_W_SAT_U_B_SAT_U_D_SAT_U_H_SAT_U_W") 1, false, false, 26, 2,  7, 1,  0, 0}, // #507
  {DBGFIELD("BNZ_B_BNZ_D_BNZ_H_BNZ_V_BNZ_W_BZ_B_BZ_D_BZ_H_BZ_V_BZ_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #508
  {DBGFIELD("CFCMSA_CTCMSA")      1, false, false, 26, 2,  1, 1,  0, 0}, // #509
  {DBGFIELD("FABS_S_FABS_D32_FABS_D64") 1, false, false, 26, 2,  1, 1,  0, 0}, // #510
  {DBGFIELD("MOVF_D32_MOVF_D64")  1, false, false, 26, 2,  1, 1,  0, 0}, // #511
  {DBGFIELD("MOVF_S")             1, false, false, 26, 2,  1, 1,  0, 0}, // #512
  {DBGFIELD("MOVT_D32_MOVT_D64")  1, false, false, 26, 2,  1, 1,  0, 0}, // #513
  {DBGFIELD("MOVT_S")             1, false, false, 26, 2,  1, 1,  0, 0}, // #514
  {DBGFIELD("FMOV_D32_FMOV_D64")  1, false, false, 26, 2,  1, 1,  0, 0}, // #515
  {DBGFIELD("FMOV_S")             1, false, false, 26, 2,  1, 1,  0, 0}, // #516
  {DBGFIELD("FNEG_S_FNEG_D32_FNEG_D64") 1, false, false, 26, 2,  1, 1,  0, 0}, // #517
  {DBGFIELD("ADD_A_B_ADD_A_D_ADD_A_H_ADD_A_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #518
  {DBGFIELD("ADDS_A_B_ADDS_A_D_ADDS_A_H_ADDS_A_W_ADDS_S_B_ADDS_S_D_ADDS_S_H_ADDS_S_W_ADDS_U_B_ADDS_U_D_ADDS_U_H_ADDS_U_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #519
  {DBGFIELD("ADDVI_B_ADDVI_D_ADDVI_H_ADDVI_W_ADDV_B_ADDV_D_ADDV_H_ADDV_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #520
  {DBGFIELD("ASUB_S_B_ASUB_S_D_ASUB_S_H_ASUB_S_W_ASUB_U_B_ASUB_U_D_ASUB_U_H_ASUB_U_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #521
  {DBGFIELD("AVER_S_B_AVER_S_D_AVER_S_H_AVER_S_W_AVER_U_B_AVER_U_D_AVER_U_H_AVER_U_W_AVE_S_B_AVE_S_D_AVE_S_H_AVE_S_W_AVE_U_B_AVE_U_D_AVE_U_H_AVE_U_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #522
  {DBGFIELD("SHF_B_SHF_H_SHF_W")  1, false, false, 26, 2,  2, 1,  0, 0}, // #523
  {DBGFIELD("FILL_B_FILL_D_FILL_H_FILL_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #524
  {DBGFIELD("SPLATI_B_SPLATI_D_SPLATI_H_SPLATI_W_SPLAT_B_SPLAT_D_SPLAT_H_SPLAT_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #525
  {DBGFIELD("MOVE_V")             1, false, false, 26, 2,  1, 1,  0, 0}, // #526
  {DBGFIELD("LDI_B_LDI_D_LDI_H_LDI_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #527
  {DBGFIELD("AND_V_NOR_V_OR_V_XOR_V") 1, false, false, 26, 2,  1, 1,  0, 0}, // #528
  {DBGFIELD("ANDI_B_NORI_B_ORI_B_XORI_B") 1, false, false, 26, 2,  1, 1,  0, 0}, // #529
  {DBGFIELD("FEXP2_D_FEXP2_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #530
  {DBGFIELD("CLTI_S_B_CLTI_S_D_CLTI_S_H_CLTI_S_W_CLTI_U_B_CLTI_U_D_CLTI_U_H_CLTI_U_W_CLT_S_B_CLT_S_D_CLT_S_H_CLT_S_W_CLT_U_B_CLT_U_D_CLT_U_H_CLT_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #531
  {DBGFIELD("CLEI_S_B_CLEI_S_D_CLEI_S_H_CLEI_S_W_CLEI_U_B_CLEI_U_D_CLEI_U_H_CLEI_U_W_CLE_S_B_CLE_S_D_CLE_S_H_CLE_S_W_CLE_U_B_CLE_U_D_CLE_U_H_CLE_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #532
  {DBGFIELD("CEQI_B_CEQI_D_CEQI_H_CEQI_W_CEQ_B_CEQ_D_CEQ_H_CEQ_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #533
  {DBGFIELD("CMP_UN_D")           1, false, false, 26, 2,  1, 1,  0, 0}, // #534
  {DBGFIELD("CMP_UN_S")           1, false, false, 26, 2,  1, 1,  0, 0}, // #535
  {DBGFIELD("CMP_UEQ_D")          1, false, false, 26, 2,  1, 1,  0, 0}, // #536
  {DBGFIELD("CMP_UEQ_S")          1, false, false, 26, 2,  1, 1,  0, 0}, // #537
  {DBGFIELD("CMP_EQ_D")           1, false, false, 26, 2,  1, 1,  0, 0}, // #538
  {DBGFIELD("CMP_EQ_S")           1, false, false, 26, 2,  1, 1,  0, 0}, // #539
  {DBGFIELD("CMP_LT_D")           1, false, false, 26, 2,  1, 1,  0, 0}, // #540
  {DBGFIELD("CMP_LT_S")           1, false, false, 26, 2,  1, 1,  0, 0}, // #541
  {DBGFIELD("CMP_ULT_D")          1, false, false, 26, 2,  1, 1,  0, 0}, // #542
  {DBGFIELD("CMP_ULT_S")          1, false, false, 26, 2,  1, 1,  0, 0}, // #543
  {DBGFIELD("CMP_LE_D")           1, false, false, 26, 2,  1, 1,  0, 0}, // #544
  {DBGFIELD("CMP_LE_S")           1, false, false, 26, 2,  1, 1,  0, 0}, // #545
  {DBGFIELD("CMP_ULE_D")          1, false, false, 26, 2,  1, 1,  0, 0}, // #546
  {DBGFIELD("CMP_ULE_S")          1, false, false, 26, 2,  1, 1,  0, 0}, // #547
  {DBGFIELD("FSAF_D_FSAF_W_FSEQ_D_FSEQ_W_FSLE_D_FSLE_W_FSLT_D_FSLT_W_FSNE_D_FSNE_W_FSOR_D_FSOR_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #548
  {DBGFIELD("FSUEQ_D_FSUEQ_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #549
  {DBGFIELD("FSULE_D_FSULE_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #550
  {DBGFIELD("FSULT_D_FSULT_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #551
  {DBGFIELD("FSUNE_D_FSUNE_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #552
  {DBGFIELD("FSUN_D_FSUN_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #553
  {DBGFIELD("FCAF_D_FCAF_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #554
  {DBGFIELD("FCEQ_D_FCEQ_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #555
  {DBGFIELD("FCLE_D_FCLE_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #556
  {DBGFIELD("FCLT_D_FCLT_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #557
  {DBGFIELD("FCNE_D_FCNE_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #558
  {DBGFIELD("FCOR_D_FCOR_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #559
  {DBGFIELD("FCUEQ_D_FCUEQ_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #560
  {DBGFIELD("FCULE_D_FCULE_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #561
  {DBGFIELD("FCULT_D_FCULT_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #562
  {DBGFIELD("FCUNE_D_FCUNE_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #563
  {DBGFIELD("FCUN_D_FCUN_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #564
  {DBGFIELD("FABS_D_FABS_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #565
  {DBGFIELD("FFINT_S_D_FFINT_S_W_FFINT_U_D_FFINT_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #566
  {DBGFIELD("FFQL_D_FFQL_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #567
  {DBGFIELD("FFQR_D_FFQR_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #568
  {DBGFIELD("FTINT_S_D_FTINT_S_W_FTINT_U_D_FTINT_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #569
  {DBGFIELD("FRINT_D_FRINT_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #570
  {DBGFIELD("FTQ_H_FTQ_W")        1, false, false, 26, 2,  1, 1,  0, 0}, // #571
  {DBGFIELD("FTRUNC_S_D_FTRUNC_S_W_FTRUNC_U_D_FTRUNC_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #572
  {DBGFIELD("FEXDO_H_FEXDO_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #573
  {DBGFIELD("FEXUPL_D_FEXUPL_W")  1, false, false, 26, 2,  1, 1,  0, 0}, // #574
  {DBGFIELD("FEXUPR_D_FEXUPR_W")  1, false, false, 26, 2,  1, 1,  0, 0}, // #575
  {DBGFIELD("FCLASS_D_FCLASS_W")  1, false, false, 26, 2,  1, 1,  0, 0}, // #576
  {DBGFIELD("FMAX_A_D_FMAX_A_W")  1, false, false, 26, 2,  1, 1,  0, 0}, // #577
  {DBGFIELD("FMAX_D_FMAX_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #578
  {DBGFIELD("FMIN_A_D_FMIN_A_W")  1, false, false, 26, 2,  1, 1,  0, 0}, // #579
  {DBGFIELD("FMIN_D_FMIN_W")      1, false, false, 26, 2,  1, 1,  0, 0}, // #580
  {DBGFIELD("FLOG2_D_FLOG2_W")    1, false, false, 26, 2,  1, 1,  0, 0}, // #581
  {DBGFIELD("ILVL_B_ILVL_D_ILVL_H_ILVL_W_ILVR_B_ILVR_D_ILVR_H_ILVR_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #582
  {DBGFIELD("ILVEV_B_ILVEV_D_ILVEV_H_ILVEV_W_ILVOD_B_ILVOD_D_ILVOD_H_ILVOD_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #583
  {DBGFIELD("INSVE_B_INSVE_D_INSVE_H_INSVE_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #584
  {DBGFIELD("SUBS_S_B_SUBS_S_D_SUBS_S_H_SUBS_S_W_SUBS_U_B_SUBS_U_D_SUBS_U_H_SUBS_U_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #585
  {DBGFIELD("SUBSUS_U_B_SUBSUS_U_D_SUBSUS_U_H_SUBSUS_U_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #586
  {DBGFIELD("SUBSUU_S_B_SUBSUU_S_D_SUBSUU_S_H_SUBSUU_S_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #587
  {DBGFIELD("SUBVI_B_SUBVI_D_SUBVI_H_SUBVI_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #588
  {DBGFIELD("SUBV_B_SUBV_D_SUBV_H_SUBV_W") 1, false, false, 26, 2,  2, 1,  0, 0}, // #589
  {DBGFIELD("MOD_S_B_MOD_S_D_MOD_S_H_MOD_S_W_MOD_U_B_MOD_U_D_MOD_U_H_MOD_U_W") 1, false, false, 55, 3, 15, 1,  0, 0}, // #590
  {DBGFIELD("DIV_S_B_DIV_S_D_DIV_S_H_DIV_S_W_DIV_U_B_DIV_U_D_DIV_U_H_DIV_U_W") 1, false, false, 55, 3, 15, 1,  0, 0}, // #591
  {DBGFIELD("HADD_S_D_HADD_S_H_HADD_S_W_HADD_U_D_HADD_U_H_HADD_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #592
  {DBGFIELD("HSUB_S_D_HSUB_S_H_HSUB_S_W_HSUB_U_D_HSUB_U_H_HSUB_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #593
  {DBGFIELD("MAX_S_B_MAX_S_D_MAX_S_H_MAX_S_W_MIN_S_B_MIN_S_D_MIN_S_H_MIN_S_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #594
  {DBGFIELD("MAX_U_B_MAX_U_D_MAX_U_H_MAX_U_W_MIN_U_B_MIN_U_D_MIN_U_H_MIN_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #595
  {DBGFIELD("MAX_A_B_MAX_A_D_MAX_A_H_MAX_A_W_MIN_A_B_MIN_A_D_MIN_A_H_MIN_A_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #596
  {DBGFIELD("MAXI_S_B_MAXI_S_D_MAXI_S_H_MAXI_S_W_MAXI_U_B_MAXI_U_D_MAXI_U_H_MAXI_U_W_MINI_S_B_MINI_S_D_MINI_S_H_MINI_S_W_MINI_U_B_MINI_U_D_MINI_U_H_MINI_U_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #597
  {DBGFIELD("SRAI_B_SRAI_D_SRAI_H_SRAI_W_SRA_B_SRA_D_SRA_H_SRA_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #598
  {DBGFIELD("SRLI_B_SRLI_D_SRLI_H_SRLI_W_SRL_B_SRL_D_SRL_H_SRL_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #599
  {DBGFIELD("SRARI_B_SRARI_D_SRARI_H_SRARI_W_SRAR_B_SRAR_D_SRAR_H_SRAR_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #600
  {DBGFIELD("SRLRI_B_SRLRI_D_SRLRI_H_SRLRI_W_SRLR_B_SRLR_D_SRLR_H_SRLR_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #601
  {DBGFIELD("SLLI_B_SLLI_D_SLLI_H_SLLI_W_SLL_B_SLL_D_SLL_H_SLL_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #602
  {DBGFIELD("PCKEV_B_PCKEV_D_PCKEV_H_PCKEV_W_PCKOD_B_PCKOD_D_PCKOD_H_PCKOD_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #603
  {DBGFIELD("NLOC_B_NLOC_D_NLOC_H_NLOC_W_NLZC_B_NLZC_D_NLZC_H_NLZC_W") 1, false, false, 26, 2,  1, 1,  0, 0}, // #604
  {DBGFIELD("FADD_D32_FADD_D64")  1, false, false, 58, 2,  6, 1,  0, 0}, // #605
  {DBGFIELD("FADD_S")             1, false, false, 58, 2,  6, 1,  0, 0}, // #606
  {DBGFIELD("FMUL_D32_FMUL_D64")  1, false, false, 58, 2,  6, 1,  0, 0}, // #607
  {DBGFIELD("FMUL_S")             1, false, false, 58, 2,  6, 1,  0, 0}, // #608
  {DBGFIELD("FSUB_D32_FSUB_D64")  1, false, false, 58, 2,  6, 1,  0, 0}, // #609
  {DBGFIELD("FSUB_S")             1, false, false, 58, 2,  6, 1,  0, 0}, // #610
  {DBGFIELD("TRUNC_L_D64_TRUNC_L_S_TRUNC_W_D32_TRUNC_W_D64_TRUNC_W_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #611
  {DBGFIELD("CVT_D32_S_CVT_D32_W_CVT_D64_L_CVT_D64_S_CVT_D64_W_CVT_L_D64_CVT_L_S_CVT_S_D32_CVT_S_D64_CVT_S_L_CVT_S_W_CVT_W_D32_CVT_W_D64_CVT_W_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #612
  {DBGFIELD("C_EQ_D32_C_EQ_D64_C_F_D32_C_F_D64_C_LE_D32_C_LE_D64_C_LT_D32_C_LT_D64_C_NGE_D32_C_NGE_D64_C_NGLE_D32_C_NGLE_D64_C_NGL_D32_C_NGL_D64_C_NGT_D32_C_NGT_D64_C_OLE_D32_C_OLE_D64_C_OLT_D32_C_OLT_D64_C_SEQ_D32_C_SEQ_D64_C_SF_D32_C_SF_D64_C_UEQ_D32_C_UEQ_D64_C_ULE_D32_C_ULE_D64_C_ULT_D32_C_ULT_D64_C_UN_D32_C_UN_D64") 1, false, false, 58, 2,  6, 1,  0, 0}, // #613
  {DBGFIELD("C_EQ_S_C_F_S_C_LE_S_C_LT_S_C_NGE_S_C_NGLE_S_C_NGL_S_C_NGT_S_C_OLE_S_C_OLT_S_C_SEQ_S_C_SF_S_C_UEQ_S_C_ULE_S_C_ULT_S_C_UN_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #614
  {DBGFIELD("FCMP_D32_FCMP_D64")  1, false, false, 58, 2,  6, 1,  0, 0}, // #615
  {DBGFIELD("FCMP_S32")           1, false, false, 58, 2,  6, 1,  0, 0}, // #616
  {DBGFIELD("PseudoCVT_D32_W_PseudoCVT_D64_L_PseudoCVT_D64_W_PseudoCVT_S_L_PseudoCVT_S_W") 1, false, false, 58, 2,  6, 1,  0, 0}, // #617
  {DBGFIELD("FDIV_S")             1, false, false, 55, 3, 15, 1,  0, 0}, // #618
  {DBGFIELD("FDIV_D32_FDIV_D64")  1, false, false, 60, 3,  5, 1,  0, 0}, // #619
  {DBGFIELD("FSQRT_S")            1, false, false, 63, 3, 16, 1,  0, 0}, // #620
  {DBGFIELD("FSQRT_D32_FSQRT_D64") 1, false, false, 66, 3, 17, 1,  0, 0}, // #621
  {DBGFIELD("FRCP_D_FRCP_W")      1, false, false, 63, 3, 16, 1,  0, 0}, // #622
  {DBGFIELD("FRSQRT_D_FRSQRT_W")  1, false, false, 63, 3, 16, 1,  0, 0}, // #623
  {DBGFIELD("RECIP_D32_RECIP_D64") 1, false, false, 63, 3, 16, 1,  0, 0}, // #624
  {DBGFIELD("RSQRT_D32_RSQRT_D64") 1, false, false, 63, 3, 16, 1,  0, 0}, // #625
  {DBGFIELD("RECIP_S")            1, false, false, 63, 3, 16, 1,  0, 0}, // #626
  {DBGFIELD("RSQRT_S")            1, false, false, 63, 3, 16, 1,  0, 0}, // #627
  {DBGFIELD("FMADD_D_FMADD_W")    1, false, false, 58, 2, 13, 1,  0, 0}, // #628
  {DBGFIELD("FMSUB_D_FMSUB_W")    1, false, false, 58, 2, 13, 1,  0, 0}, // #629
  {DBGFIELD("FDIV_W")             1, false, false, 55, 3, 15, 1,  0, 0}, // #630
  {DBGFIELD("FDIV_D")             1, false, false, 60, 3,  5, 1,  0, 0}, // #631
  {DBGFIELD("FSQRT_W")            1, false, false, 63, 3, 16, 1,  0, 0}, // #632
  {DBGFIELD("FSQRT_D")            1, false, false, 66, 3, 17, 1,  0, 0}, // #633
  {DBGFIELD("FMUL_D_FMUL_W")      1, false, false, 58, 2,  6, 1,  0, 0}, // #634
  {DBGFIELD("FADD_D_FADD_W")      1, false, false, 58, 2,  6, 1,  0, 0}, // #635
  {DBGFIELD("FSUB_D_FSUB_W")      1, false, false, 58, 2,  6, 1,  0, 0}, // #636
  {DBGFIELD("DPADD_S_D_DPADD_S_H_DPADD_S_W_DPADD_U_D_DPADD_U_H_DPADD_U_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #637
  {DBGFIELD("DPSUB_S_D_DPSUB_S_H_DPSUB_S_W_DPSUB_U_D_DPSUB_U_H_DPSUB_U_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #638
  {DBGFIELD("DOTP_S_D_DOTP_S_H_DOTP_S_W_DOTP_U_D_DOTP_U_H_DOTP_U_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #639
  {DBGFIELD("MSUBV_B_MSUBV_D_MSUBV_H_MSUBV_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #640
  {DBGFIELD("MADDV_B_MADDV_D_MADDV_H_MADDV_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #641
  {DBGFIELD("MULV_B_MULV_D_MULV_H_MULV_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #642
  {DBGFIELD("MADDR_Q_H_MADDR_Q_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #643
  {DBGFIELD("MADD_Q_H_MADD_Q_W")  1, false, false, 26, 2,  3, 1,  0, 0}, // #644
  {DBGFIELD("MSUBR_Q_H_MSUBR_Q_W") 1, false, false, 26, 2,  3, 1,  0, 0}, // #645
  {DBGFIELD("MSUB_Q_H_MSUB_Q_W")  1, false, false, 26, 2,  3, 1,  0, 0}, // #646
  {DBGFIELD("MULR_Q_H_MULR_Q_W")  1, false, false, 26, 2,  3, 1,  0, 0}, // #647
  {DBGFIELD("MUL_Q_H_MUL_Q_W")    1, false, false, 26, 2,  3, 1,  0, 0}, // #648
  {DBGFIELD("MADD_D32_MADD_D64")  1, false, false, 58, 2, 13, 1,  1, 1}, // #649
  {DBGFIELD("MADD_S")             1, false, false, 58, 2, 13, 1,  1, 1}, // #650
  {DBGFIELD("MSUB_D32_MSUB_D64")  1, false, false, 58, 2, 13, 1,  1, 1}, // #651
  {DBGFIELD("MSUB_S")             1, false, false, 58, 2, 13, 1,  1, 1}, // #652
  {DBGFIELD("NMADD_D32_NMADD_D64") 1, false, false, 58, 2, 13, 1,  1, 1}, // #653
  {DBGFIELD("NMADD_S")            1, false, false, 58, 2, 13, 1,  1, 1}, // #654
  {DBGFIELD("NMSUB_D32_NMSUB_D64") 1, false, false, 58, 2, 13, 1,  1, 1}, // #655
  {DBGFIELD("NMSUB_S")            1, false, false, 58, 2, 13, 1,  1, 1}, // #656
  {DBGFIELD("CTC1")               2, false, false, 69, 4,  1, 1,  0, 0}, // #657
  {DBGFIELD("MTC1_MTC1_D64_BuildPairF64_BuildPairF64_64") 2, false, false, 69, 4,  1, 1,  0, 0}, // #658
  {DBGFIELD("MTHC1_D32_MTHC1_D64") 2, false, false, 69, 4,  1, 1,  0, 0}, // #659
  {DBGFIELD("COPY_U_B_COPY_U_H_COPY_U_W") 2, false, false, 69, 4,  2, 1,  0, 0}, // #660
  {DBGFIELD("COPY_S_B_COPY_S_D_COPY_S_H_COPY_S_W") 2, false, false, 69, 4,  2, 1,  0, 0}, // #661
  {DBGFIELD("BC1F")               2, false, false, 69, 4,  2, 1,  0, 0}, // #662
  {DBGFIELD("BC1FL")              2, false, false, 69, 4,  2, 1,  0, 0}, // #663
  {DBGFIELD("BC1T")               2, false, false, 69, 4,  2, 1,  0, 0}, // #664
  {DBGFIELD("BC1TL")              2, false, false, 69, 4,  2, 1,  0, 0}, // #665
  {DBGFIELD("CFC1")               2, false, false, 69, 4,  2, 1,  0, 0}, // #666
  {DBGFIELD("MFC1_MFC1_D64_ExtractElementF64_ExtractElementF64_64") 2, false, false, 69, 4,  2, 1,  0, 0}, // #667
  {DBGFIELD("MFHC1_D32_MFHC1_D64") 2, false, false, 69, 4,  2, 1,  0, 0}, // #668
  {DBGFIELD("MOVF_I")             2, false, false, 69, 4,  2, 1,  0, 0}, // #669
  {DBGFIELD("MOVT_I")             2, false, false, 69, 4,  2, 1,  0, 0}, // #670
  {DBGFIELD("SDC1")               2, false, false, 69, 4,  1, 1,  0, 0}, // #671
  {DBGFIELD("SDXC1")              2, false, false, 69, 4,  1, 1,  0, 0}, // #672
  {DBGFIELD("SUXC1")              2, false, false, 69, 4,  1, 1,  0, 0}, // #673
  {DBGFIELD("SWC1")               2, false, false, 69, 4,  1, 1,  0, 0}, // #674
  {DBGFIELD("SWXC1")              2, false, false, 69, 4,  1, 1,  0, 0}, // #675
  {DBGFIELD("ST_B_ST_D_ST_H_ST_W") 2, false, false, 69, 4,  1, 1,  0, 0}, // #676
  {DBGFIELD("MOVN_I_D32_MOVN_I_D64") 2, false, false, 73, 4,  2, 1,  0, 0}, // #677
  {DBGFIELD("MOVN_I_S")           2, false, false, 73, 4,  2, 1,  0, 0}, // #678
  {DBGFIELD("MOVZ_I_D32_MOVZ_I_D64") 2, false, false, 73, 4,  2, 1,  0, 0}, // #679
  {DBGFIELD("MOVZ_I_S")           2, false, false, 73, 4,  2, 1,  0, 0}, // #680
  {DBGFIELD("LDC1")               2, false, false, 77, 4,  6, 1,  0, 0}, // #681
  {DBGFIELD("LDXC1")              2, false, false, 77, 4,  6, 1,  0, 0}, // #682
  {DBGFIELD("LWC1")               2, false, false, 77, 4,  6, 1,  0, 0}, // #683
  {DBGFIELD("LWXC1")              2, false, false, 77, 4,  6, 1,  0, 0}, // #684
  {DBGFIELD("LUXC1")              2, false, false, 77, 4,  6, 1,  0, 0}, // #685
  {DBGFIELD("LD_B_LD_D_LD_H_LD_W") 2, false, false, 77, 4,  6, 1,  0, 0}, // #686
  {DBGFIELD("CEIL_L_D64_CEIL_L_S_CEIL_W_D32_CEIL_W_D64_CEIL_W_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #687
  {DBGFIELD("FLOOR_L_D64_FLOOR_L_S_FLOOR_W_D32_FLOOR_W_D64_FLOOR_W_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #688
  {DBGFIELD("ROUND_L_D64_ROUND_L_S_ROUND_W_D32_ROUND_W_D64_ROUND_W_S") 1, false, false, 58, 2,  6, 1,  0, 0}, // #689
  {DBGFIELD("ROTRV")              16382, false, false,  0, 0,  0, 0,  0, 0}, // #690
  {DBGFIELD("EXTRV_RS_W")         0, false, false,  0, 0,  0, 0,  0, 0}, // #691
  {DBGFIELD("EXTRV_R_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #692
  {DBGFIELD("EXTRV_S_H")          0, false, false,  0, 0,  0, 0,  0, 0}, // #693
  {DBGFIELD("EXTRV_W")            0, false, false,  0, 0,  0, 0,  0, 0}, // #694
  {DBGFIELD("EXTR_RS_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #695
  {DBGFIELD("EXTR_R_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #696
  {DBGFIELD("EXTR_S_H")           0, false, false,  0, 0,  0, 0,  0, 0}, // #697
  {DBGFIELD("EXTR_W")             0, false, false,  0, 0,  0, 0,  0, 0}, // #698
  {DBGFIELD("INSV")               0, false, false,  0, 0,  0, 0,  0, 0}, // #699
  {DBGFIELD("MTHLIP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #700
  {DBGFIELD("MTHI_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #701
  {DBGFIELD("MTLO_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #702
  {DBGFIELD("ABSQ_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #703
  {DBGFIELD("ABSQ_S_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #704
  {DBGFIELD("ADDQ_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #705
  {DBGFIELD("ADDQ_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #706
  {DBGFIELD("ADDQ_S_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #707
  {DBGFIELD("ADDSC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #708
  {DBGFIELD("ADDU_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #709
  {DBGFIELD("ADDU_S_QB")          0, false, false,  0, 0,  0, 0,  0, 0}, // #710
  {DBGFIELD("ADDWC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #711
  {DBGFIELD("BITREV")             0, false, false,  0, 0,  0, 0,  0, 0}, // #712
  {DBGFIELD("BPOSGE32")           0, false, false,  0, 0,  0, 0,  0, 0}, // #713
  {DBGFIELD("CMPGU_EQ_QB")        0, false, false,  0, 0,  0, 0,  0, 0}, // #714
  {DBGFIELD("CMPGU_LE_QB")        0, false, false,  0, 0,  0, 0,  0, 0}, // #715
  {DBGFIELD("CMPGU_LT_QB")        0, false, false,  0, 0,  0, 0,  0, 0}, // #716
  {DBGFIELD("CMPU_EQ_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #717
  {DBGFIELD("CMPU_LE_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #718
  {DBGFIELD("CMPU_LT_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #719
  {DBGFIELD("CMP_EQ_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #720
  {DBGFIELD("CMP_LE_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #721
  {DBGFIELD("CMP_LT_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #722
  {DBGFIELD("DPAQ_SA_L_W")        0, false, false,  0, 0,  0, 0,  0, 0}, // #723
  {DBGFIELD("DPAQ_S_W_PH")        0, false, false,  0, 0,  0, 0,  0, 0}, // #724
  {DBGFIELD("DPAU_H_QBL")         0, false, false,  0, 0,  0, 0,  0, 0}, // #725
  {DBGFIELD("DPAU_H_QBR")         0, false, false,  0, 0,  0, 0,  0, 0}, // #726
  {DBGFIELD("DPSQ_SA_L_W")        0, false, false,  0, 0,  0, 0,  0, 0}, // #727
  {DBGFIELD("DPSQ_S_W_PH")        0, false, false,  0, 0,  0, 0,  0, 0}, // #728
  {DBGFIELD("DPSU_H_QBL")         0, false, false,  0, 0,  0, 0,  0, 0}, // #729
  {DBGFIELD("DPSU_H_QBR")         0, false, false,  0, 0,  0, 0,  0, 0}, // #730
  {DBGFIELD("EXTPDPV")            0, false, false,  0, 0,  0, 0,  0, 0}, // #731
  {DBGFIELD("EXTPDP")             0, false, false,  0, 0,  0, 0,  0, 0}, // #732
  {DBGFIELD("EXTPV")              0, false, false,  0, 0,  0, 0,  0, 0}, // #733
  {DBGFIELD("EXTP")               0, false, false,  0, 0,  0, 0,  0, 0}, // #734
  {DBGFIELD("LBUX")               0, false, false,  0, 0,  0, 0,  0, 0}, // #735
  {DBGFIELD("LHX")                0, false, false,  0, 0,  0, 0,  0, 0}, // #736
  {DBGFIELD("LWX")                0, false, false,  0, 0,  0, 0,  0, 0}, // #737
  {DBGFIELD("MADDU_DSP")          0, false, false,  0, 0,  0, 0,  0, 0}, // #738
  {DBGFIELD("MADD_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #739
  {DBGFIELD("MAQ_SA_W_PHL")       0, false, false,  0, 0,  0, 0,  0, 0}, // #740
  {DBGFIELD("MAQ_SA_W_PHR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #741
  {DBGFIELD("MAQ_S_W_PHL")        0, false, false,  0, 0,  0, 0,  0, 0}, // #742
  {DBGFIELD("MAQ_S_W_PHR")        0, false, false,  0, 0,  0, 0,  0, 0}, // #743
  {DBGFIELD("MFHI_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #744
  {DBGFIELD("MFLO_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #745
  {DBGFIELD("MODSUB")             0, false, false,  0, 0,  0, 0,  0, 0}, // #746
  {DBGFIELD("MSUBU_DSP")          0, false, false,  0, 0,  0, 0,  0, 0}, // #747
  {DBGFIELD("MSUB_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #748
  {DBGFIELD("MULEQ_S_W_PHL")      0, false, false,  0, 0,  0, 0,  0, 0}, // #749
  {DBGFIELD("MULEQ_S_W_PHR")      0, false, false,  0, 0,  0, 0,  0, 0}, // #750
  {DBGFIELD("MULEU_S_PH_QBL")     0, false, false,  0, 0,  0, 0,  0, 0}, // #751
  {DBGFIELD("MULEU_S_PH_QBR")     0, false, false,  0, 0,  0, 0,  0, 0}, // #752
  {DBGFIELD("MULQ_RS_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #753
  {DBGFIELD("MULSAQ_S_W_PH")      0, false, false,  0, 0,  0, 0,  0, 0}, // #754
  {DBGFIELD("MULTU_DSP")          0, false, false,  0, 0,  0, 0,  0, 0}, // #755
  {DBGFIELD("MULT_DSP")           0, false, false,  0, 0,  0, 0,  0, 0}, // #756
  {DBGFIELD("PACKRL_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #757
  {DBGFIELD("PICK_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #758
  {DBGFIELD("PICK_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #759
  {DBGFIELD("PRECEQU_PH_QBLA")    0, false, false,  0, 0,  0, 0,  0, 0}, // #760
  {DBGFIELD("PRECEQU_PH_QBL")     0, false, false,  0, 0,  0, 0,  0, 0}, // #761
  {DBGFIELD("PRECEQU_PH_QBRA")    0, false, false,  0, 0,  0, 0,  0, 0}, // #762
  {DBGFIELD("PRECEQU_PH_QBR")     0, false, false,  0, 0,  0, 0,  0, 0}, // #763
  {DBGFIELD("PRECEQ_W_PHL")       0, false, false,  0, 0,  0, 0,  0, 0}, // #764
  {DBGFIELD("PRECEQ_W_PHR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #765
  {DBGFIELD("PRECEU_PH_QBLA")     0, false, false,  0, 0,  0, 0,  0, 0}, // #766
  {DBGFIELD("PRECEU_PH_QBL")      0, false, false,  0, 0,  0, 0,  0, 0}, // #767
  {DBGFIELD("PRECEU_PH_QBRA")     0, false, false,  0, 0,  0, 0,  0, 0}, // #768
  {DBGFIELD("PRECEU_PH_QBR")      0, false, false,  0, 0,  0, 0,  0, 0}, // #769
  {DBGFIELD("PRECRQU_S_QB_PH")    0, false, false,  0, 0,  0, 0,  0, 0}, // #770
  {DBGFIELD("PRECRQ_PH_W")        0, false, false,  0, 0,  0, 0,  0, 0}, // #771
  {DBGFIELD("PRECRQ_QB_PH")       0, false, false,  0, 0,  0, 0,  0, 0}, // #772
  {DBGFIELD("PRECRQ_RS_PH_W")     0, false, false,  0, 0,  0, 0,  0, 0}, // #773
  {DBGFIELD("RADDU_W_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #774
  {DBGFIELD("RDDSP")              0, false, false,  0, 0,  0, 0,  0, 0}, // #775
  {DBGFIELD("REPLV_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #776
  {DBGFIELD("REPLV_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #777
  {DBGFIELD("REPL_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #778
  {DBGFIELD("REPL_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #779
  {DBGFIELD("SHILOV")             0, false, false,  0, 0,  0, 0,  0, 0}, // #780
  {DBGFIELD("SHILO")              0, false, false,  0, 0,  0, 0,  0, 0}, // #781
  {DBGFIELD("SHLLV_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #782
  {DBGFIELD("SHLLV_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #783
  {DBGFIELD("SHLLV_S_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #784
  {DBGFIELD("SHLLV_S_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #785
  {DBGFIELD("SHLL_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #786
  {DBGFIELD("SHLL_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #787
  {DBGFIELD("SHLL_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #788
  {DBGFIELD("SHLL_S_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #789
  {DBGFIELD("SHRAV_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #790
  {DBGFIELD("SHRAV_R_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #791
  {DBGFIELD("SHRAV_R_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #792
  {DBGFIELD("SHRA_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #793
  {DBGFIELD("SHRA_R_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #794
  {DBGFIELD("SHRA_R_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #795
  {DBGFIELD("SHRLV_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #796
  {DBGFIELD("SHRL_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #797
  {DBGFIELD("SUBQ_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #798
  {DBGFIELD("SUBQ_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #799
  {DBGFIELD("SUBQ_S_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #800
  {DBGFIELD("SUBU_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #801
  {DBGFIELD("SUBU_S_QB")          0, false, false,  0, 0,  0, 0,  0, 0}, // #802
  {DBGFIELD("WRDSP")              0, false, false,  0, 0,  0, 0,  0, 0}, // #803
  {DBGFIELD("ABSQ_S_QB")          0, false, false,  0, 0,  0, 0,  0, 0}, // #804
  {DBGFIELD("ADDQH_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #805
  {DBGFIELD("ADDQH_R_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #806
  {DBGFIELD("ADDQH_R_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #807
  {DBGFIELD("ADDQH_W")            0, false, false,  0, 0,  0, 0,  0, 0}, // #808
  {DBGFIELD("ADDUH_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #809
  {DBGFIELD("ADDUH_R_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #810
  {DBGFIELD("ADDU_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #811
  {DBGFIELD("ADDU_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #812
  {DBGFIELD("APPEND")             0, false, false,  0, 0,  0, 0,  0, 0}, // #813
  {DBGFIELD("BALIGN")             0, false, false,  0, 0,  0, 0,  0, 0}, // #814
  {DBGFIELD("CMPGDU_EQ_QB")       0, false, false,  0, 0,  0, 0,  0, 0}, // #815
  {DBGFIELD("CMPGDU_LE_QB")       0, false, false,  0, 0,  0, 0,  0, 0}, // #816
  {DBGFIELD("CMPGDU_LT_QB")       0, false, false,  0, 0,  0, 0,  0, 0}, // #817
  {DBGFIELD("DPA_W_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #818
  {DBGFIELD("DPAQX_SA_W_PH")      0, false, false,  0, 0,  0, 0,  0, 0}, // #819
  {DBGFIELD("DPAQX_S_W_PH")       0, false, false,  0, 0,  0, 0,  0, 0}, // #820
  {DBGFIELD("DPAX_W_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #821
  {DBGFIELD("DPS_W_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #822
  {DBGFIELD("DPSQX_S_W_PH")       0, false, false,  0, 0,  0, 0,  0, 0}, // #823
  {DBGFIELD("DPSQX_SA_W_PH")      0, false, false,  0, 0,  0, 0,  0, 0}, // #824
  {DBGFIELD("DPSX_W_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #825
  {DBGFIELD("MUL_PH")             0, false, false,  0, 0,  0, 0,  0, 0}, // #826
  {DBGFIELD("MUL_S_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #827
  {DBGFIELD("MULQ_RS_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #828
  {DBGFIELD("MULQ_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #829
  {DBGFIELD("MULQ_S_W")           0, false, false,  0, 0,  0, 0,  0, 0}, // #830
  {DBGFIELD("MULSA_W_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #831
  {DBGFIELD("PRECR_QB_PH")        0, false, false,  0, 0,  0, 0,  0, 0}, // #832
  {DBGFIELD("PRECR_SRA_PH_W")     0, false, false,  0, 0,  0, 0,  0, 0}, // #833
  {DBGFIELD("PRECR_SRA_R_PH_W")   0, false, false,  0, 0,  0, 0,  0, 0}, // #834
  {DBGFIELD("PREPEND")            0, false, false,  0, 0,  0, 0,  0, 0}, // #835
  {DBGFIELD("SHRA_QB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #836
  {DBGFIELD("SHRA_R_QB")          0, false, false,  0, 0,  0, 0,  0, 0}, // #837
  {DBGFIELD("SHRAV_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #838
  {DBGFIELD("SHRAV_R_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #839
  {DBGFIELD("SHRL_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #840
  {DBGFIELD("SHRLV_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #841
  {DBGFIELD("SUBQH_PH")           0, false, false,  0, 0,  0, 0,  0, 0}, // #842
  {DBGFIELD("SUBQH_R_PH")         0, false, false,  0, 0,  0, 0,  0, 0}, // #843
  {DBGFIELD("SUBQH_W")            0, false, false,  0, 0,  0, 0,  0, 0}, // #844
  {DBGFIELD("SUBQH_R_W")          0, false, false,  0, 0,  0, 0,  0, 0}, // #845
  {DBGFIELD("SUBU_PH")            0, false, false,  0, 0,  0, 0,  0, 0}, // #846
  {DBGFIELD("SUBU_S_PH")          0, false, false,  0, 0,  0, 0,  0, 0}, // #847
  {DBGFIELD("SUBUH_QB")           0, false, false,  0, 0,  0, 0,  0, 0}, // #848
  {DBGFIELD("SUBUH_R_QB")         0, false, false,  0, 0,  0, 0,  0, 0}, // #849
  {DBGFIELD("ABSQ_S_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #850
  {DBGFIELD("ABSQ_S_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #851
  {DBGFIELD("ADDQ_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #852
  {DBGFIELD("ADDQ_S_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #853
  {DBGFIELD("ADDQ_S_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #854
  {DBGFIELD("ADDSC_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #855
  {DBGFIELD("ADDU_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #856
  {DBGFIELD("ADDU_S_QB_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #857
  {DBGFIELD("ADDWC_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #858
  {DBGFIELD("BITREV_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #859
  {DBGFIELD("BPOSGE32_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #860
  {DBGFIELD("CMPGU_EQ_QB_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #861
  {DBGFIELD("CMPGU_LE_QB_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #862
  {DBGFIELD("CMPGU_LT_QB_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #863
  {DBGFIELD("CMPU_EQ_QB_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #864
  {DBGFIELD("CMPU_LE_QB_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #865
  {DBGFIELD("CMPU_LT_QB_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #866
  {DBGFIELD("CMP_EQ_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #867
  {DBGFIELD("CMP_LE_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #868
  {DBGFIELD("CMP_LT_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #869
  {DBGFIELD("DPAQ_SA_L_W_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #870
  {DBGFIELD("DPAQ_S_W_PH_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #871
  {DBGFIELD("DPAU_H_QBL_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #872
  {DBGFIELD("DPAU_H_QBR_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #873
  {DBGFIELD("DPSQ_SA_L_W_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #874
  {DBGFIELD("DPSQ_S_W_PH_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #875
  {DBGFIELD("DPSU_H_QBL_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #876
  {DBGFIELD("DPSU_H_QBR_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #877
  {DBGFIELD("EXTPDPV_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #878
  {DBGFIELD("EXTPDP_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #879
  {DBGFIELD("EXTPV_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #880
  {DBGFIELD("EXTP_MM")            0, false, false,  0, 0,  0, 0,  0, 0}, // #881
  {DBGFIELD("EXTRV_RS_W_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #882
  {DBGFIELD("EXTRV_R_W_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #883
  {DBGFIELD("EXTRV_S_H_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #884
  {DBGFIELD("EXTRV_W_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #885
  {DBGFIELD("EXTR_RS_W_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #886
  {DBGFIELD("EXTR_R_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #887
  {DBGFIELD("EXTR_S_H_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #888
  {DBGFIELD("EXTR_W_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #889
  {DBGFIELD("INSV_MM")            0, false, false,  0, 0,  0, 0,  0, 0}, // #890
  {DBGFIELD("LBUX_MM")            0, false, false,  0, 0,  0, 0,  0, 0}, // #891
  {DBGFIELD("LHX_MM")             0, false, false,  0, 0,  0, 0,  0, 0}, // #892
  {DBGFIELD("LWX_MM")             0, false, false,  0, 0,  0, 0,  0, 0}, // #893
  {DBGFIELD("MADDU_DSP_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #894
  {DBGFIELD("MADD_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #895
  {DBGFIELD("MAQ_SA_W_PHL_MM")    0, false, false,  0, 0,  0, 0,  0, 0}, // #896
  {DBGFIELD("MAQ_SA_W_PHR_MM")    0, false, false,  0, 0,  0, 0,  0, 0}, // #897
  {DBGFIELD("MAQ_S_W_PHL_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #898
  {DBGFIELD("MAQ_S_W_PHR_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #899
  {DBGFIELD("MFHI_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #900
  {DBGFIELD("MFLO_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #901
  {DBGFIELD("MODSUB_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #902
  {DBGFIELD("MOVEP_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #903
  {DBGFIELD("MOVEP_MMR6")         0, false, false,  0, 0,  0, 0,  0, 0}, // #904
  {DBGFIELD("MOVN_I_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #905
  {DBGFIELD("MOVZ_I_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #906
  {DBGFIELD("MSUBU_DSP_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #907
  {DBGFIELD("MSUB_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #908
  {DBGFIELD("MTHI_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #909
  {DBGFIELD("MTHLIP_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #910
  {DBGFIELD("MTLO_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #911
  {DBGFIELD("MULEQ_S_W_PHL_MM")   0, false, false,  0, 0,  0, 0,  0, 0}, // #912
  {DBGFIELD("MULEQ_S_W_PHR_MM")   0, false, false,  0, 0,  0, 0,  0, 0}, // #913
  {DBGFIELD("MULEU_S_PH_QBL_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #914
  {DBGFIELD("MULEU_S_PH_QBR_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #915
  {DBGFIELD("MULQ_RS_PH_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #916
  {DBGFIELD("MULSAQ_S_W_PH_MM")   0, false, false,  0, 0,  0, 0,  0, 0}, // #917
  {DBGFIELD("MULTU_DSP_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #918
  {DBGFIELD("MULT_DSP_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #919
  {DBGFIELD("PACKRL_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #920
  {DBGFIELD("PICK_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #921
  {DBGFIELD("PICK_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #922
  {DBGFIELD("PRECEQU_PH_QBLA_MM") 0, false, false,  0, 0,  0, 0,  0, 0}, // #923
  {DBGFIELD("PRECEQU_PH_QBL_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #924
  {DBGFIELD("PRECEQU_PH_QBRA_MM") 0, false, false,  0, 0,  0, 0,  0, 0}, // #925
  {DBGFIELD("PRECEQU_PH_QBR_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #926
  {DBGFIELD("PRECEQ_W_PHL_MM")    0, false, false,  0, 0,  0, 0,  0, 0}, // #927
  {DBGFIELD("PRECEQ_W_PHR_MM")    0, false, false,  0, 0,  0, 0,  0, 0}, // #928
  {DBGFIELD("PRECEU_PH_QBLA_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #929
  {DBGFIELD("PRECEU_PH_QBL_MM")   0, false, false,  0, 0,  0, 0,  0, 0}, // #930
  {DBGFIELD("PRECEU_PH_QBRA_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #931
  {DBGFIELD("PRECEU_PH_QBR_MM")   0, false, false,  0, 0,  0, 0,  0, 0}, // #932
  {DBGFIELD("PRECRQU_S_QB_PH_MM") 0, false, false,  0, 0,  0, 0,  0, 0}, // #933
  {DBGFIELD("PRECRQ_PH_W_MM")     0, false, false,  0, 0,  0, 0,  0, 0}, // #934
  {DBGFIELD("PRECRQ_QB_PH_MM")    0, false, false,  0, 0,  0, 0,  0, 0}, // #935
  {DBGFIELD("PRECRQ_RS_PH_W_MM")  0, false, false,  0, 0,  0, 0,  0, 0}, // #936
  {DBGFIELD("RADDU_W_QB_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #937
  {DBGFIELD("RDDSP_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #938
  {DBGFIELD("REPLV_PH_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #939
  {DBGFIELD("REPLV_QB_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #940
  {DBGFIELD("REPL_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #941
  {DBGFIELD("REPL_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #942
  {DBGFIELD("SHILOV_MM")          0, false, false,  0, 0,  0, 0,  0, 0}, // #943
  {DBGFIELD("SHILO_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #944
  {DBGFIELD("SHLLV_PH_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #945
  {DBGFIELD("SHLLV_QB_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #946
  {DBGFIELD("SHLLV_S_PH_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #947
  {DBGFIELD("SHLLV_S_W_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #948
  {DBGFIELD("SHLL_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #949
  {DBGFIELD("SHLL_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #950
  {DBGFIELD("SHLL_S_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #951
  {DBGFIELD("SHLL_S_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #952
  {DBGFIELD("SHRAV_PH_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #953
  {DBGFIELD("SHRAV_R_PH_MM")      0, false, false,  0, 0,  0, 0,  0, 0}, // #954
  {DBGFIELD("SHRAV_R_W_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #955
  {DBGFIELD("SHRA_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #956
  {DBGFIELD("SHRA_R_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #957
  {DBGFIELD("SHRA_R_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #958
  {DBGFIELD("SHRLV_QB_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #959
  {DBGFIELD("SHRL_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #960
  {DBGFIELD("SUBQ_PH_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #961
  {DBGFIELD("SUBQ_S_PH_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #962
  {DBGFIELD("SUBQ_S_W_MM")        0, false, false,  0, 0,  0, 0,  0, 0}, // #963
  {DBGFIELD("SUBU_QB_MM")         0, false, false,  0, 0,  0, 0,  0, 0}, // #964
  {DBGFIELD("SUBU_S_QB_MM")       0, false, false,  0, 0,  0, 0,  0, 0}, // #965
  {DBGFIELD("WRDSP_MM")           0, false, false,  0, 0,  0, 0,  0, 0}, // #966
  {DBGFIELD("ABSQ_S_QB_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #967
  {DBGFIELD("ADDQH_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #968
  {DBGFIELD("ADDQH_R_PH_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #969
  {DBGFIELD("ADDQH_R_W_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #970
  {DBGFIELD("ADDQH_W_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #971
  {DBGFIELD("ADDUH_QB_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #972
  {DBGFIELD("ADDUH_R_QB_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #973
  {DBGFIELD("ADDU_PH_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #974
  {DBGFIELD("ADDU_S_PH_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #975
  {DBGFIELD("APPEND_MMR2")        0, false, false,  0, 0,  0, 0,  0, 0}, // #976
  {DBGFIELD("BALIGN_MMR2")        0, false, false,  0, 0,  0, 0,  0, 0}, // #977
  {DBGFIELD("CMPGDU_EQ_QB_MMR2")  0, false, false,  0, 0,  0, 0,  0, 0}, // #978
  {DBGFIELD("CMPGDU_LE_QB_MMR2")  0, false, false,  0, 0,  0, 0,  0, 0}, // #979
  {DBGFIELD("CMPGDU_LT_QB_MMR2")  0, false, false,  0, 0,  0, 0,  0, 0}, // #980
  {DBGFIELD("DPA_W_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #981
  {DBGFIELD("DPAQX_SA_W_PH_MMR2") 0, false, false,  0, 0,  0, 0,  0, 0}, // #982
  {DBGFIELD("DPAQX_S_W_PH_MMR2")  0, false, false,  0, 0,  0, 0,  0, 0}, // #983
  {DBGFIELD("DPAX_W_PH_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #984
  {DBGFIELD("DPS_W_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #985
  {DBGFIELD("DPSQX_S_W_PH_MMR2")  0, false, false,  0, 0,  0, 0,  0, 0}, // #986
  {DBGFIELD("DPSQX_SA_W_PH_MMR2") 0, false, false,  0, 0,  0, 0,  0, 0}, // #987
  {DBGFIELD("DPSX_W_PH_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #988
  {DBGFIELD("MUL_PH_MMR2")        0, false, false,  0, 0,  0, 0,  0, 0}, // #989
  {DBGFIELD("MUL_S_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #990
  {DBGFIELD("MULQ_RS_W_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #991
  {DBGFIELD("MULQ_S_PH_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #992
  {DBGFIELD("MULQ_S_W_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #993
  {DBGFIELD("MULSA_W_PH_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #994
  {DBGFIELD("PRECR_QB_PH_MMR2")   0, false, false,  0, 0,  0, 0,  0, 0}, // #995
  {DBGFIELD("PRECR_SRA_PH_W_MMR2") 0, false, false,  0, 0,  0, 0,  0, 0}, // #996
  {DBGFIELD("PRECR_SRA_R_PH_W_MMR2") 0, false, false,  0, 0,  0, 0,  0, 0}, // #997
  {DBGFIELD("PREPEND_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #998
  {DBGFIELD("SHRA_QB_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #999
  {DBGFIELD("SHRA_R_QB_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #1000
  {DBGFIELD("SHRAV_QB_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1001
  {DBGFIELD("SHRAV_R_QB_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #1002
  {DBGFIELD("SHRL_PH_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #1003
  {DBGFIELD("SHRLV_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1004
  {DBGFIELD("SUBQH_PH_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1005
  {DBGFIELD("SUBQH_R_PH_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #1006
  {DBGFIELD("SUBQH_W_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #1007
  {DBGFIELD("SUBQH_R_W_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #1008
  {DBGFIELD("SUBU_PH_MMR2")       0, false, false,  0, 0,  0, 0,  0, 0}, // #1009
  {DBGFIELD("SUBU_S_PH_MMR2")     0, false, false,  0, 0,  0, 0,  0, 0}, // #1010
  {DBGFIELD("SUBUH_QB_MMR2")      0, false, false,  0, 0,  0, 0,  0, 0}, // #1011
  {DBGFIELD("SUBUH_R_QB_MMR2")    0, false, false,  0, 0,  0, 0,  0, 0}, // #1012
  {DBGFIELD("BPOSGE32C_MMR3")     0, false, false,  0, 0,  0, 0,  0, 0}, // #1013
  {DBGFIELD("P5600WriteALU")      1, false, false, 36, 2,  1, 1,  0, 0}, // #1014
  {DBGFIELD("P5600WriteAL2")      1, false, false,  1, 2,  1, 1,  0, 0}, // #1015
}; // MipsP5600ModelSchedClasses

static const llvm::MCSchedModel NoSchedModel = {
  MCSchedModel::DefaultIssueWidth,
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  MCSchedModel::DefaultLoadLatency,
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  0, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  nullptr, // No Itinerary
  nullptr // No extra processor descriptor
};

static const unsigned MipsGenericModelProcResourceSubUnits[] = {
  0,  // Invalid
};

// {Name, NumUnits, SuperIdx, IsBuffered, SubUnitsIdxBegin}
static const llvm::MCProcResourceDesc MipsGenericModelProcResources[] = {
  {"InvalidUnit", 0, 0, 0, 0},
  {"GenericALU",      1, 0, 1, nullptr}, // #1
  {"GenericCOP0",     1, 0, 1, nullptr}, // #2
  {"GenericCOP2",     1, 0, 1, nullptr}, // #3
  {"GenericDSP",      1, 0, 1, nullptr}, // #4
  {"GenericFPQ",      1, 0, 1, nullptr}, // #5
  {"GenericFPUDivSqrt", 1, 5, -1, nullptr}, // #6, Super=GenericFPQ
  {"GenericIssueALU", 1, 1, -1, nullptr}, // #7, Super=GenericALU
  {"GenericIssueCOP0", 1, 2, -1, nullptr}, // #8, Super=GenericCOP0
  {"GenericIssueCTISTD", 1, 1, -1, nullptr}, // #9, Super=GenericALU
  {"GenericIssueDIV", 1, 19, -1, nullptr}, // #10, Super=GenericMDU
  {"GenericIssueFPUL", 1, 5, -1, nullptr}, // #11, Super=GenericFPQ
  {"GenericIssueFPULoad", 1, 5, -1, nullptr}, // #12, Super=GenericFPQ
  {"GenericIssueFPUMove", 1, 5, -1, nullptr}, // #13, Super=GenericFPQ
  {"GenericIssueFPUS", 1, 5, -1, nullptr}, // #14, Super=GenericFPQ
  {"GenericIssueFPUStore", 1, 5, -1, nullptr}, // #15, Super=GenericFPQ
  {"GenericIssueLDST", 1, 18, -1, nullptr}, // #16, Super=GenericLDST
  {"GenericIssueMDU", 1, 1, -1, nullptr}, // #17, Super=GenericALU
  {"GenericLDST",     1, 0, 1, nullptr}, // #18
  {"GenericMDU",      1, 0, 1, nullptr}, // #19
};

static const llvm::MCSchedModel MipsGenericModel = {
  1, // IssueWidth
  0, // MicroOpBufferSize
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  2, // LoadLatency
  37, // HighLatency
  4, // MispredictPenalty
  true, // PostRAScheduler
  false, // CompleteModel
  1, // Processor ID
  MipsGenericModelProcResources,
  MipsGenericModelSchedClasses,
  20,
  1016,
  nullptr, // No Itinerary
  nullptr // No extra processor descriptor
};

static const unsigned MipsP5600ModelProcResourceSubUnits[] = {
  0,  // Invalid
};

// {Name, NumUnits, SuperIdx, IsBuffered, SubUnitsIdxBegin}
static const llvm::MCProcResourceDesc MipsP5600ModelProcResources[] = {
  {"InvalidUnit", 0, 0, 0, 0},
  {"P5600AGQ",        3, 0, 16, nullptr}, // #1
  {"P5600AL2Div",     1, 0, -1, nullptr}, // #2
  {"P5600ALQ",        1, 0, 16, nullptr}, // #3
  {"P5600CTISTD",     1, 0, -1, nullptr}, // #4
  {"P5600FPQ",        3, 0, 16, nullptr}, // #5
  {"P5600FPUDivSqrt", 2, 0, -1, nullptr}, // #6
  {"P5600IssueAL2",   1, 1, -1, nullptr}, // #7, Super=P5600AGQ
  {"P5600IssueALU",   1, 3, -1, nullptr}, // #8, Super=P5600ALQ
  {"P5600IssueCTISTD", 1, 1, -1, nullptr}, // #9, Super=P5600AGQ
  {"P5600IssueFPUL",  1, 5, -1, nullptr}, // #10, Super=P5600FPQ
  {"P5600IssueFPULoad", 1, 5, -1, nullptr}, // #11, Super=P5600FPQ
  {"P5600IssueFPUS",  1, 5, -1, nullptr}, // #12, Super=P5600FPQ
  {"P5600IssueLDST",  1, 1, -1, nullptr}, // #13, Super=P5600AGQ
};

static const llvm::MCSchedModel MipsP5600Model = {
  2, // IssueWidth
  48, // MicroOpBufferSize
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  4, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  8, // MispredictPenalty
  false, // PostRAScheduler
  false, // CompleteModel
  2, // Processor ID
  MipsP5600ModelProcResources,
  MipsP5600ModelSchedClasses,
  14,
  1016,
  nullptr, // No Itinerary
  nullptr // No extra processor descriptor
};

// Sorted (by key) array of itineraries for CPU subtype.
extern const llvm::SubtargetInfoKV MipsProcSchedKV[] = {
  { "mips1", (const void *)&MipsGenericModel },
  { "mips2", (const void *)&MipsGenericModel },
  { "mips3", (const void *)&MipsGenericModel },
  { "mips32", (const void *)&MipsGenericModel },
  { "mips32r2", (const void *)&MipsGenericModel },
  { "mips32r3", (const void *)&MipsGenericModel },
  { "mips32r5", (const void *)&MipsGenericModel },
  { "mips32r6", (const void *)&MipsGenericModel },
  { "mips4", (const void *)&MipsGenericModel },
  { "mips5", (const void *)&MipsGenericModel },
  { "mips64", (const void *)&MipsGenericModel },
  { "mips64r2", (const void *)&MipsGenericModel },
  { "mips64r3", (const void *)&MipsGenericModel },
  { "mips64r5", (const void *)&MipsGenericModel },
  { "mips64r6", (const void *)&MipsGenericModel },
  { "octeon", (const void *)&MipsGenericModel },
  { "p5600", (const void *)&MipsP5600Model },
};

#undef DBGFIELD
namespace Mips_MC {
unsigned resolveVariantSchedClassImpl(unsigned SchedClass,
    const MCInst *MI, unsigned CPUID) {
  // Don't know how to resolve this scheduling class.
  return 0;
}
} // end of namespace Mips_MC

struct MipsGenMCSubtargetInfo : public MCSubtargetInfo {
  MipsGenMCSubtargetInfo(const Triple &TT, 
    StringRef CPU, StringRef FS, ArrayRef<SubtargetFeatureKV> PF,
    ArrayRef<SubtargetFeatureKV> PD,
    const SubtargetInfoKV *ProcSched,
    const MCWriteProcResEntry *WPR,
    const MCWriteLatencyEntry *WL,
    const MCReadAdvanceEntry *RA, const InstrStage *IS,
    const unsigned *OC, const unsigned *FP) :
      MCSubtargetInfo(TT, CPU, FS, PF, PD, ProcSched,
                      WPR, WL, RA, IS, OC, FP) { }

  unsigned resolveVariantSchedClass(unsigned SchedClass,
      const MCInst *MI, unsigned CPUID) const override {
    return Mips_MC::resolveVariantSchedClassImpl(SchedClass, MI, CPUID); 
  }
};

static inline MCSubtargetInfo *createMipsMCSubtargetInfoImpl(const Triple &TT, StringRef CPU, StringRef FS) {
  return new MipsGenMCSubtargetInfo(TT, CPU, FS, MipsFeatureKV, MipsSubTypeKV, 
                      MipsProcSchedKV, MipsWriteProcResTable, MipsWriteLatencyTable, MipsReadAdvanceTable, 
                      nullptr, nullptr, nullptr);
}

} // end namespace llvm

#endif // GET_SUBTARGETINFO_MC_DESC


#ifdef GET_SUBTARGETINFO_TARGET_DESC
#undef GET_SUBTARGETINFO_TARGET_DESC

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

// ParseSubtargetFeatures - Parses features string setting specified
// subtarget options.
void llvm::MipsSubtarget::ParseSubtargetFeatures(StringRef CPU, StringRef FS) {
  LLVM_DEBUG(dbgs() << "\nFeatures:" << FS);
  LLVM_DEBUG(dbgs() << "\nCPU:" << CPU << "\n\n");
  InitMCProcessorInfo(CPU, FS);
  const FeatureBitset& Bits = getFeatureBits();
  if (Bits[Mips::FeatureCRC]) HasCRC = true;
  if (Bits[Mips::FeatureCnMips]) HasCnMips = true;
  if (Bits[Mips::FeatureDSP]) HasDSP = true;
  if (Bits[Mips::FeatureDSPR2]) HasDSPR2 = true;
  if (Bits[Mips::FeatureDSPR3]) HasDSPR3 = true;
  if (Bits[Mips::FeatureEVA]) HasEVA = true;
  if (Bits[Mips::FeatureFP64Bit]) IsFP64bit = true;
  if (Bits[Mips::FeatureFPXX]) IsFPXX = true;
  if (Bits[Mips::FeatureGINV]) HasGINV = true;
  if (Bits[Mips::FeatureGP64Bit]) IsGP64bit = true;
  if (Bits[Mips::FeatureLongCalls]) UseLongCalls = true;
  if (Bits[Mips::FeatureMSA]) HasMSA = true;
  if (Bits[Mips::FeatureMT]) HasMT = true;
  if (Bits[Mips::FeatureMadd4]) DisableMadd4 = true;
  if (Bits[Mips::FeatureMicroMips]) InMicroMipsMode = true;
  if (Bits[Mips::FeatureMips1] && MipsArchVersion < Mips1) MipsArchVersion = Mips1;
  if (Bits[Mips::FeatureMips2] && MipsArchVersion < Mips2) MipsArchVersion = Mips2;
  if (Bits[Mips::FeatureMips3] && MipsArchVersion < Mips3) MipsArchVersion = Mips3;
  if (Bits[Mips::FeatureMips3_32]) HasMips3_32 = true;
  if (Bits[Mips::FeatureMips3_32r2]) HasMips3_32r2 = true;
  if (Bits[Mips::FeatureMips4] && MipsArchVersion < Mips4) MipsArchVersion = Mips4;
  if (Bits[Mips::FeatureMips4_32]) HasMips4_32 = true;
  if (Bits[Mips::FeatureMips4_32r2]) HasMips4_32r2 = true;
  if (Bits[Mips::FeatureMips5] && MipsArchVersion < Mips5) MipsArchVersion = Mips5;
  if (Bits[Mips::FeatureMips5_32r2]) HasMips5_32r2 = true;
  if (Bits[Mips::FeatureMips16]) InMips16Mode = true;
  if (Bits[Mips::FeatureMips32] && MipsArchVersion < Mips32) MipsArchVersion = Mips32;
  if (Bits[Mips::FeatureMips32r2] && MipsArchVersion < Mips32r2) MipsArchVersion = Mips32r2;
  if (Bits[Mips::FeatureMips32r3] && MipsArchVersion < Mips32r3) MipsArchVersion = Mips32r3;
  if (Bits[Mips::FeatureMips32r5] && MipsArchVersion < Mips32r5) MipsArchVersion = Mips32r5;
  if (Bits[Mips::FeatureMips32r6] && MipsArchVersion < Mips32r6) MipsArchVersion = Mips32r6;
  if (Bits[Mips::FeatureMips64] && MipsArchVersion < Mips64) MipsArchVersion = Mips64;
  if (Bits[Mips::FeatureMips64r2] && MipsArchVersion < Mips64r2) MipsArchVersion = Mips64r2;
  if (Bits[Mips::FeatureMips64r3] && MipsArchVersion < Mips64r3) MipsArchVersion = Mips64r3;
  if (Bits[Mips::FeatureMips64r5] && MipsArchVersion < Mips64r5) MipsArchVersion = Mips64r5;
  if (Bits[Mips::FeatureMips64r6] && MipsArchVersion < Mips64r6) MipsArchVersion = Mips64r6;
  if (Bits[Mips::FeatureNaN2008]) IsNaN2008bit = true;
  if (Bits[Mips::FeatureNoABICalls]) NoABICalls = true;
  if (Bits[Mips::FeatureNoOddSPReg]) UseOddSPReg = false;
  if (Bits[Mips::FeaturePTR64Bit]) IsPTR64bit = true;
  if (Bits[Mips::FeatureSingleFloat]) IsSingleFloat = true;
  if (Bits[Mips::FeatureSoftFloat]) IsSoftFloat = true;
  if (Bits[Mips::FeatureSym32]) HasSym32 = true;
  if (Bits[Mips::FeatureUseIndirectJumpsHazard]) UseIndirectJumpsHazard = true;
  if (Bits[Mips::FeatureUseTCCInDIV]) UseTCCInDIV = false;
  if (Bits[Mips::FeatureVFPU]) HasVFPU = true;
  if (Bits[Mips::FeatureVirt]) HasVirt = true;
  if (Bits[Mips::ImplP5600] && ProcImpl < MipsSubtarget::CPU::P5600) ProcImpl = MipsSubtarget::CPU::P5600;
}
#endif // GET_SUBTARGETINFO_TARGET_DESC


#ifdef GET_SUBTARGETINFO_HEADER
#undef GET_SUBTARGETINFO_HEADER

namespace llvm {
class DFAPacketizer;
namespace Mips_MC {
unsigned resolveVariantSchedClassImpl(unsigned SchedClass, const MCInst *MI, unsigned CPUID);
}

struct MipsGenSubtargetInfo : public TargetSubtargetInfo {
  explicit MipsGenSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS);
public:
  unsigned resolveSchedClass(unsigned SchedClass,  const MachineInstr *DefMI, const TargetSchedModel *SchedModel) const override;
  unsigned resolveVariantSchedClass(unsigned SchedClass, const MCInst *MI, unsigned CPUID) const override;
  DFAPacketizer *createDFAPacketizer(const InstrItineraryData *IID) const;
};
} // end namespace llvm

#endif // GET_SUBTARGETINFO_HEADER


#ifdef GET_SUBTARGETINFO_CTOR
#undef GET_SUBTARGETINFO_CTOR

#include "llvm/CodeGen/TargetSchedule.h"

namespace llvm {
extern const llvm::SubtargetFeatureKV MipsFeatureKV[];
extern const llvm::SubtargetFeatureKV MipsSubTypeKV[];
extern const llvm::SubtargetInfoKV MipsProcSchedKV[];
extern const llvm::MCWriteProcResEntry MipsWriteProcResTable[];
extern const llvm::MCWriteLatencyEntry MipsWriteLatencyTable[];
extern const llvm::MCReadAdvanceEntry MipsReadAdvanceTable[];
MipsGenSubtargetInfo::MipsGenSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS)
  : TargetSubtargetInfo(TT, CPU, FS, makeArrayRef(MipsFeatureKV, 48), makeArrayRef(MipsSubTypeKV, 17), 
                        MipsProcSchedKV, MipsWriteProcResTable, MipsWriteLatencyTable, MipsReadAdvanceTable, 
                        nullptr, nullptr, nullptr) {}

unsigned MipsGenSubtargetInfo
::resolveSchedClass(unsigned SchedClass, const MachineInstr *MI, const TargetSchedModel *SchedModel) const {
  switch (SchedClass) {
  case 477: // ADD
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 478: // ADDi
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 479: // ADDiu
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 480: // ANDi
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 481: // ORi
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 482: // ROTR
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 483: // SEB
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 484: // SEH
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 485: // SLT_SLTu
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 486: // SLL
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 487: // SRA
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 488: // SRL
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 489: // XORi
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 490: // ADDu
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 491: // SLLV
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 492: // SRAV
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 493: // SRLV
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 494: // LSA
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 495: // COPY
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  case 690: // ROTRV
    if (SchedModel->getProcessorID() == 2) { // MipsP5600Model
      if ((1))
        return 1014; // P5600WriteALU
      if ((0))
        return 1015; // P5600WriteAL2
    }
    break;
  };
  report_fatal_error("Expected a variant SchedClass");
} // MipsGenSubtargetInfo::resolveSchedClass

unsigned MipsGenSubtargetInfo
::resolveVariantSchedClass(unsigned SchedClass, const MCInst *MI, unsigned CPUID) const {
  return Mips_MC::resolveVariantSchedClassImpl(SchedClass, MI, CPUID);
} // MipsGenSubtargetInfo::resolveVariantSchedClass
} // end namespace llvm

#endif // GET_SUBTARGETINFO_CTOR

