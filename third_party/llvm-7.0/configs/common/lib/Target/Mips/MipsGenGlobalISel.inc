/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Global Instruction Selector for the Mips target                            *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_GLOBALISEL_PREDICATE_BITSET
const unsigned MAX_SUBTARGET_PREDICATES = 41;
using PredicateBitset = llvm::PredicateBitsetImpl<MAX_SUBTARGET_PREDICATES>;
#endif // ifdef GET_GLOBALISEL_PREDICATE_BITSET

#ifdef GET_GLOBALISEL_TEMPORARIES_DECL
  mutable MatcherState State;
  typedef ComplexRendererFns(MipsInstructionSelector::*ComplexMatcherMemFn)(MachineOperand &) const;
  typedef void(MipsInstructionSelector::*CustomRendererFn)(MachineInstrBuilder &, const MachineInstr&) const;
  const ISelInfoTy<PredicateBitset, ComplexMatcherMemFn, CustomRendererFn> ISelInfo;
  static MipsInstructionSelector::ComplexMatcherMemFn ComplexPredicateFns[];
  static MipsInstructionSelector::CustomRendererFn CustomRenderers[];
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
  Feature_HasMips2Bit = 7,
  Feature_HasMips3Bit = 16,
  Feature_HasMips4_32Bit = 26,
  Feature_NotMips4_32Bit = 27,
  Feature_HasMips4_32r2Bit = 17,
  Feature_HasMips32Bit = 3,
  Feature_HasMips32r2Bit = 6,
  Feature_HasMips32r6Bit = 28,
  Feature_NotMips32r6Bit = 4,
  Feature_IsGP64bitBit = 21,
  Feature_IsPTR64bitBit = 23,
  Feature_HasMips64Bit = 24,
  Feature_HasMips64r2Bit = 22,
  Feature_HasMips64r6Bit = 29,
  Feature_NotMips64r6Bit = 5,
  Feature_InMips16ModeBit = 30,
  Feature_NotInMips16ModeBit = 0,
  Feature_HasCnMipsBit = 25,
  Feature_NotCnMipsBit = 8,
  Feature_IsN64Bit = 37,
  Feature_RelocNotPICBit = 9,
  Feature_RelocPICBit = 36,
  Feature_NoNaNsFPMathBit = 20,
  Feature_HasStdEncBit = 1,
  Feature_NotDSPBit = 11,
  Feature_InMicroMipsBit = 34,
  Feature_NotInMicroMipsBit = 2,
  Feature_IsLEBit = 39,
  Feature_IsBEBit = 40,
  Feature_IsNotNaClBit = 18,
  Feature_HasEVABit = 35,
  Feature_HasMSABit = 33,
  Feature_HasMadd4Bit = 19,
  Feature_UseIndirectJumpsHazardBit = 12,
  Feature_NoIndirectJumpGuardsBit = 10,
  Feature_AllowFPOpFusionBit = 38,
  Feature_IsFP64bitBit = 15,
  Feature_NotFP64bitBit = 14,
  Feature_IsNotSoftFloatBit = 13,
  Feature_HasDSPBit = 31,
  Feature_HasDSPR2Bit = 32,
};

PredicateBitset MipsInstructionSelector::
computeAvailableModuleFeatures(const MipsSubtarget *Subtarget) const {
  PredicateBitset Features;
  if (Subtarget->hasMips2())
    Features[Feature_HasMips2Bit] = 1;
  if (Subtarget->hasMips3())
    Features[Feature_HasMips3Bit] = 1;
  if (Subtarget->hasMips4_32())
    Features[Feature_HasMips4_32Bit] = 1;
  if (!Subtarget->hasMips4_32())
    Features[Feature_NotMips4_32Bit] = 1;
  if (Subtarget->hasMips4_32r2())
    Features[Feature_HasMips4_32r2Bit] = 1;
  if (Subtarget->hasMips32())
    Features[Feature_HasMips32Bit] = 1;
  if (Subtarget->hasMips32r2())
    Features[Feature_HasMips32r2Bit] = 1;
  if (Subtarget->hasMips32r6())
    Features[Feature_HasMips32r6Bit] = 1;
  if (!Subtarget->hasMips32r6())
    Features[Feature_NotMips32r6Bit] = 1;
  if (Subtarget->isGP64bit())
    Features[Feature_IsGP64bitBit] = 1;
  if (Subtarget->isABI_N64())
    Features[Feature_IsPTR64bitBit] = 1;
  if (Subtarget->hasMips64())
    Features[Feature_HasMips64Bit] = 1;
  if (Subtarget->hasMips64r2())
    Features[Feature_HasMips64r2Bit] = 1;
  if (Subtarget->hasMips64r6())
    Features[Feature_HasMips64r6Bit] = 1;
  if (!Subtarget->hasMips64r6())
    Features[Feature_NotMips64r6Bit] = 1;
  if (Subtarget->inMips16Mode())
    Features[Feature_InMips16ModeBit] = 1;
  if (!Subtarget->inMips16Mode())
    Features[Feature_NotInMips16ModeBit] = 1;
  if (Subtarget->hasCnMips())
    Features[Feature_HasCnMipsBit] = 1;
  if (!Subtarget->hasCnMips())
    Features[Feature_NotCnMipsBit] = 1;
  if (Subtarget->isABI_N64())
    Features[Feature_IsN64Bit] = 1;
  if (!TM.isPositionIndependent())
    Features[Feature_RelocNotPICBit] = 1;
  if (TM.isPositionIndependent())
    Features[Feature_RelocPICBit] = 1;
  if (TM.Options.NoNaNsFPMath)
    Features[Feature_NoNaNsFPMathBit] = 1;
  if (Subtarget->hasStandardEncoding())
    Features[Feature_HasStdEncBit] = 1;
  if (!Subtarget->hasDSP())
    Features[Feature_NotDSPBit] = 1;
  if (Subtarget->inMicroMipsMode())
    Features[Feature_InMicroMipsBit] = 1;
  if (!Subtarget->inMicroMipsMode())
    Features[Feature_NotInMicroMipsBit] = 1;
  if (Subtarget->isLittle())
    Features[Feature_IsLEBit] = 1;
  if (!Subtarget->isLittle())
    Features[Feature_IsBEBit] = 1;
  if (!Subtarget->isTargetNaCl())
    Features[Feature_IsNotNaClBit] = 1;
  if (Subtarget->hasEVA())
    Features[Feature_HasEVABit] = 1;
  if (Subtarget->hasMSA())
    Features[Feature_HasMSABit] = 1;
  if (!Subtarget->disableMadd4())
    Features[Feature_HasMadd4Bit] = 1;
  if (Subtarget->useIndirectJumpsHazard())
    Features[Feature_UseIndirectJumpsHazardBit] = 1;
  if (!Subtarget->useIndirectJumpsHazard())
    Features[Feature_NoIndirectJumpGuardsBit] = 1;
  if (TM.Options.AllowFPOpFusion == FPOpFusion::Fast)
    Features[Feature_AllowFPOpFusionBit] = 1;
  if (Subtarget->isFP64bit())
    Features[Feature_IsFP64bitBit] = 1;
  if (!Subtarget->isFP64bit())
    Features[Feature_NotFP64bitBit] = 1;
  if (!Subtarget->useSoftFloat())
    Features[Feature_IsNotSoftFloatBit] = 1;
  if (Subtarget->hasDSP())
    Features[Feature_HasDSPBit] = 1;
  if (Subtarget->hasDSPR2())
    Features[Feature_HasDSPR2Bit] = 1;
  return Features;
}

PredicateBitset MipsInstructionSelector::
computeAvailableFunctionFeatures(const MipsSubtarget *Subtarget, const MachineFunction *MF) const {
  PredicateBitset Features;
  return Features;
}

// LLT Objects.
enum {
  GILLT_s16,
  GILLT_s32,
  GILLT_s64,
  GILLT_v2s16,
  GILLT_v2s64,
  GILLT_v4s8,
  GILLT_v4s32,
  GILLT_v8s16,
  GILLT_v16s8,
};
const static size_t NumTypeObjects = 9;
const static LLT TypeObjects[] = {
  LLT::scalar(16),
  LLT::scalar(32),
  LLT::scalar(64),
  LLT::vector(2, 16),
  LLT::vector(2, 64),
  LLT::vector(4, 8),
  LLT::vector(4, 32),
  LLT::vector(8, 16),
  LLT::vector(16, 8),
};

