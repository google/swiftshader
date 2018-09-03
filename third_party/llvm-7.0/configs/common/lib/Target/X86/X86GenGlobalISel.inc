/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Global Instruction Selector for the X86 target                             *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_GLOBALISEL_PREDICATE_BITSET
const unsigned MAX_SUBTARGET_PREDICATES = 114;
using PredicateBitset = llvm::PredicateBitsetImpl<MAX_SUBTARGET_PREDICATES>;
#endif // ifdef GET_GLOBALISEL_PREDICATE_BITSET

#ifdef GET_GLOBALISEL_TEMPORARIES_DECL
  mutable MatcherState State;
  typedef ComplexRendererFns(X86InstructionSelector::*ComplexMatcherMemFn)(MachineOperand &) const;
  typedef void(X86InstructionSelector::*CustomRendererFn)(MachineInstrBuilder &, const MachineInstr&) const;
  const ISelInfoTy<PredicateBitset, ComplexMatcherMemFn, CustomRendererFn> ISelInfo;
  static X86InstructionSelector::ComplexMatcherMemFn ComplexPredicateFns[];
  static X86InstructionSelector::CustomRendererFn CustomRenderers[];
  bool testImmPredicate_I64(unsigned PredicateID, int64_t Imm) const override;
  bool testImmPredicate_APInt(unsigned PredicateID, const APInt &Imm) const override;
  bool testImmPredicate_APFloat(unsigned PredicateID, const APFloat &Imm) const override;
  const int64_t *getMatchTable() const override;
  bool testMIPredicate_MI(unsigned PredicateID, const MachineInstr &MI) const override;
#endif // ifdef GET_GLOBALISEL_TEMPORARIES_DECL

#ifdef GET_GLOBALISEL_TEMPORARIES_INIT
, State(0),
ISelInfo(TypeObjects, NumTypeObjects, FeatureBitsets, ComplexPredicateFns, CustomRenderers)
#endif // ifdef GET_GLOBALISEL_TEMPORARIES_INIT

#ifdef GET_GLOBALISEL_IMPL
// Bits for subtarget features that participate in instruction matching.
enum SubtargetFeatureBits : uint8_t {
  Feature_TruePredicateBit = 49,
  Feature_HasCMovBit = 21,
  Feature_NoCMovBit = 101,
  Feature_HasMMXBit = 82,
  Feature_Has3DNowBit = 84,
  Feature_HasSSE1Bit = 35,
  Feature_UseSSE1Bit = 43,
  Feature_HasSSE2Bit = 36,
  Feature_UseSSE2Bit = 44,
  Feature_HasSSE3Bit = 27,
  Feature_UseSSE3Bit = 52,
  Feature_HasSSSE3Bit = 83,
  Feature_UseSSSE3Bit = 53,
  Feature_UseSSE41Bit = 50,
  Feature_HasSSE42Bit = 57,
  Feature_UseSSE42Bit = 56,
  Feature_HasSSE4ABit = 66,
  Feature_NoAVXBit = 62,
  Feature_HasAVXBit = 45,
  Feature_HasAVX2Bit = 39,
  Feature_HasAVX1OnlyBit = 37,
  Feature_HasAVX512Bit = 70,
  Feature_UseAVXBit = 41,
  Feature_UseAVX2Bit = 68,
  Feature_NoAVX512Bit = 32,
  Feature_HasCDIBit = 74,
  Feature_HasVPOPCNTDQBit = 78,
  Feature_HasERIBit = 77,
  Feature_HasDQIBit = 72,
  Feature_NoDQIBit = 54,
  Feature_HasBWIBit = 73,
  Feature_NoBWIBit = 51,
  Feature_HasVLXBit = 71,
  Feature_NoVLXBit = 31,
  Feature_NoVLX_Or_NoBWIBit = 48,
  Feature_NoVLX_Or_NoDQIBit = 105,
  Feature_HasVNNIBit = 80,
  Feature_HasBITALGBit = 81,
  Feature_HasPOPCNTBit = 55,
  Feature_HasAESBit = 59,
  Feature_HasVAESBit = 61,
  Feature_NoVLX_Or_NoVAESBit = 60,
  Feature_HasFXSRBit = 28,
  Feature_HasXSAVEBit = 90,
  Feature_HasXSAVEOPTBit = 91,
  Feature_HasXSAVECBit = 92,
  Feature_HasXSAVESBit = 93,
  Feature_HasPCLMULBit = 63,
  Feature_NoVLX_Or_NoVPCLMULQDQBit = 64,
  Feature_HasVPCLMULQDQBit = 65,
  Feature_HasGFNIBit = 69,
  Feature_HasFMABit = 29,
  Feature_HasFMA4Bit = 33,
  Feature_NoFMA4Bit = 30,
  Feature_HasXOPBit = 34,
  Feature_HasTBMBit = 9,
  Feature_NoTBMBit = 112,
  Feature_HasLWPBit = 10,
  Feature_HasMOVBEBit = 3,
  Feature_HasRDRANDBit = 4,
  Feature_HasF16CBit = 67,
  Feature_HasFSGSBaseBit = 94,
  Feature_HasLZCNTBit = 6,
  Feature_HasBMIBit = 7,
  Feature_HasBMI2Bit = 8,
  Feature_NoBMI2Bit = 113,
  Feature_HasVBMIBit = 75,
  Feature_HasVBMI2Bit = 79,
  Feature_HasIFMABit = 76,
  Feature_HasRTMBit = 88,
  Feature_HasADXBit = 20,
  Feature_HasSHABit = 58,
  Feature_HasRDSEEDBit = 5,
  Feature_HasSSEPrefetchBit = 46,
  Feature_NoSSEPrefetchBit = 85,
  Feature_HasPrefetchWBit = 86,
  Feature_HasPREFETCHWT1Bit = 87,
  Feature_HasLAHFSAHFBit = 2,
  Feature_HasMWAITXBit = 11,
  Feature_HasCLZEROBit = 15,
  Feature_HasCLDEMOTEBit = 18,
  Feature_HasMOVDIRIBit = 13,
  Feature_HasMOVDIR64BBit = 14,
  Feature_HasPTWRITEBit = 97,
  Feature_FPStackf32Bit = 25,
  Feature_FPStackf64Bit = 26,
  Feature_HasCLFLUSHOPTBit = 16,
  Feature_HasCLWBBit = 17,
  Feature_HasWBNOINVDBit = 89,
  Feature_HasRDPIDBit = 96,
  Feature_HasWAITPKGBit = 12,
  Feature_HasINVPCIDBit = 95,
  Feature_HasCmpxchg16bBit = 102,
  Feature_Not64BitModeBit = 0,
  Feature_In64BitModeBit = 1,
  Feature_IsLP64Bit = 99,
  Feature_NotLP64Bit = 98,
  Feature_NotWin64WithoutFPBit = 100,
  Feature_IsPS4Bit = 107,
  Feature_NotPS4Bit = 106,
  Feature_KernelCodeBit = 108,
  Feature_NearDataBit = 110,
  Feature_IsNotPICBit = 109,
  Feature_OptForSizeBit = 40,
  Feature_OptForMinSizeBit = 38,
  Feature_OptForSpeedBit = 104,
  Feature_UseIncDecBit = 19,
  Feature_NoSSE41_Or_OptForSizeBit = 42,
  Feature_CallImmAddrBit = 111,
  Feature_FavorMemIndirectCallBit = 22,
  Feature_HasFastSHLDRotateBit = 103,
  Feature_HasMFenceBit = 47,
  Feature_UseRetpolineBit = 24,
  Feature_NotUseRetpolineBit = 23,
};

PredicateBitset X86InstructionSelector::
computeAvailableModuleFeatures(const X86Subtarget *Subtarget) const {
  PredicateBitset Features;
  if (true)
    Features[Feature_TruePredicateBit] = 1;
  if (Subtarget->hasCMov())
    Features[Feature_HasCMovBit] = 1;
  if (!Subtarget->hasCMov())
    Features[Feature_NoCMovBit] = 1;
  if (Subtarget->hasMMX())
    Features[Feature_HasMMXBit] = 1;
  if (Subtarget->has3DNow())
    Features[Feature_Has3DNowBit] = 1;
  if (Subtarget->hasSSE1())
    Features[Feature_HasSSE1Bit] = 1;
  if (Subtarget->hasSSE1() && !Subtarget->hasAVX())
    Features[Feature_UseSSE1Bit] = 1;
  if (Subtarget->hasSSE2())
    Features[Feature_HasSSE2Bit] = 1;
  if (Subtarget->hasSSE2() && !Subtarget->hasAVX())
    Features[Feature_UseSSE2Bit] = 1;
  if (Subtarget->hasSSE3())
    Features[Feature_HasSSE3Bit] = 1;
  if (Subtarget->hasSSE3() && !Subtarget->hasAVX())
    Features[Feature_UseSSE3Bit] = 1;
  if (Subtarget->hasSSSE3())
    Features[Feature_HasSSSE3Bit] = 1;
  if (Subtarget->hasSSSE3() && !Subtarget->hasAVX())
    Features[Feature_UseSSSE3Bit] = 1;
  if (Subtarget->hasSSE41() && !Subtarget->hasAVX())
    Features[Feature_UseSSE41Bit] = 1;
  if (Subtarget->hasSSE42())
    Features[Feature_HasSSE42Bit] = 1;
  if (Subtarget->hasSSE42() && !Subtarget->hasAVX())
    Features[Feature_UseSSE42Bit] = 1;
  if (Subtarget->hasSSE4A())
    Features[Feature_HasSSE4ABit] = 1;
  if (!Subtarget->hasAVX())
    Features[Feature_NoAVXBit] = 1;
  if (Subtarget->hasAVX())
    Features[Feature_HasAVXBit] = 1;
  if (Subtarget->hasAVX2())
    Features[Feature_HasAVX2Bit] = 1;
  if (Subtarget->hasAVX() && !Subtarget->hasAVX2())
    Features[Feature_HasAVX1OnlyBit] = 1;
  if (Subtarget->hasAVX512())
    Features[Feature_HasAVX512Bit] = 1;
  if (Subtarget->hasAVX() && !Subtarget->hasAVX512())
    Features[Feature_UseAVXBit] = 1;
  if (Subtarget->hasAVX2() && !Subtarget->hasAVX512())
    Features[Feature_UseAVX2Bit] = 1;
  if (!Subtarget->hasAVX512())
    Features[Feature_NoAVX512Bit] = 1;
  if (Subtarget->hasCDI())
    Features[Feature_HasCDIBit] = 1;
  if (Subtarget->hasVPOPCNTDQ())
    Features[Feature_HasVPOPCNTDQBit] = 1;
  if (Subtarget->hasERI())
    Features[Feature_HasERIBit] = 1;
  if (Subtarget->hasDQI())
    Features[Feature_HasDQIBit] = 1;
  if (!Subtarget->hasDQI())
    Features[Feature_NoDQIBit] = 1;
  if (Subtarget->hasBWI())
    Features[Feature_HasBWIBit] = 1;
  if (!Subtarget->hasBWI())
    Features[Feature_NoBWIBit] = 1;
  if (Subtarget->hasVLX())
    Features[Feature_HasVLXBit] = 1;
  if (!Subtarget->hasVLX())
    Features[Feature_NoVLXBit] = 1;
  if (!Subtarget->hasVLX() || !Subtarget->hasBWI())
    Features[Feature_NoVLX_Or_NoBWIBit] = 1;
  if (!Subtarget->hasVLX() || !Subtarget->hasDQI())
    Features[Feature_NoVLX_Or_NoDQIBit] = 1;
  if (Subtarget->hasVNNI())
    Features[Feature_HasVNNIBit] = 1;
  if (Subtarget->hasBITALG())
    Features[Feature_HasBITALGBit] = 1;
  if (Subtarget->hasPOPCNT())
    Features[Feature_HasPOPCNTBit] = 1;
  if (Subtarget->hasAES())
    Features[Feature_HasAESBit] = 1;
  if (Subtarget->hasVAES())
    Features[Feature_HasVAESBit] = 1;
  if (!Subtarget->hasVLX() || !Subtarget->hasVAES())
    Features[Feature_NoVLX_Or_NoVAESBit] = 1;
  if (Subtarget->hasFXSR())
    Features[Feature_HasFXSRBit] = 1;
  if (Subtarget->hasXSAVE())
    Features[Feature_HasXSAVEBit] = 1;
  if (Subtarget->hasXSAVEOPT())
    Features[Feature_HasXSAVEOPTBit] = 1;
  if (Subtarget->hasXSAVEC())
    Features[Feature_HasXSAVECBit] = 1;
  if (Subtarget->hasXSAVES())
    Features[Feature_HasXSAVESBit] = 1;
  if (Subtarget->hasPCLMUL())
    Features[Feature_HasPCLMULBit] = 1;
  if (!Subtarget->hasVLX() || !Subtarget->hasVPCLMULQDQ())
    Features[Feature_NoVLX_Or_NoVPCLMULQDQBit] = 1;
  if (Subtarget->hasVPCLMULQDQ())
    Features[Feature_HasVPCLMULQDQBit] = 1;
  if (Subtarget->hasGFNI())
    Features[Feature_HasGFNIBit] = 1;
  if (Subtarget->hasFMA())
    Features[Feature_HasFMABit] = 1;
  if (Subtarget->hasFMA4())
    Features[Feature_HasFMA4Bit] = 1;
  if (!Subtarget->hasFMA4())
    Features[Feature_NoFMA4Bit] = 1;
  if (Subtarget->hasXOP())
    Features[Feature_HasXOPBit] = 1;
  if (Subtarget->hasTBM())
    Features[Feature_HasTBMBit] = 1;
  if (!Subtarget->hasTBM())
    Features[Feature_NoTBMBit] = 1;
  if (Subtarget->hasLWP())
    Features[Feature_HasLWPBit] = 1;
  if (Subtarget->hasMOVBE())
    Features[Feature_HasMOVBEBit] = 1;
  if (Subtarget->hasRDRAND())
    Features[Feature_HasRDRANDBit] = 1;
  if (Subtarget->hasF16C())
    Features[Feature_HasF16CBit] = 1;
  if (Subtarget->hasFSGSBase())
    Features[Feature_HasFSGSBaseBit] = 1;
  if (Subtarget->hasLZCNT())
    Features[Feature_HasLZCNTBit] = 1;
  if (Subtarget->hasBMI())
    Features[Feature_HasBMIBit] = 1;
  if (Subtarget->hasBMI2())
    Features[Feature_HasBMI2Bit] = 1;
  if (!Subtarget->hasBMI2())
    Features[Feature_NoBMI2Bit] = 1;
  if (Subtarget->hasVBMI())
    Features[Feature_HasVBMIBit] = 1;
  if (Subtarget->hasVBMI2())
    Features[Feature_HasVBMI2Bit] = 1;
  if (Subtarget->hasIFMA())
    Features[Feature_HasIFMABit] = 1;
  if (Subtarget->hasRTM())
    Features[Feature_HasRTMBit] = 1;
  if (Subtarget->hasADX())
    Features[Feature_HasADXBit] = 1;
  if (Subtarget->hasSHA())
    Features[Feature_HasSHABit] = 1;
  if (Subtarget->hasRDSEED())
    Features[Feature_HasRDSEEDBit] = 1;
  if (Subtarget->hasSSEPrefetch())
    Features[Feature_HasSSEPrefetchBit] = 1;
  if (!Subtarget->hasSSEPrefetch())
    Features[Feature_NoSSEPrefetchBit] = 1;
  if (Subtarget->hasPRFCHW())
    Features[Feature_HasPrefetchWBit] = 1;
  if (Subtarget->hasPREFETCHWT1())
    Features[Feature_HasPREFETCHWT1Bit] = 1;
  if (Subtarget->hasLAHFSAHF())
    Features[Feature_HasLAHFSAHFBit] = 1;
  if (Subtarget->hasMWAITX())
    Features[Feature_HasMWAITXBit] = 1;
  if (Subtarget->hasCLZERO())
    Features[Feature_HasCLZEROBit] = 1;
  if (Subtarget->hasCLDEMOTE())
    Features[Feature_HasCLDEMOTEBit] = 1;
  if (Subtarget->hasMOVDIRI())
    Features[Feature_HasMOVDIRIBit] = 1;
  if (Subtarget->hasMOVDIR64B())
    Features[Feature_HasMOVDIR64BBit] = 1;
  if (Subtarget->hasPTWRITE())
    Features[Feature_HasPTWRITEBit] = 1;
  if (!Subtarget->hasSSE1())
    Features[Feature_FPStackf32Bit] = 1;
  if (!Subtarget->hasSSE2())
    Features[Feature_FPStackf64Bit] = 1;
  if (Subtarget->hasCLFLUSHOPT())
    Features[Feature_HasCLFLUSHOPTBit] = 1;
  if (Subtarget->hasCLWB())
    Features[Feature_HasCLWBBit] = 1;
  if (Subtarget->hasWBNOINVD())
    Features[Feature_HasWBNOINVDBit] = 1;
  if (Subtarget->hasRDPID())
    Features[Feature_HasRDPIDBit] = 1;
  if (Subtarget->hasWAITPKG())
    Features[Feature_HasWAITPKGBit] = 1;
  if (Subtarget->hasINVPCID())
    Features[Feature_HasINVPCIDBit] = 1;
  if (Subtarget->hasCmpxchg16b())
    Features[Feature_HasCmpxchg16bBit] = 1;
  if (!Subtarget->is64Bit())
    Features[Feature_Not64BitModeBit] = 1;
  if (Subtarget->is64Bit())
    Features[Feature_In64BitModeBit] = 1;
  if (Subtarget->isTarget64BitLP64())
    Features[Feature_IsLP64Bit] = 1;
  if (!Subtarget->isTarget64BitLP64())
    Features[Feature_NotLP64Bit] = 1;
  if (Subtarget->isTargetPS4())
    Features[Feature_IsPS4Bit] = 1;
  if (!Subtarget->isTargetPS4())
    Features[Feature_NotPS4Bit] = 1;
  if (TM.getCodeModel() == CodeModel::Kernel)
    Features[Feature_KernelCodeBit] = 1;
  if (TM.getCodeModel() == CodeModel::Small ||TM.getCodeModel() == CodeModel::Kernel)
    Features[Feature_NearDataBit] = 1;
  if (!TM.isPositionIndependent())
    Features[Feature_IsNotPICBit] = 1;
  if (Subtarget->isLegalToCallImmediateAddr())
    Features[Feature_CallImmAddrBit] = 1;
  if (!Subtarget->slowTwoMemOps())
    Features[Feature_FavorMemIndirectCallBit] = 1;
  if (Subtarget->hasFastSHLDRotate())
    Features[Feature_HasFastSHLDRotateBit] = 1;
  if (Subtarget->hasMFence())
    Features[Feature_HasMFenceBit] = 1;
  if (Subtarget->useRetpoline())
    Features[Feature_UseRetpolineBit] = 1;
  if (!Subtarget->useRetpoline())
    Features[Feature_NotUseRetpolineBit] = 1;
  return Features;
}

PredicateBitset X86InstructionSelector::
computeAvailableFunctionFeatures(const X86Subtarget *Subtarget, const MachineFunction *MF) const {
  PredicateBitset Features;
  if (!Subtarget->isTargetWin64() ||Subtarget->getFrameLowering()->hasFP(*MF))
    Features[Feature_NotWin64WithoutFPBit] = 1;
  if (MF->getFunction().optForSize())
    Features[Feature_OptForSizeBit] = 1;
  if (MF->getFunction().optForMinSize())
    Features[Feature_OptForMinSizeBit] = 1;
  if (!MF->getFunction().optForSize())
    Features[Feature_OptForSpeedBit] = 1;
  if (!Subtarget->slowIncDec() || MF->getFunction().optForSize())
    Features[Feature_UseIncDecBit] = 1;
  if (MF->getFunction().optForSize() || !Subtarget->hasSSE41())
    Features[Feature_NoSSE41_Or_OptForSizeBit] = 1;
  return Features;
}

// LLT Objects.
enum {
  GILLT_s1,
  GILLT_s8,
  GILLT_s16,
  GILLT_s32,
  GILLT_s64,
  GILLT_s80,
  GILLT_s128,
  GILLT_v2s1,
  GILLT_v2s64,
  GILLT_v4s1,
  GILLT_v4s32,
  GILLT_v4s64,
  GILLT_v8s1,
  GILLT_v8s16,
  GILLT_v8s32,
  GILLT_v8s64,
  GILLT_v16s1,
  GILLT_v16s8,
  GILLT_v16s16,
  GILLT_v16s32,
  GILLT_v32s1,
  GILLT_v32s8,
  GILLT_v32s16,
  GILLT_v64s1,
  GILLT_v64s8,
};
const static size_t NumTypeObjects = 25;
const static LLT TypeObjects[] = {
  LLT::scalar(1),
  LLT::scalar(8),
  LLT::scalar(16),
  LLT::scalar(32),
  LLT::scalar(64),
  LLT::scalar(80),
  LLT::scalar(128),
  LLT::vector(2, 1),
  LLT::vector(2, 64),
  LLT::vector(4, 1),
  LLT::vector(4, 32),
  LLT::vector(4, 64),
  LLT::vector(8, 1),
  LLT::vector(8, 16),
  LLT::vector(8, 32),
  LLT::vector(8, 64),
  LLT::vector(16, 1),
  LLT::vector(16, 8),
  LLT::vector(16, 16),
  LLT::vector(16, 32),
  LLT::vector(32, 1),
  LLT::vector(32, 8),
  LLT::vector(32, 16),
  LLT::vector(64, 1),
  LLT::vector(64, 8),
};

// Feature bitsets.
enum {
  GIFBS_Invalid,
  GIFBS_FPStackf32,
  GIFBS_FPStackf64,
  GIFBS_Has3DNow,
  GIFBS_HasAVX,
  GIFBS_HasAVX1Only,
  GIFBS_HasAVX2,
  GIFBS_HasAVX512,
  GIFBS_HasBMI,
  GIFBS_HasBMI2,
  GIFBS_HasBWI,
  GIFBS_HasDQI,
  GIFBS_HasLWP,
  GIFBS_HasMFence,
  GIFBS_HasMMX,
  GIFBS_HasMOVBE,
  GIFBS_HasPTWRITE,
  GIFBS_HasRTM,
  GIFBS_HasSHA,
  GIFBS_HasSSE1,
  GIFBS_HasSSE2,
  GIFBS_HasSSE42,
  GIFBS_HasSSE4A,
  GIFBS_HasTBM,
  GIFBS_HasVLX,
  GIFBS_HasWAITPKG,
  GIFBS_HasWBNOINVD,
  GIFBS_HasXOP,
  GIFBS_In64BitMode,
  GIFBS_NoDQI,
  GIFBS_Not64BitMode,
  GIFBS_UseAVX,
  GIFBS_UseIncDec,
  GIFBS_UseSSE1,
  GIFBS_UseSSE2,
  GIFBS_UseSSE41,
  GIFBS_UseSSSE3,
  GIFBS_HasAES_HasAVX,
  GIFBS_HasAES_NoAVX,
  GIFBS_HasAVX_NoVLX,
  GIFBS_HasAVX_NoVLX_Or_NoBWI,
  GIFBS_HasAVX2_NoVLX,
  GIFBS_HasAVX2_NoVLX_Or_NoBWI,
  GIFBS_HasAVX512_HasVAES,
  GIFBS_HasAVX512_HasVLX,
  GIFBS_HasAVX512_HasVPCLMULQDQ,
  GIFBS_HasBWI_HasVLX,
  GIFBS_HasDQI_HasVLX,
  GIFBS_HasDQI_NoBWI,
  GIFBS_HasFSGSBase_In64BitMode,
  GIFBS_HasPCLMUL_NoAVX,
  GIFBS_HasPTWRITE_In64BitMode,
  GIFBS_HasRDPID_Not64BitMode,
  GIFBS_HasSSE2_NoAVX512,
  GIFBS_HasVAES_HasVLX,
  GIFBS_HasVAES_NoVLX,
  GIFBS_HasVLX_HasVPCLMULQDQ,
  GIFBS_HasVPCLMULQDQ_NoVLX,
  GIFBS_HasWAITPKG_In64BitMode,
  GIFBS_HasWAITPKG_Not64BitMode,
  GIFBS_Not64BitMode_OptForSize,
  GIFBS_NotWin64WithoutFP_OptForMinSize,
  GIFBS_HasAES_HasAVX_NoVLX_Or_NoVAES,
  GIFBS_HasAVX_HasPCLMUL_NoVLX_Or_NoVPCLMULQDQ,
};
const static PredicateBitset FeatureBitsets[] {
  {}, // GIFBS_Invalid
  {Feature_FPStackf32Bit, },
  {Feature_FPStackf64Bit, },
  {Feature_Has3DNowBit, },
  {Feature_HasAVXBit, },
  {Feature_HasAVX1OnlyBit, },
  {Feature_HasAVX2Bit, },
  {Feature_HasAVX512Bit, },
  {Feature_HasBMIBit, },
  {Feature_HasBMI2Bit, },
  {Feature_HasBWIBit, },
  {Feature_HasDQIBit, },
  {Feature_HasLWPBit, },
  {Feature_HasMFenceBit, },
  {Feature_HasMMXBit, },
  {Feature_HasMOVBEBit, },
  {Feature_HasPTWRITEBit, },
  {Feature_HasRTMBit, },
  {Feature_HasSHABit, },
  {Feature_HasSSE1Bit, },
  {Feature_HasSSE2Bit, },
  {Feature_HasSSE42Bit, },
  {Feature_HasSSE4ABit, },
  {Feature_HasTBMBit, },
  {Feature_HasVLXBit, },
  {Feature_HasWAITPKGBit, },
  {Feature_HasWBNOINVDBit, },
  {Feature_HasXOPBit, },
  {Feature_In64BitModeBit, },
  {Feature_NoDQIBit, },
  {Feature_Not64BitModeBit, },
  {Feature_UseAVXBit, },
  {Feature_UseIncDecBit, },
  {Feature_UseSSE1Bit, },
  {Feature_UseSSE2Bit, },
  {Feature_UseSSE41Bit, },
  {Feature_UseSSSE3Bit, },
  {Feature_HasAESBit, Feature_HasAVXBit, },
  {Feature_HasAESBit, Feature_NoAVXBit, },
  {Feature_HasAVXBit, Feature_NoVLXBit, },
  {Feature_HasAVXBit, Feature_NoVLX_Or_NoBWIBit, },
  {Feature_HasAVX2Bit, Feature_NoVLXBit, },
  {Feature_HasAVX2Bit, Feature_NoVLX_Or_NoBWIBit, },
  {Feature_HasAVX512Bit, Feature_HasVAESBit, },
  {Feature_HasAVX512Bit, Feature_HasVLXBit, },
  {Feature_HasAVX512Bit, Feature_HasVPCLMULQDQBit, },
  {Feature_HasBWIBit, Feature_HasVLXBit, },
  {Feature_HasDQIBit, Feature_HasVLXBit, },
  {Feature_HasDQIBit, Feature_NoBWIBit, },
  {Feature_HasFSGSBaseBit, Feature_In64BitModeBit, },
  {Feature_HasPCLMULBit, Feature_NoAVXBit, },
  {Feature_HasPTWRITEBit, Feature_In64BitModeBit, },
  {Feature_HasRDPIDBit, Feature_Not64BitModeBit, },
  {Feature_HasSSE2Bit, Feature_NoAVX512Bit, },
  {Feature_HasVAESBit, Feature_HasVLXBit, },
  {Feature_HasVAESBit, Feature_NoVLXBit, },
  {Feature_HasVLXBit, Feature_HasVPCLMULQDQBit, },
  {Feature_HasVPCLMULQDQBit, Feature_NoVLXBit, },
  {Feature_HasWAITPKGBit, Feature_In64BitModeBit, },
  {Feature_HasWAITPKGBit, Feature_Not64BitModeBit, },
  {Feature_Not64BitModeBit, Feature_OptForSizeBit, },
  {Feature_NotWin64WithoutFPBit, Feature_OptForMinSizeBit, },
  {Feature_HasAESBit, Feature_HasAVXBit, Feature_NoVLX_Or_NoVAESBit, },
  {Feature_HasAVXBit, Feature_HasPCLMULBit, Feature_NoVLX_Or_NoVPCLMULQDQBit, },
};

// ComplexPattern predicates.
enum {
  GICP_Invalid,
};
// See constructor for table contents

// PatFrag predicates.
enum {
  GIPFP_I64_Predicate_AndMask64 = GIPFP_I64_Invalid + 1,
  GIPFP_I64_Predicate_BTCBTSMask64,
  GIPFP_I64_Predicate_BTRMask64,
  GIPFP_I64_Predicate_PrefetchWT1Level,
  GIPFP_I64_Predicate_i16immSExt8,
  GIPFP_I64_Predicate_i32immSExt8,
  GIPFP_I64_Predicate_i64immSExt32,
  GIPFP_I64_Predicate_i64immSExt8,
  GIPFP_I64_Predicate_i64immZExt32,
  GIPFP_I64_Predicate_i64immZExt32SExt8,
  GIPFP_I64_Predicate_immShift16,
  GIPFP_I64_Predicate_immShift32,
  GIPFP_I64_Predicate_immShift64,
  GIPFP_I64_Predicate_immShift8,
  GIPFP_I64_Predicate_immff00_ffff,
};
bool X86InstructionSelector::testImmPredicate_I64(unsigned PredicateID, int64_t Imm) const {
  switch (PredicateID) {
  case GIPFP_I64_Predicate_AndMask64: {
    
  return isMask_64(Imm) && !isUInt<32>(Imm);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_BTCBTSMask64: {
    
  return !isInt<32>(Imm) && isPowerOf2_64(Imm);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_BTRMask64: {
    
  return !isUInt<32>(Imm) && !isInt<32>(Imm) && isPowerOf2_64(~Imm);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_PrefetchWT1Level: {
    
  return Imm < 3;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i16immSExt8: {
     return isInt<8>(Imm); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i32immSExt8: {
     return isInt<8>(Imm); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i64immSExt32: {
     return isInt<32>(Imm); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i64immSExt8: {
     return isInt<8>(Imm); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i64immZExt32: {
     return isUInt<32>(Imm); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_i64immZExt32SExt8: {
    
  return isUInt<32>(Imm) && isInt<8>(static_cast<int32_t>(Imm));

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immShift16: {
    
  return countTrailingOnes<uint64_t>(Imm) >= 4;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immShift32: {
    
  return countTrailingOnes<uint64_t>(Imm) >= 5;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immShift64: {
    
  return countTrailingOnes<uint64_t>(Imm) >= 6;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immShift8: {
    
  return countTrailingOnes<uint64_t>(Imm) >= 3;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immff00_ffff: {
    
  return Imm >= 0xff00 && Imm <= 0xffff;

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  }
  llvm_unreachable("Unknown predicate");
  return false;
}
// PatFrag predicates.
enum {
  GIPFP_APFloat_Predicate_fpimm0 = GIPFP_APFloat_Invalid + 1,
  GIPFP_APFloat_Predicate_fpimm1,
  GIPFP_APFloat_Predicate_fpimmneg0,
  GIPFP_APFloat_Predicate_fpimmneg1,
};
bool X86InstructionSelector::testImmPredicate_APFloat(unsigned PredicateID, const APFloat & Imm) const {
  switch (PredicateID) {
  case GIPFP_APFloat_Predicate_fpimm0: {
    
  return Imm.isExactlyValue(+0.0);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_APFloat_Predicate_fpimm1: {
    
  return Imm.isExactlyValue(+1.0);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_APFloat_Predicate_fpimmneg0: {
    
  return Imm.isExactlyValue(-0.0);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_APFloat_Predicate_fpimmneg1: {
    
  return Imm.isExactlyValue(-1.0);

    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  }
  llvm_unreachable("Unknown predicate");
  return false;
}
bool X86InstructionSelector::testImmPredicate_APInt(unsigned PredicateID, const APInt & Imm) const {
  llvm_unreachable("Unknown predicate");
  return false;
}
bool X86InstructionSelector::testMIPredicate_MI(unsigned PredicateID, const MachineInstr & MI) const {
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineRegisterInfo &MRI = MF.getRegInfo();
  (void)MRI;
  llvm_unreachable("Unknown predicate");
  return false;
}

X86InstructionSelector::ComplexMatcherMemFn
X86InstructionSelector::ComplexPredicateFns[] = {
  nullptr, // GICP_Invalid
};

// Custom renderers.
enum {
  GICR_Invalid,
};
X86InstructionSelector::CustomRendererFn
X86InstructionSelector::CustomRenderers[] = {
  nullptr, // GICP_Invalid
};

bool X86InstructionSelector::selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const {
  MachineFunction &MF = *I.getParent()->getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  // FIXME: This should be computed on a per-function basis rather than per-insn.
  AvailableFunctionFeatures = computeAvailableFunctionFeatures(&STI, &MF);
  const PredicateBitset AvailableFeatures = getAvailableFeatures();
  NewMIVector OutMIs;
  State.MIs.clear();
  State.MIs.push_back(&I);

  if (executeMatchTable(*this, OutMIs, State, ISelInfo, getMatchTable(), TII, MRI, TRI, RBI, AvailableFeatures, CoverageInfo)) {
    return true;
  }

  return false;
}

const int64_t *X86InstructionSelector::getMatchTable() const {
  constexpr static int64_t MatchTable0[] = {
    GIM_SwitchOpcode, /*MI*/0, /*[*/34, 124, /*)*//*default:*//*Label 31*/ 32754,
    /*TargetOpcode::G_ADD*//*Label 0*/ 95,
    /*TargetOpcode::G_SUB*//*Label 1*/ 1712,
    /*TargetOpcode::G_MUL*//*Label 2*/ 2793, 0, 0, 0, 0,
    /*TargetOpcode::G_AND*//*Label 3*/ 3559,
    /*TargetOpcode::G_OR*//*Label 4*/ 6671,
    /*TargetOpcode::G_XOR*//*Label 5*/ 9342, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_BITCAST*//*Label 6*/ 11315, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_INTRINSIC*//*Label 7*/ 14608,
    /*TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS*//*Label 8*/ 22151,
    /*TargetOpcode::G_ANYEXT*//*Label 9*/ 23582,
    /*TargetOpcode::G_TRUNC*//*Label 10*/ 23698,
    /*TargetOpcode::G_CONSTANT*//*Label 11*/ 24033,
    /*TargetOpcode::G_FCONSTANT*//*Label 12*/ 24236, 0, 0,
    /*TargetOpcode::G_SEXT*//*Label 13*/ 24429,
    /*TargetOpcode::G_ZEXT*//*Label 14*/ 24933,
    /*TargetOpcode::G_SHL*//*Label 15*/ 25068,
    /*TargetOpcode::G_LSHR*//*Label 16*/ 25793,
    /*TargetOpcode::G_ASHR*//*Label 17*/ 26624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_FADD*//*Label 18*/ 27281,
    /*TargetOpcode::G_FSUB*//*Label 19*/ 27894,
    /*TargetOpcode::G_FMUL*//*Label 20*/ 28507, 0,
    /*TargetOpcode::G_FDIV*//*Label 21*/ 29120, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_FNEG*//*Label 22*/ 29733,
    /*TargetOpcode::G_FPEXT*//*Label 23*/ 29822,
    /*TargetOpcode::G_FPTRUNC*//*Label 24*/ 30098,
    /*TargetOpcode::G_FPTOSI*//*Label 25*/ 30383,
    /*TargetOpcode::G_FPTOUI*//*Label 26*/ 31013,
    /*TargetOpcode::G_SITOFP*//*Label 27*/ 31367,
    /*TargetOpcode::G_UITOFP*//*Label 28*/ 32197, 0, 0, 0,
    /*TargetOpcode::G_BR*//*Label 29*/ 32651, 0, 0, 0,
    /*TargetOpcode::G_BSWAP*//*Label 30*/ 32664,
    // Label 0: @95
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 25, /*)*//*default:*//*Label 48*/ 1711,
    /*GILLT_s8*//*Label 32*/ 125,
    /*GILLT_s16*//*Label 33*/ 239,
    /*GILLT_s32*//*Label 34*/ 411,
    /*GILLT_s64*//*Label 35*/ 583, 0, 0, 0,
    /*GILLT_v2s64*//*Label 36*/ 783, 0,
    /*GILLT_v4s32*//*Label 37*/ 864,
    /*GILLT_v4s64*//*Label 38*/ 1067, 0,
    /*GILLT_v8s16*//*Label 39*/ 1125,
    /*GILLT_v8s32*//*Label 40*/ 1328,
    /*GILLT_v8s64*//*Label 41*/ 1386, 0,
    /*GILLT_v16s8*//*Label 42*/ 1418,
    /*GILLT_v16s16*//*Label 43*/ 1499,
    /*GILLT_v16s32*//*Label 44*/ 1557, 0,
    /*GILLT_v32s8*//*Label 45*/ 1589,
    /*GILLT_v32s16*//*Label 46*/ 1647, 0,
    /*GILLT_v64s8*//*Label 47*/ 1679,
    // Label 32: @125
    GIM_Try, /*On fail goto*//*Label 49*/ 238,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 50*/ 167, // Rule ID 16134 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (add:{ *:[i8] } GR8:{ *:[i8] }:$src, 1:{ *:[i8] })  =>  (INC8r:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INC8r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16134,
        GIR_Done,
      // Label 50: @167
      GIM_Try, /*On fail goto*//*Label 51*/ 191, // Rule ID 16138 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (add:{ *:[i8] } GR8:{ *:[i8] }:$src, -1:{ *:[i8] })  =>  (DEC8r:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DEC8r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16138,
        GIR_Done,
      // Label 51: @191
      GIM_Try, /*On fail goto*//*Label 52*/ 221, // Rule ID 16088 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (ADD8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16088,
        GIR_Done,
      // Label 52: @221
      GIM_Try, /*On fail goto*//*Label 53*/ 237, // Rule ID 16080 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR8RegClassID,
        // (add:{ *:[i8] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (ADD8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD8rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16080,
        GIR_Done,
      // Label 53: @237
      GIM_Reject,
    // Label 49: @238
    GIM_Reject,
    // Label 33: @239
    GIM_Try, /*On fail goto*//*Label 54*/ 410,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 55*/ 282, // Rule ID 15946 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 128,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src1, 128:{ *:[i16] })  =>  (SUB16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, -128:{ *:[i16] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_AddImm, /*InsnID*/0, /*Imm*/-128,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15946,
        GIR_Done,
      // Label 55: @282
      GIM_Try, /*On fail goto*//*Label 56*/ 306, // Rule ID 16135 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src, 1:{ *:[i16] })  =>  (INC16r:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INC16r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16135,
        GIR_Done,
      // Label 56: @306
      GIM_Try, /*On fail goto*//*Label 57*/ 330, // Rule ID 16139 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src, -1:{ *:[i16] })  =>  (DEC16r:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DEC16r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16139,
        GIR_Done,
      // Label 57: @330
      GIM_Try, /*On fail goto*//*Label 58*/ 363, // Rule ID 16091 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (ADD16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16091,
        GIR_Done,
      // Label 58: @363
      GIM_Try, /*On fail goto*//*Label 59*/ 393, // Rule ID 16089 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (ADD16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16089,
        GIR_Done,
      // Label 59: @393
      GIM_Try, /*On fail goto*//*Label 60*/ 409, // Rule ID 16081 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (add:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (ADD16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16081,
        GIR_Done,
      // Label 60: @409
      GIM_Reject,
    // Label 54: @410
    GIM_Reject,
    // Label 34: @411
    GIM_Try, /*On fail goto*//*Label 61*/ 582,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 62*/ 454, // Rule ID 15948 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 128,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src1, 128:{ *:[i32] })  =>  (SUB32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, -128:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_AddImm, /*InsnID*/0, /*Imm*/-128,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15948,
        GIR_Done,
      // Label 62: @454
      GIM_Try, /*On fail goto*//*Label 63*/ 478, // Rule ID 16136 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] })  =>  (INC32r:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INC32r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16136,
        GIR_Done,
      // Label 63: @478
      GIM_Try, /*On fail goto*//*Label 64*/ 502, // Rule ID 16140 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] })  =>  (DEC32r:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DEC32r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16140,
        GIR_Done,
      // Label 64: @502
      GIM_Try, /*On fail goto*//*Label 65*/ 535, // Rule ID 16092 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (ADD32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16092,
        GIR_Done,
      // Label 65: @535
      GIM_Try, /*On fail goto*//*Label 66*/ 565, // Rule ID 16090 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (ADD32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16090,
        GIR_Done,
      // Label 66: @565
      GIM_Try, /*On fail goto*//*Label 67*/ 581, // Rule ID 16082 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (add:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (ADD32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16082,
        GIR_Done,
      // Label 67: @581
      GIM_Reject,
    // Label 61: @582
    GIM_Reject,
    // Label 35: @583
    GIM_Try, /*On fail goto*//*Label 68*/ 782,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 69*/ 626, // Rule ID 15950 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 128,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src1, 128:{ *:[i64] })  =>  (SUB64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, -128:{ *:[i64] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_AddImm, /*InsnID*/0, /*Imm*/-128,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15950,
        GIR_Done,
      // Label 69: @626
      GIM_Try, /*On fail goto*//*Label 70*/ 651, // Rule ID 15952 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 2147483648,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src1, 2147483648:{ *:[i64] })  =>  (SUB64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, -2147483648:{ *:[i64] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_AddImm, /*InsnID*/0, /*Imm*/-2147483648,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15952,
        GIR_Done,
      // Label 70: @651
      GIM_Try, /*On fail goto*//*Label 71*/ 675, // Rule ID 16137 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] })  =>  (INC64r:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INC64r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16137,
        GIR_Done,
      // Label 71: @675
      GIM_Try, /*On fail goto*//*Label 72*/ 699, // Rule ID 16141 //
        GIM_CheckFeatures, GIFBS_UseIncDec,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] })  =>  (DEC64r:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DEC64r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16141,
        GIR_Done,
      // Label 72: @699
      GIM_Try, /*On fail goto*//*Label 73*/ 732, // Rule ID 16093 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (ADD64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16093,
        GIR_Done,
      // Label 73: @732
      GIM_Try, /*On fail goto*//*Label 74*/ 765, // Rule ID 16094 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (ADD64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16094,
        GIR_Done,
      // Label 74: @765
      GIM_Try, /*On fail goto*//*Label 75*/ 781, // Rule ID 16083 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (add:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (ADD64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16083,
        GIR_Done,
      // Label 75: @781
      GIM_Reject,
    // Label 68: @782
    GIM_Reject,
    // Label 36: @783
    GIM_Try, /*On fail goto*//*Label 76*/ 863,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 77*/ 816, // Rule ID 1823 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPADDQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1823,
        GIR_Done,
      // Label 77: @816
      GIM_Try, /*On fail goto*//*Label 78*/ 839, // Rule ID 1825 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (PADDQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PADDQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1825,
        GIR_Done,
      // Label 78: @839
      GIM_Try, /*On fail goto*//*Label 79*/ 862, // Rule ID 3837 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (add:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPADDQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3837,
        GIR_Done,
      // Label 79: @862
      GIM_Reject,
    // Label 76: @863
    GIM_Reject,
    // Label 37: @864
    GIM_Try, /*On fail goto*//*Label 80*/ 1066,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 81*/ 935, // Rule ID 12398 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v4i32] } (mul:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2), VR128:{ *:[v4i32] }:$src3)  =>  (VPMACSDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSDDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12398,
        GIR_Done,
      // Label 81: @935
      GIM_Try, /*On fail goto*//*Label 82*/ 996, // Rule ID 18283 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src3, (mul:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2))  =>  (VPMACSDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSDDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18283,
        GIR_Done,
      // Label 82: @996
      GIM_Try, /*On fail goto*//*Label 83*/ 1019, // Rule ID 1817 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPADDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1817,
        GIR_Done,
      // Label 83: @1019
      GIM_Try, /*On fail goto*//*Label 84*/ 1042, // Rule ID 1819 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (PADDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PADDDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1819,
        GIR_Done,
      // Label 84: @1042
      GIM_Try, /*On fail goto*//*Label 85*/ 1065, // Rule ID 3864 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (add:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPADDDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3864,
        GIR_Done,
      // Label 85: @1065
      GIM_Reject,
    // Label 80: @1066
    GIM_Reject,
    // Label 38: @1067
    GIM_Try, /*On fail goto*//*Label 86*/ 1124,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 87*/ 1100, // Rule ID 1827 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (add:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPADDQYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDQYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1827,
        GIR_Done,
      // Label 87: @1100
      GIM_Try, /*On fail goto*//*Label 88*/ 1123, // Rule ID 3828 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (add:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPADDQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3828,
        GIR_Done,
      // Label 88: @1123
      GIM_Reject,
    // Label 86: @1124
    GIM_Reject,
    // Label 39: @1125
    GIM_Try, /*On fail goto*//*Label 89*/ 1327,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_Try, /*On fail goto*//*Label 90*/ 1196, // Rule ID 12397 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v8i16] } (mul:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2), VR128:{ *:[v8i16] }:$src3)  =>  (VPMACSWWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSWWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12397,
        GIR_Done,
      // Label 90: @1196
      GIM_Try, /*On fail goto*//*Label 91*/ 1257, // Rule ID 18282 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src3, (mul:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2))  =>  (VPMACSWWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSWWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18282,
        GIR_Done,
      // Label 91: @1257
      GIM_Try, /*On fail goto*//*Label 92*/ 1280, // Rule ID 1811 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPADDWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1811,
        GIR_Done,
      // Label 92: @1280
      GIM_Try, /*On fail goto*//*Label 93*/ 1303, // Rule ID 1813 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PADDWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PADDWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1813,
        GIR_Done,
      // Label 93: @1303
      GIM_Try, /*On fail goto*//*Label 94*/ 1326, // Rule ID 3885 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (add:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPADDWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDWZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3885,
        GIR_Done,
      // Label 94: @1326
      GIM_Reject,
    // Label 89: @1327
    GIM_Reject,
    // Label 40: @1328
    GIM_Try, /*On fail goto*//*Label 95*/ 1385,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 96*/ 1361, // Rule ID 1821 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (add:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPADDDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1821,
        GIR_Done,
      // Label 96: @1361
      GIM_Try, /*On fail goto*//*Label 97*/ 1384, // Rule ID 3855 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (add:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPADDDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3855,
        GIR_Done,
      // Label 97: @1384
      GIM_Reject,
    // Label 95: @1385
    GIM_Reject,
    // Label 41: @1386
    GIM_Try, /*On fail goto*//*Label 98*/ 1417, // Rule ID 3819 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (add:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPADDQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3819,
      GIR_Done,
    // Label 98: @1417
    GIM_Reject,
    // Label 42: @1418
    GIM_Try, /*On fail goto*//*Label 99*/ 1498,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_Try, /*On fail goto*//*Label 100*/ 1451, // Rule ID 1805 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (VPADDBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1805,
        GIR_Done,
      // Label 100: @1451
      GIM_Try, /*On fail goto*//*Label 101*/ 1474, // Rule ID 1807 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (add:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (PADDBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PADDBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1807,
        GIR_Done,
      // Label 101: @1474
      GIM_Try, /*On fail goto*//*Label 102*/ 1497, // Rule ID 3903 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (add:{ *:[v16i8] } VR128X:{ *:[v16i8] }:$src1, VR128X:{ *:[v16i8] }:$src2)  =>  (VPADDBZ128rr:{ *:[v16i8] } VR128X:{ *:[v16i8] }:$src1, VR128X:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDBZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3903,
        GIR_Done,
      // Label 102: @1497
      GIM_Reject,
    // Label 99: @1498
    GIM_Reject,
    // Label 43: @1499
    GIM_Try, /*On fail goto*//*Label 103*/ 1556,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_Try, /*On fail goto*//*Label 104*/ 1532, // Rule ID 1815 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (add:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPADDWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDWYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1815,
        GIR_Done,
      // Label 104: @1532
      GIM_Try, /*On fail goto*//*Label 105*/ 1555, // Rule ID 3879 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (add:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPADDWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDWZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3879,
        GIR_Done,
      // Label 105: @1555
      GIM_Reject,
    // Label 103: @1556
    GIM_Reject,
    // Label 44: @1557
    GIM_Try, /*On fail goto*//*Label 106*/ 1588, // Rule ID 3846 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (add:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPADDDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3846,
      GIR_Done,
    // Label 106: @1588
    GIM_Reject,
    // Label 45: @1589
    GIM_Try, /*On fail goto*//*Label 107*/ 1646,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s8,
      GIM_Try, /*On fail goto*//*Label 108*/ 1622, // Rule ID 1809 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (add:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)  =>  (VPADDBYrr:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDBYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1809,
        GIR_Done,
      // Label 108: @1622
      GIM_Try, /*On fail goto*//*Label 109*/ 1645, // Rule ID 3897 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (add:{ *:[v32i8] } VR256X:{ *:[v32i8] }:$src1, VR256X:{ *:[v32i8] }:$src2)  =>  (VPADDBZ256rr:{ *:[v32i8] } VR256X:{ *:[v32i8] }:$src1, VR256X:{ *:[v32i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDBZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3897,
        GIR_Done,
      // Label 109: @1645
      GIM_Reject,
    // Label 107: @1646
    GIM_Reject,
    // Label 46: @1647
    GIM_Try, /*On fail goto*//*Label 110*/ 1678, // Rule ID 3873 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (add:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPADDWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3873,
      GIR_Done,
    // Label 110: @1678
    GIM_Reject,
    // Label 47: @1679
    GIM_Try, /*On fail goto*//*Label 111*/ 1710, // Rule ID 3891 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (add:{ *:[v64i8] } VR512:{ *:[v64i8] }:$src1, VR512:{ *:[v64i8] }:$src2)  =>  (VPADDBZrr:{ *:[v64i8] } VR512:{ *:[v64i8] }:$src1, VR512:{ *:[v64i8] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPADDBZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3891,
      GIR_Done,
    // Label 111: @1710
    GIM_Reject,
    // Label 48: @1711
    GIM_Reject,
    // Label 1: @1712
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 25, /*)*//*default:*//*Label 128*/ 2792,
    /*GILLT_s8*//*Label 112*/ 1742,
    /*GILLT_s16*//*Label 113*/ 1808,
    /*GILLT_s32*//*Label 114*/ 1907,
    /*GILLT_s64*//*Label 115*/ 2006, 0, 0, 0,
    /*GILLT_v2s64*//*Label 116*/ 2108, 0,
    /*GILLT_v4s32*//*Label 117*/ 2189,
    /*GILLT_v4s64*//*Label 118*/ 2270, 0,
    /*GILLT_v8s16*//*Label 119*/ 2328,
    /*GILLT_v8s32*//*Label 120*/ 2409,
    /*GILLT_v8s64*//*Label 121*/ 2467, 0,
    /*GILLT_v16s8*//*Label 122*/ 2499,
    /*GILLT_v16s16*//*Label 123*/ 2580,
    /*GILLT_v16s32*//*Label 124*/ 2638, 0,
    /*GILLT_v32s8*//*Label 125*/ 2670,
    /*GILLT_v32s16*//*Label 126*/ 2728, 0,
    /*GILLT_v64s8*//*Label 127*/ 2760,
    // Label 112: @1742
    GIM_Try, /*On fail goto*//*Label 129*/ 1807,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 130*/ 1790, // Rule ID 16103 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SUB8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16103,
        GIR_Done,
      // Label 130: @1790
      GIM_Try, /*On fail goto*//*Label 131*/ 1806, // Rule ID 16095 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR8RegClassID,
        // (sub:{ *:[i8] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (SUB8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB8rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16095,
        GIR_Done,
      // Label 131: @1806
      GIM_Reject,
    // Label 129: @1807
    GIM_Reject,
    // Label 113: @1808
    GIM_Try, /*On fail goto*//*Label 132*/ 1906,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 133*/ 1859, // Rule ID 16106 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (SUB16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16106,
        GIR_Done,
      // Label 133: @1859
      GIM_Try, /*On fail goto*//*Label 134*/ 1889, // Rule ID 16104 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (SUB16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16104,
        GIR_Done,
      // Label 134: @1889
      GIM_Try, /*On fail goto*//*Label 135*/ 1905, // Rule ID 16096 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (sub:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (SUB16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16096,
        GIR_Done,
      // Label 135: @1905
      GIM_Reject,
    // Label 132: @1906
    GIM_Reject,
    // Label 114: @1907
    GIM_Try, /*On fail goto*//*Label 136*/ 2005,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 137*/ 1958, // Rule ID 16107 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (SUB32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16107,
        GIR_Done,
      // Label 137: @1958
      GIM_Try, /*On fail goto*//*Label 138*/ 1988, // Rule ID 16105 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (SUB32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16105,
        GIR_Done,
      // Label 138: @1988
      GIM_Try, /*On fail goto*//*Label 139*/ 2004, // Rule ID 16097 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (sub:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (SUB32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16097,
        GIR_Done,
      // Label 139: @2004
      GIM_Reject,
    // Label 136: @2005
    GIM_Reject,
    // Label 115: @2006
    GIM_Try, /*On fail goto*//*Label 140*/ 2107,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 141*/ 2057, // Rule ID 16108 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (SUB64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16108,
        GIR_Done,
      // Label 141: @2057
      GIM_Try, /*On fail goto*//*Label 142*/ 2090, // Rule ID 16109 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (SUB64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SUB64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16109,
        GIR_Done,
      // Label 142: @2090
      GIM_Try, /*On fail goto*//*Label 143*/ 2106, // Rule ID 16098 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (sub:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (SUB64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16098,
        GIR_Done,
      // Label 143: @2106
      GIM_Reject,
    // Label 140: @2107
    GIM_Reject,
    // Label 116: @2108
    GIM_Try, /*On fail goto*//*Label 144*/ 2188,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 145*/ 2141, // Rule ID 1889 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPSUBQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1889,
        GIR_Done,
      // Label 145: @2141
      GIM_Try, /*On fail goto*//*Label 146*/ 2164, // Rule ID 1891 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (PSUBQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PSUBQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1891,
        GIR_Done,
      // Label 146: @2164
      GIM_Try, /*On fail goto*//*Label 147*/ 2187, // Rule ID 3927 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (sub:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPSUBQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3927,
        GIR_Done,
      // Label 147: @2187
      GIM_Reject,
    // Label 144: @2188
    GIM_Reject,
    // Label 117: @2189
    GIM_Try, /*On fail goto*//*Label 148*/ 2269,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 149*/ 2222, // Rule ID 1883 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPSUBDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1883,
        GIR_Done,
      // Label 149: @2222
      GIM_Try, /*On fail goto*//*Label 150*/ 2245, // Rule ID 1885 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (PSUBDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PSUBDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1885,
        GIR_Done,
      // Label 150: @2245
      GIM_Try, /*On fail goto*//*Label 151*/ 2268, // Rule ID 3954 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (sub:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPSUBDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3954,
        GIR_Done,
      // Label 151: @2268
      GIM_Reject,
    // Label 148: @2269
    GIM_Reject,
    // Label 118: @2270
    GIM_Try, /*On fail goto*//*Label 152*/ 2327,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 153*/ 2303, // Rule ID 1893 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (sub:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPSUBQYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBQYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1893,
        GIR_Done,
      // Label 153: @2303
      GIM_Try, /*On fail goto*//*Label 154*/ 2326, // Rule ID 3918 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (sub:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPSUBQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3918,
        GIR_Done,
      // Label 154: @2326
      GIM_Reject,
    // Label 152: @2327
    GIM_Reject,
    // Label 119: @2328
    GIM_Try, /*On fail goto*//*Label 155*/ 2408,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_Try, /*On fail goto*//*Label 156*/ 2361, // Rule ID 1877 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPSUBWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1877,
        GIR_Done,
      // Label 156: @2361
      GIM_Try, /*On fail goto*//*Label 157*/ 2384, // Rule ID 1879 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PSUBWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PSUBWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1879,
        GIR_Done,
      // Label 157: @2384
      GIM_Try, /*On fail goto*//*Label 158*/ 2407, // Rule ID 3975 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (sub:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPSUBWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBWZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3975,
        GIR_Done,
      // Label 158: @2407
      GIM_Reject,
    // Label 155: @2408
    GIM_Reject,
    // Label 120: @2409
    GIM_Try, /*On fail goto*//*Label 159*/ 2466,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 160*/ 2442, // Rule ID 1887 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (sub:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPSUBDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1887,
        GIR_Done,
      // Label 160: @2442
      GIM_Try, /*On fail goto*//*Label 161*/ 2465, // Rule ID 3945 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (sub:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPSUBDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3945,
        GIR_Done,
      // Label 161: @2465
      GIM_Reject,
    // Label 159: @2466
    GIM_Reject,
    // Label 121: @2467
    GIM_Try, /*On fail goto*//*Label 162*/ 2498, // Rule ID 3909 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sub:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPSUBQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3909,
      GIR_Done,
    // Label 162: @2498
    GIM_Reject,
    // Label 122: @2499
    GIM_Try, /*On fail goto*//*Label 163*/ 2579,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_Try, /*On fail goto*//*Label 164*/ 2532, // Rule ID 1871 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (VPSUBBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1871,
        GIR_Done,
      // Label 164: @2532
      GIM_Try, /*On fail goto*//*Label 165*/ 2555, // Rule ID 1873 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sub:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (PSUBBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PSUBBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1873,
        GIR_Done,
      // Label 165: @2555
      GIM_Try, /*On fail goto*//*Label 166*/ 2578, // Rule ID 3993 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (sub:{ *:[v16i8] } VR128X:{ *:[v16i8] }:$src1, VR128X:{ *:[v16i8] }:$src2)  =>  (VPSUBBZ128rr:{ *:[v16i8] } VR128X:{ *:[v16i8] }:$src1, VR128X:{ *:[v16i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBBZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3993,
        GIR_Done,
      // Label 166: @2578
      GIM_Reject,
    // Label 163: @2579
    GIM_Reject,
    // Label 123: @2580
    GIM_Try, /*On fail goto*//*Label 167*/ 2637,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_Try, /*On fail goto*//*Label 168*/ 2613, // Rule ID 1881 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (sub:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPSUBWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBWYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1881,
        GIR_Done,
      // Label 168: @2613
      GIM_Try, /*On fail goto*//*Label 169*/ 2636, // Rule ID 3969 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (sub:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPSUBWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBWZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3969,
        GIR_Done,
      // Label 169: @2636
      GIM_Reject,
    // Label 167: @2637
    GIM_Reject,
    // Label 124: @2638
    GIM_Try, /*On fail goto*//*Label 170*/ 2669, // Rule ID 3936 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sub:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPSUBDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3936,
      GIR_Done,
    // Label 170: @2669
    GIM_Reject,
    // Label 125: @2670
    GIM_Try, /*On fail goto*//*Label 171*/ 2727,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s8,
      GIM_Try, /*On fail goto*//*Label 172*/ 2703, // Rule ID 1875 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (sub:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)  =>  (VPSUBBYrr:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBBYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1875,
        GIR_Done,
      // Label 172: @2703
      GIM_Try, /*On fail goto*//*Label 173*/ 2726, // Rule ID 3987 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (sub:{ *:[v32i8] } VR256X:{ *:[v32i8] }:$src1, VR256X:{ *:[v32i8] }:$src2)  =>  (VPSUBBZ256rr:{ *:[v32i8] } VR256X:{ *:[v32i8] }:$src1, VR256X:{ *:[v32i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBBZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3987,
        GIR_Done,
      // Label 173: @2726
      GIM_Reject,
    // Label 171: @2727
    GIM_Reject,
    // Label 126: @2728
    GIM_Try, /*On fail goto*//*Label 174*/ 2759, // Rule ID 3963 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sub:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPSUBWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3963,
      GIR_Done,
    // Label 174: @2759
    GIM_Reject,
    // Label 127: @2760
    GIM_Try, /*On fail goto*//*Label 175*/ 2791, // Rule ID 3981 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sub:{ *:[v64i8] } VR512:{ *:[v64i8] }:$src1, VR512:{ *:[v64i8] }:$src2)  =>  (VPSUBBZrr:{ *:[v64i8] } VR512:{ *:[v64i8] }:$src1, VR512:{ *:[v64i8] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSUBBZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3981,
      GIR_Done,
    // Label 175: @2791
    GIM_Reject,
    // Label 128: @2792
    GIM_Reject,
    // Label 2: @2793
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/2, 23, /*)*//*default:*//*Label 188*/ 3558,
    /*GILLT_s16*//*Label 176*/ 2820,
    /*GILLT_s32*//*Label 177*/ 2919,
    /*GILLT_s64*//*Label 178*/ 3018, 0, 0, 0,
    /*GILLT_v2s64*//*Label 179*/ 3120, 0,
    /*GILLT_v4s32*//*Label 180*/ 3152,
    /*GILLT_v4s64*//*Label 181*/ 3233, 0,
    /*GILLT_v8s16*//*Label 182*/ 3265,
    /*GILLT_v8s32*//*Label 183*/ 3346,
    /*GILLT_v8s64*//*Label 184*/ 3404, 0, 0,
    /*GILLT_v16s16*//*Label 185*/ 3436,
    /*GILLT_v16s32*//*Label 186*/ 3494, 0, 0,
    /*GILLT_v32s16*//*Label 187*/ 3526,
    // Label 176: @2820
    GIM_Try, /*On fail goto*//*Label 189*/ 2918,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 190*/ 2871, // Rule ID 16124 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (IMUL16rri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL16rri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16124,
        GIR_Done,
      // Label 190: @2871
      GIM_Try, /*On fail goto*//*Label 191*/ 2901, // Rule ID 16122 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (IMUL16rri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL16rri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16122,
        GIR_Done,
      // Label 191: @2901
      GIM_Try, /*On fail goto*//*Label 192*/ 2917, // Rule ID 16116 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (mul:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (IMUL16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::IMUL16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16116,
        GIR_Done,
      // Label 192: @2917
      GIM_Reject,
    // Label 189: @2918
    GIM_Reject,
    // Label 177: @2919
    GIM_Try, /*On fail goto*//*Label 193*/ 3017,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 194*/ 2970, // Rule ID 16125 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (IMUL32rri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL32rri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16125,
        GIR_Done,
      // Label 194: @2970
      GIM_Try, /*On fail goto*//*Label 195*/ 3000, // Rule ID 16123 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (IMUL32rri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL32rri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16123,
        GIR_Done,
      // Label 195: @3000
      GIM_Try, /*On fail goto*//*Label 196*/ 3016, // Rule ID 16117 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (mul:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (IMUL32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::IMUL32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16117,
        GIR_Done,
      // Label 196: @3016
      GIM_Reject,
    // Label 193: @3017
    GIM_Reject,
    // Label 178: @3018
    GIM_Try, /*On fail goto*//*Label 197*/ 3119,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 198*/ 3069, // Rule ID 16126 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (IMUL64rri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL64rri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16126,
        GIR_Done,
      // Label 198: @3069
      GIM_Try, /*On fail goto*//*Label 199*/ 3102, // Rule ID 16127 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (mul:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (IMUL64rri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::IMUL64rri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16127,
        GIR_Done,
      // Label 199: @3102
      GIM_Try, /*On fail goto*//*Label 200*/ 3118, // Rule ID 16118 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (mul:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (IMUL64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::IMUL64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16118,
        GIR_Done,
      // Label 200: @3118
      GIM_Reject,
    // Label 197: @3119
    GIM_Reject,
    // Label 179: @3120
    GIM_Try, /*On fail goto*//*Label 201*/ 3151, // Rule ID 4206 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
      // (mul:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPMULLQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4206,
      GIR_Done,
    // Label 201: @3151
    GIM_Reject,
    // Label 180: @3152
    GIM_Try, /*On fail goto*//*Label 202*/ 3232,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 203*/ 3185, // Rule ID 2511 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (mul:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPMULLDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2511,
        GIR_Done,
      // Label 203: @3185
      GIM_Try, /*On fail goto*//*Label 204*/ 3208, // Rule ID 2519 //
        GIM_CheckFeatures, GIFBS_UseSSE41,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (mul:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (PMULLDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PMULLDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2519,
        GIR_Done,
      // Label 204: @3208
      GIM_Try, /*On fail goto*//*Label 205*/ 3231, // Rule ID 4161 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (mul:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPMULLDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4161,
        GIR_Done,
      // Label 205: @3231
      GIM_Reject,
    // Label 202: @3232
    GIM_Reject,
    // Label 181: @3233
    GIM_Try, /*On fail goto*//*Label 206*/ 3264, // Rule ID 4197 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
      // (mul:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPMULLQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4197,
      GIR_Done,
    // Label 206: @3264
    GIM_Reject,
    // Label 182: @3265
    GIM_Try, /*On fail goto*//*Label 207*/ 3345,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_Try, /*On fail goto*//*Label 208*/ 3298, // Rule ID 1853 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (mul:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPMULLWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1853,
        GIR_Done,
      // Label 208: @3298
      GIM_Try, /*On fail goto*//*Label 209*/ 3321, // Rule ID 1855 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (mul:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PMULLWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PMULLWrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1855,
        GIR_Done,
      // Label 209: @3321
      GIM_Try, /*On fail goto*//*Label 210*/ 3344, // Rule ID 4182 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (mul:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPMULLWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLWZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4182,
        GIR_Done,
      // Label 210: @3344
      GIM_Reject,
    // Label 207: @3345
    GIM_Reject,
    // Label 183: @3346
    GIM_Try, /*On fail goto*//*Label 211*/ 3403,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 212*/ 3379, // Rule ID 2515 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (mul:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPMULLDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2515,
        GIR_Done,
      // Label 212: @3379
      GIM_Try, /*On fail goto*//*Label 213*/ 3402, // Rule ID 4152 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (mul:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPMULLDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4152,
        GIR_Done,
      // Label 213: @3402
      GIM_Reject,
    // Label 211: @3403
    GIM_Reject,
    // Label 184: @3404
    GIM_Try, /*On fail goto*//*Label 214*/ 3435, // Rule ID 4188 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (mul:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPMULLQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4188,
      GIR_Done,
    // Label 214: @3435
    GIM_Reject,
    // Label 185: @3436
    GIM_Try, /*On fail goto*//*Label 215*/ 3493,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_Try, /*On fail goto*//*Label 216*/ 3469, // Rule ID 1857 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX_Or_NoBWI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (mul:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPMULLWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLWYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1857,
        GIR_Done,
      // Label 216: @3469
      GIM_Try, /*On fail goto*//*Label 217*/ 3492, // Rule ID 4176 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (mul:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPMULLWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLWZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4176,
        GIR_Done,
      // Label 217: @3492
      GIM_Reject,
    // Label 215: @3493
    GIM_Reject,
    // Label 186: @3494
    GIM_Try, /*On fail goto*//*Label 218*/ 3525, // Rule ID 4143 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (mul:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPMULLDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4143,
      GIR_Done,
    // Label 218: @3525
    GIM_Reject,
    // Label 187: @3526
    GIM_Try, /*On fail goto*//*Label 219*/ 3557, // Rule ID 4170 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (mul:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPMULLWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMULLWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4170,
      GIR_Done,
    // Label 219: @3557
    GIM_Reject,
    // Label 188: @3558
    GIM_Reject,
    // Label 3: @3559
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/0, 24, /*)*//*default:*//*Label 234*/ 6670,
    /*GILLT_s1*//*Label 220*/ 3589,
    /*GILLT_s8*//*Label 221*/ 3887,
    /*GILLT_s16*//*Label 222*/ 3953,
    /*GILLT_s32*//*Label 223*/ 4052,
    /*GILLT_s64*//*Label 224*/ 5213, 0, 0,
    /*GILLT_v2s1*//*Label 225*/ 6085,
    /*GILLT_v2s64*//*Label 226*/ 6178,
    /*GILLT_v4s1*//*Label 227*/ 6259, 0,
    /*GILLT_v4s64*//*Label 228*/ 6352,
    /*GILLT_v8s1*//*Label 229*/ 6433, 0, 0,
    /*GILLT_v8s64*//*Label 230*/ 6542,
    /*GILLT_v16s1*//*Label 231*/ 6574, 0, 0, 0,
    /*GILLT_v32s1*//*Label 232*/ 6606, 0, 0,
    /*GILLT_v64s1*//*Label 233*/ 6638,
    // Label 220: @3589
    GIM_Try, /*On fail goto*//*Label 235*/ 3886,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_Try, /*On fail goto*//*Label 236*/ 3704, // Rule ID 13774 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s1,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK1RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[v1i1] } (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, -1:{ *:[v1i1] }), VK1:{ *:[v1i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KANDNWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDNWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 13774,
        GIR_Done,
      // Label 236: @3704
      GIM_Try, /*On fail goto*//*Label 237*/ 3805, // Rule ID 18358 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s1,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src2, (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, -1:{ *:[v1i1] }))  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KANDNWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/1, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDNWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 18358,
        GIR_Done,
      // Label 237: @3805
      GIM_Try, /*On fail goto*//*Label 238*/ 3885, // Rule ID 13770 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK1RegClassID,
        // (and:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, VK1:{ *:[v1i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KANDWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 13770,
        GIR_Done,
      // Label 238: @3885
      GIM_Reject,
    // Label 235: @3886
    GIM_Reject,
    // Label 221: @3887
    GIM_Try, /*On fail goto*//*Label 239*/ 3952,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 240*/ 3935, // Rule ID 16180 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (AND8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16180,
        GIR_Done,
      // Label 240: @3935
      GIM_Try, /*On fail goto*//*Label 241*/ 3951, // Rule ID 16172 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR8RegClassID,
        // (and:{ *:[i8] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (AND8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::AND8rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16172,
        GIR_Done,
      // Label 241: @3951
      GIM_Reject,
    // Label 239: @3952
    GIM_Reject,
    // Label 222: @3953
    GIM_Try, /*On fail goto*//*Label 242*/ 4051,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 243*/ 4004, // Rule ID 16183 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (AND16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16183,
        GIR_Done,
      // Label 243: @4004
      GIM_Try, /*On fail goto*//*Label 244*/ 4034, // Rule ID 16181 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (AND16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16181,
        GIR_Done,
      // Label 244: @4034
      GIM_Try, /*On fail goto*//*Label 245*/ 4050, // Rule ID 16173 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (and:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (AND16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::AND16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16173,
        GIR_Done,
      // Label 245: @4050
      GIM_Reject,
    // Label 242: @4051
    GIM_Reject,
    // Label 223: @4052
    GIM_Try, /*On fail goto*//*Label 246*/ 5212,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 247*/ 4153, // Rule ID 18214 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_LSHR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s8,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/3, /*MI*/2, /*OpIdx*/1, // MIs[3]
        GIM_CheckOpcode, /*MI*/3, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/3, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/3, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/3, /*Op*/1, 32,
        GIM_CheckRegBankForClass, /*MI*/3, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/3,
        // (and:{ *:[i32] } (srl:{ *:[i32] } -1:{ *:[i32] }, (trunc:{ *:[i8] } (sub:{ *:[i32] } 32:{ *:[i32] }, GR32:{ *:[i32] }:$lz))), GR32:{ *:[i32] }:$src)  =>  (BZHI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src, GR32:{ *:[i32] }:$lz)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BZHI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/3, /*OpIdx*/2, // lz
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18214,
        GIR_Done,
      // Label 247: @4153
      GIM_Try, /*On fail goto*//*Label 248*/ 4248, // Rule ID 12253 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_LSHR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s8,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/3, /*MI*/2, /*OpIdx*/1, // MIs[3]
        GIM_CheckOpcode, /*MI*/3, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/3, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/3, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/3, /*Op*/1, 32,
        GIM_CheckRegBankForClass, /*MI*/3, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/3,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src, (srl:{ *:[i32] } -1:{ *:[i32] }, (trunc:{ *:[i8] } (sub:{ *:[i32] } 32:{ *:[i32] }, GR32:{ *:[i32] }:$lz))))  =>  (BZHI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src, GR32:{ *:[i32] }:$lz)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BZHI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/3, /*OpIdx*/2, // lz
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12253,
        GIR_Done,
      // Label 248: @4248
      GIM_Try, /*On fail goto*//*Label 249*/ 4323, // Rule ID 18224 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLCIC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCIC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18224,
        GIR_Done,
      // Label 249: @4323
      GIM_Try, /*On fail goto*//*Label 250*/ 4398, // Rule ID 18236 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (TZMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::TZMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18236,
        GIR_Done,
      // Label 250: @4398
      GIM_Try, /*On fail goto*//*Label 251*/ 4473, // Rule ID 12269 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i32] } (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }))  =>  (BLCIC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCIC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12269,
        GIR_Done,
      // Label 251: @4473
      GIM_Try, /*On fail goto*//*Label 252*/ 4548, // Rule ID 12281 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i32] } (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (TZMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::TZMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12281,
        GIR_Done,
      // Label 252: @4548
      GIM_Try, /*On fail goto*//*Label 253*/ 4602, // Rule ID 18218 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLCFILL32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCFILL32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18218,
        GIR_Done,
      // Label 253: @4602
      GIM_Try, /*On fail goto*//*Label 254*/ 4656, // Rule ID 18728 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLSR32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSR32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18728,
        GIR_Done,
      // Label 254: @4656
      GIM_Try, /*On fail goto*//*Label 255*/ 4710, // Rule ID 18732 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, 0,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } (sub:{ *:[i32] } 0:{ *:[i32] }, GR32:{ *:[i32] }:$src), GR32:{ *:[i32] }:$src)  =>  (BLSI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18732,
        GIR_Done,
      // Label 255: @4710
      GIM_Try, /*On fail goto*//*Label 256*/ 4764, // Rule ID 12263 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }))  =>  (BLCFILL32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCFILL32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12263,
        GIR_Done,
      // Label 256: @4764
      GIM_Try, /*On fail goto*//*Label 257*/ 4818, // Rule ID 16451 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLSR32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSR32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16451,
        GIR_Done,
      // Label 257: @4818
      GIM_Try, /*On fail goto*//*Label 258*/ 4872, // Rule ID 16455 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, 0,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/2, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src, (sub:{ *:[i32] } 0:{ *:[i32] }, GR32:{ *:[i32] }:$src))  =>  (BLSI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16455,
        GIR_Done,
      // Label 258: @4872
      GIM_Try, /*On fail goto*//*Label 259*/ 4923, // Rule ID 15956 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 65535,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src1, 65535:{ *:[i32] })  =>  (MOVZX32rr16:{ *:[i32] } (EXTRACT_SUBREG:{ *:[i16] } GR32:{ *:[i32] }:$src1, sub_16bit:{ *:[i32] }))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s16,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/4, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOVZX32rr16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15956,
        GIR_Done,
      // Label 259: @4923
      GIM_Try, /*On fail goto*//*Label 260*/ 4974, // Rule ID 15957 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 255,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src1, 255:{ *:[i32] })  =>  (MOVZX32rr8:{ *:[i32] } (EXTRACT_SUBREG:{ *:[i8] } GR32:{ *:[i32] }:$src1, sub_8bit:{ *:[i32] }))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s8,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOVZX32rr8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15957,
        GIR_Done,
      // Label 260: @4974
      GIM_Try, /*On fail goto*//*Label 261*/ 5019, // Rule ID 16184 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (AND32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16184,
        GIR_Done,
      // Label 261: @5019
      GIM_Try, /*On fail goto*//*Label 262*/ 5061, // Rule ID 16182 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (AND32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16182,
        GIR_Done,
      // Label 262: @5061
      GIM_Try, /*On fail goto*//*Label 263*/ 5122, // Rule ID 12283 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, -1:{ *:[i32] }), GR32:{ *:[i32] }:$src2)  =>  (ANDN32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ANDN32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12283,
        GIR_Done,
      // Label 263: @5122
      GIM_Try, /*On fail goto*//*Label 264*/ 5183, // Rule ID 18238 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src2, (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, -1:{ *:[i32] }))  =>  (ANDN32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ANDN32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18238,
        GIR_Done,
      // Label 264: @5183
      GIM_Try, /*On fail goto*//*Label 265*/ 5211, // Rule ID 16174 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (and:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (AND32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::AND32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16174,
        GIR_Done,
      // Label 265: @5211
      GIM_Reject,
    // Label 246: @5212
    GIM_Reject,
    // Label 224: @5213
    GIM_Try, /*On fail goto*//*Label 266*/ 6084,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 267*/ 5294, // Rule ID 18225 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLCIC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCIC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18225,
        GIR_Done,
      // Label 267: @5294
      GIM_Try, /*On fail goto*//*Label 268*/ 5369, // Rule ID 18237 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (TZMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::TZMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18237,
        GIR_Done,
      // Label 268: @5369
      GIM_Try, /*On fail goto*//*Label 269*/ 5444, // Rule ID 12270 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i64] } (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }))  =>  (BLCIC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCIC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12270,
        GIR_Done,
      // Label 269: @5444
      GIM_Try, /*On fail goto*//*Label 270*/ 5519, // Rule ID 12282 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (and:{ *:[i64] } (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (TZMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::TZMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12282,
        GIR_Done,
      // Label 270: @5519
      GIM_Try, /*On fail goto*//*Label 271*/ 5573, // Rule ID 18219 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLCFILL64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCFILL64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18219,
        GIR_Done,
      // Label 271: @5573
      GIM_Try, /*On fail goto*//*Label 272*/ 5627, // Rule ID 18729 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLSR64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSR64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18729,
        GIR_Done,
      // Label 272: @5627
      GIM_Try, /*On fail goto*//*Label 273*/ 5681, // Rule ID 18733 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, 0,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } (sub:{ *:[i64] } 0:{ *:[i64] }, GR64:{ *:[i64] }:$src), GR64:{ *:[i64] }:$src)  =>  (BLSI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18733,
        GIR_Done,
      // Label 273: @5681
      GIM_Try, /*On fail goto*//*Label 274*/ 5735, // Rule ID 12264 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }))  =>  (BLCFILL64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCFILL64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12264,
        GIR_Done,
      // Label 274: @5735
      GIM_Try, /*On fail goto*//*Label 275*/ 5789, // Rule ID 16452 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLSR64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSR64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16452,
        GIR_Done,
      // Label 275: @5789
      GIM_Try, /*On fail goto*//*Label 276*/ 5843, // Rule ID 16456 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, 0,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/2, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src, (sub:{ *:[i64] } 0:{ *:[i64] }, GR64:{ *:[i64] }:$src))  =>  (BLSI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16456,
        GIR_Done,
      // Label 276: @5843
      GIM_Try, /*On fail goto*//*Label 277*/ 5888, // Rule ID 16185 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (AND64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16185,
        GIR_Done,
      // Label 277: @5888
      GIM_Try, /*On fail goto*//*Label 278*/ 5933, // Rule ID 16186 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (AND64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AND64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16186,
        GIR_Done,
      // Label 278: @5933
      GIM_Try, /*On fail goto*//*Label 279*/ 5994, // Rule ID 12284 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, -1:{ *:[i64] }), GR64:{ *:[i64] }:$src2)  =>  (ANDN64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ANDN64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12284,
        GIR_Done,
      // Label 279: @5994
      GIM_Try, /*On fail goto*//*Label 280*/ 6055, // Rule ID 18239 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src2, (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, -1:{ *:[i64] }))  =>  (ANDN64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ANDN64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18239,
        GIR_Done,
      // Label 280: @6055
      GIM_Try, /*On fail goto*//*Label 281*/ 6083, // Rule ID 16175 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (and:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (AND64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::AND64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16175,
        GIR_Done,
      // Label 281: @6083
      GIM_Reject,
    // Label 266: @6084
    GIM_Reject,
    // Label 225: @6085
    GIM_Try, /*On fail goto*//*Label 282*/ 6177, // Rule ID 13771 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK2RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK2RegClassID,
      // (and:{ *:[v2i1] } VK2:{ *:[v2i1] }:$src1, VK2:{ *:[v2i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v2i1] } (KANDWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13771,
      GIR_Done,
    // Label 282: @6177
    GIM_Reject,
    // Label 226: @6178
    GIM_Try, /*On fail goto*//*Label 283*/ 6258,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 284*/ 6211, // Rule ID 1545 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (and:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPANDrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPANDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1545,
        GIR_Done,
      // Label 284: @6211
      GIM_Try, /*On fail goto*//*Label 285*/ 6234, // Rule ID 1547 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (and:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (PANDrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PANDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1547,
        GIR_Done,
      // Label 285: @6234
      GIM_Try, /*On fail goto*//*Label 286*/ 6257, // Rule ID 4890 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (and:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPANDQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPANDQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4890,
        GIR_Done,
      // Label 286: @6257
      GIM_Reject,
    // Label 283: @6258
    GIM_Reject,
    // Label 227: @6259
    GIM_Try, /*On fail goto*//*Label 287*/ 6351, // Rule ID 13772 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK4RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK4RegClassID,
      // (and:{ *:[v4i1] } VK4:{ *:[v4i1] }:$src1, VK4:{ *:[v4i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v4i1] } (KANDWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13772,
      GIR_Done,
    // Label 287: @6351
    GIM_Reject,
    // Label 228: @6352
    GIM_Try, /*On fail goto*//*Label 288*/ 6432,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 289*/ 6385, // Rule ID 1549 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (and:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPANDYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPANDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1549,
        GIR_Done,
      // Label 289: @6385
      GIM_Try, /*On fail goto*//*Label 290*/ 6408, // Rule ID 4881 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (and:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPANDQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPANDQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4881,
        GIR_Done,
      // Label 290: @6408
      GIM_Try, /*On fail goto*//*Label 291*/ 6431, // Rule ID 12544 //
        GIM_CheckFeatures, GIFBS_HasAVX1Only,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (and:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VANDPSYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VANDPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12544,
        GIR_Done,
      // Label 291: @6431
      GIM_Reject,
    // Label 288: @6432
    GIM_Reject,
    // Label 229: @6433
    GIM_Try, /*On fail goto*//*Label 292*/ 6541,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK8RegClassID,
      GIM_Try, /*On fail goto*//*Label 293*/ 6466, // Rule ID 3605 //
        GIM_CheckFeatures, GIFBS_HasDQI,
        // (and:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (KANDBrr:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KANDBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3605,
        GIR_Done,
      // Label 293: @6466
      GIM_Try, /*On fail goto*//*Label 294*/ 6540, // Rule ID 13769 //
        GIM_CheckFeatures, GIFBS_NoDQI,
        // (and:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v8i1] } (KANDWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src2, VK16:{ *:[i32] })), VK8:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KANDWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK8*/12,
        // GIR_Coverage, 13769,
        GIR_Done,
      // Label 294: @6540
      GIM_Reject,
    // Label 292: @6541
    GIM_Reject,
    // Label 230: @6542
    GIM_Try, /*On fail goto*//*Label 295*/ 6573, // Rule ID 4872 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (and:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPANDQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPANDQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4872,
      GIR_Done,
    // Label 295: @6573
    GIM_Reject,
    // Label 231: @6574
    GIM_Try, /*On fail goto*//*Label 296*/ 6605, // Rule ID 3606 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK16RegClassID,
      // (and:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)  =>  (KANDWrr:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KANDWrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3606,
      GIR_Done,
    // Label 296: @6605
    GIM_Reject,
    // Label 232: @6606
    GIM_Try, /*On fail goto*//*Label 297*/ 6637, // Rule ID 3607 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK32RegClassID,
      // (and:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)  =>  (KANDDrr:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KANDDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3607,
      GIR_Done,
    // Label 297: @6637
    GIM_Reject,
    // Label 233: @6638
    GIM_Try, /*On fail goto*//*Label 298*/ 6669, // Rule ID 3608 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v64s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK64RegClassID,
      // (and:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)  =>  (KANDQrr:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KANDQrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3608,
      GIR_Done,
    // Label 298: @6669
    GIM_Reject,
    // Label 234: @6670
    GIM_Reject,
    // Label 4: @6671
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/0, 24, /*)*//*default:*//*Label 313*/ 9341,
    /*GILLT_s1*//*Label 299*/ 6701,
    /*GILLT_s8*//*Label 300*/ 6794,
    /*GILLT_s16*//*Label 301*/ 6860,
    /*GILLT_s32*//*Label 302*/ 6959,
    /*GILLT_s64*//*Label 303*/ 7856, 0, 0,
    /*GILLT_v2s1*//*Label 304*/ 8756,
    /*GILLT_v2s64*//*Label 305*/ 8849,
    /*GILLT_v4s1*//*Label 306*/ 8930, 0,
    /*GILLT_v4s64*//*Label 307*/ 9023,
    /*GILLT_v8s1*//*Label 308*/ 9104, 0, 0,
    /*GILLT_v8s64*//*Label 309*/ 9213,
    /*GILLT_v16s1*//*Label 310*/ 9245, 0, 0, 0,
    /*GILLT_v32s1*//*Label 311*/ 9277, 0, 0,
    /*GILLT_v64s1*//*Label 312*/ 9309,
    // Label 299: @6701
    GIM_Try, /*On fail goto*//*Label 314*/ 6793, // Rule ID 13778 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK1RegClassID,
      // (or:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, VK1:{ *:[v1i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KORWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13778,
      GIR_Done,
    // Label 314: @6793
    GIM_Reject,
    // Label 300: @6794
    GIM_Try, /*On fail goto*//*Label 315*/ 6859,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 316*/ 6842, // Rule ID 16150 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (OR8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16150,
        GIR_Done,
      // Label 316: @6842
      GIM_Try, /*On fail goto*//*Label 317*/ 6858, // Rule ID 16142 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR8RegClassID,
        // (or:{ *:[i8] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (OR8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::OR8rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16142,
        GIR_Done,
      // Label 317: @6858
      GIM_Reject,
    // Label 315: @6859
    GIM_Reject,
    // Label 301: @6860
    GIM_Try, /*On fail goto*//*Label 318*/ 6958,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 319*/ 6911, // Rule ID 16153 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (OR16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16153,
        GIR_Done,
      // Label 319: @6911
      GIM_Try, /*On fail goto*//*Label 320*/ 6941, // Rule ID 16151 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (OR16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16151,
        GIR_Done,
      // Label 320: @6941
      GIM_Try, /*On fail goto*//*Label 321*/ 6957, // Rule ID 16143 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (or:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (OR16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::OR16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16143,
        GIR_Done,
      // Label 321: @6957
      GIM_Reject,
    // Label 318: @6958
    GIM_Reject,
    // Label 302: @6959
    GIM_Try, /*On fail goto*//*Label 322*/ 7855,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 323*/ 7040, // Rule ID 18232 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLSIC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSIC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18232,
        GIR_Done,
      // Label 323: @7040
      GIM_Try, /*On fail goto*//*Label 324*/ 7115, // Rule ID 18234 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (T1MSKC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::T1MSKC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18234,
        GIR_Done,
      // Label 324: @7115
      GIM_Try, /*On fail goto*//*Label 325*/ 7190, // Rule ID 18220 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/2, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } (xor:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), -1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLCI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18220,
        GIR_Done,
      // Label 325: @7190
      GIM_Try, /*On fail goto*//*Label 326*/ 7265, // Rule ID 12277 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLSIC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSIC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12277,
        GIR_Done,
      // Label 326: @7265
      GIM_Try, /*On fail goto*//*Label 327*/ 7340, // Rule ID 12279 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }))  =>  (T1MSKC32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::T1MSKC32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12279,
        GIR_Done,
      // Label 327: @7340
      GIM_Try, /*On fail goto*//*Label 328*/ 7415, // Rule ID 12265 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src, (xor:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), -1:{ *:[i32] }))  =>  (BLCI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12265,
        GIR_Done,
      // Label 328: @7415
      GIM_Try, /*On fail goto*//*Label 329*/ 7469, // Rule ID 18228 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLCS32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCS32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18228,
        GIR_Done,
      // Label 329: @7469
      GIM_Try, /*On fail goto*//*Label 330*/ 7523, // Rule ID 18230 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLSFILL32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSFILL32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18230,
        GIR_Done,
      // Label 330: @7523
      GIM_Try, /*On fail goto*//*Label 331*/ 7577, // Rule ID 18222 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -2,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } (sub:{ *:[i32] } -2:{ *:[i32] }, GR32:{ *:[i32] }:$src), GR32:{ *:[i32] }:$src)  =>  (BLCI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18222,
        GIR_Done,
      // Label 331: @7577
      GIM_Try, /*On fail goto*//*Label 332*/ 7631, // Rule ID 12273 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }))  =>  (BLCS32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCS32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12273,
        GIR_Done,
      // Label 332: @7631
      GIM_Try, /*On fail goto*//*Label 333*/ 7685, // Rule ID 12275 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLSFILL32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSFILL32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12275,
        GIR_Done,
      // Label 333: @7685
      GIM_Try, /*On fail goto*//*Label 334*/ 7739, // Rule ID 12267 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -2,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/2, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src, (sub:{ *:[i32] } -2:{ *:[i32] }, GR32:{ *:[i32] }:$src))  =>  (BLCI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12267,
        GIR_Done,
      // Label 334: @7739
      GIM_Try, /*On fail goto*//*Label 335*/ 7784, // Rule ID 16154 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (OR32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16154,
        GIR_Done,
      // Label 335: @7784
      GIM_Try, /*On fail goto*//*Label 336*/ 7826, // Rule ID 16152 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (OR32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16152,
        GIR_Done,
      // Label 336: @7826
      GIM_Try, /*On fail goto*//*Label 337*/ 7854, // Rule ID 16144 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (or:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (OR32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::OR32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16144,
        GIR_Done,
      // Label 337: @7854
      GIM_Reject,
    // Label 322: @7855
    GIM_Reject,
    // Label 303: @7856
    GIM_Try, /*On fail goto*//*Label 338*/ 8755,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 339*/ 7937, // Rule ID 18233 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLSIC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSIC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18233,
        GIR_Done,
      // Label 339: @7937
      GIM_Try, /*On fail goto*//*Label 340*/ 8012, // Rule ID 18235 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (T1MSKC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::T1MSKC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18235,
        GIR_Done,
      // Label 340: @8012
      GIM_Try, /*On fail goto*//*Label 341*/ 8087, // Rule ID 18221 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/2, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } (xor:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), -1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLCI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18221,
        GIR_Done,
      // Label 341: @8087
      GIM_Try, /*On fail goto*//*Label 342*/ 8162, // Rule ID 12278 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLSIC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSIC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12278,
        GIR_Done,
      // Label 342: @8162
      GIM_Try, /*On fail goto*//*Label 343*/ 8237, // Rule ID 12280 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/0, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }))  =>  (T1MSKC64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::T1MSKC64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12280,
        GIR_Done,
      // Label 343: @8237
      GIM_Try, /*On fail goto*//*Label 344*/ 8312, // Rule ID 12266 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[2] src
        GIM_CheckIsSameOperand, /*MI*/2, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/2, /*Op*/2, 1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src, (xor:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), -1:{ *:[i64] }))  =>  (BLCI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12266,
        GIR_Done,
      // Label 344: @8312
      GIM_Try, /*On fail goto*//*Label 345*/ 8366, // Rule ID 18229 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLCS64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCS64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18229,
        GIR_Done,
      // Label 345: @8366
      GIM_Try, /*On fail goto*//*Label 346*/ 8420, // Rule ID 18231 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLSFILL64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSFILL64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18231,
        GIR_Done,
      // Label 346: @8420
      GIM_Try, /*On fail goto*//*Label 347*/ 8474, // Rule ID 18223 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -2,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } (sub:{ *:[i64] } -2:{ *:[i64] }, GR64:{ *:[i64] }:$src), GR64:{ *:[i64] }:$src)  =>  (BLCI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18223,
        GIR_Done,
      // Label 347: @8474
      GIM_Try, /*On fail goto*//*Label 348*/ 8528, // Rule ID 12274 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }))  =>  (BLCS64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCS64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12274,
        GIR_Done,
      // Label 348: @8528
      GIM_Try, /*On fail goto*//*Label 349*/ 8582, // Rule ID 12276 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLSFILL64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSFILL64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12276,
        GIR_Done,
      // Label 349: @8582
      GIM_Try, /*On fail goto*//*Label 350*/ 8636, // Rule ID 12268 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/1, -2,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/2, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src, (sub:{ *:[i64] } -2:{ *:[i64] }, GR64:{ *:[i64] }:$src))  =>  (BLCI64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCI64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12268,
        GIR_Done,
      // Label 350: @8636
      GIM_Try, /*On fail goto*//*Label 351*/ 8681, // Rule ID 16155 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (OR64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16155,
        GIR_Done,
      // Label 351: @8681
      GIM_Try, /*On fail goto*//*Label 352*/ 8726, // Rule ID 16156 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (OR64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::OR64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16156,
        GIR_Done,
      // Label 352: @8726
      GIM_Try, /*On fail goto*//*Label 353*/ 8754, // Rule ID 16145 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (or:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (OR64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::OR64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16145,
        GIR_Done,
      // Label 353: @8754
      GIM_Reject,
    // Label 338: @8755
    GIM_Reject,
    // Label 304: @8756
    GIM_Try, /*On fail goto*//*Label 354*/ 8848, // Rule ID 13779 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK2RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK2RegClassID,
      // (or:{ *:[v2i1] } VK2:{ *:[v2i1] }:$src1, VK2:{ *:[v2i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v2i1] } (KORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KORWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13779,
      GIR_Done,
    // Label 354: @8848
    GIM_Reject,
    // Label 305: @8849
    GIM_Try, /*On fail goto*//*Label 355*/ 8929,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 356*/ 8882, // Rule ID 1551 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (or:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPORrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPORrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1551,
        GIR_Done,
      // Label 356: @8882
      GIM_Try, /*On fail goto*//*Label 357*/ 8905, // Rule ID 1553 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (or:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (PORrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PORrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1553,
        GIR_Done,
      // Label 357: @8905
      GIM_Try, /*On fail goto*//*Label 358*/ 8928, // Rule ID 4938 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (or:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPORQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPORQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4938,
        GIR_Done,
      // Label 358: @8928
      GIM_Reject,
    // Label 355: @8929
    GIM_Reject,
    // Label 306: @8930
    GIM_Try, /*On fail goto*//*Label 359*/ 9022, // Rule ID 13780 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK4RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK4RegClassID,
      // (or:{ *:[v4i1] } VK4:{ *:[v4i1] }:$src1, VK4:{ *:[v4i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v4i1] } (KORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KORWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13780,
      GIR_Done,
    // Label 359: @9022
    GIM_Reject,
    // Label 307: @9023
    GIM_Try, /*On fail goto*//*Label 360*/ 9103,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 361*/ 9056, // Rule ID 1555 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (or:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPORYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPORYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1555,
        GIR_Done,
      // Label 361: @9056
      GIM_Try, /*On fail goto*//*Label 362*/ 9079, // Rule ID 4929 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (or:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPORQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPORQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4929,
        GIR_Done,
      // Label 362: @9079
      GIM_Try, /*On fail goto*//*Label 363*/ 9102, // Rule ID 12545 //
        GIM_CheckFeatures, GIFBS_HasAVX1Only,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (or:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VORPSYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VORPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12545,
        GIR_Done,
      // Label 363: @9102
      GIM_Reject,
    // Label 360: @9103
    GIM_Reject,
    // Label 308: @9104
    GIM_Try, /*On fail goto*//*Label 364*/ 9212,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK8RegClassID,
      GIM_Try, /*On fail goto*//*Label 365*/ 9137, // Rule ID 3609 //
        GIM_CheckFeatures, GIFBS_HasDQI,
        // (or:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (KORBrr:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KORBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3609,
        GIR_Done,
      // Label 365: @9137
      GIM_Try, /*On fail goto*//*Label 366*/ 9211, // Rule ID 13777 //
        GIM_CheckFeatures, GIFBS_NoDQI,
        // (or:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v8i1] } (KORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src2, VK16:{ *:[i32] })), VK8:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK8*/12,
        // GIR_Coverage, 13777,
        GIR_Done,
      // Label 366: @9211
      GIM_Reject,
    // Label 364: @9212
    GIM_Reject,
    // Label 309: @9213
    GIM_Try, /*On fail goto*//*Label 367*/ 9244, // Rule ID 4920 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (or:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPORQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPORQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4920,
      GIR_Done,
    // Label 367: @9244
    GIM_Reject,
    // Label 310: @9245
    GIM_Try, /*On fail goto*//*Label 368*/ 9276, // Rule ID 3610 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK16RegClassID,
      // (or:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)  =>  (KORWrr:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KORWrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3610,
      GIR_Done,
    // Label 368: @9276
    GIM_Reject,
    // Label 311: @9277
    GIM_Try, /*On fail goto*//*Label 369*/ 9308, // Rule ID 3611 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK32RegClassID,
      // (or:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)  =>  (KORDrr:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KORDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3611,
      GIR_Done,
    // Label 369: @9308
    GIM_Reject,
    // Label 312: @9309
    GIM_Try, /*On fail goto*//*Label 370*/ 9340, // Rule ID 3612 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v64s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK64RegClassID,
      // (or:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)  =>  (KORQrr:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KORQrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3612,
      GIR_Done,
    // Label 370: @9340
    GIM_Reject,
    // Label 313: @9341
    GIM_Reject,
    // Label 5: @9342
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/0, 24, /*)*//*default:*//*Label 385*/ 11314,
    /*GILLT_s1*//*Label 371*/ 9372,
    /*GILLT_s8*//*Label 372*/ 9771,
    /*GILLT_s16*//*Label 373*/ 9859,
    /*GILLT_s32*//*Label 374*/ 9980,
    /*GILLT_s64*//*Label 375*/ 10353, 0, 0,
    /*GILLT_v2s1*//*Label 376*/ 10729,
    /*GILLT_v2s64*//*Label 377*/ 10822,
    /*GILLT_v4s1*//*Label 378*/ 10903, 0,
    /*GILLT_v4s64*//*Label 379*/ 10996,
    /*GILLT_v8s1*//*Label 380*/ 11077, 0, 0,
    /*GILLT_v8s64*//*Label 381*/ 11186,
    /*GILLT_v16s1*//*Label 382*/ 11218, 0, 0, 0,
    /*GILLT_v32s1*//*Label 383*/ 11250, 0, 0,
    /*GILLT_v64s1*//*Label 384*/ 11282,
    // Label 371: @9372
    GIM_Try, /*On fail goto*//*Label 386*/ 9770,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_Try, /*On fail goto*//*Label 387*/ 9487, // Rule ID 18370 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s1,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK1RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[v1i1] } (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, -1:{ *:[v1i1] }), VK1:{ *:[v1i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KXNORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXNORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 18370,
        GIR_Done,
      // Label 387: @9487
      GIM_Try, /*On fail goto*//*Label 388*/ 9588, // Rule ID 13782 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s1,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/X86::VK1RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[v1i1] } (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, VK1:{ *:[v1i1] }:$src2), -1:{ *:[v1i1] })  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KXNORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/1, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXNORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 13782,
        GIR_Done,
      // Label 388: @9588
      GIM_Try, /*On fail goto*//*Label 389*/ 9689, // Rule ID 18371 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_XOR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s1,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src2, (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, -1:{ *:[v1i1] }))  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KXNORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/1, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/1, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXNORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 18371,
        GIR_Done,
      // Label 389: @9689
      GIM_Try, /*On fail goto*//*Label 390*/ 9769, // Rule ID 13786 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK1RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK1RegClassID,
        // (xor:{ *:[v1i1] } VK1:{ *:[v1i1] }:$src1, VK1:{ *:[v1i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v1i1] } (KXORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK1:{ *:[v1i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
        // GIR_Coverage, 13786,
        GIR_Done,
      // Label 390: @9769
      GIM_Reject,
    // Label 386: @9770
    GIM_Reject,
    // Label 372: @9771
    GIM_Try, /*On fail goto*//*Label 391*/ 9858,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 392*/ 9811, // Rule ID 152 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i8] } GR8:{ *:[i8] }:$src1, -1:{ *:[i8] })  =>  (NOT8r:{ *:[i8] } GR8:{ *:[i8] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::NOT8r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 152,
        GIR_Done,
      // Label 392: @9811
      GIM_Try, /*On fail goto*//*Label 393*/ 9841, // Rule ID 16165 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (XOR8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16165,
        GIR_Done,
      // Label 393: @9841
      GIM_Try, /*On fail goto*//*Label 394*/ 9857, // Rule ID 16157 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR8RegClassID,
        // (xor:{ *:[i8] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (XOR8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::XOR8rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16157,
        GIR_Done,
      // Label 394: @9857
      GIM_Reject,
    // Label 391: @9858
    GIM_Reject,
    // Label 373: @9859
    GIM_Try, /*On fail goto*//*Label 395*/ 9979,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 396*/ 9899, // Rule ID 153 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i16] } GR16:{ *:[i16] }:$src1, -1:{ *:[i16] })  =>  (NOT16r:{ *:[i16] } GR16:{ *:[i16] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::NOT16r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 153,
        GIR_Done,
      // Label 396: @9899
      GIM_Try, /*On fail goto*//*Label 397*/ 9932, // Rule ID 16168 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i16immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)  =>  (XOR16ri8:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] })<<P:Predicate_i16immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR16ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16168,
        GIR_Done,
      // Label 397: @9932
      GIM_Try, /*On fail goto*//*Label 398*/ 9962, // Rule ID 16166 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)  =>  (XOR16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i16] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16166,
        GIR_Done,
      // Label 398: @9962
      GIM_Try, /*On fail goto*//*Label 399*/ 9978, // Rule ID 16158 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR16RegClassID,
        // (xor:{ *:[i16] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (XOR16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::XOR16rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16158,
        GIR_Done,
      // Label 399: @9978
      GIM_Reject,
    // Label 395: @9979
    GIM_Reject,
    // Label 374: @9980
    GIM_Try, /*On fail goto*//*Label 400*/ 10352,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 401*/ 10040, // Rule ID 18226 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLCMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18226,
        GIR_Done,
      // Label 401: @10040
      GIM_Try, /*On fail goto*//*Label 402*/ 10094, // Rule ID 18730 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }), GR32:{ *:[i32] }:$src)  =>  (BLSMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18730,
        GIR_Done,
      // Label 402: @10094
      GIM_Try, /*On fail goto*//*Label 403*/ 10148, // Rule ID 12271 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, 1:{ *:[i32] }))  =>  (BLCMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12271,
        GIR_Done,
      // Label 403: @10148
      GIM_Try, /*On fail goto*//*Label 404*/ 10202, // Rule ID 16453 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src, (add:{ *:[i32] } GR32:{ *:[i32] }:$src, -1:{ *:[i32] }))  =>  (BLSMSK32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSMSK32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16453,
        GIR_Done,
      // Label 404: @10202
      GIM_Try, /*On fail goto*//*Label 405*/ 10236, // Rule ID 154 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, -1:{ *:[i32] })  =>  (NOT32r:{ *:[i32] } GR32:{ *:[i32] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::NOT32r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 154,
        GIR_Done,
      // Label 405: @10236
      GIM_Try, /*On fail goto*//*Label 406*/ 10281, // Rule ID 16169 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)  =>  (XOR32ri8:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR32ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16169,
        GIR_Done,
      // Label 406: @10281
      GIM_Try, /*On fail goto*//*Label 407*/ 10323, // Rule ID 16167 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)  =>  (XOR32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16167,
        GIR_Done,
      // Label 407: @10323
      GIM_Try, /*On fail goto*//*Label 408*/ 10351, // Rule ID 16159 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (xor:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (XOR32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::XOR32rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16159,
        GIR_Done,
      // Label 408: @10351
      GIM_Reject,
    // Label 400: @10352
    GIM_Reject,
    // Label 375: @10353
    GIM_Try, /*On fail goto*//*Label 409*/ 10728,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 410*/ 10413, // Rule ID 18227 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLCMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18227,
        GIR_Done,
      // Label 410: @10413
      GIM_Try, /*On fail goto*//*Label 411*/ 10467, // Rule ID 18731 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        // MIs[0] src
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/2, /*OtherMI*/1, /*OtherOpIdx*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }), GR64:{ *:[i64] }:$src)  =>  (BLSMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 18731,
        GIR_Done,
      // Label 411: @10467
      GIM_Try, /*On fail goto*//*Label 412*/ 10521, // Rule ID 12272 //
        GIM_CheckFeatures, GIFBS_HasTBM,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, 1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, 1:{ *:[i64] }))  =>  (BLCMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLCMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12272,
        GIR_Done,
      // Label 412: @10521
      GIM_Try, /*On fail goto*//*Label 413*/ 10575, // Rule ID 16454 //
        GIM_CheckFeatures, GIFBS_HasBMI,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        // MIs[1] src
        GIM_CheckIsSameOperand, /*MI*/1, /*OpIdx*/1, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckConstantInt, /*MI*/1, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src, (add:{ *:[i64] } GR64:{ *:[i64] }:$src, -1:{ *:[i64] }))  =>  (BLSMSK64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BLSMSK64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16454,
        GIR_Done,
      // Label 413: @10575
      GIM_Try, /*On fail goto*//*Label 414*/ 10609, // Rule ID 155 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, -1:{ *:[i64] })  =>  (NOT64r:{ *:[i64] } GR64:{ *:[i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::NOT64r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 155,
        GIR_Done,
      // Label 414: @10609
      GIM_Try, /*On fail goto*//*Label 415*/ 10654, // Rule ID 16170 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)  =>  (XOR64ri8:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR64ri8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16170,
        GIR_Done,
      // Label 415: @10654
      GIM_Try, /*On fail goto*//*Label 416*/ 10699, // Rule ID 16171 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)  =>  (XOR64ri32:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XOR64ri32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16171,
        GIR_Done,
      // Label 416: @10699
      GIM_Try, /*On fail goto*//*Label 417*/ 10727, // Rule ID 16160 //
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (xor:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (XOR64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::XOR64rr,
        GIR_AddImplicitDef, /*InsnID*/0, X86::EFLAGS,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16160,
        GIR_Done,
      // Label 417: @10727
      GIM_Reject,
    // Label 409: @10728
    GIM_Reject,
    // Label 376: @10729
    GIM_Try, /*On fail goto*//*Label 418*/ 10821, // Rule ID 13787 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK2RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK2RegClassID,
      // (xor:{ *:[v2i1] } VK2:{ *:[v2i1] }:$src1, VK2:{ *:[v2i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v2i1] } (KXORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK2:{ *:[v2i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXORWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13787,
      GIR_Done,
    // Label 418: @10821
    GIM_Reject,
    // Label 377: @10822
    GIM_Try, /*On fail goto*//*Label 419*/ 10902,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 420*/ 10855, // Rule ID 1557 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (xor:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPXORrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPXORrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1557,
        GIR_Done,
      // Label 420: @10855
      GIM_Try, /*On fail goto*//*Label 421*/ 10878, // Rule ID 1559 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (xor:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (PXORrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::PXORrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1559,
        GIR_Done,
      // Label 421: @10878
      GIM_Try, /*On fail goto*//*Label 422*/ 10901, // Rule ID 4986 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (xor:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPXORQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPXORQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4986,
        GIR_Done,
      // Label 422: @10901
      GIM_Reject,
    // Label 419: @10902
    GIM_Reject,
    // Label 378: @10903
    GIM_Try, /*On fail goto*//*Label 423*/ 10995, // Rule ID 13788 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK1RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK4RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK4RegClassID,
      // (xor:{ *:[v4i1] } VK4:{ *:[v4i1] }:$src1, VK4:{ *:[v4i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v4i1] } (KXORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK4:{ *:[v4i1] }:$src2, VK16:{ *:[i32] })), VK1:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXORWrr,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK1*/8,
      // GIR_Coverage, 13788,
      GIR_Done,
    // Label 423: @10995
    GIM_Reject,
    // Label 379: @10996
    GIM_Try, /*On fail goto*//*Label 424*/ 11076,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 425*/ 11029, // Rule ID 1561 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (xor:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPXORYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPXORYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1561,
        GIR_Done,
      // Label 425: @11029
      GIM_Try, /*On fail goto*//*Label 426*/ 11052, // Rule ID 4977 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (xor:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPXORQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPXORQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4977,
        GIR_Done,
      // Label 426: @11052
      GIM_Try, /*On fail goto*//*Label 427*/ 11075, // Rule ID 12546 //
        GIM_CheckFeatures, GIFBS_HasAVX1Only,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (xor:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VXORPSYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VXORPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12546,
        GIR_Done,
      // Label 427: @11075
      GIM_Reject,
    // Label 424: @11076
    GIM_Reject,
    // Label 380: @11077
    GIM_Try, /*On fail goto*//*Label 428*/ 11185,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK8RegClassID,
      GIM_Try, /*On fail goto*//*Label 429*/ 11110, // Rule ID 3617 //
        GIM_CheckFeatures, GIFBS_HasDQI,
        // (xor:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (KXORBrr:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KXORBrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3617,
        GIR_Done,
      // Label 429: @11110
      GIM_Try, /*On fail goto*//*Label 430*/ 11184, // Rule ID 13785 //
        GIM_CheckFeatures, GIFBS_NoDQI,
        // (xor:{ *:[v8i1] } VK8:{ *:[v8i1] }:$src1, VK8:{ *:[v8i1] }:$src2)  =>  (COPY_TO_REGCLASS:{ *:[v8i1] } (KXORWrr:{ *:[v16i1] } (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src1, VK16:{ *:[i32] }), (COPY_TO_REGCLASS:{ *:[v16i1] } VK8:{ *:[v8i1] }:$src2, VK16:{ *:[i32] })), VK8:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s1,
        GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s1,
        GIR_BuildMI, /*InsnID*/3, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/2, // src2
        GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
        GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::KXORWrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/2, /*TempRegFlags*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK8*/12,
        // GIR_Coverage, 13785,
        GIR_Done,
      // Label 430: @11184
      GIM_Reject,
    // Label 428: @11185
    GIM_Reject,
    // Label 381: @11186
    GIM_Try, /*On fail goto*//*Label 431*/ 11217, // Rule ID 4968 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (xor:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPXORQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPXORQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 4968,
      GIR_Done,
    // Label 431: @11217
    GIM_Reject,
    // Label 382: @11218
    GIM_Try, /*On fail goto*//*Label 432*/ 11249, // Rule ID 3618 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK16RegClassID,
      // (xor:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)  =>  (KXORWrr:{ *:[v16i1] } VK16:{ *:[v16i1] }:$src1, VK16:{ *:[v16i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KXORWrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3618,
      GIR_Done,
    // Label 432: @11249
    GIM_Reject,
    // Label 383: @11250
    GIM_Try, /*On fail goto*//*Label 433*/ 11281, // Rule ID 3619 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK32RegClassID,
      // (xor:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)  =>  (KXORDrr:{ *:[v32i1] } VK32:{ *:[v32i1] }:$src1, VK32:{ *:[v32i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KXORDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3619,
      GIR_Done,
    // Label 433: @11281
    GIM_Reject,
    // Label 384: @11282
    GIM_Try, /*On fail goto*//*Label 434*/ 11313, // Rule ID 3620 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s1,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v64s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VK64RegClassID,
      // (xor:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)  =>  (KXORQrr:{ *:[v64i1] } VK64:{ *:[v64i1] }:$src1, VK64:{ *:[v64i1] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::KXORQrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3620,
      GIR_Done,
    // Label 434: @11313
    GIM_Reject,
    // Label 385: @11314
    GIM_Reject,
    // Label 6: @11315
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 25, /*)*//*default:*//*Label 451*/ 14607,
    /*GILLT_s32*//*Label 435*/ 11343,
    /*GILLT_s64*//*Label 436*/ 11505, 0, 0, 0,
    /*GILLT_v2s64*//*Label 437*/ 11667, 0,
    /*GILLT_v4s32*//*Label 438*/ 11988,
    /*GILLT_v4s64*//*Label 439*/ 12309, 0,
    /*GILLT_v8s16*//*Label 440*/ 12630,
    /*GILLT_v8s32*//*Label 441*/ 12791,
    /*GILLT_v8s64*//*Label 442*/ 13112, 0,
    /*GILLT_v16s8*//*Label 443*/ 13433,
    /*GILLT_v16s16*//*Label 444*/ 13594,
    /*GILLT_v16s32*//*Label 445*/ 13755,
    /*GILLT_v32s1*//*Label 446*/ 14076,
    /*GILLT_v32s8*//*Label 447*/ 14100,
    /*GILLT_v32s16*//*Label 448*/ 14261,
    /*GILLT_v64s1*//*Label 449*/ 14422,
    /*GILLT_v64s8*//*Label 450*/ 14446,
    // Label 435: @11343
    GIM_Try, /*On fail goto*//*Label 452*/ 11366, // Rule ID 2198 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (bitconvert:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (VMOVDI2SSrr:{ *:[f32] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVDI2SSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2198,
      GIR_Done,
    // Label 452: @11366
    GIM_Try, /*On fail goto*//*Label 453*/ 11389, // Rule ID 2200 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (bitconvert:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (MOVDI2SSrr:{ *:[f32] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVDI2SSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2200,
      GIR_Done,
    // Label 453: @11389
    GIM_Try, /*On fail goto*//*Label 454*/ 11412, // Rule ID 2214 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (bitconvert:{ *:[i32] } FR32:{ *:[f32] }:$src)  =>  (VMOVSS2DIrr:{ *:[i32] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVSS2DIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2214,
      GIR_Done,
    // Label 454: @11412
    GIM_Try, /*On fail goto*//*Label 455*/ 11435, // Rule ID 2216 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (bitconvert:{ *:[i32] } FR32:{ *:[f32] }:$src)  =>  (MOVSS2DIrr:{ *:[i32] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSS2DIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2216,
      GIR_Done,
    // Label 455: @11435
    GIM_Try, /*On fail goto*//*Label 456*/ 11458, // Rule ID 3790 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (bitconvert:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (VMOVDI2SSZrr:{ *:[f32] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVDI2SSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3790,
      GIR_Done,
    // Label 456: @11458
    GIM_Try, /*On fail goto*//*Label 457*/ 11481, // Rule ID 3796 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
      // (bitconvert:{ *:[i32] } FR32X:{ *:[f32] }:$src)  =>  (VMOVSS2DIZrr:{ *:[i32] } FR32X:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVSS2DIZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3796,
      GIR_Done,
    // Label 457: @11481
    GIM_Try, /*On fail goto*//*Label 458*/ 11504, // Rule ID 13743 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      // (bitconvert:{ *:[i32] } VK32:{ *:[v32i1] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[i32] } VK32:{ *:[v32i1] }:$src, GR32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR32*/27,
      // GIR_Coverage, 13743,
      GIR_Done,
    // Label 458: @11504
    GIM_Reject,
    // Label 436: @11505
    GIM_Try, /*On fail goto*//*Label 459*/ 11528, // Rule ID 2193 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (bitconvert:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (VMOV64toSDrr:{ *:[f64] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOV64toSDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2193,
      GIR_Done,
    // Label 459: @11528
    GIM_Try, /*On fail goto*//*Label 460*/ 11551, // Rule ID 2197 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (bitconvert:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (MOV64toSDrr:{ *:[f64] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOV64toSDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2197,
      GIR_Done,
    // Label 460: @11551
    GIM_Try, /*On fail goto*//*Label 461*/ 11574, // Rule ID 2209 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (bitconvert:{ *:[i64] } FR64:{ *:[f64] }:$src)  =>  (VMOVSDto64rr:{ *:[i64] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVSDto64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2209,
      GIR_Done,
    // Label 461: @11574
    GIM_Try, /*On fail goto*//*Label 462*/ 11597, // Rule ID 2212 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (bitconvert:{ *:[i64] } FR64:{ *:[f64] }:$src)  =>  (MOVSDto64rr:{ *:[i64] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSDto64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2212,
      GIR_Done,
    // Label 462: @11597
    GIM_Try, /*On fail goto*//*Label 463*/ 11620, // Rule ID 3786 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (bitconvert:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (VMOV64toSDZrr:{ *:[f64] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOV64toSDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3786,
      GIR_Done,
    // Label 463: @11620
    GIM_Try, /*On fail goto*//*Label 464*/ 11643, // Rule ID 3788 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (bitconvert:{ *:[i64] } FR64X:{ *:[f64] }:$src)  =>  (VMOVSDto64Zrr:{ *:[i64] } FR64X:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMOVSDto64Zrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 3788,
      GIR_Done,
    // Label 464: @11643
    GIM_Try, /*On fail goto*//*Label 465*/ 11666, // Rule ID 13745 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK64RegClassID,
      // (bitconvert:{ *:[i64] } VK64:{ *:[v64i1] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[i64] } VK64:{ *:[v64i1] }:$src, GR64:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR64*/45,
      // GIR_Coverage, 13745,
      GIR_Done,
    // Label 465: @11666
    GIM_Reject,
    // Label 437: @11667
    GIM_Try, /*On fail goto*//*Label 466*/ 11699, // Rule ID 16198 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src)  =>  VR128:{ *:[v2i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16198,
      GIR_Done,
    // Label 466: @11699
    GIM_Try, /*On fail goto*//*Label 467*/ 11731, // Rule ID 16199 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2i64] } VR128:{ *:[v8i16] }:$src)  =>  VR128:{ *:[v2i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16199,
      GIR_Done,
    // Label 467: @11731
    GIM_Try, /*On fail goto*//*Label 468*/ 11763, // Rule ID 16200 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2i64] } VR128:{ *:[v16i8] }:$src)  =>  VR128:{ *:[v2i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16200,
      GIR_Done,
    // Label 468: @11763
    GIM_Try, /*On fail goto*//*Label 469*/ 11795, // Rule ID 16201 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2i64] } VR128:{ *:[v2f64] }:$src)  =>  VR128:{ *:[v2i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16201,
      GIR_Done,
    // Label 469: @11795
    GIM_Try, /*On fail goto*//*Label 470*/ 11827, // Rule ID 16202 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2i64] } VR128:{ *:[v4f32] }:$src)  =>  VR128:{ *:[v2i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16202,
      GIR_Done,
    // Label 470: @11827
    GIM_Try, /*On fail goto*//*Label 471*/ 11859, // Rule ID 16223 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2f64] } VR128:{ *:[v2i64] }:$src)  =>  VR128:{ *:[v2f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16223,
      GIR_Done,
    // Label 471: @11859
    GIM_Try, /*On fail goto*//*Label 472*/ 11891, // Rule ID 16224 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2f64] } VR128:{ *:[v4i32] }:$src)  =>  VR128:{ *:[v2f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16224,
      GIR_Done,
    // Label 472: @11891
    GIM_Try, /*On fail goto*//*Label 473*/ 11923, // Rule ID 16225 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2f64] } VR128:{ *:[v8i16] }:$src)  =>  VR128:{ *:[v2f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16225,
      GIR_Done,
    // Label 473: @11923
    GIM_Try, /*On fail goto*//*Label 474*/ 11955, // Rule ID 16226 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2f64] } VR128:{ *:[v16i8] }:$src)  =>  VR128:{ *:[v2f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16226,
      GIR_Done,
    // Label 474: @11955
    GIM_Try, /*On fail goto*//*Label 475*/ 11987, // Rule ID 16227 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v2f64] } VR128:{ *:[v4f32] }:$src)  =>  VR128:{ *:[v2f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16227,
      GIR_Done,
    // Label 475: @11987
    GIM_Reject,
    // Label 438: @11988
    GIM_Try, /*On fail goto*//*Label 476*/ 12020, // Rule ID 16203 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4i32] } VR128:{ *:[v2i64] }:$src)  =>  VR128:{ *:[v4i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16203,
      GIR_Done,
    // Label 476: @12020
    GIM_Try, /*On fail goto*//*Label 477*/ 12052, // Rule ID 16204 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src)  =>  VR128:{ *:[v4i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16204,
      GIR_Done,
    // Label 477: @12052
    GIM_Try, /*On fail goto*//*Label 478*/ 12084, // Rule ID 16205 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4i32] } VR128:{ *:[v16i8] }:$src)  =>  VR128:{ *:[v4i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16205,
      GIR_Done,
    // Label 478: @12084
    GIM_Try, /*On fail goto*//*Label 479*/ 12116, // Rule ID 16206 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4i32] } VR128:{ *:[v2f64] }:$src)  =>  VR128:{ *:[v4i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16206,
      GIR_Done,
    // Label 479: @12116
    GIM_Try, /*On fail goto*//*Label 480*/ 12148, // Rule ID 16207 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4i32] } VR128:{ *:[v4f32] }:$src)  =>  VR128:{ *:[v4i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16207,
      GIR_Done,
    // Label 480: @12148
    GIM_Try, /*On fail goto*//*Label 481*/ 12180, // Rule ID 16218 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4f32] } VR128:{ *:[v2i64] }:$src)  =>  VR128:{ *:[v4f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16218,
      GIR_Done,
    // Label 481: @12180
    GIM_Try, /*On fail goto*//*Label 482*/ 12212, // Rule ID 16219 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4f32] } VR128:{ *:[v4i32] }:$src)  =>  VR128:{ *:[v4f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16219,
      GIR_Done,
    // Label 482: @12212
    GIM_Try, /*On fail goto*//*Label 483*/ 12244, // Rule ID 16220 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4f32] } VR128:{ *:[v8i16] }:$src)  =>  VR128:{ *:[v4f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16220,
      GIR_Done,
    // Label 483: @12244
    GIM_Try, /*On fail goto*//*Label 484*/ 12276, // Rule ID 16221 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4f32] } VR128:{ *:[v16i8] }:$src)  =>  VR128:{ *:[v4f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16221,
      GIR_Done,
    // Label 484: @12276
    GIM_Try, /*On fail goto*//*Label 485*/ 12308, // Rule ID 16222 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v4f32] } VR128:{ *:[v2f64] }:$src)  =>  VR128:{ *:[v4f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16222,
      GIR_Done,
    // Label 485: @12308
    GIM_Reject,
    // Label 439: @12309
    GIM_Try, /*On fail goto*//*Label 486*/ 12341, // Rule ID 16228 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4i64] } VR256:{ *:[v8i32] }:$src)  =>  VR256:{ *:[v4i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16228,
      GIR_Done,
    // Label 486: @12341
    GIM_Try, /*On fail goto*//*Label 487*/ 12373, // Rule ID 16229 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4i64] } VR256:{ *:[v16i16] }:$src)  =>  VR256:{ *:[v4i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16229,
      GIR_Done,
    // Label 487: @12373
    GIM_Try, /*On fail goto*//*Label 488*/ 12405, // Rule ID 16230 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4i64] } VR256:{ *:[v32i8] }:$src)  =>  VR256:{ *:[v4i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16230,
      GIR_Done,
    // Label 488: @12405
    GIM_Try, /*On fail goto*//*Label 489*/ 12437, // Rule ID 16231 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4i64] } VR256:{ *:[v8f32] }:$src)  =>  VR256:{ *:[v4i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16231,
      GIR_Done,
    // Label 489: @12437
    GIM_Try, /*On fail goto*//*Label 490*/ 12469, // Rule ID 16232 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4i64] } VR256:{ *:[v4f64] }:$src)  =>  VR256:{ *:[v4i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16232,
      GIR_Done,
    // Label 490: @12469
    GIM_Try, /*On fail goto*//*Label 491*/ 12501, // Rule ID 16253 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4f64] } VR256:{ *:[v4i64] }:$src)  =>  VR256:{ *:[v4f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16253,
      GIR_Done,
    // Label 491: @12501
    GIM_Try, /*On fail goto*//*Label 492*/ 12533, // Rule ID 16254 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4f64] } VR256:{ *:[v8i32] }:$src)  =>  VR256:{ *:[v4f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16254,
      GIR_Done,
    // Label 492: @12533
    GIM_Try, /*On fail goto*//*Label 493*/ 12565, // Rule ID 16255 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4f64] } VR256:{ *:[v16i16] }:$src)  =>  VR256:{ *:[v4f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16255,
      GIR_Done,
    // Label 493: @12565
    GIM_Try, /*On fail goto*//*Label 494*/ 12597, // Rule ID 16256 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4f64] } VR256:{ *:[v32i8] }:$src)  =>  VR256:{ *:[v4f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16256,
      GIR_Done,
    // Label 494: @12597
    GIM_Try, /*On fail goto*//*Label 495*/ 12629, // Rule ID 16257 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v4f64] } VR256:{ *:[v8f32] }:$src)  =>  VR256:{ *:[v4f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16257,
      GIR_Done,
    // Label 495: @12629
    GIM_Reject,
    // Label 440: @12630
    GIM_Try, /*On fail goto*//*Label 496*/ 12662, // Rule ID 16208 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v8i16] } VR128:{ *:[v2i64] }:$src)  =>  VR128:{ *:[v8i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16208,
      GIR_Done,
    // Label 496: @12662
    GIM_Try, /*On fail goto*//*Label 497*/ 12694, // Rule ID 16209 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v8i16] } VR128:{ *:[v4i32] }:$src)  =>  VR128:{ *:[v8i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16209,
      GIR_Done,
    // Label 497: @12694
    GIM_Try, /*On fail goto*//*Label 498*/ 12726, // Rule ID 16210 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src)  =>  VR128:{ *:[v8i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16210,
      GIR_Done,
    // Label 498: @12726
    GIM_Try, /*On fail goto*//*Label 499*/ 12758, // Rule ID 16211 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v8i16] } VR128:{ *:[v2f64] }:$src)  =>  VR128:{ *:[v8i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16211,
      GIR_Done,
    // Label 499: @12758
    GIM_Try, /*On fail goto*//*Label 500*/ 12790, // Rule ID 16212 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v8i16] } VR128:{ *:[v4f32] }:$src)  =>  VR128:{ *:[v8i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16212,
      GIR_Done,
    // Label 500: @12790
    GIM_Reject,
    // Label 441: @12791
    GIM_Try, /*On fail goto*//*Label 501*/ 12823, // Rule ID 16233 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8i32] } VR256:{ *:[v4i64] }:$src)  =>  VR256:{ *:[v8i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16233,
      GIR_Done,
    // Label 501: @12823
    GIM_Try, /*On fail goto*//*Label 502*/ 12855, // Rule ID 16234 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8i32] } VR256:{ *:[v16i16] }:$src)  =>  VR256:{ *:[v8i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16234,
      GIR_Done,
    // Label 502: @12855
    GIM_Try, /*On fail goto*//*Label 503*/ 12887, // Rule ID 16235 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8i32] } VR256:{ *:[v32i8] }:$src)  =>  VR256:{ *:[v8i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16235,
      GIR_Done,
    // Label 503: @12887
    GIM_Try, /*On fail goto*//*Label 504*/ 12919, // Rule ID 16236 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8i32] } VR256:{ *:[v4f64] }:$src)  =>  VR256:{ *:[v8i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16236,
      GIR_Done,
    // Label 504: @12919
    GIM_Try, /*On fail goto*//*Label 505*/ 12951, // Rule ID 16237 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8i32] } VR256:{ *:[v8f32] }:$src)  =>  VR256:{ *:[v8i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16237,
      GIR_Done,
    // Label 505: @12951
    GIM_Try, /*On fail goto*//*Label 506*/ 12983, // Rule ID 16248 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8f32] } VR256:{ *:[v4i64] }:$src)  =>  VR256:{ *:[v8f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16248,
      GIR_Done,
    // Label 506: @12983
    GIM_Try, /*On fail goto*//*Label 507*/ 13015, // Rule ID 16249 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8f32] } VR256:{ *:[v8i32] }:$src)  =>  VR256:{ *:[v8f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16249,
      GIR_Done,
    // Label 507: @13015
    GIM_Try, /*On fail goto*//*Label 508*/ 13047, // Rule ID 16250 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8f32] } VR256:{ *:[v16i16] }:$src)  =>  VR256:{ *:[v8f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16250,
      GIR_Done,
    // Label 508: @13047
    GIM_Try, /*On fail goto*//*Label 509*/ 13079, // Rule ID 16251 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8f32] } VR256:{ *:[v32i8] }:$src)  =>  VR256:{ *:[v8f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16251,
      GIR_Done,
    // Label 509: @13079
    GIM_Try, /*On fail goto*//*Label 510*/ 13111, // Rule ID 16252 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v8f32] } VR256:{ *:[v4f64] }:$src)  =>  VR256:{ *:[v8f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16252,
      GIR_Done,
    // Label 510: @13111
    GIM_Reject,
    // Label 442: @13112
    GIM_Try, /*On fail goto*//*Label 511*/ 13144, // Rule ID 16258 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8f64] } VR512:{ *:[v8i64] }:$src)  =>  VR512:{ *:[v8f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16258,
      GIR_Done,
    // Label 511: @13144
    GIM_Try, /*On fail goto*//*Label 512*/ 13176, // Rule ID 16259 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8f64] } VR512:{ *:[v16i32] }:$src)  =>  VR512:{ *:[v8f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16259,
      GIR_Done,
    // Label 512: @13176
    GIM_Try, /*On fail goto*//*Label 513*/ 13208, // Rule ID 16260 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8f64] } VR512:{ *:[v32i16] }:$src)  =>  VR512:{ *:[v8f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16260,
      GIR_Done,
    // Label 513: @13208
    GIM_Try, /*On fail goto*//*Label 514*/ 13240, // Rule ID 16261 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8f64] } VR512:{ *:[v64i8] }:$src)  =>  VR512:{ *:[v8f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16261,
      GIR_Done,
    // Label 514: @13240
    GIM_Try, /*On fail goto*//*Label 515*/ 13272, // Rule ID 16262 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8f64] } VR512:{ *:[v16f32] }:$src)  =>  VR512:{ *:[v8f64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16262,
      GIR_Done,
    // Label 515: @13272
    GIM_Try, /*On fail goto*//*Label 516*/ 13304, // Rule ID 16268 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8i64] } VR512:{ *:[v16i32] }:$src)  =>  VR512:{ *:[v8i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16268,
      GIR_Done,
    // Label 516: @13304
    GIM_Try, /*On fail goto*//*Label 517*/ 13336, // Rule ID 16269 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8i64] } VR512:{ *:[v32i16] }:$src)  =>  VR512:{ *:[v8i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16269,
      GIR_Done,
    // Label 517: @13336
    GIM_Try, /*On fail goto*//*Label 518*/ 13368, // Rule ID 16270 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8i64] } VR512:{ *:[v64i8] }:$src)  =>  VR512:{ *:[v8i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16270,
      GIR_Done,
    // Label 518: @13368
    GIM_Try, /*On fail goto*//*Label 519*/ 13400, // Rule ID 16271 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8i64] } VR512:{ *:[v8f64] }:$src)  =>  VR512:{ *:[v8i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16271,
      GIR_Done,
    // Label 519: @13400
    GIM_Try, /*On fail goto*//*Label 520*/ 13432, // Rule ID 16272 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v8i64] } VR512:{ *:[v16f32] }:$src)  =>  VR512:{ *:[v8i64] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16272,
      GIR_Done,
    // Label 520: @13432
    GIM_Reject,
    // Label 443: @13433
    GIM_Try, /*On fail goto*//*Label 521*/ 13465, // Rule ID 16213 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v16i8] } VR128:{ *:[v2i64] }:$src)  =>  VR128:{ *:[v16i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16213,
      GIR_Done,
    // Label 521: @13465
    GIM_Try, /*On fail goto*//*Label 522*/ 13497, // Rule ID 16214 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v16i8] } VR128:{ *:[v4i32] }:$src)  =>  VR128:{ *:[v16i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16214,
      GIR_Done,
    // Label 522: @13497
    GIM_Try, /*On fail goto*//*Label 523*/ 13529, // Rule ID 16215 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v16i8] } VR128:{ *:[v8i16] }:$src)  =>  VR128:{ *:[v16i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16215,
      GIR_Done,
    // Label 523: @13529
    GIM_Try, /*On fail goto*//*Label 524*/ 13561, // Rule ID 16216 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v16i8] } VR128:{ *:[v2f64] }:$src)  =>  VR128:{ *:[v16i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16216,
      GIR_Done,
    // Label 524: @13561
    GIM_Try, /*On fail goto*//*Label 525*/ 13593, // Rule ID 16217 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (bitconvert:{ *:[v16i8] } VR128:{ *:[v4f32] }:$src)  =>  VR128:{ *:[v16i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR128*/74,
      // GIR_Coverage, 16217,
      GIR_Done,
    // Label 525: @13593
    GIM_Reject,
    // Label 444: @13594
    GIM_Try, /*On fail goto*//*Label 526*/ 13626, // Rule ID 16238 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v16i16] } VR256:{ *:[v4i64] }:$src)  =>  VR256:{ *:[v16i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16238,
      GIR_Done,
    // Label 526: @13626
    GIM_Try, /*On fail goto*//*Label 527*/ 13658, // Rule ID 16239 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v16i16] } VR256:{ *:[v8i32] }:$src)  =>  VR256:{ *:[v16i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16239,
      GIR_Done,
    // Label 527: @13658
    GIM_Try, /*On fail goto*//*Label 528*/ 13690, // Rule ID 16240 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v16i16] } VR256:{ *:[v32i8] }:$src)  =>  VR256:{ *:[v16i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16240,
      GIR_Done,
    // Label 528: @13690
    GIM_Try, /*On fail goto*//*Label 529*/ 13722, // Rule ID 16241 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v16i16] } VR256:{ *:[v4f64] }:$src)  =>  VR256:{ *:[v16i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16241,
      GIR_Done,
    // Label 529: @13722
    GIM_Try, /*On fail goto*//*Label 530*/ 13754, // Rule ID 16242 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v16i16] } VR256:{ *:[v8f32] }:$src)  =>  VR256:{ *:[v16i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16242,
      GIR_Done,
    // Label 530: @13754
    GIM_Reject,
    // Label 445: @13755
    GIM_Try, /*On fail goto*//*Label 531*/ 13787, // Rule ID 16263 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16f32] } VR512:{ *:[v8i64] }:$src)  =>  VR512:{ *:[v16f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16263,
      GIR_Done,
    // Label 531: @13787
    GIM_Try, /*On fail goto*//*Label 532*/ 13819, // Rule ID 16264 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16f32] } VR512:{ *:[v16i32] }:$src)  =>  VR512:{ *:[v16f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16264,
      GIR_Done,
    // Label 532: @13819
    GIM_Try, /*On fail goto*//*Label 533*/ 13851, // Rule ID 16265 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16f32] } VR512:{ *:[v32i16] }:$src)  =>  VR512:{ *:[v16f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16265,
      GIR_Done,
    // Label 533: @13851
    GIM_Try, /*On fail goto*//*Label 534*/ 13883, // Rule ID 16266 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16f32] } VR512:{ *:[v64i8] }:$src)  =>  VR512:{ *:[v16f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16266,
      GIR_Done,
    // Label 534: @13883
    GIM_Try, /*On fail goto*//*Label 535*/ 13915, // Rule ID 16267 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16f32] } VR512:{ *:[v8f64] }:$src)  =>  VR512:{ *:[v16f32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16267,
      GIR_Done,
    // Label 535: @13915
    GIM_Try, /*On fail goto*//*Label 536*/ 13947, // Rule ID 16273 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16i32] } VR512:{ *:[v8i64] }:$src)  =>  VR512:{ *:[v16i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16273,
      GIR_Done,
    // Label 536: @13947
    GIM_Try, /*On fail goto*//*Label 537*/ 13979, // Rule ID 16274 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16i32] } VR512:{ *:[v16f32] }:$src)  =>  VR512:{ *:[v16i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16274,
      GIR_Done,
    // Label 537: @13979
    GIM_Try, /*On fail goto*//*Label 538*/ 14011, // Rule ID 16275 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16i32] } VR512:{ *:[v32i16] }:$src)  =>  VR512:{ *:[v16i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16275,
      GIR_Done,
    // Label 538: @14011
    GIM_Try, /*On fail goto*//*Label 539*/ 14043, // Rule ID 16276 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16i32] } VR512:{ *:[v64i8] }:$src)  =>  VR512:{ *:[v16i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16276,
      GIR_Done,
    // Label 539: @14043
    GIM_Try, /*On fail goto*//*Label 540*/ 14075, // Rule ID 16277 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v16i32] } VR512:{ *:[v8f64] }:$src)  =>  VR512:{ *:[v16i32] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16277,
      GIR_Done,
    // Label 540: @14075
    GIM_Reject,
    // Label 446: @14076
    GIM_Try, /*On fail goto*//*Label 541*/ 14099, // Rule ID 13742 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (bitconvert:{ *:[v32i1] } GR32:{ *:[i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v32i1] } GR32:{ *:[i32] }:$src, VK32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK32*/31,
      // GIR_Coverage, 13742,
      GIR_Done,
    // Label 541: @14099
    GIM_Reject,
    // Label 447: @14100
    GIM_Try, /*On fail goto*//*Label 542*/ 14132, // Rule ID 16243 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v32i8] } VR256:{ *:[v4i64] }:$src)  =>  VR256:{ *:[v32i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16243,
      GIR_Done,
    // Label 542: @14132
    GIM_Try, /*On fail goto*//*Label 543*/ 14164, // Rule ID 16244 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v32i8] } VR256:{ *:[v8i32] }:$src)  =>  VR256:{ *:[v32i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16244,
      GIR_Done,
    // Label 543: @14164
    GIM_Try, /*On fail goto*//*Label 544*/ 14196, // Rule ID 16245 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v32i8] } VR256:{ *:[v16i16] }:$src)  =>  VR256:{ *:[v32i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16245,
      GIR_Done,
    // Label 544: @14196
    GIM_Try, /*On fail goto*//*Label 545*/ 14228, // Rule ID 16246 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v32i8] } VR256:{ *:[v4f64] }:$src)  =>  VR256:{ *:[v32i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16246,
      GIR_Done,
    // Label 545: @14228
    GIM_Try, /*On fail goto*//*Label 546*/ 14260, // Rule ID 16247 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (bitconvert:{ *:[v32i8] } VR256:{ *:[v8f32] }:$src)  =>  VR256:{ *:[v32i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR256*/79,
      // GIR_Coverage, 16247,
      GIR_Done,
    // Label 546: @14260
    GIM_Reject,
    // Label 448: @14261
    GIM_Try, /*On fail goto*//*Label 547*/ 14293, // Rule ID 16278 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v32i16] } VR512:{ *:[v8i64] }:$src)  =>  VR512:{ *:[v32i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16278,
      GIR_Done,
    // Label 547: @14293
    GIM_Try, /*On fail goto*//*Label 548*/ 14325, // Rule ID 16279 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v32i16] } VR512:{ *:[v16i32] }:$src)  =>  VR512:{ *:[v32i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16279,
      GIR_Done,
    // Label 548: @14325
    GIM_Try, /*On fail goto*//*Label 549*/ 14357, // Rule ID 16280 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v32i16] } VR512:{ *:[v64i8] }:$src)  =>  VR512:{ *:[v32i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16280,
      GIR_Done,
    // Label 549: @14357
    GIM_Try, /*On fail goto*//*Label 550*/ 14389, // Rule ID 16281 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v32i16] } VR512:{ *:[v8f64] }:$src)  =>  VR512:{ *:[v32i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16281,
      GIR_Done,
    // Label 550: @14389
    GIM_Try, /*On fail goto*//*Label 551*/ 14421, // Rule ID 16282 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v32i16] } VR512:{ *:[v16f32] }:$src)  =>  VR512:{ *:[v32i16] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16282,
      GIR_Done,
    // Label 551: @14421
    GIM_Reject,
    // Label 449: @14422
    GIM_Try, /*On fail goto*//*Label 552*/ 14445, // Rule ID 13744 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VK64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (bitconvert:{ *:[v64i1] } GR64:{ *:[i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v64i1] } GR64:{ *:[i64] }:$src, VK64:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VK64*/55,
      // GIR_Coverage, 13744,
      GIR_Done,
    // Label 552: @14445
    GIM_Reject,
    // Label 450: @14446
    GIM_Try, /*On fail goto*//*Label 553*/ 14478, // Rule ID 16283 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v64i8] } VR512:{ *:[v8i64] }:$src)  =>  VR512:{ *:[v64i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16283,
      GIR_Done,
    // Label 553: @14478
    GIM_Try, /*On fail goto*//*Label 554*/ 14510, // Rule ID 16284 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v64i8] } VR512:{ *:[v16i32] }:$src)  =>  VR512:{ *:[v64i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16284,
      GIR_Done,
    // Label 554: @14510
    GIM_Try, /*On fail goto*//*Label 555*/ 14542, // Rule ID 16285 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v64i8] } VR512:{ *:[v32i16] }:$src)  =>  VR512:{ *:[v64i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16285,
      GIR_Done,
    // Label 555: @14542
    GIM_Try, /*On fail goto*//*Label 556*/ 14574, // Rule ID 16286 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v64i8] } VR512:{ *:[v8f64] }:$src)  =>  VR512:{ *:[v64i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16286,
      GIR_Done,
    // Label 556: @14574
    GIM_Try, /*On fail goto*//*Label 557*/ 14606, // Rule ID 16287 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (bitconvert:{ *:[v64i8] } VR512:{ *:[v16f32] }:$src)  =>  VR512:{ *:[v64i8] }:$src
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC VR512*/82,
      // GIR_Coverage, 16287,
      GIR_Done,
    // Label 557: @14606
    GIM_Reject,
    // Label 451: @14607
    GIM_Reject,
    // Label 7: @14608
    GIM_Try, /*On fail goto*//*Label 558*/ 16670,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
      GIM_Try, /*On fail goto*//*Label 559*/ 14653, // Rule ID 1133 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphsubwd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6613:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src)  =>  (VPHSUBWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHSUBWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1133,
        GIR_Done,
      // Label 559: @14653
      GIM_Try, /*On fail goto*//*Label 560*/ 14693, // Rule ID 1135 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphsubdq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6612:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src)  =>  (VPHSUBDQrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHSUBDQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1135,
        GIR_Done,
      // Label 560: @14693
      GIM_Try, /*On fail goto*//*Label 561*/ 14733, // Rule ID 1137 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphsubbw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6611:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHSUBBWrr:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHSUBBWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1137,
        GIR_Done,
      // Label 561: @14733
      GIM_Try, /*On fail goto*//*Label 562*/ 14773, // Rule ID 1139 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddwq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6610:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src)  =>  (VPHADDWQrr:{ *:[v2i64] } VR128:{ *:[v8i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDWQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1139,
        GIR_Done,
      // Label 562: @14773
      GIM_Try, /*On fail goto*//*Label 563*/ 14813, // Rule ID 1141 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddwd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6609:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src)  =>  (VPHADDWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1141,
        GIR_Done,
      // Label 563: @14813
      GIM_Try, /*On fail goto*//*Label 564*/ 14853, // Rule ID 1143 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphadduwq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6608:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src)  =>  (VPHADDUWQrr:{ *:[v2i64] } VR128:{ *:[v8i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUWQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1143,
        GIR_Done,
      // Label 564: @14853
      GIM_Try, /*On fail goto*//*Label 565*/ 14893, // Rule ID 1145 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphadduwd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6607:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src)  =>  (VPHADDUWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1145,
        GIR_Done,
      // Label 565: @14893
      GIM_Try, /*On fail goto*//*Label 566*/ 14933, // Rule ID 1147 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddudq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6606:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src)  =>  (VPHADDUDQrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUDQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1147,
        GIR_Done,
      // Label 566: @14933
      GIM_Try, /*On fail goto*//*Label 567*/ 14973, // Rule ID 1149 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddubw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6605:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDUBWrr:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUBWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1149,
        GIR_Done,
      // Label 567: @14973
      GIM_Try, /*On fail goto*//*Label 568*/ 15013, // Rule ID 1151 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddubq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6604:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDUBQrr:{ *:[v2i64] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUBQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1151,
        GIR_Done,
      // Label 568: @15013
      GIM_Try, /*On fail goto*//*Label 569*/ 15053, // Rule ID 1153 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddubd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6603:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDUBDrr:{ *:[v4i32] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDUBDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1153,
        GIR_Done,
      // Label 569: @15053
      GIM_Try, /*On fail goto*//*Label 570*/ 15093, // Rule ID 1155 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphadddq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6602:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src)  =>  (VPHADDDQrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDDQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1155,
        GIR_Done,
      // Label 570: @15093
      GIM_Try, /*On fail goto*//*Label 571*/ 15133, // Rule ID 1157 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddbw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6601:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDBWrr:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDBWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1157,
        GIR_Done,
      // Label 571: @15133
      GIM_Try, /*On fail goto*//*Label 572*/ 15173, // Rule ID 1159 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddbq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6600:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDBQrr:{ *:[v2i64] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDBQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1159,
        GIR_Done,
      // Label 572: @15173
      GIM_Try, /*On fail goto*//*Label 573*/ 15213, // Rule ID 1161 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vphaddbd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6599:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src)  =>  (VPHADDBDrr:{ *:[v4i32] } VR128:{ *:[v16i8] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDBDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1161,
        GIR_Done,
      // Label 573: @15213
      GIM_Try, /*On fail goto*//*Label 574*/ 15253, // Rule ID 1163 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6586:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VFRCZSSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZSSrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1163,
        GIR_Done,
      // Label 574: @15253
      GIM_Try, /*On fail goto*//*Label 575*/ 15293, // Rule ID 1165 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_ps,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6583:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VFRCZPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZPSrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1165,
        GIR_Done,
      // Label 575: @15293
      GIM_Try, /*On fail goto*//*Label 576*/ 15333, // Rule ID 1167 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_ps_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v8f32] } 6584:{ *:[iPTR] }, VR256:{ *:[v8f32] }:$src)  =>  (VFRCZPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZPSYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1167,
        GIR_Done,
      // Label 576: @15333
      GIM_Try, /*On fail goto*//*Label 577*/ 15373, // Rule ID 1169 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_sd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6585:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VFRCZSDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZSDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1169,
        GIR_Done,
      // Label 577: @15373
      GIM_Try, /*On fail goto*//*Label 578*/ 15413, // Rule ID 1171 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_pd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6581:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VFRCZPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZPDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1171,
        GIR_Done,
      // Label 578: @15413
      GIM_Try, /*On fail goto*//*Label 579*/ 15453, // Rule ID 1173 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vfrcz_pd_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f64] } 6582:{ *:[iPTR] }, VR256:{ *:[v4f64] }:$src)  =>  (VFRCZPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VFRCZPDYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1173,
        GIR_Done,
      // Label 579: @15453
      GIM_Try, /*On fail goto*//*Label 580*/ 15493, // Rule ID 1363 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6418:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VCVTSD2SIrr_Int:{ *:[i32] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1363,
        GIR_Done,
      // Label 580: @15493
      GIM_Try, /*On fail goto*//*Label 581*/ 15533, // Rule ID 1365 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6419:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VCVTSD2SI64rr_Int:{ *:[i64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1365,
        GIR_Done,
      // Label 581: @15533
      GIM_Try, /*On fail goto*//*Label 582*/ 15573, // Rule ID 1367 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6418:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (CVTSD2SIrr_Int:{ *:[i32] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTSD2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1367,
        GIR_Done,
      // Label 582: @15573
      GIM_Try, /*On fail goto*//*Label 583*/ 15613, // Rule ID 1369 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6419:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (CVTSD2SI64rr_Int:{ *:[i64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTSD2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1369,
        GIR_Done,
      // Label 583: @15613
      GIM_Try, /*On fail goto*//*Label 584*/ 15653, // Rule ID 1371 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6385:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VCVTTSS2SIrr_Int:{ *:[i32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSS2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1371,
        GIR_Done,
      // Label 584: @15653
      GIM_Try, /*On fail goto*//*Label 585*/ 15693, // Rule ID 1373 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6386:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VCVTTSS2SI64rr_Int:{ *:[i64] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSS2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1373,
        GIR_Done,
      // Label 585: @15693
      GIM_Try, /*On fail goto*//*Label 586*/ 15733, // Rule ID 1375 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6423:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VCVTTSD2SIrr_Int:{ *:[i32] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSD2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1375,
        GIR_Done,
      // Label 586: @15733
      GIM_Try, /*On fail goto*//*Label 587*/ 15773, // Rule ID 1377 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6424:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (VCVTTSD2SI64rr_Int:{ *:[i64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSD2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1377,
        GIR_Done,
      // Label 587: @15773
      GIM_Try, /*On fail goto*//*Label 588*/ 15813, // Rule ID 1379 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6385:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (CVTTSS2SIrr_Int:{ *:[i32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTTSS2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1379,
        GIR_Done,
      // Label 588: @15813
      GIM_Try, /*On fail goto*//*Label 589*/ 15853, // Rule ID 1381 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6386:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (CVTTSS2SI64rr_Int:{ *:[i64] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTTSS2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1381,
        GIR_Done,
      // Label 589: @15853
      GIM_Try, /*On fail goto*//*Label 590*/ 15893, // Rule ID 1383 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6423:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (CVTTSD2SIrr_Int:{ *:[i32] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTTSD2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1383,
        GIR_Done,
      // Label 590: @15893
      GIM_Try, /*On fail goto*//*Label 591*/ 15933, // Rule ID 1385 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6424:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src)  =>  (CVTTSD2SI64rr_Int:{ *:[i64] } VR128:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTTSD2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1385,
        GIR_Done,
      // Label 591: @15933
      GIM_Try, /*On fail goto*//*Label 592*/ 15973, // Rule ID 1387 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6381:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VCVTSS2SIrr_Int:{ *:[i32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1387,
        GIR_Done,
      // Label 592: @15973
      GIM_Try, /*On fail goto*//*Label 593*/ 16013, // Rule ID 1389 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6382:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VCVTSS2SI64rr_Int:{ *:[i64] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1389,
        GIR_Done,
      // Label 593: @16013
      GIM_Try, /*On fail goto*//*Label 594*/ 16053, // Rule ID 1391 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6381:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (CVTSS2SIrr_Int:{ *:[i32] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTSS2SIrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1391,
        GIR_Done,
      // Label 594: @16053
      GIM_Try, /*On fail goto*//*Label 595*/ 16093, // Rule ID 1393 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6382:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (CVTSS2SI64rr_Int:{ *:[i64] } VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTSS2SI64rr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1393,
        GIR_Done,
      // Label 595: @16093
      GIM_Try, /*On fail goto*//*Label 596*/ 16133, // Rule ID 2627 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesimc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5427:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1)  =>  (VAESIMCrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESIMCrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2627,
        GIR_Done,
      // Label 596: @16133
      GIM_Try, /*On fail goto*//*Label 597*/ 16173, // Rule ID 2629 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesimc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5427:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1)  =>  (AESIMCrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESIMCrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2629,
        GIR_Done,
      // Label 597: @16173
      GIM_Try, /*On fail goto*//*Label 598*/ 16217, // Rule ID 12592 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_rsqrt_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6397:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (RSQRTSSr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src, VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RSQRTSSr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12592,
        GIR_Done,
      // Label 598: @16217
      GIM_Try, /*On fail goto*//*Label 599*/ 16261, // Rule ID 12594 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_rsqrt_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6397:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VRSQRTSSr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src, VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VRSQRTSSr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12594,
        GIR_Done,
      // Label 599: @16261
      GIM_Try, /*On fail goto*//*Label 600*/ 16305, // Rule ID 12604 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_rcp_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6395:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (RCPSSr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src, VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RCPSSr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12604,
        GIR_Done,
      // Label 600: @16305
      GIM_Try, /*On fail goto*//*Label 601*/ 16349, // Rule ID 12606 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_rcp_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6395:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src)  =>  (VRCPSSr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src, VR128:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VRCPSSr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12606,
        GIR_Done,
      // Label 601: @16349
      GIM_Try, /*On fail goto*//*Label 602*/ 16389, // Rule ID 14835 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6381:{ *:[iPTR] }, VR128X:{ *:[v4f32] }:$src)  =>  (VCVTSS2SIZrr_Int:{ *:[i32] } VR128X:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SIZrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14835,
        GIR_Done,
      // Label 602: @16389
      GIM_Try, /*On fail goto*//*Label 603*/ 16429, // Rule ID 14837 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvtss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6382:{ *:[iPTR] }, VR128X:{ *:[v4f32] }:$src)  =>  (VCVTSS2SI64Zrr_Int:{ *:[i64] } VR128X:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SI64Zrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14837,
        GIR_Done,
      // Label 603: @16429
      GIM_Try, /*On fail goto*//*Label 604*/ 16469, // Rule ID 14839 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6418:{ *:[iPTR] }, VR128X:{ *:[v2f64] }:$src)  =>  (VCVTSD2SIZrr_Int:{ *:[i32] } VR128X:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SIZrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14839,
        GIR_Done,
      // Label 604: @16469
      GIM_Try, /*On fail goto*//*Label 605*/ 16509, // Rule ID 14841 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6419:{ *:[iPTR] }, VR128X:{ *:[v2f64] }:$src)  =>  (VCVTSD2SI64Zrr_Int:{ *:[i64] } VR128X:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SI64Zrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14841,
        GIR_Done,
      // Label 605: @16509
      GIM_Try, /*On fail goto*//*Label 606*/ 16549, // Rule ID 14859 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6385:{ *:[iPTR] }, VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTSS2SIZrr_Int:{ *:[i32] } VR128X:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSS2SIZrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14859,
        GIR_Done,
      // Label 606: @16549
      GIM_Try, /*On fail goto*//*Label 607*/ 16589, // Rule ID 14861 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cvttss2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6386:{ *:[iPTR] }, VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTSS2SI64Zrr_Int:{ *:[i64] } VR128X:{ *:[v4f32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSS2SI64Zrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14861,
        GIR_Done,
      // Label 607: @16589
      GIM_Try, /*On fail goto*//*Label 608*/ 16629, // Rule ID 14863 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6423:{ *:[iPTR] }, VR128X:{ *:[v2f64] }:$src)  =>  (VCVTTSD2SIZrr_Int:{ *:[i32] } VR128X:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSD2SIZrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14863,
        GIR_Done,
      // Label 608: @16629
      GIM_Try, /*On fail goto*//*Label 609*/ 16669, // Rule ID 14865 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvttsd2si64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6424:{ *:[iPTR] }, VR128X:{ *:[v2f64] }:$src)  =>  (VCVTTSD2SI64Zrr_Int:{ *:[i64] } VR128X:{ *:[v2f64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTTSD2SI64Zrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14865,
        GIR_Done,
      // Label 609: @16669
      GIM_Reject,
    // Label 558: @16670
    GIM_Try, /*On fail goto*//*Label 610*/ 19700,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/4,
      GIM_Try, /*On fail goto*//*Label 611*/ 16731, // Rule ID 2631 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aeskeygenassist,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5428:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (VAESKEYGENASSIST128rr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESKEYGENASSIST128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2631,
        GIR_Done,
      // Label 611: @16731
      GIM_Try, /*On fail goto*//*Label 612*/ 16787, // Rule ID 2633 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aeskeygenassist,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5428:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (AESKEYGENASSIST128rr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESKEYGENASSIST128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2633,
        GIR_Done,
      // Label 612: @16787
      GIM_Try, /*On fail goto*//*Label 613*/ 16839, // Rule ID 82 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_bmi_pdep_32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6230:{ *:[iPTR] }, GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (PDEP32rr:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PDEP32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 82,
        GIR_Done,
      // Label 613: @16839
      GIM_Try, /*On fail goto*//*Label 614*/ 16891, // Rule ID 84 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_bmi_pdep_64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR64RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6231:{ *:[iPTR] }, GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (PDEP64rr:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PDEP64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 84,
        GIR_Done,
      // Label 614: @16891
      GIM_Try, /*On fail goto*//*Label 615*/ 16943, // Rule ID 86 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_bmi_pext_32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6232:{ *:[iPTR] }, GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (PEXT32rr:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PEXT32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 86,
        GIR_Done,
      // Label 615: @16943
      GIM_Try, /*On fail goto*//*Label 616*/ 16995, // Rule ID 88 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_bmi_pext_64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR64RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6233:{ *:[iPTR] }, GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (PEXT64rr:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PEXT64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 88,
        GIR_Done,
      // Label 616: @16995
      GIM_Try, /*On fail goto*//*Label 617*/ 17047, // Rule ID 1403 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6420:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (VCVTSD2SSrr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SSrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1403,
        GIR_Done,
      // Label 617: @17047
      GIM_Try, /*On fail goto*//*Label 618*/ 17099, // Rule ID 1405 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cvtsd2ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6420:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (CVTSD2SSrr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CVTSD2SSrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1405,
        GIR_Done,
      // Label 618: @17099
      GIM_Try, /*On fail goto*//*Label 619*/ 17151, // Rule ID 2313 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_b_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 6541:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (VPSIGNBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNBrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2313,
        GIR_Done,
      // Label 619: @17151
      GIM_Try, /*On fail goto*//*Label 620*/ 17203, // Rule ID 2315 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_w_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6545:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPSIGNWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2315,
        GIR_Done,
      // Label 620: @17203
      GIM_Try, /*On fail goto*//*Label 621*/ 17255, // Rule ID 2317 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_d_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6543:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPSIGNDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2317,
        GIR_Done,
      // Label 621: @17255
      GIM_Try, /*On fail goto*//*Label 622*/ 17307, // Rule ID 2319 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_phadd_sw_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6525:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPHADDSWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDSWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2319,
        GIR_Done,
      // Label 622: @17307
      GIM_Try, /*On fail goto*//*Label 623*/ 17359, // Rule ID 2321 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_phsub_sw_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6531:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (VPHSUBSWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHSUBSWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2321,
        GIR_Done,
      // Label 623: @17359
      GIM_Try, /*On fail goto*//*Label 624*/ 17411, // Rule ID 2337 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_psign_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v32s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v32i8] } 5535:{ *:[iPTR] }, VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)  =>  (VPSIGNBYrr:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNBYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2337,
        GIR_Done,
      // Label 624: @17411
      GIM_Try, /*On fail goto*//*Label 625*/ 17463, // Rule ID 2339 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_psign_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i16] } 5537:{ *:[iPTR] }, VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPSIGNWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNWYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2339,
        GIR_Done,
      // Label 625: @17463
      GIM_Try, /*On fail goto*//*Label 626*/ 17515, // Rule ID 2341 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_psign_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i32] } 5536:{ *:[iPTR] }, VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPSIGNDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPSIGNDYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2341,
        GIR_Done,
      // Label 626: @17515
      GIM_Try, /*On fail goto*//*Label 627*/ 17567, // Rule ID 2343 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_phadd_sw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i16] } 5522:{ *:[iPTR] }, VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPHADDSWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHADDSWYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2343,
        GIR_Done,
      // Label 627: @17567
      GIM_Try, /*On fail goto*//*Label 628*/ 17619, // Rule ID 2345 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_phsub_sw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i16] } 5525:{ *:[iPTR] }, VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)  =>  (VPHSUBSWYrr:{ *:[v16i16] } VR256:{ *:[v16i16] }:$src1, VR256:{ *:[v16i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPHSUBSWYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2345,
        GIR_Done,
      // Label 628: @17619
      GIM_Try, /*On fail goto*//*Label 629*/ 17671, // Rule ID 2355 //
        GIM_CheckFeatures, GIFBS_UseSSSE3,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_b_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 6541:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)  =>  (PSIGNBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PSIGNBrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2355,
        GIR_Done,
      // Label 629: @17671
      GIM_Try, /*On fail goto*//*Label 630*/ 17723, // Rule ID 2357 //
        GIM_CheckFeatures, GIFBS_UseSSSE3,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_w_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6545:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PSIGNWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PSIGNWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2357,
        GIR_Done,
      // Label 630: @17723
      GIM_Try, /*On fail goto*//*Label 631*/ 17775, // Rule ID 2359 //
        GIM_CheckFeatures, GIFBS_UseSSSE3,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_psign_d_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6543:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (PSIGNDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PSIGNDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2359,
        GIR_Done,
      // Label 631: @17775
      GIM_Try, /*On fail goto*//*Label 632*/ 17827, // Rule ID 2363 //
        GIM_CheckFeatures, GIFBS_UseSSSE3,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_phadd_sw_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6525:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PHADDSWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PHADDSWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2363,
        GIR_Done,
      // Label 632: @17827
      GIM_Try, /*On fail goto*//*Label 633*/ 17879, // Rule ID 2365 //
        GIM_CheckFeatures, GIFBS_UseSSSE3,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_ssse3_phsub_sw_128,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6531:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)  =>  (PHSUBSWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PHSUBSWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2365,
        GIR_Done,
      // Label 633: @17879
      GIM_Try, /*On fail goto*//*Label 634*/ 17931, // Rule ID 2582 //
        GIM_CheckFeatures, GIFBS_HasSSE42,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse42_crc32_32_8,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR8RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6499:{ *:[iPTR] }, GR32:{ *:[i32] }:$src1, GR8:{ *:[i8] }:$src2)  =>  (CRC32r32r8:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR8:{ *:[i8] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CRC32r32r8,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2582,
        GIR_Done,
      // Label 634: @17931
      GIM_Try, /*On fail goto*//*Label 635*/ 17983, // Rule ID 2584 //
        GIM_CheckFeatures, GIFBS_HasSSE42,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse42_crc32_32_16,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR16RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6497:{ *:[iPTR] }, GR32:{ *:[i32] }:$src1, GR16:{ *:[i16] }:$src2)  =>  (CRC32r32r16:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR16:{ *:[i16] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CRC32r32r16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2584,
        GIR_Done,
      // Label 635: @17983
      GIM_Try, /*On fail goto*//*Label 636*/ 18035, // Rule ID 2586 //
        GIM_CheckFeatures, GIFBS_HasSSE42,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse42_crc32_32_32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 6498:{ *:[iPTR] }, GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)  =>  (CRC32r32r32:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CRC32r32r32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2586,
        GIR_Done,
      // Label 636: @18035
      GIM_Try, /*On fail goto*//*Label 637*/ 18087, // Rule ID 2588 //
        GIM_CheckFeatures, GIFBS_HasSSE42,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse42_crc32_64_64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::GR64RegClassID,
        // (intrinsic_wo_chain:{ *:[i64] } 6500:{ *:[iPTR] }, GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)  =>  (CRC32r64r64:{ *:[i64] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CRC32r64r64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2588,
        GIR_Done,
      // Label 637: @18087
      GIM_Try, /*On fail goto*//*Label 638*/ 18139, // Rule ID 2591 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha1nexte,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6363:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (SHA1NEXTErr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA1NEXTErr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2591,
        GIR_Done,
      // Label 638: @18139
      GIM_Try, /*On fail goto*//*Label 639*/ 18191, // Rule ID 2593 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha1msg1,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6361:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (SHA1MSG1rr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA1MSG1rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2593,
        GIR_Done,
      // Label 639: @18191
      GIM_Try, /*On fail goto*//*Label 640*/ 18243, // Rule ID 2595 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha1msg2,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6362:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (SHA1MSG2rr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA1MSG2rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2595,
        GIR_Done,
      // Label 640: @18243
      GIM_Try, /*On fail goto*//*Label 641*/ 18295, // Rule ID 2599 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha256msg1,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6365:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (SHA256MSG1rr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA256MSG1rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2599,
        GIR_Done,
      // Label 641: @18295
      GIM_Try, /*On fail goto*//*Label 642*/ 18347, // Rule ID 2601 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha256msg2,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6366:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (SHA256MSG2rr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA256MSG2rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2601,
        GIR_Done,
      // Label 642: @18347
      GIM_Try, /*On fail goto*//*Label 643*/ 18399, // Rule ID 2603 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX_NoVLX_Or_NoVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5421:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VAESENCrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2603,
        GIR_Done,
      // Label 643: @18399
      GIM_Try, /*On fail goto*//*Label 644*/ 18451, // Rule ID 2605 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX_NoVLX_Or_NoVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5424:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VAESENCLASTrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCLASTrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2605,
        GIR_Done,
      // Label 644: @18451
      GIM_Try, /*On fail goto*//*Label 645*/ 18503, // Rule ID 2607 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX_NoVLX_Or_NoVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5415:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VAESDECrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2607,
        GIR_Done,
      // Label 645: @18503
      GIM_Try, /*On fail goto*//*Label 646*/ 18555, // Rule ID 2609 //
        GIM_CheckFeatures, GIFBS_HasAES_HasAVX_NoVLX_Or_NoVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5418:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VAESDECLASTrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECLASTrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2609,
        GIR_Done,
      // Label 646: @18555
      GIM_Try, /*On fail goto*//*Label 647*/ 18607, // Rule ID 2611 //
        GIM_CheckFeatures, GIFBS_HasVAES_NoVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5422:{ *:[iPTR] }, VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VAESENCYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2611,
        GIR_Done,
      // Label 647: @18607
      GIM_Try, /*On fail goto*//*Label 648*/ 18659, // Rule ID 2613 //
        GIM_CheckFeatures, GIFBS_HasVAES_NoVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5425:{ *:[iPTR] }, VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VAESENCLASTYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCLASTYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2613,
        GIR_Done,
      // Label 648: @18659
      GIM_Try, /*On fail goto*//*Label 649*/ 18711, // Rule ID 2615 //
        GIM_CheckFeatures, GIFBS_HasVAES_NoVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5416:{ *:[iPTR] }, VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VAESDECYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2615,
        GIR_Done,
      // Label 649: @18711
      GIM_Try, /*On fail goto*//*Label 650*/ 18763, // Rule ID 2617 //
        GIM_CheckFeatures, GIFBS_HasVAES_NoVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5419:{ *:[iPTR] }, VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VAESDECLASTYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECLASTYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2617,
        GIR_Done,
      // Label 650: @18763
      GIM_Try, /*On fail goto*//*Label 651*/ 18815, // Rule ID 2619 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5421:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (AESENCrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESENCrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2619,
        GIR_Done,
      // Label 651: @18815
      GIM_Try, /*On fail goto*//*Label 652*/ 18867, // Rule ID 2621 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5424:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (AESENCLASTrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESENCLASTrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2621,
        GIR_Done,
      // Label 652: @18867
      GIM_Try, /*On fail goto*//*Label 653*/ 18919, // Rule ID 2623 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5415:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (AESDECrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESDECrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2623,
        GIR_Done,
      // Label 653: @18919
      GIM_Try, /*On fail goto*//*Label 654*/ 18971, // Rule ID 2625 //
        GIM_CheckFeatures, GIFBS_HasAES_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5418:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (AESDECLASTrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AESDECLASTrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2625,
        GIR_Done,
      // Label 654: @18971
      GIM_Try, /*On fail goto*//*Label 655*/ 19023, // Rule ID 2642 //
        GIM_CheckFeatures, GIFBS_HasSSE4A,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse4a_extrq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6515:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src, VR128:{ *:[v16i8] }:$mask)  =>  (EXTRQ:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src, VR128:{ *:[v16i8] }:$mask)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::EXTRQ,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // mask
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2642,
        GIR_Done,
      // Label 655: @19023
      GIM_Try, /*On fail goto*//*Label 656*/ 19075, // Rule ID 2644 //
        GIM_CheckFeatures, GIFBS_HasSSE4A,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse4a_insertq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6517:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src, VR128:{ *:[v2i64] }:$mask)  =>  (INSERTQ:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src, VR128:{ *:[v2i64] }:$mask)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INSERTQ,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // mask
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2644,
        GIR_Done,
      // Label 656: @19075
      GIM_Try, /*On fail goto*//*Label 657*/ 19127, // Rule ID 11166 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5421:{ *:[iPTR] }, VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VAESENCZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCZ128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11166,
        GIR_Done,
      // Label 657: @19127
      GIM_Try, /*On fail goto*//*Label 658*/ 19179, // Rule ID 11168 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256XRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5422:{ *:[iPTR] }, VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VAESENCZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCZ256rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11168,
        GIR_Done,
      // Label 658: @19179
      GIM_Try, /*On fail goto*//*Label 659*/ 19231, // Rule ID 11170 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenc_512,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR512RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i64] } 5423:{ *:[iPTR] }, VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VAESENCZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11170,
        GIR_Done,
      // Label 659: @19231
      GIM_Try, /*On fail goto*//*Label 660*/ 19283, // Rule ID 11172 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5424:{ *:[iPTR] }, VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VAESENCLASTZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCLASTZ128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11172,
        GIR_Done,
      // Label 660: @19283
      GIM_Try, /*On fail goto*//*Label 661*/ 19335, // Rule ID 11174 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256XRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5425:{ *:[iPTR] }, VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VAESENCLASTZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCLASTZ256rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11174,
        GIR_Done,
      // Label 661: @19335
      GIM_Try, /*On fail goto*//*Label 662*/ 19387, // Rule ID 11176 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesenclast_512,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR512RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i64] } 5426:{ *:[iPTR] }, VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VAESENCLASTZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESENCLASTZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11176,
        GIR_Done,
      // Label 662: @19387
      GIM_Try, /*On fail goto*//*Label 663*/ 19439, // Rule ID 11178 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5415:{ *:[iPTR] }, VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VAESDECZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECZ128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11178,
        GIR_Done,
      // Label 663: @19439
      GIM_Try, /*On fail goto*//*Label 664*/ 19491, // Rule ID 11180 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256XRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5416:{ *:[iPTR] }, VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VAESDECZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECZ256rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11180,
        GIR_Done,
      // Label 664: @19491
      GIM_Try, /*On fail goto*//*Label 665*/ 19543, // Rule ID 11182 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdec_512,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR512RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i64] } 5417:{ *:[iPTR] }, VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VAESDECZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11182,
        GIR_Done,
      // Label 665: @19543
      GIM_Try, /*On fail goto*//*Label 666*/ 19595, // Rule ID 11184 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128XRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 5418:{ *:[iPTR] }, VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VAESDECLASTZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECLASTZ128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11184,
        GIR_Done,
      // Label 666: @19595
      GIM_Try, /*On fail goto*//*Label 667*/ 19647, // Rule ID 11186 //
        GIM_CheckFeatures, GIFBS_HasVAES_HasVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256XRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i64] } 5419:{ *:[iPTR] }, VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VAESDECLASTZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECLASTZ256rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11186,
        GIR_Done,
      // Label 667: @19647
      GIM_Try, /*On fail goto*//*Label 668*/ 19699, // Rule ID 11188 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVAES,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_aesni_aesdeclast_512,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR512RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i64] } 5420:{ *:[iPTR] }, VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VAESDECLASTZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VAESDECLASTZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11188,
        GIR_Done,
      // Label 668: @19699
      GIM_Reject,
    // Label 610: @19700
    GIM_Try, /*On fail goto*//*Label 669*/ 22150,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/5,
      GIM_Try, /*On fail goto*//*Label 670*/ 19773, // Rule ID 1459 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cmp_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6370:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src, (imm:{ *:[i8] }):$cc)  =>  (VCMPSSrr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src, (imm:{ *:[i8] }):$cc)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCMPSSrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cc
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1459,
        GIR_Done,
      // Label 670: @19773
      GIM_Try, /*On fail goto*//*Label 671*/ 19841, // Rule ID 1461 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cmp_sd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6408:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src, (imm:{ *:[i8] }):$cc)  =>  (VCMPSDrr_Int:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src, (imm:{ *:[i8] }):$cc)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCMPSDrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cc
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1461,
        GIR_Done,
      // Label 671: @19841
      GIM_Try, /*On fail goto*//*Label 672*/ 19909, // Rule ID 1463 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse_cmp_ss,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6370:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src, (imm:{ *:[i8] }):$cc)  =>  (CMPSSrr_Int:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src, (imm:{ *:[i8] }):$cc)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CMPSSrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cc
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1463,
        GIR_Done,
      // Label 672: @19909
      GIM_Try, /*On fail goto*//*Label 673*/ 19977, // Rule ID 1465 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse2_cmp_sd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6408:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src, (imm:{ *:[i8] }):$cc)  =>  (CMPSDrr_Int:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src, (imm:{ *:[i8] }):$cc)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::CMPSDrr_Int,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cc
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1465,
        GIR_Done,
      // Label 673: @19977
      GIM_Try, /*On fail goto*//*Label 674*/ 20045, // Rule ID 2523 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_mpsadbw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6486:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VMPSADBWrri:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VMPSADBWrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2523,
        GIR_Done,
      // Label 674: @20045
      GIM_Try, /*On fail goto*//*Label 675*/ 20113, // Rule ID 2525 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_dpps,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6484:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VDPPSrri:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VDPPSrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2525,
        GIR_Done,
      // Label 675: @20113
      GIM_Try, /*On fail goto*//*Label 676*/ 20181, // Rule ID 2527 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_dppd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6483:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VDPPDrri:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VDPPDrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2527,
        GIR_Done,
      // Label 676: @20181
      GIM_Try, /*On fail goto*//*Label 677*/ 20249, // Rule ID 2529 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx_dp_ps_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8f32] } 5440:{ *:[iPTR] }, VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VDPPSYrri:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VDPPSYrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2529,
        GIR_Done,
      // Label 677: @20249
      GIM_Try, /*On fail goto*//*Label 678*/ 20317, // Rule ID 2531 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_mpsadbw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i16] } 5509:{ *:[iPTR] }, VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VMPSADBWYrri:{ *:[v16i16] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VMPSADBWYrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2531,
        GIR_Done,
      // Label 678: @20317
      GIM_Try, /*On fail goto*//*Label 679*/ 20385, // Rule ID 2533 //
        GIM_CheckFeatures, GIFBS_UseSSE41,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_mpsadbw,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6486:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (MPSADBWrri:{ *:[v8i16] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MPSADBWrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2533,
        GIR_Done,
      // Label 679: @20385
      GIM_Try, /*On fail goto*//*Label 680*/ 20453, // Rule ID 2535 //
        GIM_CheckFeatures, GIFBS_UseSSE41,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_dpps,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6484:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (DPPSrri:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DPPSrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2535,
        GIR_Done,
      // Label 680: @20453
      GIM_Try, /*On fail goto*//*Label 681*/ 20521, // Rule ID 2537 //
        GIM_CheckFeatures, GIFBS_UseSSE41,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_dppd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6483:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (DPPDrri:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::DPPDrri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2537,
        GIR_Done,
      // Label 681: @20521
      GIM_Try, /*On fail goto*//*Label 682*/ 20589, // Rule ID 2589 //
        GIM_CheckFeatures, GIFBS_HasSHA,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sha1rnds4,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6364:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (SHA1RNDS4rri:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHA1RNDS4rri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2589,
        GIR_Done,
      // Label 682: @20589
      GIM_Try, /*On fail goto*//*Label 683*/ 20657, // Rule ID 2635 //
        GIM_CheckFeatures, GIFBS_HasPCLMUL_NoAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6332:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (PCLMULQDQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PCLMULQDQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2635,
        GIR_Done,
      // Label 683: @20657
      GIM_Try, /*On fail goto*//*Label 684*/ 20725, // Rule ID 2637 //
        GIM_CheckFeatures, GIFBS_HasAVX_HasPCLMUL_NoVLX_Or_NoVPCLMULQDQ,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6332:{ *:[iPTR] }, VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VPCLMULQDQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPCLMULQDQrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2637,
        GIR_Done,
      // Label 684: @20725
      GIM_Try, /*On fail goto*//*Label 685*/ 20793, // Rule ID 2639 //
        GIM_CheckFeatures, GIFBS_HasVPCLMULQDQ_NoVLX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i64] } 6333:{ *:[iPTR] }, VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VPCLMULQDQYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPCLMULQDQYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2639,
        GIR_Done,
      // Label 685: @20793
      GIM_Try, /*On fail goto*//*Label 686*/ 20861, // Rule ID 11190 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVPCLMULQDQ,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq_512,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR512RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i64] } 6334:{ *:[iPTR] }, VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VPCLMULQDQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPCLMULQDQZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11190,
        GIR_Done,
      // Label 686: @20861
      GIM_Try, /*On fail goto*//*Label 687*/ 20929, // Rule ID 11192 //
        GIM_CheckFeatures, GIFBS_HasVLX_HasVPCLMULQDQ,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128XRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6332:{ *:[iPTR] }, VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VPCLMULQDQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPCLMULQDQZ128rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11192,
        GIR_Done,
      // Label 687: @20929
      GIM_Try, /*On fail goto*//*Label 688*/ 20997, // Rule ID 11194 //
        GIM_CheckFeatures, GIFBS_HasVLX_HasVPCLMULQDQ,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_pclmulqdq_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256XRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i64] } 6333:{ *:[iPTR] }, VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2, (imm:{ *:[i8] }):$src3)  =>  (VPCLMULQDQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2, (imm:{ *:[i8] }):$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPCLMULQDQZ256rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11194,
        GIR_Done,
      // Label 688: @20997
      GIM_Try, /*On fail goto*//*Label 689*/ 21061, // Rule ID 1219 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmadcswd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6625:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMADCSWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMADCSWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1219,
        GIR_Done,
      // Label 689: @21061
      GIM_Try, /*On fail goto*//*Label 690*/ 21125, // Rule ID 1221 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmadcsswd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6624:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMADCSSWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMADCSSWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1221,
        GIR_Done,
      // Label 690: @21125
      GIM_Try, /*On fail goto*//*Label 691*/ 21189, // Rule ID 1223 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacsww,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6623:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)  =>  (VPMACSWWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSWWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1223,
        GIR_Done,
      // Label 691: @21189
      GIM_Try, /*On fail goto*//*Label 692*/ 21253, // Rule ID 1225 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacswd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6622:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMACSWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1225,
        GIR_Done,
      // Label 692: @21253
      GIM_Try, /*On fail goto*//*Label 693*/ 21317, // Rule ID 1227 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacssww,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 6621:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)  =>  (VPMACSSWWrr:{ *:[v8i16] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v8i16] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSSWWrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1227,
        GIR_Done,
      // Label 693: @21317
      GIM_Try, /*On fail goto*//*Label 694*/ 21381, // Rule ID 1229 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacsswd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6620:{ *:[iPTR] }, VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMACSSWDrr:{ *:[v4i32] } VR128:{ *:[v8i16] }:$src1, VR128:{ *:[v8i16] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSSWDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1229,
        GIR_Done,
      // Label 694: @21381
      GIM_Try, /*On fail goto*//*Label 695*/ 21445, // Rule ID 1231 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacssdql,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6619:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)  =>  (VPMACSSDQLrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSSDQLrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1231,
        GIR_Done,
      // Label 695: @21445
      GIM_Try, /*On fail goto*//*Label 696*/ 21509, // Rule ID 1233 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacssdqh,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6618:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)  =>  (VPMACSSDQHrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSSDQHrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1233,
        GIR_Done,
      // Label 696: @21509
      GIM_Try, /*On fail goto*//*Label 697*/ 21573, // Rule ID 1235 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacssdd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6617:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMACSSDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSSDDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1235,
        GIR_Done,
      // Label 697: @21573
      GIM_Try, /*On fail goto*//*Label 698*/ 21637, // Rule ID 1237 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacsdql,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6616:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)  =>  (VPMACSDQLrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSDQLrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1237,
        GIR_Done,
      // Label 698: @21637
      GIM_Try, /*On fail goto*//*Label 699*/ 21701, // Rule ID 1239 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacsdqh,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 6615:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)  =>  (VPMACSDQHrr:{ *:[v2i64] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v2i64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSDQHrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1239,
        GIR_Done,
      // Label 699: @21701
      GIM_Try, /*On fail goto*//*Label 700*/ 21765, // Rule ID 1241 //
        GIM_CheckFeatures, GIFBS_HasXOP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xop_vpmacsdd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 6614:{ *:[iPTR] }, VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)  =>  (VPMACSDDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2, VR128:{ *:[v4i32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMACSDDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1241,
        GIR_Done,
      // Label 700: @21765
      GIM_Try, /*On fail goto*//*Label 701*/ 21829, // Rule ID 2557 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_blendvpd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 6481:{ *:[iPTR] }, VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, VR128:{ *:[v2f64] }:$src3)  =>  (VBLENDVPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2, VR128:{ *:[v2f64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VBLENDVPDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2557,
        GIR_Done,
      // Label 701: @21829
      GIM_Try, /*On fail goto*//*Label 702*/ 21893, // Rule ID 2559 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx_blendv_pd_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f64] } 5431:{ *:[iPTR] }, VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2, VR256:{ *:[v4f64] }:$src3)  =>  (VBLENDVPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2, VR256:{ *:[v4f64] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VBLENDVPDYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2559,
        GIR_Done,
      // Label 702: @21893
      GIM_Try, /*On fail goto*//*Label 703*/ 21957, // Rule ID 2561 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_blendvps,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 6482:{ *:[iPTR] }, VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, VR128:{ *:[v4f32] }:$src3)  =>  (VBLENDVPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2, VR128:{ *:[v4f32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VBLENDVPSrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2561,
        GIR_Done,
      // Label 703: @21957
      GIM_Try, /*On fail goto*//*Label 704*/ 22021, // Rule ID 2563 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx_blendv_ps_256,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v8f32] } 5432:{ *:[iPTR] }, VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2, VR256:{ *:[v8f32] }:$src3)  =>  (VBLENDVPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2, VR256:{ *:[v8f32] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VBLENDVPSYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2563,
        GIR_Done,
      // Label 704: @22021
      GIM_Try, /*On fail goto*//*Label 705*/ 22085, // Rule ID 2565 //
        GIM_CheckFeatures, GIFBS_HasAVX,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_sse41_pblendvb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR128RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 6488:{ *:[iPTR] }, VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, VR128:{ *:[v16i8] }:$src3)  =>  (VPBLENDVBrr:{ *:[v16i8] } VR128:{ *:[v16i8] }:$src1, VR128:{ *:[v16i8] }:$src2, VR128:{ *:[v16i8] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPBLENDVBrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2565,
        GIR_Done,
      // Label 705: @22085
      GIM_Try, /*On fail goto*//*Label 706*/ 22149, // Rule ID 2567 //
        GIM_CheckFeatures, GIFBS_HasAVX2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_avx2_pblendvb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v32s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v32s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/X86::VR256RegClassID,
        // (intrinsic_wo_chain:{ *:[v32i8] } 5518:{ *:[iPTR] }, VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2, VR256:{ *:[v32i8] }:$src3)  =>  (VPBLENDVBYrr:{ *:[v32i8] } VR256:{ *:[v32i8] }:$src1, VR256:{ *:[v32i8] }:$src2, VR256:{ *:[v32i8] }:$src3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPBLENDVBYrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // src2
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // src3
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2567,
        GIR_Done,
      // Label 706: @22149
      GIM_Reject,
    // Label 669: @22150
    GIM_Reject,
    // Label 8: @22151
    GIM_Try, /*On fail goto*//*Label 707*/ 22169, // Rule ID 1793 //
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_sse2_pause,
      // (intrinsic_void 6440:{ *:[iPTR] })  =>  (PAUSE)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PAUSE,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1793,
      GIR_Done,
    // Label 707: @22169
    GIM_Try, /*On fail goto*//*Label 708*/ 22189, // Rule ID 1794 //
      GIM_CheckFeatures, GIFBS_HasSSE1,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_sse_sfence,
      // (intrinsic_void 6398:{ *:[iPTR] })  =>  (SFENCE)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SFENCE,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1794,
      GIR_Done,
    // Label 708: @22189
    GIM_Try, /*On fail goto*//*Label 709*/ 22209, // Rule ID 1795 //
      GIM_CheckFeatures, GIFBS_HasSSE2,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_sse2_lfence,
      // (intrinsic_void 6425:{ *:[iPTR] })  =>  (LFENCE)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LFENCE,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1795,
      GIR_Done,
    // Label 709: @22209
    GIM_Try, /*On fail goto*//*Label 710*/ 22229, // Rule ID 1796 //
      GIM_CheckFeatures, GIFBS_HasMFence,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_sse2_mfence,
      // (intrinsic_void 6429:{ *:[iPTR] })  =>  (MFENCE)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MFENCE,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1796,
      GIR_Done,
    // Label 710: @22229
    GIM_Try, /*On fail goto*//*Label 711*/ 22249, // Rule ID 2677 //
      GIM_CheckFeatures, GIFBS_HasAVX,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_avx_vzeroall,
      // (intrinsic_void 5483:{ *:[iPTR] })  =>  (VZEROALL)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VZEROALL,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2677,
      GIR_Done,
    // Label 711: @22249
    GIM_Try, /*On fail goto*//*Label 712*/ 22269, // Rule ID 2678 //
      GIM_CheckFeatures, GIFBS_HasAVX,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_avx_vzeroupper,
      // (intrinsic_void 5484:{ *:[iPTR] })  =>  (VZEROUPPER)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VZEROUPPER,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2678,
      GIR_Done,
    // Label 712: @22269
    GIM_Try, /*On fail goto*//*Label 713*/ 22289, // Rule ID 11766 //
      GIM_CheckFeatures, GIFBS_HasMMX,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_mmx_emms,
      // (intrinsic_void 6258:{ *:[iPTR] })  =>  (MMX_EMMS)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MMX_EMMS,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 11766,
      GIR_Done,
    // Label 713: @22289
    GIM_Try, /*On fail goto*//*Label 714*/ 22309, // Rule ID 11985 //
      GIM_CheckFeatures, GIFBS_Has3DNow,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_mmx_femms,
      // (intrinsic_void 6259:{ *:[iPTR] })  =>  (FEMMS)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::FEMMS,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 11985,
      GIR_Done,
    // Label 714: @22309
    GIM_Try, /*On fail goto*//*Label 715*/ 22329, // Rule ID 12000 //
      GIM_CheckFeatures, GIFBS_HasRTM,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_xend,
      // (intrinsic_void 6579:{ *:[iPTR] })  =>  (XEND)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XEND,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12000,
      GIR_Done,
    // Label 715: @22329
    GIM_Try, /*On fail goto*//*Label 716*/ 22347, // Rule ID 12009 //
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wbinvd,
      // (intrinsic_void 6566:{ *:[iPTR] })  =>  (WBINVD)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WBINVD,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12009,
      GIR_Done,
    // Label 716: @22347
    GIM_Try, /*On fail goto*//*Label 717*/ 22367, // Rule ID 12010 //
      GIM_CheckFeatures, GIFBS_HasWBNOINVD,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wbnoinvd,
      // (intrinsic_void 6567:{ *:[iPTR] })  =>  (WBNOINVD)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WBNOINVD,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12010,
      GIR_Done,
    // Label 717: @22367
    GIM_Try, /*On fail goto*//*Label 718*/ 22385, // Rule ID 12015 //
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_saveprevssp,
      // (intrinsic_void 6355:{ *:[iPTR] })  =>  (SAVEPREVSSP)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAVEPREVSSP,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12015,
      GIR_Done,
    // Label 718: @22385
    GIM_Try, /*On fail goto*//*Label 719*/ 22403, // Rule ID 12021 //
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_setssbsy,
      // (intrinsic_void 6360:{ *:[iPTR] })  =>  (SETSSBSY)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SETSSBSY,
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12021,
      GIR_Done,
    // Label 719: @22403
    GIM_Try, /*On fail goto*//*Label 720*/ 23363,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/2,
      GIM_Try, /*On fail goto*//*Label 721*/ 22434, // Rule ID 12006 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_int,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/1, 3,
        // (intrinsic_void 6251:{ *:[iPTR] }, 3:{ *:[i8] })  =>  (INT3)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INT3,
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12006,
        GIR_Done,
      // Label 721: @22434
      GIM_Try, /*On fail goto*//*Label 722*/ 22471, // Rule ID 12002 //
        GIM_CheckFeatures, GIFBS_HasRTM,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_xabort,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_void 6577:{ *:[iPTR] }, (imm:{ *:[i8] }):$imm)  =>  (XABORT (imm:{ *:[i8] }):$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XABORT,
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12002,
        GIR_Done,
      // Label 722: @22471
      GIM_Try, /*On fail goto*//*Label 723*/ 22506, // Rule ID 12007 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_int,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_void 6251:{ *:[iPTR] }, (imm:{ *:[i8] }):$trap)  =>  (INT (imm:{ *:[i8] }):$trap)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INT,
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // trap
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12007,
        GIR_Done,
      // Label 723: @22506
      GIM_Try, /*On fail goto*//*Label 724*/ 22538, // Rule ID 1 //
        GIM_CheckFeatures, GIFBS_Not64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_flags_read_u32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6241:{ *:[iPTR] })  =>  (RDFLAGS32:{ *:[i32] }:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDFLAGS32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1,
        GIR_Done,
      // Label 724: @22538
      GIM_Try, /*On fail goto*//*Label 725*/ 22570, // Rule ID 2 //
        GIM_CheckFeatures, GIFBS_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_flags_read_u64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        // (intrinsic_w_chain:{ *:[i64] } 6242:{ *:[iPTR] })  =>  (RDFLAGS64:{ *:[i64] }:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDFLAGS64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2,
        GIR_Done,
      // Label 725: @22570
      GIM_Try, /*On fail goto*//*Label 726*/ 22602, // Rule ID 95 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_slwpcb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6368:{ *:[iPTR] })  =>  (SLWPCB:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SLWPCB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 95,
        GIR_Done,
      // Label 726: @22602
      GIM_Try, /*On fail goto*//*Label 727*/ 22634, // Rule ID 97 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_slwpcb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        // (intrinsic_w_chain:{ *:[i64] } 6368:{ *:[iPTR] })  =>  (SLWPCB64:{ *:[i64] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SLWPCB64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 97,
        GIR_Done,
      // Label 727: @22634
      GIM_Try, /*On fail goto*//*Label 728*/ 22666, // Rule ID 11999 //
        GIM_CheckFeatures, GIFBS_HasRTM,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_xbegin,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6578:{ *:[iPTR] })  =>  (XBEGIN:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::XBEGIN,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 11999,
        GIR_Done,
      // Label 728: @22666
      GIM_Try, /*On fail goto*//*Label 729*/ 22696, // Rule ID 12037 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdpkru,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6342:{ *:[iPTR] })  =>  (RDPKRU:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDPKRU,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12037,
        GIR_Done,
      // Label 729: @22696
      GIM_Try, /*On fail goto*//*Label 730*/ 22728, // Rule ID 12038 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdfsbase_32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6337:{ *:[iPTR] })  =>  (RDFSBASE:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDFSBASE,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12038,
        GIR_Done,
      // Label 730: @22728
      GIM_Try, /*On fail goto*//*Label 731*/ 22760, // Rule ID 12039 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdfsbase_64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        // (intrinsic_w_chain:{ *:[i64] } 6338:{ *:[iPTR] })  =>  (RDFSBASE64:{ *:[i64] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDFSBASE64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12039,
        GIR_Done,
      // Label 731: @22760
      GIM_Try, /*On fail goto*//*Label 732*/ 22792, // Rule ID 12040 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdgsbase_32,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6339:{ *:[iPTR] })  =>  (RDGSBASE:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDGSBASE,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12040,
        GIR_Done,
      // Label 732: @22792
      GIM_Try, /*On fail goto*//*Label 733*/ 22824, // Rule ID 12041 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdgsbase_64,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        // (intrinsic_w_chain:{ *:[i64] } 6340:{ *:[iPTR] })  =>  (RDGSBASE64:{ *:[i64] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDGSBASE64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12041,
        GIR_Done,
      // Label 733: @22824
      GIM_Try, /*On fail goto*//*Label 734*/ 22856, // Rule ID 12047 //
        GIM_CheckFeatures, GIFBS_HasRDPID_Not64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdpid,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6341:{ *:[iPTR] })  =>  (RDPID32:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDPID32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12047,
        GIR_Done,
      // Label 734: @22856
      GIM_Try, /*On fail goto*//*Label 735*/ 22888, // Rule ID 3 //
        GIM_CheckFeatures, GIFBS_Not64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_flags_write_u32,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6243:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (WRFLAGS32:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRFLAGS32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 3,
        GIR_Done,
      // Label 735: @22888
      GIM_Try, /*On fail goto*//*Label 736*/ 22920, // Rule ID 4 //
        GIM_CheckFeatures, GIFBS_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_flags_write_u64,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6244:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (WRFLAGS64:{ *:[i64] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRFLAGS64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 4,
        GIR_Done,
      // Label 736: @22920
      GIM_Try, /*On fail goto*//*Label 737*/ 22952, // Rule ID 94 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_llwpcb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6253:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (LLWPCB GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LLWPCB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 94,
        GIR_Done,
      // Label 737: @22952
      GIM_Try, /*On fail goto*//*Label 738*/ 22984, // Rule ID 96 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_llwpcb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6253:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (LLWPCB64 GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LLWPCB64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 96,
        GIR_Done,
      // Label 738: @22984
      GIM_Try, /*On fail goto*//*Label 739*/ 23016, // Rule ID 108 //
        GIM_CheckFeatures, GIFBS_HasWAITPKG_Not64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_umonitor,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
        // (intrinsic_void 6551:{ *:[iPTR] }, GR16:{ *:[i16] }:$src)  =>  (UMONITOR16 GR16:{ *:[i16] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::UMONITOR16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 108,
        GIR_Done,
      // Label 739: @23016
      GIM_Try, /*On fail goto*//*Label 740*/ 23048, // Rule ID 109 //
        GIM_CheckFeatures, GIFBS_HasWAITPKG,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_umonitor,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6551:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (UMONITOR32 GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::UMONITOR32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 109,
        GIR_Done,
      // Label 740: @23048
      GIM_Try, /*On fail goto*//*Label 741*/ 23080, // Rule ID 110 //
        GIM_CheckFeatures, GIFBS_HasWAITPKG_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_umonitor,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6551:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (UMONITOR64 GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::UMONITOR64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 110,
        GIR_Done,
      // Label 741: @23080
      GIM_Try, /*On fail goto*//*Label 742*/ 23110, // Rule ID 12011 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_incsspd,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6249:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (INCSSPD GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INCSSPD,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12011,
        GIR_Done,
      // Label 742: @23110
      GIM_Try, /*On fail goto*//*Label 743*/ 23140, // Rule ID 12012 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_incsspq,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6250:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (INCSSPQ GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::INCSSPQ,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12012,
        GIR_Done,
      // Label 743: @23140
      GIM_Try, /*On fail goto*//*Label 744*/ 23170, // Rule ID 12036 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wrpkru,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6572:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (WRPKRU GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRPKRU,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12036,
        GIR_Done,
      // Label 744: @23170
      GIM_Try, /*On fail goto*//*Label 745*/ 23202, // Rule ID 12042 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wrfsbase_32,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6568:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (WRFSBASE GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRFSBASE,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12042,
        GIR_Done,
      // Label 745: @23202
      GIM_Try, /*On fail goto*//*Label 746*/ 23234, // Rule ID 12043 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wrfsbase_64,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6569:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (WRFSBASE64 GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRFSBASE64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12043,
        GIR_Done,
      // Label 746: @23234
      GIM_Try, /*On fail goto*//*Label 747*/ 23266, // Rule ID 12044 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wrgsbase_32,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6570:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (WRGSBASE GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRGSBASE,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12044,
        GIR_Done,
      // Label 747: @23266
      GIM_Try, /*On fail goto*//*Label 748*/ 23298, // Rule ID 12045 //
        GIM_CheckFeatures, GIFBS_HasFSGSBase_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_wrgsbase_64,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6571:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (WRGSBASE64 GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::WRGSBASE64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12045,
        GIR_Done,
      // Label 748: @23298
      GIM_Try, /*On fail goto*//*Label 749*/ 23330, // Rule ID 12050 //
        GIM_CheckFeatures, GIFBS_HasPTWRITE,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_ptwrite32,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        // (intrinsic_void 6335:{ *:[iPTR] }, GR32:{ *:[i32] }:$dst)  =>  (PTWRITEr GR32:{ *:[i32] }:$dst)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PTWRITEr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12050,
        GIR_Done,
      // Label 749: @23330
      GIM_Try, /*On fail goto*//*Label 750*/ 23362, // Rule ID 12051 //
        GIM_CheckFeatures, GIFBS_HasPTWRITE_In64BitMode,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_ptwrite64,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        // (intrinsic_void 6336:{ *:[iPTR] }, GR64:{ *:[i64] }:$dst)  =>  (PTWRITE64r GR64:{ *:[i64] }:$dst)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::PTWRITE64r,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // dst
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12051,
        GIR_Done,
      // Label 750: @23362
      GIM_Reject,
    // Label 720: @23363
    GIM_Try, /*On fail goto*//*Label 751*/ 23453,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
      GIM_Try, /*On fail goto*//*Label 752*/ 23410, // Rule ID 12013 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdsspd,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 6350:{ *:[iPTR] }, GR32:{ *:[i32] }:$src)  =>  (RDSSPD:{ *:[i32] } GR32:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDSSPD,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12013,
        GIR_Done,
      // Label 752: @23410
      GIM_Try, /*On fail goto*//*Label 753*/ 23452, // Rule ID 12014 //
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::x86_rdsspq,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR64RegClassID,
        // (intrinsic_w_chain:{ *:[i64] } 6351:{ *:[iPTR] }, GR64:{ *:[i64] }:$src)  =>  (RDSSPQ:{ *:[i64] } GR64:{ *:[i64] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::RDSSPQ,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12014,
        GIR_Done,
      // Label 753: @23452
      GIM_Reject,
    // Label 751: @23453
    GIM_Try, /*On fail goto*//*Label 754*/ 23581,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/4,
      GIM_Try, /*On fail goto*//*Label 755*/ 23519, // Rule ID 102 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_lwpval32,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_void 6256:{ *:[iPTR] }, GR32:{ *:[i32] }:$src0, GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$cntl)  =>  (LWPVAL32rri GR32:{ *:[i32] }:$src0, GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$cntl)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LWPVAL32rri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src0
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cntl
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 102,
        GIR_Done,
      // Label 755: @23519
      GIM_Try, /*On fail goto*//*Label 756*/ 23580, // Rule ID 104 //
        GIM_CheckFeatures, GIFBS_HasLWP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::x86_lwpval64,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_void 6257:{ *:[iPTR] }, GR64:{ *:[i64] }:$src0, GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$cntl)  =>  (LWPVAL64rri GR64:{ *:[i64] }:$src0, GR32:{ *:[i32] }:$src1, (imm:{ *:[i32] }):$cntl)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LWPVAL64rri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src0
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // cntl
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 104,
        GIR_Done,
      // Label 756: @23580
      GIM_Reject,
    // Label 754: @23581
    GIM_Reject,
    // Label 9: @23582
    GIM_Try, /*On fail goto*//*Label 757*/ 23697,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_SwitchType, /*MI*/0, /*Op*/1, /*[*/1, 3, /*)*//*default:*//*Label 760*/ 23696,
      /*GILLT_s8*//*Label 758*/ 23596,
      /*GILLT_s16*//*Label 759*/ 23654,
      // Label 758: @23596
      GIM_Try, /*On fail goto*//*Label 761*/ 23653,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_Try, /*On fail goto*//*Label 762*/ 23639, // Rule ID 13741 //
          GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
          GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_BITCAST,
          GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s1,
          GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK8RegClassID,
          GIM_CheckIsSafeToFold, /*InsnID*/1,
          // (anyext:{ *:[i32] } (bitconvert:{ *:[i8] } VK8:{ *:[v8i1] }:$src))  =>  (COPY_TO_REGCLASS:{ *:[i32] } VK8:{ *:[v8i1] }:$src, GR32:{ *:[i32] })
          GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
          GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
          GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
          GIR_EraseFromParent, /*InsnID*/0,
          GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR32*/27,
          // GIR_Coverage, 13741,
          GIR_Done,
        // Label 762: @23639
        GIM_Try, /*On fail goto*//*Label 763*/ 23652, // Rule ID 15933 //
          GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
          // (anyext:{ *:[i32] } GR8:{ *:[i8] }:$src)  =>  (MOVZX32rr8:{ *:[i32] } GR8:{ *:[i8] }:$src)
          GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVZX32rr8,
          GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
          // GIR_Coverage, 15933,
          GIR_Done,
        // Label 763: @23652
        GIM_Reject,
      // Label 761: @23653
      GIM_Reject,
      // Label 759: @23654
      GIM_Try, /*On fail goto*//*Label 764*/ 23695, // Rule ID 13739 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_BITCAST,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v16s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK16RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (anyext:{ *:[i32] } (bitconvert:{ *:[i16] } VK16:{ *:[v16i1] }:$src))  =>  (COPY_TO_REGCLASS:{ *:[i32] } VK16:{ *:[v16i1] }:$src, GR32:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR32*/27,
        // GIR_Coverage, 13739,
        GIR_Done,
      // Label 764: @23695
      GIM_Reject,
      // Label 760: @23696
      GIM_Reject,
    // Label 757: @23697
    GIM_Reject,
    // Label 10: @23698
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 22, /*)*//*default:*//*Label 773*/ 24032,
    /*GILLT_s8*//*Label 765*/ 23725,
    /*GILLT_s16*//*Label 766*/ 23804, 0, 0, 0, 0, 0, 0, 0,
    /*GILLT_v4s32*//*Label 767*/ 23842, 0, 0,
    /*GILLT_v8s16*//*Label 768*/ 23866,
    /*GILLT_v8s32*//*Label 769*/ 23913, 0, 0,
    /*GILLT_v16s8*//*Label 770*/ 23937,
    /*GILLT_v16s16*//*Label 771*/ 23984, 0, 0,
    /*GILLT_v32s8*//*Label 772*/ 24008,
    // Label 765: @23725
    GIM_Try, /*On fail goto*//*Label 774*/ 23764, // Rule ID 15981 //
      GIM_CheckFeatures, GIFBS_In64BitMode,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (trunc:{ *:[i8] } GR32:{ *:[i32] }:$src)  =>  (EXTRACT_SUBREG:{ *:[i8] } GR32:{ *:[i32] }:$src, sub_8bit:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopySubReg, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR8*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/1, /*RC GR32*/27,
      // GIR_Coverage, 15981,
      GIR_Done,
    // Label 774: @23764
    GIM_Try, /*On fail goto*//*Label 775*/ 23803, // Rule ID 15982 //
      GIM_CheckFeatures, GIFBS_In64BitMode,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      // (trunc:{ *:[i8] } GR16:{ *:[i16] }:$src)  =>  (EXTRACT_SUBREG:{ *:[i8] } GR16:{ *:[i16] }:$src, sub_8bit:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopySubReg, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR8*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/1, /*RC GR16*/6,
      // GIR_Coverage, 15982,
      GIR_Done,
    // Label 775: @23803
    GIM_Reject,
    // Label 766: @23804
    GIM_Try, /*On fail goto*//*Label 776*/ 23841, // Rule ID 15975 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (trunc:{ *:[i16] } GR32:{ *:[i32] }:$src)  =>  (EXTRACT_SUBREG:{ *:[i16] } GR32:{ *:[i32] }:$src, sub_16bit:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopySubReg, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/4, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GR16*/6,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/1, /*RC GR32*/27,
      // GIR_Coverage, 15975,
      GIR_Done,
    // Label 776: @23841
    GIM_Reject,
    // Label 767: @23842
    GIM_Try, /*On fail goto*//*Label 777*/ 23865, // Rule ID 9395 //
      GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (trunc:{ *:[v4i32] } VR256X:{ *:[v4i64] }:$src1)  =>  (VPMOVQDZ256rr:{ *:[v4i32] } VR256X:{ *:[v4i64] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVQDZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9395,
      GIR_Done,
    // Label 777: @23865
    GIM_Reject,
    // Label 768: @23866
    GIM_Try, /*On fail goto*//*Label 778*/ 23889, // Rule ID 9371 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (trunc:{ *:[v8i16] } VR512:{ *:[v8i64] }:$src1)  =>  (VPMOVQWZrr:{ *:[v8i16] } VR512:{ *:[v8i64] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVQWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9371,
      GIR_Done,
    // Label 778: @23889
    GIM_Try, /*On fail goto*//*Label 779*/ 23912, // Rule ID 9449 //
      GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (trunc:{ *:[v8i16] } VR256X:{ *:[v8i32] }:$src1)  =>  (VPMOVDWZ256rr:{ *:[v8i16] } VR256X:{ *:[v8i32] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVDWZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9449,
      GIR_Done,
    // Label 779: @23912
    GIM_Reject,
    // Label 769: @23913
    GIM_Try, /*On fail goto*//*Label 780*/ 23936, // Rule ID 9398 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (trunc:{ *:[v8i32] } VR512:{ *:[v8i64] }:$src1)  =>  (VPMOVQDZrr:{ *:[v8i32] } VR512:{ *:[v8i64] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVQDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9398,
      GIR_Done,
    // Label 780: @23936
    GIM_Reject,
    // Label 770: @23937
    GIM_Try, /*On fail goto*//*Label 781*/ 23960, // Rule ID 9425 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (trunc:{ *:[v16i8] } VR512:{ *:[v16i32] }:$src1)  =>  (VPMOVDBZrr:{ *:[v16i8] } VR512:{ *:[v16i32] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVDBZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9425,
      GIR_Done,
    // Label 781: @23960
    GIM_Try, /*On fail goto*//*Label 782*/ 23983, // Rule ID 9476 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (trunc:{ *:[v16i8] } VR256X:{ *:[v16i16] }:$src1)  =>  (VPMOVWBZ256rr:{ *:[v16i8] } VR256X:{ *:[v16i16] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVWBZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9476,
      GIR_Done,
    // Label 782: @23983
    GIM_Reject,
    // Label 771: @23984
    GIM_Try, /*On fail goto*//*Label 783*/ 24007, // Rule ID 9452 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (trunc:{ *:[v16i16] } VR512:{ *:[v16i32] }:$src1)  =>  (VPMOVDWZrr:{ *:[v16i16] } VR512:{ *:[v16i32] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVDWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9452,
      GIR_Done,
    // Label 783: @24007
    GIM_Reject,
    // Label 772: @24008
    GIM_Try, /*On fail goto*//*Label 784*/ 24031, // Rule ID 9479 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (trunc:{ *:[v32i8] } VR512:{ *:[v32i16] }:$src1)  =>  (VPMOVWBZrr:{ *:[v32i8] } VR512:{ *:[v32i16] }:$src1)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVWBZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9479,
      GIR_Done,
    // Label 784: @24031
    GIM_Reject,
    // Label 773: @24032
    GIM_Reject,
    // Label 11: @24033
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 5, /*)*//*default:*//*Label 789*/ 24235,
    /*GILLT_s8*//*Label 785*/ 24043,
    /*GILLT_s16*//*Label 786*/ 24065,
    /*GILLT_s32*//*Label 787*/ 24087,
    /*GILLT_s64*//*Label 788*/ 24184,
    // Label 785: @24043
    GIM_Try, /*On fail goto*//*Label 790*/ 24064, // Rule ID 19 //
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (imm:{ *:[i8] }):$src  =>  (MOV8ri:{ *:[i8] } (imm:{ *:[i8] }):$src)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV8ri,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 19,
      GIR_Done,
    // Label 790: @24064
    GIM_Reject,
    // Label 786: @24065
    GIM_Try, /*On fail goto*//*Label 791*/ 24086, // Rule ID 20 //
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (imm:{ *:[i16] }):$src  =>  (MOV16ri:{ *:[i16] } (imm:{ *:[i16] }):$src)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV16ri,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 20,
      GIR_Done,
    // Label 791: @24086
    GIM_Reject,
    // Label 787: @24087
    GIM_Try, /*On fail goto*//*Label 792*/ 24109, // Rule ID 12069 //
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckLiteralInt, /*MI*/0, /*Op*/1, 0,
      // 0:{ *:[i32] }  =>  (MOV32r0:{ *:[i32] }:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV32r0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12069,
      GIR_Done,
    // Label 792: @24109
    GIM_Try, /*On fail goto*//*Label 793*/ 24133, // Rule ID 12070 //
      GIM_CheckFeatures, GIFBS_Not64BitMode_OptForSize,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckLiteralInt, /*MI*/0, /*Op*/1, 1,
      // 1:{ *:[i32] }  =>  (MOV32r1:{ *:[i32] }:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV32r1,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12070,
      GIR_Done,
    // Label 793: @24133
    GIM_Try, /*On fail goto*//*Label 794*/ 24157, // Rule ID 12071 //
      GIM_CheckFeatures, GIFBS_Not64BitMode_OptForSize,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckLiteralInt, /*MI*/0, /*Op*/1, -1,
      // -1:{ *:[i32] }  =>  (MOV32r_1:{ *:[i32] }:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV32r_1,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12071,
      GIR_Done,
    // Label 794: @24157
    GIM_Try, /*On fail goto*//*Label 795*/ 24183, // Rule ID 12072 //
      GIM_CheckFeatures, GIFBS_NotWin64WithoutFP_OptForMinSize,
      GIM_CheckI64ImmPredicate, /*MI*/0, /*Predicate*/GIPFP_I64_Predicate_i32immSExt8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (imm:{ *:[i32] })<<P:Predicate_i32immSExt8>>:$src  =>  (MOV32ImmSExti8:{ *:[i32] } (imm:{ *:[i32] }):$src)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV32ImmSExti8,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12072,
      GIR_Done,
    // Label 795: @24183
    GIM_Reject,
    // Label 788: @24184
    GIM_Try, /*On fail goto*//*Label 796*/ 24210, // Rule ID 12073 //
      GIM_CheckFeatures, GIFBS_NotWin64WithoutFP_OptForMinSize,
      GIM_CheckI64ImmPredicate, /*MI*/0, /*Predicate*/GIPFP_I64_Predicate_i64immSExt8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (imm:{ *:[i64] })<<P:Predicate_i64immSExt8>>:$src  =>  (MOV64ImmSExti8:{ *:[i64] } (imm:{ *:[i64] }):$src)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV64ImmSExti8,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12073,
      GIR_Done,
    // Label 796: @24210
    GIM_Try, /*On fail goto*//*Label 797*/ 24234, // Rule ID 22 //
      GIM_CheckI64ImmPredicate, /*MI*/0, /*Predicate*/GIPFP_I64_Predicate_i64immSExt32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (imm:{ *:[i64] })<<P:Predicate_i64immSExt32>>:$src  =>  (MOV64ri32:{ *:[i64] } (imm:{ *:[i64] }):$src)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::MOV64ri32,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 22,
      GIR_Done,
    // Label 797: @24234
    GIM_Reject,
    // Label 789: @24235
    GIM_Reject,
    // Label 12: @24236
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 6, /*)*//*default:*//*Label 801*/ 24428,
    /*GILLT_s32*//*Label 798*/ 24245,
    /*GILLT_s64*//*Label 799*/ 24292,
    /*GILLT_s80*//*Label 800*/ 24385,
    // Label 798: @24245
    GIM_Try, /*On fail goto*//*Label 802*/ 24268, // Rule ID 891 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm0,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f32] })<<P:Predicate_fpimm0>>  =>  (LD_Fp032:{ *:[f32] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp032,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 891,
      GIR_Done,
    // Label 802: @24268
    GIM_Try, /*On fail goto*//*Label 803*/ 24291, // Rule ID 892 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f32] })<<P:Predicate_fpimm1>>  =>  (LD_Fp132:{ *:[f32] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp132,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 892,
      GIR_Done,
    // Label 803: @24291
    GIM_Reject,
    // Label 799: @24292
    GIM_Try, /*On fail goto*//*Label 804*/ 24315, // Rule ID 893 //
      GIM_CheckFeatures, GIFBS_FPStackf64,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm0,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f64] })<<P:Predicate_fpimm0>>  =>  (LD_Fp064:{ *:[f64] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp064,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 893,
      GIR_Done,
    // Label 804: @24315
    GIM_Try, /*On fail goto*//*Label 805*/ 24338, // Rule ID 894 //
      GIM_CheckFeatures, GIFBS_FPStackf64,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f64] })<<P:Predicate_fpimm1>>  =>  (LD_Fp164:{ *:[f64] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp164,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 894,
      GIR_Done,
    // Label 805: @24338
    GIM_Try, /*On fail goto*//*Label 806*/ 24361, // Rule ID 1281 //
      GIM_CheckFeatures, GIFBS_HasSSE2_NoAVX512,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm0,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f64] })<<P:Predicate_fpimm0>>  =>  (FsFLD0SD:{ *:[f64] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::FsFLD0SD,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1281,
      GIR_Done,
    // Label 806: @24361
    GIM_Try, /*On fail goto*//*Label 807*/ 24384, // Rule ID 2784 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm0,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f64] })<<P:Predicate_fpimm0>>  =>  (AVX512_FsFLD0SD:{ *:[f64] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::AVX512_FsFLD0SD,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2784,
      GIR_Done,
    // Label 807: @24384
    GIM_Reject,
    // Label 800: @24385
    GIM_Try, /*On fail goto*//*Label 808*/ 24406, // Rule ID 895 //
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm0,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f80] })<<P:Predicate_fpimm0>>  =>  (LD_Fp080:{ *:[f80] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp080,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 895,
      GIR_Done,
    // Label 808: @24406
    GIM_Try, /*On fail goto*//*Label 809*/ 24427, // Rule ID 896 //
      GIM_CheckAPFloatImmPredicate, /*MI*/0, /*Predicate*/GIPFP_APFloat_Predicate_fpimm1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      // MIs[0] Operand 1
      // No operand predicates
      // (fpimm:{ *:[f80] })<<P:Predicate_fpimm1>>  =>  (LD_Fp180:{ *:[f80] }:{ *:[i16] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::LD_Fp180,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 896,
      GIR_Done,
    // Label 809: @24427
    GIM_Reject,
    // Label 801: @24428
    GIM_Reject,
    // Label 13: @24429
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 25, /*)*//*default:*//*Label 824*/ 24932,
    /*GILLT_s32*//*Label 810*/ 24457,
    /*GILLT_s64*//*Label 811*/ 24500, 0, 0, 0,
    /*GILLT_v2s64*//*Label 812*/ 24566, 0,
    /*GILLT_v4s32*//*Label 813*/ 24590,
    /*GILLT_v4s64*//*Label 814*/ 24614, 0,
    /*GILLT_v8s16*//*Label 815*/ 24638,
    /*GILLT_v8s32*//*Label 816*/ 24662,
    /*GILLT_v8s64*//*Label 817*/ 24686, 0,
    /*GILLT_v16s8*//*Label 818*/ 24710,
    /*GILLT_v16s16*//*Label 819*/ 24773,
    /*GILLT_v16s32*//*Label 820*/ 24836, 0,
    /*GILLT_v32s8*//*Label 821*/ 24860,
    /*GILLT_v32s16*//*Label 822*/ 24884, 0,
    /*GILLT_v64s8*//*Label 823*/ 24908,
    // Label 810: @24457
    GIM_Try, /*On fail goto*//*Label 825*/ 24478, // Rule ID 536 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      // (sext:{ *:[i32] } GR8:{ *:[i8] }:$src)  =>  (MOVSX32rr8:{ *:[i32] } GR8:{ *:[i8] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSX32rr8,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 536,
      GIR_Done,
    // Label 825: @24478
    GIM_Try, /*On fail goto*//*Label 826*/ 24499, // Rule ID 538 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      // (sext:{ *:[i32] } GR16:{ *:[i16] }:$src)  =>  (MOVSX32rr16:{ *:[i32] } GR16:{ *:[i16] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSX32rr16,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 538,
      GIR_Done,
    // Label 826: @24499
    GIM_Reject,
    // Label 811: @24500
    GIM_Try, /*On fail goto*//*Label 827*/ 24521, // Rule ID 544 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      // (sext:{ *:[i64] } GR8:{ *:[i8] }:$src)  =>  (MOVSX64rr8:{ *:[i64] } GR8:{ *:[i8] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSX64rr8,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 544,
      GIR_Done,
    // Label 827: @24521
    GIM_Try, /*On fail goto*//*Label 828*/ 24542, // Rule ID 546 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      // (sext:{ *:[i64] } GR16:{ *:[i16] }:$src)  =>  (MOVSX64rr16:{ *:[i64] } GR16:{ *:[i16] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSX64rr16,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 546,
      GIR_Done,
    // Label 828: @24542
    GIM_Try, /*On fail goto*//*Label 829*/ 24565, // Rule ID 548 //
      GIM_CheckFeatures, GIFBS_In64BitMode,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sext:{ *:[i64] } GR32:{ *:[i32] }:$src)  =>  (MOVSX64rr32:{ *:[i64] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVSX64rr32,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 548,
      GIR_Done,
    // Label 829: @24565
    GIM_Reject,
    // Label 812: @24566
    GIM_Try, /*On fail goto*//*Label 830*/ 24589, // Rule ID 9775 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK2RegClassID,
      // (sext:{ *:[v2i64] } VK2:{ *:[v2i1] }:$src)  =>  (VPMOVM2QZ128rr:{ *:[v2i64] } VK2:{ *:[v2i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2QZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9775,
      GIR_Done,
    // Label 830: @24589
    GIM_Reject,
    // Label 813: @24590
    GIM_Try, /*On fail goto*//*Label 831*/ 24613, // Rule ID 9772 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK4RegClassID,
      // (sext:{ *:[v4i32] } VK4:{ *:[v4i1] }:$src)  =>  (VPMOVM2DZ128rr:{ *:[v4i32] } VK4:{ *:[v4i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2DZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9772,
      GIR_Done,
    // Label 831: @24613
    GIM_Reject,
    // Label 814: @24614
    GIM_Try, /*On fail goto*//*Label 832*/ 24637, // Rule ID 9774 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK4RegClassID,
      // (sext:{ *:[v4i64] } VK4:{ *:[v4i1] }:$src)  =>  (VPMOVM2QZ256rr:{ *:[v4i64] } VK4:{ *:[v4i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2QZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9774,
      GIR_Done,
    // Label 832: @24637
    GIM_Reject,
    // Label 815: @24638
    GIM_Try, /*On fail goto*//*Label 833*/ 24661, // Rule ID 9769 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      // (sext:{ *:[v8i16] } VK8:{ *:[v8i1] }:$src)  =>  (VPMOVM2WZ128rr:{ *:[v8i16] } VK8:{ *:[v8i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2WZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9769,
      GIR_Done,
    // Label 833: @24661
    GIM_Reject,
    // Label 816: @24662
    GIM_Try, /*On fail goto*//*Label 834*/ 24685, // Rule ID 9771 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      // (sext:{ *:[v8i32] } VK8:{ *:[v8i1] }:$src)  =>  (VPMOVM2DZ256rr:{ *:[v8i32] } VK8:{ *:[v8i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2DZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9771,
      GIR_Done,
    // Label 834: @24685
    GIM_Reject,
    // Label 817: @24686
    GIM_Try, /*On fail goto*//*Label 835*/ 24709, // Rule ID 9773 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK8RegClassID,
      // (sext:{ *:[v8i64] } VK8:{ *:[v8i1] }:$src)  =>  (VPMOVM2QZrr:{ *:[v8i64] } VK8:{ *:[v8i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2QZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9773,
      GIR_Done,
    // Label 835: @24709
    GIM_Reject,
    // Label 818: @24710
    GIM_Try, /*On fail goto*//*Label 836*/ 24772,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      GIM_Try, /*On fail goto*//*Label 837*/ 24735, // Rule ID 9766 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        // (sext:{ *:[v16i8] } VK16:{ *:[v16i1] }:$src)  =>  (VPMOVM2BZ128rr:{ *:[v16i8] } VK16:{ *:[v16i1] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2BZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 9766,
        GIR_Done,
      // Label 837: @24735
      GIM_Try, /*On fail goto*//*Label 838*/ 24771, // Rule ID 15227 //
        GIM_CheckFeatures, GIFBS_HasDQI_NoBWI,
        // (sext:{ *:[v16i8] } VK16:{ *:[v16i1] }:$src)  =>  (VPMOVDBZrr:{ *:[v16i8] } (VPMOVM2DZrr:{ *:[v16i32] } VK16:{ *:[v16i1] }:$src))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::VPMOVM2DZrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMOVDBZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15227,
        GIR_Done,
      // Label 838: @24771
      GIM_Reject,
    // Label 836: @24772
    GIM_Reject,
    // Label 819: @24773
    GIM_Try, /*On fail goto*//*Label 839*/ 24835,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      GIM_Try, /*On fail goto*//*Label 840*/ 24798, // Rule ID 9768 //
        GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
        // (sext:{ *:[v16i16] } VK16:{ *:[v16i1] }:$src)  =>  (VPMOVM2WZ256rr:{ *:[v16i16] } VK16:{ *:[v16i1] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2WZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 9768,
        GIR_Done,
      // Label 840: @24798
      GIM_Try, /*On fail goto*//*Label 841*/ 24834, // Rule ID 15228 //
        GIM_CheckFeatures, GIFBS_HasDQI_NoBWI,
        // (sext:{ *:[v16i16] } VK16:{ *:[v16i1] }:$src)  =>  (VPMOVDWZrr:{ *:[v16i16] } (VPMOVM2DZrr:{ *:[v16i32] } VK16:{ *:[v16i1] }:$src))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/X86::VPMOVM2DZrr,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VPMOVDWZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15228,
        GIR_Done,
      // Label 841: @24834
      GIM_Reject,
    // Label 839: @24835
    GIM_Reject,
    // Label 820: @24836
    GIM_Try, /*On fail goto*//*Label 842*/ 24859, // Rule ID 9770 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK16RegClassID,
      // (sext:{ *:[v16i32] } VK16:{ *:[v16i1] }:$src)  =>  (VPMOVM2DZrr:{ *:[v16i32] } VK16:{ *:[v16i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2DZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9770,
      GIR_Done,
    // Label 842: @24859
    GIM_Reject,
    // Label 821: @24860
    GIM_Try, /*On fail goto*//*Label 843*/ 24883, // Rule ID 9765 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      // (sext:{ *:[v32i8] } VK32:{ *:[v32i1] }:$src)  =>  (VPMOVM2BZ256rr:{ *:[v32i8] } VK32:{ *:[v32i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2BZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9765,
      GIR_Done,
    // Label 843: @24883
    GIM_Reject,
    // Label 822: @24884
    GIM_Try, /*On fail goto*//*Label 844*/ 24907, // Rule ID 9767 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK32RegClassID,
      // (sext:{ *:[v32i16] } VK32:{ *:[v32i1] }:$src)  =>  (VPMOVM2WZrr:{ *:[v32i16] } VK32:{ *:[v32i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2WZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9767,
      GIR_Done,
    // Label 844: @24907
    GIM_Reject,
    // Label 823: @24908
    GIM_Try, /*On fail goto*//*Label 845*/ 24931, // Rule ID 9764 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v64s1,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VK64RegClassID,
      // (sext:{ *:[v64i8] } VK64:{ *:[v64i1] }:$src)  =>  (VPMOVM2BZrr:{ *:[v64i8] } VK64:{ *:[v64i1] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPMOVM2BZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 9764,
      GIR_Done,
    // Label 845: @24931
    GIM_Reject,
    // Label 824: @24932
    GIM_Reject,
    // Label 14: @24933
    GIM_Try, /*On fail goto*//*Label 846*/ 25067,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_SwitchType, /*MI*/0, /*Op*/1, /*[*/1, 3, /*)*//*default:*//*Label 849*/ 25045,
      /*GILLT_s8*//*Label 847*/ 24947,
      /*GILLT_s16*//*Label 848*/ 25005,
      // Label 847: @24947
      GIM_Try, /*On fail goto*//*Label 850*/ 25004,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_Try, /*On fail goto*//*Label 851*/ 24990, // Rule ID 13740 //
          GIM_CheckFeatures, GIFBS_HasDQI,
          GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
          GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_BITCAST,
          GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s1,
          GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK8RegClassID,
          GIM_CheckIsSafeToFold, /*InsnID*/1,
          // (zext:{ *:[i32] } (bitconvert:{ *:[i8] } VK8:{ *:[v8i1] }:$src))  =>  (KMOVBrk:{ *:[i32] } VK8:{ *:[v8i1] }:$src)
          GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::KMOVBrk,
          GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
          GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
          GIR_EraseFromParent, /*InsnID*/0,
          GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
          // GIR_Coverage, 13740,
          GIR_Done,
        // Label 851: @24990
        GIM_Try, /*On fail goto*//*Label 852*/ 25003, // Rule ID 540 //
          GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
          // (zext:{ *:[i32] } GR8:{ *:[i8] }:$src)  =>  (MOVZX32rr8:{ *:[i32] } GR8:{ *:[i8] }:$src)
          GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVZX32rr8,
          GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
          // GIR_Coverage, 540,
          GIR_Done,
        // Label 852: @25003
        GIM_Reject,
      // Label 850: @25004
      GIM_Reject,
      // Label 848: @25005
      GIM_Try, /*On fail goto*//*Label 853*/ 25044, // Rule ID 13738 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_BITCAST,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v16s1,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::VK16RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (zext:{ *:[i32] } (bitconvert:{ *:[i16] } VK16:{ *:[v16i1] }:$src))  =>  (KMOVWrk:{ *:[i32] } VK16:{ *:[v16i1] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::KMOVWrk,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 13738,
        GIR_Done,
      // Label 853: @25044
      GIM_Reject,
      // Label 849: @25045
      GIM_Try, /*On fail goto*//*Label 854*/ 25066, // Rule ID 542 //
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
        // (zext:{ *:[i32] } GR16:{ *:[i16] }:$src)  =>  (MOVZX32rr16:{ *:[i32] } GR16:{ *:[i16] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MOVZX32rr16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 542,
        GIR_Done,
      // Label 854: @25066
      GIM_Reject,
    // Label 846: @25067
    GIM_Reject,
    // Label 15: @25068
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 23, /*)*//*default:*//*Label 868*/ 25792,
    /*GILLT_s8*//*Label 855*/ 25096,
    /*GILLT_s16*//*Label 856*/ 25172,
    /*GILLT_s32*//*Label 857*/ 25248,
    /*GILLT_s64*//*Label 858*/ 25324, 0, 0, 0,
    /*GILLT_v2s64*//*Label 859*/ 25400, 0,
    /*GILLT_v4s32*//*Label 860*/ 25458,
    /*GILLT_v4s64*//*Label 861*/ 25516, 0,
    /*GILLT_v8s16*//*Label 862*/ 25574,
    /*GILLT_v8s32*//*Label 863*/ 25606,
    /*GILLT_v8s64*//*Label 864*/ 25664, 0, 0,
    /*GILLT_v16s16*//*Label 865*/ 25696,
    /*GILLT_v16s32*//*Label 866*/ 25728, 0, 0,
    /*GILLT_v32s16*//*Label 867*/ 25760,
    // Label 855: @25096
    GIM_Try, /*On fail goto*//*Label 869*/ 25171,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 870*/ 25140, // Rule ID 15997 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (shl:{ *:[i8] } GR8:{ *:[i8] }:$src1, 1:{ *:[i8] })  =>  (ADD8rr:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, GR8:{ *:[i8] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD8rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15997,
        GIR_Done,
      // Label 870: @25140
      GIM_Try, /*On fail goto*//*Label 871*/ 25170, // Rule ID 599 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHL8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHL8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 599,
        GIR_Done,
      // Label 871: @25170
      GIM_Reject,
    // Label 869: @25171
    GIM_Reject,
    // Label 856: @25172
    GIM_Try, /*On fail goto*//*Label 872*/ 25247,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 873*/ 25216, // Rule ID 15998 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (shl:{ *:[i16] } GR16:{ *:[i16] }:$src1, 1:{ *:[i8] })  =>  (ADD16rr:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, GR16:{ *:[i16] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD16rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15998,
        GIR_Done,
      // Label 873: @25216
      GIM_Try, /*On fail goto*//*Label 874*/ 25246, // Rule ID 600 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHL16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHL16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 600,
        GIR_Done,
      // Label 874: @25246
      GIM_Reject,
    // Label 872: @25247
    GIM_Reject,
    // Label 857: @25248
    GIM_Try, /*On fail goto*//*Label 875*/ 25323,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 876*/ 25292, // Rule ID 15999 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (shl:{ *:[i32] } GR32:{ *:[i32] }:$src1, 1:{ *:[i8] })  =>  (ADD32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, GR32:{ *:[i32] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 15999,
        GIR_Done,
      // Label 876: @25292
      GIM_Try, /*On fail goto*//*Label 877*/ 25322, // Rule ID 601 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHL32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHL32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 601,
        GIR_Done,
      // Label 877: @25322
      GIM_Reject,
    // Label 875: @25323
    GIM_Reject,
    // Label 858: @25324
    GIM_Try, /*On fail goto*//*Label 878*/ 25399,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 879*/ 25368, // Rule ID 16000 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (shl:{ *:[i64] } GR64:{ *:[i64] }:$src1, 1:{ *:[i8] })  =>  (ADD64rr:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, GR64:{ *:[i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ADD64rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 16000,
        GIR_Done,
      // Label 879: @25368
      GIM_Try, /*On fail goto*//*Label 880*/ 25398, // Rule ID 602 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHL64ri:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHL64ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 602,
        GIR_Done,
      // Label 880: @25398
      GIM_Reject,
    // Label 878: @25399
    GIM_Reject,
    // Label 859: @25400
    GIM_Try, /*On fail goto*//*Label 881*/ 25457,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 882*/ 25433, // Rule ID 2727 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (shl:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPSLLVQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2727,
        GIR_Done,
      // Label 882: @25433
      GIM_Try, /*On fail goto*//*Label 883*/ 25456, // Rule ID 6401 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (shl:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPSLLVQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6401,
        GIR_Done,
      // Label 883: @25456
      GIM_Reject,
    // Label 881: @25457
    GIM_Reject,
    // Label 860: @25458
    GIM_Try, /*On fail goto*//*Label 884*/ 25515,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 885*/ 25491, // Rule ID 2723 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (shl:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPSLLVDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2723,
        GIR_Done,
      // Label 885: @25491
      GIM_Try, /*On fail goto*//*Label 886*/ 25514, // Rule ID 6374 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (shl:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPSLLVDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6374,
        GIR_Done,
      // Label 886: @25514
      GIM_Reject,
    // Label 884: @25515
    GIM_Reject,
    // Label 861: @25516
    GIM_Try, /*On fail goto*//*Label 887*/ 25573,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 888*/ 25549, // Rule ID 2729 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (shl:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPSLLVQYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVQYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2729,
        GIR_Done,
      // Label 888: @25549
      GIM_Try, /*On fail goto*//*Label 889*/ 25572, // Rule ID 6392 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (shl:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPSLLVQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6392,
        GIR_Done,
      // Label 889: @25572
      GIM_Reject,
    // Label 887: @25573
    GIM_Reject,
    // Label 862: @25574
    GIM_Try, /*On fail goto*//*Label 890*/ 25605, // Rule ID 6422 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
      // (shl:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPSLLVWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVWZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6422,
      GIR_Done,
    // Label 890: @25605
    GIM_Reject,
    // Label 863: @25606
    GIM_Try, /*On fail goto*//*Label 891*/ 25663,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 892*/ 25639, // Rule ID 2725 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (shl:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPSLLVDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2725,
        GIR_Done,
      // Label 892: @25639
      GIM_Try, /*On fail goto*//*Label 893*/ 25662, // Rule ID 6365 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (shl:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPSLLVDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6365,
        GIR_Done,
      // Label 893: @25662
      GIM_Reject,
    // Label 891: @25663
    GIM_Reject,
    // Label 864: @25664
    GIM_Try, /*On fail goto*//*Label 894*/ 25695, // Rule ID 6383 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (shl:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPSLLVQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6383,
      GIR_Done,
    // Label 894: @25695
    GIM_Reject,
    // Label 865: @25696
    GIM_Try, /*On fail goto*//*Label 895*/ 25727, // Rule ID 6416 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
      // (shl:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPSLLVWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVWZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6416,
      GIR_Done,
    // Label 895: @25727
    GIM_Reject,
    // Label 866: @25728
    GIM_Try, /*On fail goto*//*Label 896*/ 25759, // Rule ID 6356 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (shl:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPSLLVDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6356,
      GIR_Done,
    // Label 896: @25759
    GIM_Reject,
    // Label 867: @25760
    GIM_Try, /*On fail goto*//*Label 897*/ 25791, // Rule ID 6410 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (shl:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPSLLVWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSLLVWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6410,
      GIR_Done,
    // Label 897: @25791
    GIM_Reject,
    // Label 868: @25792
    GIM_Reject,
    // Label 16: @25793
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 23, /*)*//*default:*//*Label 911*/ 26623,
    /*GILLT_s8*//*Label 898*/ 25821,
    /*GILLT_s16*//*Label 899*/ 25893,
    /*GILLT_s32*//*Label 900*/ 25965,
    /*GILLT_s64*//*Label 901*/ 26159, 0, 0, 0,
    /*GILLT_v2s64*//*Label 902*/ 26231, 0,
    /*GILLT_v4s32*//*Label 903*/ 26289,
    /*GILLT_v4s64*//*Label 904*/ 26347, 0,
    /*GILLT_v8s16*//*Label 905*/ 26405,
    /*GILLT_v8s32*//*Label 906*/ 26437,
    /*GILLT_v8s64*//*Label 907*/ 26495, 0, 0,
    /*GILLT_v16s16*//*Label 908*/ 26527,
    /*GILLT_v16s32*//*Label 909*/ 26559, 0, 0,
    /*GILLT_v32s16*//*Label 910*/ 26591,
    // Label 898: @25821
    GIM_Try, /*On fail goto*//*Label 912*/ 25892,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 913*/ 25861, // Rule ID 623 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (srl:{ *:[i8] } GR8:{ *:[i8] }:$src1, 1:{ *:[i8] })  =>  (SHR8r1:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR8r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 623,
        GIR_Done,
      // Label 913: @25861
      GIM_Try, /*On fail goto*//*Label 914*/ 25891, // Rule ID 619 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHR8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 619,
        GIR_Done,
      // Label 914: @25891
      GIM_Reject,
    // Label 912: @25892
    GIM_Reject,
    // Label 899: @25893
    GIM_Try, /*On fail goto*//*Label 915*/ 25964,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 916*/ 25933, // Rule ID 624 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (srl:{ *:[i16] } GR16:{ *:[i16] }:$src1, 1:{ *:[i8] })  =>  (SHR16r1:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR16r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 624,
        GIR_Done,
      // Label 916: @25933
      GIM_Try, /*On fail goto*//*Label 917*/ 25963, // Rule ID 620 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHR16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 620,
        GIR_Done,
      // Label 917: @25963
      GIM_Reject,
    // Label 915: @25964
    GIM_Reject,
    // Label 900: @25965
    GIM_Try, /*On fail goto*//*Label 918*/ 26158,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 919*/ 26097, // Rule ID 12257 //
        GIM_CheckFeatures, GIFBS_HasBMI2,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SHL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s8,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/3, /*MI*/2, /*OpIdx*/1, // MIs[3]
        GIM_CheckOpcode, /*MI*/3, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/3, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/3, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/3, /*Op*/1, 32,
        GIM_CheckRegBankForClass, /*MI*/3, /*Op*/2, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/4, /*MI*/0, /*OpIdx*/2, // MIs[4]
        GIM_CheckOpcode, /*MI*/4, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/4, /*Op*/1, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/5, /*MI*/4, /*OpIdx*/1, // MIs[5]
        GIM_CheckOpcode, /*MI*/5, TargetOpcode::G_SUB,
        GIM_CheckType, /*MI*/5, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckConstantInt, /*MI*/5, /*Op*/1, 32,
        // MIs[5] lz
        GIM_CheckIsSameOperand, /*MI*/5, /*OpIdx*/2, /*OtherMI*/3, /*OtherOpIdx*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        GIM_CheckIsSafeToFold, /*InsnID*/3,
        GIM_CheckIsSafeToFold, /*InsnID*/4,
        GIM_CheckIsSafeToFold, /*InsnID*/5,
        // (srl:{ *:[i32] } (shl:{ *:[i32] } GR32:{ *:[i32] }:$src, (trunc:{ *:[i8] } (sub:{ *:[i32] } 32:{ *:[i32] }, GR32:{ *:[i32] }:$lz))), (trunc:{ *:[i8] } (sub:{ *:[i32] } 32:{ *:[i32] }, GR32:{ *:[i32] }:$lz)))  =>  (BZHI32rr:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src, GR32:{ *:[i32] }:$lz)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::BZHI32rr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/3, /*OpIdx*/2, // lz
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12257,
        GIR_Done,
      // Label 919: @26097
      GIM_Try, /*On fail goto*//*Label 920*/ 26123, // Rule ID 625 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (srl:{ *:[i32] } GR32:{ *:[i32] }:$src1, 1:{ *:[i8] })  =>  (SHR32r1:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR32r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 625,
        GIR_Done,
      // Label 920: @26123
      GIM_Try, /*On fail goto*//*Label 921*/ 26157, // Rule ID 621 //
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHR32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 621,
        GIR_Done,
      // Label 921: @26157
      GIM_Reject,
    // Label 918: @26158
    GIM_Reject,
    // Label 901: @26159
    GIM_Try, /*On fail goto*//*Label 922*/ 26230,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 923*/ 26199, // Rule ID 626 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (srl:{ *:[i64] } GR64:{ *:[i64] }:$src1, 1:{ *:[i8] })  =>  (SHR64r1:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR64r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 626,
        GIR_Done,
      // Label 923: @26199
      GIM_Try, /*On fail goto*//*Label 924*/ 26229, // Rule ID 622 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SHR64ri:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SHR64ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 622,
        GIR_Done,
      // Label 924: @26229
      GIM_Reject,
    // Label 922: @26230
    GIM_Reject,
    // Label 902: @26231
    GIM_Try, /*On fail goto*//*Label 925*/ 26288,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 926*/ 26264, // Rule ID 2735 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (srl:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)  =>  (VPSRLVQrr:{ *:[v2i64] } VR128:{ *:[v2i64] }:$src1, VR128:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVQrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2735,
        GIR_Done,
      // Label 926: @26264
      GIM_Try, /*On fail goto*//*Label 927*/ 26287, // Rule ID 6545 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (srl:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPSRLVQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVQZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6545,
        GIR_Done,
      // Label 927: @26287
      GIM_Reject,
    // Label 925: @26288
    GIM_Reject,
    // Label 903: @26289
    GIM_Try, /*On fail goto*//*Label 928*/ 26346,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 929*/ 26322, // Rule ID 2731 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (srl:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPSRLVDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2731,
        GIR_Done,
      // Label 929: @26322
      GIM_Try, /*On fail goto*//*Label 930*/ 26345, // Rule ID 6518 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (srl:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPSRLVDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6518,
        GIR_Done,
      // Label 930: @26345
      GIM_Reject,
    // Label 928: @26346
    GIM_Reject,
    // Label 904: @26347
    GIM_Try, /*On fail goto*//*Label 931*/ 26404,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 932*/ 26380, // Rule ID 2737 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (srl:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)  =>  (VPSRLVQYrr:{ *:[v4i64] } VR256:{ *:[v4i64] }:$src1, VR256:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVQYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2737,
        GIR_Done,
      // Label 932: @26380
      GIM_Try, /*On fail goto*//*Label 933*/ 26403, // Rule ID 6536 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (srl:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPSRLVQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVQZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6536,
        GIR_Done,
      // Label 933: @26403
      GIM_Reject,
    // Label 931: @26404
    GIM_Reject,
    // Label 905: @26405
    GIM_Try, /*On fail goto*//*Label 934*/ 26436, // Rule ID 6566 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
      // (srl:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPSRLVWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVWZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6566,
      GIR_Done,
    // Label 934: @26436
    GIM_Reject,
    // Label 906: @26437
    GIM_Try, /*On fail goto*//*Label 935*/ 26494,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 936*/ 26470, // Rule ID 2733 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (srl:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPSRLVDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2733,
        GIR_Done,
      // Label 936: @26470
      GIM_Try, /*On fail goto*//*Label 937*/ 26493, // Rule ID 6509 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (srl:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPSRLVDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6509,
        GIR_Done,
      // Label 937: @26493
      GIM_Reject,
    // Label 935: @26494
    GIM_Reject,
    // Label 907: @26495
    GIM_Try, /*On fail goto*//*Label 938*/ 26526, // Rule ID 6527 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (srl:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPSRLVQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6527,
      GIR_Done,
    // Label 938: @26526
    GIM_Reject,
    // Label 908: @26527
    GIM_Try, /*On fail goto*//*Label 939*/ 26558, // Rule ID 6560 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
      // (srl:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPSRLVWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVWZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6560,
      GIR_Done,
    // Label 939: @26558
    GIM_Reject,
    // Label 909: @26559
    GIM_Try, /*On fail goto*//*Label 940*/ 26590, // Rule ID 6500 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (srl:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPSRLVDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6500,
      GIR_Done,
    // Label 940: @26590
    GIM_Reject,
    // Label 910: @26591
    GIM_Try, /*On fail goto*//*Label 941*/ 26622, // Rule ID 6554 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (srl:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPSRLVWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRLVWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6554,
      GIR_Done,
    // Label 941: @26622
    GIM_Reject,
    // Label 911: @26623
    GIM_Reject,
    // Label 17: @26624
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 23, /*)*//*default:*//*Label 955*/ 27280,
    /*GILLT_s8*//*Label 942*/ 26652,
    /*GILLT_s16*//*Label 943*/ 26724,
    /*GILLT_s32*//*Label 944*/ 26796,
    /*GILLT_s64*//*Label 945*/ 26868, 0, 0, 0,
    /*GILLT_v2s64*//*Label 946*/ 26940, 0,
    /*GILLT_v4s32*//*Label 947*/ 26972,
    /*GILLT_v4s64*//*Label 948*/ 27030, 0,
    /*GILLT_v8s16*//*Label 949*/ 27062,
    /*GILLT_v8s32*//*Label 950*/ 27094,
    /*GILLT_v8s64*//*Label 951*/ 27152, 0, 0,
    /*GILLT_v16s16*//*Label 952*/ 27184,
    /*GILLT_v16s32*//*Label 953*/ 27216, 0, 0,
    /*GILLT_v32s16*//*Label 954*/ 27248,
    // Label 942: @26652
    GIM_Try, /*On fail goto*//*Label 956*/ 26723,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR8RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR8RegClassID,
      GIM_Try, /*On fail goto*//*Label 957*/ 26692, // Rule ID 647 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (sra:{ *:[i8] } GR8:{ *:[i8] }:$src1, 1:{ *:[i8] })  =>  (SAR8r1:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR8r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 647,
        GIR_Done,
      // Label 957: @26692
      GIM_Try, /*On fail goto*//*Label 958*/ 26722, // Rule ID 643 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i8] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SAR8ri:{ *:[i8] }:{ *:[i32] } GR8:{ *:[i8] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR8ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 643,
        GIR_Done,
      // Label 958: @26722
      GIM_Reject,
    // Label 956: @26723
    GIM_Reject,
    // Label 943: @26724
    GIM_Try, /*On fail goto*//*Label 959*/ 26795,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      GIM_Try, /*On fail goto*//*Label 960*/ 26764, // Rule ID 648 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (sra:{ *:[i16] } GR16:{ *:[i16] }:$src1, 1:{ *:[i8] })  =>  (SAR16r1:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR16r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 648,
        GIR_Done,
      // Label 960: @26764
      GIM_Try, /*On fail goto*//*Label 961*/ 26794, // Rule ID 644 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i16] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SAR16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR16ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 644,
        GIR_Done,
      // Label 961: @26794
      GIM_Reject,
    // Label 959: @26795
    GIM_Reject,
    // Label 944: @26796
    GIM_Try, /*On fail goto*//*Label 962*/ 26867,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 963*/ 26836, // Rule ID 649 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (sra:{ *:[i32] } GR32:{ *:[i32] }:$src1, 1:{ *:[i8] })  =>  (SAR32r1:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR32r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 649,
        GIR_Done,
      // Label 963: @26836
      GIM_Try, /*On fail goto*//*Label 964*/ 26866, // Rule ID 645 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SAR32ri:{ *:[i32] }:{ *:[i32] } GR32:{ *:[i32] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR32ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 645,
        GIR_Done,
      // Label 964: @26866
      GIM_Reject,
    // Label 962: @26867
    GIM_Reject,
    // Label 945: @26868
    GIM_Try, /*On fail goto*//*Label 965*/ 26939,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 966*/ 26908, // Rule ID 650 //
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 1,
        // (sra:{ *:[i64] } GR64:{ *:[i64] }:$src1, 1:{ *:[i8] })  =>  (SAR64r1:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR64r1,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 650,
        GIR_Done,
      // Label 966: @26908
      GIM_Try, /*On fail goto*//*Label 967*/ 26938, // Rule ID 646 //
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i64] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)  =>  (SAR64ri:{ *:[i64] }:{ *:[i32] } GR64:{ *:[i64] }:$src1, (imm:{ *:[i8] }):$src2)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::SAR64ri,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src1
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // src2
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 646,
        GIR_Done,
      // Label 967: @26938
      GIM_Reject,
    // Label 965: @26939
    GIM_Reject,
    // Label 946: @26940
    GIM_Try, /*On fail goto*//*Label 968*/ 26971, // Rule ID 6473 //
      GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
      // (sra:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)  =>  (VPSRAVQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2i64] }:$src1, VR128X:{ *:[v2i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6473,
      GIR_Done,
    // Label 968: @26971
    GIM_Reject,
    // Label 947: @26972
    GIM_Try, /*On fail goto*//*Label 969*/ 27029,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 970*/ 27005, // Rule ID 2739 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (sra:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)  =>  (VPSRAVDrr:{ *:[v4i32] } VR128:{ *:[v4i32] }:$src1, VR128:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2739,
        GIR_Done,
      // Label 970: @27005
      GIM_Try, /*On fail goto*//*Label 971*/ 27028, // Rule ID 6446 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (sra:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)  =>  (VPSRAVDZ128rr:{ *:[v4i32] } VR128X:{ *:[v4i32] }:$src1, VR128X:{ *:[v4i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6446,
        GIR_Done,
      // Label 971: @27028
      GIM_Reject,
    // Label 969: @27029
    GIM_Reject,
    // Label 948: @27030
    GIM_Try, /*On fail goto*//*Label 972*/ 27061, // Rule ID 6464 //
      GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
      // (sra:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)  =>  (VPSRAVQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4i64] }:$src1, VR256X:{ *:[v4i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6464,
      GIR_Done,
    // Label 972: @27061
    GIM_Reject,
    // Label 949: @27062
    GIM_Try, /*On fail goto*//*Label 973*/ 27093, // Rule ID 6494 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
      // (sra:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)  =>  (VPSRAVWZ128rr:{ *:[v8i16] } VR128X:{ *:[v8i16] }:$src1, VR128X:{ *:[v8i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVWZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6494,
      GIR_Done,
    // Label 973: @27093
    GIM_Reject,
    // Label 950: @27094
    GIM_Try, /*On fail goto*//*Label 974*/ 27151,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 975*/ 27127, // Rule ID 2741 //
        GIM_CheckFeatures, GIFBS_HasAVX2_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (sra:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)  =>  (VPSRAVDYrr:{ *:[v8i32] } VR256:{ *:[v8i32] }:$src1, VR256:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2741,
        GIR_Done,
      // Label 975: @27127
      GIM_Try, /*On fail goto*//*Label 976*/ 27150, // Rule ID 6437 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (sra:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)  =>  (VPSRAVDZ256rr:{ *:[v8i32] } VR256X:{ *:[v8i32] }:$src1, VR256X:{ *:[v8i32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 6437,
        GIR_Done,
      // Label 976: @27150
      GIM_Reject,
    // Label 974: @27151
    GIM_Reject,
    // Label 951: @27152
    GIM_Try, /*On fail goto*//*Label 977*/ 27183, // Rule ID 6455 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sra:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)  =>  (VPSRAVQZrr:{ *:[v8i64] } VR512:{ *:[v8i64] }:$src1, VR512:{ *:[v8i64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6455,
      GIR_Done,
    // Label 977: @27183
    GIM_Reject,
    // Label 952: @27184
    GIM_Try, /*On fail goto*//*Label 978*/ 27215, // Rule ID 6488 //
      GIM_CheckFeatures, GIFBS_HasBWI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
      // (sra:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)  =>  (VPSRAVWZ256rr:{ *:[v16i16] } VR256X:{ *:[v16i16] }:$src1, VR256X:{ *:[v16i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVWZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6488,
      GIR_Done,
    // Label 978: @27215
    GIM_Reject,
    // Label 953: @27216
    GIM_Try, /*On fail goto*//*Label 979*/ 27247, // Rule ID 6428 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sra:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)  =>  (VPSRAVDZrr:{ *:[v16i32] } VR512:{ *:[v16i32] }:$src1, VR512:{ *:[v16i32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6428,
      GIR_Done,
    // Label 979: @27247
    GIM_Reject,
    // Label 954: @27248
    GIM_Try, /*On fail goto*//*Label 980*/ 27279, // Rule ID 6482 //
      GIM_CheckFeatures, GIFBS_HasBWI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v32s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v32s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (sra:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)  =>  (VPSRAVWZrr:{ *:[v32i16] } VR512:{ *:[v32i16] }:$src1, VR512:{ *:[v32i16] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VPSRAVWZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6482,
      GIR_Done,
    // Label 980: @27279
    GIM_Reject,
    // Label 955: @27280
    GIM_Reject,
    // Label 18: @27281
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 990*/ 27893,
    /*GILLT_s32*//*Label 981*/ 27304,
    /*GILLT_s64*//*Label 982*/ 27411,
    /*GILLT_s80*//*Label 983*/ 27518, 0, 0,
    /*GILLT_v2s64*//*Label 984*/ 27551, 0,
    /*GILLT_v4s32*//*Label 985*/ 27632,
    /*GILLT_v4s64*//*Label 986*/ 27713, 0, 0,
    /*GILLT_v8s32*//*Label 987*/ 27771,
    /*GILLT_v8s64*//*Label 988*/ 27829, 0, 0, 0,
    /*GILLT_v16s32*//*Label 989*/ 27861,
    // Label 981: @27304
    GIM_Try, /*On fail goto*//*Label 991*/ 27410,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 992*/ 27340, // Rule ID 744 //
        GIM_CheckFeatures, GIFBS_FPStackf32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP32RegClassID,
        // (fadd:{ *:[f32] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)  =>  (ADD_Fp32:{ *:[f32] }:{ *:[i16] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD_Fp32,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 744,
        GIR_Done,
      // Label 992: @27340
      GIM_Try, /*On fail goto*//*Label 993*/ 27363, // Rule ID 1581 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fadd:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (VADDSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1581,
        GIR_Done,
      // Label 993: @27363
      GIM_Try, /*On fail goto*//*Label 994*/ 27386, // Rule ID 1585 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fadd:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (ADDSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADDSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1585,
        GIR_Done,
      // Label 994: @27386
      GIM_Try, /*On fail goto*//*Label 995*/ 27409, // Rule ID 5070 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32XRegClassID,
        // (fadd:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)  =>  (VADDSSZrr:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDSSZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5070,
        GIR_Done,
      // Label 995: @27409
      GIM_Reject,
    // Label 991: @27410
    GIM_Reject,
    // Label 982: @27411
    GIM_Try, /*On fail goto*//*Label 996*/ 27517,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 997*/ 27447, // Rule ID 745 //
        GIM_CheckFeatures, GIFBS_FPStackf64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP64RegClassID,
        // (fadd:{ *:[f64] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)  =>  (ADD_Fp64:{ *:[f64] }:{ *:[i16] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD_Fp64,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 745,
        GIR_Done,
      // Label 997: @27447
      GIM_Try, /*On fail goto*//*Label 998*/ 27470, // Rule ID 1583 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fadd:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (VADDSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1583,
        GIR_Done,
      // Label 998: @27470
      GIM_Try, /*On fail goto*//*Label 999*/ 27493, // Rule ID 1587 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fadd:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (ADDSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADDSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1587,
        GIR_Done,
      // Label 999: @27493
      GIM_Try, /*On fail goto*//*Label 1000*/ 27516, // Rule ID 5081 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64XRegClassID,
        // (fadd:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)  =>  (VADDSDZrr:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDSDZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5081,
        GIR_Done,
      // Label 1000: @27516
      GIM_Reject,
    // Label 996: @27517
    GIM_Reject,
    // Label 983: @27518
    GIM_Try, /*On fail goto*//*Label 1001*/ 27550, // Rule ID 746 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP80RegClassID,
      // (fadd:{ *:[f80] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)  =>  (ADD_Fp80:{ *:[f80] }:{ *:[i16] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADD_Fp80,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 746,
      GIR_Done,
    // Label 1001: @27550
    GIM_Reject,
    // Label 984: @27551
    GIM_Try, /*On fail goto*//*Label 1002*/ 27631,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 1003*/ 27584, // Rule ID 1571 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fadd:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (VADDPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1571,
        GIR_Done,
      // Label 1003: @27584
      GIM_Try, /*On fail goto*//*Label 1004*/ 27607, // Rule ID 1579 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fadd:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (ADDPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADDPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1579,
        GIR_Done,
      // Label 1004: @27607
      GIM_Try, /*On fail goto*//*Label 1005*/ 27630, // Rule ID 5240 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fadd:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)  =>  (VADDPDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5240,
        GIR_Done,
      // Label 1005: @27630
      GIM_Reject,
    // Label 1002: @27631
    GIM_Reject,
    // Label 985: @27632
    GIM_Try, /*On fail goto*//*Label 1006*/ 27712,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 1007*/ 27665, // Rule ID 1569 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fadd:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (VADDPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1569,
        GIR_Done,
      // Label 1007: @27665
      GIM_Try, /*On fail goto*//*Label 1008*/ 27688, // Rule ID 1577 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fadd:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (ADDPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::ADDPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1577,
        GIR_Done,
      // Label 1008: @27688
      GIM_Try, /*On fail goto*//*Label 1009*/ 27711, // Rule ID 5222 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fadd:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)  =>  (VADDPSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPSZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5222,
        GIR_Done,
      // Label 1009: @27711
      GIM_Reject,
    // Label 1006: @27712
    GIM_Reject,
    // Label 986: @27713
    GIM_Try, /*On fail goto*//*Label 1010*/ 27770,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 1011*/ 27746, // Rule ID 1575 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fadd:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)  =>  (VADDPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1575,
        GIR_Done,
      // Label 1011: @27746
      GIM_Try, /*On fail goto*//*Label 1012*/ 27769, // Rule ID 5249 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fadd:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)  =>  (VADDPDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5249,
        GIR_Done,
      // Label 1012: @27769
      GIM_Reject,
    // Label 1010: @27770
    GIM_Reject,
    // Label 987: @27771
    GIM_Try, /*On fail goto*//*Label 1013*/ 27828,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 1014*/ 27804, // Rule ID 1573 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fadd:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)  =>  (VADDPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1573,
        GIR_Done,
      // Label 1014: @27804
      GIM_Try, /*On fail goto*//*Label 1015*/ 27827, // Rule ID 5231 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fadd:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)  =>  (VADDPSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPSZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5231,
        GIR_Done,
      // Label 1015: @27827
      GIM_Reject,
    // Label 1013: @27828
    GIM_Reject,
    // Label 988: @27829
    GIM_Try, /*On fail goto*//*Label 1016*/ 27860, // Rule ID 5213 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fadd:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)  =>  (VADDPDZrr:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5213,
      GIR_Done,
    // Label 1016: @27860
    GIM_Reject,
    // Label 989: @27861
    GIM_Try, /*On fail goto*//*Label 1017*/ 27892, // Rule ID 5204 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fadd:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)  =>  (VADDPSZrr:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VADDPSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5204,
      GIR_Done,
    // Label 1017: @27892
    GIM_Reject,
    // Label 990: @27893
    GIM_Reject,
    // Label 19: @27894
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1027*/ 28506,
    /*GILLT_s32*//*Label 1018*/ 27917,
    /*GILLT_s64*//*Label 1019*/ 28024,
    /*GILLT_s80*//*Label 1020*/ 28131, 0, 0,
    /*GILLT_v2s64*//*Label 1021*/ 28164, 0,
    /*GILLT_v4s32*//*Label 1022*/ 28245,
    /*GILLT_v4s64*//*Label 1023*/ 28326, 0, 0,
    /*GILLT_v8s32*//*Label 1024*/ 28384,
    /*GILLT_v8s64*//*Label 1025*/ 28442, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1026*/ 28474,
    // Label 1018: @27917
    GIM_Try, /*On fail goto*//*Label 1028*/ 28023,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 1029*/ 27953, // Rule ID 747 //
        GIM_CheckFeatures, GIFBS_FPStackf32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP32RegClassID,
        // (fsub:{ *:[f32] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)  =>  (SUB_Fp32:{ *:[f32] }:{ *:[i16] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB_Fp32,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 747,
        GIR_Done,
      // Label 1029: @27953
      GIM_Try, /*On fail goto*//*Label 1030*/ 27976, // Rule ID 1621 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fsub:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (VSUBSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1621,
        GIR_Done,
      // Label 1030: @27976
      GIM_Try, /*On fail goto*//*Label 1031*/ 27999, // Rule ID 1625 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fsub:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (SUBSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUBSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1625,
        GIR_Done,
      // Label 1031: @27999
      GIM_Try, /*On fail goto*//*Label 1032*/ 28022, // Rule ID 5114 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32XRegClassID,
        // (fsub:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)  =>  (VSUBSSZrr:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBSSZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5114,
        GIR_Done,
      // Label 1032: @28022
      GIM_Reject,
    // Label 1028: @28023
    GIM_Reject,
    // Label 1019: @28024
    GIM_Try, /*On fail goto*//*Label 1033*/ 28130,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 1034*/ 28060, // Rule ID 748 //
        GIM_CheckFeatures, GIFBS_FPStackf64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP64RegClassID,
        // (fsub:{ *:[f64] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)  =>  (SUB_Fp64:{ *:[f64] }:{ *:[i16] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB_Fp64,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 748,
        GIR_Done,
      // Label 1034: @28060
      GIM_Try, /*On fail goto*//*Label 1035*/ 28083, // Rule ID 1623 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fsub:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (VSUBSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1623,
        GIR_Done,
      // Label 1035: @28083
      GIM_Try, /*On fail goto*//*Label 1036*/ 28106, // Rule ID 1627 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fsub:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (SUBSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUBSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1627,
        GIR_Done,
      // Label 1036: @28106
      GIM_Try, /*On fail goto*//*Label 1037*/ 28129, // Rule ID 5125 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64XRegClassID,
        // (fsub:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)  =>  (VSUBSDZrr:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBSDZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5125,
        GIR_Done,
      // Label 1037: @28129
      GIM_Reject,
    // Label 1033: @28130
    GIM_Reject,
    // Label 1020: @28131
    GIM_Try, /*On fail goto*//*Label 1038*/ 28163, // Rule ID 749 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP80RegClassID,
      // (fsub:{ *:[f80] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)  =>  (SUB_Fp80:{ *:[f80] }:{ *:[i16] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUB_Fp80,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 749,
      GIR_Done,
    // Label 1038: @28163
    GIM_Reject,
    // Label 1021: @28164
    GIM_Try, /*On fail goto*//*Label 1039*/ 28244,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 1040*/ 28197, // Rule ID 1611 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fsub:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (VSUBPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1611,
        GIR_Done,
      // Label 1040: @28197
      GIM_Try, /*On fail goto*//*Label 1041*/ 28220, // Rule ID 1619 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fsub:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (SUBPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUBPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1619,
        GIR_Done,
      // Label 1041: @28220
      GIM_Try, /*On fail goto*//*Label 1042*/ 28243, // Rule ID 5360 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fsub:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)  =>  (VSUBPDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5360,
        GIR_Done,
      // Label 1042: @28243
      GIM_Reject,
    // Label 1039: @28244
    GIM_Reject,
    // Label 1022: @28245
    GIM_Try, /*On fail goto*//*Label 1043*/ 28325,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 1044*/ 28278, // Rule ID 1609 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fsub:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (VSUBPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1609,
        GIR_Done,
      // Label 1044: @28278
      GIM_Try, /*On fail goto*//*Label 1045*/ 28301, // Rule ID 1617 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fsub:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (SUBPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::SUBPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1617,
        GIR_Done,
      // Label 1045: @28301
      GIM_Try, /*On fail goto*//*Label 1046*/ 28324, // Rule ID 5342 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fsub:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)  =>  (VSUBPSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPSZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5342,
        GIR_Done,
      // Label 1046: @28324
      GIM_Reject,
    // Label 1043: @28325
    GIM_Reject,
    // Label 1023: @28326
    GIM_Try, /*On fail goto*//*Label 1047*/ 28383,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 1048*/ 28359, // Rule ID 1615 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fsub:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)  =>  (VSUBPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1615,
        GIR_Done,
      // Label 1048: @28359
      GIM_Try, /*On fail goto*//*Label 1049*/ 28382, // Rule ID 5369 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fsub:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)  =>  (VSUBPDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5369,
        GIR_Done,
      // Label 1049: @28382
      GIM_Reject,
    // Label 1047: @28383
    GIM_Reject,
    // Label 1024: @28384
    GIM_Try, /*On fail goto*//*Label 1050*/ 28441,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 1051*/ 28417, // Rule ID 1613 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fsub:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)  =>  (VSUBPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1613,
        GIR_Done,
      // Label 1051: @28417
      GIM_Try, /*On fail goto*//*Label 1052*/ 28440, // Rule ID 5351 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fsub:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)  =>  (VSUBPSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPSZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5351,
        GIR_Done,
      // Label 1052: @28440
      GIM_Reject,
    // Label 1050: @28441
    GIM_Reject,
    // Label 1025: @28442
    GIM_Try, /*On fail goto*//*Label 1053*/ 28473, // Rule ID 5333 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fsub:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)  =>  (VSUBPDZrr:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5333,
      GIR_Done,
    // Label 1053: @28473
    GIM_Reject,
    // Label 1026: @28474
    GIM_Try, /*On fail goto*//*Label 1054*/ 28505, // Rule ID 5324 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fsub:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)  =>  (VSUBPSZrr:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VSUBPSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5324,
      GIR_Done,
    // Label 1054: @28505
    GIM_Reject,
    // Label 1027: @28506
    GIM_Reject,
    // Label 20: @28507
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1064*/ 29119,
    /*GILLT_s32*//*Label 1055*/ 28530,
    /*GILLT_s64*//*Label 1056*/ 28637,
    /*GILLT_s80*//*Label 1057*/ 28744, 0, 0,
    /*GILLT_v2s64*//*Label 1058*/ 28777, 0,
    /*GILLT_v4s32*//*Label 1059*/ 28858,
    /*GILLT_v4s64*//*Label 1060*/ 28939, 0, 0,
    /*GILLT_v8s32*//*Label 1061*/ 28997,
    /*GILLT_v8s64*//*Label 1062*/ 29055, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1063*/ 29087,
    // Label 1055: @28530
    GIM_Try, /*On fail goto*//*Label 1065*/ 28636,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 1066*/ 28566, // Rule ID 750 //
        GIM_CheckFeatures, GIFBS_FPStackf32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP32RegClassID,
        // (fmul:{ *:[f32] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)  =>  (MUL_Fp32:{ *:[f32] }:{ *:[i16] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MUL_Fp32,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 750,
        GIR_Done,
      // Label 1066: @28566
      GIM_Try, /*On fail goto*//*Label 1067*/ 28589, // Rule ID 1601 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fmul:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (VMULSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1601,
        GIR_Done,
      // Label 1067: @28589
      GIM_Try, /*On fail goto*//*Label 1068*/ 28612, // Rule ID 1605 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fmul:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (MULSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MULSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1605,
        GIR_Done,
      // Label 1068: @28612
      GIM_Try, /*On fail goto*//*Label 1069*/ 28635, // Rule ID 5092 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32XRegClassID,
        // (fmul:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)  =>  (VMULSSZrr:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULSSZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5092,
        GIR_Done,
      // Label 1069: @28635
      GIM_Reject,
    // Label 1065: @28636
    GIM_Reject,
    // Label 1056: @28637
    GIM_Try, /*On fail goto*//*Label 1070*/ 28743,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 1071*/ 28673, // Rule ID 751 //
        GIM_CheckFeatures, GIFBS_FPStackf64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP64RegClassID,
        // (fmul:{ *:[f64] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)  =>  (MUL_Fp64:{ *:[f64] }:{ *:[i16] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MUL_Fp64,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 751,
        GIR_Done,
      // Label 1071: @28673
      GIM_Try, /*On fail goto*//*Label 1072*/ 28696, // Rule ID 1603 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fmul:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (VMULSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1603,
        GIR_Done,
      // Label 1072: @28696
      GIM_Try, /*On fail goto*//*Label 1073*/ 28719, // Rule ID 1607 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fmul:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (MULSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MULSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1607,
        GIR_Done,
      // Label 1073: @28719
      GIM_Try, /*On fail goto*//*Label 1074*/ 28742, // Rule ID 5103 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64XRegClassID,
        // (fmul:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)  =>  (VMULSDZrr:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULSDZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5103,
        GIR_Done,
      // Label 1074: @28742
      GIM_Reject,
    // Label 1070: @28743
    GIM_Reject,
    // Label 1057: @28744
    GIM_Try, /*On fail goto*//*Label 1075*/ 28776, // Rule ID 752 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP80RegClassID,
      // (fmul:{ *:[f80] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)  =>  (MUL_Fp80:{ *:[f80] }:{ *:[i16] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MUL_Fp80,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 752,
      GIR_Done,
    // Label 1075: @28776
    GIM_Reject,
    // Label 1058: @28777
    GIM_Try, /*On fail goto*//*Label 1076*/ 28857,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 1077*/ 28810, // Rule ID 1591 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fmul:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (VMULPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1591,
        GIR_Done,
      // Label 1077: @28810
      GIM_Try, /*On fail goto*//*Label 1078*/ 28833, // Rule ID 1599 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fmul:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (MULPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MULPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1599,
        GIR_Done,
      // Label 1078: @28833
      GIM_Try, /*On fail goto*//*Label 1079*/ 28856, // Rule ID 5300 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fmul:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)  =>  (VMULPDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5300,
        GIR_Done,
      // Label 1079: @28856
      GIM_Reject,
    // Label 1076: @28857
    GIM_Reject,
    // Label 1059: @28858
    GIM_Try, /*On fail goto*//*Label 1080*/ 28938,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 1081*/ 28891, // Rule ID 1589 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fmul:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (VMULPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1589,
        GIR_Done,
      // Label 1081: @28891
      GIM_Try, /*On fail goto*//*Label 1082*/ 28914, // Rule ID 1597 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fmul:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (MULPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::MULPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1597,
        GIR_Done,
      // Label 1082: @28914
      GIM_Try, /*On fail goto*//*Label 1083*/ 28937, // Rule ID 5282 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fmul:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)  =>  (VMULPSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPSZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5282,
        GIR_Done,
      // Label 1083: @28937
      GIM_Reject,
    // Label 1080: @28938
    GIM_Reject,
    // Label 1060: @28939
    GIM_Try, /*On fail goto*//*Label 1084*/ 28996,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 1085*/ 28972, // Rule ID 1595 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fmul:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)  =>  (VMULPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1595,
        GIR_Done,
      // Label 1085: @28972
      GIM_Try, /*On fail goto*//*Label 1086*/ 28995, // Rule ID 5309 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fmul:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)  =>  (VMULPDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5309,
        GIR_Done,
      // Label 1086: @28995
      GIM_Reject,
    // Label 1084: @28996
    GIM_Reject,
    // Label 1061: @28997
    GIM_Try, /*On fail goto*//*Label 1087*/ 29054,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 1088*/ 29030, // Rule ID 1593 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fmul:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)  =>  (VMULPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1593,
        GIR_Done,
      // Label 1088: @29030
      GIM_Try, /*On fail goto*//*Label 1089*/ 29053, // Rule ID 5291 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fmul:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)  =>  (VMULPSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPSZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5291,
        GIR_Done,
      // Label 1089: @29053
      GIM_Reject,
    // Label 1087: @29054
    GIM_Reject,
    // Label 1062: @29055
    GIM_Try, /*On fail goto*//*Label 1090*/ 29086, // Rule ID 5273 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fmul:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)  =>  (VMULPDZrr:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5273,
      GIR_Done,
    // Label 1090: @29086
    GIM_Reject,
    // Label 1063: @29087
    GIM_Try, /*On fail goto*//*Label 1091*/ 29118, // Rule ID 5264 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fmul:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)  =>  (VMULPSZrr:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VMULPSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5264,
      GIR_Done,
    // Label 1091: @29118
    GIM_Reject,
    // Label 1064: @29119
    GIM_Reject,
    // Label 21: @29120
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1101*/ 29732,
    /*GILLT_s32*//*Label 1092*/ 29143,
    /*GILLT_s64*//*Label 1093*/ 29250,
    /*GILLT_s80*//*Label 1094*/ 29357, 0, 0,
    /*GILLT_v2s64*//*Label 1095*/ 29390, 0,
    /*GILLT_v4s32*//*Label 1096*/ 29471,
    /*GILLT_v4s64*//*Label 1097*/ 29552, 0, 0,
    /*GILLT_v8s32*//*Label 1098*/ 29610,
    /*GILLT_v8s64*//*Label 1099*/ 29668, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1100*/ 29700,
    // Label 1092: @29143
    GIM_Try, /*On fail goto*//*Label 1102*/ 29249,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 1103*/ 29179, // Rule ID 753 //
        GIM_CheckFeatures, GIFBS_FPStackf32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP32RegClassID,
        // (fdiv:{ *:[f32] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)  =>  (DIV_Fp32:{ *:[f32] }:{ *:[i16] } RFP32:{ *:[f32] }:$src1, RFP32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIV_Fp32,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 753,
        GIR_Done,
      // Label 1103: @29179
      GIM_Try, /*On fail goto*//*Label 1104*/ 29202, // Rule ID 1641 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fdiv:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (VDIVSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1641,
        GIR_Done,
      // Label 1104: @29202
      GIM_Try, /*On fail goto*//*Label 1105*/ 29225, // Rule ID 1645 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32RegClassID,
        // (fdiv:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)  =>  (DIVSSrr:{ *:[f32] } FR32:{ *:[f32] }:$src1, FR32:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIVSSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1645,
        GIR_Done,
      // Label 1105: @29225
      GIM_Try, /*On fail goto*//*Label 1106*/ 29248, // Rule ID 5136 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR32XRegClassID,
        // (fdiv:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)  =>  (VDIVSSZrr:{ *:[f32] } FR32X:{ *:[f32] }:$src1, FR32X:{ *:[f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVSSZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5136,
        GIR_Done,
      // Label 1106: @29248
      GIM_Reject,
    // Label 1102: @29249
    GIM_Reject,
    // Label 1093: @29250
    GIM_Try, /*On fail goto*//*Label 1107*/ 29356,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 1108*/ 29286, // Rule ID 754 //
        GIM_CheckFeatures, GIFBS_FPStackf64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP64RegClassID,
        // (fdiv:{ *:[f64] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)  =>  (DIV_Fp64:{ *:[f64] }:{ *:[i16] } RFP64:{ *:[f64] }:$src1, RFP64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIV_Fp64,
        GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 754,
        GIR_Done,
      // Label 1108: @29286
      GIM_Try, /*On fail goto*//*Label 1109*/ 29309, // Rule ID 1643 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fdiv:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (VDIVSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1643,
        GIR_Done,
      // Label 1109: @29309
      GIM_Try, /*On fail goto*//*Label 1110*/ 29332, // Rule ID 1647 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64RegClassID,
        // (fdiv:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)  =>  (DIVSDrr:{ *:[f64] } FR64:{ *:[f64] }:$src1, FR64:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIVSDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1647,
        GIR_Done,
      // Label 1110: @29332
      GIM_Try, /*On fail goto*//*Label 1111*/ 29355, // Rule ID 5147 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::FR64XRegClassID,
        // (fdiv:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)  =>  (VDIVSDZrr:{ *:[f64] } FR64X:{ *:[f64] }:$src1, FR64X:{ *:[f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVSDZrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5147,
        GIR_Done,
      // Label 1111: @29355
      GIM_Reject,
    // Label 1107: @29356
    GIM_Reject,
    // Label 1094: @29357
    GIM_Try, /*On fail goto*//*Label 1112*/ 29389, // Rule ID 755 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::RFP80RegClassID,
      // (fdiv:{ *:[f80] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)  =>  (DIV_Fp80:{ *:[f80] }:{ *:[i16] } RFP80:{ *:[f80] }:$src1, RFP80:{ *:[f80] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIV_Fp80,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 755,
      GIR_Done,
    // Label 1112: @29389
    GIM_Reject,
    // Label 1095: @29390
    GIM_Try, /*On fail goto*//*Label 1113*/ 29470,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 1114*/ 29423, // Rule ID 1631 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fdiv:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (VDIVPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1631,
        GIR_Done,
      // Label 1114: @29423
      GIM_Try, /*On fail goto*//*Label 1115*/ 29446, // Rule ID 1639 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fdiv:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)  =>  (DIVPDrr:{ *:[v2f64] } VR128:{ *:[v2f64] }:$src1, VR128:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIVPDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1639,
        GIR_Done,
      // Label 1115: @29446
      GIM_Try, /*On fail goto*//*Label 1116*/ 29469, // Rule ID 5420 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fdiv:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)  =>  (VDIVPDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2f64] }:$src1, VR128X:{ *:[v2f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPDZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5420,
        GIR_Done,
      // Label 1116: @29469
      GIM_Reject,
    // Label 1113: @29470
    GIM_Reject,
    // Label 1096: @29471
    GIM_Try, /*On fail goto*//*Label 1117*/ 29551,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 1118*/ 29504, // Rule ID 1629 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fdiv:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (VDIVPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1629,
        GIR_Done,
      // Label 1118: @29504
      GIM_Try, /*On fail goto*//*Label 1119*/ 29527, // Rule ID 1637 //
        GIM_CheckFeatures, GIFBS_UseSSE1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128RegClassID,
        // (fdiv:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)  =>  (DIVPSrr:{ *:[v4f32] } VR128:{ *:[v4f32] }:$src1, VR128:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::DIVPSrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1637,
        GIR_Done,
      // Label 1119: @29527
      GIM_Try, /*On fail goto*//*Label 1120*/ 29550, // Rule ID 5402 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR128XRegClassID,
        // (fdiv:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)  =>  (VDIVPSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4f32] }:$src1, VR128X:{ *:[v4f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPSZ128rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5402,
        GIR_Done,
      // Label 1120: @29550
      GIM_Reject,
    // Label 1117: @29551
    GIM_Reject,
    // Label 1097: @29552
    GIM_Try, /*On fail goto*//*Label 1121*/ 29609,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 1122*/ 29585, // Rule ID 1635 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fdiv:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)  =>  (VDIVPDYrr:{ *:[v4f64] } VR256:{ *:[v4f64] }:$src1, VR256:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1635,
        GIR_Done,
      // Label 1122: @29585
      GIM_Try, /*On fail goto*//*Label 1123*/ 29608, // Rule ID 5429 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fdiv:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)  =>  (VDIVPDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4f64] }:$src1, VR256X:{ *:[v4f64] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5429,
        GIR_Done,
      // Label 1123: @29608
      GIM_Reject,
    // Label 1121: @29609
    GIM_Reject,
    // Label 1098: @29610
    GIM_Try, /*On fail goto*//*Label 1124*/ 29667,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s32,
      GIM_Try, /*On fail goto*//*Label 1125*/ 29643, // Rule ID 1633 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256RegClassID,
        // (fdiv:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)  =>  (VDIVPSYrr:{ *:[v8f32] } VR256:{ *:[v8f32] }:$src1, VR256:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1633,
        GIR_Done,
      // Label 1125: @29643
      GIM_Try, /*On fail goto*//*Label 1126*/ 29666, // Rule ID 5411 //
        GIM_CheckFeatures, GIFBS_HasAVX512_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR256XRegClassID,
        // (fdiv:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)  =>  (VDIVPSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8f32] }:$src1, VR256X:{ *:[v8f32] }:$src2)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPSZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 5411,
        GIR_Done,
      // Label 1126: @29666
      GIM_Reject,
    // Label 1124: @29667
    GIM_Reject,
    // Label 1099: @29668
    GIM_Try, /*On fail goto*//*Label 1127*/ 29699, // Rule ID 5393 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fdiv:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)  =>  (VDIVPDZrr:{ *:[v8f64] } VR512:{ *:[v8f64] }:$src1, VR512:{ *:[v8f64] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5393,
      GIR_Done,
    // Label 1127: @29699
    GIM_Reject,
    // Label 1100: @29700
    GIM_Try, /*On fail goto*//*Label 1128*/ 29731, // Rule ID 5384 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/X86::VR512RegClassID,
      // (fdiv:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)  =>  (VDIVPSZrr:{ *:[v16f32] } VR512:{ *:[v16f32] }:$src1, VR512:{ *:[v16f32] }:$src2)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VDIVPSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5384,
      GIR_Done,
    // Label 1128: @29731
    GIM_Reject,
    // Label 1101: @29732
    GIM_Reject,
    // Label 22: @29733
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 6, /*)*//*default:*//*Label 1132*/ 29821,
    /*GILLT_s32*//*Label 1129*/ 29742,
    /*GILLT_s64*//*Label 1130*/ 29769,
    /*GILLT_s80*//*Label 1131*/ 29796,
    // Label 1129: @29742
    GIM_Try, /*On fail goto*//*Label 1133*/ 29768, // Rule ID 822 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
      // (fneg:{ *:[f32] } RFP32:{ *:[f32] }:$src)  =>  (CHS_Fp32:{ *:[f32] }:{ *:[i16] } RFP32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CHS_Fp32,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 822,
      GIR_Done,
    // Label 1133: @29768
    GIM_Reject,
    // Label 1130: @29769
    GIM_Try, /*On fail goto*//*Label 1134*/ 29795, // Rule ID 823 //
      GIM_CheckFeatures, GIFBS_FPStackf64,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
      // (fneg:{ *:[f64] } RFP64:{ *:[f64] }:$src)  =>  (CHS_Fp64:{ *:[f64] }:{ *:[i16] } RFP64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CHS_Fp64,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 823,
      GIR_Done,
    // Label 1134: @29795
    GIM_Reject,
    // Label 1131: @29796
    GIM_Try, /*On fail goto*//*Label 1135*/ 29820, // Rule ID 824 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      // (fneg:{ *:[f80] } RFP80:{ *:[f80] }:$src)  =>  (CHS_Fp80:{ *:[f80] }:{ *:[i16] } RFP80:{ *:[f80] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CHS_Fp80,
      GIR_AddImplicitDef, /*InsnID*/0, X86::FPSW,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 824,
      GIR_Done,
    // Label 1135: @29820
    GIM_Reject,
    // Label 1132: @29821
    GIM_Reject,
    // Label 23: @29822
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 16, /*)*//*default:*//*Label 1140*/ 30097,
    /*GILLT_s64*//*Label 1136*/ 29840,
    /*GILLT_s80*//*Label 1137*/ 29976, 0, 0, 0, 0, 0,
    /*GILLT_v4s64*//*Label 1138*/ 30027, 0, 0, 0,
    /*GILLT_v8s64*//*Label 1139*/ 30073,
    // Label 1136: @29840
    GIM_Try, /*On fail goto*//*Label 1141*/ 29975,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 1142*/ 29865, // Rule ID 1407 //
        GIM_CheckFeatures, GIFBS_UseSSE2,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        // (fpextend:{ *:[f64] } FR32:{ *:[f32] }:$src)  =>  (CVTSS2SDrr:{ *:[f64] } FR32:{ *:[f32] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSS2SDrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1407,
        GIR_Done,
      // Label 1142: @29865
      GIM_Try, /*On fail goto*//*Label 1143*/ 29886, // Rule ID 12327 //
        GIM_CheckFeatures, GIFBS_FPStackf32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
        // (fpextend:{ *:[f64] } RFP32:{ *:[f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f64] } RFP32:{ *:[f32] }:$src, RFP64:{ *:[i32] })
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP64*/43,
        // GIR_Coverage, 12327,
        GIR_Done,
      // Label 1143: @29886
      GIM_Try, /*On fail goto*//*Label 1144*/ 29930, // Rule ID 12476 //
        GIM_CheckFeatures, GIFBS_UseAVX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
        // (fpextend:{ *:[f64] } FR32:{ *:[f32] }:$src)  =>  (VCVTSS2SDrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), FR32:{ *:[f32] }:$src)
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SDrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 12476,
        GIR_Done,
      // Label 1144: @29930
      GIM_Try, /*On fail goto*//*Label 1145*/ 29974, // Rule ID 14867 //
        GIM_CheckFeatures, GIFBS_HasAVX512,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
        // (fpextend:{ *:[f64] } FR32X:{ *:[f32] }:$src)  =>  (VCVTSS2SDZrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), FR32X:{ *:[f32] }:$src)
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSS2SDZrr,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 14867,
        GIR_Done,
      // Label 1145: @29974
      GIM_Reject,
    // Label 1141: @29975
    GIM_Reject,
    // Label 1137: @29976
    GIM_Try, /*On fail goto*//*Label 1146*/ 30001, // Rule ID 12328 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP32RegClassID,
      // (fpextend:{ *:[f80] } RFP32:{ *:[f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f80] } RFP32:{ *:[f32] }:$src, RFP80:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP80*/72,
      // GIR_Coverage, 12328,
      GIR_Done,
    // Label 1146: @30001
    GIM_Try, /*On fail goto*//*Label 1147*/ 30026, // Rule ID 12329 //
      GIM_CheckFeatures, GIFBS_FPStackf64,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP80RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
      // (fpextend:{ *:[f80] } RFP64:{ *:[f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f80] } RFP64:{ *:[f64] }:$src, RFP80:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP80*/72,
      // GIR_Coverage, 12329,
      GIR_Done,
    // Label 1147: @30026
    GIM_Reject,
    // Label 1138: @30027
    GIM_Try, /*On fail goto*//*Label 1148*/ 30072,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 1149*/ 30052, // Rule ID 1435 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
        // (fpextend:{ *:[v4f64] } VR128:{ *:[v4f32] }:$src)  =>  (VCVTPS2PDYrr:{ *:[v4f64] } VR128:{ *:[v4f32] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPS2PDYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1435,
        GIR_Done,
      // Label 1149: @30052
      GIM_Try, /*On fail goto*//*Label 1150*/ 30071, // Rule ID 8159 //
        GIM_CheckFeatures, GIFBS_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
        // (fpextend:{ *:[v4f64] } VR128X:{ *:[v4f32] }:$src)  =>  (VCVTPS2PDZ256rr:{ *:[v4f64] } VR128X:{ *:[v4f32] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPS2PDZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 8159,
        GIR_Done,
      // Label 1150: @30071
      GIM_Reject,
    // Label 1148: @30072
    GIM_Reject,
    // Label 1139: @30073
    GIM_Try, /*On fail goto*//*Label 1151*/ 30096, // Rule ID 8138 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fpextend:{ *:[v8f64] } VR256X:{ *:[v8f32] }:$src)  =>  (VCVTPS2PDZrr:{ *:[v8f64] } VR256X:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPS2PDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8138,
      GIR_Done,
    // Label 1151: @30096
    GIM_Reject,
    // Label 1140: @30097
    GIM_Reject,
    // Label 24: @30098
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 15, /*)*//*default:*//*Label 1156*/ 30382,
    /*GILLT_s32*//*Label 1152*/ 30116,
    /*GILLT_s64*//*Label 1153*/ 30286, 0, 0, 0, 0, 0,
    /*GILLT_v4s32*//*Label 1154*/ 30312, 0, 0, 0,
    /*GILLT_v8s32*//*Label 1155*/ 30358,
    // Label 1152: @30116
    GIM_Try, /*On fail goto*//*Label 1157*/ 30139, // Rule ID 1401 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fpround:{ *:[f32] } FR64:{ *:[f64] }:$src)  =>  (CVTSD2SSrr:{ *:[f32] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSD2SSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1401,
      GIR_Done,
    // Label 1157: @30139
    GIM_Try, /*On fail goto*//*Label 1158*/ 30164, // Rule ID 12330 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP64RegClassID,
      // (fpround:{ *:[f32] } RFP64:{ *:[f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f32] } RFP64:{ *:[f64] }:$src, RFP32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP32*/33,
      // GIR_Coverage, 12330,
      GIR_Done,
    // Label 1158: @30164
    GIM_Try, /*On fail goto*//*Label 1159*/ 30189, // Rule ID 12331 //
      GIM_CheckFeatures, GIFBS_FPStackf32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      // (fpround:{ *:[f32] } RFP80:{ *:[f80] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f32] } RFP80:{ *:[f80] }:$src, RFP32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP32*/33,
      // GIR_Coverage, 12331,
      GIR_Done,
    // Label 1159: @30189
    GIM_Try, /*On fail goto*//*Label 1160*/ 30237, // Rule ID 12475 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fpround:{ *:[f32] } FR64:{ *:[f64] }:$src)  =>  (VCVTSD2SSrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), FR64:{ *:[f64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SSrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12475,
      GIR_Done,
    // Label 1160: @30237
    GIM_Try, /*On fail goto*//*Label 1161*/ 30285, // Rule ID 14871 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (fpround:{ *:[f32] } FR64X:{ *:[f64] }:$src)  =>  (VCVTSD2SSZrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), FR64X:{ *:[f64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSD2SSZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14871,
      GIR_Done,
    // Label 1161: @30285
    GIM_Reject,
    // Label 1153: @30286
    GIM_Try, /*On fail goto*//*Label 1162*/ 30311, // Rule ID 12332 //
      GIM_CheckFeatures, GIFBS_FPStackf64,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s80,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::RFP64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::RFP80RegClassID,
      // (fpround:{ *:[f64] } RFP80:{ *:[f80] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f64] } RFP80:{ *:[f80] }:$src, RFP64:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC RFP64*/43,
      // GIR_Coverage, 12332,
      GIR_Done,
    // Label 1162: @30311
    GIM_Reject,
    // Label 1154: @30312
    GIM_Try, /*On fail goto*//*Label 1163*/ 30357,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_Try, /*On fail goto*//*Label 1164*/ 30337, // Rule ID 1447 //
        GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
        // (fpround:{ *:[v4f32] } VR256:{ *:[v4f64] }:$src)  =>  (VCVTPD2PSYrr:{ *:[v4f32] } VR256:{ *:[v4f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPD2PSYrr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1447,
        GIR_Done,
      // Label 1164: @30337
      GIM_Try, /*On fail goto*//*Label 1165*/ 30356, // Rule ID 8129 //
        GIM_CheckFeatures, GIFBS_HasVLX,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
        // (fpround:{ *:[v4f32] } VR256X:{ *:[v4f64] }:$src)  =>  (VCVTPD2PSZ256rr:{ *:[v4f32] } VR256X:{ *:[v4f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPD2PSZ256rr,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 8129,
        GIR_Done,
      // Label 1165: @30356
      GIM_Reject,
    // Label 1163: @30357
    GIM_Reject,
    // Label 1155: @30358
    GIM_Try, /*On fail goto*//*Label 1166*/ 30381, // Rule ID 8108 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fpround:{ *:[v8f32] } VR512:{ *:[v8f64] }:$src)  =>  (VCVTPD2PSZrr:{ *:[v8f32] } VR512:{ *:[v8f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTPD2PSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8108,
      GIR_Done,
    // Label 1166: @30381
    GIM_Reject,
    // Label 1156: @30382
    GIM_Reject,
    // Label 25: @30383
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1175*/ 31012,
    /*GILLT_s32*//*Label 1167*/ 30406,
    /*GILLT_s64*//*Label 1168*/ 30545, 0, 0, 0,
    /*GILLT_v2s64*//*Label 1169*/ 30684, 0,
    /*GILLT_v4s32*//*Label 1170*/ 30708,
    /*GILLT_v4s64*//*Label 1171*/ 30824, 0, 0,
    /*GILLT_v8s32*//*Label 1172*/ 30871,
    /*GILLT_v8s64*//*Label 1173*/ 30941, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1174*/ 30988,
    // Label 1167: @30406
    GIM_Try, /*On fail goto*//*Label 1176*/ 30429, // Rule ID 1339 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (fp_to_sint:{ *:[i32] } FR32:{ *:[f32] }:$src)  =>  (VCVTTSS2SIrr:{ *:[i32] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2SIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1339,
      GIR_Done,
    // Label 1176: @30429
    GIM_Try, /*On fail goto*//*Label 1177*/ 30452, // Rule ID 1343 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fp_to_sint:{ *:[i32] } FR64:{ *:[f64] }:$src)  =>  (VCVTTSD2SIrr:{ *:[i32] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2SIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1343,
      GIR_Done,
    // Label 1177: @30452
    GIM_Try, /*On fail goto*//*Label 1178*/ 30475, // Rule ID 1347 //
      GIM_CheckFeatures, GIFBS_UseSSE1,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (fp_to_sint:{ *:[i32] } FR32:{ *:[f32] }:$src)  =>  (CVTTSS2SIrr:{ *:[i32] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTTSS2SIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1347,
      GIR_Done,
    // Label 1178: @30475
    GIM_Try, /*On fail goto*//*Label 1179*/ 30498, // Rule ID 1351 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fp_to_sint:{ *:[i32] } FR64:{ *:[f64] }:$src)  =>  (CVTTSD2SIrr:{ *:[i32] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTTSD2SIrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1351,
      GIR_Done,
    // Label 1179: @30498
    GIM_Try, /*On fail goto*//*Label 1180*/ 30521, // Rule ID 8050 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
      // (fp_to_sint:{ *:[i32] } FR32X:{ *:[f32] }:$src)  =>  (VCVTTSS2SIZrr:{ *:[i32] } FR32X:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2SIZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8050,
      GIR_Done,
    // Label 1180: @30521
    GIM_Try, /*On fail goto*//*Label 1181*/ 30544, // Rule ID 8060 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (fp_to_sint:{ *:[i32] } FR64X:{ *:[f64] }:$src)  =>  (VCVTTSD2SIZrr:{ *:[i32] } FR64X:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2SIZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8060,
      GIR_Done,
    // Label 1181: @30544
    GIM_Reject,
    // Label 1168: @30545
    GIM_Try, /*On fail goto*//*Label 1182*/ 30568, // Rule ID 1341 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (fp_to_sint:{ *:[i64] } FR32:{ *:[f32] }:$src)  =>  (VCVTTSS2SI64rr:{ *:[i64] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2SI64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1341,
      GIR_Done,
    // Label 1182: @30568
    GIM_Try, /*On fail goto*//*Label 1183*/ 30591, // Rule ID 1345 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fp_to_sint:{ *:[i64] } FR64:{ *:[f64] }:$src)  =>  (VCVTTSD2SI64rr:{ *:[i64] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2SI64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1345,
      GIR_Done,
    // Label 1183: @30591
    GIM_Try, /*On fail goto*//*Label 1184*/ 30614, // Rule ID 1349 //
      GIM_CheckFeatures, GIFBS_UseSSE1,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32RegClassID,
      // (fp_to_sint:{ *:[i64] } FR32:{ *:[f32] }:$src)  =>  (CVTTSS2SI64rr:{ *:[i64] } FR32:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTTSS2SI64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1349,
      GIR_Done,
    // Label 1184: @30614
    GIM_Try, /*On fail goto*//*Label 1185*/ 30637, // Rule ID 1353 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64RegClassID,
      // (fp_to_sint:{ *:[i64] } FR64:{ *:[f64] }:$src)  =>  (CVTTSD2SI64rr:{ *:[i64] } FR64:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTTSD2SI64rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1353,
      GIR_Done,
    // Label 1185: @30637
    GIM_Try, /*On fail goto*//*Label 1186*/ 30660, // Rule ID 8055 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
      // (fp_to_sint:{ *:[i64] } FR32X:{ *:[f32] }:$src)  =>  (VCVTTSS2SI64Zrr:{ *:[i64] } FR32X:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2SI64Zrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8055,
      GIR_Done,
    // Label 1186: @30660
    GIM_Try, /*On fail goto*//*Label 1187*/ 30683, // Rule ID 8065 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (fp_to_sint:{ *:[i64] } FR64X:{ *:[f64] }:$src)  =>  (VCVTTSD2SI64Zrr:{ *:[i64] } FR64X:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2SI64Zrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8065,
      GIR_Done,
    // Label 1187: @30683
    GIM_Reject,
    // Label 1169: @30684
    GIM_Try, /*On fail goto*//*Label 1188*/ 30707, // Rule ID 14911 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_sint:{ *:[v2i64] } VR128X:{ *:[v2f64] }:$src)  =>  (VCVTTPD2QQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2QQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14911,
      GIR_Done,
    // Label 1188: @30707
    GIM_Reject,
    // Label 1170: @30708
    GIM_Try, /*On fail goto*//*Label 1189*/ 30731, // Rule ID 12502 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (fp_to_sint:{ *:[v4i32] } VR128:{ *:[v4f32] }:$src)  =>  (VCVTTPS2DQrr:{ *:[v4i32] } VR128:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2DQrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12502,
      GIR_Done,
    // Label 1189: @30731
    GIM_Try, /*On fail goto*//*Label 1190*/ 30754, // Rule ID 12506 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (fp_to_sint:{ *:[v4i32] } VR128:{ *:[v4f32] }:$src)  =>  (CVTTPS2DQrr:{ *:[v4i32] } VR128:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTTPS2DQrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12506,
      GIR_Done,
    // Label 1190: @30754
    GIM_Try, /*On fail goto*//*Label 1191*/ 30777, // Rule ID 12508 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (fp_to_sint:{ *:[v4i32] } VR256:{ *:[v4f64] }:$src)  =>  (VCVTTPD2DQYrr:{ *:[v4i32] } VR256:{ *:[v4f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2DQYrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12508,
      GIR_Done,
    // Label 1191: @30777
    GIM_Try, /*On fail goto*//*Label 1192*/ 30800, // Rule ID 14887 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_sint:{ *:[v4i32] } VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTPS2DQZ128rr:{ *:[v4i32] } VR128X:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2DQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14887,
      GIR_Done,
    // Label 1192: @30800
    GIM_Try, /*On fail goto*//*Label 1193*/ 30823, // Rule ID 14895 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_sint:{ *:[v4i32] } VR256X:{ *:[v4f64] }:$src)  =>  (VCVTTPD2DQZ256rr:{ *:[v4i32] } VR256X:{ *:[v4f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2DQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14895,
      GIR_Done,
    // Label 1193: @30823
    GIM_Reject,
    // Label 1171: @30824
    GIM_Try, /*On fail goto*//*Label 1194*/ 30847, // Rule ID 14907 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_sint:{ *:[v4i64] } VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTPS2QQZ256rr:{ *:[v4i64] } VR128X:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2QQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14907,
      GIR_Done,
    // Label 1194: @30847
    GIM_Try, /*On fail goto*//*Label 1195*/ 30870, // Rule ID 14915 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_sint:{ *:[v4i64] } VR256X:{ *:[v4f64] }:$src)  =>  (VCVTTPD2QQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2QQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14915,
      GIR_Done,
    // Label 1195: @30870
    GIM_Reject,
    // Label 1172: @30871
    GIM_Try, /*On fail goto*//*Label 1196*/ 30894, // Rule ID 12504 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (fp_to_sint:{ *:[v8i32] } VR256:{ *:[v8f32] }:$src)  =>  (VCVTTPS2DQYrr:{ *:[v8i32] } VR256:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2DQYrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12504,
      GIR_Done,
    // Label 1196: @30894
    GIM_Try, /*On fail goto*//*Label 1197*/ 30917, // Rule ID 14883 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_sint:{ *:[v8i32] } VR512:{ *:[v8f64] }:$src)  =>  (VCVTTPD2DQZrr:{ *:[v8i32] } VR512:{ *:[v8f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2DQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14883,
      GIR_Done,
    // Label 1197: @30917
    GIM_Try, /*On fail goto*//*Label 1198*/ 30940, // Rule ID 14891 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_sint:{ *:[v8i32] } VR256X:{ *:[v8f32] }:$src)  =>  (VCVTTPS2DQZ256rr:{ *:[v8i32] } VR256X:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2DQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14891,
      GIR_Done,
    // Label 1198: @30940
    GIM_Reject,
    // Label 1173: @30941
    GIM_Try, /*On fail goto*//*Label 1199*/ 30964, // Rule ID 14899 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_sint:{ *:[v8i64] } VR256X:{ *:[v8f32] }:$src)  =>  (VCVTTPS2QQZrr:{ *:[v8i64] } VR256X:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2QQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14899,
      GIR_Done,
    // Label 1199: @30964
    GIM_Try, /*On fail goto*//*Label 1200*/ 30987, // Rule ID 14903 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_sint:{ *:[v8i64] } VR512:{ *:[v8f64] }:$src)  =>  (VCVTTPD2QQZrr:{ *:[v8i64] } VR512:{ *:[v8f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2QQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14903,
      GIR_Done,
    // Label 1200: @30987
    GIM_Reject,
    // Label 1174: @30988
    GIM_Try, /*On fail goto*//*Label 1201*/ 31011, // Rule ID 14879 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_sint:{ *:[v16i32] } VR512:{ *:[v16f32] }:$src)  =>  (VCVTTPS2DQZrr:{ *:[v16i32] } VR512:{ *:[v16f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2DQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14879,
      GIR_Done,
    // Label 1201: @31011
    GIM_Reject,
    // Label 1175: @31012
    GIM_Reject,
    // Label 26: @31013
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1210*/ 31366,
    /*GILLT_s32*//*Label 1202*/ 31036,
    /*GILLT_s64*//*Label 1203*/ 31083, 0, 0, 0,
    /*GILLT_v2s64*//*Label 1204*/ 31130, 0,
    /*GILLT_v4s32*//*Label 1205*/ 31154,
    /*GILLT_v4s64*//*Label 1206*/ 31201, 0, 0,
    /*GILLT_v8s32*//*Label 1207*/ 31248,
    /*GILLT_v8s64*//*Label 1208*/ 31295, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1209*/ 31342,
    // Label 1202: @31036
    GIM_Try, /*On fail goto*//*Label 1211*/ 31059, // Rule ID 8070 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
      // (fp_to_uint:{ *:[i32] } FR32X:{ *:[f32] }:$src)  =>  (VCVTTSS2USIZrr:{ *:[i32] } FR32X:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2USIZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8070,
      GIR_Done,
    // Label 1211: @31059
    GIM_Try, /*On fail goto*//*Label 1212*/ 31082, // Rule ID 8080 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (fp_to_uint:{ *:[i32] } FR64X:{ *:[f64] }:$src)  =>  (VCVTTSD2USIZrr:{ *:[i32] } FR64X:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2USIZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8080,
      GIR_Done,
    // Label 1212: @31082
    GIM_Reject,
    // Label 1203: @31083
    GIM_Try, /*On fail goto*//*Label 1213*/ 31106, // Rule ID 8075 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR32XRegClassID,
      // (fp_to_uint:{ *:[i64] } FR32X:{ *:[f32] }:$src)  =>  (VCVTTSS2USI64Zrr:{ *:[i64] } FR32X:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSS2USI64Zrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8075,
      GIR_Done,
    // Label 1213: @31106
    GIM_Try, /*On fail goto*//*Label 1214*/ 31129, // Rule ID 8085 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::FR64XRegClassID,
      // (fp_to_uint:{ *:[i64] } FR64X:{ *:[f64] }:$src)  =>  (VCVTTSD2USI64Zrr:{ *:[i64] } FR64X:{ *:[f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTSD2USI64Zrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8085,
      GIR_Done,
    // Label 1214: @31129
    GIM_Reject,
    // Label 1204: @31130
    GIM_Try, /*On fail goto*//*Label 1215*/ 31153, // Rule ID 14913 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_uint:{ *:[v2i64] } VR128X:{ *:[v2f64] }:$src)  =>  (VCVTTPD2UQQZ128rr:{ *:[v2i64] } VR128X:{ *:[v2f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2UQQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14913,
      GIR_Done,
    // Label 1215: @31153
    GIM_Reject,
    // Label 1205: @31154
    GIM_Try, /*On fail goto*//*Label 1216*/ 31177, // Rule ID 14889 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_uint:{ *:[v4i32] } VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTPS2UDQZ128rr:{ *:[v4i32] } VR128X:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2UDQZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14889,
      GIR_Done,
    // Label 1216: @31177
    GIM_Try, /*On fail goto*//*Label 1217*/ 31200, // Rule ID 14897 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_uint:{ *:[v4i32] } VR256X:{ *:[v4f64] }:$src)  =>  (VCVTTPD2UDQZ256rr:{ *:[v4i32] } VR256X:{ *:[v4f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2UDQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14897,
      GIR_Done,
    // Label 1217: @31200
    GIM_Reject,
    // Label 1206: @31201
    GIM_Try, /*On fail goto*//*Label 1218*/ 31224, // Rule ID 14909 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (fp_to_uint:{ *:[v4i64] } VR128X:{ *:[v4f32] }:$src)  =>  (VCVTTPS2UQQZ256rr:{ *:[v4i64] } VR128X:{ *:[v4f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2UQQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14909,
      GIR_Done,
    // Label 1218: @31224
    GIM_Try, /*On fail goto*//*Label 1219*/ 31247, // Rule ID 14917 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_uint:{ *:[v4i64] } VR256X:{ *:[v4f64] }:$src)  =>  (VCVTTPD2UQQZ256rr:{ *:[v4i64] } VR256X:{ *:[v4f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2UQQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14917,
      GIR_Done,
    // Label 1219: @31247
    GIM_Reject,
    // Label 1207: @31248
    GIM_Try, /*On fail goto*//*Label 1220*/ 31271, // Rule ID 14885 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_uint:{ *:[v8i32] } VR512:{ *:[v8f64] }:$src)  =>  (VCVTTPD2UDQZrr:{ *:[v8i32] } VR512:{ *:[v8f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2UDQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14885,
      GIR_Done,
    // Label 1220: @31271
    GIM_Try, /*On fail goto*//*Label 1221*/ 31294, // Rule ID 14893 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_uint:{ *:[v8i32] } VR256X:{ *:[v8f32] }:$src)  =>  (VCVTTPS2UDQZ256rr:{ *:[v8i32] } VR256X:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2UDQZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14893,
      GIR_Done,
    // Label 1221: @31294
    GIM_Reject,
    // Label 1208: @31295
    GIM_Try, /*On fail goto*//*Label 1222*/ 31318, // Rule ID 14901 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (fp_to_uint:{ *:[v8i64] } VR256X:{ *:[v8f32] }:$src)  =>  (VCVTTPS2UQQZrr:{ *:[v8i64] } VR256X:{ *:[v8f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2UQQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14901,
      GIR_Done,
    // Label 1222: @31318
    GIM_Try, /*On fail goto*//*Label 1223*/ 31341, // Rule ID 14905 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_uint:{ *:[v8i64] } VR512:{ *:[v8f64] }:$src)  =>  (VCVTTPD2UQQZrr:{ *:[v8i64] } VR512:{ *:[v8f64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPD2UQQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14905,
      GIR_Done,
    // Label 1223: @31341
    GIM_Reject,
    // Label 1209: @31342
    GIM_Try, /*On fail goto*//*Label 1224*/ 31365, // Rule ID 14881 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (fp_to_uint:{ *:[v16i32] } VR512:{ *:[v16f32] }:$src)  =>  (VCVTTPS2UDQZrr:{ *:[v16i32] } VR512:{ *:[v16f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTTPS2UDQZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14881,
      GIR_Done,
    // Label 1224: @31365
    GIM_Reject,
    // Label 1210: @31366
    GIM_Reject,
    // Label 27: @31367
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1233*/ 32196,
    /*GILLT_s32*//*Label 1225*/ 31390,
    /*GILLT_s64*//*Label 1226*/ 31629, 0, 0, 0,
    /*GILLT_v2s64*//*Label 1227*/ 31868, 0,
    /*GILLT_v4s32*//*Label 1228*/ 31892,
    /*GILLT_v4s64*//*Label 1229*/ 31985, 0, 0,
    /*GILLT_v8s32*//*Label 1230*/ 32055,
    /*GILLT_v8s64*//*Label 1231*/ 32125, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1232*/ 32172,
    // Label 1225: @31390
    GIM_Try, /*On fail goto*//*Label 1234*/ 31413, // Rule ID 1355 //
      GIM_CheckFeatures, GIFBS_UseSSE1,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (CVTSI2SSrr:{ *:[f32] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSI2SSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1355,
      GIR_Done,
    // Label 1234: @31413
    GIM_Try, /*On fail goto*//*Label 1235*/ 31436, // Rule ID 1357 //
      GIM_CheckFeatures, GIFBS_UseSSE1,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f32] } GR64:{ *:[i64] }:$src)  =>  (CVTSI642SSrr:{ *:[f32] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSI642SSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1357,
      GIR_Done,
    // Label 1235: @31436
    GIM_Try, /*On fail goto*//*Label 1236*/ 31484, // Rule ID 12471 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (VCVTSI2SSrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI2SSrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12471,
      GIR_Done,
    // Label 1236: @31484
    GIM_Try, /*On fail goto*//*Label 1237*/ 31532, // Rule ID 12472 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f32] } GR64:{ *:[i64] }:$src)  =>  (VCVTSI642SSrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI642SSrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12472,
      GIR_Done,
    // Label 1237: @31532
    GIM_Try, /*On fail goto*//*Label 1238*/ 31580, // Rule ID 14823 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (VCVTSI2SSZrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI2SSZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14823,
      GIR_Done,
    // Label 1238: @31580
    GIM_Try, /*On fail goto*//*Label 1239*/ 31628, // Rule ID 14824 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f32] } GR64:{ *:[i64] }:$src)  =>  (VCVTSI642SSZrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI642SSZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14824,
      GIR_Done,
    // Label 1239: @31628
    GIM_Reject,
    // Label 1226: @31629
    GIM_Try, /*On fail goto*//*Label 1240*/ 31652, // Rule ID 1359 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f64] } GR32:{ *:[i32] }:$src)  =>  (CVTSI2SDrr:{ *:[f64] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSI2SDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1359,
      GIR_Done,
    // Label 1240: @31652
    GIM_Try, /*On fail goto*//*Label 1241*/ 31675, // Rule ID 1361 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (CVTSI642SDrr:{ *:[f64] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTSI642SDrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1361,
      GIR_Done,
    // Label 1241: @31675
    GIM_Try, /*On fail goto*//*Label 1242*/ 31723, // Rule ID 12473 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f64] } GR32:{ *:[i32] }:$src)  =>  (VCVTSI2SDrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI2SDrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12473,
      GIR_Done,
    // Label 1242: @31723
    GIM_Try, /*On fail goto*//*Label 1243*/ 31771, // Rule ID 12474 //
      GIM_CheckFeatures, GIFBS_UseAVX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (VCVTSI642SDrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI642SDrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 12474,
      GIR_Done,
    // Label 1243: @31771
    GIM_Try, /*On fail goto*//*Label 1244*/ 31819, // Rule ID 14825 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (sint_to_fp:{ *:[f64] } GR32:{ *:[i32] }:$src)  =>  (VCVTSI2SDZrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI2SDZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14825,
      GIR_Done,
    // Label 1244: @31819
    GIM_Try, /*On fail goto*//*Label 1245*/ 31867, // Rule ID 14826 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (sint_to_fp:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (VCVTSI642SDZrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTSI642SDZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14826,
      GIR_Done,
    // Label 1245: @31867
    GIM_Reject,
    // Label 1227: @31868
    GIM_Try, /*On fail goto*//*Label 1246*/ 31891, // Rule ID 8774 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (sint_to_fp:{ *:[v2f64] } VR128X:{ *:[v2i64] }:$src)  =>  (VCVTQQ2PDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTQQ2PDZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8774,
      GIR_Done,
    // Label 1246: @31891
    GIM_Reject,
    // Label 1228: @31892
    GIM_Try, /*On fail goto*//*Label 1247*/ 31915, // Rule ID 1395 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (sint_to_fp:{ *:[v4f32] } VR128:{ *:[v4i32] }:$src)  =>  (VCVTDQ2PSrr:{ *:[v4f32] } VR128:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1395,
      GIR_Done,
    // Label 1247: @31915
    GIM_Try, /*On fail goto*//*Label 1248*/ 31938, // Rule ID 1399 //
      GIM_CheckFeatures, GIFBS_UseSSE2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (sint_to_fp:{ *:[v4f32] } VR128:{ *:[v4i32] }:$src)  =>  (CVTDQ2PSrr:{ *:[v4f32] } VR128:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::CVTDQ2PSrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1399,
      GIR_Done,
    // Label 1248: @31938
    GIM_Try, /*On fail goto*//*Label 1249*/ 31961, // Rule ID 8207 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (sint_to_fp:{ *:[v4f32] } VR128X:{ *:[v4i32] }:$src)  =>  (VCVTDQ2PSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PSZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8207,
      GIR_Done,
    // Label 1249: @31961
    GIM_Try, /*On fail goto*//*Label 1250*/ 31984, // Rule ID 8843 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (sint_to_fp:{ *:[v4f32] } VR256X:{ *:[v4i64] }:$src)  =>  (VCVTQQ2PSZ256rr:{ *:[v4f32] } VR256X:{ *:[v4i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTQQ2PSZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8843,
      GIR_Done,
    // Label 1250: @31984
    GIM_Reject,
    // Label 1229: @31985
    GIM_Try, /*On fail goto*//*Label 1251*/ 32008, // Rule ID 1442 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128RegClassID,
      // (sint_to_fp:{ *:[v4f64] } VR128:{ *:[v4i32] }:$src)  =>  (VCVTDQ2PDYrr:{ *:[v4f64] } VR128:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PDYrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1442,
      GIR_Done,
    // Label 1251: @32008
    GIM_Try, /*On fail goto*//*Label 1252*/ 32031, // Rule ID 8186 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (sint_to_fp:{ *:[v4f64] } VR128X:{ *:[v4i32] }:$src)  =>  (VCVTDQ2PDZ256rr:{ *:[v4f64] } VR128X:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PDZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8186,
      GIR_Done,
    // Label 1252: @32031
    GIM_Try, /*On fail goto*//*Label 1253*/ 32054, // Rule ID 8783 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (sint_to_fp:{ *:[v4f64] } VR256X:{ *:[v4i64] }:$src)  =>  (VCVTQQ2PDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTQQ2PDZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8783,
      GIR_Done,
    // Label 1253: @32054
    GIM_Reject,
    // Label 1230: @32055
    GIM_Try, /*On fail goto*//*Label 1254*/ 32078, // Rule ID 1397 //
      GIM_CheckFeatures, GIFBS_HasAVX_NoVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256RegClassID,
      // (sint_to_fp:{ *:[v8f32] } VR256:{ *:[v8i32] }:$src)  =>  (VCVTDQ2PSYrr:{ *:[v8f32] } VR256:{ *:[v8i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PSYrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1397,
      GIR_Done,
    // Label 1254: @32078
    GIM_Try, /*On fail goto*//*Label 1255*/ 32101, // Rule ID 8216 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (sint_to_fp:{ *:[v8f32] } VR256X:{ *:[v8i32] }:$src)  =>  (VCVTDQ2PSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PSZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8216,
      GIR_Done,
    // Label 1255: @32101
    GIM_Try, /*On fail goto*//*Label 1256*/ 32124, // Rule ID 8822 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (sint_to_fp:{ *:[v8f32] } VR512:{ *:[v8i64] }:$src)  =>  (VCVTQQ2PSZrr:{ *:[v8f32] } VR512:{ *:[v8i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTQQ2PSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8822,
      GIR_Done,
    // Label 1256: @32124
    GIM_Reject,
    // Label 1231: @32125
    GIM_Try, /*On fail goto*//*Label 1257*/ 32148, // Rule ID 8168 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (sint_to_fp:{ *:[v8f64] } VR256X:{ *:[v8i32] }:$src)  =>  (VCVTDQ2PDZrr:{ *:[v8f64] } VR256X:{ *:[v8i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8168,
      GIR_Done,
    // Label 1257: @32148
    GIM_Try, /*On fail goto*//*Label 1258*/ 32171, // Rule ID 8762 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (sint_to_fp:{ *:[v8f64] } VR512:{ *:[v8i64] }:$src)  =>  (VCVTQQ2PDZrr:{ *:[v8f64] } VR512:{ *:[v8i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTQQ2PDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8762,
      GIR_Done,
    // Label 1258: @32171
    GIM_Reject,
    // Label 1232: @32172
    GIM_Try, /*On fail goto*//*Label 1259*/ 32195, // Rule ID 8195 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (sint_to_fp:{ *:[v16f32] } VR512:{ *:[v16i32] }:$src)  =>  (VCVTDQ2PSZrr:{ *:[v16f32] } VR512:{ *:[v16i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTDQ2PSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8195,
      GIR_Done,
    // Label 1259: @32195
    GIM_Reject,
    // Label 1233: @32196
    GIM_Reject,
    // Label 28: @32197
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/3, 20, /*)*//*default:*//*Label 1268*/ 32650,
    /*GILLT_s32*//*Label 1260*/ 32220,
    /*GILLT_s64*//*Label 1261*/ 32317, 0, 0, 0,
    /*GILLT_v2s64*//*Label 1262*/ 32414, 0,
    /*GILLT_v4s32*//*Label 1263*/ 32438,
    /*GILLT_v4s64*//*Label 1264*/ 32485, 0, 0,
    /*GILLT_v8s32*//*Label 1265*/ 32532,
    /*GILLT_v8s64*//*Label 1266*/ 32579, 0, 0, 0,
    /*GILLT_v16s32*//*Label 1267*/ 32626,
    // Label 1260: @32220
    GIM_Try, /*On fail goto*//*Label 1269*/ 32268, // Rule ID 14831 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (uint_to_fp:{ *:[f32] } GR32:{ *:[i32] }:$src)  =>  (VCVTUSI2SSZrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTUSI2SSZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14831,
      GIR_Done,
    // Label 1269: @32268
    GIM_Try, /*On fail goto*//*Label 1270*/ 32316, // Rule ID 14832 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR32XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (uint_to_fp:{ *:[f32] } GR64:{ *:[i64] }:$src)  =>  (VCVTUSI642SSZrr:{ *:[f32] } (IMPLICIT_DEF:{ *:[f32] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTUSI642SSZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14832,
      GIR_Done,
    // Label 1270: @32316
    GIM_Reject,
    // Label 1261: @32317
    GIM_Try, /*On fail goto*//*Label 1271*/ 32365, // Rule ID 14833 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (uint_to_fp:{ *:[f64] } GR32:{ *:[i32] }:$src)  =>  (VCVTUSI2SDZrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR32:{ *:[i32] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTUSI2SDZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14833,
      GIR_Done,
    // Label 1271: @32365
    GIM_Try, /*On fail goto*//*Label 1272*/ 32413, // Rule ID 14834 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::FR64XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (uint_to_fp:{ *:[f64] } GR64:{ *:[i64] }:$src)  =>  (VCVTUSI642SDZrr:{ *:[f64] } (IMPLICIT_DEF:{ *:[f64] }), GR64:{ *:[i64] }:$src)
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::IMPLICIT_DEF,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::VCVTUSI642SDZrr,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 14834,
      GIR_Done,
    // Label 1272: @32413
    GIM_Reject,
    // Label 1262: @32414
    GIM_Try, /*On fail goto*//*Label 1273*/ 32437, // Rule ID 8804 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (uint_to_fp:{ *:[v2f64] } VR128X:{ *:[v2i64] }:$src)  =>  (VCVTUQQ2PDZ128rr:{ *:[v2f64] } VR128X:{ *:[v2i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUQQ2PDZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8804,
      GIR_Done,
    // Label 1273: @32437
    GIM_Reject,
    // Label 1263: @32438
    GIM_Try, /*On fail goto*//*Label 1274*/ 32461, // Rule ID 8384 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (uint_to_fp:{ *:[v4f32] } VR128X:{ *:[v4i32] }:$src)  =>  (VCVTUDQ2PSZ128rr:{ *:[v4f32] } VR128X:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUDQ2PSZ128rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8384,
      GIR_Done,
    // Label 1274: @32461
    GIM_Try, /*On fail goto*//*Label 1275*/ 32484, // Rule ID 8873 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR128XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (uint_to_fp:{ *:[v4f32] } VR256X:{ *:[v4i64] }:$src)  =>  (VCVTUQQ2PSZ256rr:{ *:[v4f32] } VR256X:{ *:[v4i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUQQ2PSZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8873,
      GIR_Done,
    // Label 1275: @32484
    GIM_Reject,
    // Label 1264: @32485
    GIM_Try, /*On fail goto*//*Label 1276*/ 32508, // Rule ID 8363 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR128XRegClassID,
      // (uint_to_fp:{ *:[v4f64] } VR128X:{ *:[v4i32] }:$src)  =>  (VCVTUDQ2PDZ256rr:{ *:[v4f64] } VR128X:{ *:[v4i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUDQ2PDZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8363,
      GIR_Done,
    // Label 1276: @32508
    GIM_Try, /*On fail goto*//*Label 1277*/ 32531, // Rule ID 8813 //
      GIM_CheckFeatures, GIFBS_HasDQI_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (uint_to_fp:{ *:[v4f64] } VR256X:{ *:[v4i64] }:$src)  =>  (VCVTUQQ2PDZ256rr:{ *:[v4f64] } VR256X:{ *:[v4i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUQQ2PDZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8813,
      GIR_Done,
    // Label 1277: @32531
    GIM_Reject,
    // Label 1265: @32532
    GIM_Try, /*On fail goto*//*Label 1278*/ 32555, // Rule ID 8393 //
      GIM_CheckFeatures, GIFBS_HasVLX,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (uint_to_fp:{ *:[v8f32] } VR256X:{ *:[v8i32] }:$src)  =>  (VCVTUDQ2PSZ256rr:{ *:[v8f32] } VR256X:{ *:[v8i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUDQ2PSZ256rr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8393,
      GIR_Done,
    // Label 1278: @32555
    GIM_Try, /*On fail goto*//*Label 1279*/ 32578, // Rule ID 8852 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR256XRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (uint_to_fp:{ *:[v8f32] } VR512:{ *:[v8i64] }:$src)  =>  (VCVTUQQ2PSZrr:{ *:[v8f32] } VR512:{ *:[v8i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUQQ2PSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8852,
      GIR_Done,
    // Label 1279: @32578
    GIM_Reject,
    // Label 1266: @32579
    GIM_Try, /*On fail goto*//*Label 1280*/ 32602, // Rule ID 8345 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR256XRegClassID,
      // (uint_to_fp:{ *:[v8f64] } VR256X:{ *:[v8i32] }:$src)  =>  (VCVTUDQ2PDZrr:{ *:[v8f64] } VR256X:{ *:[v8i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUDQ2PDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8345,
      GIR_Done,
    // Label 1280: @32602
    GIM_Try, /*On fail goto*//*Label 1281*/ 32625, // Rule ID 8792 //
      GIM_CheckFeatures, GIFBS_HasDQI,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (uint_to_fp:{ *:[v8f64] } VR512:{ *:[v8i64] }:$src)  =>  (VCVTUQQ2PDZrr:{ *:[v8f64] } VR512:{ *:[v8i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUQQ2PDZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8792,
      GIR_Done,
    // Label 1281: @32625
    GIM_Reject,
    // Label 1267: @32626
    GIM_Try, /*On fail goto*//*Label 1282*/ 32649, // Rule ID 8372 //
      GIM_CheckFeatures, GIFBS_HasAVX512,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::VR512RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::VR512RegClassID,
      // (uint_to_fp:{ *:[v16f32] } VR512:{ *:[v16i32] }:$src)  =>  (VCVTUDQ2PSZrr:{ *:[v16f32] } VR512:{ *:[v16i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::VCVTUDQ2PSZrr,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 8372,
      GIR_Done,
    // Label 1282: @32649
    GIM_Reject,
    // Label 1268: @32650
    GIM_Reject,
    // Label 29: @32651
    GIM_Try, /*On fail goto*//*Label 1283*/ 32663, // Rule ID 552 //
      // MIs[0] dst
      GIM_CheckIsMBB, /*MI*/0, /*Op*/0,
      // (br (bb:{ *:[Other] }):$dst)  =>  (JMP_1 (bb:{ *:[Other] }):$dst)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::JMP_1,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 552,
      GIR_Done,
    // Label 1283: @32663
    GIM_Reject,
    // Label 30: @32664
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/2, 5, /*)*//*default:*//*Label 1287*/ 32753,
    /*GILLT_s16*//*Label 1284*/ 32673,
    /*GILLT_s32*//*Label 1285*/ 32709,
    /*GILLT_s64*//*Label 1286*/ 32731,
    // Label 1284: @32673
    GIM_Try, /*On fail goto*//*Label 1288*/ 32708, // Rule ID 16193 //
      GIM_CheckFeatures, GIFBS_HasMOVBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR16RegClassID,
      // (bswap:{ *:[i16] } GR16:{ *:[i16] }:$src)  =>  (ROL16ri:{ *:[i16] }:{ *:[i32] } GR16:{ *:[i16] }:$src, 8:{ *:[i8] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/X86::ROL16ri,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_AddImm, /*InsnID*/0, /*Imm*/8,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 16193,
      GIR_Done,
    // Label 1288: @32708
    GIM_Reject,
    // Label 1285: @32709
    GIM_Try, /*On fail goto*//*Label 1289*/ 32730, // Rule ID 5 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR32RegClassID,
      // (bswap:{ *:[i32] } GR32:{ *:[i32] }:$src)  =>  (BSWAP32r:{ *:[i32] } GR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::BSWAP32r,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 5,
      GIR_Done,
    // Label 1289: @32730
    GIM_Reject,
    // Label 1286: @32731
    GIM_Try, /*On fail goto*//*Label 1290*/ 32752, // Rule ID 6 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/X86::GR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/X86::GR64RegClassID,
      // (bswap:{ *:[i64] } GR64:{ *:[i64] }:$src)  =>  (BSWAP64r:{ *:[i64] } GR64:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/X86::BSWAP64r,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 6,
      GIR_Done,
    // Label 1290: @32752
    GIM_Reject,
    // Label 1287: @32753
    GIM_Reject,
    // Label 31: @32754
    GIM_Reject,
    };
  return MatchTable0;
}
#endif // ifdef GET_GLOBALISEL_IMPL
#ifdef GET_GLOBALISEL_PREDICATES_DECL
PredicateBitset AvailableModuleFeatures;
mutable PredicateBitset AvailableFunctionFeatures;
PredicateBitset getAvailableFeatures() const {
  return AvailableModuleFeatures | AvailableFunctionFeatures;
}
PredicateBitset
computeAvailableModuleFeatures(const X86Subtarget *Subtarget) const;
PredicateBitset
computeAvailableFunctionFeatures(const X86Subtarget *Subtarget,
                                 const MachineFunction *MF) const;
#endif // ifdef GET_GLOBALISEL_PREDICATES_DECL
#ifdef GET_GLOBALISEL_PREDICATES_INIT
AvailableModuleFeatures(computeAvailableModuleFeatures(&STI)),
AvailableFunctionFeatures()
#endif // ifdef GET_GLOBALISEL_PREDICATES_INIT