// Feature bitsets.
enum {
  GIFBS_Invalid,
  GIFBS_HasCnMips,
  GIFBS_HasDSP,
  GIFBS_HasDSPR2,
  GIFBS_HasMSA,
  GIFBS_InMicroMips,
  GIFBS_InMips16Mode,
  GIFBS_IsFP64bit,
  GIFBS_NotFP64bit,
  GIFBS_HasDSP_InMicroMips,
  GIFBS_HasDSP_NotInMicroMips,
  GIFBS_HasDSPR2_InMicroMips,
  GIFBS_HasMSA_HasStdEnc,
  GIFBS_HasMSA_IsBE,
  GIFBS_HasMSA_IsLE,
  GIFBS_HasMips32r6_InMicroMips,
  GIFBS_HasMips64r2_HasStdEnc,
  GIFBS_HasMips64r6_HasStdEnc,
  GIFBS_HasStdEnc_IsNotSoftFloat,
  GIFBS_HasStdEnc_NotInMicroMips,
  GIFBS_HasStdEnc_NotMips4_32,
  GIFBS_InMicroMips_IsFP64bit,
  GIFBS_InMicroMips_IsNotSoftFloat,
  GIFBS_InMicroMips_NotFP64bit,
  GIFBS_InMicroMips_NotMips32r6,
  GIFBS_IsGP64bit_NotInMips16Mode,
  GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
  GIFBS_HasMSA_HasMips64_HasStdEnc,
  GIFBS_HasMips3_HasStdEnc_IsGP64bit,
  GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
  GIFBS_HasMips32r2_HasStdEnc_NotInMicroMips,
  GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
  GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
  GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
  GIFBS_HasStdEnc_IsFP64bit_NotInMicroMips,
  GIFBS_HasStdEnc_IsFP64bit_NotMips4_32,
  GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
  GIFBS_HasStdEnc_NotFP64bit_NotInMicroMips,
  GIFBS_HasStdEnc_NotFP64bit_NotMips4_32,
  GIFBS_HasStdEnc_NotInMicroMips_RelocNotPIC,
  GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
  GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
  GIFBS_InMicroMips_NotFP64bit_NotMips32r6,
  GIFBS_InMicroMips_NotMips32r6_RelocNotPIC,
  GIFBS_InMicroMips_NotMips32r6_RelocPIC,
  GIFBS_HasMips3_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
  GIFBS_HasMips32r6_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
  GIFBS_HasMips4_32_HasStdEnc_NotMips32r6_NotMips64r6,
  GIFBS_HasMips64r2_HasStdEnc_IsGP64bit_NotInMicroMips,
  GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
  GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
  GIFBS_HasMips32_HasStdEnc_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMips4_32_HasStdEnc_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotMips32r6_NotMips32r6,
  GIFBS_HasMips4_32_HasStdEnc_IsFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMips4_32_HasStdEnc_IsGP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMips4_32_HasStdEnc_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotFP64bit_NotMips32r6_NotMips32r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
  GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
};
const static PredicateBitset FeatureBitsets[] {
  {}, // GIFBS_Invalid
  {Feature_HasCnMipsBit, },
  {Feature_HasDSPBit, },
  {Feature_HasDSPR2Bit, },
  {Feature_HasMSABit, },
  {Feature_InMicroMipsBit, },
  {Feature_InMips16ModeBit, },
  {Feature_IsFP64bitBit, },
  {Feature_NotFP64bitBit, },
  {Feature_HasDSPBit, Feature_InMicroMipsBit, },
  {Feature_HasDSPBit, Feature_NotInMicroMipsBit, },
  {Feature_HasDSPR2Bit, Feature_InMicroMipsBit, },
  {Feature_HasMSABit, Feature_HasStdEncBit, },
  {Feature_HasMSABit, Feature_IsBEBit, },
  {Feature_HasMSABit, Feature_IsLEBit, },
  {Feature_HasMips32r6Bit, Feature_InMicroMipsBit, },
  {Feature_HasMips64r2Bit, Feature_HasStdEncBit, },
  {Feature_HasMips64r6Bit, Feature_HasStdEncBit, },
  {Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, },
  {Feature_HasStdEncBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_NotMips4_32Bit, },
  {Feature_InMicroMipsBit, Feature_IsFP64bitBit, },
  {Feature_InMicroMipsBit, Feature_IsNotSoftFloatBit, },
  {Feature_InMicroMipsBit, Feature_NotFP64bitBit, },
  {Feature_InMicroMipsBit, Feature_NotMips32r6Bit, },
  {Feature_IsGP64bitBit, Feature_NotInMips16ModeBit, },
  {Feature_AllowFPOpFusionBit, Feature_HasMSABit, Feature_HasStdEncBit, },
  {Feature_HasMSABit, Feature_HasMips64Bit, Feature_HasStdEncBit, },
  {Feature_HasMips3Bit, Feature_HasStdEncBit, Feature_IsGP64bitBit, },
  {Feature_HasMips3Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips32r2Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips32r6Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips32r6Bit, Feature_InMicroMipsBit, Feature_IsNotSoftFloatBit, },
  {Feature_HasMips64r6Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_NotMips4_32Bit, },
  {Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_NotFP64bitBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_NotFP64bitBit, Feature_NotMips4_32Bit, },
  {Feature_HasStdEncBit, Feature_NotInMicroMipsBit, Feature_RelocNotPICBit, },
  {Feature_InMicroMipsBit, Feature_IsFP64bitBit, Feature_IsNotSoftFloatBit, },
  {Feature_InMicroMipsBit, Feature_IsNotSoftFloatBit, Feature_NotFP64bitBit, },
  {Feature_InMicroMipsBit, Feature_NotFP64bitBit, Feature_NotMips32r6Bit, },
  {Feature_InMicroMipsBit, Feature_NotMips32r6Bit, Feature_RelocNotPICBit, },
  {Feature_InMicroMipsBit, Feature_NotMips32r6Bit, Feature_RelocPICBit, },
  {Feature_HasMips3Bit, Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips32r6Bit, Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips4_32Bit, Feature_HasStdEncBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMips64r2Bit, Feature_HasStdEncBit, Feature_IsGP64bitBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, },
  {Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotFP64bitBit, Feature_NotInMicroMipsBit, },
  {Feature_HasMips32Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMips4_32Bit, Feature_HasStdEncBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_InMicroMipsBit, Feature_InMicroMipsBit, Feature_NoNaNsFPMathBit, Feature_NotMips32r6Bit, Feature_NotMips32r6Bit, },
  {Feature_HasMips4_32Bit, Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMips4_32Bit, Feature_HasStdEncBit, Feature_IsGP64bitBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMips4_32Bit, Feature_HasStdEncBit, Feature_NotFP64bitBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_NoNaNsFPMathBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_InMicroMipsBit, Feature_InMicroMipsBit, Feature_NoNaNsFPMathBit, Feature_NotFP64bitBit, Feature_NotMips32r6Bit, Feature_NotMips32r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_IsNotSoftFloatBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_IsFP64bitBit, Feature_NoNaNsFPMathBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_IsNotSoftFloatBit, Feature_NotFP64bitBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
  {Feature_HasMadd4Bit, Feature_HasMips4_32r2Bit, Feature_HasStdEncBit, Feature_NoNaNsFPMathBit, Feature_NotFP64bitBit, Feature_NotInMicroMipsBit, Feature_NotMips32r6Bit, Feature_NotMips64r6Bit, },
};

// ComplexPattern predicates.
enum {
  GICP_Invalid,
};
// See constructor for table contents

// PatFrag predicates.
enum {
  GIPFP_I64_Predicate_immLi16 = GIPFP_I64_Invalid + 1,
  GIPFP_I64_Predicate_immSExt10,
  GIPFP_I64_Predicate_immSExt6,
  GIPFP_I64_Predicate_immSExtAddiur2,
  GIPFP_I64_Predicate_immSExtAddius5,
  GIPFP_I64_Predicate_immZExt1,
  GIPFP_I64_Predicate_immZExt10,
  GIPFP_I64_Predicate_immZExt1Ptr,
  GIPFP_I64_Predicate_immZExt2,
  GIPFP_I64_Predicate_immZExt2Lsa,
  GIPFP_I64_Predicate_immZExt2Ptr,
  GIPFP_I64_Predicate_immZExt2Shift,
  GIPFP_I64_Predicate_immZExt3,
  GIPFP_I64_Predicate_immZExt3Ptr,
  GIPFP_I64_Predicate_immZExt4,
  GIPFP_I64_Predicate_immZExt4Ptr,
  GIPFP_I64_Predicate_immZExt5,
  GIPFP_I64_Predicate_immZExt5_64,
  GIPFP_I64_Predicate_immZExt6,
  GIPFP_I64_Predicate_immZExt8,
  GIPFP_I64_Predicate_immZExtAndi16,
  GIPFP_I64_Predicate_immi32Cst15,
  GIPFP_I64_Predicate_immi32Cst31,
  GIPFP_I64_Predicate_immi32Cst7,
};
bool MipsInstructionSelector::testImmPredicate_I64(unsigned PredicateID, int64_t Imm) const {
  switch (PredicateID) {
  case GIPFP_I64_Predicate_immLi16: {
    return Imm >= -1 && Imm <= 126;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immSExt10: {
    return isInt<10>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immSExt6: {
    return isInt<6>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immSExtAddiur2: {
    return Imm == 1 || Imm == -1 ||
                                           ((Imm % 4 == 0) &&
                                            Imm < 28 && Imm > 0);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immSExtAddius5: {
    return Imm >= -8 && Imm <= 7;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt1: {
    return isUInt<1>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt10: {
    return isUInt<10>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt1Ptr: {
    return isUInt<1>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt2: {
    return isUInt<2>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt2Lsa: {
    return isUInt<2>(Imm - 1);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt2Ptr: {
    return isUInt<2>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt2Shift: {
    return Imm >= 1 && Imm <= 8;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt3: {
    return isUInt<3>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt3Ptr: {
    return isUInt<3>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt4: {
    return isUInt<4>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt4Ptr: {
    return isUInt<4>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt5: {
    return Imm == (Imm & 0x1f);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt5_64: {
     return Imm == (Imm & 0x1f); 
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt6: {
    return Imm == (Imm & 0x3f);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExt8: {
    return isUInt<8>(Imm);
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immZExtAndi16: {
    return (Imm == 128 || (Imm >= 1 && Imm <= 4) || Imm == 7 || Imm == 8 ||
            Imm == 15 || Imm == 16 || Imm == 31 || Imm == 32 || Imm == 63 ||
            Imm == 64 || Imm == 255 || Imm == 32768 || Imm == 65535 );
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immi32Cst15: {
    return isUInt<32>(Imm) && Imm == 15;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immi32Cst31: {
    return isUInt<32>(Imm) && Imm == 31;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  case GIPFP_I64_Predicate_immi32Cst7: {
    return isUInt<32>(Imm) && Imm == 7;
    llvm_unreachable("ImmediateCode should have returned");
    return false;
  }
  }
  llvm_unreachable("Unknown predicate");
  return false;
}
bool MipsInstructionSelector::testImmPredicate_APFloat(unsigned PredicateID, const APFloat & Imm) const {
  llvm_unreachable("Unknown predicate");
  return false;
}
bool MipsInstructionSelector::testImmPredicate_APInt(unsigned PredicateID, const APInt & Imm) const {
  llvm_unreachable("Unknown predicate");
  return false;
}
bool MipsInstructionSelector::testMIPredicate_MI(unsigned PredicateID, const MachineInstr & MI) const {
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineRegisterInfo &MRI = MF.getRegInfo();
  (void)MRI;
  llvm_unreachable("Unknown predicate");
  return false;
}

MipsInstructionSelector::ComplexMatcherMemFn
MipsInstructionSelector::ComplexPredicateFns[] = {
  nullptr, // GICP_Invalid
};

// Custom renderers.
enum {
  GICR_Invalid,
};
MipsInstructionSelector::CustomRendererFn
MipsInstructionSelector::CustomRenderers[] = {
  nullptr, // GICP_Invalid
};

bool MipsInstructionSelector::selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const {
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

const int64_t *MipsInstructionSelector::getMatchTable() const {
  constexpr static int64_t MatchTable0[] = {
    GIM_SwitchOpcode, /*MI*/0, /*[*/34, 124, /*)*//*default:*//*Label 40*/ 37669,
    /*TargetOpcode::G_ADD*//*Label 0*/ 95,
    /*TargetOpcode::G_SUB*//*Label 1*/ 1272,
    /*TargetOpcode::G_MUL*//*Label 2*/ 1884,
    /*TargetOpcode::G_SDIV*//*Label 3*/ 2260,
    /*TargetOpcode::G_UDIV*//*Label 4*/ 2481,
    /*TargetOpcode::G_SREM*//*Label 5*/ 2702,
    /*TargetOpcode::G_UREM*//*Label 6*/ 2923,
    /*TargetOpcode::G_AND*//*Label 7*/ 3144,
    /*TargetOpcode::G_OR*//*Label 8*/ 3588,
    /*TargetOpcode::G_XOR*//*Label 9*/ 3890, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_BITCAST*//*Label 10*/ 4684,
    /*TargetOpcode::G_LOAD*//*Label 11*/ 8337,
    /*TargetOpcode::G_SEXTLOAD*//*Label 12*/ 8403,
    /*TargetOpcode::G_ZEXTLOAD*//*Label 13*/ 8469, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_INTRINSIC*//*Label 14*/ 8535,
    /*TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS*//*Label 15*/ 25046, 0,
    /*TargetOpcode::G_TRUNC*//*Label 16*/ 29970,
    /*TargetOpcode::G_CONSTANT*//*Label 17*/ 30027, 0, 0, 0,
    /*TargetOpcode::G_SEXT*//*Label 18*/ 30087,
    /*TargetOpcode::G_ZEXT*//*Label 19*/ 30115,
    /*TargetOpcode::G_SHL*//*Label 20*/ 30200,
    /*TargetOpcode::G_LSHR*//*Label 21*/ 30724,
    /*TargetOpcode::G_ASHR*//*Label 22*/ 31248, 0, 0,
    /*TargetOpcode::G_SELECT*//*Label 23*/ 31729, 0, 0, 0, 0, 0, 0, 0, 0,
    /*TargetOpcode::G_FADD*//*Label 24*/ 33183,
    /*TargetOpcode::G_FSUB*//*Label 25*/ 34062,
    /*TargetOpcode::G_FMUL*//*Label 26*/ 34638,
    /*TargetOpcode::G_FMA*//*Label 27*/ 35075,
    /*TargetOpcode::G_FDIV*//*Label 28*/ 35165, 0, 0, 0,
    /*TargetOpcode::G_FEXP2*//*Label 29*/ 35416, 0,
    /*TargetOpcode::G_FLOG2*//*Label 30*/ 35474,
    /*TargetOpcode::G_FNEG*//*Label 31*/ 35532,
    /*TargetOpcode::G_FPEXT*//*Label 32*/ 36828,
    /*TargetOpcode::G_FPTRUNC*//*Label 33*/ 36977,
    /*TargetOpcode::G_FPTOSI*//*Label 34*/ 37105,
    /*TargetOpcode::G_FPTOUI*//*Label 35*/ 37163,
    /*TargetOpcode::G_SITOFP*//*Label 36*/ 37221,
    /*TargetOpcode::G_UITOFP*//*Label 37*/ 37374, 0, 0, 0,
    /*TargetOpcode::G_BR*//*Label 38*/ 37432, 0, 0, 0,
    /*TargetOpcode::G_BSWAP*//*Label 39*/ 37517,
    // Label 0: @95
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 49*/ 1271,
    /*GILLT_s32*//*Label 41*/ 109,
    /*GILLT_s64*//*Label 42*/ 458,
    /*GILLT_v2s16*//*Label 43*/ 621,
    /*GILLT_v2s64*//*Label 44*/ 648,
    /*GILLT_v4s8*//*Label 45*/ 797,
    /*GILLT_v4s32*//*Label 46*/ 824,
    /*GILLT_v8s16*//*Label 47*/ 973,
    /*GILLT_v16s8*//*Label 48*/ 1122,
    // Label 41: @109
    GIM_Try, /*On fail goto*//*Label 50*/ 457,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 51*/ 187, // Rule ID 2291 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SHL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/2, /*Predicate*/GIPFP_I64_Predicate_immZExt2Lsa,
        // MIs[2] Operand 1
        // No operand predicates
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (add:{ *:[i32] } (shl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2Lsa>>:$sa), GPR32Opnd:{ *:[i32] }:$rt)  =>  (LSA:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LSA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/2, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2291,
        GIR_Done,
      // Label 51: @187
      GIM_Try, /*On fail goto*//*Label 52*/ 255, // Rule ID 802 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SHL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/2, /*Predicate*/GIPFP_I64_Predicate_immZExt2Lsa,
        // MIs[2] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (add:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (shl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2Lsa>>:$sa))  =>  (LSA:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LSA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/2, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 802,
        GIR_Done,
      // Label 52: @255
      GIM_Try, /*On fail goto*//*Label 53*/ 298, // Rule ID 2067 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immSExtAddiur2,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immSExtAddiur2>>:$imm)  =>  (ADDIUR2_MM:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immSExtAddiur2>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDIUR2_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2067,
        GIR_Done,
      // Label 53: @298
      GIM_Try, /*On fail goto*//*Label 54*/ 341, // Rule ID 2068 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immSExtAddius5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immSExtAddius5>>:$imm)  =>  (ADDIUS5_MM:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immSExtAddius5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDIUS5_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2068,
        GIR_Done,
      // Label 54: @341
      GIM_Try, /*On fail goto*//*Label 55*/ 364, // Rule ID 1165 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (add:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (ADDU16_MMR6:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDU16_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1165,
        GIR_Done,
      // Label 55: @364
      GIM_Try, /*On fail goto*//*Label 56*/ 387, // Rule ID 34 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (add:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDu:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDu,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 34,
        GIR_Done,
      // Label 56: @387
      GIM_Try, /*On fail goto*//*Label 57*/ 410, // Rule ID 1028 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (add:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (ADDU16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDU16_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1028,
        GIR_Done,
      // Label 57: @410
      GIM_Try, /*On fail goto*//*Label 58*/ 433, // Rule ID 1040 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (add:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDu_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDu_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1040,
        GIR_Done,
      // Label 58: @433
      GIM_Try, /*On fail goto*//*Label 59*/ 456, // Rule ID 1736 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (add:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (AdduRxRyRz16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AdduRxRyRz16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1736,
        GIR_Done,
      // Label 59: @456
      GIM_Reject,
    // Label 50: @457
    GIM_Reject,
    // Label 42: @458
    GIM_Try, /*On fail goto*//*Label 60*/ 620,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 61*/ 536, // Rule ID 2292 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasMips64_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SHL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/2, /*Predicate*/GIPFP_I64_Predicate_immZExt2Lsa,
        // MIs[2] Operand 1
        // No operand predicates
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (add:{ *:[i64] } (shl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2Lsa>>:$sa), GPR64Opnd:{ *:[i64] }:$rt)  =>  (DLSA:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DLSA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/2, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2292,
        GIR_Done,
      // Label 61: @536
      GIM_Try, /*On fail goto*//*Label 62*/ 600, // Rule ID 803 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasMips64_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_SHL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/2, /*Predicate*/GIPFP_I64_Predicate_immZExt2Lsa,
        // MIs[2] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (add:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (shl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2Lsa>>:$sa))  =>  (DLSA:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DLSA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/2, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 803,
        GIR_Done,
      // Label 62: @600
      GIM_Try, /*On fail goto*//*Label 63*/ 619, // Rule ID 180 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        // (add:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DADDu:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DADDu,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 180,
        GIR_Done,
      // Label 63: @619
      GIM_Reject,
    // Label 60: @620
    GIM_Reject,
    // Label 43: @621
    GIM_Try, /*On fail goto*//*Label 64*/ 647, // Rule ID 1835 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      // (add:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (ADDQ_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDQ_PH,
      GIR_AddImplicitDef, /*InsnID*/0, Mips::DSPOutFlag20,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1835,
      GIR_Done,
    // Label 64: @647
    GIM_Reject,
    // Label 44: @648
    GIM_Try, /*On fail goto*//*Label 65*/ 796,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_Try, /*On fail goto*//*Label 66*/ 719, // Rule ID 2296 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v2i64] } (mul:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt), MSA128DOpnd:{ *:[v2i64] }:$wd_in)  =>  (MADDV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2296,
        GIR_Done,
      // Label 66: @719
      GIM_Try, /*On fail goto*//*Label 67*/ 776, // Rule ID 811 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, (mul:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt))  =>  (MADDV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 811,
        GIR_Done,
      // Label 67: @776
      GIM_Try, /*On fail goto*//*Label 68*/ 795, // Rule ID 478 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (add:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ADDV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDV_D,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 478,
        GIR_Done,
      // Label 68: @795
      GIM_Reject,
    // Label 65: @796
    GIM_Reject,
    // Label 45: @797
    GIM_Try, /*On fail goto*//*Label 69*/ 823, // Rule ID 1841 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      // (add:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)  =>  (ADDU_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDU_QB,
      GIR_AddImplicitDef, /*InsnID*/0, Mips::DSPOutFlag20,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1841,
      GIR_Done,
    // Label 69: @823
    GIM_Reject,
    // Label 46: @824
    GIM_Try, /*On fail goto*//*Label 70*/ 972,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_Try, /*On fail goto*//*Label 71*/ 895, // Rule ID 2295 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v4i32] } (mul:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt), MSA128WOpnd:{ *:[v4i32] }:$wd_in)  =>  (MADDV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2295,
        GIR_Done,
      // Label 71: @895
      GIM_Try, /*On fail goto*//*Label 72*/ 952, // Rule ID 810 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, (mul:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt))  =>  (MADDV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 810,
        GIR_Done,
      // Label 72: @952
      GIM_Try, /*On fail goto*//*Label 73*/ 971, // Rule ID 477 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (add:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ADDV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDV_W,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 477,
        GIR_Done,
      // Label 73: @971
      GIM_Reject,
    // Label 70: @972
    GIM_Reject,
    // Label 47: @973
    GIM_Try, /*On fail goto*//*Label 74*/ 1121,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_Try, /*On fail goto*//*Label 75*/ 1044, // Rule ID 2294 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v8i16] } (mul:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt), MSA128HOpnd:{ *:[v8i16] }:$wd_in)  =>  (MADDV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2294,
        GIR_Done,
      // Label 75: @1044
      GIM_Try, /*On fail goto*//*Label 76*/ 1101, // Rule ID 809 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, (mul:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt))  =>  (MADDV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 809,
        GIR_Done,
      // Label 76: @1101
      GIM_Try, /*On fail goto*//*Label 77*/ 1120, // Rule ID 476 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (add:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ADDV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDV_H,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 476,
        GIR_Done,
      // Label 77: @1120
      GIM_Reject,
    // Label 74: @1121
    GIM_Reject,
    // Label 48: @1122
    GIM_Try, /*On fail goto*//*Label 78*/ 1270,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_Try, /*On fail goto*//*Label 79*/ 1193, // Rule ID 2293 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v16i8] } (mul:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt), MSA128BOpnd:{ *:[v16i8] }:$wd_in)  =>  (MADDV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2293,
        GIR_Done,
      // Label 79: @1193
      GIM_Try, /*On fail goto*//*Label 80*/ 1250, // Rule ID 808 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (add:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, (mul:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt))  =>  (MADDV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDV_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 808,
        GIR_Done,
      // Label 80: @1250
      GIM_Try, /*On fail goto*//*Label 81*/ 1269, // Rule ID 475 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        // (add:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ADDV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::ADDV_B,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 475,
        GIR_Done,
      // Label 81: @1269
      GIM_Reject,
    // Label 78: @1270
    GIM_Reject,
    // Label 49: @1271
    GIM_Reject,
    // Label 1: @1272
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 90*/ 1883,
    /*GILLT_s32*//*Label 82*/ 1286,
    /*GILLT_s64*//*Label 83*/ 1445,
    /*GILLT_v2s16*//*Label 84*/ 1477,
    /*GILLT_v2s64*//*Label 85*/ 1504,
    /*GILLT_v4s8*//*Label 86*/ 1592,
    /*GILLT_v4s32*//*Label 87*/ 1619,
    /*GILLT_v8s16*//*Label 88*/ 1707,
    /*GILLT_v16s8*//*Label 89*/ 1795,
    // Label 82: @1286
    GIM_Try, /*On fail goto*//*Label 91*/ 1444,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 92*/ 1328, // Rule ID 1735 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/1, 0,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (sub:{ *:[i32] } 0:{ *:[i32] }, CPU16Regs:{ *:[i32] }:$r)  =>  (NegRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NegRxRy16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // r
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1735,
        GIR_Done,
      // Label 92: @1328
      GIM_Try, /*On fail goto*//*Label 93*/ 1351, // Rule ID 1167 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (sub:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (SUBU16_MMR6:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBU16_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1167,
        GIR_Done,
      // Label 93: @1351
      GIM_Try, /*On fail goto*//*Label 94*/ 1374, // Rule ID 35 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (sub:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBu:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBu,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 35,
        GIR_Done,
      // Label 94: @1374
      GIM_Try, /*On fail goto*//*Label 95*/ 1397, // Rule ID 1032 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (sub:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (SUBU16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBU16_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1032,
        GIR_Done,
      // Label 95: @1397
      GIM_Try, /*On fail goto*//*Label 96*/ 1420, // Rule ID 1041 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (sub:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBu_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBu_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1041,
        GIR_Done,
      // Label 96: @1420
      GIM_Try, /*On fail goto*//*Label 97*/ 1443, // Rule ID 1740 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (sub:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (SubuRxRyRz16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SubuRxRyRz16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1740,
        GIR_Done,
      // Label 97: @1443
      GIM_Reject,
    // Label 91: @1444
    GIM_Reject,
    // Label 83: @1445
    GIM_Try, /*On fail goto*//*Label 98*/ 1476, // Rule ID 181 //
      GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (sub:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DSUBu:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DSUBu,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 181,
      GIR_Done,
    // Label 98: @1476
    GIM_Reject,
    // Label 84: @1477
    GIM_Try, /*On fail goto*//*Label 99*/ 1503, // Rule ID 1837 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      // (sub:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (SUBQ_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBQ_PH,
      GIR_AddImplicitDef, /*InsnID*/0, Mips::DSPOutFlag20,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1837,
      GIR_Done,
    // Label 99: @1503
    GIM_Reject,
    // Label 85: @1504
    GIM_Try, /*On fail goto*//*Label 100*/ 1591,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_Try, /*On fail goto*//*Label 101*/ 1575, // Rule ID 867 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, (mul:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt))  =>  (MSUBV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBV_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 867,
        GIR_Done,
      // Label 101: @1575
      GIM_Try, /*On fail goto*//*Label 102*/ 1590, // Rule ID 996 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (sub:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SUBV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBV_D,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 996,
        GIR_Done,
      // Label 102: @1590
      GIM_Reject,
    // Label 100: @1591
    GIM_Reject,
    // Label 86: @1592
    GIM_Try, /*On fail goto*//*Label 103*/ 1618, // Rule ID 1843 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      // (sub:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)  =>  (SUBU_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBU_QB,
      GIR_AddImplicitDef, /*InsnID*/0, Mips::DSPOutFlag20,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1843,
      GIR_Done,
    // Label 103: @1618
    GIM_Reject,
    // Label 87: @1619
    GIM_Try, /*On fail goto*//*Label 104*/ 1706,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_Try, /*On fail goto*//*Label 105*/ 1690, // Rule ID 866 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, (mul:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt))  =>  (MSUBV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBV_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 866,
        GIR_Done,
      // Label 105: @1690
      GIM_Try, /*On fail goto*//*Label 106*/ 1705, // Rule ID 995 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (sub:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SUBV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBV_W,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 995,
        GIR_Done,
      // Label 106: @1705
      GIM_Reject,
    // Label 104: @1706
    GIM_Reject,
    // Label 88: @1707
    GIM_Try, /*On fail goto*//*Label 107*/ 1794,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_Try, /*On fail goto*//*Label 108*/ 1778, // Rule ID 865 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, (mul:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt))  =>  (MSUBV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBV_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 865,
        GIR_Done,
      // Label 108: @1778
      GIM_Try, /*On fail goto*//*Label 109*/ 1793, // Rule ID 994 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (sub:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SUBV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBV_H,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 994,
        GIR_Done,
      // Label 109: @1793
      GIM_Reject,
    // Label 107: @1794
    GIM_Reject,
    // Label 89: @1795
    GIM_Try, /*On fail goto*//*Label 110*/ 1882,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_Try, /*On fail goto*//*Label 111*/ 1866, // Rule ID 864 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_MUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sub:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, (mul:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt))  =>  (MSUBV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBV_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 864,
        GIR_Done,
      // Label 111: @1866
      GIM_Try, /*On fail goto*//*Label 112*/ 1881, // Rule ID 993 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        // (sub:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SUBV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SUBV_B,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 993,
        GIR_Done,
      // Label 112: @1881
      GIM_Reject,
    // Label 110: @1882
    GIM_Reject,
    // Label 90: @1883
    GIM_Reject,
    // Label 2: @1884
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 120*/ 2259,
    /*GILLT_s32*//*Label 113*/ 1898,
    /*GILLT_s64*//*Label 114*/ 2043,
    /*GILLT_v2s16*//*Label 115*/ 2104,
    /*GILLT_v2s64*//*Label 116*/ 2131, 0,
    /*GILLT_v4s32*//*Label 117*/ 2163,
    /*GILLT_v8s16*//*Label 118*/ 2195,
    /*GILLT_v16s8*//*Label 119*/ 2227,
    // Label 113: @1898
    GIM_Try, /*On fail goto*//*Label 121*/ 2042,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 122*/ 1937, // Rule ID 36 //
        GIM_CheckFeatures, GIFBS_HasMips32_HasStdEnc_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (mul:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MUL:{ *:[i32] }:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MUL,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::HI0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::LO0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 36,
        GIR_Done,
      // Label 122: @1937
      GIM_Try, /*On fail goto*//*Label 123*/ 1960, // Rule ID 304 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (mul:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MUL_R6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MUL_R6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 304,
        GIR_Done,
      // Label 123: @1960
      GIM_Try, /*On fail goto*//*Label 124*/ 1989, // Rule ID 1042 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (mul:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MUL_MM:{ *:[i32] }:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MUL_MM,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::HI0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::LO0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1042,
        GIR_Done,
      // Label 124: @1989
      GIM_Try, /*On fail goto*//*Label 125*/ 2012, // Rule ID 1136 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (mul:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MUL_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MUL_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1136,
        GIR_Done,
      // Label 125: @2012
      GIM_Try, /*On fail goto*//*Label 126*/ 2041, // Rule ID 1738 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (mul:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (MultRxRyRz16:{ *:[i32] }:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MultRxRyRz16,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::HI0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::LO0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1738,
        GIR_Done,
      // Label 126: @2041
      GIM_Reject,
    // Label 121: @2042
    GIM_Reject,
    // Label 114: @2043
    GIM_Try, /*On fail goto*//*Label 127*/ 2103,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 128*/ 2091, // Rule ID 246 //
        GIM_CheckFeatures, GIFBS_HasCnMips,
        // (mul:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DMUL:{ *:[i64] }:{ *:[i32] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMUL,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::HI0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::LO0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::P0,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::P1,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::P2,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 246,
        GIR_Done,
      // Label 128: @2091
      GIM_Try, /*On fail goto*//*Label 129*/ 2102, // Rule ID 319 //
        GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
        // (mul:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DMUL_R6:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMUL_R6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 319,
        GIR_Done,
      // Label 129: @2102
      GIM_Reject,
    // Label 127: @2103
    GIM_Reject,
    // Label 115: @2104
    GIM_Try, /*On fail goto*//*Label 130*/ 2130, // Rule ID 1839 //
      GIM_CheckFeatures, GIFBS_HasDSPR2,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      // (mul:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (MUL_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MUL_PH,
      GIR_AddImplicitDef, /*InsnID*/0, Mips::DSPOutFlag21,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1839,
      GIR_Done,
    // Label 130: @2130
    GIM_Reject,
    // Label 116: @2131
    GIM_Try, /*On fail goto*//*Label 131*/ 2162, // Rule ID 875 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (mul:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (MULV_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MULV_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 875,
      GIR_Done,
    // Label 131: @2162
    GIM_Reject,
    // Label 117: @2163
    GIM_Try, /*On fail goto*//*Label 132*/ 2194, // Rule ID 874 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (mul:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MULV_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MULV_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 874,
      GIR_Done,
    // Label 132: @2194
    GIM_Reject,
    // Label 118: @2195
    GIM_Try, /*On fail goto*//*Label 133*/ 2226, // Rule ID 873 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (mul:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MULV_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MULV_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 873,
      GIR_Done,
    // Label 133: @2226
    GIM_Reject,
    // Label 119: @2227
    GIM_Try, /*On fail goto*//*Label 134*/ 2258, // Rule ID 872 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (mul:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (MULV_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MULV_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 872,
      GIR_Done,
    // Label 134: @2258
    GIM_Reject,
    // Label 120: @2259
    GIM_Reject,
    // Label 3: @2260
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 141*/ 2480,
    /*GILLT_s32*//*Label 135*/ 2274,
    /*GILLT_s64*//*Label 136*/ 2320, 0,
    /*GILLT_v2s64*//*Label 137*/ 2352, 0,
    /*GILLT_v4s32*//*Label 138*/ 2384,
    /*GILLT_v8s16*//*Label 139*/ 2416,
    /*GILLT_v16s8*//*Label 140*/ 2448,
    // Label 135: @2274
    GIM_Try, /*On fail goto*//*Label 142*/ 2319,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 143*/ 2307, // Rule ID 298 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
        // (sdiv:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (DIV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 298,
        GIR_Done,
      // Label 143: @2307
      GIM_Try, /*On fail goto*//*Label 144*/ 2318, // Rule ID 1129 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        // (sdiv:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (DIV_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1129,
        GIR_Done,
      // Label 144: @2318
      GIM_Reject,
    // Label 142: @2319
    GIM_Reject,
    // Label 136: @2320
    GIM_Try, /*On fail goto*//*Label 145*/ 2351, // Rule ID 313 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (sdiv:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DDIV:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DDIV,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 313,
      GIR_Done,
    // Label 145: @2351
    GIM_Reject,
    // Label 137: @2352
    GIM_Try, /*On fail goto*//*Label 146*/ 2383, // Rule ID 615 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (sdiv:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (DIV_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_S_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 615,
      GIR_Done,
    // Label 146: @2383
    GIM_Reject,
    // Label 138: @2384
    GIM_Try, /*On fail goto*//*Label 147*/ 2415, // Rule ID 614 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (sdiv:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DIV_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_S_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 614,
      GIR_Done,
    // Label 147: @2415
    GIM_Reject,
    // Label 139: @2416
    GIM_Try, /*On fail goto*//*Label 148*/ 2447, // Rule ID 613 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (sdiv:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DIV_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_S_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 613,
      GIR_Done,
    // Label 148: @2447
    GIM_Reject,
    // Label 140: @2448
    GIM_Try, /*On fail goto*//*Label 149*/ 2479, // Rule ID 612 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (sdiv:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DIV_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_S_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 612,
      GIR_Done,
    // Label 149: @2479
    GIM_Reject,
    // Label 141: @2480
    GIM_Reject,
    // Label 4: @2481
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 156*/ 2701,
    /*GILLT_s32*//*Label 150*/ 2495,
    /*GILLT_s64*//*Label 151*/ 2541, 0,
    /*GILLT_v2s64*//*Label 152*/ 2573, 0,
    /*GILLT_v4s32*//*Label 153*/ 2605,
    /*GILLT_v8s16*//*Label 154*/ 2637,
    /*GILLT_v16s8*//*Label 155*/ 2669,
    // Label 150: @2495
    GIM_Try, /*On fail goto*//*Label 157*/ 2540,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 158*/ 2528, // Rule ID 299 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
        // (udiv:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (DIVU:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIVU,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 299,
        GIR_Done,
      // Label 158: @2528
      GIM_Try, /*On fail goto*//*Label 159*/ 2539, // Rule ID 1130 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        // (udiv:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (DIVU_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIVU_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1130,
        GIR_Done,
      // Label 159: @2539
      GIM_Reject,
    // Label 157: @2540
    GIM_Reject,
    // Label 151: @2541
    GIM_Try, /*On fail goto*//*Label 160*/ 2572, // Rule ID 314 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (udiv:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DDIVU:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DDIVU,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 314,
      GIR_Done,
    // Label 160: @2572
    GIM_Reject,
    // Label 152: @2573
    GIM_Try, /*On fail goto*//*Label 161*/ 2604, // Rule ID 619 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (udiv:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (DIV_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_U_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 619,
      GIR_Done,
    // Label 161: @2604
    GIM_Reject,
    // Label 153: @2605
    GIM_Try, /*On fail goto*//*Label 162*/ 2636, // Rule ID 618 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (udiv:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DIV_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_U_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 618,
      GIR_Done,
    // Label 162: @2636
    GIM_Reject,
    // Label 154: @2637
    GIM_Try, /*On fail goto*//*Label 163*/ 2668, // Rule ID 617 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (udiv:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DIV_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_U_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 617,
      GIR_Done,
    // Label 163: @2668
    GIM_Reject,
    // Label 155: @2669
    GIM_Try, /*On fail goto*//*Label 164*/ 2700, // Rule ID 616 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (udiv:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DIV_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DIV_U_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 616,
      GIR_Done,
    // Label 164: @2700
    GIM_Reject,
    // Label 156: @2701
    GIM_Reject,
    // Label 5: @2702
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 171*/ 2922,
    /*GILLT_s32*//*Label 165*/ 2716,
    /*GILLT_s64*//*Label 166*/ 2762, 0,
    /*GILLT_v2s64*//*Label 167*/ 2794, 0,
    /*GILLT_v4s32*//*Label 168*/ 2826,
    /*GILLT_v8s16*//*Label 169*/ 2858,
    /*GILLT_v16s8*//*Label 170*/ 2890,
    // Label 165: @2716
    GIM_Try, /*On fail goto*//*Label 172*/ 2761,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 173*/ 2749, // Rule ID 300 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
        // (srem:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MOD:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 300,
        GIR_Done,
      // Label 173: @2749
      GIM_Try, /*On fail goto*//*Label 174*/ 2760, // Rule ID 1134 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        // (srem:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MOD_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1134,
        GIR_Done,
      // Label 174: @2760
      GIM_Reject,
    // Label 172: @2761
    GIM_Reject,
    // Label 166: @2762
    GIM_Try, /*On fail goto*//*Label 175*/ 2793, // Rule ID 315 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (srem:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DMOD:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMOD,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 315,
      GIR_Done,
    // Label 175: @2793
    GIM_Reject,
    // Label 167: @2794
    GIM_Try, /*On fail goto*//*Label 176*/ 2825, // Rule ID 855 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (srem:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (MOD_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_S_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 855,
      GIR_Done,
    // Label 176: @2825
    GIM_Reject,
    // Label 168: @2826
    GIM_Try, /*On fail goto*//*Label 177*/ 2857, // Rule ID 854 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (srem:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MOD_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_S_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 854,
      GIR_Done,
    // Label 177: @2857
    GIM_Reject,
    // Label 169: @2858
    GIM_Try, /*On fail goto*//*Label 178*/ 2889, // Rule ID 853 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (srem:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MOD_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_S_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 853,
      GIR_Done,
    // Label 178: @2889
    GIM_Reject,
    // Label 170: @2890
    GIM_Try, /*On fail goto*//*Label 179*/ 2921, // Rule ID 852 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (srem:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (MOD_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_S_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 852,
      GIR_Done,
    // Label 179: @2921
    GIM_Reject,
    // Label 171: @2922
    GIM_Reject,
    // Label 6: @2923
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 186*/ 3143,
    /*GILLT_s32*//*Label 180*/ 2937,
    /*GILLT_s64*//*Label 181*/ 2983, 0,
    /*GILLT_v2s64*//*Label 182*/ 3015, 0,
    /*GILLT_v4s32*//*Label 183*/ 3047,
    /*GILLT_v8s16*//*Label 184*/ 3079,
    /*GILLT_v16s8*//*Label 185*/ 3111,
    // Label 180: @2937
    GIM_Try, /*On fail goto*//*Label 187*/ 2982,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 188*/ 2970, // Rule ID 301 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
        // (urem:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MODU:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MODU,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 301,
        GIR_Done,
      // Label 188: @2970
      GIM_Try, /*On fail goto*//*Label 189*/ 2981, // Rule ID 1135 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        // (urem:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MODU_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MODU_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1135,
        GIR_Done,
      // Label 189: @2981
      GIM_Reject,
    // Label 187: @2982
    GIM_Reject,
    // Label 181: @2983
    GIM_Try, /*On fail goto*//*Label 190*/ 3014, // Rule ID 316 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (urem:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (DMODU:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMODU,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 316,
      GIR_Done,
    // Label 190: @3014
    GIM_Reject,
    // Label 182: @3015
    GIM_Try, /*On fail goto*//*Label 191*/ 3046, // Rule ID 859 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (urem:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (MOD_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_U_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 859,
      GIR_Done,
    // Label 191: @3046
    GIM_Reject,
    // Label 183: @3047
    GIM_Try, /*On fail goto*//*Label 192*/ 3078, // Rule ID 858 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (urem:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MOD_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_U_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 858,
      GIR_Done,
    // Label 192: @3078
    GIM_Reject,
    // Label 184: @3079
    GIM_Try, /*On fail goto*//*Label 193*/ 3110, // Rule ID 857 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (urem:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MOD_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_U_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 857,
      GIR_Done,
    // Label 193: @3110
    GIM_Reject,
    // Label 185: @3111
    GIM_Try, /*On fail goto*//*Label 194*/ 3142, // Rule ID 856 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (urem:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (MOD_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MOD_U_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 856,
      GIR_Done,
    // Label 194: @3142
    GIM_Reject,
    // Label 186: @3143
    GIM_Reject,
    // Label 7: @3144
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 201*/ 3587,
    /*GILLT_s32*//*Label 195*/ 3158,
    /*GILLT_s64*//*Label 196*/ 3371, 0,
    /*GILLT_v2s64*//*Label 197*/ 3459, 0,
    /*GILLT_v4s32*//*Label 198*/ 3491,
    /*GILLT_v8s16*//*Label 199*/ 3523,
    /*GILLT_v16s8*//*Label 200*/ 3555,
    // Label 195: @3158
    GIM_Try, /*On fail goto*//*Label 202*/ 3370,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 203*/ 3211, // Rule ID 2070 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExtAndi16,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExtAndi16>>:$imm)  =>  (ANDI16_MM:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExtAndi16>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ANDI16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2070,
        GIR_Done,
      // Label 203: @3211
      GIM_Try, /*On fail goto*//*Label 204*/ 3254, // Rule ID 2222 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExtAndi16,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExtAndi16>>:$imm)  =>  (ANDI16_MMR6:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExtAndi16>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ANDI16_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2222,
        GIR_Done,
      // Label 204: @3254
      GIM_Try, /*On fail goto*//*Label 205*/ 3277, // Rule ID 39 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (and:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (AND:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 39,
        GIR_Done,
      // Label 205: @3277
      GIM_Try, /*On fail goto*//*Label 206*/ 3300, // Rule ID 1029 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (and:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (AND16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND16_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1029,
        GIR_Done,
      // Label 206: @3300
      GIM_Try, /*On fail goto*//*Label 207*/ 3323, // Rule ID 1045 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (and:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (AND_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1045,
        GIR_Done,
      // Label 207: @3323
      GIM_Try, /*On fail goto*//*Label 208*/ 3346, // Rule ID 1127 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (and:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (AND_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1127,
        GIR_Done,
      // Label 208: @3346
      GIM_Try, /*On fail goto*//*Label 209*/ 3369, // Rule ID 1737 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (and:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (AndRxRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AndRxRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1737,
        GIR_Done,
      // Label 209: @3369
      GIM_Reject,
    // Label 202: @3370
    GIM_Reject,
    // Label 196: @3371
    GIM_Try, /*On fail goto*//*Label 210*/ 3458,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 211*/ 3438, // Rule ID 241 //
        GIM_CheckFeatures, GIFBS_HasCnMips,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, 255,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (and:{ *:[i64] } (add:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt), 255:{ *:[i64] })  =>  (BADDu:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BADDu,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 241,
        GIR_Done,
      // Label 211: @3438
      GIM_Try, /*On fail goto*//*Label 212*/ 3457, // Rule ID 184 //
        GIM_CheckFeatures, GIFBS_IsGP64bit_NotInMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        // (and:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (AND64:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 184,
        GIR_Done,
      // Label 212: @3457
      GIM_Reject,
    // Label 210: @3458
    GIM_Reject,
    // Label 197: @3459
    GIM_Try, /*On fail goto*//*Label 213*/ 3490, // Rule ID 486 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (and:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (AND_V_D_PSEUDO:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_V_D_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 486,
      GIR_Done,
    // Label 213: @3490
    GIM_Reject,
    // Label 198: @3491
    GIM_Try, /*On fail goto*//*Label 214*/ 3522, // Rule ID 485 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (and:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (AND_V_W_PSEUDO:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_V_W_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 485,
      GIR_Done,
    // Label 214: @3522
    GIM_Reject,
    // Label 199: @3523
    GIM_Try, /*On fail goto*//*Label 215*/ 3554, // Rule ID 484 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (and:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (AND_V_H_PSEUDO:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_V_H_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 484,
      GIR_Done,
    // Label 215: @3554
    GIM_Reject,
    // Label 200: @3555
    GIM_Try, /*On fail goto*//*Label 216*/ 3586, // Rule ID 483 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (and:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (AND_V:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::AND_V,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 483,
      GIR_Done,
    // Label 216: @3586
    GIM_Reject,
    // Label 201: @3587
    GIM_Reject,
    // Label 8: @3588
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 223*/ 3889,
    /*GILLT_s32*//*Label 217*/ 3602,
    /*GILLT_s64*//*Label 218*/ 3729, 0,
    /*GILLT_v2s64*//*Label 219*/ 3761, 0,
    /*GILLT_v4s32*//*Label 220*/ 3793,
    /*GILLT_v8s16*//*Label 221*/ 3825,
    /*GILLT_v16s8*//*Label 222*/ 3857,
    // Label 217: @3602
    GIM_Try, /*On fail goto*//*Label 224*/ 3728,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 225*/ 3635, // Rule ID 40 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (OR:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 40,
        GIR_Done,
      // Label 225: @3635
      GIM_Try, /*On fail goto*//*Label 226*/ 3658, // Rule ID 1031 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (or:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (OR16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR16_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1031,
        GIR_Done,
      // Label 226: @3658
      GIM_Try, /*On fail goto*//*Label 227*/ 3681, // Rule ID 1046 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (OR_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1046,
        GIR_Done,
      // Label 227: @3681
      GIM_Try, /*On fail goto*//*Label 228*/ 3704, // Rule ID 1140 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (OR_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1140,
        GIR_Done,
      // Label 228: @3704
      GIM_Try, /*On fail goto*//*Label 229*/ 3727, // Rule ID 1739 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (or:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (OrRxRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OrRxRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1739,
        GIR_Done,
      // Label 229: @3727
      GIM_Reject,
    // Label 224: @3728
    GIM_Reject,
    // Label 218: @3729
    GIM_Try, /*On fail goto*//*Label 230*/ 3760, // Rule ID 185 //
      GIM_CheckFeatures, GIFBS_IsGP64bit_NotInMips16Mode,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      // (or:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (OR64:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR64,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 185,
      GIR_Done,
    // Label 230: @3760
    GIM_Reject,
    // Label 219: @3761
    GIM_Try, /*On fail goto*//*Label 231*/ 3792, // Rule ID 892 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (or:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (OR_V_D_PSEUDO:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_V_D_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 892,
      GIR_Done,
    // Label 231: @3792
    GIM_Reject,
    // Label 220: @3793
    GIM_Try, /*On fail goto*//*Label 232*/ 3824, // Rule ID 891 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (or:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (OR_V_W_PSEUDO:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_V_W_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 891,
      GIR_Done,
    // Label 232: @3824
    GIM_Reject,
    // Label 221: @3825
    GIM_Try, /*On fail goto*//*Label 233*/ 3856, // Rule ID 890 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (or:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (OR_V_H_PSEUDO:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_V_H_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 890,
      GIR_Done,
    // Label 233: @3856
    GIM_Reject,
    // Label 222: @3857
    GIM_Try, /*On fail goto*//*Label 234*/ 3888, // Rule ID 889 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (or:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (OR_V:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::OR_V,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 889,
      GIR_Done,
    // Label 234: @3888
    GIM_Reject,
    // Label 223: @3889
    GIM_Reject,
    // Label 9: @3890
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 241*/ 4683,
    /*GILLT_s32*//*Label 235*/ 3904,
    /*GILLT_s64*//*Label 236*/ 4467, 0,
    /*GILLT_v2s64*//*Label 237*/ 4555, 0,
    /*GILLT_v4s32*//*Label 238*/ 4587,
    /*GILLT_v8s16*//*Label 239*/ 4619,
    /*GILLT_v16s8*//*Label 240*/ 4651,
    // Label 235: @3904
    GIM_Try, /*On fail goto*//*Label 242*/ 4466,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 243*/ 3971, // Rule ID 42 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_OR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt), -1:{ *:[i32] })  =>  (NOR:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 42,
        GIR_Done,
      // Label 243: @3971
      GIM_Try, /*On fail goto*//*Label 244*/ 4028, // Rule ID 1048 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_OR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt), -1:{ *:[i32] })  =>  (NOR_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1048,
        GIR_Done,
      // Label 244: @4028
      GIM_Try, /*On fail goto*//*Label 245*/ 4085, // Rule ID 1139 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_OR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i32] } (or:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt), -1:{ *:[i32] })  =>  (NOR_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1139,
        GIR_Done,
      // Label 245: @4085
      GIM_Try, /*On fail goto*//*Label 246*/ 4117, // Rule ID 1166 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, -1:{ *:[i32] })  =>  (NOT16_MMR6:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOT16_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1166,
        GIR_Done,
      // Label 246: @4117
      GIM_Try, /*On fail goto*//*Label 247*/ 4149, // Rule ID 1030 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, -1:{ *:[i32] })  =>  (NOT16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOT16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1030,
        GIR_Done,
      // Label 247: @4149
      GIM_Try, /*On fail goto*//*Label 248*/ 4184, // Rule ID 1353 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPR32:{ *:[i32] }:$in, -1:{ *:[i32] })  =>  (NOR:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$in, ZERO:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_AddRegister, /*InsnID*/0, Mips::ZERO,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1353,
        GIR_Done,
      // Label 248: @4184
      GIM_Try, /*On fail goto*//*Label 249*/ 4216, // Rule ID 1734 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, -1:{ *:[i32] })  =>  (NotRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NotRxRy16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // r
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1734,
        GIR_Done,
      // Label 249: @4216
      GIM_Try, /*On fail goto*//*Label 250*/ 4248, // Rule ID 2065 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPRMM16:{ *:[i32] }:$in, -1:{ *:[i32] })  =>  (NOT16_MM:{ *:[i32] } GPRMM16:{ *:[i32] }:$in)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOT16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2065,
        GIR_Done,
      // Label 250: @4248
      GIM_Try, /*On fail goto*//*Label 251*/ 4283, // Rule ID 2066 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPR32:{ *:[i32] }:$in, -1:{ *:[i32] })  =>  (NOR_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$in, ZERO:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_AddRegister, /*InsnID*/0, Mips::ZERO,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2066,
        GIR_Done,
      // Label 251: @4283
      GIM_Try, /*On fail goto*//*Label 252*/ 4315, // Rule ID 2225 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPRMM16:{ *:[i32] }:$in, -1:{ *:[i32] })  =>  (NOT16_MMR6:{ *:[i32] } GPRMM16:{ *:[i32] }:$in)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOT16_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2225,
        GIR_Done,
      // Label 252: @4315
      GIM_Try, /*On fail goto*//*Label 253*/ 4350, // Rule ID 2226 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        // (xor:{ *:[i32] } GPR32:{ *:[i32] }:$in, -1:{ *:[i32] })  =>  (NOR_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$in, ZERO:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_AddRegister, /*InsnID*/0, Mips::ZERO,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2226,
        GIR_Done,
      // Label 253: @4350
      GIM_Try, /*On fail goto*//*Label 254*/ 4373, // Rule ID 41 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (xor:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (XOR:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 41,
        GIR_Done,
      // Label 254: @4373
      GIM_Try, /*On fail goto*//*Label 255*/ 4396, // Rule ID 1033 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPRMM16RegClassID,
        // (xor:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)  =>  (XOR16_MM:{ *:[i32] } GPRMM16Opnd:{ *:[i32] }:$rs, GPRMM16Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR16_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1033,
        GIR_Done,
      // Label 255: @4396
      GIM_Try, /*On fail goto*//*Label 256*/ 4419, // Rule ID 1047 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (xor:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (XOR_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1047,
        GIR_Done,
      // Label 256: @4419
      GIM_Try, /*On fail goto*//*Label 257*/ 4442, // Rule ID 1143 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (xor:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (XOR_MMR6:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1143,
        GIR_Done,
      // Label 257: @4442
      GIM_Try, /*On fail goto*//*Label 258*/ 4465, // Rule ID 1741 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (xor:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)  =>  (XorRxRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$l, CPU16Regs:{ *:[i32] }:$r)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XorRxRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1741,
        GIR_Done,
      // Label 258: @4465
      GIM_Reject,
    // Label 242: @4466
    GIM_Reject,
    // Label 236: @4467
    GIM_Try, /*On fail goto*//*Label 259*/ 4554,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 260*/ 4534, // Rule ID 187 //
        GIM_CheckFeatures, GIFBS_IsGP64bit_NotInMips16Mode,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_OR,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckConstantInt, /*MI*/0, /*Op*/2, -1,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (xor:{ *:[i64] } (or:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt), -1:{ *:[i64] })  =>  (NOR64:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NOR64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 187,
        GIR_Done,
      // Label 260: @4534
      GIM_Try, /*On fail goto*//*Label 261*/ 4553, // Rule ID 186 //
        GIM_CheckFeatures, GIFBS_IsGP64bit_NotInMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
        // (xor:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)  =>  (XOR64:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rs, GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 186,
        GIR_Done,
      // Label 261: @4553
      GIM_Reject,
    // Label 259: @4554
    GIM_Reject,
    // Label 237: @4555
    GIM_Try, /*On fail goto*//*Label 262*/ 4586, // Rule ID 1008 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (xor:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (XOR_V_D_PSEUDO:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_V_D_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1008,
      GIR_Done,
    // Label 262: @4586
    GIM_Reject,
    // Label 238: @4587
    GIM_Try, /*On fail goto*//*Label 263*/ 4618, // Rule ID 1007 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (xor:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (XOR_V_W_PSEUDO:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_V_W_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1007,
      GIR_Done,
    // Label 263: @4618
    GIM_Reject,
    // Label 239: @4619
    GIM_Try, /*On fail goto*//*Label 264*/ 4650, // Rule ID 1006 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (xor:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (XOR_V_H_PSEUDO:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_V_H_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1006,
      GIR_Done,
    // Label 264: @4650
    GIM_Reject,
    // Label 240: @4651
    GIM_Try, /*On fail goto*//*Label 265*/ 4682, // Rule ID 1005 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (xor:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (XOR_V:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::XOR_V,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1005,
      GIR_Done,
    // Label 265: @4682
    GIM_Reject,
    // Label 241: @4683
    GIM_Reject,
    // Label 10: @4684
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 274*/ 8336,
    /*GILLT_s32*//*Label 266*/ 4698,
    /*GILLT_s64*//*Label 267*/ 4937,
    /*GILLT_v2s16*//*Label 268*/ 4983,
    /*GILLT_v2s64*//*Label 269*/ 5029,
    /*GILLT_v4s8*//*Label 270*/ 6002,
    /*GILLT_v4s32*//*Label 271*/ 6048,
    /*GILLT_v8s16*//*Label 272*/ 6951,
    /*GILLT_v16s8*//*Label 273*/ 7749,
    // Label 266: @4698
    GIM_Try, /*On fail goto*//*Label 275*/ 4721, // Rule ID 117 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (bitconvert:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (MFC1:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MFC1,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 117,
      GIR_Done,
    // Label 275: @4721
    GIM_Try, /*On fail goto*//*Label 276*/ 4744, // Rule ID 118 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (bitconvert:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)  =>  (MTC1:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MTC1,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 118,
      GIR_Done,
    // Label 276: @4744
    GIM_Try, /*On fail goto*//*Label 277*/ 4767, // Rule ID 1119 //
      GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (bitconvert:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (MFC1_MM:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MFC1_MM,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1119,
      GIR_Done,
    // Label 277: @4767
    GIM_Try, /*On fail goto*//*Label 278*/ 4790, // Rule ID 1120 //
      GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (bitconvert:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)  =>  (MTC1_MM:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MTC1_MM,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1120,
      GIR_Done,
    // Label 278: @4790
    GIM_Try, /*On fail goto*//*Label 279*/ 4813, // Rule ID 1132 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (bitconvert:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)  =>  (MTC1_MMR6:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$rt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MTC1_MMR6,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1132,
      GIR_Done,
    // Label 279: @4813
    GIM_Try, /*On fail goto*//*Label 280*/ 4836, // Rule ID 1133 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (bitconvert:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (MFC1_MMR6:{ *:[i32] } FGR32Opnd:{ *:[f32] }:$fs)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MFC1_MMR6,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1133,
      GIR_Done,
    // Label 280: @4836
    GIM_Try, /*On fail goto*//*Label 281*/ 4861, // Rule ID 1822 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
      // (bitconvert:{ *:[i32] } DSPR:{ *:[v2i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[i32] } DSPR:{ *:[v2i16] }:$src, GPR32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GPR32*/11,
      // GIR_Coverage, 1822,
      GIR_Done,
    // Label 281: @4861
    GIM_Try, /*On fail goto*//*Label 282*/ 4886, // Rule ID 1823 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
      // (bitconvert:{ *:[i32] } DSPR:{ *:[v4i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[i32] } DSPR:{ *:[v4i8] }:$src, GPR32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC GPR32*/11,
      // GIR_Coverage, 1823,
      GIR_Done,
    // Label 282: @4886
    GIM_Try, /*On fail goto*//*Label 283*/ 4911, // Rule ID 1826 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
      // (bitconvert:{ *:[f32] } DSPR:{ *:[v2i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f32] } DSPR:{ *:[v2i16] }:$src, FGR32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC FGR32*/8,
      // GIR_Coverage, 1826,
      GIR_Done,
    // Label 283: @4911
    GIM_Try, /*On fail goto*//*Label 284*/ 4936, // Rule ID 1827 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
      // (bitconvert:{ *:[f32] } DSPR:{ *:[v4i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[f32] } DSPR:{ *:[v4i8] }:$src, FGR32:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC FGR32*/8,
      // GIR_Coverage, 1827,
      GIR_Done,
    // Label 284: @4936
    GIM_Reject,
    // Label 267: @4937
    GIM_Try, /*On fail goto*//*Label 285*/ 4982,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 286*/ 4962, // Rule ID 119 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        // (bitconvert:{ *:[f64] } GPR64Opnd:{ *:[i64] }:$rt)  =>  (DMTC1:{ *:[f64] } GPR64Opnd:{ *:[i64] }:$rt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMTC1,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 119,
        GIR_Done,
      // Label 286: @4962
      GIM_Try, /*On fail goto*//*Label 287*/ 4981, // Rule ID 120 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        // (bitconvert:{ *:[i64] } FGR64Opnd:{ *:[f64] }:$fs)  =>  (DMFC1:{ *:[i64] } FGR64Opnd:{ *:[f64] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DMFC1,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 120,
        GIR_Done,
      // Label 287: @4981
      GIM_Reject,
    // Label 285: @4982
    GIM_Reject,
    // Label 268: @4983
    GIM_Try, /*On fail goto*//*Label 288*/ 5028,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      GIM_Try, /*On fail goto*//*Label 289*/ 5010, // Rule ID 1824 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        // (bitconvert:{ *:[v2i16] } GPR32:{ *:[i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i16] } GPR32:{ *:[i32] }:$src, DSPR:{ *:[i32] })
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC DSPR*/7,
        // GIR_Coverage, 1824,
        GIR_Done,
      // Label 289: @5010
      GIM_Try, /*On fail goto*//*Label 290*/ 5027, // Rule ID 1828 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        // (bitconvert:{ *:[v2i16] } FGR32:{ *:[f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i16] } FGR32:{ *:[f32] }:$src, DSPR:{ *:[i32] })
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC DSPR*/7,
        // GIR_Coverage, 1828,
        GIR_Done,
      // Label 290: @5027
      GIM_Reject,
    // Label 288: @5028
    GIM_Reject,
    // Label 269: @5029
    GIM_Try, /*On fail goto*//*Label 291*/ 5050, // Rule ID 1909 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v2f64:{ *:[v2f64] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1909,
      GIR_Done,
    // Label 291: @5050
    GIM_Try, /*On fail goto*//*Label 292*/ 5071, // Rule ID 1912 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v2i64:{ *:[v2i64] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1912,
      GIR_Done,
    // Label 292: @5071
    GIM_Try, /*On fail goto*//*Label 293*/ 5092, // Rule ID 1929 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v16i8:{ *:[v16i8] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1929,
      GIR_Done,
    // Label 293: @5092
    GIM_Try, /*On fail goto*//*Label 294*/ 5113, // Rule ID 1930 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v8i16:{ *:[v8i16] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1930,
      GIR_Done,
    // Label 294: @5113
    GIM_Try, /*On fail goto*//*Label 295*/ 5134, // Rule ID 1931 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v4i32:{ *:[v4i32] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1931,
      GIR_Done,
    // Label 295: @5134
    GIM_Try, /*On fail goto*//*Label 296*/ 5155, // Rule ID 1932 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v8f16:{ *:[v8f16] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1932,
      GIR_Done,
    // Label 296: @5155
    GIM_Try, /*On fail goto*//*Label 297*/ 5176, // Rule ID 1933 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } v4f32:{ *:[v4f32] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1933,
      GIR_Done,
    // Label 297: @5176
    GIM_Try, /*On fail goto*//*Label 298*/ 5197, // Rule ID 1939 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v16i8:{ *:[v16i8] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1939,
      GIR_Done,
    // Label 298: @5197
    GIM_Try, /*On fail goto*//*Label 299*/ 5218, // Rule ID 1940 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v8i16:{ *:[v8i16] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1940,
      GIR_Done,
    // Label 299: @5218
    GIM_Try, /*On fail goto*//*Label 300*/ 5239, // Rule ID 1941 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v4i32:{ *:[v4i32] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1941,
      GIR_Done,
    // Label 300: @5239
    GIM_Try, /*On fail goto*//*Label 301*/ 5260, // Rule ID 1942 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v8f16:{ *:[v8f16] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1942,
      GIR_Done,
    // Label 301: @5260
    GIM_Try, /*On fail goto*//*Label 302*/ 5281, // Rule ID 1943 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } v4f32:{ *:[v4f32] }:$src, MSA128D:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1943,
      GIR_Done,
    // Label 302: @5281
    GIM_Try, /*On fail goto*//*Label 303*/ 5381, // Rule ID 1948 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/3, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/4, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/4, /*TempRegID*/3, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/4, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/4,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/3, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/3, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1948,
      GIR_Done,
    // Label 303: @5381
    GIM_Try, /*On fail goto*//*Label 304*/ 5481, // Rule ID 1949 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/3, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/4, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/4, /*TempRegID*/3, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/4, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/4,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/3, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/3, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1949,
      GIR_Done,
    // Label 304: @5481
    GIM_Try, /*On fail goto*//*Label 305*/ 5546, // Rule ID 1953 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8i16:{ *:[v8i16] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1953,
      GIR_Done,
    // Label 305: @5546
    GIM_Try, /*On fail goto*//*Label 306*/ 5611, // Rule ID 1954 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8i16:{ *:[v8i16] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1954,
      GIR_Done,
    // Label 306: @5611
    GIM_Try, /*On fail goto*//*Label 307*/ 5676, // Rule ID 1958 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1958,
      GIR_Done,
    // Label 307: @5676
    GIM_Try, /*On fail goto*//*Label 308*/ 5741, // Rule ID 1959 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1959,
      GIR_Done,
    // Label 308: @5741
    GIM_Try, /*On fail goto*//*Label 309*/ 5806, // Rule ID 1963 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v4i32:{ *:[v4i32] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1963,
      GIR_Done,
    // Label 309: @5806
    GIM_Try, /*On fail goto*//*Label 310*/ 5871, // Rule ID 1964 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v4i32:{ *:[v4i32] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1964,
      GIR_Done,
    // Label 310: @5871
    GIM_Try, /*On fail goto*//*Label 311*/ 5936, // Rule ID 1968 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2i64] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2i64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v4f32:{ *:[v4f32] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1968,
      GIR_Done,
    // Label 311: @5936
    GIM_Try, /*On fail goto*//*Label 312*/ 6001, // Rule ID 1969 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      // (bitconvert:{ *:[v2f64] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v2f64] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v4f32:{ *:[v4f32] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128D:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128D*/67,
      // GIR_Coverage, 1969,
      GIR_Done,
    // Label 312: @6001
    GIM_Reject,
    // Label 270: @6002
    GIM_Try, /*On fail goto*//*Label 313*/ 6047,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
      GIM_Try, /*On fail goto*//*Label 314*/ 6029, // Rule ID 1825 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        // (bitconvert:{ *:[v4i8] } GPR32:{ *:[i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i8] } GPR32:{ *:[i32] }:$src, DSPR:{ *:[i32] })
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC DSPR*/7,
        // GIR_Coverage, 1825,
        GIR_Done,
      // Label 314: @6029
      GIM_Try, /*On fail goto*//*Label 315*/ 6046, // Rule ID 1829 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        // (bitconvert:{ *:[v4i8] } FGR32:{ *:[f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i8] } FGR32:{ *:[f32] }:$src, DSPR:{ *:[i32] })
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
        GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC DSPR*/7,
        // GIR_Coverage, 1829,
        GIR_Done,
      // Label 315: @6046
      GIM_Reject,
    // Label 313: @6047
    GIM_Reject,
    // Label 271: @6048
    GIM_Try, /*On fail goto*//*Label 316*/ 6069, // Rule ID 1908 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v4f32:{ *:[v4f32] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1908,
      GIR_Done,
    // Label 316: @6069
    GIM_Try, /*On fail goto*//*Label 317*/ 6090, // Rule ID 1911 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v4i32:{ *:[v4i32] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1911,
      GIR_Done,
    // Label 317: @6090
    GIM_Try, /*On fail goto*//*Label 318*/ 6111, // Rule ID 1924 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v16i8:{ *:[v16i8] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1924,
      GIR_Done,
    // Label 318: @6111
    GIM_Try, /*On fail goto*//*Label 319*/ 6132, // Rule ID 1925 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v8i16:{ *:[v8i16] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1925,
      GIR_Done,
    // Label 319: @6132
    GIM_Try, /*On fail goto*//*Label 320*/ 6153, // Rule ID 1926 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v2i64:{ *:[v2i64] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1926,
      GIR_Done,
    // Label 320: @6153
    GIM_Try, /*On fail goto*//*Label 321*/ 6174, // Rule ID 1927 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v8f16:{ *:[v8f16] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1927,
      GIR_Done,
    // Label 321: @6174
    GIM_Try, /*On fail goto*//*Label 322*/ 6195, // Rule ID 1928 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } v2f64:{ *:[v2f64] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1928,
      GIR_Done,
    // Label 322: @6195
    GIM_Try, /*On fail goto*//*Label 323*/ 6216, // Rule ID 1934 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v16i8:{ *:[v16i8] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1934,
      GIR_Done,
    // Label 323: @6216
    GIM_Try, /*On fail goto*//*Label 324*/ 6237, // Rule ID 1935 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v8i16:{ *:[v8i16] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1935,
      GIR_Done,
    // Label 324: @6237
    GIM_Try, /*On fail goto*//*Label 325*/ 6258, // Rule ID 1936 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v2i64:{ *:[v2i64] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1936,
      GIR_Done,
    // Label 325: @6258
    GIM_Try, /*On fail goto*//*Label 326*/ 6279, // Rule ID 1937 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v8f16:{ *:[v8f16] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1937,
      GIR_Done,
    // Label 326: @6279
    GIM_Try, /*On fail goto*//*Label 327*/ 6300, // Rule ID 1938 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } v2f64:{ *:[v2f64] }:$src, MSA128W:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1938,
      GIR_Done,
    // Label 327: @6300
    GIM_Try, /*On fail goto*//*Label 328*/ 6365, // Rule ID 1946 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1946,
      GIR_Done,
    // Label 328: @6365
    GIM_Try, /*On fail goto*//*Label 329*/ 6430, // Rule ID 1947 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1947,
      GIR_Done,
    // Label 329: @6430
    GIM_Try, /*On fail goto*//*Label 330*/ 6495, // Rule ID 1951 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8i16:{ *:[v8i16] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1951,
      GIR_Done,
    // Label 330: @6495
    GIM_Try, /*On fail goto*//*Label 331*/ 6560, // Rule ID 1952 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8i16:{ *:[v8i16] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1952,
      GIR_Done,
    // Label 331: @6560
    GIM_Try, /*On fail goto*//*Label 332*/ 6625, // Rule ID 1956 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1956,
      GIR_Done,
    // Label 332: @6625
    GIM_Try, /*On fail goto*//*Label 333*/ 6690, // Rule ID 1957 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1957,
      GIR_Done,
    // Label 333: @6690
    GIM_Try, /*On fail goto*//*Label 334*/ 6755, // Rule ID 1973 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v2i64:{ *:[v2i64] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1973,
      GIR_Done,
    // Label 334: @6755
    GIM_Try, /*On fail goto*//*Label 335*/ 6820, // Rule ID 1974 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v2i64:{ *:[v2i64] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1974,
      GIR_Done,
    // Label 335: @6820
    GIM_Try, /*On fail goto*//*Label 336*/ 6885, // Rule ID 1978 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4i32] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v2f64:{ *:[v2f64] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1978,
      GIR_Done,
    // Label 336: @6885
    GIM_Try, /*On fail goto*//*Label 337*/ 6950, // Rule ID 1979 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      // (bitconvert:{ *:[v4f32] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v4f32] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } v2f64:{ *:[v2f64] }:$src, MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128W:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128W*/69,
      // GIR_Coverage, 1979,
      GIR_Done,
    // Label 337: @6950
    GIM_Reject,
    // Label 272: @6951
    GIM_Try, /*On fail goto*//*Label 338*/ 6972, // Rule ID 1907 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v8f16:{ *:[v8f16] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1907,
      GIR_Done,
    // Label 338: @6972
    GIM_Try, /*On fail goto*//*Label 339*/ 6993, // Rule ID 1910 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } v8i16:{ *:[v8i16] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1910,
      GIR_Done,
    // Label 339: @6993
    GIM_Try, /*On fail goto*//*Label 340*/ 7014, // Rule ID 1919 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v16i8:{ *:[v16i8] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1919,
      GIR_Done,
    // Label 340: @7014
    GIM_Try, /*On fail goto*//*Label 341*/ 7035, // Rule ID 1920 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v4i32:{ *:[v4i32] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1920,
      GIR_Done,
    // Label 341: @7035
    GIM_Try, /*On fail goto*//*Label 342*/ 7056, // Rule ID 1921 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v2i64:{ *:[v2i64] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1921,
      GIR_Done,
    // Label 342: @7056
    GIM_Try, /*On fail goto*//*Label 343*/ 7077, // Rule ID 1922 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v4f32:{ *:[v4f32] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1922,
      GIR_Done,
    // Label 343: @7077
    GIM_Try, /*On fail goto*//*Label 344*/ 7098, // Rule ID 1923 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } v2f64:{ *:[v2f64] }:$src, MSA128H:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1923,
      GIR_Done,
    // Label 344: @7098
    GIM_Try, /*On fail goto*//*Label 345*/ 7163, // Rule ID 1944 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1944,
      GIR_Done,
    // Label 345: @7163
    GIM_Try, /*On fail goto*//*Label 346*/ 7228, // Rule ID 1945 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v16i8:{ *:[v16i8] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v16i8:{ *:[v16i8] }:$src, MSA128B:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1945,
      GIR_Done,
    // Label 346: @7228
    GIM_Try, /*On fail goto*//*Label 347*/ 7293, // Rule ID 1961 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v4i32:{ *:[v4i32] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1961,
      GIR_Done,
    // Label 347: @7293
    GIM_Try, /*On fail goto*//*Label 348*/ 7358, // Rule ID 1962 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v4i32:{ *:[v4i32] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1962,
      GIR_Done,
    // Label 348: @7358
    GIM_Try, /*On fail goto*//*Label 349*/ 7423, // Rule ID 1966 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v4f32:{ *:[v4f32] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1966,
      GIR_Done,
    // Label 349: @7423
    GIM_Try, /*On fail goto*//*Label 350*/ 7488, // Rule ID 1967 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v4f32:{ *:[v4f32] }:$src, MSA128H:{ *:[i32] }), 177:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1967,
      GIR_Done,
    // Label 350: @7488
    GIM_Try, /*On fail goto*//*Label 351*/ 7553, // Rule ID 1971 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v2i64:{ *:[v2i64] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1971,
      GIR_Done,
    // Label 351: @7553
    GIM_Try, /*On fail goto*//*Label 352*/ 7618, // Rule ID 1972 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v2i64:{ *:[v2i64] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1972,
      GIR_Done,
    // Label 352: @7618
    GIM_Try, /*On fail goto*//*Label 353*/ 7683, // Rule ID 1976 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8i16] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8i16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v2f64:{ *:[v2f64] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1976,
      GIR_Done,
    // Label 353: @7683
    GIM_Try, /*On fail goto*//*Label 354*/ 7748, // Rule ID 1977 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      // (bitconvert:{ *:[v8f16] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v8f16] } (SHF_H:{ *:[v8i16] } (COPY_TO_REGCLASS:{ *:[v8i16] } v2f64:{ *:[v2f64] }:$src, MSA128H:{ *:[i32] }), 27:{ *:[i32] }), MSA128H:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v8s16,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v8s16,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_H,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128H*/68,
      // GIR_Coverage, 1977,
      GIR_Done,
    // Label 354: @7748
    GIM_Reject,
    // Label 273: @7749
    GIM_Try, /*On fail goto*//*Label 355*/ 7770, // Rule ID 1913 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v8i16:{ *:[v8i16] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1913,
      GIR_Done,
    // Label 355: @7770
    GIM_Try, /*On fail goto*//*Label 356*/ 7791, // Rule ID 1914 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v4i32:{ *:[v4i32] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1914,
      GIR_Done,
    // Label 356: @7791
    GIM_Try, /*On fail goto*//*Label 357*/ 7812, // Rule ID 1915 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v2i64:{ *:[v2i64] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1915,
      GIR_Done,
    // Label 357: @7812
    GIM_Try, /*On fail goto*//*Label 358*/ 7833, // Rule ID 1916 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v8f16:{ *:[v8f16] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1916,
      GIR_Done,
    // Label 358: @7833
    GIM_Try, /*On fail goto*//*Label 359*/ 7854, // Rule ID 1917 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v4f32:{ *:[v4f32] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1917,
      GIR_Done,
    // Label 359: @7854
    GIM_Try, /*On fail goto*//*Label 360*/ 7875, // Rule ID 1918 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsLE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } v2f64:{ *:[v2f64] }:$src, MSA128B:{ *:[i32] })
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1918,
      GIR_Done,
    // Label 360: @7875
    GIM_Try, /*On fail goto*//*Label 361*/ 7940, // Rule ID 1950 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v8i16:{ *:[v8i16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v8i16:{ *:[v8i16] }:$src, MSA128B:{ *:[i32] }), 177:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1950,
      GIR_Done,
    // Label 361: @7940
    GIM_Try, /*On fail goto*//*Label 362*/ 8005, // Rule ID 1955 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v8f16:{ *:[v8f16] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v8f16:{ *:[v8f16] }:$src, MSA128B:{ *:[i32] }), 177:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1955,
      GIR_Done,
    // Label 362: @8005
    GIM_Try, /*On fail goto*//*Label 363*/ 8070, // Rule ID 1960 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v4i32:{ *:[v4i32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v4i32:{ *:[v4i32] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1960,
      GIR_Done,
    // Label 363: @8070
    GIM_Try, /*On fail goto*//*Label 364*/ 8135, // Rule ID 1965 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v4f32:{ *:[v4f32] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v4f32:{ *:[v4f32] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1965,
      GIR_Done,
    // Label 364: @8135
    GIM_Try, /*On fail goto*//*Label 365*/ 8235, // Rule ID 1970 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v2i64:{ *:[v2i64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v2i64:{ *:[v2i64] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/3, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/4, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/4, /*TempRegID*/3, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/4, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/4,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/3, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/3, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1970,
      GIR_Done,
    // Label 365: @8235
    GIM_Try, /*On fail goto*//*Label 366*/ 8335, // Rule ID 1975 //
      GIM_CheckFeatures, GIFBS_HasMSA_IsBE,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      // (bitconvert:{ *:[v16i8] } v2f64:{ *:[v2f64] }:$src)  =>  (COPY_TO_REGCLASS:{ *:[v16i8] } (SHF_W:{ *:[v4i32] } (COPY_TO_REGCLASS:{ *:[v4i32] } (SHF_B:{ *:[v16i8] } (COPY_TO_REGCLASS:{ *:[v16i8] } v2f64:{ *:[v2f64] }:$src, MSA128B:{ *:[i32] }), 27:{ *:[i32] }), MSA128W:{ *:[i32] }), 177:{ *:[i32] }), MSA128B:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_v4s32,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_v16s8,
      GIR_MakeTempReg, /*TempRegID*/3, /*TypeID*/GILLT_v16s8,
      GIR_BuildMI, /*InsnID*/4, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/4, /*TempRegID*/3, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/4, /*OldInsnID*/0, /*OpIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/4,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/Mips::SHF_B,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/3, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/3, /*Imm*/27,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SHF_W,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/1, /*Imm*/177,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/TargetOpcode::COPY,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainOperandRC, /*InsnID*/0, /*Op*/0, /*RC MSA128B*/66,
      // GIR_Coverage, 1975,
      GIR_Done,
    // Label 366: @8335
    GIM_Reject,
    // Label 274: @8336
    GIM_Reject,
    // Label 11: @8337
    GIM_Try, /*On fail goto*//*Label 367*/ 8402, // Rule ID 1898 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_CheckMemorySizeEqualToLLT, /*MI*/0, /*MMO*/0, /*OpIdx*/0,
      GIM_CheckAtomicOrdering, /*MI*/0, /*Order*/(int64_t)AtomicOrdering::NotAtomic,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckPointerToAny, /*MI*/0, /*Op*/1, /*SizeInBits*/32,
      GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
      GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
      GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckIsSafeToFold, /*InsnID*/1,
      // (ld:{ *:[i32] } (add:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index))<<P:Predicate_unindexedload>><<P:Predicate_load>>  =>  (LWX:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LWX,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // base
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // index
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1898,
      GIR_Done,
    // Label 367: @8402
    GIM_Reject,
    // Label 12: @8403
    GIM_Try, /*On fail goto*//*Label 368*/ 8468, // Rule ID 1897 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_CheckMemorySizeEqualTo, /*MI*/0, /*MMO*/0, /*Size*/2,
      GIM_CheckAtomicOrdering, /*MI*/0, /*Order*/(int64_t)AtomicOrdering::NotAtomic,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckPointerToAny, /*MI*/0, /*Op*/1, /*SizeInBits*/32,
      GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
      GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
      GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckIsSafeToFold, /*InsnID*/1,
      // (ld:{ *:[i32] } (add:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index))<<P:Predicate_unindexedload>><<P:Predicate_sextload>><<P:Predicate_sextloadi16>>  =>  (LHX:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LHX,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // base
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // index
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1897,
      GIR_Done,
    // Label 368: @8468
    GIM_Reject,
    // Label 13: @8469
    GIM_Try, /*On fail goto*//*Label 369*/ 8534, // Rule ID 1896 //
      GIM_CheckFeatures, GIFBS_HasDSP,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_CheckMemorySizeEqualTo, /*MI*/0, /*MMO*/0, /*Size*/1,
      GIM_CheckAtomicOrdering, /*MI*/0, /*Order*/(int64_t)AtomicOrdering::NotAtomic,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // MIs[0] Operand 1
      GIM_CheckPointerToAny, /*MI*/0, /*Op*/1, /*SizeInBits*/32,
      GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
      GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_ADD,
      GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckIsSafeToFold, /*InsnID*/1,
      // (ld:{ *:[i32] } (add:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index))<<P:Predicate_unindexedload>><<P:Predicate_zextload>><<P:Predicate_zextloadi8>>  =>  (LBUX:{ *:[i32] } i32:{ *:[i32] }:$base, i32:{ *:[i32] }:$index)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LBUX,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // base
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // index
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1896,
      GIR_Done,
    // Label 369: @8534
    GIM_Reject,
    // Label 14: @8535
    GIM_Try, /*On fail goto*//*Label 370*/ 10729,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
      GIM_Try, /*On fail goto*//*Label 371*/ 8587, // Rule ID 400 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3471:{ *:[iPTR] }, (imm:{ *:[i32] })<<P:Predicate_immZExt8>>:$imm)  =>  (REPL_QB:{ *:[v4i8] } (imm:{ *:[i32] }):$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPL_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 400,
        GIR_Done,
      // Label 371: @8587
      GIM_Try, /*On fail goto*//*Label 372*/ 8634, // Rule ID 401 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immSExt10,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3470:{ *:[iPTR] }, (imm:{ *:[i32] })<<P:Predicate_immSExt10>>:$imm)  =>  (REPL_PH:{ *:[v2i16] } (imm:{ *:[i32] }):$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPL_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 401,
        GIR_Done,
      // Label 372: @8634
      GIM_Try, /*On fail goto*//*Label 373*/ 8681, // Rule ID 1245 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immSExt10,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3470:{ *:[iPTR] }, (imm:{ *:[i32] })<<P:Predicate_immSExt10>>:$imm)  =>  (REPL_PH_MM:{ *:[v2i16] } (imm:{ *:[i32] }):$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPL_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1245,
        GIR_Done,
      // Label 373: @8681
      GIM_Try, /*On fail goto*//*Label 374*/ 8728, // Rule ID 1246 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt8,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3471:{ *:[iPTR] }, (imm:{ *:[i32] })<<P:Predicate_immZExt8>>:$imm)  =>  (REPL_QB_MM:{ *:[v4i8] } (imm:{ *:[i32] }):$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPL_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1246,
        GIR_Done,
      // Label 374: @8728
      GIM_Try, /*On fail goto*//*Label 375*/ 8768, // Rule ID 334 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_raddu_w_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3468:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (RADDU_W_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::RADDU_W_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 334,
        GIR_Done,
      // Label 375: @8768
      GIM_Try, /*On fail goto*//*Label 376*/ 8808, // Rule ID 341 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceq_w_phl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3450:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECEQ_W_PHL:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQ_W_PHL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 341,
        GIR_Done,
      // Label 376: @8808
      GIM_Try, /*On fail goto*//*Label 377*/ 8848, // Rule ID 342 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceq_w_phr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3451:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECEQ_W_PHR:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQ_W_PHR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 342,
        GIR_Done,
      // Label 377: @8848
      GIM_Try, /*On fail goto*//*Label 378*/ 8888, // Rule ID 343 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3452:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEQU_PH_QBL:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 343,
        GIR_Done,
      // Label 378: @8888
      GIM_Try, /*On fail goto*//*Label 379*/ 8928, // Rule ID 344 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3454:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEQU_PH_QBR:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 344,
        GIR_Done,
      // Label 379: @8928
      GIM_Try, /*On fail goto*//*Label 380*/ 8968, // Rule ID 345 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbla,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3453:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEQU_PH_QBLA:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBLA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 345,
        GIR_Done,
      // Label 380: @8968
      GIM_Try, /*On fail goto*//*Label 381*/ 9008, // Rule ID 346 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbra,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3455:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEQU_PH_QBRA:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBRA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 346,
        GIR_Done,
      // Label 381: @9008
      GIM_Try, /*On fail goto*//*Label 382*/ 9048, // Rule ID 347 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3456:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEU_PH_QBL:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 347,
        GIR_Done,
      // Label 382: @9048
      GIM_Try, /*On fail goto*//*Label 383*/ 9088, // Rule ID 348 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3458:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEU_PH_QBR:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 348,
        GIR_Done,
      // Label 383: @9088
      GIM_Try, /*On fail goto*//*Label 384*/ 9128, // Rule ID 349 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbla,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3457:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEU_PH_QBLA:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBLA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 349,
        GIR_Done,
      // Label 384: @9128
      GIM_Try, /*On fail goto*//*Label 385*/ 9168, // Rule ID 350 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbra,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3459:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PRECEU_PH_QBRA:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBRA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 350,
        GIR_Done,
      // Label 385: @9168
      GIM_Try, /*On fail goto*//*Label 386*/ 9208, // Rule ID 398 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_bitrev,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3026:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt)  =>  (BITREV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BITREV,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 398,
        GIR_Done,
      // Label 386: @9208
      GIM_Try, /*On fail goto*//*Label 387*/ 9248, // Rule ID 402 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3471:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt)  =>  (REPLV_QB:{ *:[v4i8] } GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPLV_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 402,
        GIR_Done,
      // Label 387: @9248
      GIM_Try, /*On fail goto*//*Label 388*/ 9288, // Rule ID 403 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3470:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt)  =>  (REPLV_PH:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPLV_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 403,
        GIR_Done,
      // Label 388: @9288
      GIM_Try, /*On fail goto*//*Label 389*/ 9328, // Rule ID 648 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fclass_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3178:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FCLASS_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FCLASS_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 648,
        GIR_Done,
      // Label 389: @9328
      GIM_Try, /*On fail goto*//*Label 390*/ 9368, // Rule ID 649 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fclass_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3177:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FCLASS_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FCLASS_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 649,
        GIR_Done,
      // Label 390: @9368
      GIM_Try, /*On fail goto*//*Label 391*/ 9408, // Rule ID 672 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexupl_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3204:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8f16] }:$ws)  =>  (FEXUPL_W:{ *:[v4f32] } MSA128HOpnd:{ *:[v8f16] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXUPL_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 672,
        GIR_Done,
      // Label 391: @9408
      GIM_Try, /*On fail goto*//*Label 392*/ 9448, // Rule ID 673 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexupl_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3203:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FEXUPL_D:{ *:[v2f64] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXUPL_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 673,
        GIR_Done,
      // Label 392: @9448
      GIM_Try, /*On fail goto*//*Label 393*/ 9488, // Rule ID 674 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexupr_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3206:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8f16] }:$ws)  =>  (FEXUPR_W:{ *:[v4f32] } MSA128HOpnd:{ *:[v8f16] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXUPR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 674,
        GIR_Done,
      // Label 393: @9488
      GIM_Try, /*On fail goto*//*Label 394*/ 9528, // Rule ID 675 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexupr_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3205:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FEXUPR_D:{ *:[v2f64] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXUPR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 675,
        GIR_Done,
      // Label 394: @9528
      GIM_Try, /*On fail goto*//*Label 395*/ 9568, // Rule ID 680 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ffql_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3212:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws)  =>  (FFQL_W:{ *:[v4f32] } MSA128HOpnd:{ *:[v8i16] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FFQL_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 680,
        GIR_Done,
      // Label 395: @9568
      GIM_Try, /*On fail goto*//*Label 396*/ 9608, // Rule ID 681 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ffql_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3211:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws)  =>  (FFQL_D:{ *:[v2f64] } MSA128WOpnd:{ *:[v4i32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FFQL_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 681,
        GIR_Done,
      // Label 396: @9608
      GIM_Try, /*On fail goto*//*Label 397*/ 9648, // Rule ID 682 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ffqr_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3214:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws)  =>  (FFQR_W:{ *:[v4f32] } MSA128HOpnd:{ *:[v8i16] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FFQR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 682,
        GIR_Done,
      // Label 397: @9648
      GIM_Try, /*On fail goto*//*Label 398*/ 9688, // Rule ID 683 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ffqr_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3213:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws)  =>  (FFQR_D:{ *:[v2f64] } MSA128WOpnd:{ *:[v4i32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FFQR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 683,
        GIR_Done,
      // Label 398: @9688
      GIM_Try, /*On fail goto*//*Label 399*/ 9728, // Rule ID 708 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_frcp_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3236:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FRCP_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FRCP_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 708,
        GIR_Done,
      // Label 399: @9728
      GIM_Try, /*On fail goto*//*Label 400*/ 9768, // Rule ID 709 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_frcp_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3235:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FRCP_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FRCP_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 709,
        GIR_Done,
      // Label 400: @9768
      GIM_Try, /*On fail goto*//*Label 401*/ 9808, // Rule ID 710 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_frsqrt_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3240:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FRSQRT_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FRSQRT_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 710,
        GIR_Done,
      // Label 401: @9808
      GIM_Try, /*On fail goto*//*Label 402*/ 9848, // Rule ID 711 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_frsqrt_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3239:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FRSQRT_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FRSQRT_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 711,
        GIR_Done,
      // Label 402: @9848
      GIM_Try, /*On fail goto*//*Label 403*/ 9888, // Rule ID 738 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftint_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3268:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FTINT_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTINT_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 738,
        GIR_Done,
      // Label 403: @9888
      GIM_Try, /*On fail goto*//*Label 404*/ 9928, // Rule ID 739 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftint_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3267:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FTINT_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTINT_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 739,
        GIR_Done,
      // Label 404: @9928
      GIM_Try, /*On fail goto*//*Label 405*/ 9968, // Rule ID 740 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftint_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3270:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FTINT_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTINT_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 740,
        GIR_Done,
      // Label 405: @9968
      GIM_Try, /*On fail goto*//*Label 406*/ 10008, // Rule ID 741 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftint_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3269:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FTINT_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTINT_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 741,
        GIR_Done,
      // Label 406: @10008
      GIM_Try, /*On fail goto*//*Label 407*/ 10048, // Rule ID 876 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_nloc_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3423:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws)  =>  (NLOC_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NLOC_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 876,
        GIR_Done,
      // Label 407: @10048
      GIM_Try, /*On fail goto*//*Label 408*/ 10088, // Rule ID 877 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_nloc_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3425:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws)  =>  (NLOC_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NLOC_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 877,
        GIR_Done,
      // Label 408: @10088
      GIM_Try, /*On fail goto*//*Label 409*/ 10128, // Rule ID 878 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_nloc_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3426:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws)  =>  (NLOC_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NLOC_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 878,
        GIR_Done,
      // Label 409: @10128
      GIM_Try, /*On fail goto*//*Label 410*/ 10168, // Rule ID 879 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_nloc_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3424:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws)  =>  (NLOC_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NLOC_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 879,
        GIR_Done,
      // Label 410: @10168
      GIM_Try, /*On fail goto*//*Label 411*/ 10208, // Rule ID 1208 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceq_w_phl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3450:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs)  =>  (PRECEQ_W_PHL_MM:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQ_W_PHL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1208,
        GIR_Done,
      // Label 411: @10208
      GIM_Try, /*On fail goto*//*Label 412*/ 10248, // Rule ID 1209 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceq_w_phr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3451:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs)  =>  (PRECEQ_W_PHR_MM:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQ_W_PHR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1209,
        GIR_Done,
      // Label 412: @10248
      GIM_Try, /*On fail goto*//*Label 413*/ 10288, // Rule ID 1210 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3452:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEQU_PH_QBL_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1210,
        GIR_Done,
      // Label 413: @10288
      GIM_Try, /*On fail goto*//*Label 414*/ 10328, // Rule ID 1211 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbla,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3453:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEQU_PH_QBLA_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBLA_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1211,
        GIR_Done,
      // Label 414: @10328
      GIM_Try, /*On fail goto*//*Label 415*/ 10368, // Rule ID 1212 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3454:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEQU_PH_QBR_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1212,
        GIR_Done,
      // Label 415: @10368
      GIM_Try, /*On fail goto*//*Label 416*/ 10408, // Rule ID 1213 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precequ_ph_qbra,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3455:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEQU_PH_QBRA_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEQU_PH_QBRA_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1213,
        GIR_Done,
      // Label 416: @10408
      GIM_Try, /*On fail goto*//*Label 417*/ 10448, // Rule ID 1214 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3456:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEU_PH_QBL_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1214,
        GIR_Done,
      // Label 417: @10448
      GIM_Try, /*On fail goto*//*Label 418*/ 10488, // Rule ID 1215 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbla,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3457:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEU_PH_QBLA_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBLA_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1215,
        GIR_Done,
      // Label 418: @10488
      GIM_Try, /*On fail goto*//*Label 419*/ 10528, // Rule ID 1216 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3458:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEU_PH_QBR_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1216,
        GIR_Done,
      // Label 419: @10528
      GIM_Try, /*On fail goto*//*Label 420*/ 10568, // Rule ID 1217 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_preceu_ph_qbra,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3459:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (PRECEU_PH_QBRA_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECEU_PH_QBRA_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1217,
        GIR_Done,
      // Label 420: @10568
      GIM_Try, /*On fail goto*//*Label 421*/ 10608, // Rule ID 1243 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_raddu_w_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3468:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (RADDU_W_QB_MM:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::RADDU_W_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1243,
        GIR_Done,
      // Label 421: @10608
      GIM_Try, /*On fail goto*//*Label 422*/ 10648, // Rule ID 1247 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3470:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs)  =>  (REPLV_PH_MM:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPLV_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1247,
        GIR_Done,
      // Label 422: @10648
      GIM_Try, /*On fail goto*//*Label 423*/ 10688, // Rule ID 1248 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_repl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3471:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs)  =>  (REPLV_QB_MM:{ *:[v4i8] } GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::REPLV_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1248,
        GIR_Done,
      // Label 423: @10688
      GIM_Try, /*On fail goto*//*Label 424*/ 10728, // Rule ID 1258 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_bitrev,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3026:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs)  =>  (BITREV_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BITREV_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1258,
        GIR_Done,
      // Label 424: @10728
      GIM_Reject,
    // Label 370: @10729
    GIM_Try, /*On fail goto*//*Label 425*/ 21997,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/4,
      GIM_Try, /*On fail goto*//*Label 426*/ 10793, // Rule ID 357 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3490:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$rs_sa)  =>  (SHRA_R_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, (imm:{ *:[i32] }):$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 357,
        GIR_Done,
      // Label 426: @10793
      GIM_Try, /*On fail goto*//*Label 427*/ 10852, // Rule ID 361 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3492:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$rs_sa)  =>  (SHRA_R_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 361,
        GIR_Done,
      // Label 427: @10852
      GIM_Try, /*On fail goto*//*Label 428*/ 10911, // Rule ID 452 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3491:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$rs_sa)  =>  (SHRA_R_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, (imm:{ *:[i32] }):$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 452,
        GIR_Done,
      // Label 428: @10911
      GIM_Try, /*On fail goto*//*Label 429*/ 10970, // Rule ID 906 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3472:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$m)  =>  (SAT_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 906,
        GIR_Done,
      // Label 429: @10970
      GIM_Try, /*On fail goto*//*Label 430*/ 11029, // Rule ID 907 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3474:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$m)  =>  (SAT_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 907,
        GIR_Done,
      // Label 430: @11029
      GIM_Try, /*On fail goto*//*Label 431*/ 11088, // Rule ID 908 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3475:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$m)  =>  (SAT_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 908,
        GIR_Done,
      // Label 431: @11088
      GIM_Try, /*On fail goto*//*Label 432*/ 11147, // Rule ID 909 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3473:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$m)  =>  (SAT_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 909,
        GIR_Done,
      // Label 432: @11147
      GIM_Try, /*On fail goto*//*Label 433*/ 11206, // Rule ID 910 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3476:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$m)  =>  (SAT_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 910,
        GIR_Done,
      // Label 433: @11206
      GIM_Try, /*On fail goto*//*Label 434*/ 11265, // Rule ID 911 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3478:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$m)  =>  (SAT_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 911,
        GIR_Done,
      // Label 434: @11265
      GIM_Try, /*On fail goto*//*Label 435*/ 11324, // Rule ID 912 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3479:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$m)  =>  (SAT_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 912,
        GIR_Done,
      // Label 435: @11324
      GIM_Try, /*On fail goto*//*Label 436*/ 11383, // Rule ID 913 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sat_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3477:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$m)  =>  (SAT_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SAT_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 913,
        GIR_Done,
      // Label 436: @11383
      GIM_Try, /*On fail goto*//*Label 437*/ 11442, // Rule ID 953 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srari_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3531:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$m)  =>  (SRARI_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRARI_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 953,
        GIR_Done,
      // Label 437: @11442
      GIM_Try, /*On fail goto*//*Label 438*/ 11501, // Rule ID 954 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srari_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3533:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$m)  =>  (SRARI_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRARI_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 954,
        GIR_Done,
      // Label 438: @11501
      GIM_Try, /*On fail goto*//*Label 439*/ 11560, // Rule ID 955 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srari_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3534:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$m)  =>  (SRARI_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRARI_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 955,
        GIR_Done,
      // Label 439: @11560
      GIM_Try, /*On fail goto*//*Label 440*/ 11619, // Rule ID 956 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srari_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3532:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$m)  =>  (SRARI_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRARI_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 956,
        GIR_Done,
      // Label 440: @11619
      GIM_Try, /*On fail goto*//*Label 441*/ 11678, // Rule ID 969 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlri_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3547:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$m)  =>  (SRLRI_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLRI_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 969,
        GIR_Done,
      // Label 441: @11678
      GIM_Try, /*On fail goto*//*Label 442*/ 11737, // Rule ID 970 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlri_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3549:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$m)  =>  (SRLRI_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLRI_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 970,
        GIR_Done,
      // Label 442: @11737
      GIM_Try, /*On fail goto*//*Label 443*/ 11796, // Rule ID 971 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlri_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3550:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$m)  =>  (SRLRI_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLRI_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 971,
        GIR_Done,
      // Label 443: @11796
      GIM_Try, /*On fail goto*//*Label 444*/ 11855, // Rule ID 972 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlri_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3548:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$m)  =>  (SRLRI_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] }):$m)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLRI_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // m
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 972,
        GIR_Done,
      // Label 444: @11855
      GIM_Try, /*On fail goto*//*Label 445*/ 11914, // Rule ID 1202 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3490:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$sa)  =>  (SHRA_R_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1202,
        GIR_Done,
      // Label 445: @11914
      GIM_Try, /*On fail goto*//*Label 446*/ 11973, // Rule ID 1206 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3492:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (SHRA_R_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1206,
        GIR_Done,
      // Label 446: @11973
      GIM_Try, /*On fail goto*//*Label 447*/ 12032, // Rule ID 1281 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3491:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$sa)  =>  (SHRA_R_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_R_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1281,
        GIR_Done,
      // Label 447: @12032
      GIM_Try, /*On fail goto*//*Label 448*/ 12087, // Rule ID 1852 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3488:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)  =>  (SHRA_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1852,
        GIR_Done,
      // Label 448: @12087
      GIM_Try, /*On fail goto*//*Label 449*/ 12142, // Rule ID 1853 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3493:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)  =>  (SHRL_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRL_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1853,
        GIR_Done,
      // Label 449: @12142
      GIM_Try, /*On fail goto*//*Label 450*/ 12197, // Rule ID 1858 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3489:{ *:[iPTR] }, v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)  =>  (SHRA_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRA_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1858,
        GIR_Done,
      // Label 450: @12197
      GIM_Try, /*On fail goto*//*Label 451*/ 12252, // Rule ID 1859 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3494:{ *:[iPTR] }, v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)  =>  (SHRL_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRL_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1859,
        GIR_Done,
      // Label 451: @12252
      GIM_Try, /*On fail goto*//*Label 452*/ 12304, // Rule ID 327 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2962:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDU_S_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_S_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 327,
        GIR_Done,
      // Label 452: @12304
      GIM_Try, /*On fail goto*//*Label 453*/ 12356, // Rule ID 328 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3581:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBU_S_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_S_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 328,
        GIR_Done,
      // Label 453: @12356
      GIM_Try, /*On fail goto*//*Label 454*/ 12408, // Rule ID 329 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2940:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQ_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQ_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 329,
        GIR_Done,
      // Label 454: @12408
      GIM_Try, /*On fail goto*//*Label 455*/ 12460, // Rule ID 330 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3556:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQ_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQ_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 330,
        GIR_Done,
      // Label 455: @12460
      GIM_Try, /*On fail goto*//*Label 456*/ 12512, // Rule ID 333 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_modsub,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3388:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MODSUB:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MODSUB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 333,
        GIR_Done,
      // Label 456: @12512
      GIM_Try, /*On fail goto*//*Label 457*/ 12564, // Rule ID 337 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3464:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECRQ_QB_PH:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_QB_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 337,
        GIR_Done,
      // Label 457: @12564
      GIM_Try, /*On fail goto*//*Label 458*/ 12616, // Rule ID 338 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3463:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (PRECRQ_PH_W:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_PH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 338,
        GIR_Done,
      // Label 458: @12616
      GIM_Try, /*On fail goto*//*Label 459*/ 12668, // Rule ID 352 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3494:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRLV_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRLV_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 352,
        GIR_Done,
      // Label 459: @12668
      GIM_Try, /*On fail goto*//*Label 460*/ 12720, // Rule ID 356 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3488:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRAV_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 356,
        GIR_Done,
      // Label 460: @12720
      GIM_Try, /*On fail goto*//*Label 461*/ 12772, // Rule ID 358 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3490:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRAV_R_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 358,
        GIR_Done,
      // Label 461: @12772
      GIM_Try, /*On fail goto*//*Label 462*/ 12824, // Rule ID 362 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3492:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRAV_R_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 362,
        GIR_Done,
      // Label 462: @12824
      GIM_Try, /*On fail goto*//*Label 463*/ 12876, // Rule ID 399 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_packrl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3435:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PACKRL_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PACKRL_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 399,
        GIR_Done,
      // Label 463: @12876
      GIM_Try, /*On fail goto*//*Label 464*/ 12928, // Rule ID 423 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adduh_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2963:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDUH_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDUH_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 423,
        GIR_Done,
      // Label 464: @12928
      GIM_Try, /*On fail goto*//*Label 465*/ 12980, // Rule ID 424 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adduh_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2964:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDUH_R_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDUH_R_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 424,
        GIR_Done,
      // Label 465: @12980
      GIM_Try, /*On fail goto*//*Label 466*/ 13032, // Rule ID 425 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subuh_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3582:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBUH_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBUH_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 425,
        GIR_Done,
      // Label 466: @13032
      GIM_Try, /*On fail goto*//*Label 467*/ 13084, // Rule ID 426 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subuh_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3583:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBUH_R_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBUH_R_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 426,
        GIR_Done,
      // Label 467: @13084
      GIM_Try, /*On fail goto*//*Label 468*/ 13136, // Rule ID 427 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2942:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQH_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 427,
        GIR_Done,
      // Label 468: @13136
      GIM_Try, /*On fail goto*//*Label 469*/ 13188, // Rule ID 428 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2943:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQH_R_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_R_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 428,
        GIR_Done,
      // Label 469: @13188
      GIM_Try, /*On fail goto*//*Label 470*/ 13240, // Rule ID 429 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3558:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQH_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 429,
        GIR_Done,
      // Label 470: @13240
      GIM_Try, /*On fail goto*//*Label 471*/ 13292, // Rule ID 430 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3559:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQH_R_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_R_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 430,
        GIR_Done,
      // Label 471: @13292
      GIM_Try, /*On fail goto*//*Label 472*/ 13344, // Rule ID 431 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 2945:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQH_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 431,
        GIR_Done,
      // Label 472: @13344
      GIM_Try, /*On fail goto*//*Label 473*/ 13396, // Rule ID 432 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 2944:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQH_R_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_R_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 432,
        GIR_Done,
      // Label 473: @13396
      GIM_Try, /*On fail goto*//*Label 474*/ 13448, // Rule ID 433 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3561:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQH_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 433,
        GIR_Done,
      // Label 474: @13448
      GIM_Try, /*On fail goto*//*Label 475*/ 13500, // Rule ID 434 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3560:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQH_R_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_R_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 434,
        GIR_Done,
      // Label 475: @13500
      GIM_Try, /*On fail goto*//*Label 476*/ 13552, // Rule ID 451 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3489:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRAV_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 451,
        GIR_Done,
      // Label 476: @13552
      GIM_Try, /*On fail goto*//*Label 477*/ 13604, // Rule ID 453 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3491:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRAV_R_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 453,
        GIR_Done,
      // Label 477: @13604
      GIM_Try, /*On fail goto*//*Label 478*/ 13656, // Rule ID 454 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3493:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHRLV_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRLV_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 454,
        GIR_Done,
      // Label 478: @13656
      GIM_Try, /*On fail goto*//*Label 479*/ 13708, // Rule ID 459 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_add_a_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2935:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ADD_A_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADD_A_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 459,
        GIR_Done,
      // Label 479: @13708
      GIM_Try, /*On fail goto*//*Label 480*/ 13760, // Rule ID 460 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_add_a_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2937:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ADD_A_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADD_A_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 460,
        GIR_Done,
      // Label 480: @13760
      GIM_Try, /*On fail goto*//*Label 481*/ 13812, // Rule ID 461 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_add_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2938:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ADD_A_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADD_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 461,
        GIR_Done,
      // Label 481: @13812
      GIM_Try, /*On fail goto*//*Label 482*/ 13864, // Rule ID 462 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_add_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2936:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ADD_A_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADD_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 462,
        GIR_Done,
      // Label 482: @13864
      GIM_Try, /*On fail goto*//*Label 483*/ 13916, // Rule ID 463 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_a_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2946:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ADDS_A_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_A_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 463,
        GIR_Done,
      // Label 483: @13916
      GIM_Try, /*On fail goto*//*Label 484*/ 13968, // Rule ID 464 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_a_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2948:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ADDS_A_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_A_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 464,
        GIR_Done,
      // Label 484: @13968
      GIM_Try, /*On fail goto*//*Label 485*/ 14020, // Rule ID 465 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2949:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ADDS_A_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 465,
        GIR_Done,
      // Label 485: @14020
      GIM_Try, /*On fail goto*//*Label 486*/ 14072, // Rule ID 466 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2947:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ADDS_A_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 466,
        GIR_Done,
      // Label 486: @14072
      GIM_Try, /*On fail goto*//*Label 487*/ 14124, // Rule ID 467 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2950:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ADDS_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 467,
        GIR_Done,
      // Label 487: @14124
      GIM_Try, /*On fail goto*//*Label 488*/ 14176, // Rule ID 468 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2952:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ADDS_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 468,
        GIR_Done,
      // Label 488: @14176
      GIM_Try, /*On fail goto*//*Label 489*/ 14228, // Rule ID 469 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2953:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ADDS_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 469,
        GIR_Done,
      // Label 489: @14228
      GIM_Try, /*On fail goto*//*Label 490*/ 14280, // Rule ID 470 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2951:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ADDS_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 470,
        GIR_Done,
      // Label 490: @14280
      GIM_Try, /*On fail goto*//*Label 491*/ 14332, // Rule ID 471 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2954:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ADDS_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 471,
        GIR_Done,
      // Label 491: @14332
      GIM_Try, /*On fail goto*//*Label 492*/ 14384, // Rule ID 472 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2956:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ADDS_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 472,
        GIR_Done,
      // Label 492: @14384
      GIM_Try, /*On fail goto*//*Label 493*/ 14436, // Rule ID 473 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2957:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ADDS_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 473,
        GIR_Done,
      // Label 493: @14436
      GIM_Try, /*On fail goto*//*Label 494*/ 14488, // Rule ID 474 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adds_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2955:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ADDS_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDS_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 474,
        GIR_Done,
      // Label 494: @14488
      GIM_Try, /*On fail goto*//*Label 495*/ 14540, // Rule ID 488 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2977:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ASUB_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 488,
        GIR_Done,
      // Label 495: @14540
      GIM_Try, /*On fail goto*//*Label 496*/ 14592, // Rule ID 489 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2979:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ASUB_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 489,
        GIR_Done,
      // Label 496: @14592
      GIM_Try, /*On fail goto*//*Label 497*/ 14644, // Rule ID 490 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2980:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ASUB_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 490,
        GIR_Done,
      // Label 497: @14644
      GIM_Try, /*On fail goto*//*Label 498*/ 14696, // Rule ID 491 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2978:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ASUB_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 491,
        GIR_Done,
      // Label 498: @14696
      GIM_Try, /*On fail goto*//*Label 499*/ 14748, // Rule ID 492 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2981:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (ASUB_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 492,
        GIR_Done,
      // Label 499: @14748
      GIM_Try, /*On fail goto*//*Label 500*/ 14800, // Rule ID 493 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2983:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (ASUB_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 493,
        GIR_Done,
      // Label 500: @14800
      GIM_Try, /*On fail goto*//*Label 501*/ 14852, // Rule ID 494 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2984:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (ASUB_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 494,
        GIR_Done,
      // Label 501: @14852
      GIM_Try, /*On fail goto*//*Label 502*/ 14904, // Rule ID 495 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_asub_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2982:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (ASUB_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ASUB_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 495,
        GIR_Done,
      // Label 502: @14904
      GIM_Try, /*On fail goto*//*Label 503*/ 14956, // Rule ID 496 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2985:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (AVE_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 496,
        GIR_Done,
      // Label 503: @14956
      GIM_Try, /*On fail goto*//*Label 504*/ 15008, // Rule ID 497 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2987:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (AVE_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 497,
        GIR_Done,
      // Label 504: @15008
      GIM_Try, /*On fail goto*//*Label 505*/ 15060, // Rule ID 498 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2988:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (AVE_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 498,
        GIR_Done,
      // Label 505: @15060
      GIM_Try, /*On fail goto*//*Label 506*/ 15112, // Rule ID 499 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2986:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (AVE_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 499,
        GIR_Done,
      // Label 506: @15112
      GIM_Try, /*On fail goto*//*Label 507*/ 15164, // Rule ID 500 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2989:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (AVE_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 500,
        GIR_Done,
      // Label 507: @15164
      GIM_Try, /*On fail goto*//*Label 508*/ 15216, // Rule ID 501 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2991:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (AVE_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 501,
        GIR_Done,
      // Label 508: @15216
      GIM_Try, /*On fail goto*//*Label 509*/ 15268, // Rule ID 502 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2992:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (AVE_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 502,
        GIR_Done,
      // Label 509: @15268
      GIM_Try, /*On fail goto*//*Label 510*/ 15320, // Rule ID 503 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ave_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2990:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (AVE_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVE_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 503,
        GIR_Done,
      // Label 510: @15320
      GIM_Try, /*On fail goto*//*Label 511*/ 15372, // Rule ID 504 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2993:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (AVER_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 504,
        GIR_Done,
      // Label 511: @15372
      GIM_Try, /*On fail goto*//*Label 512*/ 15424, // Rule ID 505 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2995:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (AVER_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 505,
        GIR_Done,
      // Label 512: @15424
      GIM_Try, /*On fail goto*//*Label 513*/ 15476, // Rule ID 506 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 2996:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (AVER_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 506,
        GIR_Done,
      // Label 513: @15476
      GIM_Try, /*On fail goto*//*Label 514*/ 15528, // Rule ID 507 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2994:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (AVER_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 507,
        GIR_Done,
      // Label 514: @15528
      GIM_Try, /*On fail goto*//*Label 515*/ 15580, // Rule ID 508 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 2997:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (AVER_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 508,
        GIR_Done,
      // Label 515: @15580
      GIM_Try, /*On fail goto*//*Label 516*/ 15632, // Rule ID 509 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 2999:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (AVER_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 509,
        GIR_Done,
      // Label 516: @15632
      GIM_Try, /*On fail goto*//*Label 517*/ 15684, // Rule ID 510 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3000:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (AVER_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 510,
        GIR_Done,
      // Label 517: @15684
      GIM_Try, /*On fail goto*//*Label 518*/ 15736, // Rule ID 511 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_aver_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 2998:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (AVER_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::AVER_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 511,
        GIR_Done,
      // Label 518: @15736
      GIM_Try, /*On fail goto*//*Label 519*/ 15788, // Rule ID 620 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3132:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DOTP_S_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 620,
        GIR_Done,
      // Label 519: @15788
      GIM_Try, /*On fail goto*//*Label 520*/ 15840, // Rule ID 621 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3133:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DOTP_S_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 621,
        GIR_Done,
      // Label 520: @15840
      GIM_Try, /*On fail goto*//*Label 521*/ 15892, // Rule ID 622 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3131:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DOTP_S_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 622,
        GIR_Done,
      // Label 521: @15892
      GIM_Try, /*On fail goto*//*Label 522*/ 15944, // Rule ID 623 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3135:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DOTP_U_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 623,
        GIR_Done,
      // Label 522: @15944
      GIM_Try, /*On fail goto*//*Label 523*/ 15996, // Rule ID 624 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3136:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DOTP_U_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 624,
        GIR_Done,
      // Label 523: @15996
      GIM_Try, /*On fail goto*//*Label 524*/ 16048, // Rule ID 625 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dotp_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3134:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DOTP_U_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DOTP_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 625,
        GIR_Done,
      // Label 524: @16048
      GIM_Try, /*On fail goto*//*Label 525*/ 16100, // Rule ID 640 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fcaf_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3174:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FCAF_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FCAF_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 640,
        GIR_Done,
      // Label 525: @16100
      GIM_Try, /*On fail goto*//*Label 526*/ 16152, // Rule ID 641 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fcaf_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3173:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FCAF_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FCAF_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 641,
        GIR_Done,
      // Label 526: @16152
      GIM_Try, /*On fail goto*//*Label 527*/ 16204, // Rule ID 666 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexdo_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v8f16] } 3199:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FEXDO_H:{ *:[v8f16] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXDO_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 666,
        GIR_Done,
      // Label 527: @16204
      GIM_Try, /*On fail goto*//*Label 528*/ 16256, // Rule ID 667 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fexdo_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3200:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FEXDO_W:{ *:[v4f32] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXDO_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 667,
        GIR_Done,
      // Label 528: @16256
      GIM_Try, /*On fail goto*//*Label 529*/ 16308, // Rule ID 694 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmax_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3226:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMAX_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMAX_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 694,
        GIR_Done,
      // Label 529: @16308
      GIM_Try, /*On fail goto*//*Label 530*/ 16360, // Rule ID 695 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmax_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3225:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMAX_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMAX_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 695,
        GIR_Done,
      // Label 530: @16360
      GIM_Try, /*On fail goto*//*Label 531*/ 16412, // Rule ID 696 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmax_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3224:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMAX_A_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMAX_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 696,
        GIR_Done,
      // Label 531: @16412
      GIM_Try, /*On fail goto*//*Label 532*/ 16464, // Rule ID 697 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmax_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3223:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMAX_A_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMAX_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 697,
        GIR_Done,
      // Label 532: @16464
      GIM_Try, /*On fail goto*//*Label 533*/ 16516, // Rule ID 698 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmin_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3230:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMIN_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMIN_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 698,
        GIR_Done,
      // Label 533: @16516
      GIM_Try, /*On fail goto*//*Label 534*/ 16568, // Rule ID 699 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmin_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3229:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMIN_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMIN_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 699,
        GIR_Done,
      // Label 534: @16568
      GIM_Try, /*On fail goto*//*Label 535*/ 16620, // Rule ID 700 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmin_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4f32] } 3228:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMIN_A_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMIN_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 700,
        GIR_Done,
      // Label 535: @16620
      GIM_Try, /*On fail goto*//*Label 536*/ 16672, // Rule ID 701 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fmin_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2f64] } 3227:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMIN_A_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMIN_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 701,
        GIR_Done,
      // Label 536: @16672
      GIM_Try, /*On fail goto*//*Label 537*/ 16724, // Rule ID 712 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsaf_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3242:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSAF_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSAF_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 712,
        GIR_Done,
      // Label 537: @16724
      GIM_Try, /*On fail goto*//*Label 538*/ 16776, // Rule ID 713 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsaf_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3241:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSAF_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSAF_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 713,
        GIR_Done,
      // Label 538: @16776
      GIM_Try, /*On fail goto*//*Label 539*/ 16828, // Rule ID 714 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fseq_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3244:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSEQ_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSEQ_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 714,
        GIR_Done,
      // Label 539: @16828
      GIM_Try, /*On fail goto*//*Label 540*/ 16880, // Rule ID 715 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fseq_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3243:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSEQ_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSEQ_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 715,
        GIR_Done,
      // Label 540: @16880
      GIM_Try, /*On fail goto*//*Label 541*/ 16932, // Rule ID 716 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsle_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3246:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSLE_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSLE_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 716,
        GIR_Done,
      // Label 541: @16932
      GIM_Try, /*On fail goto*//*Label 542*/ 16984, // Rule ID 717 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsle_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3245:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSLE_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSLE_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 717,
        GIR_Done,
      // Label 542: @16984
      GIM_Try, /*On fail goto*//*Label 543*/ 17036, // Rule ID 718 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fslt_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3248:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSLT_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSLT_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 718,
        GIR_Done,
      // Label 543: @17036
      GIM_Try, /*On fail goto*//*Label 544*/ 17088, // Rule ID 719 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fslt_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3247:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSLT_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSLT_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 719,
        GIR_Done,
      // Label 544: @17088
      GIM_Try, /*On fail goto*//*Label 545*/ 17140, // Rule ID 720 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsne_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3250:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSNE_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSNE_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 720,
        GIR_Done,
      // Label 545: @17140
      GIM_Try, /*On fail goto*//*Label 546*/ 17192, // Rule ID 721 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsne_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3249:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSNE_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSNE_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 721,
        GIR_Done,
      // Label 546: @17192
      GIM_Try, /*On fail goto*//*Label 547*/ 17244, // Rule ID 722 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsor_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3252:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSOR_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSOR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 722,
        GIR_Done,
      // Label 547: @17244
      GIM_Try, /*On fail goto*//*Label 548*/ 17296, // Rule ID 723 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsor_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3251:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSOR_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSOR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 723,
        GIR_Done,
      // Label 548: @17296
      GIM_Try, /*On fail goto*//*Label 549*/ 17348, // Rule ID 728 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsueq_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3258:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSUEQ_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUEQ_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 728,
        GIR_Done,
      // Label 549: @17348
      GIM_Try, /*On fail goto*//*Label 550*/ 17400, // Rule ID 729 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsueq_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3257:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSUEQ_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUEQ_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 729,
        GIR_Done,
      // Label 550: @17400
      GIM_Try, /*On fail goto*//*Label 551*/ 17452, // Rule ID 730 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsule_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3260:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSULE_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSULE_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 730,
        GIR_Done,
      // Label 551: @17452
      GIM_Try, /*On fail goto*//*Label 552*/ 17504, // Rule ID 731 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsule_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3259:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSULE_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSULE_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 731,
        GIR_Done,
      // Label 552: @17504
      GIM_Try, /*On fail goto*//*Label 553*/ 17556, // Rule ID 732 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsult_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3262:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSULT_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSULT_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 732,
        GIR_Done,
      // Label 553: @17556
      GIM_Try, /*On fail goto*//*Label 554*/ 17608, // Rule ID 733 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsult_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3261:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSULT_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSULT_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 733,
        GIR_Done,
      // Label 554: @17608
      GIM_Try, /*On fail goto*//*Label 555*/ 17660, // Rule ID 734 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsun_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3264:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSUN_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUN_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 734,
        GIR_Done,
      // Label 555: @17660
      GIM_Try, /*On fail goto*//*Label 556*/ 17712, // Rule ID 735 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsun_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3263:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSUN_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUN_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 735,
        GIR_Done,
      // Label 556: @17712
      GIM_Try, /*On fail goto*//*Label 557*/ 17764, // Rule ID 736 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsune_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3266:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSUNE_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUNE_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 736,
        GIR_Done,
      // Label 557: @17764
      GIM_Try, /*On fail goto*//*Label 558*/ 17816, // Rule ID 737 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_fsune_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3265:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSUNE_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUNE_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 737,
        GIR_Done,
      // Label 558: @17816
      GIM_Try, /*On fail goto*//*Label 559*/ 17868, // Rule ID 742 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftq_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3271:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FTQ_H:{ *:[v8i16] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTQ_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 742,
        GIR_Done,
      // Label 559: @17868
      GIM_Try, /*On fail goto*//*Label 560*/ 17920, // Rule ID 743 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_ftq_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3272:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FTQ_W:{ *:[v4i32] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FTQ_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 743,
        GIR_Done,
      // Label 560: @17920
      GIM_Try, /*On fail goto*//*Label 561*/ 17972, // Rule ID 748 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3278:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (HADD_S_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 748,
        GIR_Done,
      // Label 561: @17972
      GIM_Try, /*On fail goto*//*Label 562*/ 18024, // Rule ID 749 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3279:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (HADD_S_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 749,
        GIR_Done,
      // Label 562: @18024
      GIM_Try, /*On fail goto*//*Label 563*/ 18076, // Rule ID 750 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3277:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (HADD_S_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 750,
        GIR_Done,
      // Label 563: @18076
      GIM_Try, /*On fail goto*//*Label 564*/ 18128, // Rule ID 751 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3281:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (HADD_U_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 751,
        GIR_Done,
      // Label 564: @18128
      GIM_Try, /*On fail goto*//*Label 565*/ 18180, // Rule ID 752 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3282:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (HADD_U_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 752,
        GIR_Done,
      // Label 565: @18180
      GIM_Try, /*On fail goto*//*Label 566*/ 18232, // Rule ID 753 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hadd_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3280:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (HADD_U_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HADD_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 753,
        GIR_Done,
      // Label 566: @18232
      GIM_Try, /*On fail goto*//*Label 567*/ 18284, // Rule ID 754 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3284:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (HSUB_S_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 754,
        GIR_Done,
      // Label 567: @18284
      GIM_Try, /*On fail goto*//*Label 568*/ 18336, // Rule ID 755 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3285:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (HSUB_S_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 755,
        GIR_Done,
      // Label 568: @18336
      GIM_Try, /*On fail goto*//*Label 569*/ 18388, // Rule ID 756 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3283:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (HSUB_S_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 756,
        GIR_Done,
      // Label 569: @18388
      GIM_Try, /*On fail goto*//*Label 570*/ 18440, // Rule ID 757 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3287:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (HSUB_U_H:{ *:[v8i16] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 757,
        GIR_Done,
      // Label 570: @18440
      GIM_Try, /*On fail goto*//*Label 571*/ 18492, // Rule ID 758 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3288:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (HSUB_U_W:{ *:[v4i32] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 758,
        GIR_Done,
      // Label 571: @18492
      GIM_Try, /*On fail goto*//*Label 572*/ 18544, // Rule ID 759 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_hsub_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3286:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (HSUB_U_D:{ *:[v2i64] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::HSUB_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 759,
        GIR_Done,
      // Label 572: @18544
      GIM_Try, /*On fail goto*//*Label 573*/ 18596, // Rule ID 812 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_max_a_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3340:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (MAX_A_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MAX_A_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 812,
        GIR_Done,
      // Label 573: @18596
      GIM_Try, /*On fail goto*//*Label 574*/ 18648, // Rule ID 813 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_max_a_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3342:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MAX_A_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MAX_A_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 813,
        GIR_Done,
      // Label 574: @18648
      GIM_Try, /*On fail goto*//*Label 575*/ 18700, // Rule ID 814 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_max_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3343:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MAX_A_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MAX_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 814,
        GIR_Done,
      // Label 575: @18700
      GIM_Try, /*On fail goto*//*Label 576*/ 18752, // Rule ID 815 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_max_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3341:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (MAX_A_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MAX_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 815,
        GIR_Done,
      // Label 576: @18752
      GIM_Try, /*On fail goto*//*Label 577*/ 18804, // Rule ID 832 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_min_a_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3360:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (MIN_A_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MIN_A_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 832,
        GIR_Done,
      // Label 577: @18804
      GIM_Try, /*On fail goto*//*Label 578*/ 18856, // Rule ID 833 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_min_a_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3362:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MIN_A_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MIN_A_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 833,
        GIR_Done,
      // Label 578: @18856
      GIM_Try, /*On fail goto*//*Label 579*/ 18908, // Rule ID 834 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_min_a_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3363:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MIN_A_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MIN_A_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 834,
        GIR_Done,
      // Label 579: @18908
      GIM_Try, /*On fail goto*//*Label 580*/ 18960, // Rule ID 835 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_min_a_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3361:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (MIN_A_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MIN_A_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 835,
        GIR_Done,
      // Label 580: @18960
      GIM_Try, /*On fail goto*//*Label 581*/ 19012, // Rule ID 868 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mul_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3402:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MUL_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MUL_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 868,
        GIR_Done,
      // Label 581: @19012
      GIM_Try, /*On fail goto*//*Label 582*/ 19064, // Rule ID 869 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mul_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3403:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MUL_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MUL_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 869,
        GIR_Done,
      // Label 582: @19064
      GIM_Try, /*On fail goto*//*Label 583*/ 19116, // Rule ID 870 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulr_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3413:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MULR_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULR_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 870,
        GIR_Done,
      // Label 583: @19116
      GIM_Try, /*On fail goto*//*Label 584*/ 19168, // Rule ID 871 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulr_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3414:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MULR_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULR_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 871,
        GIR_Done,
      // Label 584: @19168
      GIM_Try, /*On fail goto*//*Label 585*/ 19220, // Rule ID 949 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srar_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3527:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SRAR_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRAR_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 949,
        GIR_Done,
      // Label 585: @19220
      GIM_Try, /*On fail goto*//*Label 586*/ 19272, // Rule ID 950 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srar_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3529:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SRAR_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRAR_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 950,
        GIR_Done,
      // Label 586: @19272
      GIM_Try, /*On fail goto*//*Label 587*/ 19324, // Rule ID 951 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srar_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3530:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SRAR_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRAR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 951,
        GIR_Done,
      // Label 587: @19324
      GIM_Try, /*On fail goto*//*Label 588*/ 19376, // Rule ID 952 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srar_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3528:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SRAR_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRAR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 952,
        GIR_Done,
      // Label 588: @19376
      GIM_Try, /*On fail goto*//*Label 589*/ 19428, // Rule ID 965 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlr_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3543:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SRLR_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLR_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 965,
        GIR_Done,
      // Label 589: @19428
      GIM_Try, /*On fail goto*//*Label 590*/ 19480, // Rule ID 966 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlr_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3545:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SRLR_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLR_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 966,
        GIR_Done,
      // Label 590: @19480
      GIM_Try, /*On fail goto*//*Label 591*/ 19532, // Rule ID 967 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlr_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3546:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SRLR_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 967,
        GIR_Done,
      // Label 591: @19532
      GIM_Try, /*On fail goto*//*Label 592*/ 19584, // Rule ID 968 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_srlr_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3544:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SRLR_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRLR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 968,
        GIR_Done,
      // Label 592: @19584
      GIM_Try, /*On fail goto*//*Label 593*/ 19636, // Rule ID 977 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3562:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SUBS_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 977,
        GIR_Done,
      // Label 593: @19636
      GIM_Try, /*On fail goto*//*Label 594*/ 19688, // Rule ID 978 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3564:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SUBS_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 978,
        GIR_Done,
      // Label 594: @19688
      GIM_Try, /*On fail goto*//*Label 595*/ 19740, // Rule ID 979 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3565:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SUBS_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 979,
        GIR_Done,
      // Label 595: @19740
      GIM_Try, /*On fail goto*//*Label 596*/ 19792, // Rule ID 980 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3563:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SUBS_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 980,
        GIR_Done,
      // Label 596: @19792
      GIM_Try, /*On fail goto*//*Label 597*/ 19844, // Rule ID 981 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3566:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SUBS_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 981,
        GIR_Done,
      // Label 597: @19844
      GIM_Try, /*On fail goto*//*Label 598*/ 19896, // Rule ID 982 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3568:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SUBS_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 982,
        GIR_Done,
      // Label 598: @19896
      GIM_Try, /*On fail goto*//*Label 599*/ 19948, // Rule ID 983 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3569:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SUBS_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 983,
        GIR_Done,
      // Label 599: @19948
      GIM_Try, /*On fail goto*//*Label 600*/ 20000, // Rule ID 984 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subs_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3567:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SUBS_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBS_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 984,
        GIR_Done,
      // Label 600: @20000
      GIM_Try, /*On fail goto*//*Label 601*/ 20052, // Rule ID 985 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsus_u_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3570:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SUBSUS_U_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUS_U_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 985,
        GIR_Done,
      // Label 601: @20052
      GIM_Try, /*On fail goto*//*Label 602*/ 20104, // Rule ID 986 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsus_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3572:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SUBSUS_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUS_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 986,
        GIR_Done,
      // Label 602: @20104
      GIM_Try, /*On fail goto*//*Label 603*/ 20156, // Rule ID 987 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsus_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3573:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SUBSUS_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUS_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 987,
        GIR_Done,
      // Label 603: @20156
      GIM_Try, /*On fail goto*//*Label 604*/ 20208, // Rule ID 988 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsus_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3571:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SUBSUS_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUS_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 988,
        GIR_Done,
      // Label 604: @20208
      GIM_Try, /*On fail goto*//*Label 605*/ 20260, // Rule ID 989 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsuu_s_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3574:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SUBSUU_S_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUU_S_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 989,
        GIR_Done,
      // Label 605: @20260
      GIM_Try, /*On fail goto*//*Label 606*/ 20312, // Rule ID 990 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsuu_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3576:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SUBSUU_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUU_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 990,
        GIR_Done,
      // Label 606: @20312
      GIM_Try, /*On fail goto*//*Label 607*/ 20364, // Rule ID 991 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsuu_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3577:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SUBSUU_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUU_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 991,
        GIR_Done,
      // Label 607: @20364
      GIM_Try, /*On fail goto*//*Label 608*/ 20416, // Rule ID 992 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subsuu_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3575:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SUBSUU_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBSUU_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 992,
        GIR_Done,
      // Label 608: @20416
      GIM_Try, /*On fail goto*//*Label 609*/ 20468, // Rule ID 1180 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2940:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQ_S_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQ_S_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1180,
        GIR_Done,
      // Label 609: @20468
      GIM_Try, /*On fail goto*//*Label 610*/ 20520, // Rule ID 1182 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2962:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDU_S_QB_MM:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_S_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1182,
        GIR_Done,
      // Label 610: @20520
      GIM_Try, /*On fail goto*//*Label 611*/ 20572, // Rule ID 1203 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3488:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRAV_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1203,
        GIR_Done,
      // Label 611: @20572
      GIM_Try, /*On fail goto*//*Label 612*/ 20624, // Rule ID 1204 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3490:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRAV_R_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1204,
        GIR_Done,
      // Label 612: @20624
      GIM_Try, /*On fail goto*//*Label 613*/ 20676, // Rule ID 1205 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3492:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRAV_R_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1205,
        GIR_Done,
      // Label 613: @20676
      GIM_Try, /*On fail goto*//*Label 614*/ 20728, // Rule ID 1207 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3494:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRLV_QB_MM:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRLV_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1207,
        GIR_Done,
      // Label 614: @20728
      GIM_Try, /*On fail goto*//*Label 615*/ 20780, // Rule ID 1218 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3556:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQ_S_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQ_S_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1218,
        GIR_Done,
      // Label 615: @20780
      GIM_Try, /*On fail goto*//*Label 616*/ 20832, // Rule ID 1220 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3581:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBU_S_QB_MM:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_S_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1220,
        GIR_Done,
      // Label 616: @20832
      GIM_Try, /*On fail goto*//*Label 617*/ 20884, // Rule ID 1230 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3463:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (PRECRQ_PH_W_MM:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_PH_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1230,
        GIR_Done,
      // Label 617: @20884
      GIM_Try, /*On fail goto*//*Label 618*/ 20936, // Rule ID 1231 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3464:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECRQ_QB_PH_MM:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_QB_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1231,
        GIR_Done,
      // Label 618: @20936
      GIM_Try, /*On fail goto*//*Label 619*/ 20988, // Rule ID 1250 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_packrl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3435:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PACKRL_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PACKRL_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1250,
        GIR_Done,
      // Label 619: @20988
      GIM_Try, /*On fail goto*//*Label 620*/ 21040, // Rule ID 1256 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_modsub,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3388:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MODSUB_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MODSUB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1256,
        GIR_Done,
      // Label 620: @21040
      GIM_Try, /*On fail goto*//*Label 621*/ 21092, // Rule ID 1269 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2942:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQH_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1269,
        GIR_Done,
      // Label 621: @21092
      GIM_Try, /*On fail goto*//*Label 622*/ 21144, // Rule ID 1270 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2943:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDQH_R_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_R_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1270,
        GIR_Done,
      // Label 622: @21144
      GIM_Try, /*On fail goto*//*Label 623*/ 21196, // Rule ID 1271 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 2945:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQH_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1271,
        GIR_Done,
      // Label 623: @21196
      GIM_Try, /*On fail goto*//*Label 624*/ 21248, // Rule ID 1272 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addqh_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 2944:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQH_R_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQH_R_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1272,
        GIR_Done,
      // Label 624: @21248
      GIM_Try, /*On fail goto*//*Label 625*/ 21300, // Rule ID 1275 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adduh_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2963:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDUH_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDUH_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1275,
        GIR_Done,
      // Label 625: @21300
      GIM_Try, /*On fail goto*//*Label 626*/ 21352, // Rule ID 1276 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_adduh_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2964:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ADDUH_R_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDUH_R_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1276,
        GIR_Done,
      // Label 626: @21352
      GIM_Try, /*On fail goto*//*Label 627*/ 21404, // Rule ID 1282 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3489:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRAV_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1282,
        GIR_Done,
      // Label 627: @21404
      GIM_Try, /*On fail goto*//*Label 628*/ 21456, // Rule ID 1283 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shra_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3491:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRAV_R_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRAV_R_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1283,
        GIR_Done,
      // Label 628: @21456
      GIM_Try, /*On fail goto*//*Label 629*/ 21508, // Rule ID 1288 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shrl_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3493:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHRLV_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHRLV_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1288,
        GIR_Done,
      // Label 629: @21508
      GIM_Try, /*On fail goto*//*Label 630*/ 21560, // Rule ID 1289 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3558:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQH_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1289,
        GIR_Done,
      // Label 630: @21560
      GIM_Try, /*On fail goto*//*Label 631*/ 21612, // Rule ID 1290 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_r_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3559:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBQH_R_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_R_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1290,
        GIR_Done,
      // Label 631: @21612
      GIM_Try, /*On fail goto*//*Label 632*/ 21664, // Rule ID 1291 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3561:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQH_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1291,
        GIR_Done,
      // Label 632: @21664
      GIM_Try, /*On fail goto*//*Label 633*/ 21716, // Rule ID 1292 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subqh_r_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[i32] } 3560:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQH_R_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQH_R_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1292,
        GIR_Done,
      // Label 633: @21716
      GIM_Try, /*On fail goto*//*Label 634*/ 21768, // Rule ID 1295 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subuh_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3582:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBUH_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBUH_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1295,
        GIR_Done,
      // Label 634: @21768
      GIM_Try, /*On fail goto*//*Label 635*/ 21820, // Rule ID 1296 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subuh_r_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3583:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (SUBUH_R_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBUH_R_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1296,
        GIR_Done,
      // Label 635: @21820
      GIM_Try, /*On fail goto*//*Label 636*/ 21864, // Rule ID 1834 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addq_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 2939:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (ADDQ_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQ_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1834,
        GIR_Done,
      // Label 636: @21864
      GIM_Try, /*On fail goto*//*Label 637*/ 21908, // Rule ID 1836 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subq_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3555:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (SUBQ_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQ_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1836,
        GIR_Done,
      // Label 637: @21908
      GIM_Try, /*On fail goto*//*Label 638*/ 21952, // Rule ID 1840 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 2960:{ *:[iPTR] }, v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)  =>  (ADDU_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1840,
        GIR_Done,
      // Label 638: @21952
      GIM_Try, /*On fail goto*//*Label 639*/ 21996, // Rule ID 1842 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i8] } 3579:{ *:[iPTR] }, v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)  =>  (SUBU_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, v4i8:{ *:[v4i8] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1842,
        GIR_Done,
      // Label 639: @21996
      GIM_Reject,
    // Label 425: @21997
    GIM_Try, /*On fail goto*//*Label 640*/ 25045,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/5,
      GIM_Try, /*On fail goto*//*Label 641*/ 22073, // Rule ID 449 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_sra_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3461:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PRECR_SRA_PH_W:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_SRA_PH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 449,
        GIR_Done,
      // Label 641: @22073
      GIM_Try, /*On fail goto*//*Label 642*/ 22144, // Rule ID 450 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_sra_r_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3462:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PRECR_SRA_R_PH_W:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_SRA_R_PH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 450,
        GIR_Done,
      // Label 642: @22144
      GIM_Try, /*On fail goto*//*Label 643*/ 22215, // Rule ID 455 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_append,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 2976:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (APPEND:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::APPEND,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 455,
        GIR_Done,
      // Label 643: @22215
      GIM_Try, /*On fail goto*//*Label 644*/ 22286, // Rule ID 456 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_balign,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt2,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3001:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2>>:$sa)  =>  (BALIGN:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BALIGN,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 456,
        GIR_Done,
      // Label 644: @22286
      GIM_Try, /*On fail goto*//*Label 645*/ 22357, // Rule ID 457 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_prepend,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3467:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PREPEND:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PREPEND,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 457,
        GIR_Done,
      // Label 645: @22357
      GIM_Try, /*On fail goto*//*Label 646*/ 22428, // Rule ID 921 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sldi_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3499:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$n)  =>  (SLDI_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, (imm:{ *:[i32] }):$n)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLDI_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // n
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 921,
        GIR_Done,
      // Label 646: @22428
      GIM_Try, /*On fail goto*//*Label 647*/ 22499, // Rule ID 922 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sldi_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3501:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$n)  =>  (SLDI_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, (imm:{ *:[i32] }):$n)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLDI_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // n
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 922,
        GIR_Done,
      // Label 647: @22499
      GIM_Try, /*On fail goto*//*Label 648*/ 22570, // Rule ID 923 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sldi_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt2,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3502:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt2>>:$n)  =>  (SLDI_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, (imm:{ *:[i32] }):$n)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLDI_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // n
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 923,
        GIR_Done,
      // Label 648: @22570
      GIM_Try, /*On fail goto*//*Label 649*/ 22641, // Rule ID 924 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sldi_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt1,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3500:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] })<<P:Predicate_immZExt1>>:$n)  =>  (SLDI_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, (imm:{ *:[i32] }):$n)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLDI_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // n
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 924,
        GIR_Done,
      // Label 649: @22641
      GIM_Try, /*On fail goto*//*Label 650*/ 22712, // Rule ID 1284 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_balign,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt2,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3001:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt2>>:$bp)  =>  (BALIGN_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$bp, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BALIGN_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // bp
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1284,
        GIR_Done,
      // Label 650: @22712
      GIM_Try, /*On fail goto*//*Label 651*/ 22783, // Rule ID 1306 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_sra_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3461:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PRECR_SRA_PH_W_MMR2:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_SRA_PH_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1306,
        GIR_Done,
      // Label 651: @22783
      GIM_Try, /*On fail goto*//*Label 652*/ 22854, // Rule ID 1307 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_sra_r_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[v2i16] } 3462:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PRECR_SRA_R_PH_W_MMR2:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_SRA_R_PH_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1307,
        GIR_Done,
      // Label 652: @22854
      GIM_Try, /*On fail goto*//*Label 653*/ 22925, // Rule ID 1308 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_prepend,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 3467:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (PREPEND_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PREPEND_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1308,
        GIR_Done,
      // Label 653: @22925
      GIM_Try, /*On fail goto*//*Label 654*/ 22996, // Rule ID 1309 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_append,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/4, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_wo_chain:{ *:[i32] } 2976:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (APPEND_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa, GPR32Opnd:{ *:[i32] }:$src)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::APPEND_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1309,
        GIR_Done,
      // Label 654: @22996
      GIM_Try, /*On fail goto*//*Label 655*/ 23060, // Rule ID 520 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsl_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3010:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (BINSL_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSL_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 520,
        GIR_Done,
      // Label 655: @23060
      GIM_Try, /*On fail goto*//*Label 656*/ 23124, // Rule ID 521 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsl_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3012:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (BINSL_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSL_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 521,
        GIR_Done,
      // Label 656: @23124
      GIM_Try, /*On fail goto*//*Label 657*/ 23188, // Rule ID 522 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsl_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3013:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (BINSL_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSL_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 522,
        GIR_Done,
      // Label 657: @23188
      GIM_Try, /*On fail goto*//*Label 658*/ 23252, // Rule ID 523 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsl_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3011:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (BINSL_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSL_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 523,
        GIR_Done,
      // Label 658: @23252
      GIM_Try, /*On fail goto*//*Label 659*/ 23316, // Rule ID 528 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsr_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3018:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (BINSR_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSR_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 528,
        GIR_Done,
      // Label 659: @23316
      GIM_Try, /*On fail goto*//*Label 660*/ 23380, // Rule ID 529 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsr_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3020:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (BINSR_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSR_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 529,
        GIR_Done,
      // Label 660: @23380
      GIM_Try, /*On fail goto*//*Label 661*/ 23444, // Rule ID 530 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsr_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3021:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (BINSR_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSR_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 530,
        GIR_Done,
      // Label 661: @23444
      GIM_Try, /*On fail goto*//*Label 662*/ 23508, // Rule ID 531 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_binsr_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128DRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3019:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (BINSR_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BINSR_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 531,
        GIR_Done,
      // Label 662: @23508
      GIM_Try, /*On fail goto*//*Label 663*/ 23572, // Rule ID 626 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3139:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DPADD_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 626,
        GIR_Done,
      // Label 663: @23572
      GIM_Try, /*On fail goto*//*Label 664*/ 23636, // Rule ID 627 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3140:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DPADD_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 627,
        GIR_Done,
      // Label 664: @23636
      GIM_Try, /*On fail goto*//*Label 665*/ 23700, // Rule ID 628 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3138:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DPADD_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 628,
        GIR_Done,
      // Label 665: @23700
      GIM_Try, /*On fail goto*//*Label 666*/ 23764, // Rule ID 629 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3142:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DPADD_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 629,
        GIR_Done,
      // Label 666: @23764
      GIM_Try, /*On fail goto*//*Label 667*/ 23828, // Rule ID 630 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3143:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DPADD_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 630,
        GIR_Done,
      // Label 667: @23828
      GIM_Try, /*On fail goto*//*Label 668*/ 23892, // Rule ID 631 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpadd_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3141:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DPADD_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPADD_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 631,
        GIR_Done,
      // Label 668: @23892
      GIM_Try, /*On fail goto*//*Label 669*/ 23956, // Rule ID 632 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_s_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3159:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DPSUB_S_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_S_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 632,
        GIR_Done,
      // Label 669: @23956
      GIM_Try, /*On fail goto*//*Label 670*/ 24020, // Rule ID 633 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3160:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DPSUB_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 633,
        GIR_Done,
      // Label 670: @24020
      GIM_Try, /*On fail goto*//*Label 671*/ 24084, // Rule ID 634 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_s_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3158:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DPSUB_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_S_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 634,
        GIR_Done,
      // Label 671: @24084
      GIM_Try, /*On fail goto*//*Label 672*/ 24148, // Rule ID 635 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_u_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v16s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128BRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3162:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (DPSUB_U_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_U_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 635,
        GIR_Done,
      // Label 672: @24148
      GIM_Try, /*On fail goto*//*Label 673*/ 24212, // Rule ID 636 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_u_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3163:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (DPSUB_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_U_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 636,
        GIR_Done,
      // Label 673: @24212
      GIM_Try, /*On fail goto*//*Label 674*/ 24276, // Rule ID 637 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_dpsub_u_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3161:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (DPSUB_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DPSUB_U_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 637,
        GIR_Done,
      // Label 674: @24276
      GIM_Try, /*On fail goto*//*Label 675*/ 24340, // Rule ID 804 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_madd_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3327:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MADD_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 804,
        GIR_Done,
      // Label 675: @24340
      GIM_Try, /*On fail goto*//*Label 676*/ 24404, // Rule ID 805 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_madd_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3328:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MADD_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 805,
        GIR_Done,
      // Label 676: @24404
      GIM_Try, /*On fail goto*//*Label 677*/ 24468, // Rule ID 806 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_maddr_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3329:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MADDR_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDR_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 806,
        GIR_Done,
      // Label 677: @24468
      GIM_Try, /*On fail goto*//*Label 678*/ 24532, // Rule ID 807 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_maddr_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3330:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MADDR_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADDR_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 807,
        GIR_Done,
      // Label 678: @24532
      GIM_Try, /*On fail goto*//*Label 679*/ 24596, // Rule ID 860 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_msub_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3391:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MSUB_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUB_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 860,
        GIR_Done,
      // Label 679: @24596
      GIM_Try, /*On fail goto*//*Label 680*/ 24660, // Rule ID 861 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_msub_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3392:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MSUB_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUB_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 861,
        GIR_Done,
      // Label 680: @24660
      GIM_Try, /*On fail goto*//*Label 681*/ 24724, // Rule ID 862 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_msubr_q_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v8s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128HRegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3393:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (MSUBR_Q_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBR_Q_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 862,
        GIR_Done,
      // Label 681: @24724
      GIM_Try, /*On fail goto*//*Label 682*/ 24788, // Rule ID 863 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_msubr_q_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::MSA128WRegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3394:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (MSUBR_Q_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUBR_Q_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 863,
        GIR_Done,
      // Label 682: @24788
      GIM_Try, /*On fail goto*//*Label 683*/ 24852, // Rule ID 917 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sld_b,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v16s8,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128BRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v16i8] } 3495:{ *:[iPTR] }, MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SLD_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$wd_in, MSA128BOpnd:{ *:[v16i8] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLD_B,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 917,
        GIR_Done,
      // Label 683: @24852
      GIM_Try, /*On fail goto*//*Label 684*/ 24916, // Rule ID 918 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sld_h,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v8s16,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128HRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v8i16] } 3497:{ *:[iPTR] }, MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SLD_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$wd_in, MSA128HOpnd:{ *:[v8i16] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLD_H,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 918,
        GIR_Done,
      // Label 684: @24916
      GIM_Try, /*On fail goto*//*Label 685*/ 24980, // Rule ID 919 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sld_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v4i32] } 3498:{ *:[iPTR] }, MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SLD_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$wd_in, MSA128WOpnd:{ *:[v4i32] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLD_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 919,
        GIR_Done,
      // Label 685: @24980
      GIM_Try, /*On fail goto*//*Label 686*/ 25044, // Rule ID 920 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_sld_d,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/0, /*Op*/4, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/4, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_wo_chain:{ *:[v2i64] } 3496:{ *:[iPTR] }, MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SLD_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$wd_in, MSA128DOpnd:{ *:[v2i64] }:$ws, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLD_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd_in
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/4, // rt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 920,
        GIR_Done,
      // Label 686: @25044
      GIM_Reject,
    // Label 640: @25045
    GIM_Reject,
    // Label 15: @25046
    GIM_Try, /*On fail goto*//*Label 687*/ 25079, // Rule ID 326 //
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/2,
      GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_bposge32,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // (intrinsic_w_chain:{ *:[i32] } 3044:{ *:[iPTR] })  =>  (BPOSGE32_PSEUDO:{ *:[i32] })
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::BPOSGE32_PSEUDO,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // dst
      GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 326,
      GIR_Done,
    // Label 687: @25079
    GIM_Try, /*On fail goto*//*Label 688*/ 25981,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
      GIM_Try, /*On fail goto*//*Label 689*/ 25136, // Rule ID 413 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_rddsp,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt10,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[i32] } 3469:{ *:[iPTR] }, (imm:{ *:[i32] })<<P:Predicate_immZExt10>>:$mask)  =>  (RDDSP:{ *:[i32] } (imm:{ *:[i32] }):$mask)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::RDDSP,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // mask
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 413,
        GIR_Done,
      // Label 689: @25136
      GIM_Try, /*On fail goto*//*Label 690*/ 25188, // Rule ID 414 //
        GIM_CheckFeatures, GIFBS_HasDSP_NotInMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_wrdsp,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt10,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_void 3596:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt10>>:$mask)  =>  (WRDSP GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$mask)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::WRDSP,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // mask
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 414,
        GIR_Done,
      // Label 690: @25188
      GIM_Try, /*On fail goto*//*Label 691*/ 25232, // Rule ID 335 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2932:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ABSQ_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 335,
        GIR_Done,
      // Label 691: @25232
      GIM_Try, /*On fail goto*//*Label 692*/ 25276, // Rule ID 336 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2934:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ABSQ_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 336,
        GIR_Done,
      // Label 692: @25276
      GIM_Try, /*On fail goto*//*Label 693*/ 25320, // Rule ID 422 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 2933:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (ABSQ_S_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 422,
        GIR_Done,
      // Label 693: @25320
      GIM_Try, /*On fail goto*//*Label 694*/ 25364, // Rule ID 1187 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2932:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs)  =>  (ABSQ_S_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1187,
        GIR_Done,
      // Label 694: @25364
      GIM_Try, /*On fail goto*//*Label 695*/ 25408, // Rule ID 1188 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2934:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs)  =>  (ABSQ_S_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1188,
        GIR_Done,
      // Label 695: @25408
      GIM_Try, /*On fail goto*//*Label 696*/ 25452, // Rule ID 1268 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_absq_s_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 2933:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs)  =>  (ABSQ_S_QB_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ABSQ_S_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1268,
        GIR_Done,
      // Label 696: @25452
      GIM_Try, /*On fail goto*//*Label 697*/ 25496, // Rule ID 389 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3110:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_EQ_QB DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_EQ_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 389,
        GIR_Done,
      // Label 697: @25496
      GIM_Try, /*On fail goto*//*Label 698*/ 25540, // Rule ID 390 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3112:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_LT_QB DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_LT_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 390,
        GIR_Done,
      // Label 698: @25540
      GIM_Try, /*On fail goto*//*Label 699*/ 25584, // Rule ID 391 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3111:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_LE_QB DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_LE_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 391,
        GIR_Done,
      // Label 699: @25584
      GIM_Try, /*On fail goto*//*Label 700*/ 25628, // Rule ID 395 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_eq_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3101:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_EQ_PH DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_EQ_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 395,
        GIR_Done,
      // Label 700: @25628
      GIM_Try, /*On fail goto*//*Label 701*/ 25672, // Rule ID 396 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_lt_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3103:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_LT_PH DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_LT_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 396,
        GIR_Done,
      // Label 701: @25672
      GIM_Try, /*On fail goto*//*Label 702*/ 25716, // Rule ID 397 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_le_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3102:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_LE_PH DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_LE_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 397,
        GIR_Done,
      // Label 702: @25716
      GIM_Try, /*On fail goto*//*Label 703*/ 25760, // Rule ID 1259 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_eq_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3101:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_EQ_PH_MM DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_EQ_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1259,
        GIR_Done,
      // Label 703: @25760
      GIM_Try, /*On fail goto*//*Label 704*/ 25804, // Rule ID 1260 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_lt_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3103:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_LT_PH_MM DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_LT_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1260,
        GIR_Done,
      // Label 704: @25804
      GIM_Try, /*On fail goto*//*Label 705*/ 25848, // Rule ID 1261 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmp_le_ph,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3102:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (CMP_LE_PH_MM DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMP_LE_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1261,
        GIR_Done,
      // Label 705: @25848
      GIM_Try, /*On fail goto*//*Label 706*/ 25892, // Rule ID 1265 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3110:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_EQ_QB_MM DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_EQ_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1265,
        GIR_Done,
      // Label 706: @25892
      GIM_Try, /*On fail goto*//*Label 707*/ 25936, // Rule ID 1266 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3112:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_LT_QB_MM DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_LT_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1266,
        GIR_Done,
      // Label 707: @25936
      GIM_Try, /*On fail goto*//*Label 708*/ 25980, // Rule ID 1267 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/0, Intrinsic::mips_cmpu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_void 3111:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPU_LE_QB_MM DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPU_LE_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1267,
        GIR_Done,
      // Label 708: @25980
      GIM_Reject,
    // Label 688: @25981
    GIM_Try, /*On fail goto*//*Label 709*/ 29969,
      GIM_CheckNumOperands, /*MI*/0, /*Expected*/4,
      GIM_Try, /*On fail goto*//*Label 710*/ 26050, // Rule ID 354 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[v2i16] } 3486:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$rs_sa)  =>  (SHLL_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, (imm:{ *:[i32] }):$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 354,
        GIR_Done,
      // Label 710: @26050
      GIM_Try, /*On fail goto*//*Label 711*/ 26114, // Rule ID 359 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[i32] } 3487:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$rs_sa)  =>  (SHLL_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 359,
        GIR_Done,
      // Label 711: @26114
      GIM_Try, /*On fail goto*//*Label 712*/ 26178, // Rule ID 1196 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[v2i16] } 3486:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$sa)  =>  (SHLL_S_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_S_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1196,
        GIR_Done,
      // Label 712: @26178
      GIM_Try, /*On fail goto*//*Label 713*/ 26242, // Rule ID 1201 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[i32] } 3487:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$sa)  =>  (SHLL_S_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, (imm:{ *:[i32] }):$sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_S_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, 1, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1201,
        GIR_Done,
      // Label 713: @26242
      GIM_Try, /*On fail goto*//*Label 714*/ 26297, // Rule ID 1851 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt4,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[v2i16] } 3484:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)  =>  (SHLL_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt4>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1851,
        GIR_Done,
      // Label 714: @26297
      GIM_Try, /*On fail goto*//*Label 715*/ 26352, // Rule ID 1857 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/3, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt3,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (intrinsic_w_chain:{ *:[v4i8] } 3485:{ *:[iPTR] }, v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)  =>  (SHLL_QB:{ *:[v4i8] } v4i8:{ *:[v4i8] }:$a, (imm:{ *:[i32] })<<P:Predicate_immZExt3>>:$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLL_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1857,
        GIR_Done,
      // Label 715: @26352
      GIM_Try, /*On fail goto*//*Label 716*/ 26408, // Rule ID 331 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2941:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQ_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQ_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 331,
        GIR_Done,
      // Label 716: @26408
      GIM_Try, /*On fail goto*//*Label 717*/ 26464, // Rule ID 332 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3557:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQ_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQ_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 332,
        GIR_Done,
      // Label 717: @26464
      GIM_Try, /*On fail goto*//*Label 718*/ 26520, // Rule ID 339 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_rs_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3465:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (PRECRQ_RS_PH_W:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_RS_PH_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 339,
        GIR_Done,
      // Label 718: @26520
      GIM_Try, /*On fail goto*//*Label 719*/ 26576, // Rule ID 340 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrqu_s_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3466:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECRQU_S_QB_PH:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQU_S_QB_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 340,
        GIR_Done,
      // Label 719: @26576
      GIM_Try, /*On fail goto*//*Label 720*/ 26632, // Rule ID 351 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3485:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHLLV_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 351,
        GIR_Done,
      // Label 720: @26632
      GIM_Try, /*On fail goto*//*Label 721*/ 26688, // Rule ID 353 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3484:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHLLV_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 353,
        GIR_Done,
      // Label 721: @26688
      GIM_Try, /*On fail goto*//*Label 722*/ 26744, // Rule ID 355 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3486:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHLLV_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 355,
        GIR_Done,
      // Label 722: @26744
      GIM_Try, /*On fail goto*//*Label 723*/ 26800, // Rule ID 360 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3487:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)  =>  (SHLLV_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs_sa)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs_sa
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 360,
        GIR_Done,
      // Label 723: @26800
      GIM_Try, /*On fail goto*//*Label 724*/ 26856, // Rule ID 363 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleu_s_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3407:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEU_S_PH_QBL:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEU_S_PH_QBL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 363,
        GIR_Done,
      // Label 724: @26856
      GIM_Try, /*On fail goto*//*Label 725*/ 26912, // Rule ID 364 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleu_s_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3408:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEU_S_PH_QBR:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEU_S_PH_QBR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 364,
        GIR_Done,
      // Label 725: @26912
      GIM_Try, /*On fail goto*//*Label 726*/ 26968, // Rule ID 365 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleq_s_w_phl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3405:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEQ_S_W_PHL:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEQ_S_W_PHL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 365,
        GIR_Done,
      // Label 726: @26968
      GIM_Try, /*On fail goto*//*Label 727*/ 27024, // Rule ID 366 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleq_s_w_phr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3406:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEQ_S_W_PHR:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEQ_S_W_PHR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 366,
        GIR_Done,
      // Label 727: @27024
      GIM_Try, /*On fail goto*//*Label 728*/ 27080, // Rule ID 367 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_rs_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3409:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULQ_RS_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_RS_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 367,
        GIR_Done,
      // Label 728: @27080
      GIM_Try, /*On fail goto*//*Label 729*/ 27136, // Rule ID 392 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3107:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_EQ_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_EQ_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 392,
        GIR_Done,
      // Label 729: @27136
      GIM_Try, /*On fail goto*//*Label 730*/ 27192, // Rule ID 393 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3109:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_LT_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_LT_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 393,
        GIR_Done,
      // Label 730: @27192
      GIM_Try, /*On fail goto*//*Label 731*/ 27248, // Rule ID 394 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3108:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_LE_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_LE_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 394,
        GIR_Done,
      // Label 731: @27248
      GIM_Try, /*On fail goto*//*Label 732*/ 27304, // Rule ID 404 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_pick_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3449:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PICK_QB:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PICK_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 404,
        GIR_Done,
      // Label 732: @27304
      GIM_Try, /*On fail goto*//*Label 733*/ 27360, // Rule ID 405 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_pick_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3448:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PICK_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PICK_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 405,
        GIR_Done,
      // Label 733: @27360
      GIM_Try, /*On fail goto*//*Label 734*/ 27416, // Rule ID 409 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_insv,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3309:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs)  =>  (INSV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::INSV,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 409,
        GIR_Done,
      // Label 734: @27416
      GIM_Try, /*On fail goto*//*Label 735*/ 27472, // Rule ID 415 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2959:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDU_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 415,
        GIR_Done,
      // Label 735: @27472
      GIM_Try, /*On fail goto*//*Label 736*/ 27528, // Rule ID 416 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2961:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDU_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 416,
        GIR_Done,
      // Label 736: @27528
      GIM_Try, /*On fail goto*//*Label 737*/ 27584, // Rule ID 417 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3578:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBU_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 417,
        GIR_Done,
      // Label 737: @27584
      GIM_Try, /*On fail goto*//*Label 738*/ 27640, // Rule ID 418 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3580:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBU_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 418,
        GIR_Done,
      // Label 738: @27640
      GIM_Try, /*On fail goto*//*Label 739*/ 27696, // Rule ID 419 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3104:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_EQ_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_EQ_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 419,
        GIR_Done,
      // Label 739: @27696
      GIM_Try, /*On fail goto*//*Label 740*/ 27752, // Rule ID 420 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3106:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_LT_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_LT_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 420,
        GIR_Done,
      // Label 740: @27752
      GIM_Try, /*On fail goto*//*Label 741*/ 27808, // Rule ID 421 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3105:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_LE_QB:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_LE_QB,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 421,
        GIR_Done,
      // Label 741: @27808
      GIM_Try, /*On fail goto*//*Label 742*/ 27864, // Rule ID 435 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mul_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3404:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MUL_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MUL_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 435,
        GIR_Done,
      // Label 742: @27864
      GIM_Try, /*On fail goto*//*Label 743*/ 27920, // Rule ID 436 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3412:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MULQ_S_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_S_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 436,
        GIR_Done,
      // Label 743: @27920
      GIM_Try, /*On fail goto*//*Label 744*/ 27976, // Rule ID 437 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_rs_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3410:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MULQ_RS_W:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_RS_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 437,
        GIR_Done,
      // Label 744: @27976
      GIM_Try, /*On fail goto*//*Label 745*/ 28032, // Rule ID 438 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3411:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULQ_S_PH:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_S_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 438,
        GIR_Done,
      // Label 745: @28032
      GIM_Try, /*On fail goto*//*Label 746*/ 28088, // Rule ID 448 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3460:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECR_QB_PH:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_QB_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 448,
        GIR_Done,
      // Label 746: @28088
      GIM_Try, /*On fail goto*//*Label 747*/ 28144, // Rule ID 1181 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2941:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (ADDQ_S_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDQ_S_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1181,
        GIR_Done,
      // Label 747: @28144
      GIM_Try, /*On fail goto*//*Label 748*/ 28200, // Rule ID 1189 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_insv,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3309:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs)  =>  (INSV_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$src, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::INSV_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1189,
        GIR_Done,
      // Label 748: @28200
      GIM_Try, /*On fail goto*//*Label 749*/ 28256, // Rule ID 1197 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3484:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHLLV_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1197,
        GIR_Done,
      // Label 749: @28256
      GIM_Try, /*On fail goto*//*Label 750*/ 28312, // Rule ID 1198 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3486:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHLLV_S_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_S_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1198,
        GIR_Done,
      // Label 750: @28312
      GIM_Try, /*On fail goto*//*Label 751*/ 28368, // Rule ID 1199 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3485:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHLLV_QB_MM:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1199,
        GIR_Done,
      // Label 751: @28368
      GIM_Try, /*On fail goto*//*Label 752*/ 28424, // Rule ID 1200 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_shll_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3487:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SHLLV_S_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SHLLV_S_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rs
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1200,
        GIR_Done,
      // Label 752: @28424
      GIM_Try, /*On fail goto*//*Label 753*/ 28480, // Rule ID 1219 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3557:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (SUBQ_S_W_MM:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBQ_S_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1219,
        GIR_Done,
      // Label 753: @28480
      GIM_Try, /*On fail goto*//*Label 754*/ 28536, // Rule ID 1225 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleq_s_w_phl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3405:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEQ_S_W_PHL_MM:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEQ_S_W_PHL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1225,
        GIR_Done,
      // Label 754: @28536
      GIM_Try, /*On fail goto*//*Label 755*/ 28592, // Rule ID 1226 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleq_s_w_phr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3406:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEQ_S_W_PHR_MM:{ *:[i32] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEQ_S_W_PHR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1226,
        GIR_Done,
      // Label 755: @28592
      GIM_Try, /*On fail goto*//*Label 756*/ 28648, // Rule ID 1227 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleu_s_ph_qbl,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3407:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEU_S_PH_QBL_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEU_S_PH_QBL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1227,
        GIR_Done,
      // Label 756: @28648
      GIM_Try, /*On fail goto*//*Label 757*/ 28704, // Rule ID 1228 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_muleu_s_ph_qbr,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3408:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULEU_S_PH_QBR_MM:{ *:[v2i16] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULEU_S_PH_QBR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1228,
        GIR_Done,
      // Label 757: @28704
      GIM_Try, /*On fail goto*//*Label 758*/ 28760, // Rule ID 1229 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_rs_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3409:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULQ_RS_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_RS_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1229,
        GIR_Done,
      // Label 758: @28760
      GIM_Try, /*On fail goto*//*Label 759*/ 28816, // Rule ID 1232 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrqu_s_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3466:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECRQU_S_QB_PH_MM:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQU_S_QB_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1232,
        GIR_Done,
      // Label 759: @28816
      GIM_Try, /*On fail goto*//*Label 760*/ 28872, // Rule ID 1233 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precrq_rs_ph_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3465:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (PRECRQ_RS_PH_W_MM:{ *:[v2i16] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECRQ_RS_PH_W_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1233,
        GIR_Done,
      // Label 760: @28872
      GIM_Try, /*On fail goto*//*Label 761*/ 28928, // Rule ID 1251 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_pick_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3448:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PICK_PH_MM:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PICK_PH_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1251,
        GIR_Done,
      // Label 761: @28928
      GIM_Try, /*On fail goto*//*Label 762*/ 28984, // Rule ID 1252 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_pick_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3449:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (PICK_QB_MM:{ *:[v4i8] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PICK_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1252,
        GIR_Done,
      // Label 762: @28984
      GIM_Try, /*On fail goto*//*Label 763*/ 29040, // Rule ID 1262 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3107:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_EQ_QB_MM:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_EQ_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1262,
        GIR_Done,
      // Label 763: @29040
      GIM_Try, /*On fail goto*//*Label 764*/ 29096, // Rule ID 1263 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3109:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_LT_QB_MM:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_LT_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1263,
        GIR_Done,
      // Label 764: @29096
      GIM_Try, /*On fail goto*//*Label 765*/ 29152, // Rule ID 1264 //
        GIM_CheckFeatures, GIFBS_HasDSP_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3108:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGU_LE_QB_MM:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGU_LE_QB_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1264,
        GIR_Done,
      // Label 765: @29152
      GIM_Try, /*On fail goto*//*Label 766*/ 29208, // Rule ID 1273 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2959:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDU_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1273,
        GIR_Done,
      // Label 766: @29208
      GIM_Try, /*On fail goto*//*Label 767*/ 29264, // Rule ID 1274 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addu_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 2961:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (ADDU_S_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDU_S_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1274,
        GIR_Done,
      // Label 767: @29264
      GIM_Try, /*On fail goto*//*Label 768*/ 29320, // Rule ID 1285 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_eq_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3104:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_EQ_QB_MMR2:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_EQ_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1285,
        GIR_Done,
      // Label 768: @29320
      GIM_Try, /*On fail goto*//*Label 769*/ 29376, // Rule ID 1286 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_lt_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3106:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_LT_QB_MMR2:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_LT_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1286,
        GIR_Done,
      // Label 769: @29376
      GIM_Try, /*On fail goto*//*Label 770*/ 29432, // Rule ID 1287 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_cmpgdu_le_qb,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s8,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3105:{ *:[iPTR] }, DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)  =>  (CMPGDU_LE_QB_MMR2:{ *:[i32] } DSPROpnd:{ *:[v4i8] }:$rs, DSPROpnd:{ *:[v4i8] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::CMPGDU_LE_QB_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1287,
        GIR_Done,
      // Label 770: @29432
      GIM_Try, /*On fail goto*//*Label 771*/ 29488, // Rule ID 1293 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3578:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBU_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1293,
        GIR_Done,
      // Label 771: @29488
      GIM_Try, /*On fail goto*//*Label 772*/ 29544, // Rule ID 1294 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_subu_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3580:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (SUBU_S_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SUBU_S_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1294,
        GIR_Done,
      // Label 772: @29544
      GIM_Try, /*On fail goto*//*Label 773*/ 29600, // Rule ID 1301 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mul_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3404:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MUL_S_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MUL_S_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1301,
        GIR_Done,
      // Label 773: @29600
      GIM_Try, /*On fail goto*//*Label 774*/ 29656, // Rule ID 1302 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_rs_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3410:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MULQ_RS_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_RS_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1302,
        GIR_Done,
      // Label 774: @29656
      GIM_Try, /*On fail goto*//*Label 775*/ 29712, // Rule ID 1303 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_s_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3411:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (MULQ_S_PH_MMR2:{ *:[v2i16] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_S_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1303,
        GIR_Done,
      // Label 775: @29712
      GIM_Try, /*On fail goto*//*Label 776*/ 29768, // Rule ID 1304 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mulq_s_w,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 3412:{ *:[iPTR] }, GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)  =>  (MULQ_S_W_MMR2:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rs, GPR32Opnd:{ *:[i32] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MULQ_S_W_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1304,
        GIR_Done,
      // Label 776: @29768
      GIM_Try, /*On fail goto*//*Label 777*/ 29824, // Rule ID 1305 //
        GIM_CheckFeatures, GIFBS_HasDSPR2_InMicroMips,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_precr_qb_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v4s8,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::DSPRRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v4i8] } 3460:{ *:[iPTR] }, DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)  =>  (PRECR_QB_PH_MMR2:{ *:[v4i8] } DSPROpnd:{ *:[v2i16] }:$rs, DSPROpnd:{ *:[v2i16] }:$rt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::PRECR_QB_PH_MMR2,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // rs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // rt
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1305,
        GIR_Done,
      // Label 777: @29824
      GIM_Try, /*On fail goto*//*Label 778*/ 29872, // Rule ID 1838 //
        GIM_CheckFeatures, GIFBS_HasDSPR2,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_mul_ph,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s16,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::DSPRRegClassID,
        // (intrinsic_w_chain:{ *:[v2i16] } 3401:{ *:[iPTR] }, v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)  =>  (MUL_PH:{ *:[v2i16] } v2i16:{ *:[v2i16] }:$a, v2i16:{ *:[v2i16] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MUL_PH,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1838,
        GIR_Done,
      // Label 778: @29872
      GIM_Try, /*On fail goto*//*Label 779*/ 29920, // Rule ID 1844 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addsc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2958:{ *:[iPTR] }, i32:{ *:[i32] }:$a, i32:{ *:[i32] }:$b)  =>  (ADDSC:{ *:[i32] } i32:{ *:[i32] }:$a, i32:{ *:[i32] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDSC,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1844,
        GIR_Done,
      // Label 779: @29920
      GIM_Try, /*On fail goto*//*Label 780*/ 29968, // Rule ID 1846 //
        GIM_CheckFeatures, GIFBS_HasDSP,
        GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, Intrinsic::mips_addwc,
        GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        // (intrinsic_w_chain:{ *:[i32] } 2973:{ *:[iPTR] }, i32:{ *:[i32] }:$a, i32:{ *:[i32] }:$b)  =>  (ADDWC:{ *:[i32] } i32:{ *:[i32] }:$a, i32:{ *:[i32] }:$b)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ADDWC,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // a
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // b
        GIR_MergeMemOperands, /*InsnID*/0, /*MergeInsnID's*/0, GIU_MergeMemOperands_EndOfList,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1846,
        GIR_Done,
      // Label 780: @29968
      GIM_Reject,
    // Label 709: @29969
    GIM_Reject,
    // Label 16: @29970
    GIM_Try, /*On fail goto*//*Label 781*/ 30026, // Rule ID 1514 //
      GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      // (trunc:{ *:[i32] } GPR64:{ *:[i64] }:$src)  =>  (SLL:{ *:[i32] } (EXTRACT_SUBREG:{ *:[i32] } GPR64:{ *:[i64] }:$src, sub_32:{ *:[i32] }), 0:{ *:[i32] })
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, /*SubRegIdx*/1, // src
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLL,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_AddImm, /*InsnID*/0, /*Imm*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1514,
      GIR_Done,
    // Label 781: @30026
    GIM_Reject,
    // Label 17: @30027
    GIM_Try, /*On fail goto*//*Label 782*/ 30086,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 783*/ 30059, // Rule ID 2060 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckI64ImmPredicate, /*MI*/0, /*Predicate*/GIPFP_I64_Predicate_immLi16,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        // MIs[0] Operand 1
        // No operand predicates
        // (imm:{ *:[i32] })<<P:Predicate_immLi16>>:$imm  =>  (LI16_MM:{ *:[i32] } (imm:{ *:[i32] })<<P:Predicate_immLi16>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LI16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2060,
        GIR_Done,
      // Label 783: @30059
      GIM_Try, /*On fail goto*//*Label 784*/ 30085, // Rule ID 1762 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        // MIs[0] Operand 1
        // No operand predicates
        // (imm:{ *:[i32] }):$imm  =>  (LwConstant32:{ *:[i32] } (imm:{ *:[i32] }):$imm, -1:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::LwConstant32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/0, // imm
        GIR_AddImm, /*InsnID*/0, /*Imm*/-1,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1762,
        GIR_Done,
      // Label 784: @30085
      GIM_Reject,
    // Label 782: @30086
    GIM_Reject,
    // Label 18: @30087
    GIM_Try, /*On fail goto*//*Label 785*/ 30114, // Rule ID 1521 //
      GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (sext:{ *:[i64] } GPR32:{ *:[i32] }:$src)  =>  (SLL64_32:{ *:[i64] } GPR32:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLL64_32,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1521,
      GIR_Done,
    // Label 785: @30114
    GIM_Reject,
    // Label 19: @30115
    GIM_Try, /*On fail goto*//*Label 786*/ 30199,
      GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 787*/ 30172, // Rule ID 1520 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
        // (zext:{ *:[i64] } GPR32:{ *:[i32] }:$src)  =>  (DSRL:{ *:[i64] } (DSLL64_32:{ *:[i64] } GPR32:{ *:[i32] }:$src), 32:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::DSLL64_32,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSRL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_AddImm, /*InsnID*/0, /*Imm*/32,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1520,
        GIR_Done,
      // Label 787: @30172
      GIM_Try, /*On fail goto*//*Label 788*/ 30198, // Rule ID 1522 //
        GIM_CheckFeatures, GIFBS_HasMips64r2_HasStdEnc_IsGP64bit_NotInMicroMips,
        // (zext:{ *:[i64] } GPR32:{ *:[i32] }:$src)  =>  (DEXT64_32:{ *:[i64] } GPR32:{ *:[i32] }:$src, 0:{ *:[i32] }, 32:{ *:[i32] })
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DEXT64_32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rt
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_AddImm, /*InsnID*/0, /*Imm*/0,
        GIR_AddImm, /*InsnID*/0, /*Imm*/32,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1522,
        GIR_Done,
      // Label 788: @30198
      GIM_Reject,
    // Label 786: @30199
    GIM_Reject,
    // Label 20: @30200
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 795*/ 30723,
    /*GILLT_s32*//*Label 789*/ 30214,
    /*GILLT_s64*//*Label 790*/ 30467, 0,
    /*GILLT_v2s64*//*Label 791*/ 30595, 0,
    /*GILLT_v4s32*//*Label 792*/ 30627,
    /*GILLT_v8s16*//*Label 793*/ 30659,
    /*GILLT_v16s8*//*Label 794*/ 30691,
    // Label 789: @30214
    GIM_Try, /*On fail goto*//*Label 796*/ 30466,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 797*/ 30267, // Rule ID 43 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$shamt)  =>  (SLL:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 43,
        GIR_Done,
      // Label 797: @30267
      GIM_Try, /*On fail goto*//*Label 798*/ 30310, // Rule ID 1744 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SllX16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SllX16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1744,
        GIR_Done,
      // Label 798: @30310
      GIM_Try, /*On fail goto*//*Label 799*/ 30353, // Rule ID 2072 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt2Shift,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt2Shift>>:$imm)  =>  (SLL16_MM:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt2Shift>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLL16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2072,
        GIR_Done,
      // Label 799: @30353
      GIM_Try, /*On fail goto*//*Label 800*/ 30396, // Rule ID 2073 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SLL_MM:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SLL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2073,
        GIR_Done,
      // Label 800: @30396
      GIM_Try, /*On fail goto*//*Label 801*/ 30419, // Rule ID 46 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (shl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SLLV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLLV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 46,
        GIR_Done,
      // Label 801: @30419
      GIM_Try, /*On fail goto*//*Label 802*/ 30442, // Rule ID 1747 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (shl:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)  =>  (SllvRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SllvRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1747,
        GIR_Done,
      // Label 802: @30442
      GIM_Try, /*On fail goto*//*Label 803*/ 30465, // Rule ID 2074 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (shl:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)  =>  (SLLV_MM:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLLV_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2074,
        GIR_Done,
      // Label 803: @30465
      GIM_Reject,
    // Label 796: @30466
    GIM_Reject,
    // Label 790: @30467
    GIM_Try, /*On fail goto*//*Label 804*/ 30594,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 805*/ 30520, // Rule ID 188 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$shamt)  =>  (DSLL:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSLL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 188,
        GIR_Done,
      // Label 805: @30520
      GIM_Try, /*On fail goto*//*Label 806*/ 30578, // Rule ID 1515 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (shl:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (trunc:{ *:[i32] } GPR64:{ *:[i64] }:$rs))  =>  (DSLLV:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (EXTRACT_SUBREG:{ *:[i32] } GPR64:{ *:[i64] }:$rs, sub_32:{ *:[i32] }))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/1, /*OpIdx*/1, /*SubRegIdx*/1, // rs
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSLLV,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1515,
        GIR_Done,
      // Label 806: @30578
      GIM_Try, /*On fail goto*//*Label 807*/ 30593, // Rule ID 191 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (shl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (DSLLV:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DSLLV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 191,
        GIR_Done,
      // Label 807: @30593
      GIM_Reject,
    // Label 804: @30594
    GIM_Reject,
    // Label 791: @30595
    GIM_Try, /*On fail goto*//*Label 808*/ 30626, // Rule ID 928 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (shl:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SLL_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLL_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 928,
      GIR_Done,
    // Label 808: @30626
    GIM_Reject,
    // Label 792: @30627
    GIM_Try, /*On fail goto*//*Label 809*/ 30658, // Rule ID 927 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (shl:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SLL_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLL_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 927,
      GIR_Done,
    // Label 809: @30658
    GIM_Reject,
    // Label 793: @30659
    GIM_Try, /*On fail goto*//*Label 810*/ 30690, // Rule ID 926 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (shl:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SLL_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLL_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 926,
      GIR_Done,
    // Label 810: @30690
    GIM_Reject,
    // Label 794: @30691
    GIM_Try, /*On fail goto*//*Label 811*/ 30722, // Rule ID 925 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (shl:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SLL_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SLL_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 925,
      GIR_Done,
    // Label 811: @30722
    GIM_Reject,
    // Label 795: @30723
    GIM_Reject,
    // Label 21: @30724
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 818*/ 31247,
    /*GILLT_s32*//*Label 812*/ 30738,
    /*GILLT_s64*//*Label 813*/ 30991, 0,
    /*GILLT_v2s64*//*Label 814*/ 31119, 0,
    /*GILLT_v4s32*//*Label 815*/ 31151,
    /*GILLT_v8s16*//*Label 816*/ 31183,
    /*GILLT_v16s8*//*Label 817*/ 31215,
    // Label 812: @30738
    GIM_Try, /*On fail goto*//*Label 819*/ 30990,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 820*/ 30791, // Rule ID 44 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$shamt)  =>  (SRL:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 44,
        GIR_Done,
      // Label 820: @30791
      GIM_Try, /*On fail goto*//*Label 821*/ 30834, // Rule ID 1745 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SrlX16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SrlX16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1745,
        GIR_Done,
      // Label 821: @30834
      GIM_Try, /*On fail goto*//*Label 822*/ 30877, // Rule ID 2075 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPRMM16RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPRMM16RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt2Shift,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt2Shift>>:$imm)  =>  (SRL16_MM:{ *:[i32] } GPRMM16:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt2Shift>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRL16_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2075,
        GIR_Done,
      // Label 822: @30877
      GIM_Try, /*On fail goto*//*Label 823*/ 30920, // Rule ID 2076 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SRL_MM:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRL_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2076,
        GIR_Done,
      // Label 823: @30920
      GIM_Try, /*On fail goto*//*Label 824*/ 30943, // Rule ID 47 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (srl:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SRLV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRLV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 47,
        GIR_Done,
      // Label 824: @30943
      GIM_Try, /*On fail goto*//*Label 825*/ 30966, // Rule ID 1749 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (srl:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)  =>  (SrlvRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SrlvRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1749,
        GIR_Done,
      // Label 825: @30966
      GIM_Try, /*On fail goto*//*Label 826*/ 30989, // Rule ID 2077 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (srl:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)  =>  (SRLV_MM:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRLV_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2077,
        GIR_Done,
      // Label 826: @30989
      GIM_Reject,
    // Label 819: @30990
    GIM_Reject,
    // Label 813: @30991
    GIM_Try, /*On fail goto*//*Label 827*/ 31118,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 828*/ 31044, // Rule ID 189 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$shamt)  =>  (DSRL:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSRL,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 189,
        GIR_Done,
      // Label 828: @31044
      GIM_Try, /*On fail goto*//*Label 829*/ 31102, // Rule ID 1516 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (srl:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (trunc:{ *:[i32] } GPR64:{ *:[i64] }:$rs))  =>  (DSRLV:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (EXTRACT_SUBREG:{ *:[i32] } GPR64:{ *:[i64] }:$rs, sub_32:{ *:[i32] }))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/1, /*OpIdx*/1, /*SubRegIdx*/1, // rs
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSRLV,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1516,
        GIR_Done,
      // Label 829: @31102
      GIM_Try, /*On fail goto*//*Label 830*/ 31117, // Rule ID 193 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (srl:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (DSRLV:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DSRLV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 193,
        GIR_Done,
      // Label 830: @31117
      GIM_Reject,
    // Label 827: @31118
    GIM_Reject,
    // Label 814: @31119
    GIM_Try, /*On fail goto*//*Label 831*/ 31150, // Rule ID 960 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (srl:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SRL_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRL_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 960,
      GIR_Done,
    // Label 831: @31150
    GIM_Reject,
    // Label 815: @31151
    GIM_Try, /*On fail goto*//*Label 832*/ 31182, // Rule ID 959 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (srl:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SRL_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRL_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 959,
      GIR_Done,
    // Label 832: @31182
    GIM_Reject,
    // Label 816: @31183
    GIM_Try, /*On fail goto*//*Label 833*/ 31214, // Rule ID 958 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (srl:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SRL_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRL_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 958,
      GIR_Done,
    // Label 833: @31214
    GIM_Reject,
    // Label 817: @31215
    GIM_Try, /*On fail goto*//*Label 834*/ 31246, // Rule ID 957 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (srl:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SRL_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRL_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 957,
      GIR_Done,
    // Label 834: @31246
    GIM_Reject,
    // Label 818: @31247
    GIM_Reject,
    // Label 22: @31248
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 9, /*)*//*default:*//*Label 841*/ 31728,
    /*GILLT_s32*//*Label 835*/ 31262,
    /*GILLT_s64*//*Label 836*/ 31472, 0,
    /*GILLT_v2s64*//*Label 837*/ 31600, 0,
    /*GILLT_v4s32*//*Label 838*/ 31632,
    /*GILLT_v8s16*//*Label 839*/ 31664,
    /*GILLT_v16s8*//*Label 840*/ 31696,
    // Label 835: @31262
    GIM_Try, /*On fail goto*//*Label 842*/ 31471,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_Try, /*On fail goto*//*Label 843*/ 31315, // Rule ID 45 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$shamt)  =>  (SRA:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 45,
        GIR_Done,
      // Label 843: @31315
      GIM_Try, /*On fail goto*//*Label 844*/ 31358, // Rule ID 1746 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SraX16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$in, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SraX16,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rx
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // in
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1746,
        GIR_Done,
      // Label 844: @31358
      GIM_Try, /*On fail goto*//*Label 845*/ 31401, // Rule ID 2078 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt5,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)  =>  (SRA_MM:{ *:[i32] } GPR32:{ *:[i32] }:$src, (imm:{ *:[i32] })<<P:Predicate_immZExt5>>:$imm)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SRA_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // src
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // imm
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2078,
        GIR_Done,
      // Label 845: @31401
      GIM_Try, /*On fail goto*//*Label 846*/ 31424, // Rule ID 48 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (sra:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (SRAV:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRAV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 48,
        GIR_Done,
      // Label 846: @31424
      GIM_Try, /*On fail goto*//*Label 847*/ 31447, // Rule ID 1748 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
        // (sra:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)  =>  (SravRxRy16:{ *:[i32] } CPU16Regs:{ *:[i32] }:$r, CPU16Regs:{ *:[i32] }:$ra)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SravRxRy16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1748,
        GIR_Done,
      // Label 847: @31447
      GIM_Try, /*On fail goto*//*Label 848*/ 31470, // Rule ID 2079 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (sra:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)  =>  (SRAV_MM:{ *:[i32] } GPR32:{ *:[i32] }:$lhs, GPR32:{ *:[i32] }:$rhs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRAV_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2079,
        GIR_Done,
      // Label 848: @31470
      GIM_Reject,
    // Label 842: @31471
    GIM_Reject,
    // Label 836: @31472
    GIM_Try, /*On fail goto*//*Label 849*/ 31599,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_Try, /*On fail goto*//*Label 850*/ 31525, // Rule ID 190 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_CONSTANT,
        GIM_CheckI64ImmPredicate, /*MI*/1, /*Predicate*/GIPFP_I64_Predicate_immZExt6,
        // MIs[1] Operand 1
        // No operand predicates
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] })<<P:Predicate_immZExt6>>:$shamt)  =>  (DSRA:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, (imm:{ *:[i32] }):$shamt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSRA,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_CopyConstantAsSImm, /*NewInsnID*/0, /*OldInsnID*/1, // shamt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 190,
        GIR_Done,
      // Label 850: @31525
      GIM_Try, /*On fail goto*//*Label 851*/ 31583, // Rule ID 1517 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_IsGP64bit,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_TRUNC,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (sra:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (trunc:{ *:[i32] } GPR64:{ *:[i64] }:$rs))  =>  (DSRAV:{ *:[i64] } GPR64:{ *:[i64] }:$rt, (EXTRACT_SUBREG:{ *:[i32] } GPR64:{ *:[i64] }:$rs, sub_32:{ *:[i32] }))
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/TargetOpcode::COPY,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_CopySubReg, /*NewInsnID*/1, /*OldInsnID*/1, /*OpIdx*/1, /*SubRegIdx*/1, // rs
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSRAV,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1517,
        GIR_Done,
      // Label 851: @31583
      GIM_Try, /*On fail goto*//*Label 852*/ 31598, // Rule ID 192 //
        GIM_CheckFeatures, GIFBS_HasMips3_HasStdEnc_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
        // (sra:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)  =>  (DSRAV:{ *:[i64] } GPR64Opnd:{ *:[i64] }:$rt, GPR32Opnd:{ *:[i32] }:$rs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::DSRAV,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 192,
        GIR_Done,
      // Label 852: @31598
      GIM_Reject,
    // Label 849: @31599
    GIM_Reject,
    // Label 837: @31600
    GIM_Try, /*On fail goto*//*Label 853*/ 31631, // Rule ID 944 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (sra:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)  =>  (SRA_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2i64] }:$ws, MSA128DOpnd:{ *:[v2i64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRA_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 944,
      GIR_Done,
    // Label 853: @31631
    GIM_Reject,
    // Label 838: @31632
    GIM_Try, /*On fail goto*//*Label 854*/ 31663, // Rule ID 943 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (sra:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)  =>  (SRA_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4i32] }:$ws, MSA128WOpnd:{ *:[v4i32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRA_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 943,
      GIR_Done,
    // Label 854: @31663
    GIM_Reject,
    // Label 839: @31664
    GIM_Try, /*On fail goto*//*Label 855*/ 31695, // Rule ID 942 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v8s16,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v8s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128HRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128HRegClassID,
      // (sra:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)  =>  (SRA_H:{ *:[v8i16] } MSA128HOpnd:{ *:[v8i16] }:$ws, MSA128HOpnd:{ *:[v8i16] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRA_H,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 942,
      GIR_Done,
    // Label 855: @31695
    GIM_Reject,
    // Label 840: @31696
    GIM_Try, /*On fail goto*//*Label 856*/ 31727, // Rule ID 941 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v16s8,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v16s8,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128BRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128BRegClassID,
      // (sra:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)  =>  (SRA_B:{ *:[v16i8] } MSA128BOpnd:{ *:[v16i8] }:$ws, MSA128BOpnd:{ *:[v16i8] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::SRA_B,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 941,
      GIR_Done,
    // Label 856: @31727
    GIM_Reject,
    // Label 841: @31728
    GIM_Reject,
    // Label 23: @31729
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 3, /*)*//*default:*//*Label 859*/ 33182,
    /*GILLT_s32*//*Label 857*/ 31737,
    /*GILLT_s64*//*Label 858*/ 32536,
    // Label 857: @31737
    GIM_Try, /*On fail goto*//*Label 860*/ 31776, // Rule ID 267 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_NotMips4_32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$cond, GPR32Opnd:{ *:[i32] }:$T, GPR32Opnd:{ *:[i32] }:$F)  =>  (PseudoSELECT_I:{ *:[i32] } GPR32Opnd:{ *:[i32] }:$cond, GPR32Opnd:{ *:[i32] }:$T, GPR32Opnd:{ *:[i32] }:$F)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoSELECT_I,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 267,
      GIR_Done,
    // Label 860: @31776
    GIM_Try, /*On fail goto*//*Label 861*/ 31815, // Rule ID 269 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_NotMips4_32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$cond, FGR32Opnd:{ *:[f32] }:$T, FGR32Opnd:{ *:[f32] }:$F)  =>  (PseudoSELECT_S:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$cond, FGR32Opnd:{ *:[f32] }:$T, FGR32Opnd:{ *:[f32] }:$F)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoSELECT_S,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 269,
      GIR_Done,
    // Label 861: @31815
    GIM_Try, /*On fail goto*//*Label 862*/ 31871, // Rule ID 306 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGRCCRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } FGRCCOpnd:{ *:[i32] }:$fd_in, FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)  =>  (SEL_S:{ *:[f32] } FGRCCOpnd:{ *:[i32] }:$fd_in, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SEL_S,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fd_in
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // fs
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 306,
      GIR_Done,
    // Label 862: @31871
    GIM_Try, /*On fail goto*//*Label 863*/ 31927, // Rule ID 1170 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGRCCRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } FGRCCOpnd:{ *:[i32] }:$fd_in, FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)  =>  (SEL_S_MMR6:{ *:[f32] } FGRCCOpnd:{ *:[i32] }:$fd_in, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SEL_S_MMR6,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fd_in
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // fs
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1170,
      GIR_Done,
    // Label 863: @31927
    GIM_Try, /*On fail goto*//*Label 864*/ 31983, // Rule ID 1567 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$F)  =>  (MOVN_I_I:{ *:[i32] } GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_I,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1567,
      GIR_Done,
    // Label 864: @31983
    GIM_Try, /*On fail goto*//*Label 865*/ 32039, // Rule ID 1606 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsGP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } GPR64:{ *:[i64] }:$cond, GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$F)  =>  (MOVN_I64_I:{ *:[i32] } GPR32:{ *:[i32] }:$T, GPR64:{ *:[i64] }:$cond, GPR32:{ *:[i32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I64_I,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1606,
      GIR_Done,
    // Label 865: @32039
    GIM_Try, /*On fail goto*//*Label 866*/ 32095, // Rule ID 1622 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } GPR32:{ *:[i32] }:$cond, FGR32:{ *:[f32] }:$T, FGR32:{ *:[f32] }:$F)  =>  (MOVN_I_S:{ *:[f32] } FGR32:{ *:[f32] }:$T, GPR32:{ *:[i32] }:$cond, FGR32:{ *:[f32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_S,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1622,
      GIR_Done,
    // Label 866: @32095
    GIM_Try, /*On fail goto*//*Label 867*/ 32151, // Rule ID 1635 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsGP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } GPR64:{ *:[i64] }:$cond, FGR32:{ *:[f32] }:$T, FGR32:{ *:[f32] }:$F)  =>  (MOVN_I64_S:{ *:[f32] } FGR32:{ *:[f32] }:$T, GPR64:{ *:[i64] }:$cond, FGR32:{ *:[f32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I64_S,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1635,
      GIR_Done,
    // Label 867: @32151
    GIM_Try, /*On fail goto*//*Label 868*/ 32207, // Rule ID 1793 //
      GIM_CheckFeatures, GIFBS_InMips16Mode,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::CPU16RegsRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::CPU16RegsRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::CPU16RegsRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::CPU16RegsRegClassID,
      // (select:{ *:[i32] } CPU16Regs:{ *:[i32] }:$a, CPU16Regs:{ *:[i32] }:$x, CPU16Regs:{ *:[i32] }:$y)  =>  (SelBneZ:{ *:[i32] } CPU16Regs:{ *:[i32] }:$x, CPU16Regs:{ *:[i32] }:$y, CPU16Regs:{ *:[i32] }:$a)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::SelBneZ,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd_
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // x
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // y
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // a
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1793,
      GIR_Done,
    // Label 868: @32207
    GIM_Try, /*On fail goto*//*Label 869*/ 32263, // Rule ID 2129 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$F)  =>  (MOVN_I_MM:{ *:[i32] } GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_MM,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2129,
      GIR_Done,
    // Label 869: @32263
    GIM_Try, /*On fail goto*//*Label 870*/ 32319, // Rule ID 2143 //
      GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$F)  =>  (MOVN_I_MM:{ *:[i32] } GPR32:{ *:[i32] }:$T, GPR32:{ *:[i32] }:$cond, GPR32:{ *:[i32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_MM,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2143,
      GIR_Done,
    // Label 870: @32319
    GIM_Try, /*On fail goto*//*Label 871*/ 32375, // Rule ID 2171 //
      GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR32RegClassID,
      // (select:{ *:[f32] } GPR32:{ *:[i32] }:$cond, FGR32:{ *:[f32] }:$T, FGR32:{ *:[f32] }:$F)  =>  (MOVN_I_S_MM:{ *:[f32] } FGR32:{ *:[f32] }:$T, GPR32:{ *:[i32] }:$cond, FGR32:{ *:[f32] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_S_MM,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2171,
      GIR_Done,
    // Label 871: @32375
    GIM_Try, /*On fail goto*//*Label 872*/ 32455, // Rule ID 1704 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_HasStdEnc_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } i32:{ *:[i32] }:$cond, i32:{ *:[i32] }:$t, i32:{ *:[i32] }:$f)  =>  (OR:{ *:[i32] } (SELNEZ:{ *:[i32] } i32:{ *:[i32] }:$t, i32:{ *:[i32] }:$cond), (SELEQZ:{ *:[i32] } i32:{ *:[i32] }:$f, i32:{ *:[i32] }:$cond))
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/Mips::SELEQZ,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/3, // f
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SELNEZ,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // t
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::OR,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1704,
      GIR_Done,
    // Label 872: @32455
    GIM_Try, /*On fail goto*//*Label 873*/ 32535, // Rule ID 2188 //
      GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      // (select:{ *:[i32] } i32:{ *:[i32] }:$cond, i32:{ *:[i32] }:$t, i32:{ *:[i32] }:$f)  =>  (OR_MM:{ *:[i32] } (SELNEZ_MMR6:{ *:[i32] } i32:{ *:[i32] }:$t, i32:{ *:[i32] }:$cond), (SELEQZ_MMR6:{ *:[i32] } i32:{ *:[i32] }:$f, i32:{ *:[i32] }:$cond))
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_s32,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/Mips::SELEQZ_MMR6,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/3, // f
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SELNEZ_MMR6,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // t
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::OR_MM,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2188,
      GIR_Done,
    // Label 873: @32535
    GIM_Reject,
    // Label 858: @32536
    GIM_Try, /*On fail goto*//*Label 874*/ 32575, // Rule ID 268 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_NotMips4_32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR64RegClassID,
      // (select:{ *:[i64] } GPR32Opnd:{ *:[i32] }:$cond, GPR64Opnd:{ *:[i64] }:$T, GPR64Opnd:{ *:[i64] }:$F)  =>  (PseudoSELECT_I64:{ *:[i64] } GPR32Opnd:{ *:[i32] }:$cond, GPR64Opnd:{ *:[i64] }:$T, GPR64Opnd:{ *:[i64] }:$F)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoSELECT_I64,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 268,
      GIR_Done,
    // Label 874: @32575
    GIM_Try, /*On fail goto*//*Label 875*/ 32614, // Rule ID 270 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_NotFP64bit_NotMips4_32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::AFGR64RegClassID,
      // (select:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$cond, AFGR64Opnd:{ *:[f64] }:$T, AFGR64Opnd:{ *:[f64] }:$F)  =>  (PseudoSELECT_D32:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$cond, AFGR64Opnd:{ *:[f64] }:$T, AFGR64Opnd:{ *:[f64] }:$F)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoSELECT_D32,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 270,
      GIR_Done,
    // Label 875: @32614
    GIM_Try, /*On fail goto*//*Label 876*/ 32653, // Rule ID 271 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_NotMips4_32,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR64RegClassID,
      // (select:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$cond, FGR64Opnd:{ *:[f64] }:$T, FGR64Opnd:{ *:[f64] }:$F)  =>  (PseudoSELECT_D64:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$cond, FGR64Opnd:{ *:[f64] }:$T, FGR64Opnd:{ *:[f64] }:$F)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoSELECT_D64,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 271,
      GIR_Done,
    // Label 876: @32653
    GIM_Try, /*On fail goto*//*Label 877*/ 32709, // Rule ID 1603 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsGP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR64RegClassID,
      // (select:{ *:[i64] } GPR32:{ *:[i32] }:$cond, GPR64:{ *:[i64] }:$T, GPR64:{ *:[i64] }:$F)  =>  (MOVN_I_I64:{ *:[i64] } GPR64:{ *:[i64] }:$T, GPR32:{ *:[i32] }:$cond, GPR64:{ *:[i64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_I64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1603,
      GIR_Done,
    // Label 877: @32709
    GIM_Try, /*On fail goto*//*Label 878*/ 32765, // Rule ID 1609 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsGP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::GPR64RegClassID,
      // (select:{ *:[i64] } GPR64:{ *:[i64] }:$cond, GPR64:{ *:[i64] }:$T, GPR64:{ *:[i64] }:$F)  =>  (MOVN_I64_I64:{ *:[i64] } GPR64:{ *:[i64] }:$T, GPR64:{ *:[i64] }:$cond, GPR64:{ *:[i64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I64_I64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1609,
      GIR_Done,
    // Label 878: @32765
    GIM_Try, /*On fail goto*//*Label 879*/ 32821, // Rule ID 1648 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::AFGR64RegClassID,
      // (select:{ *:[f64] } GPR32:{ *:[i32] }:$cond, AFGR64:{ *:[f64] }:$T, AFGR64:{ *:[f64] }:$F)  =>  (MOVN_I_D32:{ *:[f64] } AFGR64:{ *:[f64] }:$T, GPR32:{ *:[i32] }:$cond, AFGR64:{ *:[f64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_D32,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1648,
      GIR_Done,
    // Label 879: @32821
    GIM_Try, /*On fail goto*//*Label 880*/ 32877, // Rule ID 1671 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR64RegClassID,
      // (select:{ *:[f64] } GPR32:{ *:[i32] }:$cond, FGR64:{ *:[f64] }:$T, FGR64:{ *:[f64] }:$F)  =>  (MOVN_I_D64:{ *:[f64] } FGR64:{ *:[f64] }:$T, GPR32:{ *:[i32] }:$cond, FGR64:{ *:[f64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_D64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1671,
      GIR_Done,
    // Label 880: @32877
    GIM_Try, /*On fail goto*//*Label 881*/ 32933, // Rule ID 1674 //
      GIM_CheckFeatures, GIFBS_HasMips4_32_HasStdEnc_IsFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::FGR64RegClassID,
      // (select:{ *:[f64] } GPR64:{ *:[i64] }:$cond, FGR64:{ *:[f64] }:$T, FGR64:{ *:[f64] }:$F)  =>  (MOVN_I64_D64:{ *:[f64] } FGR64:{ *:[f64] }:$T, GPR64:{ *:[i64] }:$cond, FGR64:{ *:[f64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I64_D64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1674,
      GIR_Done,
    // Label 881: @32933
    GIM_Try, /*On fail goto*//*Label 882*/ 32989, // Rule ID 2184 //
      GIM_CheckFeatures, GIFBS_InMicroMips_NotFP64bit_NotMips32r6,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::AFGR64RegClassID,
      // (select:{ *:[f64] } GPR32:{ *:[i32] }:$cond, AFGR64:{ *:[f64] }:$T, AFGR64:{ *:[f64] }:$F)  =>  (MOVN_I_D32_MM:{ *:[f64] } AFGR64:{ *:[f64] }:$T, GPR32:{ *:[i32] }:$cond, AFGR64:{ *:[f64] }:$F)
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MOVN_I_D32_MM,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // T
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/3, // F
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2184,
      GIR_Done,
    // Label 882: @32989
    GIM_Try, /*On fail goto*//*Label 883*/ 33069, // Rule ID 1707 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      // (select:{ *:[i64] } i64:{ *:[i64] }:$cond, i64:{ *:[i64] }:$t, i64:{ *:[i64] }:$f)  =>  (OR64:{ *:[i64] } (SELNEZ64:{ *:[i64] } i64:{ *:[i64] }:$t, i64:{ *:[i64] }:$cond), (SELEQZ64:{ *:[i64] } i64:{ *:[i64] }:$f, i64:{ *:[i64] }:$cond))
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/Mips::SELEQZ64,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/3, // f
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SELNEZ64,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // t
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::OR64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1707,
      GIR_Done,
    // Label 883: @33069
    GIM_Try, /*On fail goto*//*Label 884*/ 33181, // Rule ID 1718 //
      GIM_CheckFeatures, GIFBS_HasMips64r6_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      // (select:{ *:[i64] } i32:{ *:[i32] }:$cond, i64:{ *:[i64] }:$t, i64:{ *:[i64] }:$f)  =>  (OR64:{ *:[i64] } (SELNEZ64:{ *:[i64] } i64:{ *:[i64] }:$t, (SLL64_32:{ *:[i64] } i32:{ *:[i32] }:$cond)), (SELEQZ64:{ *:[i64] } i64:{ *:[i64] }:$f, (SLL64_32:{ *:[i64] } i32:{ *:[i32] }:$cond)))
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_MakeTempReg, /*TempRegID*/1, /*TypeID*/GILLT_s64,
      GIR_MakeTempReg, /*TempRegID*/2, /*TypeID*/GILLT_s64,
      GIR_MakeTempReg, /*TempRegID*/3, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/4, /*Opcode*/Mips::SLL64_32,
      GIR_AddTempRegister, /*InsnID*/4, /*TempRegID*/3, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/4, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/4,
      GIR_BuildMI, /*InsnID*/3, /*Opcode*/Mips::SELEQZ64,
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/2, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/3, /*OldInsnID*/0, /*OpIdx*/3, // f
      GIR_AddTempRegister, /*InsnID*/3, /*TempRegID*/3, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/3,
      GIR_BuildMI, /*InsnID*/2, /*Opcode*/Mips::SLL64_32,
      GIR_AddTempRegister, /*InsnID*/2, /*TempRegID*/1, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/2, /*OldInsnID*/0, /*OpIdx*/1, // cond
      GIR_ConstrainSelectedInstOperands, /*InsnID*/2,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::SELNEZ64,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // t
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/1, /*TempRegFlags*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::OR64,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/2, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1718,
      GIR_Done,
    // Label 884: @33181
    GIM_Reject,
    // Label 859: @33182
    GIM_Reject,
    // Label 24: @33183
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 7, /*)*//*default:*//*Label 889*/ 34061,
    /*GILLT_s32*//*Label 885*/ 33195,
    /*GILLT_s64*//*Label 886*/ 33395, 0,
    /*GILLT_v2s64*//*Label 887*/ 33743, 0,
    /*GILLT_v4s32*//*Label 888*/ 33902,
    // Label 885: @33195
    GIM_Try, /*On fail goto*//*Label 890*/ 33394,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 891*/ 33266, // Rule ID 145 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr)  =>  (MADD_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 145,
        GIR_Done,
      // Label 891: @33266
      GIM_Try, /*On fail goto*//*Label 892*/ 33323, // Rule ID 2242 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft))  =>  (MADD_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2242,
        GIR_Done,
      // Label 892: @33323
      GIM_Try, /*On fail goto*//*Label 893*/ 33342, // Rule ID 133 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FADD_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_S,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 133,
        GIR_Done,
      // Label 893: @33342
      GIM_Try, /*On fail goto*//*Label 894*/ 33361, // Rule ID 1089 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FADD_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_S_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1089,
        GIR_Done,
      // Label 894: @33361
      GIM_Try, /*On fail goto*//*Label 895*/ 33393, // Rule ID 1145 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FADD_S_MMR6:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FADD_S_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1145,
        GIR_Done,
      // Label 895: @33393
      GIM_Reject,
    // Label 890: @33394
    GIM_Reject,
    // Label 886: @33395
    GIM_Try, /*On fail goto*//*Label 896*/ 33742,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 897*/ 33466, // Rule ID 147 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr)  =>  (MADD_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 147,
        GIR_Done,
      // Label 897: @33466
      GIM_Try, /*On fail goto*//*Label 898*/ 33527, // Rule ID 149 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f64] } (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft), FGR64Opnd:{ *:[f64] }:$fr)  =>  (MADD_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 149,
        GIR_Done,
      // Label 898: @33527
      GIM_Try, /*On fail goto*//*Label 899*/ 33588, // Rule ID 2243 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft))  =>  (MADD_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2243,
        GIR_Done,
      // Label 899: @33588
      GIM_Try, /*On fail goto*//*Label 900*/ 33649, // Rule ID 2244 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft))  =>  (MADD_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MADD_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2244,
        GIR_Done,
      // Label 900: @33649
      GIM_Try, /*On fail goto*//*Label 901*/ 33672, // Rule ID 134 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fadd:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FADD_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 134,
        GIR_Done,
      // Label 901: @33672
      GIM_Try, /*On fail goto*//*Label 902*/ 33695, // Rule ID 135 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fadd:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FADD_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 135,
        GIR_Done,
      // Label 902: @33695
      GIM_Try, /*On fail goto*//*Label 903*/ 33718, // Rule ID 1093 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fadd:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FADD_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1093,
        GIR_Done,
      // Label 903: @33718
      GIM_Try, /*On fail goto*//*Label 904*/ 33741, // Rule ID 1094 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fadd:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FADD_D64_MM:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1094,
        GIR_Done,
      // Label 904: @33741
      GIM_Reject,
    // Label 896: @33742
    GIM_Reject,
    // Label 887: @33743
    GIM_Try, /*On fail goto*//*Label 905*/ 33901,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 906*/ 33815, // Rule ID 2349 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/2,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[v2f64] } (fmul:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt), MSA128DOpnd:{ *:[v2f64] }:$wd)  =>  (FMADD_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMADD_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2349,
        GIR_Done,
      // Label 906: @33815
      GIM_Try, /*On fail goto*//*Label 907*/ 33877, // Rule ID 1902 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd, (fmul:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt))  =>  (FMADD_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMADD_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1902,
        GIR_Done,
      // Label 907: @33877
      GIM_Try, /*On fail goto*//*Label 908*/ 33900, // Rule ID 639 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (fadd:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FADD_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_D,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 639,
        GIR_Done,
      // Label 908: @33900
      GIM_Reject,
    // Label 905: @33901
    GIM_Reject,
    // Label 888: @33902
    GIM_Try, /*On fail goto*//*Label 909*/ 34060,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 910*/ 33974, // Rule ID 2348 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/2,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[v4f32] } (fmul:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt), MSA128WOpnd:{ *:[v4f32] }:$wd)  =>  (FMADD_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMADD_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2348,
        GIR_Done,
      // Label 910: @33974
      GIM_Try, /*On fail goto*//*Label 911*/ 34036, // Rule ID 1901 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fadd:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd, (fmul:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt))  =>  (FMADD_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMADD_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1901,
        GIR_Done,
      // Label 911: @34036
      GIM_Try, /*On fail goto*//*Label 912*/ 34059, // Rule ID 638 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (fadd:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FADD_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FADD_W,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 638,
        GIR_Done,
      // Label 912: @34059
      GIM_Reject,
    // Label 909: @34060
    GIM_Reject,
    // Label 889: @34061
    GIM_Reject,
    // Label 25: @34062
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 7, /*)*//*default:*//*Label 917*/ 34637,
    /*GILLT_s32*//*Label 913*/ 34074,
    /*GILLT_s64*//*Label 914*/ 34217, 0,
    /*GILLT_v2s64*//*Label 915*/ 34443, 0,
    /*GILLT_v4s32*//*Label 916*/ 34540,
    // Label 913: @34074
    GIM_Try, /*On fail goto*//*Label 918*/ 34216,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 919*/ 34145, // Rule ID 146 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fsub:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr)  =>  (MSUB_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUB_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 146,
        GIR_Done,
      // Label 919: @34145
      GIM_Try, /*On fail goto*//*Label 920*/ 34164, // Rule ID 142 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fsub:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FSUB_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_S,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 142,
        GIR_Done,
      // Label 920: @34164
      GIM_Try, /*On fail goto*//*Label 921*/ 34183, // Rule ID 1092 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fsub:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FSUB_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_S_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1092,
        GIR_Done,
      // Label 921: @34183
      GIM_Try, /*On fail goto*//*Label 922*/ 34215, // Rule ID 1146 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        // (fsub:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FSUB_S_MMR6:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FSUB_S_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1146,
        GIR_Done,
      // Label 922: @34215
      GIM_Reject,
    // Label 918: @34216
    GIM_Reject,
    // Label 914: @34217
    GIM_Try, /*On fail goto*//*Label 923*/ 34442,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 924*/ 34288, // Rule ID 148 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fsub:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr)  =>  (MSUB_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUB_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 148,
        GIR_Done,
      // Label 924: @34288
      GIM_Try, /*On fail goto*//*Label 925*/ 34349, // Rule ID 150 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fsub:{ *:[f64] } (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft), FGR64Opnd:{ *:[f64] }:$fr)  =>  (MSUB_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::MSUB_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 150,
        GIR_Done,
      // Label 925: @34349
      GIM_Try, /*On fail goto*//*Label 926*/ 34372, // Rule ID 143 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fsub:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FSUB_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 143,
        GIR_Done,
      // Label 926: @34372
      GIM_Try, /*On fail goto*//*Label 927*/ 34395, // Rule ID 144 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fsub:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FSUB_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 144,
        GIR_Done,
      // Label 927: @34395
      GIM_Try, /*On fail goto*//*Label 928*/ 34418, // Rule ID 1099 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fsub:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FSUB_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1099,
        GIR_Done,
      // Label 928: @34418
      GIM_Try, /*On fail goto*//*Label 929*/ 34441, // Rule ID 1100 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fsub:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FSUB_D64_MM:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1100,
        GIR_Done,
      // Label 929: @34441
      GIM_Reject,
    // Label 923: @34442
    GIM_Reject,
    // Label 915: @34443
    GIM_Try, /*On fail goto*//*Label 930*/ 34539,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_Try, /*On fail goto*//*Label 931*/ 34515, // Rule ID 1900 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fsub:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd, (fmul:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt))  =>  (FMSUB_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMSUB_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1900,
        GIR_Done,
      // Label 931: @34515
      GIM_Try, /*On fail goto*//*Label 932*/ 34538, // Rule ID 727 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (fsub:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FSUB_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_D,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 727,
        GIR_Done,
      // Label 932: @34538
      GIM_Reject,
    // Label 930: @34539
    GIM_Reject,
    // Label 916: @34540
    GIM_Try, /*On fail goto*//*Label 933*/ 34636,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_Try, /*On fail goto*//*Label 934*/ 34612, // Rule ID 1899 //
        GIM_CheckFeatures, GIFBS_AllowFPOpFusion_HasMSA_HasStdEnc,
        GIM_CheckIsSameOperand, /*MI*/0, /*OpIdx*/0, /*OtherMI*/0, /*OtherOpIdx*/1,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fsub:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd, (fmul:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt))  =>  (FMSUB_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMSUB_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1899,
        GIR_Done,
      // Label 934: @34612
      GIM_Try, /*On fail goto*//*Label 935*/ 34635, // Rule ID 726 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (fsub:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FSUB_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FSUB_W,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 726,
        GIR_Done,
      // Label 935: @34635
      GIM_Reject,
    // Label 933: @34636
    GIM_Reject,
    // Label 917: @34637
    GIM_Reject,
    // Label 26: @34638
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 7, /*)*//*default:*//*Label 940*/ 35074,
    /*GILLT_s32*//*Label 936*/ 34650,
    /*GILLT_s64*//*Label 937*/ 34720, 0,
    /*GILLT_v2s64*//*Label 938*/ 34824, 0,
    /*GILLT_v4s32*//*Label 939*/ 34949,
    // Label 936: @34650
    GIM_Try, /*On fail goto*//*Label 941*/ 34719,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 942*/ 34683, // Rule ID 139 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        // (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FMUL_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_S,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 139,
        GIR_Done,
      // Label 942: @34683
      GIM_Try, /*On fail goto*//*Label 943*/ 34694, // Rule ID 1091 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
        // (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FMUL_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_S_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1091,
        GIR_Done,
      // Label 943: @34694
      GIM_Try, /*On fail goto*//*Label 944*/ 34718, // Rule ID 1147 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
        // (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FMUL_S_MMR6:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FMUL_S_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1147,
        GIR_Done,
      // Label 944: @34718
      GIM_Reject,
    // Label 941: @34719
    GIM_Reject,
    // Label 937: @34720
    GIM_Try, /*On fail goto*//*Label 945*/ 34823,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 946*/ 34753, // Rule ID 140 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FMUL_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 140,
        GIR_Done,
      // Label 946: @34753
      GIM_Try, /*On fail goto*//*Label 947*/ 34776, // Rule ID 141 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FMUL_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 141,
        GIR_Done,
      // Label 947: @34776
      GIM_Try, /*On fail goto*//*Label 948*/ 34799, // Rule ID 1097 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FMUL_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1097,
        GIR_Done,
      // Label 948: @34799
      GIM_Try, /*On fail goto*//*Label 949*/ 34822, // Rule ID 1098 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FMUL_D64_MM:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1098,
        GIR_Done,
      // Label 949: @34822
      GIM_Reject,
    // Label 945: @34823
    GIM_Reject,
    // Label 938: @34824
    GIM_Try, /*On fail goto*//*Label 950*/ 34948,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_Try, /*On fail goto*//*Label 951*/ 34883, // Rule ID 2290 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FEXP2,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fmul:{ *:[v2f64] } (fexp2:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wt), MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FEXP2_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXP2_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2290,
        GIR_Done,
      // Label 951: @34883
      GIM_Try, /*On fail goto*//*Label 952*/ 34928, // Rule ID 669 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FEXP2,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v2s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fmul:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, (fexp2:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wt))  =>  (FEXP2_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXP2_D,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 669,
        GIR_Done,
      // Label 952: @34928
      GIM_Try, /*On fail goto*//*Label 953*/ 34947, // Rule ID 705 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
        // (fmul:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMUL_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_D,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 705,
        GIR_Done,
      // Label 953: @34947
      GIM_Reject,
    // Label 950: @34948
    GIM_Reject,
    // Label 939: @34949
    GIM_Try, /*On fail goto*//*Label 954*/ 35073,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_Try, /*On fail goto*//*Label 955*/ 35008, // Rule ID 2289 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FEXP2,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fmul:{ *:[v4f32] } (fexp2:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wt), MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FEXP2_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXP2_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2289,
        GIR_Done,
      // Label 955: @35008
      GIM_Try, /*On fail goto*//*Label 956*/ 35053, // Rule ID 668 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/2, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FEXP2,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_v4s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        // (fmul:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, (fexp2:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wt))  =>  (FEXP2_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FEXP2_W,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // wd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // ws
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // wt
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 668,
        GIR_Done,
      // Label 956: @35053
      GIM_Try, /*On fail goto*//*Label 957*/ 35072, // Rule ID 704 //
        GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
        // (fmul:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMUL_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMUL_W,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 704,
        GIR_Done,
      // Label 957: @35072
      GIM_Reject,
    // Label 954: @35073
    GIM_Reject,
    // Label 940: @35074
    GIM_Reject,
    // Label 27: @35075
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 960*/ 35164,
    /*GILLT_v2s64*//*Label 958*/ 35084, 0,
    /*GILLT_v4s32*//*Label 959*/ 35124,
    // Label 958: @35084
    GIM_Try, /*On fail goto*//*Label 961*/ 35123, // Rule ID 693 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128DRegClassID,
      // (fma:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd_in, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FMADD_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$wd_in, MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMADD_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 693,
      GIR_Done,
    // Label 961: @35123
    GIM_Reject,
    // Label 959: @35124
    GIM_Try, /*On fail goto*//*Label 962*/ 35163, // Rule ID 692 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/3, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/3, /*RC*/Mips::MSA128WRegClassID,
      // (fma:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd_in, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FMADD_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$wd_in, MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FMADD_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 692,
      GIR_Done,
    // Label 962: @35163
    GIM_Reject,
    // Label 960: @35164
    GIM_Reject,
    // Label 28: @35165
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 7, /*)*//*default:*//*Label 967*/ 35415,
    /*GILLT_s32*//*Label 963*/ 35177,
    /*GILLT_s64*//*Label 964*/ 35247, 0,
    /*GILLT_v2s64*//*Label 965*/ 35351, 0,
    /*GILLT_v4s32*//*Label 966*/ 35383,
    // Label 963: @35177
    GIM_Try, /*On fail goto*//*Label 968*/ 35246,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 969*/ 35210, // Rule ID 136 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotInMicroMips,
        // (fdiv:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FDIV_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_S,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 136,
        GIR_Done,
      // Label 969: @35210
      GIM_Try, /*On fail goto*//*Label 970*/ 35221, // Rule ID 1090 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
        // (fdiv:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FDIV_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_S_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1090,
        GIR_Done,
      // Label 970: @35221
      GIM_Try, /*On fail goto*//*Label 971*/ 35245, // Rule ID 1148 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
        // (fdiv:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)  =>  (FDIV_S_MMR6:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$ft, FGR32Opnd:{ *:[f32] }:$fs)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::FDIV_S_MMR6,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // ft
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // fs
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1148,
        GIR_Done,
      // Label 971: @35245
      GIM_Reject,
    // Label 968: @35246
    GIM_Reject,
    // Label 964: @35247
    GIM_Try, /*On fail goto*//*Label 972*/ 35350,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 973*/ 35280, // Rule ID 137 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fdiv:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FDIV_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 137,
        GIR_Done,
      // Label 973: @35280
      GIM_Try, /*On fail goto*//*Label 974*/ 35303, // Rule ID 138 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fdiv:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FDIV_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 138,
        GIR_Done,
      // Label 974: @35303
      GIM_Try, /*On fail goto*//*Label 975*/ 35326, // Rule ID 1095 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        // (fdiv:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)  =>  (FDIV_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1095,
        GIR_Done,
      // Label 975: @35326
      GIM_Try, /*On fail goto*//*Label 976*/ 35349, // Rule ID 1096 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        // (fdiv:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)  =>  (FDIV_D64_MM:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1096,
        GIR_Done,
      // Label 976: @35349
      GIM_Reject,
    // Label 972: @35350
    GIM_Reject,
    // Label 965: @35351
    GIM_Try, /*On fail goto*//*Label 977*/ 35382, // Rule ID 665 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128DRegClassID,
      // (fdiv:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)  =>  (FDIV_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws, MSA128DOpnd:{ *:[v2f64] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 665,
      GIR_Done,
    // Label 977: @35382
    GIM_Reject,
    // Label 966: @35383
    GIM_Try, /*On fail goto*//*Label 978*/ 35414, // Rule ID 664 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/Mips::MSA128WRegClassID,
      // (fdiv:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)  =>  (FDIV_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws, MSA128WOpnd:{ *:[v4f32] }:$wt)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FDIV_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 664,
      GIR_Done,
    // Label 978: @35414
    GIM_Reject,
    // Label 967: @35415
    GIM_Reject,
    // Label 29: @35416
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 981*/ 35473,
    /*GILLT_v2s64*//*Label 979*/ 35425, 0,
    /*GILLT_v4s32*//*Label 980*/ 35449,
    // Label 979: @35425
    GIM_Try, /*On fail goto*//*Label 982*/ 35448, // Rule ID 671 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (fexp2:{ *:[v2f64] } MSA128D:{ *:[v2f64] }:$ws)  =>  (FEXP2_D_1_PSEUDO:{ *:[v2f64] } MSA128D:{ *:[v2f64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FEXP2_D_1_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 671,
      GIR_Done,
    // Label 982: @35448
    GIM_Reject,
    // Label 980: @35449
    GIM_Try, /*On fail goto*//*Label 983*/ 35472, // Rule ID 670 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (fexp2:{ *:[v4f32] } MSA128W:{ *:[v4f32] }:$ws)  =>  (FEXP2_W_1_PSEUDO:{ *:[v4f32] } MSA128W:{ *:[v4f32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FEXP2_W_1_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 670,
      GIR_Done,
    // Label 983: @35472
    GIM_Reject,
    // Label 981: @35473
    GIM_Reject,
    // Label 30: @35474
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 986*/ 35531,
    /*GILLT_v2s64*//*Label 984*/ 35483, 0,
    /*GILLT_v4s32*//*Label 985*/ 35507,
    // Label 984: @35483
    GIM_Try, /*On fail goto*//*Label 987*/ 35506, // Rule ID 691 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (flog2:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FLOG2_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FLOG2_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 691,
      GIR_Done,
    // Label 987: @35506
    GIM_Reject,
    // Label 985: @35507
    GIM_Try, /*On fail goto*//*Label 988*/ 35530, // Rule ID 690 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (flog2:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FLOG2_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FLOG2_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 690,
      GIR_Done,
    // Label 988: @35530
    GIM_Reject,
    // Label 986: @35531
    GIM_Reject,
    // Label 31: @35532
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 3, /*)*//*default:*//*Label 991*/ 36827,
    /*GILLT_s32*//*Label 989*/ 35540,
    /*GILLT_s64*//*Label 990*/ 36041,
    // Label 989: @35540
    GIM_Try, /*On fail goto*//*Label 992*/ 36040,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 993*/ 35624, // Rule ID 1414 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fadd:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr))  =>  (NMADD_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1414,
        GIR_Done,
      // Label 993: @35624
      GIM_Try, /*On fail goto*//*Label 994*/ 35698, // Rule ID 2145 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotMips32r6_NotMips32r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fadd:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr))  =>  (NMADD_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_S_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2145,
        GIR_Done,
      // Label 994: @35698
      GIM_Try, /*On fail goto*//*Label 995*/ 35772, // Rule ID 2324 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)))  =>  (NMADD_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2324,
        GIR_Done,
      // Label 995: @35772
      GIM_Try, /*On fail goto*//*Label 996*/ 35846, // Rule ID 2421 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotMips32r6_NotMips32r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fadd:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)))  =>  (NMADD_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_S_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2421,
        GIR_Done,
      // Label 996: @35846
      GIM_Try, /*On fail goto*//*Label 997*/ 35920, // Rule ID 1415 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FSUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fsub:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr))  =>  (NMSUB_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMSUB_S,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1415,
        GIR_Done,
      // Label 997: @35920
      GIM_Try, /*On fail goto*//*Label 998*/ 35994, // Rule ID 2146 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotMips32r6_NotMips32r6,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FSUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s32,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s32,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s32,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR32RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f32] } (fsub:{ *:[f32] } (fmul:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft), FGR32Opnd:{ *:[f32] }:$fr))  =>  (NMSUB_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fr, FGR32Opnd:{ *:[f32] }:$fs, FGR32Opnd:{ *:[f32] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMSUB_S_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2146,
        GIR_Done,
      // Label 998: @35994
      GIM_Try, /*On fail goto*//*Label 999*/ 36009, // Rule ID 111 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        // (fneg:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (FNEG_S:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_S,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 111,
        GIR_Done,
      // Label 999: @36009
      GIM_Try, /*On fail goto*//*Label 1000*/ 36024, // Rule ID 1112 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        // (fneg:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (FNEG_S_MM:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_S_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1112,
        GIR_Done,
      // Label 1000: @36024
      GIM_Try, /*On fail goto*//*Label 1001*/ 36039, // Rule ID 1149 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
        // (fneg:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (FNEG_S_MMR6:{ *:[f32] } FGR32Opnd:{ *:[f32] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_S_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1149,
        GIR_Done,
      // Label 1001: @36039
      GIM_Reject,
    // Label 992: @36040
    GIM_Reject,
    // Label 990: @36041
    GIM_Try, /*On fail goto*//*Label 1002*/ 36826,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_Try, /*On fail goto*//*Label 1003*/ 36125, // Rule ID 1416 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr))  =>  (NMADD_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1416,
        GIR_Done,
      // Label 1003: @36125
      GIM_Try, /*On fail goto*//*Label 1004*/ 36203, // Rule ID 1418 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft), FGR64Opnd:{ *:[f64] }:$fr))  =>  (NMADD_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1418,
        GIR_Done,
      // Label 1004: @36203
      GIM_Try, /*On fail goto*//*Label 1005*/ 36281, // Rule ID 2147 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotFP64bit_NotMips32r6_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr))  =>  (NMADD_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D32_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2147,
        GIR_Done,
      // Label 1005: @36281
      GIM_Try, /*On fail goto*//*Label 1006*/ 36359, // Rule ID 2325 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)))  =>  (NMADD_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2325,
        GIR_Done,
      // Label 1006: @36359
      GIM_Try, /*On fail goto*//*Label 1007*/ 36437, // Rule ID 2326 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)))  =>  (NMADD_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2326,
        GIR_Done,
      // Label 1007: @36437
      GIM_Try, /*On fail goto*//*Label 1008*/ 36515, // Rule ID 2422 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotFP64bit_NotMips32r6_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FADD,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/2, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fadd:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)))  =>  (NMADD_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMADD_D32_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/1, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2422,
        GIR_Done,
      // Label 1008: @36515
      GIM_Try, /*On fail goto*//*Label 1009*/ 36593, // Rule ID 1417 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_NoNaNsFPMath_NotFP64bit_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FSUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fsub:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr))  =>  (NMSUB_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMSUB_D32,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1417,
        GIR_Done,
      // Label 1009: @36593
      GIM_Try, /*On fail goto*//*Label 1010*/ 36671, // Rule ID 1419 //
        GIM_CheckFeatures, GIFBS_HasMadd4_HasMips4_32r2_HasStdEnc_IsFP64bit_NoNaNsFPMath_NotInMicroMips_NotMips32r6_NotMips64r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FSUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fsub:{ *:[f64] } (fmul:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft), FGR64Opnd:{ *:[f64] }:$fr))  =>  (NMSUB_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fr, FGR64Opnd:{ *:[f64] }:$fs, FGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMSUB_D64,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1419,
        GIR_Done,
      // Label 1010: @36671
      GIM_Try, /*On fail goto*//*Label 1011*/ 36749, // Rule ID 2148 //
        GIM_CheckFeatures, GIFBS_HasMadd4_InMicroMips_InMicroMips_NoNaNsFPMath_NotFP64bit_NotMips32r6_NotMips32r6,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_RecordInsn, /*DefineMI*/1, /*MI*/0, /*OpIdx*/1, // MIs[1]
        GIM_CheckOpcode, /*MI*/1, TargetOpcode::G_FSUB,
        GIM_CheckType, /*MI*/1, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/1, /*Op*/2, /*Type*/GILLT_s64,
        GIM_RecordInsn, /*DefineMI*/2, /*MI*/1, /*OpIdx*/1, // MIs[2]
        GIM_CheckOpcode, /*MI*/2, TargetOpcode::G_FMUL,
        GIM_CheckType, /*MI*/2, /*Op*/1, /*Type*/GILLT_s64,
        GIM_CheckType, /*MI*/2, /*Op*/2, /*Type*/GILLT_s64,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/2, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/1, /*Op*/2, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckIsSafeToFold, /*InsnID*/1,
        GIM_CheckIsSafeToFold, /*InsnID*/2,
        // (fneg:{ *:[f64] } (fsub:{ *:[f64] } (fmul:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft), AFGR64Opnd:{ *:[f64] }:$fr))  =>  (NMSUB_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fr, AFGR64Opnd:{ *:[f64] }:$fs, AFGR64Opnd:{ *:[f64] }:$ft)
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::NMSUB_D32_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // fd
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/1, /*OpIdx*/2, // fr
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/1, // fs
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/2, /*OpIdx*/2, // ft
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2148,
        GIR_Done,
      // Label 1011: @36749
      GIM_Try, /*On fail goto*//*Label 1012*/ 36768, // Rule ID 112 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsNotSoftFloat_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        // (fneg:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs)  =>  (FNEG_D32:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 112,
        GIR_Done,
      // Label 1012: @36768
      GIM_Try, /*On fail goto*//*Label 1013*/ 36787, // Rule ID 113 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_IsNotSoftFloat_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        // (fneg:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs)  =>  (FNEG_D64:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 113,
        GIR_Done,
      // Label 1013: @36787
      GIM_Try, /*On fail goto*//*Label 1014*/ 36806, // Rule ID 1113 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsNotSoftFloat_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        // (fneg:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs)  =>  (FNEG_D32_MM:{ *:[f64] } AFGR64Opnd:{ *:[f64] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1113,
        GIR_Done,
      // Label 1014: @36806
      GIM_Try, /*On fail goto*//*Label 1015*/ 36825, // Rule ID 1114 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit_IsNotSoftFloat,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        // (fneg:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs)  =>  (FNEG_D64_MM:{ *:[f64] } FGR64Opnd:{ *:[f64] }:$fs)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FNEG_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1114,
        GIR_Done,
      // Label 1015: @36825
      GIM_Reject,
    // Label 1002: @36826
    GIM_Reject,
    // Label 991: @36827
    GIM_Reject,
    // Label 32: @36828
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 3, /*)*//*default:*//*Label 1018*/ 36976,
    /*GILLT_s32*//*Label 1016*/ 36836,
    /*GILLT_s64*//*Label 1017*/ 36860,
    // Label 1016: @36836
    GIM_Try, /*On fail goto*//*Label 1019*/ 36859, // Rule ID 1024 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128F16RegClassID,
      // (fpextend:{ *:[f32] } MSA128F16:{ *:[f16] }:$ws)  =>  (MSA_FP_EXTEND_W_PSEUDO:{ *:[f32] } MSA128F16:{ *:[f16] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MSA_FP_EXTEND_W_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1024,
      GIR_Done,
    // Label 1019: @36859
    GIM_Reject,
    // Label 1017: @36860
    GIM_Try, /*On fail goto*//*Label 1020*/ 36883, // Rule ID 1026 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s16,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128F16RegClassID,
      // (fpextend:{ *:[f64] } MSA128F16:{ *:[f16] }:$ws)  =>  (MSA_FP_EXTEND_D_PSEUDO:{ *:[f64] } MSA128F16:{ *:[f16] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MSA_FP_EXTEND_D_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1026,
      GIR_Done,
    // Label 1020: @36883
    GIM_Try, /*On fail goto*//*Label 1021*/ 36906, // Rule ID 1403 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_NotFP64bit_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (fpextend:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)  =>  (CVT_D32_S:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_D32_S,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1403,
      GIR_Done,
    // Label 1021: @36906
    GIM_Try, /*On fail goto*//*Label 1022*/ 36929, // Rule ID 1413 //
      GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_NotInMicroMips,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (fpextend:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)  =>  (CVT_D64_S:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_D64_S,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1413,
      GIR_Done,
    // Label 1022: @36929
    GIM_Try, /*On fail goto*//*Label 1023*/ 36952, // Rule ID 2156 //
      GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (fpextend:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)  =>  (CVT_D64_S_MM:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_D64_S_MM,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2156,
      GIR_Done,
    // Label 1023: @36952
    GIM_Try, /*On fail goto*//*Label 1024*/ 36975, // Rule ID 2158 //
      GIM_CheckFeatures, GIFBS_InMicroMips_NotFP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (fpextend:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)  =>  (CVT_D32_S_MM:{ *:[f64] } FGR32Opnd:{ *:[f32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_D32_S_MM,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 2158,
      GIR_Done,
    // Label 1024: @36975
    GIM_Reject,
    // Label 1018: @36976
    GIM_Reject,
    // Label 33: @36977
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/0, 2, /*)*//*default:*//*Label 1027*/ 37104,
    /*GILLT_s16*//*Label 1025*/ 36985,
    /*GILLT_s32*//*Label 1026*/ 37032,
    // Label 1025: @36985
    GIM_Try, /*On fail goto*//*Label 1028*/ 37008, // Rule ID 1025 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128F16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR32RegClassID,
      // (fpround:{ *:[f16] } FGR32Opnd:{ *:[f32] }:$fs)  =>  (MSA_FP_ROUND_W_PSEUDO:{ *:[f16] } FGR32Opnd:{ *:[f32] }:$fs)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MSA_FP_ROUND_W_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1025,
      GIR_Done,
    // Label 1028: @37008
    GIM_Try, /*On fail goto*//*Label 1029*/ 37031, // Rule ID 1027 //
      GIM_CheckFeatures, GIFBS_HasMSA,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128F16RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
      // (fpround:{ *:[f16] } FGR64Opnd:{ *:[f64] }:$fs)  =>  (MSA_FP_ROUND_D_PSEUDO:{ *:[f16] } FGR64Opnd:{ *:[f64] }:$fs)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::MSA_FP_ROUND_D_PSEUDO,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1027,
      GIR_Done,
    // Label 1029: @37031
    GIM_Reject,
    // Label 1026: @37032
    GIM_Try, /*On fail goto*//*Label 1030*/ 37103,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 1031*/ 37057, // Rule ID 1402 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        // (fpround:{ *:[f32] } AFGR64Opnd:{ *:[f64] }:$src)  =>  (CVT_S_D32:{ *:[f32] } AFGR64Opnd:{ *:[f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_S_D32,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1402,
        GIR_Done,
      // Label 1031: @37057
      GIM_Try, /*On fail goto*//*Label 1032*/ 37072, // Rule ID 1412 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_IsFP64bit_NotInMicroMips,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        // (fpround:{ *:[f32] } FGR64Opnd:{ *:[f64] }:$src)  =>  (CVT_S_D64:{ *:[f32] } FGR64Opnd:{ *:[f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_S_D64,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1412,
        GIR_Done,
      // Label 1032: @37072
      GIM_Try, /*On fail goto*//*Label 1033*/ 37087, // Rule ID 2155 //
        GIM_CheckFeatures, GIFBS_InMicroMips_IsFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::FGR64RegClassID,
        // (fpround:{ *:[f32] } FGR64Opnd:{ *:[f64] }:$src)  =>  (CVT_S_D64_MM:{ *:[f32] } FGR64Opnd:{ *:[f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_S_D64_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2155,
        GIR_Done,
      // Label 1033: @37087
      GIM_Try, /*On fail goto*//*Label 1034*/ 37102, // Rule ID 2157 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotFP64bit,
        GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::AFGR64RegClassID,
        // (fpround:{ *:[f32] } AFGR64Opnd:{ *:[f64] }:$src)  =>  (CVT_S_D32_MM:{ *:[f32] } AFGR64Opnd:{ *:[f64] }:$src)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::CVT_S_D32_MM,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2157,
        GIR_Done,
      // Label 1034: @37102
      GIM_Reject,
    // Label 1030: @37103
    GIM_Reject,
    // Label 1027: @37104
    GIM_Reject,
    // Label 34: @37105
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 1037*/ 37162,
    /*GILLT_v2s64*//*Label 1035*/ 37114, 0,
    /*GILLT_v4s32*//*Label 1036*/ 37138,
    // Label 1035: @37114
    GIM_Try, /*On fail goto*//*Label 1038*/ 37137, // Rule ID 745 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (fp_to_sint:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FTRUNC_S_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FTRUNC_S_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 745,
      GIR_Done,
    // Label 1038: @37137
    GIM_Reject,
    // Label 1036: @37138
    GIM_Try, /*On fail goto*//*Label 1039*/ 37161, // Rule ID 744 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (fp_to_sint:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FTRUNC_S_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FTRUNC_S_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 744,
      GIR_Done,
    // Label 1039: @37161
    GIM_Reject,
    // Label 1037: @37162
    GIM_Reject,
    // Label 35: @37163
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 1042*/ 37220,
    /*GILLT_v2s64*//*Label 1040*/ 37172, 0,
    /*GILLT_v4s32*//*Label 1041*/ 37196,
    // Label 1040: @37172
    GIM_Try, /*On fail goto*//*Label 1043*/ 37195, // Rule ID 747 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (fp_to_uint:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)  =>  (FTRUNC_U_D:{ *:[v2i64] } MSA128DOpnd:{ *:[v2f64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FTRUNC_U_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 747,
      GIR_Done,
    // Label 1043: @37195
    GIM_Reject,
    // Label 1041: @37196
    GIM_Try, /*On fail goto*//*Label 1044*/ 37219, // Rule ID 746 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (fp_to_uint:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)  =>  (FTRUNC_U_W:{ *:[v4i32] } MSA128WOpnd:{ *:[v4f32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FTRUNC_U_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 746,
      GIR_Done,
    // Label 1044: @37219
    GIM_Reject,
    // Label 1042: @37220
    GIM_Reject,
    // Label 36: @37221
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 7, /*)*//*default:*//*Label 1049*/ 37373,
    /*GILLT_s32*//*Label 1045*/ 37233,
    /*GILLT_s64*//*Label 1046*/ 37255, 0,
    /*GILLT_v2s64*//*Label 1047*/ 37325, 0,
    /*GILLT_v4s32*//*Label 1048*/ 37349,
    // Label 1045: @37233
    GIM_Try, /*On fail goto*//*Label 1050*/ 37254, // Rule ID 1397 //
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (sint_to_fp:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$src)  =>  (PseudoCVT_S_W:{ *:[f32] } GPR32Opnd:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoCVT_S_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1397,
      GIR_Done,
    // Label 1050: @37254
    GIM_Reject,
    // Label 1046: @37255
    GIM_Try, /*On fail goto*//*Label 1051*/ 37278, // Rule ID 1400 //
      GIM_CheckFeatures, GIFBS_NotFP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::AFGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (sint_to_fp:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$src)  =>  (PseudoCVT_D32_W:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoCVT_D32_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1400,
      GIR_Done,
    // Label 1051: @37278
    GIM_Try, /*On fail goto*//*Label 1052*/ 37301, // Rule ID 1406 //
      GIM_CheckFeatures, GIFBS_IsFP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      // (sint_to_fp:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$src)  =>  (PseudoCVT_D64_W:{ *:[f64] } GPR32Opnd:{ *:[i32] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoCVT_D64_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1406,
      GIR_Done,
    // Label 1052: @37301
    GIM_Try, /*On fail goto*//*Label 1053*/ 37324, // Rule ID 1408 //
      GIM_CheckFeatures, GIFBS_IsFP64bit,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::FGR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      // (sint_to_fp:{ *:[f64] } GPR64Opnd:{ *:[i64] }:$src)  =>  (PseudoCVT_D64_L:{ *:[f64] } GPR64Opnd:{ *:[i64] }:$src)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::PseudoCVT_D64_L,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1408,
      GIR_Done,
    // Label 1053: @37324
    GIM_Reject,
    // Label 1047: @37325
    GIM_Try, /*On fail goto*//*Label 1054*/ 37348, // Rule ID 677 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (sint_to_fp:{ *:[v2f64] } MSA128DOpnd:{ *:[v2i64] }:$ws)  =>  (FFINT_S_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2i64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FFINT_S_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 677,
      GIR_Done,
    // Label 1054: @37348
    GIM_Reject,
    // Label 1048: @37349
    GIM_Try, /*On fail goto*//*Label 1055*/ 37372, // Rule ID 676 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (sint_to_fp:{ *:[v4f32] } MSA128WOpnd:{ *:[v4i32] }:$ws)  =>  (FFINT_S_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4i32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FFINT_S_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 676,
      GIR_Done,
    // Label 1055: @37372
    GIM_Reject,
    // Label 1049: @37373
    GIM_Reject,
    // Label 37: @37374
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/4, 7, /*)*//*default:*//*Label 1058*/ 37431,
    /*GILLT_v2s64*//*Label 1056*/ 37383, 0,
    /*GILLT_v4s32*//*Label 1057*/ 37407,
    // Label 1056: @37383
    GIM_Try, /*On fail goto*//*Label 1059*/ 37406, // Rule ID 679 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v2s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128DRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128DRegClassID,
      // (uint_to_fp:{ *:[v2f64] } MSA128DOpnd:{ *:[v2i64] }:$ws)  =>  (FFINT_U_D:{ *:[v2f64] } MSA128DOpnd:{ *:[v2i64] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FFINT_U_D,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 679,
      GIR_Done,
    // Label 1059: @37406
    GIM_Reject,
    // Label 1057: @37407
    GIM_Try, /*On fail goto*//*Label 1060*/ 37430, // Rule ID 678 //
      GIM_CheckFeatures, GIFBS_HasMSA_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_v4s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::MSA128WRegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::MSA128WRegClassID,
      // (uint_to_fp:{ *:[v4f32] } MSA128WOpnd:{ *:[v4i32] }:$ws)  =>  (FFINT_U_W:{ *:[v4f32] } MSA128WOpnd:{ *:[v4i32] }:$ws)
      GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::FFINT_U_W,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 678,
      GIR_Done,
    // Label 1060: @37430
    GIM_Reject,
    // Label 1058: @37431
    GIM_Reject,
    // Label 38: @37432
    GIM_Try, /*On fail goto*//*Label 1061*/ 37516,
      GIM_CheckIsMBB, /*MI*/0, /*Op*/0,
      GIM_Try, /*On fail goto*//*Label 1062*/ 37451, // Rule ID 73 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips_RelocNotPIC,
        // (br (bb:{ *:[Other] }):$target)  =>  (J (bb:{ *:[Other] }):$target)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::J,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::AT,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 73,
        GIR_Done,
      // Label 1062: @37451
      GIM_Try, /*On fail goto*//*Label 1063*/ 37465, // Rule ID 80 //
        GIM_CheckFeatures, GIFBS_HasStdEnc_NotInMicroMips,
        // (br (bb:{ *:[Other] }):$offset)  =>  (B (bb:{ *:[Other] }):$offset)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::B,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::AT,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 80,
        GIR_Done,
      // Label 1063: @37465
      GIM_Try, /*On fail goto*//*Label 1064*/ 37479, // Rule ID 1072 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6_RelocNotPIC,
        // (br (bb:{ *:[Other] }):$target)  =>  (J_MM (bb:{ *:[Other] }):$target)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::J_MM,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::AT,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1072,
        GIR_Done,
      // Label 1064: @37479
      GIM_Try, /*On fail goto*//*Label 1065*/ 37493, // Rule ID 1081 //
        GIM_CheckFeatures, GIFBS_InMicroMips_NotMips32r6_RelocPIC,
        // (br (bb:{ *:[Other] }):$offset)  =>  (B_MM (bb:{ *:[Other] }):$offset)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::B_MM,
        GIR_AddImplicitDef, /*InsnID*/0, Mips::AT,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1081,
        GIR_Done,
      // Label 1065: @37493
      GIM_Try, /*On fail goto*//*Label 1066*/ 37504, // Rule ID 1128 //
        GIM_CheckFeatures, GIFBS_HasMips32r6_InMicroMips,
        // (br (bb:{ *:[Other] }):$offset)  =>  (BC_MMR6 (bb:{ *:[Other] }):$offset)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::BC_MMR6,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1128,
        GIR_Done,
      // Label 1066: @37504
      GIM_Try, /*On fail goto*//*Label 1067*/ 37515, // Rule ID 1776 //
        GIM_CheckFeatures, GIFBS_InMips16Mode,
        // (br (bb:{ *:[Other] }):$imm16)  =>  (Bimm16 (bb:{ *:[Other] }):$imm16)
        GIR_MutateOpcode, /*InsnID*/0, /*RecycleInsnID*/0, /*Opcode*/Mips::Bimm16,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1776,
        GIR_Done,
      // Label 1067: @37515
      GIM_Reject,
    // Label 1061: @37516
    GIM_Reject,
    // Label 39: @37517
    GIM_SwitchType, /*MI*/0, /*Op*/0, /*[*/1, 3, /*)*//*default:*//*Label 1070*/ 37668,
    /*GILLT_s32*//*Label 1068*/ 37525,
    /*GILLT_s64*//*Label 1069*/ 37619,
    // Label 1068: @37525
    GIM_Try, /*On fail goto*//*Label 1071*/ 37618,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR32RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR32RegClassID,
      GIM_Try, /*On fail goto*//*Label 1072*/ 37578, // Rule ID 1383 //
        GIM_CheckFeatures, GIFBS_HasMips32r2_HasStdEnc_NotInMicroMips,
        // (bswap:{ *:[i32] } GPR32:{ *:[i32] }:$rt)  =>  (ROTR:{ *:[i32] } (WSBH:{ *:[i32] } GPR32:{ *:[i32] }:$rt), 16:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::WSBH,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ROTR,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_AddImm, /*InsnID*/0, /*Imm*/16,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 1383,
        GIR_Done,
      // Label 1072: @37578
      GIM_Try, /*On fail goto*//*Label 1073*/ 37617, // Rule ID 2089 //
        GIM_CheckFeatures, GIFBS_InMicroMips,
        // (bswap:{ *:[i32] } GPR32:{ *:[i32] }:$rt)  =>  (ROTR_MM:{ *:[i32] } (WSBH_MM:{ *:[i32] } GPR32:{ *:[i32] }:$rt), 16:{ *:[i32] })
        GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
        GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::WSBH_MM,
        GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
        GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // rt
        GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
        GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::ROTR_MM,
        GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
        GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
        GIR_AddImm, /*InsnID*/0, /*Imm*/16,
        GIR_EraseFromParent, /*InsnID*/0,
        GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
        // GIR_Coverage, 2089,
        GIR_Done,
      // Label 1073: @37617
      GIM_Reject,
    // Label 1071: @37618
    GIM_Reject,
    // Label 1069: @37619
    GIM_Try, /*On fail goto*//*Label 1074*/ 37667, // Rule ID 1525 //
      GIM_CheckFeatures, GIFBS_HasMips64r2_HasStdEnc,
      GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s64,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/Mips::GPR64RegClassID,
      GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/Mips::GPR64RegClassID,
      // (bswap:{ *:[i64] } GPR64:{ *:[i64] }:$rt)  =>  (DSHD:{ *:[i64] } (DSBH:{ *:[i64] } GPR64:{ *:[i64] }:$rt))
      GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s64,
      GIR_BuildMI, /*InsnID*/1, /*Opcode*/Mips::DSBH,
      GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/RegState::Define,
      GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/1, // rt
      GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
      GIR_BuildMI, /*InsnID*/0, /*Opcode*/Mips::DSHD,
      GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // rd
      GIR_AddTempRegister, /*InsnID*/0, /*TempRegID*/0, /*TempRegFlags*/0,
      GIR_EraseFromParent, /*InsnID*/0,
      GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
      // GIR_Coverage, 1525,
      GIR_Done,
    // Label 1074: @37667
    GIM_Reject,
    // Label 1070: @37668
    GIM_Reject,
    // Label 40: @37669
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
computeAvailableModuleFeatures(const MipsSubtarget *Subtarget) const;
PredicateBitset
computeAvailableFunctionFeatures(const MipsSubtarget *Subtarget,
                                 const MachineFunction *MF) const;
#endif // ifdef GET_GLOBALISEL_PREDICATES_DECL
#ifdef GET_GLOBALISEL_PREDICATES_INIT
AvailableModuleFeatures(computeAvailableModuleFeatures(&STI)),
AvailableFunctionFeatures()
#endif // ifdef GET_GLOBALISEL_PREDICATES_INIT
