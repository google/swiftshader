/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Assembly Matcher Source Fragment                                           *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_ASSEMBLER_HEADER
#undef GET_ASSEMBLER_HEADER
  // This should be included into the middle of the declaration of
  // your subclasses implementation of MCTargetAsmParser.
  uint64_t ComputeAvailableFeatures(const FeatureBitset& FB) const;
  void convertToMCInst(unsigned Kind, MCInst &Inst, unsigned Opcode,
                       const OperandVector &Operands);
  void convertToMapAndConstraints(unsigned Kind,
                           const OperandVector &Operands) override;
  unsigned MatchInstructionImpl(const OperandVector &Operands,
                                MCInst &Inst,
                                uint64_t &ErrorInfo,
                                bool matchingInlineAsm,
                                unsigned VariantID = 0);
#endif // GET_ASSEMBLER_HEADER_INFO


#ifdef GET_OPERAND_DIAGNOSTIC_TYPES
#undef GET_OPERAND_DIAGNOSTIC_TYPES

#endif // GET_OPERAND_DIAGNOSTIC_TYPES


#ifdef GET_REGISTER_MATCHER
#undef GET_REGISTER_MATCHER

// Flags for subtarget features that participate in instruction matching.
enum SubtargetFeatureFlag : uint8_t {
  Feature_None = 0
};

#endif // GET_REGISTER_MATCHER


#ifdef GET_SUBTARGET_FEATURE_NAME
#undef GET_SUBTARGET_FEATURE_NAME

// User-level names for subtarget features that participate in
// instruction matching.
static const char *getSubtargetFeatureName(uint64_t Val) {
  return "(unknown)";
}

#endif // GET_SUBTARGET_FEATURE_NAME


#ifdef GET_MATCHER_IMPLEMENTATION
#undef GET_MATCHER_IMPLEMENTATION

static void applyMnemonicAliases(StringRef &Mnemonic, uint64_t Features, unsigned VariantID) {
  switch (VariantID) {
    case 0:
      switch (Mnemonic.size()) {
      default: break;
      case 5:	 // 1 string to match.
        if (memcmp(Mnemonic.data()+0, "cntlz", 5) != 0)
          break;
        Mnemonic = "cntlzw";	 // "cntlz"
        return;
      case 6:	 // 1 string to match.
        if (memcmp(Mnemonic.data()+0, "cntlz.", 6) != 0)
          break;
        Mnemonic = "cntlzw.";	 // "cntlz."
        return;
      }
    break;
  }
  switch (Mnemonic.size()) {
  default: break;
  case 5:	 // 1 string to match.
    if (memcmp(Mnemonic.data()+0, "cntlz", 5) != 0)
      break;
    Mnemonic = "cntlzw";	 // "cntlz"
    return;
  case 6:	 // 1 string to match.
    if (memcmp(Mnemonic.data()+0, "cntlz.", 6) != 0)
      break;
    Mnemonic = "cntlzw.";	 // "cntlz."
    return;
  }
}

enum {
  Tie0_1_1,
};

static const uint8_t TiedAsmOperandTable[][3] = {
  /* Tie0_1_1 */ { 0, 1, 1 },
};

namespace {
enum OperatorConversionKind {
  CVT_Done,
  CVT_Reg,
  CVT_Tied,
  CVT_95_addRegG8RCOperands,
  CVT_95_addTLSRegOperands,
  CVT_95_addRegGPRCOperands,
  CVT_95_addRegGPRCNoR0Operands,
  CVT_95_addS16ImmOperands,
  CVT_95_addImmOperands,
  CVT_95_addU16ImmOperands,
  CVT_95_addBranchTargetOperands,
  CVT_95_addRegCRBITRCOperands,
  CVT_imm_95_3,
  CVT_imm_95_2,
  CVT_imm_95_0,
  CVT_95_addRegVRRCOperands,
  CVT_imm_95_8,
  CVT_imm_95_10,
  CVT_imm_95_76,
  CVT_regCR0,
  CVT_95_addRegCRRCOperands,
  CVT_imm_95_79,
  CVT_imm_95_78,
  CVT_imm_95_4,
  CVT_imm_95_7,
  CVT_imm_95_6,
  CVT_imm_95_44,
  CVT_imm_95_47,
  CVT_imm_95_46,
  CVT_imm_95_36,
  CVT_imm_95_39,
  CVT_imm_95_38,
  CVT_imm_95_12,
  CVT_imm_95_15,
  CVT_imm_95_14,
  CVT_imm_95_68,
  CVT_imm_95_71,
  CVT_imm_95_70,
  CVT_imm_95_100,
  CVT_imm_95_103,
  CVT_imm_95_102,
  CVT_imm_95_108,
  CVT_imm_95_111,
  CVT_imm_95_110,
  CVT_imm_95_31,
  CVT_95_addRegGxRCNoR0Operands,
  CVT_95_addRegGxRCOperands,
  CVT_regR0,
  CVT_95_addRegSPERCOperands,
  CVT_95_addRegSPE4RCOperands,
  CVT_95_addRegF4RCOperands,
  CVT_95_addRegF8RCOperands,
  CVT_imm_95_1,
  CVT_95_addRegVFRCOperands,
  CVT_95_addRegVSFRCOperands,
  CVT_95_addRegVSSRCOperands,
  CVT_95_addRegVSRCOperands,
  CVT_imm_95_29,
  CVT_imm_95_280,
  CVT_imm_95_128,
  CVT_imm_95_129,
  CVT_imm_95_130,
  CVT_imm_95_131,
  CVT_imm_95_132,
  CVT_imm_95_133,
  CVT_imm_95_134,
  CVT_imm_95_135,
  CVT_imm_95_28,
  CVT_imm_95_19,
  CVT_imm_95_537,
  CVT_imm_95_539,
  CVT_imm_95_541,
  CVT_imm_95_543,
  CVT_imm_95_536,
  CVT_imm_95_538,
  CVT_imm_95_540,
  CVT_imm_95_542,
  CVT_imm_95_1018,
  CVT_imm_95_981,
  CVT_imm_95_22,
  CVT_imm_95_17,
  CVT_imm_95_18,
  CVT_imm_95_980,
  CVT_imm_95_529,
  CVT_imm_95_531,
  CVT_imm_95_533,
  CVT_imm_95_535,
  CVT_imm_95_528,
  CVT_imm_95_530,
  CVT_imm_95_532,
  CVT_imm_95_534,
  CVT_imm_95_1019,
  CVT_95_addCRBitMaskOperands,
  CVT_imm_95_48,
  CVT_imm_95_287,
  CVT_imm_95_5,
  CVT_imm_95_25,
  CVT_imm_95_512,
  CVT_imm_95_272,
  CVT_imm_95_273,
  CVT_imm_95_274,
  CVT_imm_95_275,
  CVT_imm_95_260,
  CVT_imm_95_261,
  CVT_imm_95_262,
  CVT_imm_95_263,
  CVT_imm_95_26,
  CVT_imm_95_27,
  CVT_imm_95_990,
  CVT_imm_95_991,
  CVT_imm_95_268,
  CVT_imm_95_988,
  CVT_imm_95_989,
  CVT_imm_95_269,
  CVT_imm_95_986,
  CVT_imm_95_255,
  CVT_imm_95_284,
  CVT_imm_95_285,
  CVT_95_addRegG8RCNoX0Operands,
  CVT_95_addRegQFRCOperands,
  CVT_95_addRegQSRCOperands,
  CVT_95_addRegQBRCOperands,
  CVT_imm_95_9,
  CVT_imm_95_13,
  CVT_imm_95_20,
  CVT_imm_95_16,
  CVT_imm_95_24,
  CVT_NUM_CONVERTERS
};

enum InstructionConversionKind {
  Convert__RegG8RC1_0__RegG8RC1_1__TLSReg1_2,
  Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2,
  Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3,
  Convert__RegGPRC1_0__RegGPRCNoR01_1__S16Imm1_2,
  Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2,
  Convert__RegGPRC1_1__RegGPRC1_2__S16Imm1_3,
  Convert__RegGPRC1_0__RegGPRCNoR01_1__S17Imm1_2,
  Convert__RegGPRC1_0__RegGPRC1_1,
  Convert__RegGPRC1_1__RegGPRC1_2,
  Convert__RegG8RC1_0__Imm1_1,
  Convert__RegGPRC1_1__RegGPRC1_2__U16Imm1_3,
  Convert_NoOperands,
  Convert__DirectBr1_0,
  Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2,
  Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3,
  Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2,
  Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2,
  Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0,
  Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2,
  Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3,
  Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3,
  Convert__RegVRRC1_1__RegVRRC1_2,
  Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3__U1Imm1_4,
  Convert__CondBr1_0,
  Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_0__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_8__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_2__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_10__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_76__regCR0__CondBr1_0,
  Convert__imm_95_76__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_79__regCR0__CondBr1_0,
  Convert__imm_95_79__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_78__regCR0__CondBr1_0,
  Convert__imm_95_78__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_76__regCR0,
  Convert__imm_95_76__RegCRRC1_0,
  Convert__imm_95_79__regCR0,
  Convert__imm_95_79__RegCRRC1_0,
  Convert__imm_95_78__regCR0,
  Convert__imm_95_78__RegCRRC1_0,
  Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_4__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_7__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_6__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_4__regCR0__CondBr1_0,
  Convert__imm_95_4__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_7__regCR0__CondBr1_0,
  Convert__imm_95_7__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_6__regCR0__CondBr1_0,
  Convert__imm_95_6__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_4__regCR0,
  Convert__imm_95_4__RegCRRC1_0,
  Convert__imm_95_7__regCR0,
  Convert__imm_95_7__RegCRRC1_0,
  Convert__imm_95_6__regCR0,
  Convert__imm_95_6__RegCRRC1_0,
  Convert__imm_95_44__regCR0__CondBr1_0,
  Convert__imm_95_44__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_47__regCR0__CondBr1_0,
  Convert__imm_95_47__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_46__regCR0__CondBr1_0,
  Convert__imm_95_46__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_44__regCR0,
  Convert__imm_95_44__RegCRRC1_0,
  Convert__imm_95_47__regCR0,
  Convert__imm_95_47__RegCRRC1_0,
  Convert__imm_95_46__regCR0,
  Convert__imm_95_46__RegCRRC1_0,
  Convert__DirectBr1_0__Imm1_1,
  Convert__imm_95_36__regCR0__CondBr1_0,
  Convert__imm_95_36__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_39__regCR0__CondBr1_0,
  Convert__imm_95_39__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_38__regCR0__CondBr1_0,
  Convert__imm_95_38__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_36__regCR0,
  Convert__imm_95_36__RegCRRC1_0,
  Convert__imm_95_39__regCR0,
  Convert__imm_95_39__RegCRRC1_0,
  Convert__imm_95_38__regCR0,
  Convert__imm_95_38__RegCRRC1_0,
  Convert__imm_95_12__regCR0__CondBr1_0,
  Convert__imm_95_12__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_15__regCR0__CondBr1_0,
  Convert__imm_95_15__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_14__regCR0__CondBr1_0,
  Convert__imm_95_14__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_12__regCR0,
  Convert__imm_95_12__RegCRRC1_0,
  Convert__imm_95_15__regCR0,
  Convert__imm_95_15__RegCRRC1_0,
  Convert__imm_95_14__regCR0,
  Convert__imm_95_14__RegCRRC1_0,
  Convert__imm_95_68__regCR0__CondBr1_0,
  Convert__imm_95_68__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_71__regCR0__CondBr1_0,
  Convert__imm_95_71__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_70__regCR0__CondBr1_0,
  Convert__imm_95_70__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_68__regCR0,
  Convert__imm_95_68__RegCRRC1_0,
  Convert__imm_95_71__regCR0,
  Convert__imm_95_71__RegCRRC1_0,
  Convert__imm_95_70__regCR0,
  Convert__imm_95_70__RegCRRC1_0,
  Convert__imm_95_100__regCR0__CondBr1_0,
  Convert__imm_95_100__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_103__regCR0__CondBr1_0,
  Convert__imm_95_103__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_102__regCR0__CondBr1_0,
  Convert__imm_95_102__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_100__regCR0,
  Convert__imm_95_100__RegCRRC1_0,
  Convert__imm_95_103__regCR0,
  Convert__imm_95_103__RegCRRC1_0,
  Convert__imm_95_102__regCR0,
  Convert__imm_95_102__RegCRRC1_0,
  Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2,
  Convert__imm_95_108__regCR0__CondBr1_0,
  Convert__imm_95_108__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_111__regCR0__CondBr1_0,
  Convert__imm_95_111__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_110__regCR0__CondBr1_0,
  Convert__imm_95_110__RegCRRC1_0__CondBr1_1,
  Convert__imm_95_108__regCR0,
  Convert__imm_95_108__RegCRRC1_0,
  Convert__imm_95_111__regCR0,
  Convert__imm_95_111__RegCRRC1_0,
  Convert__imm_95_110__regCR0,
  Convert__imm_95_110__RegCRRC1_0,
  Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1,
  Convert__imm_95_12__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_15__RegCRBITRC1_0__imm_95_0,
  Convert__imm_95_14__RegCRBITRC1_0__imm_95_0,
  Convert__RegG8RC1_0__RegG8RC1_1__imm_95_0__U6Imm1_2,
  Convert__RegG8RC1_0__RegGPRC1_1__imm_95_0__U6Imm1_2,
  Convert__RegG8RC1_1__RegG8RC1_2__imm_95_0__U6Imm1_3,
  Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3,
  Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4,
  Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3,
  Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4,
  Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0__U5Imm1_2__imm_95_31,
  Convert__RegGPRC1_1__RegGPRC1_2__imm_95_0__U5Imm1_3__imm_95_31,
  Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2,
  Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3,
  Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2,
  Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3,
  Convert__RegCRRC1_0__RegGPRC1_2__RegGPRC1_3,
  Convert__RegCRRC1_0__RegG8RC1_2__RegG8RC1_3,
  Convert__regCR0__RegG8RC1_0__RegG8RC1_1,
  Convert__RegCRRC1_0__RegG8RC1_1__RegG8RC1_2,
  Convert__regCR0__RegG8RC1_0__S16Imm1_1,
  Convert__RegCRRC1_0__RegG8RC1_1__S16Imm1_2,
  Convert__RegCRBITRC1_0__RegG8RC1_1__RegG8RC1_2,
  Convert__RegCRRC1_0__RegGPRC1_2__S16Imm1_3,
  Convert__RegCRRC1_0__RegG8RC1_2__S16Imm1_3,
  Convert__regCR0__RegG8RC1_0__U16Imm1_1,
  Convert__RegCRRC1_0__RegG8RC1_1__U16Imm1_2,
  Convert__RegCRRC1_0__RegGPRC1_2__U16Imm1_3,
  Convert__RegCRRC1_0__RegG8RC1_2__U16Imm1_3,
  Convert__regCR0__RegGPRC1_0__RegGPRC1_1,
  Convert__RegCRRC1_0__RegGPRC1_1__RegGPRC1_2,
  Convert__regCR0__RegGPRC1_0__U16Imm1_1,
  Convert__RegCRRC1_0__RegGPRC1_1__U16Imm1_2,
  Convert__RegCRBITRC1_0__U1Imm1_1__RegG8RC1_2__RegG8RC1_3,
  Convert__regCR0__RegGPRC1_0__S16Imm1_1,
  Convert__RegCRRC1_0__RegGPRC1_1__S16Imm1_2,
  Convert__RegG8RC1_0__RegG8RC1_1,
  Convert__RegG8RC1_1__RegG8RC1_2,
  Convert__RegGPRC1_0__RegGPRC1_1__U1Imm1_2,
  Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2,
  Convert__RegCRBITRC1_0__RegCRBITRC1_0__RegCRBITRC1_0,
  Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_1,
  Convert__RegGxRCNoR01_0__RegGxRC1_1,
  Convert__U5Imm1_2__RegGxRCNoR01_0__RegGxRC1_1,
  Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2,
  Convert__RegGxRCNoR01_1__RegGxRC1_2__U5Imm1_0,
  Convert__regR0__regR0,
  Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3,
  Convert__U5Imm1_0,
  Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1,
  Convert__RegSPERC1_0__RegSPERC1_1,
  Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2,
  Convert__RegSPERC1_0__RegSPE4RC1_1,
  Convert__RegSPERC1_0__RegGPRC1_1,
  Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2,
  Convert__RegGPRC1_0__RegSPERC1_1,
  Convert__RegSPE4RC1_0__RegSPE4RC1_1,
  Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2,
  Convert__RegSPE4RC1_0__RegSPERC1_1,
  Convert__RegSPE4RC1_0__RegGPRC1_1,
  Convert__RegCRRC1_0__RegSPE4RC1_1__RegSPE4RC1_2,
  Convert__RegGPRC1_0__RegSPE4RC1_1,
  Convert__RegSPERC1_0__RegSPERC1_2__U5Imm1_1,
  Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2,
  Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegSPERC1_0__DispSPE21_1__RegGxRCNoR01_2,
  Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2,
  Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2,
  Convert__RegSPERC1_1__RegSPERC1_2__RegSPERC1_3__imm_95_0,
  Convert__RegSPERC1_0__S5Imm1_1,
  Convert__RegSPERC1_0__U5Imm1_1__RegSPERC1_2,
  Convert__RegF4RC1_0__RegF4RC1_1,
  Convert__RegF4RC1_1__RegF4RC1_2,
  Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2,
  Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3,
  Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2,
  Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3,
  Convert__RegF8RC1_0__RegF8RC1_1,
  Convert__RegF8RC1_1__RegF8RC1_2,
  Convert__RegF4RC1_0__RegF8RC1_1,
  Convert__RegF4RC1_1__RegF8RC1_2,
  Convert__RegCRRC1_0__RegF4RC1_1__RegF4RC1_2,
  Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3,
  Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4,
  Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3,
  Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4,
  Convert__RegF4RC1_0__RegF8RC1_1__RegF4RC1_2__RegF4RC1_3,
  Convert__RegF4RC1_1__RegF8RC1_2__RegF4RC1_3__RegF4RC1_4,
  Convert__RegCRRC1_0__RegF8RC1_1__RegF8RC1_2,
  Convert__RegCRRC1_0__RegF8RC1_1,
  Convert__U4Imm1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__U4Imm1_1__RegGxRCNoR01_2__RegGxRC1_3,
  Convert__RegGPRC1_0__RegGPRCNoR01_1__RegGPRC1_2__RegCRBITRC1_3,
  Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2,
  Convert__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2,
  Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2,
  Convert__RegG8RC1_0__imm_95_0__DispRIX1_1__RegGxRCNoR01_2,
  Convert__RegG8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegF8RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegF8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegF4RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegF4RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegGPRC1_0__S16Imm1_1,
  Convert__RegGPRC1_0__S17Imm1_1,
  Convert__RegG8RC1_0__imm_95_0,
  Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__imm_95_1,
  Convert__RegSPE4RC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__RegSPE4RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2,
  Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegVSSRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegVSRC1_0__DispRIX161_1__RegGxRCNoR01_2,
  Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2,
  Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3,
  Convert__imm_95_0,
  Convert__RegCRRC1_0__RegCRRC1_1,
  Convert__RegCRRC1_0,
  Convert__RegGPRC1_0__imm_95_29,
  Convert__RegGPRC1_0__imm_95_280,
  Convert__RegGPRC1_0__U10Imm1_1__imm_95_0,
  Convert__RegGPRC1_0__imm_95_128,
  Convert__RegGPRC1_0__imm_95_129,
  Convert__RegGPRC1_0__imm_95_130,
  Convert__RegGPRC1_0__imm_95_131,
  Convert__RegGPRC1_0__imm_95_132,
  Convert__RegGPRC1_0__imm_95_133,
  Convert__RegGPRC1_0__imm_95_134,
  Convert__RegGPRC1_0__imm_95_135,
  Convert__RegGPRC1_0__imm_95_28,
  Convert__RegGPRC1_0,
  Convert__RegGPRC1_0__imm_95_19,
  Convert__RegGPRC1_0__imm_95_537,
  Convert__RegGPRC1_0__imm_95_539,
  Convert__RegGPRC1_0__imm_95_541,
  Convert__RegGPRC1_0__imm_95_543,
  Convert__RegGPRC1_0__imm_95_536,
  Convert__RegGPRC1_0__imm_95_538,
  Convert__RegGPRC1_0__imm_95_540,
  Convert__RegGPRC1_0__imm_95_542,
  Convert__RegGPRC1_0__imm_95_1018,
  Convert__RegGPRC1_0__Imm1_1,
  Convert__RegGPRC1_0__imm_95_981,
  Convert__RegGPRC1_0__imm_95_22,
  Convert__RegGPRC1_0__imm_95_17,
  Convert__RegGPRC1_0__imm_95_18,
  Convert__RegGPRC1_0__imm_95_980,
  Convert__RegG8RC1_0__RegF8RC1_1,
  Convert__RegF8RC1_0,
  Convert__RegF8RC1_1,
  Convert__RegF8RC1_0__U3Imm1_1,
  Convert__RegF8RC1_0__U2Imm1_1,
  Convert__RegGPRC1_0__imm_95_529,
  Convert__RegGPRC1_0__imm_95_531,
  Convert__RegGPRC1_0__imm_95_533,
  Convert__RegGPRC1_0__imm_95_535,
  Convert__RegGPRC1_0__imm_95_528,
  Convert__RegGPRC1_0__imm_95_530,
  Convert__RegGPRC1_0__imm_95_532,
  Convert__RegGPRC1_0__imm_95_534,
  Convert__RegGPRC1_0__imm_95_1019,
  Convert__RegGPRC1_0__CRBitMask1_1,
  Convert__RegGPRC1_0__imm_95_48,
  Convert__RegGPRC1_0__imm_95_287,
  Convert__RegGPRC1_0__imm_95_5,
  Convert__RegGPRC1_0__imm_95_4,
  Convert__RegGPRC1_0__imm_95_25,
  Convert__RegGPRC1_0__imm_95_512,
  Convert__RegGPRC1_0__imm_95_272,
  Convert__RegGPRC1_0__imm_95_273,
  Convert__RegGPRC1_0__imm_95_274,
  Convert__RegGPRC1_0__imm_95_275,
  Convert__RegGPRC1_0__imm_95_260,
  Convert__RegGPRC1_0__imm_95_261,
  Convert__RegGPRC1_0__imm_95_262,
  Convert__RegGPRC1_0__imm_95_263,
  Convert__RegGPRC1_0__U4Imm1_1,
  Convert__RegGPRC1_0__imm_95_26,
  Convert__RegGPRC1_0__imm_95_27,
  Convert__RegGPRC1_0__imm_95_990,
  Convert__RegGPRC1_0__imm_95_991,
  Convert__RegGPRC1_0__imm_95_268,
  Convert__RegGPRC1_0__imm_95_988,
  Convert__RegGPRC1_0__imm_95_989,
  Convert__RegGPRC1_0__imm_95_269,
  Convert__RegGPRC1_0__imm_95_986,
  Convert__RegG8RC1_0__RegVRRC1_1,
  Convert__RegVRRC1_0,
  Convert__RegG8RC1_0__RegVSFRC1_1,
  Convert__RegG8RC1_0__RegVSRC1_1,
  Convert__RegGPRC1_0__RegVSFRC1_1,
  Convert__RegGPRC1_0__imm_95_1,
  Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_1,
  Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_2,
  Convert__imm_95_29__RegGPRC1_0,
  Convert__imm_95_280__RegGPRC1_0,
  Convert__imm_95_28__RegGPRC1_0,
  Convert__imm_95_255__RegG8RC1_0,
  Convert__Imm1_0__RegGPRC1_1,
  Convert__imm_95_19__RegGPRC1_0,
  Convert__imm_95_537__RegGPRC1_1,
  Convert__imm_95_539__RegGPRC1_1,
  Convert__imm_95_541__RegGPRC1_1,
  Convert__imm_95_543__RegGPRC1_1,
  Convert__imm_95_536__RegGPRC1_1,
  Convert__imm_95_538__RegGPRC1_1,
  Convert__imm_95_540__RegGPRC1_1,
  Convert__imm_95_542__RegGPRC1_1,
  Convert__imm_95_1018__RegGPRC1_0,
  Convert__RegGPRC1_1__Imm1_0,
  Convert__imm_95_981__RegGPRC1_0,
  Convert__imm_95_22__RegGPRC1_0,
  Convert__imm_95_17__RegGPRC1_0,
  Convert__imm_95_18__RegGPRC1_0,
  Convert__imm_95_980__RegGPRC1_0,
  Convert__Imm1_0__RegF8RC1_1__imm_95_0__imm_95_0,
  Convert__Imm1_1__RegF8RC1_2__imm_95_0__imm_95_0,
  Convert__Imm1_0__RegF8RC1_1__Imm1_2__Imm1_3,
  Convert__Imm1_1__RegF8RC1_2__Imm1_3__Imm1_4,
  Convert__RegCRRC1_0__Imm1_1__imm_95_0,
  Convert__RegCRRC1_1__Imm1_2__imm_95_0,
  Convert__RegCRRC1_0__Imm1_1__Imm1_2,
  Convert__RegCRRC1_1__Imm1_2__Imm1_3,
  Convert__imm_95_529__RegGPRC1_1,
  Convert__imm_95_531__RegGPRC1_1,
  Convert__imm_95_533__RegGPRC1_1,
  Convert__imm_95_535__RegGPRC1_1,
  Convert__imm_95_528__RegGPRC1_1,
  Convert__imm_95_530__RegGPRC1_1,
  Convert__imm_95_532__RegGPRC1_1,
  Convert__imm_95_534__RegGPRC1_1,
  Convert__imm_95_1019__RegGPRC1_0,
  Convert__RegGPRC1_0__imm_95_0,
  Convert__CRBitMask1_0__RegGPRC1_1,
  Convert__imm_95_48__RegGPRC1_0,
  Convert__imm_95_25__RegGPRC1_0,
  Convert__imm_95_512__RegGPRC1_0,
  Convert__imm_95_272__RegGPRC1_1,
  Convert__imm_95_273__RegGPRC1_1,
  Convert__imm_95_274__RegGPRC1_1,
  Convert__imm_95_275__RegGPRC1_1,
  Convert__imm_95_260__RegGPRC1_1,
  Convert__imm_95_261__RegGPRC1_1,
  Convert__imm_95_262__RegGPRC1_1,
  Convert__imm_95_263__RegGPRC1_1,
  Convert__imm_95_272__RegGPRC1_0,
  Convert__imm_95_273__RegGPRC1_0,
  Convert__imm_95_274__RegGPRC1_0,
  Convert__imm_95_275__RegGPRC1_0,
  Convert__imm_95_260__RegGPRC1_0,
  Convert__imm_95_261__RegGPRC1_0,
  Convert__imm_95_262__RegGPRC1_0,
  Convert__imm_95_263__RegGPRC1_0,
  Convert__RegGPRC1_1__U4Imm1_0,
  Convert__imm_95_26__RegGPRC1_0,
  Convert__imm_95_27__RegGPRC1_0,
  Convert__imm_95_990__RegGPRC1_0,
  Convert__imm_95_991__RegGPRC1_0,
  Convert__imm_95_988__RegGPRC1_0,
  Convert__imm_95_284__RegGPRC1_0,
  Convert__imm_95_989__RegGPRC1_0,
  Convert__imm_95_285__RegGPRC1_0,
  Convert__imm_95_986__RegGPRC1_0,
  Convert__RegVSFRC1_0__RegG8RC1_1,
  Convert__RegVSRC1_0__RegG8RCNoX01_1__RegG8RC1_2,
  Convert__RegVSFRC1_0__RegGPRC1_1,
  Convert__RegVSRC1_0__RegGPRC1_1,
  Convert__imm_95_1__RegGPRC1_0,
  Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2,
  Convert__RegGPRC1_1__RegGPRC1_2__U1Imm1_3,
  Convert__imm_95_2,
  Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__U2Imm1_3,
  Convert__RegQFRC1_0__RegQFRC1_1__U2Imm1_2,
  Convert__RegQFRC1_0__RegQFRC1_1,
  Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2,
  Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_2,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_1,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_4,
  Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_0,
  Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_5,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_9,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__U12Imm1_3,
  Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2,
  Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_14,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_8,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_10,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_7,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_13,
  Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__RegQFRC1_3,
  Convert__RegQSRC1_0__RegQFRC1_1,
  Convert__RegQFRC1_0__RegQBRC1_1__RegQFRC1_3__RegQFRC1_2,
  Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_15,
  Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_6,
  Convert__RegQFRC1_0__U12Imm1_1,
  Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegQFRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegQSRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__imm_95_0__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__imm_95_0__RegQSRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__U1Imm1_0,
  Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__U6Imm1_3,
  Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__U6Imm1_4,
  Convert__RegG8RC1_0__Tie0_1_1__RegG8RC1_1__U6Imm1_2__U6Imm1_3,
  Convert__RegG8RC1_1__Tie0_1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4,
  Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2__Imm1_3,
  Convert__RegG8RC1_1__RegG8RC1_2__U5Imm1_3__Imm1_4,
  Convert__RegGPRC1_0__Tie0_1_1__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4,
  Convert__RegGPRC1_1__Tie0_1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5,
  Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4,
  Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5,
  Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4,
  Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__U5Imm1_4__U5Imm1_5,
  Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__imm_95_0,
  Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__imm_95_0,
  Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__imm_95_0,
  Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__imm_95_0,
  Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__imm_95_0__imm_95_31,
  Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__imm_95_0__imm_95_31,
  Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__imm_95_0__imm_95_31,
  Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__imm_95_0__imm_95_31,
  Convert__Imm1_0,
  Convert__RegG8RC1_0__RegCRRC1_1,
  Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2,
  Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3,
  Convert__RegGPRC1_1__RegGxRCNoR01_2__RegGxRC1_3,
  Convert__imm_95_0__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__imm_95_0__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegG8RC1_1__RegGxRCNoR01_2__RegGxRC1_3,
  Convert__imm_95_0__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2,
  Convert__imm_95_0__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__imm_95_0__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__imm_95_0__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__imm_95_0__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2,
  Convert__imm_95_0__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2,
  Convert__RegG8RC1_0__RegG8RC1_2__RegG8RC1_1,
  Convert__RegG8RC1_1__RegG8RC1_3__RegG8RC1_2,
  Convert__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_0__RegGPRC1_1,
  Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__RegGPRC1_3,
  Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__U5Imm1_3,
  Convert__imm_95_0__U1Imm1_1,
  Convert__U5Imm1_0__RegG8RC1_1__RegG8RC1_2,
  Convert__imm_95_4__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_4__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_12__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_12__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_8__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_8__RegG8RC1_0__S16Imm1_1,
  Convert__U5Imm1_0__RegG8RC1_1__S16Imm1_2,
  Convert__imm_95_20__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_20__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_5__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_5__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_1__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_1__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_6__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_6__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_2__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_2__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_16__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_16__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_24__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_24__RegG8RC1_0__S16Imm1_1,
  Convert__imm_95_31__RegG8RC1_0__RegG8RC1_1,
  Convert__imm_95_31__RegG8RC1_0__S16Imm1_1,
  Convert__regR0__RegGPRC1_0,
  Convert__RegGPRC1_1__RegGPRC1_0,
  Convert__RegGPRC1_0__RegGPRC1_1__Imm1_2,
  Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0,
  Convert__RegGPRC1_0__RegGPRC1_1__imm_95_1,
  Convert__U5Imm1_0__RegGPRC1_1__RegGPRC1_2,
  Convert__imm_95_4__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_4__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_12__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_12__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_8__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_8__RegGPRC1_0__S16Imm1_1,
  Convert__U5Imm1_0__RegGPRC1_1__S16Imm1_2,
  Convert__imm_95_20__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_20__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_5__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_5__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_1__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_1__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_6__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_6__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_2__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_2__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_16__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_16__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_24__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_24__RegGPRC1_0__S16Imm1_1,
  Convert__imm_95_31__RegGPRC1_0__RegGPRC1_1,
  Convert__imm_95_31__RegGPRC1_0__S16Imm1_1,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3,
  Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1,
  Convert__RegVRRC1_0__RegVRRC1_1,
  Convert__RegGPRC1_0__RegVRRC1_1,
  Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1,
  Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2,
  Convert__RegVRRC1_0__Tie0_1_1__U4Imm1_2__RegVRRC1_1,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_1,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__Tie0_1_1,
  Convert__RegVRRC1_0__RegVRRC1_1__U1Imm1_2__U4Imm1_3,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__U4Imm1_3,
  Convert__RegVRRC1_0__S5Imm1_1,
  Convert__regR0__regR0__imm_95_0,
  Convert__RegVSFRC1_0__RegVSFRC1_1,
  Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2,
  Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2,
  Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2,
  Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2,
  Convert__RegCRRC1_0__RegVRRC1_1__RegVRRC1_2,
  Convert__RegVRRC1_0__RegVFRC1_1,
  Convert__RegVSRC1_0__RegVSSRC1_1,
  Convert__RegVFRC1_0__RegVRRC1_1,
  Convert__RegVSSRC1_0__RegVSRC1_1,
  Convert__RegVSSRC1_0__RegVSFRC1_1,
  Convert__RegVSRC1_0__RegG8RC1_1__RegG8RC1_2,
  Convert__RegVRRC1_0__RegVRRC1_1__RegVSFRC1_2,
  Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2,
  Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2,
  Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2,
  Convert__RegVSSRC1_0__RegVSSRC1_1,
  Convert__RegVRRC1_1__U1Imm1_0__RegVRRC1_2__U2Imm1_3,
  Convert__RegCRRC1_0__RegVSFRC1_1,
  Convert__RegCRRC1_0__U7Imm1_2__RegVSFRC1_1,
  Convert__RegCRRC1_0__U7Imm1_2__RegVRRC1_1,
  Convert__RegVSRC1_0__RegVSRC1_1,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2,
  Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3,
  Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1,
  Convert__RegCRRC1_0__RegVSRC1_1__RegVSRC1_2,
  Convert__RegCRRC1_0__RegVSRC1_1,
  Convert__RegVSRC1_0__U7Imm1_2__RegVSRC1_1,
  Convert__RegVSFRC1_0__RegVSRC1_1__U4Imm1_2,
  Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__U4Imm1_2,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_0,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_3,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__U2Imm1_3,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3,
  Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_0,
  Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_3,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_0,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_3,
  Convert__RegVSRC1_0__U8Imm1_1,
  Convert__RegVSRC1_0__RegVSRC1_1__U2Imm1_2,
  Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_2,
  Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_2,
  CVT_NUM_SIGNATURES
};

} // end anonymous namespace

static const uint8_t ConversionTable[CVT_NUM_SIGNATURES][13] = {
  // Convert__RegG8RC1_0__RegG8RC1_1__TLSReg1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addTLSRegOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRCNoR01_1__S16Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCNoR0Operands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__S16Imm1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addS16ImmOperands, 4, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRCNoR01_1__S17Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCNoR0Operands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__Imm1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U16Imm1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addU16ImmOperands, 4, CVT_Done },
  // Convert_NoOperands
  { CVT_Done },
  // Convert__DirectBr1_0
  { CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addBranchTargetOperands, 3, CVT_Done },
  // Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3
  { CVT_95_addImmOperands, 2, CVT_95_addImmOperands, 1, CVT_95_addRegCRBITRCOperands, 3, CVT_95_addBranchTargetOperands, 4, CVT_Done },
  // Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2
  { CVT_95_addImmOperands, 1, CVT_imm_95_3, 0, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addBranchTargetOperands, 3, CVT_Done },
  // Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2
  { CVT_95_addImmOperands, 1, CVT_imm_95_2, 0, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addBranchTargetOperands, 3, CVT_Done },
  // Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0
  { CVT_95_addImmOperands, 1, CVT_95_addRegCRBITRCOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3
  { CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3
  { CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_95_addRegVRRCOperands, 4, CVT_Done },
  // Convert__RegVRRC1_1__RegVRRC1_2
  { CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3__U1Imm1_4
  { CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_95_addRegVRRCOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__CondBr1_0
  { CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_0, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_0__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_0, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_8, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_8__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_8, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_2, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_2__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_2, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_10, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_10__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_10, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_76__regCR0__CondBr1_0
  { CVT_imm_95_76, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_76__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_76, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_79__regCR0__CondBr1_0
  { CVT_imm_95_79, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_79__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_79, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_78__regCR0__CondBr1_0
  { CVT_imm_95_78, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_78__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_78, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_76__regCR0
  { CVT_imm_95_76, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_76__RegCRRC1_0
  { CVT_imm_95_76, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_79__regCR0
  { CVT_imm_95_79, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_79__RegCRRC1_0
  { CVT_imm_95_79, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_78__regCR0
  { CVT_imm_95_78, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_78__RegCRRC1_0
  { CVT_imm_95_78, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_4, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_7, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_6, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_4__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_4, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_7__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_7, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_6__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_6, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_4__regCR0__CondBr1_0
  { CVT_imm_95_4, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_4__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_4, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_7__regCR0__CondBr1_0
  { CVT_imm_95_7, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_7__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_7, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_6__regCR0__CondBr1_0
  { CVT_imm_95_6, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_6__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_6, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_4__regCR0
  { CVT_imm_95_4, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_4__RegCRRC1_0
  { CVT_imm_95_4, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_7__regCR0
  { CVT_imm_95_7, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_7__RegCRRC1_0
  { CVT_imm_95_7, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_6__regCR0
  { CVT_imm_95_6, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_6__RegCRRC1_0
  { CVT_imm_95_6, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_44__regCR0__CondBr1_0
  { CVT_imm_95_44, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_44__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_44, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_47__regCR0__CondBr1_0
  { CVT_imm_95_47, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_47__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_47, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_46__regCR0__CondBr1_0
  { CVT_imm_95_46, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_46__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_46, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_44__regCR0
  { CVT_imm_95_44, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_44__RegCRRC1_0
  { CVT_imm_95_44, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_47__regCR0
  { CVT_imm_95_47, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_47__RegCRRC1_0
  { CVT_imm_95_47, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_46__regCR0
  { CVT_imm_95_46, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_46__RegCRRC1_0
  { CVT_imm_95_46, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__DirectBr1_0__Imm1_1
  { CVT_95_addBranchTargetOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__imm_95_36__regCR0__CondBr1_0
  { CVT_imm_95_36, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_36__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_36, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_39__regCR0__CondBr1_0
  { CVT_imm_95_39, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_39__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_39, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_38__regCR0__CondBr1_0
  { CVT_imm_95_38, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_38__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_38, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_36__regCR0
  { CVT_imm_95_36, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_36__RegCRRC1_0
  { CVT_imm_95_36, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_39__regCR0
  { CVT_imm_95_39, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_39__RegCRRC1_0
  { CVT_imm_95_39, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_38__regCR0
  { CVT_imm_95_38, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_38__RegCRRC1_0
  { CVT_imm_95_38, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_12__regCR0__CondBr1_0
  { CVT_imm_95_12, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_12__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_12, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_15__regCR0__CondBr1_0
  { CVT_imm_95_15, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_15__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_15, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_14__regCR0__CondBr1_0
  { CVT_imm_95_14, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_14__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_14, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_12__regCR0
  { CVT_imm_95_12, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_12__RegCRRC1_0
  { CVT_imm_95_12, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_15__regCR0
  { CVT_imm_95_15, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_15__RegCRRC1_0
  { CVT_imm_95_15, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_14__regCR0
  { CVT_imm_95_14, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_14__RegCRRC1_0
  { CVT_imm_95_14, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_68__regCR0__CondBr1_0
  { CVT_imm_95_68, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_68__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_68, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_71__regCR0__CondBr1_0
  { CVT_imm_95_71, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_71__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_71, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_70__regCR0__CondBr1_0
  { CVT_imm_95_70, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_70__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_70, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_68__regCR0
  { CVT_imm_95_68, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_68__RegCRRC1_0
  { CVT_imm_95_68, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_71__regCR0
  { CVT_imm_95_71, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_71__RegCRRC1_0
  { CVT_imm_95_71, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_70__regCR0
  { CVT_imm_95_70, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_70__RegCRRC1_0
  { CVT_imm_95_70, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_100__regCR0__CondBr1_0
  { CVT_imm_95_100, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_100__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_100, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_103__regCR0__CondBr1_0
  { CVT_imm_95_103, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_103__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_103, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_102__regCR0__CondBr1_0
  { CVT_imm_95_102, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_102__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_102, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_100__regCR0
  { CVT_imm_95_100, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_100__RegCRRC1_0
  { CVT_imm_95_100, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_103__regCR0
  { CVT_imm_95_103, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_103__RegCRRC1_0
  { CVT_imm_95_103, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_102__regCR0
  { CVT_imm_95_102, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_102__RegCRRC1_0
  { CVT_imm_95_102, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__imm_95_108__regCR0__CondBr1_0
  { CVT_imm_95_108, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_108__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_108, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_111__regCR0__CondBr1_0
  { CVT_imm_95_111, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_111__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_111, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_110__regCR0__CondBr1_0
  { CVT_imm_95_110, 0, CVT_regCR0, 0, CVT_95_addBranchTargetOperands, 1, CVT_Done },
  // Convert__imm_95_110__RegCRRC1_0__CondBr1_1
  { CVT_imm_95_110, 0, CVT_95_addRegCRRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_108__regCR0
  { CVT_imm_95_108, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_108__RegCRRC1_0
  { CVT_imm_95_108, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_111__regCR0
  { CVT_imm_95_111, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_111__RegCRRC1_0
  { CVT_imm_95_111, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_110__regCR0
  { CVT_imm_95_110, 0, CVT_regCR0, 0, CVT_Done },
  // Convert__imm_95_110__RegCRRC1_0
  { CVT_imm_95_110, 0, CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_12, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_15, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1
  { CVT_imm_95_14, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addBranchTargetOperands, 2, CVT_Done },
  // Convert__imm_95_12__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_12, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_15__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_15, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__imm_95_14__RegCRBITRC1_0__imm_95_0
  { CVT_imm_95_14, 0, CVT_95_addRegCRBITRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__imm_95_0__U6Imm1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_imm_95_0, 0, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegGPRC1_1__imm_95_0__U6Imm1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_imm_95_0, 0, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__imm_95_0__U6Imm1_3
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_imm_95_0, 0, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0__U5Imm1_2__imm_95_31
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_imm_95_0, 0, CVT_95_addImmOperands, 3, CVT_imm_95_31, 0, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__imm_95_0__U5Imm1_3__imm_95_31
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_imm_95_0, 0, CVT_95_addImmOperands, 4, CVT_imm_95_31, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_2__RegGPRC1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_2__RegG8RC1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 4, CVT_Done },
  // Convert__regCR0__RegG8RC1_0__RegG8RC1_1
  { CVT_regCR0, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__regCR0__RegG8RC1_0__S16Imm1_1
  { CVT_regCR0, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_1__S16Imm1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__RegCRBITRC1_0__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addRegCRBITRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_2__S16Imm1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 3, CVT_95_addS16ImmOperands, 4, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_2__S16Imm1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 3, CVT_95_addS16ImmOperands, 4, CVT_Done },
  // Convert__regCR0__RegG8RC1_0__U16Imm1_1
  { CVT_regCR0, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addU16ImmOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_1__U16Imm1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addU16ImmOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_2__U16Imm1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 3, CVT_95_addU16ImmOperands, 4, CVT_Done },
  // Convert__RegCRRC1_0__RegG8RC1_2__U16Imm1_3
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegG8RCOperands, 3, CVT_95_addU16ImmOperands, 4, CVT_Done },
  // Convert__regCR0__RegGPRC1_0__RegGPRC1_1
  { CVT_regCR0, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_1__RegGPRC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_Done },
  // Convert__regCR0__RegGPRC1_0__U16Imm1_1
  { CVT_regCR0, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addU16ImmOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_1__U16Imm1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addU16ImmOperands, 3, CVT_Done },
  // Convert__RegCRBITRC1_0__U1Imm1_1__RegG8RC1_2__RegG8RC1_3
  { CVT_95_addRegCRBITRCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 4, CVT_Done },
  // Convert__regCR0__RegGPRC1_0__S16Imm1_1
  { CVT_regCR0, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegGPRC1_1__S16Imm1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U1Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2
  { CVT_95_addRegCRBITRCOperands, 1, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addRegCRBITRCOperands, 3, CVT_Done },
  // Convert__RegCRBITRC1_0__RegCRBITRC1_0__RegCRBITRC1_0
  { CVT_95_addRegCRBITRCOperands, 1, CVT_95_addRegCRBITRCOperands, 1, CVT_95_addRegCRBITRCOperands, 1, CVT_Done },
  // Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_1
  { CVT_95_addRegCRBITRCOperands, 1, CVT_95_addRegCRBITRCOperands, 2, CVT_95_addRegCRBITRCOperands, 2, CVT_Done },
  // Convert__RegGxRCNoR01_0__RegGxRC1_1
  { CVT_95_addRegGxRCNoR0Operands, 1, CVT_95_addRegGxRCOperands, 2, CVT_Done },
  // Convert__U5Imm1_2__RegGxRCNoR01_0__RegGxRC1_1
  { CVT_95_addImmOperands, 3, CVT_95_addRegGxRCNoR0Operands, 1, CVT_95_addRegGxRCOperands, 2, CVT_Done },
  // Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2
  { CVT_95_addRegGxRCNoR0Operands, 1, CVT_95_addRegGxRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegGxRCNoR01_1__RegGxRC1_2__U5Imm1_0
  { CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__regR0__regR0
  { CVT_regR0, 0, CVT_regR0, 0, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 4, CVT_Done },
  // Convert__U5Imm1_0
  { CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1
  { CVT_95_addImmOperands, 3, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__RegSPERC1_1
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_95_addRegSPERCOperands, 3, CVT_Done },
  // Convert__RegSPERC1_0__RegSPE4RC1_1
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegSPE4RCOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__RegGPRC1_1
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_95_addRegSPERCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegSPERC1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_Done },
  // Convert__RegSPE4RC1_0__RegSPE4RC1_1
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addRegSPE4RCOperands, 2, CVT_Done },
  // Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addRegSPE4RCOperands, 2, CVT_95_addRegSPE4RCOperands, 3, CVT_Done },
  // Convert__RegSPE4RC1_0__RegSPERC1_1
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_Done },
  // Convert__RegSPE4RC1_0__RegGPRC1_1
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegSPE4RC1_1__RegSPE4RC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegSPE4RCOperands, 2, CVT_95_addRegSPE4RCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegSPE4RC1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegSPE4RCOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__RegSPERC1_2__U5Imm1_1
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegSPERCOperands, 3, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegSPERC1_0__DispSPE21_1__RegGxRCNoR01_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addRegSPERCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegSPERC1_1__RegSPERC1_2__RegSPERC1_3__imm_95_0
  { CVT_95_addRegSPERCOperands, 2, CVT_95_addRegSPERCOperands, 3, CVT_95_addRegSPERCOperands, 4, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegSPERC1_0__S5Imm1_1
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegSPERC1_0__U5Imm1_1__RegSPERC1_2
  { CVT_95_addRegSPERCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegSPERCOperands, 3, CVT_Done },
  // Convert__RegF4RC1_0__RegF4RC1_1
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegF4RCOperands, 2, CVT_Done },
  // Convert__RegF4RC1_1__RegF4RC1_2
  { CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_Done },
  // Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_Done },
  // Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3
  { CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_95_addRegF8RCOperands, 4, CVT_Done },
  // Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_Done },
  // Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3
  { CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_95_addRegF4RCOperands, 4, CVT_Done },
  // Convert__RegF8RC1_0__RegF8RC1_1
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_Done },
  // Convert__RegF8RC1_1__RegF8RC1_2
  { CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_Done },
  // Convert__RegF4RC1_0__RegF8RC1_1
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_Done },
  // Convert__RegF4RC1_1__RegF8RC1_2
  { CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegF4RC1_1__RegF4RC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_Done },
  // Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_95_addRegF8RCOperands, 4, CVT_Done },
  // Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4
  { CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_95_addRegF8RCOperands, 4, CVT_95_addRegF8RCOperands, 5, CVT_Done },
  // Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_95_addRegF4RCOperands, 4, CVT_Done },
  // Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4
  { CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_95_addRegF4RCOperands, 4, CVT_95_addRegF4RCOperands, 5, CVT_Done },
  // Convert__RegF4RC1_0__RegF8RC1_1__RegF4RC1_2__RegF4RC1_3
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF4RCOperands, 3, CVT_95_addRegF4RCOperands, 4, CVT_Done },
  // Convert__RegF4RC1_1__RegF8RC1_2__RegF4RC1_3__RegF4RC1_4
  { CVT_95_addRegF4RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_95_addRegF4RCOperands, 4, CVT_95_addRegF4RCOperands, 5, CVT_Done },
  // Convert__RegCRRC1_0__RegF8RC1_1__RegF8RC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegF8RC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_Done },
  // Convert__U4Imm1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__U4Imm1_1__RegGxRCNoR01_2__RegGxRC1_3
  { CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_95_addRegGxRCOperands, 4, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRCNoR01_1__RegGPRC1_2__RegCRBITRC1_3
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCNoR0Operands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegCRBITRCOperands, 4, CVT_Done },
  // Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_0, 0, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addTLSRegOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__imm_95_0__DispRIX1_1__RegGxRCNoR01_2
  { CVT_95_addRegG8RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegG8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegF8RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegF8RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegF8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegF8RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegF4RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegF4RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegF4RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegF4RCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegF4RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__S16Imm1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__S17Imm1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__RegG8RC1_0__imm_95_0
  { CVT_95_addRegG8RCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__imm_95_1
  { CVT_imm_95_1, 0, CVT_Done },
  // Convert__RegSPE4RC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegSPE4RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegSPE4RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2
  { CVT_95_addRegVFRCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegVSSRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegVSSRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__DispRIX161_1__RegGxRCNoR01_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 4, CVT_Done },
  // Convert__imm_95_0
  { CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegCRRC1_0__RegCRRC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegCRRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0
  { CVT_95_addRegCRRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_29
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_29, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_280
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_280, 0, CVT_Done },
  // Convert__RegGPRC1_0__U10Imm1_1__imm_95_0
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addImmOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_128
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_128, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_129
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_129, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_130
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_130, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_131
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_131, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_132
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_132, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_133
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_133, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_134
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_134, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_135
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_135, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_28
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_28, 0, CVT_Done },
  // Convert__RegGPRC1_0
  { CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_19
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_19, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_537
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_537, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_539
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_539, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_541
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_541, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_543
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_543, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_536
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_536, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_538
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_538, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_540
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_540, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_542
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_542, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_1018
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_1018, 0, CVT_Done },
  // Convert__RegGPRC1_0__Imm1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_981
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_981, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_22
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_22, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_17
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_17, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_18
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_18, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_980
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_980, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegF8RC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_Done },
  // Convert__RegF8RC1_0
  { CVT_95_addRegF8RCOperands, 1, CVT_Done },
  // Convert__RegF8RC1_1
  { CVT_95_addRegF8RCOperands, 2, CVT_Done },
  // Convert__RegF8RC1_0__U3Imm1_1
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegF8RC1_0__U2Imm1_1
  { CVT_95_addRegF8RCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_529
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_529, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_531
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_531, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_533
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_533, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_535
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_535, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_528
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_528, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_530
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_530, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_532
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_532, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_534
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_534, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_1019
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_1019, 0, CVT_Done },
  // Convert__RegGPRC1_0__CRBitMask1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addCRBitMaskOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_48
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_48, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_287
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_287, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_5
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_5, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_4
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_4, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_25
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_25, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_512
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_512, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_272
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_272, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_273
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_273, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_274
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_274, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_275
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_275, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_260
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_260, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_261
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_261, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_262
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_262, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_263
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_263, 0, CVT_Done },
  // Convert__RegGPRC1_0__U4Imm1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_26
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_26, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_27
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_27, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_990
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_990, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_991
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_991, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_268
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_268, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_988
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_988, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_989
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_989, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_269
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_269, 0, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_986
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_986, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegVRRC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0
  { CVT_95_addRegVRRCOperands, 1, CVT_Done },
  // Convert__RegG8RC1_0__RegVSFRC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_0__RegVSRC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__RegVSFRC1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_1
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_1, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_2
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__imm_95_29__RegGPRC1_0
  { CVT_imm_95_29, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_280__RegGPRC1_0
  { CVT_imm_95_280, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_28__RegGPRC1_0
  { CVT_imm_95_28, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_255__RegG8RC1_0
  { CVT_imm_95_255, 0, CVT_95_addRegG8RCOperands, 1, CVT_Done },
  // Convert__Imm1_0__RegGPRC1_1
  { CVT_95_addImmOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_19__RegGPRC1_0
  { CVT_imm_95_19, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_537__RegGPRC1_1
  { CVT_imm_95_537, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_539__RegGPRC1_1
  { CVT_imm_95_539, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_541__RegGPRC1_1
  { CVT_imm_95_541, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_543__RegGPRC1_1
  { CVT_imm_95_543, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_536__RegGPRC1_1
  { CVT_imm_95_536, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_538__RegGPRC1_1
  { CVT_imm_95_538, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_540__RegGPRC1_1
  { CVT_imm_95_540, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_542__RegGPRC1_1
  { CVT_imm_95_542, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_1018__RegGPRC1_0
  { CVT_imm_95_1018, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_1__Imm1_0
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__imm_95_981__RegGPRC1_0
  { CVT_imm_95_981, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_22__RegGPRC1_0
  { CVT_imm_95_22, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_17__RegGPRC1_0
  { CVT_imm_95_17, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_18__RegGPRC1_0
  { CVT_imm_95_18, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_980__RegGPRC1_0
  { CVT_imm_95_980, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__Imm1_0__RegF8RC1_1__imm_95_0__imm_95_0
  { CVT_95_addImmOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_imm_95_0, 0, CVT_imm_95_0, 0, CVT_Done },
  // Convert__Imm1_1__RegF8RC1_2__imm_95_0__imm_95_0
  { CVT_95_addImmOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_imm_95_0, 0, CVT_imm_95_0, 0, CVT_Done },
  // Convert__Imm1_0__RegF8RC1_1__Imm1_2__Imm1_3
  { CVT_95_addImmOperands, 1, CVT_95_addRegF8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__Imm1_1__RegF8RC1_2__Imm1_3__Imm1_4
  { CVT_95_addImmOperands, 2, CVT_95_addRegF8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegCRRC1_0__Imm1_1__imm_95_0
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addImmOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegCRRC1_1__Imm1_2__imm_95_0
  { CVT_95_addRegCRRCOperands, 2, CVT_95_addImmOperands, 3, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegCRRC1_0__Imm1_1__Imm1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegCRRC1_1__Imm1_2__Imm1_3
  { CVT_95_addRegCRRCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__imm_95_529__RegGPRC1_1
  { CVT_imm_95_529, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_531__RegGPRC1_1
  { CVT_imm_95_531, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_533__RegGPRC1_1
  { CVT_imm_95_533, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_535__RegGPRC1_1
  { CVT_imm_95_535, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_528__RegGPRC1_1
  { CVT_imm_95_528, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_530__RegGPRC1_1
  { CVT_imm_95_530, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_532__RegGPRC1_1
  { CVT_imm_95_532, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_534__RegGPRC1_1
  { CVT_imm_95_534, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_1019__RegGPRC1_0
  { CVT_imm_95_1019, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_0__imm_95_0
  { CVT_95_addRegGPRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__CRBitMask1_0__RegGPRC1_1
  { CVT_95_addCRBitMaskOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_48__RegGPRC1_0
  { CVT_imm_95_48, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_25__RegGPRC1_0
  { CVT_imm_95_25, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_512__RegGPRC1_0
  { CVT_imm_95_512, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_272__RegGPRC1_1
  { CVT_imm_95_272, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_273__RegGPRC1_1
  { CVT_imm_95_273, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_274__RegGPRC1_1
  { CVT_imm_95_274, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_275__RegGPRC1_1
  { CVT_imm_95_275, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_260__RegGPRC1_1
  { CVT_imm_95_260, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_261__RegGPRC1_1
  { CVT_imm_95_261, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_262__RegGPRC1_1
  { CVT_imm_95_262, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_263__RegGPRC1_1
  { CVT_imm_95_263, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_272__RegGPRC1_0
  { CVT_imm_95_272, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_273__RegGPRC1_0
  { CVT_imm_95_273, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_274__RegGPRC1_0
  { CVT_imm_95_274, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_275__RegGPRC1_0
  { CVT_imm_95_275, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_260__RegGPRC1_0
  { CVT_imm_95_260, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_261__RegGPRC1_0
  { CVT_imm_95_261, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_262__RegGPRC1_0
  { CVT_imm_95_262, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_263__RegGPRC1_0
  { CVT_imm_95_263, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_1__U4Imm1_0
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__imm_95_26__RegGPRC1_0
  { CVT_imm_95_26, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_27__RegGPRC1_0
  { CVT_imm_95_27, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_990__RegGPRC1_0
  { CVT_imm_95_990, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_991__RegGPRC1_0
  { CVT_imm_95_991, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_988__RegGPRC1_0
  { CVT_imm_95_988, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_284__RegGPRC1_0
  { CVT_imm_95_284, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_989__RegGPRC1_0
  { CVT_imm_95_989, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_285__RegGPRC1_0
  { CVT_imm_95_285, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__imm_95_986__RegGPRC1_0
  { CVT_imm_95_986, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegVSFRC1_0__RegG8RC1_1
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegG8RCNoX01_1__RegG8RC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegG8RCNoX0Operands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegVSFRC1_0__RegGPRC1_1
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegGPRC1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_1__RegGPRC1_0
  { CVT_imm_95_1, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addU16ImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U1Imm1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__imm_95_2
  { CVT_imm_95_2, 0, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__U2Imm1_3
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addRegQFRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1__U2Imm1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addRegQFRCOperands, 3, CVT_Done },
  // Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_2
  { CVT_95_addRegQSRCOperands, 1, CVT_95_addRegQSRCOperands, 2, CVT_95_addRegQSRCOperands, 3, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_1
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_1, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_4
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_4, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_0
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 1, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addRegQFRCOperands, 3, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_5
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 2, CVT_imm_95_5, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_9
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_9, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__U12Imm1_3
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addRegQFRCOperands, 4, CVT_95_addRegQFRCOperands, 3, CVT_Done },
  // Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2
  { CVT_95_addRegQSRCOperands, 1, CVT_95_addRegQSRCOperands, 2, CVT_95_addRegQSRCOperands, 4, CVT_95_addRegQSRCOperands, 3, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_14
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_14, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_8
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_8, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_10
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 2, CVT_imm_95_10, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_7
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_7, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_13
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_13, 0, CVT_Done },
  // Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__RegQFRC1_3
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_95_addRegQFRCOperands, 3, CVT_95_addRegQFRCOperands, 4, CVT_Done },
  // Convert__RegQSRC1_0__RegQFRC1_1
  { CVT_95_addRegQSRCOperands, 1, CVT_95_addRegQFRCOperands, 2, CVT_Done },
  // Convert__RegQFRC1_0__RegQBRC1_1__RegQFRC1_3__RegQFRC1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQFRCOperands, 4, CVT_95_addRegQFRCOperands, 3, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_15
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 1, CVT_imm_95_15, 0, CVT_Done },
  // Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_6
  { CVT_95_addRegQBRCOperands, 1, CVT_95_addRegQBRCOperands, 2, CVT_95_addRegQBRCOperands, 3, CVT_imm_95_6, 0, CVT_Done },
  // Convert__RegQFRC1_0__U12Imm1_1
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegQFRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegQFRCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegQSRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_95_addRegQSRCOperands, 1, CVT_imm_95_0, 0, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__imm_95_0__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegQFRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__imm_95_0__RegQSRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegQSRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__U1Imm1_0
  { CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__U6Imm1_3
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__U6Imm1_4
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegG8RC1_0__Tie0_1_1__RegG8RC1_1__U6Imm1_2__U6Imm1_3
  { CVT_95_addRegG8RCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegG8RC1_1__Tie0_1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4
  { CVT_95_addRegG8RCOperands, 2, CVT_Tied, Tie0_1_1, CVT_95_addRegG8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2__Imm1_3
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__U5Imm1_3__Imm1_4
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_0__Tie0_1_1__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4
  { CVT_95_addRegGPRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_1__Tie0_1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5
  { CVT_95_addRegGPRCOperands, 2, CVT_Tied, Tie0_1_1, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_95_addImmOperands, 6, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_95_addImmOperands, 6, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_95_addImmOperands, 5, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__U5Imm1_4__U5Imm1_5
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_95_addImmOperands, 5, CVT_95_addImmOperands, 6, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__imm_95_0
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__imm_95_0
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__imm_95_0
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addImmOperands, 3, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__imm_95_0
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addImmOperands, 4, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__imm_95_0__imm_95_31
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_imm_95_0, 0, CVT_imm_95_31, 0, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__imm_95_0__imm_95_31
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_imm_95_0, 0, CVT_imm_95_31, 0, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__imm_95_0__imm_95_31
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_imm_95_0, 0, CVT_imm_95_31, 0, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__imm_95_0__imm_95_31
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_imm_95_0, 0, CVT_imm_95_31, 0, CVT_Done },
  // Convert__Imm1_0
  { CVT_95_addImmOperands, 1, CVT_Done },
  // Convert__RegG8RC1_0__RegCRRC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegCRRCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_Done },
  // Convert__RegGPRC1_1__RegGxRCNoR01_2__RegGxRC1_3
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_95_addRegGxRCOperands, 4, CVT_Done },
  // Convert__imm_95_0__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_imm_95_0, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__imm_95_0__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_1__RegGxRCNoR01_2__RegGxRC1_3
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_95_addRegGxRCOperands, 4, CVT_Done },
  // Convert__imm_95_0__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2
  { CVT_imm_95_0, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__imm_95_0__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__imm_95_0__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_imm_95_0, 0, CVT_95_addRegF8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__imm_95_0__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegF8RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__imm_95_0__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2
  { CVT_imm_95_0, 0, CVT_95_addRegF4RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_95_addRegGxRCNoR0Operands, 3, CVT_Done },
  // Convert__imm_95_0__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2
  { CVT_imm_95_0, 0, CVT_95_addRegF4RCOperands, 1, CVT_95_addRegGxRCNoR0Operands, 2, CVT_95_addRegGxRCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_2__RegG8RC1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 3, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_1__RegG8RC1_3__RegG8RC1_2
  { CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 4, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegG8RC1_0__S16Imm1_1
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_0__RegGPRC1_1
  { CVT_imm_95_0, 0, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__RegGPRC1_3
  { CVT_imm_95_0, 0, CVT_95_addImmOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addRegGPRCOperands, 4, CVT_Done },
  // Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__U5Imm1_3
  { CVT_imm_95_0, 0, CVT_95_addImmOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__imm_95_0__U1Imm1_1
  { CVT_imm_95_0, 0, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__U5Imm1_0__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__imm_95_4__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_4, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_4__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_4, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_12__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_12, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_12__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_12, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_8__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_8, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_8__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_8, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__U5Imm1_0__RegG8RC1_1__S16Imm1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__imm_95_20__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_20, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_20__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_20, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_5__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_5, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_5__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_5, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_1__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_1, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_1__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_1, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_6__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_6, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_6__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_6, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_2__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_2, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_2__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_2, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_16__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_16, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_16__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_16, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_24__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_24, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_24__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_24, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_31__RegG8RC1_0__RegG8RC1_1
  { CVT_imm_95_31, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_Done },
  // Convert__imm_95_31__RegG8RC1_0__S16Imm1_1
  { CVT_imm_95_31, 0, CVT_95_addRegG8RCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__regR0__RegGPRC1_0
  { CVT_regR0, 0, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_1__RegGPRC1_0
  { CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 1, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__Imm1_2
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegGPRC1_0__RegGPRC1_1__imm_95_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_imm_95_1, 0, CVT_Done },
  // Convert__U5Imm1_0__RegGPRC1_1__RegGPRC1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addRegGPRCOperands, 3, CVT_Done },
  // Convert__imm_95_4__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_4, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_4__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_4, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_12__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_12, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_12__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_12, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_8__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_8, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_8__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_8, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__U5Imm1_0__RegGPRC1_1__S16Imm1_2
  { CVT_95_addImmOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_95_addS16ImmOperands, 3, CVT_Done },
  // Convert__imm_95_20__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_20, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_20__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_20, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_5__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_5, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_5__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_5, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_1__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_1, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_1__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_1, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_6__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_6, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_6__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_6, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_2__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_2, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_2__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_2, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_16__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_16, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_16__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_16, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_24__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_24, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_24__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_24, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__imm_95_31__RegGPRC1_0__RegGPRC1_1
  { CVT_imm_95_31, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addRegGPRCOperands, 2, CVT_Done },
  // Convert__imm_95_31__RegGPRC1_0__S16Imm1_1
  { CVT_imm_95_31, 0, CVT_95_addRegGPRCOperands, 1, CVT_95_addS16ImmOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_95_addRegVRRCOperands, 4, CVT_Done },
  // Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addImmOperands, 3, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegGPRC1_0__RegVRRC1_1
  { CVT_95_addRegGPRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addImmOperands, 3, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2
  { CVT_95_addRegG8RCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_0__Tie0_1_1__U4Imm1_2__RegVRRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addImmOperands, 3, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__Tie0_1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Tied, Tie0_1_1, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__U1Imm1_2__U4Imm1_3
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addImmOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__U4Imm1_3
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegVRRC1_0__S5Imm1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__regR0__regR0__imm_95_0
  { CVT_regR0, 0, CVT_regR0, 0, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegVSFRC1_0__RegVSFRC1_1
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_95_addRegVSFRCOperands, 3, CVT_Done },
  // Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2
  { CVT_95_addRegVSSRCOperands, 1, CVT_95_addRegVSSRCOperands, 2, CVT_95_addRegVSSRCOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_95_addRegVSFRCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_95_addRegVSFRCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegVRRC1_1__RegVRRC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_0__RegVFRC1_1
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVFRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegVSSRC1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSSRCOperands, 2, CVT_Done },
  // Convert__RegVFRC1_0__RegVRRC1_1
  { CVT_95_addRegVFRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVSSRC1_0__RegVSRC1_1
  { CVT_95_addRegVSSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegVSSRC1_0__RegVSFRC1_1
  { CVT_95_addRegVSSRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegG8RC1_1__RegG8RC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegG8RCOperands, 2, CVT_95_addRegG8RCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_0__RegVRRC1_1__RegVSFRC1_2
  { CVT_95_addRegVRRCOperands, 1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVSFRCOperands, 3, CVT_Done },
  // Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2
  { CVT_95_addRegVSFRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegVSFRCOperands, 2, CVT_95_addRegVSFRCOperands, 3, CVT_Done },
  // Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2
  { CVT_95_addRegVSSRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegVSSRCOperands, 2, CVT_95_addRegVSSRCOperands, 3, CVT_Done },
  // Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2
  { CVT_95_addRegVRRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegVRRCOperands, 2, CVT_95_addRegVRRCOperands, 3, CVT_Done },
  // Convert__RegVSSRC1_0__RegVSSRC1_1
  { CVT_95_addRegVSSRCOperands, 1, CVT_95_addRegVSSRCOperands, 2, CVT_Done },
  // Convert__RegVRRC1_1__U1Imm1_0__RegVRRC1_2__U2Imm1_3
  { CVT_95_addRegVRRCOperands, 2, CVT_95_addImmOperands, 1, CVT_95_addRegVRRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegCRRC1_0__RegVSFRC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__U7Imm1_2__RegVSFRC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addImmOperands, 3, CVT_95_addRegVSFRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__U7Imm1_2__RegVRRC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addImmOperands, 3, CVT_95_addRegVRRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_Done },
  // Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3
  { CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_95_addRegVSRCOperands, 4, CVT_Done },
  // Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegCRRC1_0__RegVSRC1_1__RegVSRC1_2
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_Done },
  // Convert__RegCRRC1_0__RegVSRC1_1
  { CVT_95_addRegCRRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__U7Imm1_2__RegVSRC1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addImmOperands, 3, CVT_95_addRegVSRCOperands, 2, CVT_Done },
  // Convert__RegVSFRC1_0__RegVSRC1_1__U4Imm1_2
  { CVT_95_addRegVSFRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__U4Imm1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_Tied, Tie0_1_1, CVT_95_addRegVSRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_0
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_3
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_imm_95_3, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__U2Imm1_3
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_95_addImmOperands, 4, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 3, CVT_95_addRegVSRCOperands, 4, CVT_Done },
  // Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_0
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_3
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_imm_95_3, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_0
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 2, CVT_imm_95_0, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_3
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 2, CVT_imm_95_3, 0, CVT_Done },
  // Convert__RegVSRC1_0__U8Imm1_1
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addImmOperands, 2, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__U2Imm1_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addImmOperands, 3, CVT_Done },
  // Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSFRCOperands, 2, CVT_imm_95_2, 0, CVT_Done },
  // Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_2
  { CVT_95_addRegVSRCOperands, 1, CVT_95_addRegVSRCOperands, 2, CVT_95_addRegVSRCOperands, 2, CVT_imm_95_2, 0, CVT_Done },
};

void PPCAsmParser::
convertToMCInst(unsigned Kind, MCInst &Inst, unsigned Opcode,
                const OperandVector &Operands) {
  assert(Kind < CVT_NUM_SIGNATURES && "Invalid signature!");
  const uint8_t *Converter = ConversionTable[Kind];
  unsigned OpIdx;
  Inst.setOpcode(Opcode);
  for (const uint8_t *p = Converter; *p; p+= 2) {
    OpIdx = *(p + 1);
    switch (*p) {
    default: llvm_unreachable("invalid conversion entry!");
    case CVT_Reg:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegOperands(Inst, 1);
      break;
    case CVT_Tied: {
      assert(OpIdx < (size_t)(std::end(TiedAsmOperandTable) -
                          std::begin(TiedAsmOperandTable)) &&
             "Tied operand not found");
      unsigned TiedResOpnd = TiedAsmOperandTable[OpIdx][0];
      if (TiedResOpnd != (uint8_t) -1)
        Inst.addOperand(Inst.getOperand(TiedResOpnd));
      break;
    }
    case CVT_95_addRegG8RCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegG8RCOperands(Inst, 1);
      break;
    case CVT_95_addTLSRegOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addTLSRegOperands(Inst, 1);
      break;
    case CVT_95_addRegGPRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegGPRCOperands(Inst, 1);
      break;
    case CVT_95_addRegGPRCNoR0Operands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegGPRCNoR0Operands(Inst, 1);
      break;
    case CVT_95_addS16ImmOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addS16ImmOperands(Inst, 1);
      break;
    case CVT_95_addImmOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addImmOperands(Inst, 1);
      break;
    case CVT_95_addU16ImmOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addU16ImmOperands(Inst, 1);
      break;
    case CVT_95_addBranchTargetOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addBranchTargetOperands(Inst, 1);
      break;
    case CVT_95_addRegCRBITRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegCRBITRCOperands(Inst, 1);
      break;
    case CVT_imm_95_3:
      Inst.addOperand(MCOperand::createImm(3));
      break;
    case CVT_imm_95_2:
      Inst.addOperand(MCOperand::createImm(2));
      break;
    case CVT_imm_95_0:
      Inst.addOperand(MCOperand::createImm(0));
      break;
    case CVT_95_addRegVRRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegVRRCOperands(Inst, 1);
      break;
    case CVT_imm_95_8:
      Inst.addOperand(MCOperand::createImm(8));
      break;
    case CVT_imm_95_10:
      Inst.addOperand(MCOperand::createImm(10));
      break;
    case CVT_imm_95_76:
      Inst.addOperand(MCOperand::createImm(76));
      break;
    case CVT_regCR0:
      Inst.addOperand(MCOperand::createReg(PPC::CR0));
      break;
    case CVT_95_addRegCRRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegCRRCOperands(Inst, 1);
      break;
    case CVT_imm_95_79:
      Inst.addOperand(MCOperand::createImm(79));
      break;
    case CVT_imm_95_78:
      Inst.addOperand(MCOperand::createImm(78));
      break;
    case CVT_imm_95_4:
      Inst.addOperand(MCOperand::createImm(4));
      break;
    case CVT_imm_95_7:
      Inst.addOperand(MCOperand::createImm(7));
      break;
    case CVT_imm_95_6:
      Inst.addOperand(MCOperand::createImm(6));
      break;
    case CVT_imm_95_44:
      Inst.addOperand(MCOperand::createImm(44));
      break;
    case CVT_imm_95_47:
      Inst.addOperand(MCOperand::createImm(47));
      break;
    case CVT_imm_95_46:
      Inst.addOperand(MCOperand::createImm(46));
      break;
    case CVT_imm_95_36:
      Inst.addOperand(MCOperand::createImm(36));
      break;
    case CVT_imm_95_39:
      Inst.addOperand(MCOperand::createImm(39));
      break;
    case CVT_imm_95_38:
      Inst.addOperand(MCOperand::createImm(38));
      break;
    case CVT_imm_95_12:
      Inst.addOperand(MCOperand::createImm(12));
      break;
    case CVT_imm_95_15:
      Inst.addOperand(MCOperand::createImm(15));
      break;
    case CVT_imm_95_14:
      Inst.addOperand(MCOperand::createImm(14));
      break;
    case CVT_imm_95_68:
      Inst.addOperand(MCOperand::createImm(68));
      break;
    case CVT_imm_95_71:
      Inst.addOperand(MCOperand::createImm(71));
      break;
    case CVT_imm_95_70:
      Inst.addOperand(MCOperand::createImm(70));
      break;
    case CVT_imm_95_100:
      Inst.addOperand(MCOperand::createImm(100));
      break;
    case CVT_imm_95_103:
      Inst.addOperand(MCOperand::createImm(103));
      break;
    case CVT_imm_95_102:
      Inst.addOperand(MCOperand::createImm(102));
      break;
    case CVT_imm_95_108:
      Inst.addOperand(MCOperand::createImm(108));
      break;
    case CVT_imm_95_111:
      Inst.addOperand(MCOperand::createImm(111));
      break;
    case CVT_imm_95_110:
      Inst.addOperand(MCOperand::createImm(110));
      break;
    case CVT_imm_95_31:
      Inst.addOperand(MCOperand::createImm(31));
      break;
    case CVT_95_addRegGxRCNoR0Operands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegGxRCNoR0Operands(Inst, 1);
      break;
    case CVT_95_addRegGxRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegGxRCOperands(Inst, 1);
      break;
    case CVT_regR0:
      Inst.addOperand(MCOperand::createReg(PPC::R0));
      break;
    case CVT_95_addRegSPERCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegSPERCOperands(Inst, 1);
      break;
    case CVT_95_addRegSPE4RCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegSPE4RCOperands(Inst, 1);
      break;
    case CVT_95_addRegF4RCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegF4RCOperands(Inst, 1);
      break;
    case CVT_95_addRegF8RCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegF8RCOperands(Inst, 1);
      break;
    case CVT_imm_95_1:
      Inst.addOperand(MCOperand::createImm(1));
      break;
    case CVT_95_addRegVFRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegVFRCOperands(Inst, 1);
      break;
    case CVT_95_addRegVSFRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegVSFRCOperands(Inst, 1);
      break;
    case CVT_95_addRegVSSRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegVSSRCOperands(Inst, 1);
      break;
    case CVT_95_addRegVSRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegVSRCOperands(Inst, 1);
      break;
    case CVT_imm_95_29:
      Inst.addOperand(MCOperand::createImm(29));
      break;
    case CVT_imm_95_280:
      Inst.addOperand(MCOperand::createImm(280));
      break;
    case CVT_imm_95_128:
      Inst.addOperand(MCOperand::createImm(128));
      break;
    case CVT_imm_95_129:
      Inst.addOperand(MCOperand::createImm(129));
      break;
    case CVT_imm_95_130:
      Inst.addOperand(MCOperand::createImm(130));
      break;
    case CVT_imm_95_131:
      Inst.addOperand(MCOperand::createImm(131));
      break;
    case CVT_imm_95_132:
      Inst.addOperand(MCOperand::createImm(132));
      break;
    case CVT_imm_95_133:
      Inst.addOperand(MCOperand::createImm(133));
      break;
    case CVT_imm_95_134:
      Inst.addOperand(MCOperand::createImm(134));
      break;
    case CVT_imm_95_135:
      Inst.addOperand(MCOperand::createImm(135));
      break;
    case CVT_imm_95_28:
      Inst.addOperand(MCOperand::createImm(28));
      break;
    case CVT_imm_95_19:
      Inst.addOperand(MCOperand::createImm(19));
      break;
    case CVT_imm_95_537:
      Inst.addOperand(MCOperand::createImm(537));
      break;
    case CVT_imm_95_539:
      Inst.addOperand(MCOperand::createImm(539));
      break;
    case CVT_imm_95_541:
      Inst.addOperand(MCOperand::createImm(541));
      break;
    case CVT_imm_95_543:
      Inst.addOperand(MCOperand::createImm(543));
      break;
    case CVT_imm_95_536:
      Inst.addOperand(MCOperand::createImm(536));
      break;
    case CVT_imm_95_538:
      Inst.addOperand(MCOperand::createImm(538));
      break;
    case CVT_imm_95_540:
      Inst.addOperand(MCOperand::createImm(540));
      break;
    case CVT_imm_95_542:
      Inst.addOperand(MCOperand::createImm(542));
      break;
    case CVT_imm_95_1018:
      Inst.addOperand(MCOperand::createImm(1018));
      break;
    case CVT_imm_95_981:
      Inst.addOperand(MCOperand::createImm(981));
      break;
    case CVT_imm_95_22:
      Inst.addOperand(MCOperand::createImm(22));
      break;
    case CVT_imm_95_17:
      Inst.addOperand(MCOperand::createImm(17));
      break;
    case CVT_imm_95_18:
      Inst.addOperand(MCOperand::createImm(18));
      break;
    case CVT_imm_95_980:
      Inst.addOperand(MCOperand::createImm(980));
      break;
    case CVT_imm_95_529:
      Inst.addOperand(MCOperand::createImm(529));
      break;
    case CVT_imm_95_531:
      Inst.addOperand(MCOperand::createImm(531));
      break;
    case CVT_imm_95_533:
      Inst.addOperand(MCOperand::createImm(533));
      break;
    case CVT_imm_95_535:
      Inst.addOperand(MCOperand::createImm(535));
      break;
    case CVT_imm_95_528:
      Inst.addOperand(MCOperand::createImm(528));
      break;
    case CVT_imm_95_530:
      Inst.addOperand(MCOperand::createImm(530));
      break;
    case CVT_imm_95_532:
      Inst.addOperand(MCOperand::createImm(532));
      break;
    case CVT_imm_95_534:
      Inst.addOperand(MCOperand::createImm(534));
      break;
    case CVT_imm_95_1019:
      Inst.addOperand(MCOperand::createImm(1019));
      break;
    case CVT_95_addCRBitMaskOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addCRBitMaskOperands(Inst, 1);
      break;
    case CVT_imm_95_48:
      Inst.addOperand(MCOperand::createImm(48));
      break;
    case CVT_imm_95_287:
      Inst.addOperand(MCOperand::createImm(287));
      break;
    case CVT_imm_95_5:
      Inst.addOperand(MCOperand::createImm(5));
      break;
    case CVT_imm_95_25:
      Inst.addOperand(MCOperand::createImm(25));
      break;
    case CVT_imm_95_512:
      Inst.addOperand(MCOperand::createImm(512));
      break;
    case CVT_imm_95_272:
      Inst.addOperand(MCOperand::createImm(272));
      break;
    case CVT_imm_95_273:
      Inst.addOperand(MCOperand::createImm(273));
      break;
    case CVT_imm_95_274:
      Inst.addOperand(MCOperand::createImm(274));
      break;
    case CVT_imm_95_275:
      Inst.addOperand(MCOperand::createImm(275));
      break;
    case CVT_imm_95_260:
      Inst.addOperand(MCOperand::createImm(260));
      break;
    case CVT_imm_95_261:
      Inst.addOperand(MCOperand::createImm(261));
      break;
    case CVT_imm_95_262:
      Inst.addOperand(MCOperand::createImm(262));
      break;
    case CVT_imm_95_263:
      Inst.addOperand(MCOperand::createImm(263));
      break;
    case CVT_imm_95_26:
      Inst.addOperand(MCOperand::createImm(26));
      break;
    case CVT_imm_95_27:
      Inst.addOperand(MCOperand::createImm(27));
      break;
    case CVT_imm_95_990:
      Inst.addOperand(MCOperand::createImm(990));
      break;
    case CVT_imm_95_991:
      Inst.addOperand(MCOperand::createImm(991));
      break;
    case CVT_imm_95_268:
      Inst.addOperand(MCOperand::createImm(268));
      break;
    case CVT_imm_95_988:
      Inst.addOperand(MCOperand::createImm(988));
      break;
    case CVT_imm_95_989:
      Inst.addOperand(MCOperand::createImm(989));
      break;
    case CVT_imm_95_269:
      Inst.addOperand(MCOperand::createImm(269));
      break;
    case CVT_imm_95_986:
      Inst.addOperand(MCOperand::createImm(986));
      break;
    case CVT_imm_95_255:
      Inst.addOperand(MCOperand::createImm(255));
      break;
    case CVT_imm_95_284:
      Inst.addOperand(MCOperand::createImm(284));
      break;
    case CVT_imm_95_285:
      Inst.addOperand(MCOperand::createImm(285));
      break;
    case CVT_95_addRegG8RCNoX0Operands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegG8RCNoX0Operands(Inst, 1);
      break;
    case CVT_95_addRegQFRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegQFRCOperands(Inst, 1);
      break;
    case CVT_95_addRegQSRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegQSRCOperands(Inst, 1);
      break;
    case CVT_95_addRegQBRCOperands:
      static_cast<PPCOperand&>(*Operands[OpIdx]).addRegQBRCOperands(Inst, 1);
      break;
    case CVT_imm_95_9:
      Inst.addOperand(MCOperand::createImm(9));
      break;
    case CVT_imm_95_13:
      Inst.addOperand(MCOperand::createImm(13));
      break;
    case CVT_imm_95_20:
      Inst.addOperand(MCOperand::createImm(20));
      break;
    case CVT_imm_95_16:
      Inst.addOperand(MCOperand::createImm(16));
      break;
    case CVT_imm_95_24:
      Inst.addOperand(MCOperand::createImm(24));
      break;
    }
  }
}

void PPCAsmParser::
convertToMapAndConstraints(unsigned Kind,
                           const OperandVector &Operands) {
  assert(Kind < CVT_NUM_SIGNATURES && "Invalid signature!");
  unsigned NumMCOperands = 0;
  const uint8_t *Converter = ConversionTable[Kind];
  for (const uint8_t *p = Converter; *p; p+= 2) {
    switch (*p) {
    default: llvm_unreachable("invalid conversion entry!");
    case CVT_Reg:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("r");
      ++NumMCOperands;
      break;
    case CVT_Tied:
      ++NumMCOperands;
      break;
    case CVT_95_addRegG8RCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addTLSRegOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegGPRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegGPRCNoR0Operands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addS16ImmOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addImmOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addU16ImmOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addBranchTargetOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegCRBITRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_3:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_2:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_0:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_95_addRegVRRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_8:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_10:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_76:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_regCR0:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      ++NumMCOperands;
      break;
    case CVT_95_addRegCRRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_79:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_78:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_4:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_7:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_6:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_44:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_47:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_46:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_36:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_39:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_38:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_12:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_15:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_14:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_68:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_71:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_70:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_100:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_103:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_102:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_108:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_111:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_110:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_31:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_95_addRegGxRCNoR0Operands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegGxRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_regR0:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      ++NumMCOperands;
      break;
    case CVT_95_addRegSPERCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegSPE4RCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegF4RCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegF8RCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_1:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_95_addRegVFRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegVSFRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegVSSRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegVSRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_29:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_280:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_128:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_129:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_130:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_131:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_132:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_133:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_134:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_135:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_28:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_19:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_537:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_539:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_541:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_543:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_536:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_538:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_540:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_542:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_1018:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_981:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_22:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_17:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_18:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_980:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_529:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_531:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_533:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_535:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_528:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_530:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_532:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_534:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_1019:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_95_addCRBitMaskOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_48:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_287:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_5:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_25:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_512:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_272:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_273:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_274:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_275:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_260:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_261:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_262:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_263:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_26:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_27:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_990:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_991:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_268:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_988:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_989:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_269:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_986:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_255:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_284:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_285:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_95_addRegG8RCNoX0Operands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegQFRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegQSRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_95_addRegQBRCOperands:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("m");
      NumMCOperands += 1;
      break;
    case CVT_imm_95_9:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_13:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_20:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_16:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    case CVT_imm_95_24:
      Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
      Operands[*(p + 1)]->setConstraint("");
      ++NumMCOperands;
      break;
    }
  }
}

namespace {

/// MatchClassKind - The kinds of classes which participate in
/// instruction matching.
enum MatchClassKind {
  InvalidMatchClass = 0,
  OptionalMatchClass = 1,
  MCK__DOT_, // '.'
  MCK_0, // '0'
  MCK_1, // '1'
  MCK_2, // '2'
  MCK_3, // '3'
  MCK_4, // '4'
  MCK_5, // '5'
  MCK_6, // '6'
  MCK_7, // '7'
  MCK_crD, // 'crD'
  MCK_LAST_TOKEN = MCK_crD,
  MCK_CRRC0, // register class 'CRRC0'
  MCK_CTRRC, // register class 'CTRRC'
  MCK_CTRRC8, // register class 'CTRRC8'
  MCK_VRSAVERC, // register class 'VRSAVERC'
  MCK_CARRYRC, // register class 'CARRYRC'
  MCK_CRRC, // register class 'CRRC'
  MCK_Reg21, // derived register class
  MCK_Reg15, // derived register class
  MCK_Reg9, // derived register class
  MCK_Reg17, // derived register class
  MCK_Reg13, // derived register class
  MCK_Reg8, // derived register class
  MCK_CRBITRC, // register class 'CRBITRC'
  MCK_F4RC, // register class 'F4RC,F8RC'
  MCK_QSRC, // register class 'QSRC,QBRC,QFRC'
  MCK_SPERC, // register class 'SPERC'
  MCK_VFRC, // register class 'VFRC'
  MCK_VRRC, // register class 'VRRC'
  MCK_VSLRC, // register class 'VSLRC'
  MCK_Reg6, // derived register class
  MCK_Reg2, // derived register class
  MCK_Reg20, // derived register class
  MCK_Reg12, // derived register class
  MCK_G8RC, // register class 'G8RC'
  MCK_G8RC_NOX0, // register class 'G8RC_NOX0'
  MCK_GPRC, // register class 'GPRC,SPE4RC'
  MCK_GPRC_NOR0, // register class 'GPRC_NOR0'
  MCK_VSRC, // register class 'VSRC'
  MCK_VSSRC, // register class 'VSSRC,VSFRC'
  MCK_SPILLTOVSRRC, // register class 'SPILLTOVSRRC'
  MCK_LAST_REGISTER = MCK_SPILLTOVSRRC,
  MCK_Imm, // user defined class 'ImmAsmOperand'
  MCK_ATBitsAsHint, // user defined class 'PPCATBitsAsHintAsmOperand'
  MCK_CRBitMask, // user defined class 'PPCCRBitMaskOperand'
  MCK_CondBr, // user defined class 'PPCCondBrAsmOperand'
  MCK_DirectBr, // user defined class 'PPCDirectBrAsmOperand'
  MCK_DispRI, // user defined class 'PPCDispRIOperand'
  MCK_DispRIX16, // user defined class 'PPCDispRIX16Operand'
  MCK_DispRIX, // user defined class 'PPCDispRIXOperand'
  MCK_DispSPE2, // user defined class 'PPCDispSPE2Operand'
  MCK_DispSPE4, // user defined class 'PPCDispSPE4Operand'
  MCK_DispSPE8, // user defined class 'PPCDispSPE8Operand'
  MCK_RegCRBITRC, // user defined class 'PPCRegCRBITRCAsmOperand'
  MCK_RegCRRC, // user defined class 'PPCRegCRRCAsmOperand'
  MCK_RegF4RC, // user defined class 'PPCRegF4RCAsmOperand'
  MCK_RegF8RC, // user defined class 'PPCRegF8RCAsmOperand'
  MCK_RegG8RC, // user defined class 'PPCRegG8RCAsmOperand'
  MCK_RegG8RCNoX0, // user defined class 'PPCRegG8RCNoX0AsmOperand'
  MCK_RegGPRC, // user defined class 'PPCRegGPRCAsmOperand'
  MCK_RegGPRCNoR0, // user defined class 'PPCRegGPRCNoR0AsmOperand'
  MCK_RegGxRCNoR0, // user defined class 'PPCRegGxRCNoR0Operand'
  MCK_RegGxRC, // user defined class 'PPCRegGxRCOperand'
  MCK_RegQBRC, // user defined class 'PPCRegQBRCAsmOperand'
  MCK_RegQFRC, // user defined class 'PPCRegQFRCAsmOperand'
  MCK_RegQSRC, // user defined class 'PPCRegQSRCAsmOperand'
  MCK_RegSPE4RC, // user defined class 'PPCRegSPE4RCAsmOperand'
  MCK_RegSPERC, // user defined class 'PPCRegSPERCAsmOperand'
  MCK_RegSPILLTOVSRRC, // user defined class 'PPCRegSPILLTOVSRRCAsmOperand'
  MCK_RegVFRC, // user defined class 'PPCRegVFRCAsmOperand'
  MCK_RegVRRC, // user defined class 'PPCRegVRRCAsmOperand'
  MCK_RegVSFRC, // user defined class 'PPCRegVSFRCAsmOperand'
  MCK_RegVSRC, // user defined class 'PPCRegVSRCAsmOperand'
  MCK_RegVSSRC, // user defined class 'PPCRegVSSRCAsmOperand'
  MCK_S16Imm, // user defined class 'PPCS16ImmAsmOperand'
  MCK_S17Imm, // user defined class 'PPCS17ImmAsmOperand'
  MCK_S5Imm, // user defined class 'PPCS5ImmAsmOperand'
  MCK_TLSReg, // user defined class 'PPCTLSRegOperand'
  MCK_U10Imm, // user defined class 'PPCU10ImmAsmOperand'
  MCK_U12Imm, // user defined class 'PPCU12ImmAsmOperand'
  MCK_U16Imm, // user defined class 'PPCU16ImmAsmOperand'
  MCK_U1Imm, // user defined class 'PPCU1ImmAsmOperand'
  MCK_U2Imm, // user defined class 'PPCU2ImmAsmOperand'
  MCK_U3Imm, // user defined class 'PPCU3ImmAsmOperand'
  MCK_U4Imm, // user defined class 'PPCU4ImmAsmOperand'
  MCK_U5Imm, // user defined class 'PPCU5ImmAsmOperand'
  MCK_U6Imm, // user defined class 'PPCU6ImmAsmOperand'
  MCK_U7Imm, // user defined class 'PPCU7ImmAsmOperand'
  MCK_U8Imm, // user defined class 'PPCU8ImmAsmOperand'
  NumMatchClassKinds
};

}

static unsigned getDiagKindFromRegisterClass(MatchClassKind RegisterClass) {
  return MCTargetAsmParser::Match_InvalidOperand;
}

static MatchClassKind matchTokenString(StringRef Name) {
  switch (Name.size()) {
  default: break;
  case 1:	 // 9 strings to match.
    switch (Name[0]) {
    default: break;
    case '.':	 // 1 string to match.
      return MCK__DOT_;	 // "."
    case '0':	 // 1 string to match.
      return MCK_0;	 // "0"
    case '1':	 // 1 string to match.
      return MCK_1;	 // "1"
    case '2':	 // 1 string to match.
      return MCK_2;	 // "2"
    case '3':	 // 1 string to match.
      return MCK_3;	 // "3"
    case '4':	 // 1 string to match.
      return MCK_4;	 // "4"
    case '5':	 // 1 string to match.
      return MCK_5;	 // "5"
    case '6':	 // 1 string to match.
      return MCK_6;	 // "6"
    case '7':	 // 1 string to match.
      return MCK_7;	 // "7"
    }
    break;
  case 3:	 // 1 string to match.
    if (memcmp(Name.data()+0, "crD", 3) != 0)
      break;
    return MCK_crD;	 // "crD"
  }
  return InvalidMatchClass;
}

/// isSubclass - Compute whether \p A is a subclass of \p B.
static bool isSubclass(MatchClassKind A, MatchClassKind B) {
  if (A == B)
    return true;

  switch (A) {
  default:
    return false;

  case MCK_CRRC0:
    return B == MCK_CRRC;

  case MCK_Reg21:
    switch (B) {
    default: return false;
    case MCK_VSLRC: return true;
    case MCK_Reg20: return true;
    case MCK_VSRC: return true;
    }

  case MCK_Reg15:
    return B == MCK_QSRC;

  case MCK_Reg9:
    switch (B) {
    default: return false;
    case MCK_F4RC: return true;
    case MCK_Reg12: return true;
    case MCK_VSSRC: return true;
    case MCK_SPILLTOVSRRC: return true;
    }

  case MCK_Reg17:
    switch (B) {
    default: return false;
    case MCK_VRRC: return true;
    case MCK_Reg20: return true;
    case MCK_VSRC: return true;
    }

  case MCK_Reg13:
    switch (B) {
    default: return false;
    case MCK_VFRC: return true;
    case MCK_Reg12: return true;
    case MCK_VSSRC: return true;
    case MCK_SPILLTOVSRRC: return true;
    }

  case MCK_Reg8:
    return B == MCK_SPERC;

  case MCK_F4RC:
    return B == MCK_VSSRC;

  case MCK_VFRC:
    return B == MCK_VSSRC;

  case MCK_VRRC:
    return B == MCK_VSRC;

  case MCK_VSLRC:
    return B == MCK_VSRC;

  case MCK_Reg6:
    switch (B) {
    default: return false;
    case MCK_G8RC: return true;
    case MCK_G8RC_NOX0: return true;
    case MCK_SPILLTOVSRRC: return true;
    }

  case MCK_Reg2:
    switch (B) {
    default: return false;
    case MCK_GPRC: return true;
    case MCK_GPRC_NOR0: return true;
    }

  case MCK_Reg20:
    return B == MCK_VSRC;

  case MCK_Reg12:
    switch (B) {
    default: return false;
    case MCK_VSSRC: return true;
    case MCK_SPILLTOVSRRC: return true;
    }

  case MCK_G8RC:
    return B == MCK_SPILLTOVSRRC;
  }
}

static unsigned validateOperandClass(MCParsedAsmOperand &GOp, MatchClassKind Kind) {
  PPCOperand &Operand = (PPCOperand&)GOp;
  if (Kind == InvalidMatchClass)
    return MCTargetAsmParser::Match_InvalidOperand;

  if (Operand.isToken() && Kind <= MCK_LAST_TOKEN)
    return isSubclass(matchTokenString(Operand.getToken()), Kind) ?
             MCTargetAsmParser::Match_Success :
             MCTargetAsmParser::Match_InvalidOperand;

  switch (Kind) {
  default: break;
  // 'Imm' class
  case MCK_Imm: {
    DiagnosticPredicate DP(Operand.isImm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'ATBitsAsHint' class
  case MCK_ATBitsAsHint: {
    DiagnosticPredicate DP(Operand.isATBitsAsHint());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'CRBitMask' class
  case MCK_CRBitMask: {
    DiagnosticPredicate DP(Operand.isCRBitMask());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'CondBr' class
  case MCK_CondBr: {
    DiagnosticPredicate DP(Operand.isCondBr());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DirectBr' class
  case MCK_DirectBr: {
    DiagnosticPredicate DP(Operand.isDirectBr());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispRI' class
  case MCK_DispRI: {
    DiagnosticPredicate DP(Operand.isS16Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispRIX16' class
  case MCK_DispRIX16: {
    DiagnosticPredicate DP(Operand.isS16ImmX16());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispRIX' class
  case MCK_DispRIX: {
    DiagnosticPredicate DP(Operand.isS16ImmX4());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispSPE2' class
  case MCK_DispSPE2: {
    DiagnosticPredicate DP(Operand.isU6ImmX2());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispSPE4' class
  case MCK_DispSPE4: {
    DiagnosticPredicate DP(Operand.isU7ImmX4());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'DispSPE8' class
  case MCK_DispSPE8: {
    DiagnosticPredicate DP(Operand.isU8ImmX8());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegCRBITRC' class
  case MCK_RegCRBITRC: {
    DiagnosticPredicate DP(Operand.isCRBitNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegCRRC' class
  case MCK_RegCRRC: {
    DiagnosticPredicate DP(Operand.isCCRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegF4RC' class
  case MCK_RegF4RC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegF8RC' class
  case MCK_RegF8RC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegG8RC' class
  case MCK_RegG8RC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegG8RCNoX0' class
  case MCK_RegG8RCNoX0: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegGPRC' class
  case MCK_RegGPRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegGPRCNoR0' class
  case MCK_RegGPRCNoR0: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegGxRCNoR0' class
  case MCK_RegGxRCNoR0: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegGxRC' class
  case MCK_RegGxRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegQBRC' class
  case MCK_RegQBRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegQFRC' class
  case MCK_RegQFRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegQSRC' class
  case MCK_RegQSRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegSPE4RC' class
  case MCK_RegSPE4RC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegSPERC' class
  case MCK_RegSPERC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegSPILLTOVSRRC' class
  case MCK_RegSPILLTOVSRRC: {
    DiagnosticPredicate DP(Operand.isVSRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegVFRC' class
  case MCK_RegVFRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegVRRC' class
  case MCK_RegVRRC: {
    DiagnosticPredicate DP(Operand.isRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegVSFRC' class
  case MCK_RegVSFRC: {
    DiagnosticPredicate DP(Operand.isVSRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegVSRC' class
  case MCK_RegVSRC: {
    DiagnosticPredicate DP(Operand.isVSRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'RegVSSRC' class
  case MCK_RegVSSRC: {
    DiagnosticPredicate DP(Operand.isVSRegNumber());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'S16Imm' class
  case MCK_S16Imm: {
    DiagnosticPredicate DP(Operand.isS16Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'S17Imm' class
  case MCK_S17Imm: {
    DiagnosticPredicate DP(Operand.isS17Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'S5Imm' class
  case MCK_S5Imm: {
    DiagnosticPredicate DP(Operand.isS5Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'TLSReg' class
  case MCK_TLSReg: {
    DiagnosticPredicate DP(Operand.isTLSReg());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U10Imm' class
  case MCK_U10Imm: {
    DiagnosticPredicate DP(Operand.isU10Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U12Imm' class
  case MCK_U12Imm: {
    DiagnosticPredicate DP(Operand.isU12Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U16Imm' class
  case MCK_U16Imm: {
    DiagnosticPredicate DP(Operand.isU16Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U1Imm' class
  case MCK_U1Imm: {
    DiagnosticPredicate DP(Operand.isU1Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U2Imm' class
  case MCK_U2Imm: {
    DiagnosticPredicate DP(Operand.isU2Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U3Imm' class
  case MCK_U3Imm: {
    DiagnosticPredicate DP(Operand.isU3Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U4Imm' class
  case MCK_U4Imm: {
    DiagnosticPredicate DP(Operand.isU4Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U5Imm' class
  case MCK_U5Imm: {
    DiagnosticPredicate DP(Operand.isU5Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U6Imm' class
  case MCK_U6Imm: {
    DiagnosticPredicate DP(Operand.isU6Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U7Imm' class
  case MCK_U7Imm: {
    DiagnosticPredicate DP(Operand.isU7Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  // 'U8Imm' class
  case MCK_U8Imm: {
    DiagnosticPredicate DP(Operand.isU8Imm());
    if (DP.isMatch())
      return MCTargetAsmParser::Match_Success;
    break;
    }
  } // end switch (Kind)

  if (Operand.isReg()) {
    MatchClassKind OpKind;
    switch (Operand.getReg()) {
    default: OpKind = InvalidMatchClass; break;
    case PPC::R0: OpKind = MCK_GPRC; break;
    case PPC::R1: OpKind = MCK_Reg2; break;
    case PPC::R2: OpKind = MCK_Reg2; break;
    case PPC::R3: OpKind = MCK_Reg2; break;
    case PPC::R4: OpKind = MCK_Reg2; break;
    case PPC::R5: OpKind = MCK_Reg2; break;
    case PPC::R6: OpKind = MCK_Reg2; break;
    case PPC::R7: OpKind = MCK_Reg2; break;
    case PPC::R8: OpKind = MCK_Reg2; break;
    case PPC::R9: OpKind = MCK_Reg2; break;
    case PPC::R10: OpKind = MCK_Reg2; break;
    case PPC::R11: OpKind = MCK_Reg2; break;
    case PPC::R12: OpKind = MCK_Reg2; break;
    case PPC::R13: OpKind = MCK_Reg2; break;
    case PPC::R14: OpKind = MCK_Reg2; break;
    case PPC::R15: OpKind = MCK_Reg2; break;
    case PPC::R16: OpKind = MCK_Reg2; break;
    case PPC::R17: OpKind = MCK_Reg2; break;
    case PPC::R18: OpKind = MCK_Reg2; break;
    case PPC::R19: OpKind = MCK_Reg2; break;
    case PPC::R20: OpKind = MCK_Reg2; break;
    case PPC::R21: OpKind = MCK_Reg2; break;
    case PPC::R22: OpKind = MCK_Reg2; break;
    case PPC::R23: OpKind = MCK_Reg2; break;
    case PPC::R24: OpKind = MCK_Reg2; break;
    case PPC::R25: OpKind = MCK_Reg2; break;
    case PPC::R26: OpKind = MCK_Reg2; break;
    case PPC::R27: OpKind = MCK_Reg2; break;
    case PPC::R28: OpKind = MCK_Reg2; break;
    case PPC::R29: OpKind = MCK_Reg2; break;
    case PPC::R30: OpKind = MCK_Reg2; break;
    case PPC::R31: OpKind = MCK_Reg2; break;
    case PPC::X0: OpKind = MCK_G8RC; break;
    case PPC::X1: OpKind = MCK_Reg6; break;
    case PPC::X2: OpKind = MCK_Reg6; break;
    case PPC::X3: OpKind = MCK_Reg6; break;
    case PPC::X4: OpKind = MCK_Reg6; break;
    case PPC::X5: OpKind = MCK_Reg6; break;
    case PPC::X6: OpKind = MCK_Reg6; break;
    case PPC::X7: OpKind = MCK_Reg6; break;
    case PPC::X8: OpKind = MCK_Reg6; break;
    case PPC::X9: OpKind = MCK_Reg6; break;
    case PPC::X10: OpKind = MCK_Reg6; break;
    case PPC::X11: OpKind = MCK_Reg6; break;
    case PPC::X12: OpKind = MCK_Reg6; break;
    case PPC::X13: OpKind = MCK_Reg6; break;
    case PPC::X14: OpKind = MCK_Reg6; break;
    case PPC::X15: OpKind = MCK_Reg6; break;
    case PPC::X16: OpKind = MCK_Reg6; break;
    case PPC::X17: OpKind = MCK_Reg6; break;
    case PPC::X18: OpKind = MCK_Reg6; break;
    case PPC::X19: OpKind = MCK_Reg6; break;
    case PPC::X20: OpKind = MCK_Reg6; break;
    case PPC::X21: OpKind = MCK_Reg6; break;
    case PPC::X22: OpKind = MCK_Reg6; break;
    case PPC::X23: OpKind = MCK_Reg6; break;
    case PPC::X24: OpKind = MCK_Reg6; break;
    case PPC::X25: OpKind = MCK_Reg6; break;
    case PPC::X26: OpKind = MCK_Reg6; break;
    case PPC::X27: OpKind = MCK_Reg6; break;
    case PPC::X28: OpKind = MCK_Reg6; break;
    case PPC::X29: OpKind = MCK_Reg6; break;
    case PPC::X30: OpKind = MCK_Reg6; break;
    case PPC::X31: OpKind = MCK_Reg6; break;
    case PPC::S0: OpKind = MCK_SPERC; break;
    case PPC::S1: OpKind = MCK_Reg8; break;
    case PPC::S2: OpKind = MCK_Reg8; break;
    case PPC::S3: OpKind = MCK_Reg8; break;
    case PPC::S4: OpKind = MCK_Reg8; break;
    case PPC::S5: OpKind = MCK_Reg8; break;
    case PPC::S6: OpKind = MCK_Reg8; break;
    case PPC::S7: OpKind = MCK_Reg8; break;
    case PPC::S8: OpKind = MCK_Reg8; break;
    case PPC::S9: OpKind = MCK_Reg8; break;
    case PPC::S10: OpKind = MCK_Reg8; break;
    case PPC::S11: OpKind = MCK_Reg8; break;
    case PPC::S12: OpKind = MCK_Reg8; break;
    case PPC::S13: OpKind = MCK_Reg8; break;
    case PPC::S14: OpKind = MCK_Reg8; break;
    case PPC::S15: OpKind = MCK_Reg8; break;
    case PPC::S16: OpKind = MCK_Reg8; break;
    case PPC::S17: OpKind = MCK_Reg8; break;
    case PPC::S18: OpKind = MCK_Reg8; break;
    case PPC::S19: OpKind = MCK_Reg8; break;
    case PPC::S20: OpKind = MCK_Reg8; break;
    case PPC::S21: OpKind = MCK_Reg8; break;
    case PPC::S22: OpKind = MCK_Reg8; break;
    case PPC::S23: OpKind = MCK_Reg8; break;
    case PPC::S24: OpKind = MCK_Reg8; break;
    case PPC::S25: OpKind = MCK_Reg8; break;
    case PPC::S26: OpKind = MCK_Reg8; break;
    case PPC::S27: OpKind = MCK_Reg8; break;
    case PPC::S28: OpKind = MCK_Reg8; break;
    case PPC::S29: OpKind = MCK_Reg8; break;
    case PPC::S30: OpKind = MCK_Reg8; break;
    case PPC::S31: OpKind = MCK_Reg8; break;
    case PPC::F0: OpKind = MCK_Reg9; break;
    case PPC::F1: OpKind = MCK_Reg9; break;
    case PPC::F2: OpKind = MCK_Reg9; break;
    case PPC::F3: OpKind = MCK_Reg9; break;
    case PPC::F4: OpKind = MCK_Reg9; break;
    case PPC::F5: OpKind = MCK_Reg9; break;
    case PPC::F6: OpKind = MCK_Reg9; break;
    case PPC::F7: OpKind = MCK_Reg9; break;
    case PPC::F8: OpKind = MCK_Reg9; break;
    case PPC::F9: OpKind = MCK_Reg9; break;
    case PPC::F10: OpKind = MCK_Reg9; break;
    case PPC::F11: OpKind = MCK_Reg9; break;
    case PPC::F12: OpKind = MCK_Reg9; break;
    case PPC::F13: OpKind = MCK_Reg9; break;
    case PPC::F14: OpKind = MCK_F4RC; break;
    case PPC::F15: OpKind = MCK_F4RC; break;
    case PPC::F16: OpKind = MCK_F4RC; break;
    case PPC::F17: OpKind = MCK_F4RC; break;
    case PPC::F18: OpKind = MCK_F4RC; break;
    case PPC::F19: OpKind = MCK_F4RC; break;
    case PPC::F20: OpKind = MCK_F4RC; break;
    case PPC::F21: OpKind = MCK_F4RC; break;
    case PPC::F22: OpKind = MCK_F4RC; break;
    case PPC::F23: OpKind = MCK_F4RC; break;
    case PPC::F24: OpKind = MCK_F4RC; break;
    case PPC::F25: OpKind = MCK_F4RC; break;
    case PPC::F26: OpKind = MCK_F4RC; break;
    case PPC::F27: OpKind = MCK_F4RC; break;
    case PPC::F28: OpKind = MCK_F4RC; break;
    case PPC::F29: OpKind = MCK_F4RC; break;
    case PPC::F30: OpKind = MCK_F4RC; break;
    case PPC::F31: OpKind = MCK_F4RC; break;
    case PPC::VF0: OpKind = MCK_Reg13; break;
    case PPC::VF1: OpKind = MCK_Reg13; break;
    case PPC::VF2: OpKind = MCK_Reg13; break;
    case PPC::VF3: OpKind = MCK_Reg13; break;
    case PPC::VF4: OpKind = MCK_Reg13; break;
    case PPC::VF5: OpKind = MCK_Reg13; break;
    case PPC::VF6: OpKind = MCK_Reg13; break;
    case PPC::VF7: OpKind = MCK_Reg13; break;
    case PPC::VF8: OpKind = MCK_Reg13; break;
    case PPC::VF9: OpKind = MCK_Reg13; break;
    case PPC::VF10: OpKind = MCK_Reg13; break;
    case PPC::VF11: OpKind = MCK_Reg13; break;
    case PPC::VF12: OpKind = MCK_Reg13; break;
    case PPC::VF13: OpKind = MCK_Reg13; break;
    case PPC::VF14: OpKind = MCK_Reg13; break;
    case PPC::VF15: OpKind = MCK_Reg13; break;
    case PPC::VF16: OpKind = MCK_Reg13; break;
    case PPC::VF17: OpKind = MCK_Reg13; break;
    case PPC::VF18: OpKind = MCK_Reg13; break;
    case PPC::VF19: OpKind = MCK_Reg13; break;
    case PPC::VF20: OpKind = MCK_VFRC; break;
    case PPC::VF21: OpKind = MCK_VFRC; break;
    case PPC::VF22: OpKind = MCK_VFRC; break;
    case PPC::VF23: OpKind = MCK_VFRC; break;
    case PPC::VF24: OpKind = MCK_VFRC; break;
    case PPC::VF25: OpKind = MCK_VFRC; break;
    case PPC::VF26: OpKind = MCK_VFRC; break;
    case PPC::VF27: OpKind = MCK_VFRC; break;
    case PPC::VF28: OpKind = MCK_VFRC; break;
    case PPC::VF29: OpKind = MCK_VFRC; break;
    case PPC::VF30: OpKind = MCK_VFRC; break;
    case PPC::VF31: OpKind = MCK_VFRC; break;
    case PPC::QF0: OpKind = MCK_Reg15; break;
    case PPC::QF1: OpKind = MCK_Reg15; break;
    case PPC::QF2: OpKind = MCK_Reg15; break;
    case PPC::QF3: OpKind = MCK_Reg15; break;
    case PPC::QF4: OpKind = MCK_Reg15; break;
    case PPC::QF5: OpKind = MCK_Reg15; break;
    case PPC::QF6: OpKind = MCK_Reg15; break;
    case PPC::QF7: OpKind = MCK_Reg15; break;
    case PPC::QF8: OpKind = MCK_Reg15; break;
    case PPC::QF9: OpKind = MCK_Reg15; break;
    case PPC::QF10: OpKind = MCK_Reg15; break;
    case PPC::QF11: OpKind = MCK_Reg15; break;
    case PPC::QF12: OpKind = MCK_Reg15; break;
    case PPC::QF13: OpKind = MCK_Reg15; break;
    case PPC::QF14: OpKind = MCK_QSRC; break;
    case PPC::QF15: OpKind = MCK_QSRC; break;
    case PPC::QF16: OpKind = MCK_QSRC; break;
    case PPC::QF17: OpKind = MCK_QSRC; break;
    case PPC::QF18: OpKind = MCK_QSRC; break;
    case PPC::QF19: OpKind = MCK_QSRC; break;
    case PPC::QF20: OpKind = MCK_QSRC; break;
    case PPC::QF21: OpKind = MCK_QSRC; break;
    case PPC::QF22: OpKind = MCK_QSRC; break;
    case PPC::QF23: OpKind = MCK_QSRC; break;
    case PPC::QF24: OpKind = MCK_QSRC; break;
    case PPC::QF25: OpKind = MCK_QSRC; break;
    case PPC::QF26: OpKind = MCK_QSRC; break;
    case PPC::QF27: OpKind = MCK_QSRC; break;
    case PPC::QF28: OpKind = MCK_QSRC; break;
    case PPC::QF29: OpKind = MCK_QSRC; break;
    case PPC::QF30: OpKind = MCK_QSRC; break;
    case PPC::QF31: OpKind = MCK_QSRC; break;
    case PPC::V0: OpKind = MCK_Reg17; break;
    case PPC::V1: OpKind = MCK_Reg17; break;
    case PPC::V2: OpKind = MCK_Reg17; break;
    case PPC::V3: OpKind = MCK_Reg17; break;
    case PPC::V4: OpKind = MCK_Reg17; break;
    case PPC::V5: OpKind = MCK_Reg17; break;
    case PPC::V6: OpKind = MCK_Reg17; break;
    case PPC::V7: OpKind = MCK_Reg17; break;
    case PPC::V8: OpKind = MCK_Reg17; break;
    case PPC::V9: OpKind = MCK_Reg17; break;
    case PPC::V10: OpKind = MCK_Reg17; break;
    case PPC::V11: OpKind = MCK_Reg17; break;
    case PPC::V12: OpKind = MCK_Reg17; break;
    case PPC::V13: OpKind = MCK_Reg17; break;
    case PPC::V14: OpKind = MCK_Reg17; break;
    case PPC::V15: OpKind = MCK_Reg17; break;
    case PPC::V16: OpKind = MCK_Reg17; break;
    case PPC::V17: OpKind = MCK_Reg17; break;
    case PPC::V18: OpKind = MCK_Reg17; break;
    case PPC::V19: OpKind = MCK_Reg17; break;
    case PPC::V20: OpKind = MCK_VRRC; break;
    case PPC::V21: OpKind = MCK_VRRC; break;
    case PPC::V22: OpKind = MCK_VRRC; break;
    case PPC::V23: OpKind = MCK_VRRC; break;
    case PPC::V24: OpKind = MCK_VRRC; break;
    case PPC::V25: OpKind = MCK_VRRC; break;
    case PPC::V26: OpKind = MCK_VRRC; break;
    case PPC::V27: OpKind = MCK_VRRC; break;
    case PPC::V28: OpKind = MCK_VRRC; break;
    case PPC::V29: OpKind = MCK_VRRC; break;
    case PPC::V30: OpKind = MCK_VRRC; break;
    case PPC::V31: OpKind = MCK_VRRC; break;
    case PPC::VSL0: OpKind = MCK_Reg21; break;
    case PPC::VSL1: OpKind = MCK_Reg21; break;
    case PPC::VSL2: OpKind = MCK_Reg21; break;
    case PPC::VSL3: OpKind = MCK_Reg21; break;
    case PPC::VSL4: OpKind = MCK_Reg21; break;
    case PPC::VSL5: OpKind = MCK_Reg21; break;
    case PPC::VSL6: OpKind = MCK_Reg21; break;
    case PPC::VSL7: OpKind = MCK_Reg21; break;
    case PPC::VSL8: OpKind = MCK_Reg21; break;
    case PPC::VSL9: OpKind = MCK_Reg21; break;
    case PPC::VSL10: OpKind = MCK_Reg21; break;
    case PPC::VSL11: OpKind = MCK_Reg21; break;
    case PPC::VSL12: OpKind = MCK_Reg21; break;
    case PPC::VSL13: OpKind = MCK_Reg21; break;
    case PPC::VSL14: OpKind = MCK_VSLRC; break;
    case PPC::VSL15: OpKind = MCK_VSLRC; break;
    case PPC::VSL16: OpKind = MCK_VSLRC; break;
    case PPC::VSL17: OpKind = MCK_VSLRC; break;
    case PPC::VSL18: OpKind = MCK_VSLRC; break;
    case PPC::VSL19: OpKind = MCK_VSLRC; break;
    case PPC::VSL20: OpKind = MCK_VSLRC; break;
    case PPC::VSL21: OpKind = MCK_VSLRC; break;
    case PPC::VSL22: OpKind = MCK_VSLRC; break;
    case PPC::VSL23: OpKind = MCK_VSLRC; break;
    case PPC::VSL24: OpKind = MCK_VSLRC; break;
    case PPC::VSL25: OpKind = MCK_VSLRC; break;
    case PPC::VSL26: OpKind = MCK_VSLRC; break;
    case PPC::VSL27: OpKind = MCK_VSLRC; break;
    case PPC::VSL28: OpKind = MCK_VSLRC; break;
    case PPC::VSL29: OpKind = MCK_VSLRC; break;
    case PPC::VSL30: OpKind = MCK_VSLRC; break;
    case PPC::VSL31: OpKind = MCK_VSLRC; break;
    case PPC::ZERO: OpKind = MCK_GPRC_NOR0; break;
    case PPC::ZERO8: OpKind = MCK_G8RC_NOX0; break;
    case PPC::FP: OpKind = MCK_Reg2; break;
    case PPC::FP8: OpKind = MCK_Reg6; break;
    case PPC::BP: OpKind = MCK_Reg2; break;
    case PPC::BP8: OpKind = MCK_Reg6; break;
    case PPC::CR0LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR0GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR0EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR0UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR1LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR1GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR1EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR1UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR2LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR2GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR2EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR2UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR3LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR3GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR3EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR3UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR4LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR4GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR4EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR4UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR5LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR5GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR5EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR5UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR6LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR6GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR6EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR6UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR7LT: OpKind = MCK_CRBITRC; break;
    case PPC::CR7GT: OpKind = MCK_CRBITRC; break;
    case PPC::CR7EQ: OpKind = MCK_CRBITRC; break;
    case PPC::CR7UN: OpKind = MCK_CRBITRC; break;
    case PPC::CR0: OpKind = MCK_CRRC0; break;
    case PPC::CR1: OpKind = MCK_CRRC; break;
    case PPC::CR2: OpKind = MCK_CRRC; break;
    case PPC::CR3: OpKind = MCK_CRRC; break;
    case PPC::CR4: OpKind = MCK_CRRC; break;
    case PPC::CR5: OpKind = MCK_CRRC; break;
    case PPC::CR6: OpKind = MCK_CRRC; break;
    case PPC::CR7: OpKind = MCK_CRRC; break;
    case PPC::CTR: OpKind = MCK_CTRRC; break;
    case PPC::CTR8: OpKind = MCK_CTRRC8; break;
    case PPC::VRSAVE: OpKind = MCK_VRSAVERC; break;
    case PPC::XER: OpKind = MCK_CARRYRC; break;
    case PPC::CARRY: OpKind = MCK_CARRYRC; break;
    }
    return isSubclass(OpKind, Kind) ? (unsigned)MCTargetAsmParser::Match_Success :
                                      getDiagKindFromRegisterClass(Kind);
  }

  if (Kind > MCK_LAST_TOKEN && Kind <= MCK_LAST_REGISTER)
    return getDiagKindFromRegisterClass(Kind);

  return MCTargetAsmParser::Match_InvalidOperand;
}

#ifndef NDEBUG
const char *getMatchClassName(MatchClassKind Kind) {
  switch (Kind) {
  case InvalidMatchClass: return "InvalidMatchClass";
  case OptionalMatchClass: return "OptionalMatchClass";
  case MCK__DOT_: return "MCK__DOT_";
  case MCK_0: return "MCK_0";
  case MCK_1: return "MCK_1";
  case MCK_2: return "MCK_2";
  case MCK_3: return "MCK_3";
  case MCK_4: return "MCK_4";
  case MCK_5: return "MCK_5";
  case MCK_6: return "MCK_6";
  case MCK_7: return "MCK_7";
  case MCK_crD: return "MCK_crD";
  case MCK_CRRC0: return "MCK_CRRC0";
  case MCK_CTRRC: return "MCK_CTRRC";
  case MCK_CTRRC8: return "MCK_CTRRC8";
  case MCK_VRSAVERC: return "MCK_VRSAVERC";
  case MCK_CARRYRC: return "MCK_CARRYRC";
  case MCK_CRRC: return "MCK_CRRC";
  case MCK_Reg21: return "MCK_Reg21";
  case MCK_Reg15: return "MCK_Reg15";
  case MCK_Reg9: return "MCK_Reg9";
  case MCK_Reg17: return "MCK_Reg17";
  case MCK_Reg13: return "MCK_Reg13";
  case MCK_Reg8: return "MCK_Reg8";
  case MCK_CRBITRC: return "MCK_CRBITRC";
  case MCK_F4RC: return "MCK_F4RC";
  case MCK_QSRC: return "MCK_QSRC";
  case MCK_SPERC: return "MCK_SPERC";
  case MCK_VFRC: return "MCK_VFRC";
  case MCK_VRRC: return "MCK_VRRC";
  case MCK_VSLRC: return "MCK_VSLRC";
  case MCK_Reg6: return "MCK_Reg6";
  case MCK_Reg2: return "MCK_Reg2";
  case MCK_Reg20: return "MCK_Reg20";
  case MCK_Reg12: return "MCK_Reg12";
  case MCK_G8RC: return "MCK_G8RC";
  case MCK_G8RC_NOX0: return "MCK_G8RC_NOX0";
  case MCK_GPRC: return "MCK_GPRC";
  case MCK_GPRC_NOR0: return "MCK_GPRC_NOR0";
  case MCK_VSRC: return "MCK_VSRC";
  case MCK_VSSRC: return "MCK_VSSRC";
  case MCK_SPILLTOVSRRC: return "MCK_SPILLTOVSRRC";
  case MCK_Imm: return "MCK_Imm";
  case MCK_ATBitsAsHint: return "MCK_ATBitsAsHint";
  case MCK_CRBitMask: return "MCK_CRBitMask";
  case MCK_CondBr: return "MCK_CondBr";
  case MCK_DirectBr: return "MCK_DirectBr";
  case MCK_DispRI: return "MCK_DispRI";
  case MCK_DispRIX16: return "MCK_DispRIX16";
  case MCK_DispRIX: return "MCK_DispRIX";
  case MCK_DispSPE2: return "MCK_DispSPE2";
  case MCK_DispSPE4: return "MCK_DispSPE4";
  case MCK_DispSPE8: return "MCK_DispSPE8";
  case MCK_RegCRBITRC: return "MCK_RegCRBITRC";
  case MCK_RegCRRC: return "MCK_RegCRRC";
  case MCK_RegF4RC: return "MCK_RegF4RC";
  case MCK_RegF8RC: return "MCK_RegF8RC";
  case MCK_RegG8RC: return "MCK_RegG8RC";
  case MCK_RegG8RCNoX0: return "MCK_RegG8RCNoX0";
  case MCK_RegGPRC: return "MCK_RegGPRC";
  case MCK_RegGPRCNoR0: return "MCK_RegGPRCNoR0";
  case MCK_RegGxRCNoR0: return "MCK_RegGxRCNoR0";
  case MCK_RegGxRC: return "MCK_RegGxRC";
  case MCK_RegQBRC: return "MCK_RegQBRC";
  case MCK_RegQFRC: return "MCK_RegQFRC";
  case MCK_RegQSRC: return "MCK_RegQSRC";
  case MCK_RegSPE4RC: return "MCK_RegSPE4RC";
  case MCK_RegSPERC: return "MCK_RegSPERC";
  case MCK_RegSPILLTOVSRRC: return "MCK_RegSPILLTOVSRRC";
  case MCK_RegVFRC: return "MCK_RegVFRC";
  case MCK_RegVRRC: return "MCK_RegVRRC";
  case MCK_RegVSFRC: return "MCK_RegVSFRC";
  case MCK_RegVSRC: return "MCK_RegVSRC";
  case MCK_RegVSSRC: return "MCK_RegVSSRC";
  case MCK_S16Imm: return "MCK_S16Imm";
  case MCK_S17Imm: return "MCK_S17Imm";
  case MCK_S5Imm: return "MCK_S5Imm";
  case MCK_TLSReg: return "MCK_TLSReg";
  case MCK_U10Imm: return "MCK_U10Imm";
  case MCK_U12Imm: return "MCK_U12Imm";
  case MCK_U16Imm: return "MCK_U16Imm";
  case MCK_U1Imm: return "MCK_U1Imm";
  case MCK_U2Imm: return "MCK_U2Imm";
  case MCK_U3Imm: return "MCK_U3Imm";
  case MCK_U4Imm: return "MCK_U4Imm";
  case MCK_U5Imm: return "MCK_U5Imm";
  case MCK_U6Imm: return "MCK_U6Imm";
  case MCK_U7Imm: return "MCK_U7Imm";
  case MCK_U8Imm: return "MCK_U8Imm";
  case NumMatchClassKinds: return "NumMatchClassKinds";
  }
  llvm_unreachable("unhandled MatchClassKind!");
}

#endif // NDEBUG
uint64_t PPCAsmParser::
ComputeAvailableFeatures(const FeatureBitset& FB) const {
  uint64_t Features = 0;
  return Features;
}

static bool checkAsmTiedOperandConstraints(const PPCAsmParser&AsmParser,
                               unsigned Kind,
                               const OperandVector &Operands,
                               uint64_t &ErrorInfo) {
  assert(Kind < CVT_NUM_SIGNATURES && "Invalid signature!");
  const uint8_t *Converter = ConversionTable[Kind];
  for (const uint8_t *p = Converter; *p; p+= 2) {
    switch (*p) {
    case CVT_Tied: {
      unsigned OpIdx = *(p+1);
      assert(OpIdx < (size_t)(std::end(TiedAsmOperandTable) -
                              std::begin(TiedAsmOperandTable)) &&
             "Tied operand not found");
      unsigned OpndNum1 = TiedAsmOperandTable[OpIdx][1];
      unsigned OpndNum2 = TiedAsmOperandTable[OpIdx][2];
      if (OpndNum1 != OpndNum2) {
        auto &SrcOp1 = Operands[OpndNum1];
        auto &SrcOp2 = Operands[OpndNum2];
        if (SrcOp1->isReg() && SrcOp2->isReg()) {
          if (!AsmParser.regsEqual(*SrcOp1, *SrcOp2)) {
            ErrorInfo = OpndNum2;
            return false;
          }
        }
      }
      break;
    }
    default:
      break;
    }
  }
  return true;
}

static const char *const MnemonicTable =
    "\003add\004addc\004adde\004addi\005addic\005addis\005addme\007addpcis\005"
    "addze\003and\004andc\004andi\005andis\004attn\001b\002ba\002bc\003bc+\003"
    "bc-\003bca\004bca+\004bca-\005bcctr\006bcctrl\006bcdcfn\007bcdcfsq\006b"
    "cdcfz\010bcdcpsgn\006bcdctn\007bcdctsq\006bcdctz\004bcds\tbcdsetsgn\005"
    "bcdsr\010bcdtrunc\005bcdus\tbcdutrunc\003bcl\004bcl+\004bcl-\004bcla\005"
    "bcla+\005bcla-\004bclr\005bclrl\004bctr\005bctrl\004bdnz\005bdnz+\005bd"
    "nz-\005bdnza\006bdnza+\006bdnza-\005bdnzf\006bdnzfa\006bdnzfl\007bdnzfl"
    "a\007bdnzflr\010bdnzflrl\005bdnzl\006bdnzl+\006bdnzl-\006bdnzla\007bdnz"
    "la+\007bdnzla-\006bdnzlr\007bdnzlr+\007bdnzlr-\007bdnzlrl\010bdnzlrl+\010"
    "bdnzlrl-\005bdnzt\006bdnzta\006bdnztl\007bdnztla\007bdnztlr\010bdnztlrl"
    "\003bdz\004bdz+\004bdz-\004bdza\005bdza+\005bdza-\004bdzf\005bdzfa\005b"
    "dzfl\006bdzfla\006bdzflr\007bdzflrl\004bdzl\005bdzl+\005bdzl-\005bdzla\006"
    "bdzla+\006bdzla-\005bdzlr\006bdzlr+\006bdzlr-\006bdzlrl\007bdzlrl+\007b"
    "dzlrl-\004bdzt\005bdzta\005bdztl\006bdztla\006bdztlr\007bdztlrl\003beq\004"
    "beq+\004beq-\004beqa\005beqa+\005beqa-\006beqctr\007beqctr+\007beqctr-\007"
    "beqctrl\010beqctrl+\010beqctrl-\004beql\005beql+\005beql-\005beqla\006b"
    "eqla+\006beqla-\005beqlr\006beqlr+\006beqlr-\006beqlrl\007beqlrl+\007be"
    "qlrl-\002bf\003bf+\003bf-\003bfa\004bfa+\004bfa-\005bfctr\006bfctr+\006"
    "bfctr-\006bfctrl\007bfctrl+\007bfctrl-\003bfl\004bfl+\004bfl-\004bfla\005"
    "bfla+\005bfla-\004bflr\005bflr+\005bflr-\005bflrl\006bflrl+\006bflrl-\003"
    "bge\004bge+\004bge-\004bgea\005bgea+\005bgea-\006bgectr\007bgectr+\007b"
    "gectr-\007bgectrl\010bgectrl+\010bgectrl-\004bgel\005bgel+\005bgel-\005"
    "bgela\006bgela+\006bgela-\005bgelr\006bgelr+\006bgelr-\006bgelrl\007bge"
    "lrl+\007bgelrl-\003bgt\004bgt+\004bgt-\004bgta\005bgta+\005bgta-\006bgt"
    "ctr\007bgtctr+\007bgtctr-\007bgtctrl\010bgtctrl+\010bgtctrl-\004bgtl\005"
    "bgtl+\005bgtl-\005bgtla\006bgtla+\006bgtla-\005bgtlr\006bgtlr+\006bgtlr"
    "-\006bgtlrl\007bgtlrl+\007bgtlrl-\002bl\003bla\003ble\004ble+\004ble-\004"
    "blea\005blea+\005blea-\006blectr\007blectr+\007blectr-\007blectrl\010bl"
    "ectrl+\010blectrl-\004blel\005blel+\005blel-\005blela\006blela+\006blel"
    "a-\005blelr\006blelr+\006blelr-\006blelrl\007blelrl+\007blelrl-\003blr\004"
    "blrl\003blt\004blt+\004blt-\004blta\005blta+\005blta-\006bltctr\007bltc"
    "tr+\007bltctr-\007bltctrl\010bltctrl+\010bltctrl-\004bltl\005bltl+\005b"
    "ltl-\005bltla\006bltla+\006bltla-\005bltlr\006bltlr+\006bltlr-\006bltlr"
    "l\007bltlrl+\007bltlrl-\003bne\004bne+\004bne-\004bnea\005bnea+\005bnea"
    "-\006bnectr\007bnectr+\007bnectr-\007bnectrl\010bnectrl+\010bnectrl-\004"
    "bnel\005bnel+\005bnel-\005bnela\006bnela+\006bnela-\005bnelr\006bnelr+\006"
    "bnelr-\006bnelrl\007bnelrl+\007bnelrl-\003bng\004bng+\004bng-\004bnga\005"
    "bnga+\005bnga-\006bngctr\007bngctr+\007bngctr-\007bngctrl\010bngctrl+\010"
    "bngctrl-\004bngl\005bngl+\005bngl-\005bngla\006bngla+\006bngla-\005bngl"
    "r\006bnglr+\006bnglr-\006bnglrl\007bnglrl+\007bnglrl-\003bnl\004bnl+\004"
    "bnl-\004bnla\005bnla+\005bnla-\006bnlctr\007bnlctr+\007bnlctr-\007bnlct"
    "rl\010bnlctrl+\010bnlctrl-\004bnll\005bnll+\005bnll-\005bnlla\006bnlla+"
    "\006bnlla-\005bnllr\006bnllr+\006bnllr-\006bnllrl\007bnllrl+\007bnllrl-"
    "\003bns\004bns+\004bns-\004bnsa\005bnsa+\005bnsa-\006bnsctr\007bnsctr+\007"
    "bnsctr-\007bnsctrl\010bnsctrl+\010bnsctrl-\004bnsl\005bnsl+\005bnsl-\005"
    "bnsla\006bnsla+\006bnsla-\005bnslr\006bnslr+\006bnslr-\006bnslrl\007bns"
    "lrl+\007bnslrl-\003bnu\004bnu+\004bnu-\004bnua\005bnua+\005bnua-\006bnu"
    "ctr\007bnuctr+\007bnuctr-\007bnuctrl\010bnuctrl+\010bnuctrl-\004bnul\005"
    "bnul+\005bnul-\005bnula\006bnula+\006bnula-\005bnulr\006bnulr+\006bnulr"
    "-\006bnulrl\007bnulrl+\007bnulrl-\006bpermd\005brinc\003bso\004bso+\004"
    "bso-\004bsoa\005bsoa+\005bsoa-\006bsoctr\007bsoctr+\007bsoctr-\007bsoct"
    "rl\010bsoctrl+\010bsoctrl-\004bsol\005bsol+\005bsol-\005bsola\006bsola+"
    "\006bsola-\005bsolr\006bsolr+\006bsolr-\006bsolrl\007bsolrl+\007bsolrl-"
    "\002bt\003bt+\003bt-\003bta\004bta+\004bta-\005btctr\006btctr+\006btctr"
    "-\006btctrl\007btctrl+\007btctrl-\003btl\004btl+\004btl-\004btla\005btl"
    "a+\005btla-\004btlr\005btlr+\005btlr-\005btlrl\006btlrl+\006btlrl-\003b"
    "un\004bun+\004bun-\004buna\005buna+\005buna-\006bunctr\007bunctr+\007bu"
    "nctr-\007bunctrl\010bunctrl+\010bunctrl-\004bunl\005bunl+\005bunl-\005b"
    "unla\006bunla+\006bunla-\005bunlr\006bunlr+\006bunlr-\006bunlrl\007bunl"
    "rl+\007bunlrl-\007clrbhrb\006clrldi\010clrlsldi\010clrlslwi\006clrlwi\006"
    "clrrdi\006clrrwi\003cmp\004cmpb\004cmpd\005cmpdi\006cmpeqb\004cmpi\004c"
    "mpl\005cmpld\006cmpldi\005cmpli\005cmplw\006cmplwi\005cmprb\004cmpw\005"
    "cmpwi\006cntlzd\006cntlzw\006cnttzd\006cnttzw\004copy\ncopy_first\010cp"
    "_abort\005crand\006crandc\005crclr\005creqv\006crmove\006crnand\005crno"
    "r\005crnot\004cror\005crorc\005crset\005crxor\004darn\004dcba\004dcbf\006"
    "dcbfep\005dcbfl\006dcbflp\004dcbi\005dcbst\007dcbstep\004dcbt\006dcbtct"
    "\006dcbtds\006dcbtep\006dcbtst\010dcbtstct\010dcbtstds\010dcbtstep\007d"
    "cbtstt\005dcbtt\004dcbz\006dcbzep\005dcbzl\007dcbzlep\005dccci\003dci\004"
    "divd\005divde\006divdeu\005divdu\004divw\005divwe\006divweu\005divwu\003"
    "dss\006dssall\003dst\005dstst\006dststt\004dstt\006efdabs\006efdadd\006"
    "efdcfs\007efdcfsf\007efdcfsi\010efdcfsid\007efdcfuf\007efdcfui\010efdcf"
    "uid\010efdcmpeq\010efdcmpgt\010efdcmplt\007efdctsf\007efdctsi\tefdctsid"
    "z\010efdctsiz\007efdctuf\007efdctui\tefdctuidz\010efdctuiz\006efddiv\006"
    "efdmul\007efdnabs\006efdneg\006efdsub\010efdtsteq\010efdtstgt\010efdtst"
    "lt\006efsabs\006efsadd\006efscfd\007efscfsf\007efscfsi\007efscfuf\007ef"
    "scfui\010efscmpeq\010efscmpgt\010efscmplt\007efsctsf\007efsctsi\010efsc"
    "tsiz\007efsctuf\007efsctui\010efsctuiz\006efsdiv\006efsmul\007efsnabs\006"
    "efsneg\006efssub\010efststeq\010efststgt\010efststlt\005eieio\003eqv\005"
    "evabs\007evaddiw\013evaddsmiaaw\013evaddssiaaw\013evaddumiaaw\013evaddu"
    "siaaw\006evaddw\005evand\006evandc\007evcmpeq\010evcmpgts\010evcmpgtu\010"
    "evcmplts\010evcmpltu\010evcntlsw\010evcntlzw\007evdivws\007evdivwu\005e"
    "veqv\007evextsb\007evextsh\007evfsabs\007evfsadd\010evfscfsf\010evfscfs"
    "i\010evfscfuf\010evfscfui\tevfscmpeq\tevfscmpgt\tevfscmplt\010evfsctsf\010"
    "evfsctsi\tevfsctsiz\010evfsctui\007evfsdiv\007evfsmul\010evfsnabs\007ev"
    "fsneg\007evfssub\tevfststeq\tevfststgt\tevfststlt\005evldd\006evlddx\005"
    "evldh\006evldhx\005evldw\006evldwx\013evlhhesplat\014evlhhesplatx\014ev"
    "lhhossplat\015evlhhossplatx\014evlhhousplat\015evlhhousplatx\006evlwhe\007"
    "evlwhex\007evlwhos\010evlwhosx\007evlwhou\010evlwhoux\nevlwhsplat\013ev"
    "lwhsplatx\nevlwwsplat\013evlwwsplatx\tevmergehi\013evmergehilo\tevmerge"
    "lo\013evmergelohi\013evmhegsmfaa\013evmhegsmfan\013evmhegsmiaa\013evmhe"
    "gsmian\013evmhegumiaa\013evmhegumian\010evmhesmf\tevmhesmfa\013evmhesmf"
    "aaw\013evmhesmfanw\010evmhesmi\tevmhesmia\013evmhesmiaaw\013evmhesmianw"
    "\010evmhessf\tevmhessfa\013evmhessfaaw\013evmhessfanw\013evmhessiaaw\013"
    "evmhessianw\010evmheumi\tevmheumia\013evmheumiaaw\013evmheumianw\013evm"
    "heusiaaw\013evmheusianw\013evmhogsmfaa\013evmhogsmfan\013evmhogsmiaa\013"
    "evmhogsmian\013evmhogumiaa\013evmhogumian\010evmhosmf\tevmhosmfa\013evm"
    "hosmfaaw\013evmhosmfanw\010evmhosmi\tevmhosmia\013evmhosmiaaw\013evmhos"
    "mianw\010evmhossf\tevmhossfa\013evmhossfaaw\013evmhossfanw\013evmhossia"
    "aw\013evmhossianw\010evmhoumi\tevmhoumia\013evmhoumiaaw\013evmhoumianw\013"
    "evmhousiaaw\013evmhousianw\005evmra\010evmwhsmf\tevmwhsmfa\010evmwhsmi\t"
    "evmwhsmia\010evmwhssf\tevmwhssfa\010evmwhumi\tevmwhumia\013evmwlsmiaaw\013"
    "evmwlsmianw\013evmwlssiaaw\013evmwlssianw\010evmwlumi\tevmwlumia\013evm"
    "wlumiaaw\013evmwlumianw\013evmwlusiaaw\013evmwlusianw\007evmwsmf\010evm"
    "wsmfa\tevmwsmfaa\tevmwsmfan\007evmwsmi\010evmwsmia\tevmwsmiaa\tevmwsmia"
    "n\007evmwssf\010evmwssfa\tevmwssfaa\tevmwssfan\007evmwumi\010evmwumia\t"
    "evmwumiaa\tevmwumian\006evnand\005evneg\005evnor\004evor\005evorc\005ev"
    "rlw\006evrlwi\006evrndw\005evsel\005evslw\006evslwi\tevsplatfi\010evspl"
    "ati\007evsrwis\007evsrwiu\006evsrws\006evsrwu\006evstdd\007evstddx\006e"
    "vstdh\007evstdhx\006evstdw\007evstdwx\007evstwhe\010evstwhex\007evstwho"
    "\010evstwhox\007evstwwe\010evstwwex\007evstwwo\010evstwwox\014evsubfsmi"
    "aaw\014evsubfssiaaw\014evsubfumiaaw\014evsubfusiaaw\007evsubfw\010evsub"
    "ifw\005evxor\006extldi\006extlwi\006extrdi\006extrwi\005extsb\005extsh\005"
    "extsw\010extswsli\004fabs\004fadd\005fadds\005fcfid\006fcfids\006fcfidu"
    "\007fcfidus\005fcmpu\006fcpsgn\005fctid\006fctidu\007fctiduz\006fctidz\005"
    "fctiw\006fctiwu\007fctiwuz\006fctiwz\004fdiv\005fdivs\005fmadd\006fmadd"
    "s\003fmr\005fmsub\006fmsubs\004fmul\005fmuls\005fnabs\004fneg\006fnmadd"
    "\007fnmadds\006fnmsub\007fnmsubs\003fre\004fres\004frim\004frin\004frip"
    "\004friz\004frsp\007frsqrte\010frsqrtes\004fsel\005fsqrt\006fsqrts\004f"
    "sub\005fsubs\005ftdiv\006ftsqrt\005hrfid\004icbi\006icbiep\005icblc\005"
    "icblq\004icbt\006icbtls\005iccci\003ici\006inslwi\006insrdi\006insrwi\004"
    "isel\005isync\002la\005lbarx\005lbepx\003lbz\006lbzcix\004lbzu\005lbzux"
    "\004lbzx\002ld\005ldarx\004ldat\005ldbrx\005ldcix\004ldmx\003ldu\004ldu"
    "x\003ldx\003lfd\006lfdepx\004lfdu\005lfdux\004lfdx\006lfiwax\006lfiwzx\003"
    "lfs\004lfsu\005lfsux\004lfsx\003lha\005lharx\004lhau\005lhaux\004lhax\005"
    "lhbrx\005lhepx\003lhz\006lhzcix\004lhzu\005lhzux\004lhzx\002li\003lis\003"
    "lmw\004lnia\004lswi\005lvebx\005lvehx\005lvewx\004lvsl\004lvsr\003lvx\004"
    "lvxl\003lwa\005lwarx\004lwat\005lwaux\004lwax\005lwbrx\005lwepx\006lwsy"
    "nc\003lwz\006lwzcix\004lwzu\005lwzux\004lwzx\004lxsd\005lxsdx\007lxsibz"
    "x\007lxsihzx\007lxsiwax\007lxsiwzx\005lxssp\006lxsspx\003lxv\007lxvb16x"
    "\006lxvd2x\006lxvdsx\006lxvh8x\004lxvl\005lxvll\006lxvw4x\006lxvwsx\004"
    "lxvx\006maddhd\007maddhdu\006maddld\004mbar\004mcrf\005mcrfs\006mcrxrx\005"
    "mfamr\005mfasr\007mfbhrbe\005mfbr0\005mfbr1\005mfbr2\005mfbr3\005mfbr4\005"
    "mfbr5\005mfbr6\005mfbr7\006mfcfar\004mfcr\005mfctr\005mfdar\007mfdbatl\007"
    "mfdbatu\006mfdccr\005mfdcr\006mfdear\005mfdec\006mfdscr\007mfdsisr\005m"
    "fesr\006mffprd\004mffs\010mffscdrn\tmffscdrni\006mffsce\007mffscrn\010m"
    "ffscrni\005mffsl\007mfibatl\007mfibatu\006mficcr\004mflr\005mfmsr\006mf"
    "ocrf\005mfpid\005mfpmr\005mfpvr\006mfrtcl\006mfrtcu\006mfsdr1\tmfspefsc"
    "r\005mfspr\006mfsprg\007mfsprg0\007mfsprg1\007mfsprg2\007mfsprg3\007mfs"
    "prg4\007mfsprg5\007mfsprg6\007mfsprg7\004mfsr\006mfsrin\006mfsrr0\006mf"
    "srr1\006mfsrr2\006mfsrr3\004mftb\006mftbhi\005mftbl\006mftblo\005mftbu\005"
    "mftcr\005mfvrd\010mfvrsave\006mfvscr\006mfvsrd\007mfvsrld\007mfvsrwz\005"
    "mfxer\005modsd\005modsw\005modud\005moduw\002mr\007msgsync\005msync\005"
    "mtamr\005mtasr\005mtbr0\005mtbr1\005mtbr2\005mtbr3\005mtbr4\005mtbr5\005"
    "mtbr6\005mtbr7\006mtcfar\004mtcr\005mtcrf\005mtctr\005mtdar\007mtdbatl\007"
    "mtdbatu\006mtdccr\005mtdcr\006mtdear\005mtdec\006mtdscr\007mtdsisr\005m"
    "tesr\006mtfsb0\006mtfsb1\005mtfsf\006mtfsfi\007mtibatl\007mtibatu\006mt"
    "iccr\004mtlr\005mtmsr\006mtmsrd\006mtocrf\005mtpid\005mtpmr\006mtsdr1\t"
    "mtspefscr\005mtspr\006mtsprg\007mtsprg0\007mtsprg1\007mtsprg2\007mtsprg"
    "3\007mtsprg4\007mtsprg5\007mtsprg6\007mtsprg7\004mtsr\006mtsrin\006mtsr"
    "r0\006mtsrr1\006mtsrr2\006mtsrr3\006mttbhi\005mttbl\006mttblo\005mttbu\005"
    "mttcr\010mtvrsave\006mtvscr\006mtvsrd\007mtvsrdd\007mtvsrwa\007mtvsrws\007"
    "mtvsrwz\005mtxer\005mulhd\006mulhdu\005mulhw\006mulhwu\005mulld\005mull"
    "i\005mullw\004nand\003nap\003neg\003nop\003nor\003not\002or\003orc\003o"
    "ri\004oris\005paste\npaste_last\007popcntb\007popcntd\007popcntw\007pte"
    "sync\010qvaligni\tqvesplati\006qvfabs\006qvfadd\007qvfadds\006qvfand\007"
    "qvfandc\007qvfcfid\010qvfcfids\010qvfcfidu\tqvfcfidus\006qvfclr\010qvfc"
    "mpeq\010qvfcmpgt\010qvfcmplt\010qvfcpsgn\007qvfctfb\007qvfctid\010qvfct"
    "idu\tqvfctiduz\010qvfctidz\007qvfctiw\010qvfctiwu\tqvfctiwuz\010qvfctiw"
    "z\006qvfequ\nqvflogical\007qvfmadd\010qvfmadds\005qvfmr\007qvfmsub\010q"
    "vfmsubs\006qvfmul\007qvfmuls\007qvfnabs\007qvfnand\006qvfneg\010qvfnmad"
    "d\tqvfnmadds\010qvfnmsub\tqvfnmsubs\006qvfnor\006qvfnot\005qvfor\006qvf"
    "orc\007qvfperm\005qvfre\006qvfres\006qvfrim\006qvfrin\006qvfrip\006qvfr"
    "iz\006qvfrsp\tqvfrsqrte\nqvfrsqrtes\006qvfsel\006qvfset\006qvfsub\007qv"
    "fsubs\tqvftstnan\010qvfxmadd\tqvfxmadds\007qvfxmul\010qvfxmuls\006qvfxo"
    "r\014qvfxxcpnmadd\015qvfxxcpnmadds\tqvfxxmadd\nqvfxxmadds\013qvfxxnpmad"
    "d\014qvfxxnpmadds\006qvgpci\010qvlfcdux\tqvlfcduxa\007qvlfcdx\010qvlfcd"
    "xa\010qvlfcsux\tqvlfcsuxa\007qvlfcsx\010qvlfcsxa\007qvlfdux\010qvlfduxa"
    "\006qvlfdx\007qvlfdxa\010qvlfiwax\tqvlfiwaxa\010qvlfiwzx\tqvlfiwzxa\007"
    "qvlfsux\010qvlfsuxa\006qvlfsx\007qvlfsxa\010qvlpcldx\010qvlpclsx\010qvl"
    "pcrdx\010qvlpcrsx\tqvstfcdux\nqvstfcduxa\nqvstfcduxi\013qvstfcduxia\010"
    "qvstfcdx\tqvstfcdxa\tqvstfcdxi\nqvstfcdxia\tqvstfcsux\nqvstfcsuxa\nqvst"
    "fcsuxi\013qvstfcsuxia\010qvstfcsx\tqvstfcsxa\tqvstfcsxi\nqvstfcsxia\010"
    "qvstfdux\tqvstfduxa\tqvstfduxi\nqvstfduxia\007qvstfdx\010qvstfdxa\010qv"
    "stfdxi\tqvstfdxia\010qvstfiwx\tqvstfiwxa\010qvstfsux\tqvstfsuxa\tqvstfs"
    "uxi\nqvstfsuxia\007qvstfsx\010qvstfsxa\010qvstfsxi\tqvstfsxia\004rfci\004"
    "rfdi\005rfebb\003rfi\004rfid\005rfmci\005rldcl\005rldcr\005rldic\006rld"
    "icl\006rldicr\006rldimi\006rlwimi\006rlwinm\005rlwnm\005rotld\006rotldi"
    "\005rotlw\006rotlwi\006rotrdi\006rotrwi\002sc\004setb\005slbia\005slbie"
    "\006slbieg\007slbmfee\007slbmfev\006slbmte\007slbsync\003sld\004sldi\003"
    "slw\004slwi\004srad\005sradi\004sraw\005srawi\003srd\004srdi\003srw\004"
    "srwi\003stb\006stbcix\005stbcx\006stbepx\004stbu\005stbux\004stbx\003st"
    "d\005stdat\006stdbrx\006stdcix\005stdcx\004stdu\005stdux\004stdx\004stf"
    "d\007stfdepx\005stfdu\006stfdux\005stfdx\006stfiwx\004stfs\005stfsu\006"
    "stfsux\005stfsx\003sth\006sthbrx\006sthcix\005sthcx\006sthepx\004sthu\005"
    "sthux\004sthx\004stmw\004stop\005stswi\006stvebx\006stvehx\006stvewx\004"
    "stvx\005stvxl\003stw\005stwat\006stwbrx\006stwcix\005stwcx\006stwepx\004"
    "stwu\005stwux\004stwx\005stxsd\006stxsdx\007stxsibx\007stxsihx\007stxsi"
    "wx\006stxssp\007stxsspx\004stxv\010stxvb16x\007stxvd2x\007stxvh8x\005st"
    "xvl\006stxvll\007stxvw4x\005stxvx\003sub\004subc\004subf\005subfc\005su"
    "bfe\006subfic\006subfme\006subfze\004subi\005subic\005subis\007subpcis\004"
    "sync\006tabort\010tabortdc\ttabortdci\010tabortwc\ttabortwci\006tbegin\006"
    "tcheck\002td\004tdeq\005tdeqi\004tdge\005tdgei\004tdgt\005tdgti\003tdi\004"
    "tdle\005tdlei\005tdlge\006tdlgei\005tdlgt\006tdlgti\005tdlle\006tdllei\005"
    "tdllt\006tdllti\005tdlng\006tdlngi\005tdlnl\006tdlnli\004tdlt\005tdlti\004"
    "tdne\005tdnei\004tdng\005tdngi\004tdnl\005tdnli\003tdu\004tdui\004tend\005"
    "tlbia\005tlbie\006tlbiel\007tlbivax\005tlbld\005tlbli\005tlbre\007tlbre"
    "hi\007tlbrelo\005tlbsx\007tlbsync\005tlbwe\007tlbwehi\007tlbwelo\004tra"
    "p\010trechkpt\010treclaim\003tsr\002tw\004tweq\005tweqi\004twge\005twge"
    "i\004twgt\005twgti\003twi\004twle\005twlei\005twlge\006twlgei\005twlgt\006"
    "twlgti\005twlle\006twllei\005twllt\006twllti\005twlng\006twlngi\005twln"
    "l\006twlnli\004twlt\005twlti\004twne\005twnei\004twng\005twngi\004twnl\005"
    "twnli\003twu\004twui\007vabsdub\007vabsduh\007vabsduw\007vaddcuq\007vad"
    "dcuw\010vaddecuq\010vaddeuqm\006vaddfp\007vaddsbs\007vaddshs\007vaddsws"
    "\007vaddubm\007vaddubs\007vaddudm\007vadduhm\007vadduhs\007vadduqm\007v"
    "adduwm\007vadduws\004vand\005vandc\006vavgsb\006vavgsh\006vavgsw\006vav"
    "gub\006vavguh\006vavguw\007vbpermd\007vbpermq\005vcfsx\005vcfux\007vcip"
    "her\013vcipherlast\005vclzb\005vclzd\005vclzh\010vclzlsbb\005vclzw\007v"
    "cmpbfp\010vcmpeqfp\010vcmpequb\010vcmpequd\010vcmpequh\010vcmpequw\010v"
    "cmpgefp\010vcmpgtfp\010vcmpgtsb\010vcmpgtsd\010vcmpgtsh\010vcmpgtsw\010"
    "vcmpgtub\010vcmpgtud\010vcmpgtuh\010vcmpgtuw\007vcmpneb\007vcmpneh\007v"
    "cmpnew\010vcmpnezb\010vcmpnezh\010vcmpnezw\006vctsxs\006vctuxs\005vctzb"
    "\005vctzd\005vctzh\010vctzlsbb\005vctzw\004veqv\010vexptefp\tvextractd\n"
    "vextractub\nvextractuh\nvextractuw\010vextsb2d\010vextsb2w\010vextsh2d\010"
    "vextsh2w\010vextsw2d\010vextublx\010vextubrx\010vextuhlx\010vextuhrx\010"
    "vextuwlx\010vextuwrx\005vgbbd\010vinsertb\010vinsertd\010vinserth\010vi"
    "nsertw\007vlogefp\007vmaddfp\006vmaxfp\006vmaxsb\006vmaxsd\006vmaxsh\006"
    "vmaxsw\006vmaxub\006vmaxud\006vmaxuh\006vmaxuw\tvmhaddshs\nvmhraddshs\006"
    "vminfp\006vminsb\006vminsd\006vminsh\006vminsw\006vminub\006vminud\006v"
    "minuh\006vminuw\tvmladduhm\003vmr\006vmrgew\006vmrghb\006vmrghh\006vmrg"
    "hw\006vmrglb\006vmrglh\006vmrglw\006vmrgow\010vmsummbm\010vmsumshm\010v"
    "msumshs\010vmsumubm\010vmsumuhm\010vmsumuhs\tvmul10cuq\nvmul10ecuq\tvmu"
    "l10euq\010vmul10uq\007vmulesb\007vmulesh\007vmulesw\007vmuleub\007vmule"
    "uh\007vmuleuw\007vmulosb\007vmulosh\007vmulosw\007vmuloub\007vmulouh\007"
    "vmulouw\007vmuluwm\005vnand\010vncipher\014vncipherlast\005vnegd\005vne"
    "gw\010vnmsubfp\004vnor\004vnot\003vor\004vorc\005vperm\006vpermr\010vpe"
    "rmxor\005vpkpx\007vpksdss\007vpksdus\007vpkshss\007vpkshus\007vpkswss\007"
    "vpkswus\007vpkudum\007vpkudus\007vpkuhum\007vpkuhus\007vpkuwum\007vpkuw"
    "us\007vpmsumb\007vpmsumd\007vpmsumh\007vpmsumw\010vpopcntb\010vpopcntd\010"
    "vpopcnth\010vpopcntw\007vprtybd\007vprtybq\007vprtybw\005vrefp\005vrfim"
    "\005vrfin\005vrfip\005vrfiz\004vrlb\004vrld\006vrldmi\006vrldnm\004vrlh"
    "\004vrlw\006vrlwmi\006vrlwnm\tvrsqrtefp\005vsbox\004vsel\nvshasigmad\nv"
    "shasigmaw\003vsl\004vslb\004vsld\006vsldoi\004vslh\004vslo\004vslv\004v"
    "slw\006vspltb\006vsplth\010vspltisb\010vspltish\010vspltisw\006vspltw\003"
    "vsr\005vsrab\005vsrad\005vsrah\005vsraw\004vsrb\004vsrd\004vsrh\004vsro"
    "\004vsrv\004vsrw\007vsubcuq\007vsubcuw\010vsubecuq\010vsubeuqm\006vsubf"
    "p\007vsubsbs\007vsubshs\007vsubsws\007vsububm\007vsububs\007vsubudm\007"
    "vsubuhm\007vsubuhs\007vsubuqm\007vsubuwm\007vsubuws\010vsum2sws\010vsum"
    "4sbs\010vsum4shs\010vsum4ubs\007vsumsws\007vupkhpx\007vupkhsb\007vupkhs"
    "h\007vupkhsw\007vupklpx\007vupklsb\007vupklsh\007vupklsw\004vxor\004wai"
    "t\010waitimpl\007waitrsv\005wrtee\006wrteei\004xnop\003xor\004xori\005x"
    "oris\007xsabsdp\007xsabsqp\007xsadddp\007xsaddqp\010xsaddqpo\007xsaddsp"
    "\txscmpeqdp\nxscmpexpdp\nxscmpexpqp\txscmpgedp\txscmpgtdp\010xscmpodp\010"
    "xscmpoqp\010xscmpudp\010xscmpuqp\txscpsgndp\txscpsgnqp\010xscvdphp\010x"
    "scvdpqp\010xscvdpsp\txscvdpspn\nxscvdpsxds\nxscvdpsxws\nxscvdpuxds\nxsc"
    "vdpuxws\010xscvhpdp\010xscvqpdp\txscvqpdpo\txscvqpsdz\txscvqpswz\txscvq"
    "pudz\txscvqpuwz\010xscvsdqp\010xscvspdp\txscvspdpn\txscvsxddp\txscvsxds"
    "p\010xscvudqp\txscvuxddp\txscvuxdsp\007xsdivdp\007xsdivqp\010xsdivqpo\007"
    "xsdivsp\010xsiexpdp\010xsiexpqp\txsmaddadp\txsmaddasp\txsmaddmdp\txsmad"
    "dmsp\010xsmaddqp\txsmaddqpo\010xsmaxcdp\007xsmaxdp\010xsmaxjdp\010xsmin"
    "cdp\007xsmindp\010xsminjdp\txsmsubadp\txsmsubasp\txsmsubmdp\txsmsubmsp\010"
    "xsmsubqp\txsmsubqpo\007xsmuldp\007xsmulqp\010xsmulqpo\007xsmulsp\010xsn"
    "absdp\010xsnabsqp\007xsnegdp\007xsnegqp\nxsnmaddadp\nxsnmaddasp\nxsnmad"
    "dmdp\nxsnmaddmsp\txsnmaddqp\nxsnmaddqpo\nxsnmsubadp\nxsnmsubasp\nxsnmsu"
    "bmdp\nxsnmsubmsp\txsnmsubqp\nxsnmsubqpo\006xsrdpi\007xsrdpic\007xsrdpim"
    "\007xsrdpip\007xsrdpiz\006xsredp\006xsresp\006xsrqpi\007xsrqpix\007xsrq"
    "pxp\005xsrsp\nxsrsqrtedp\nxsrsqrtesp\010xssqrtdp\010xssqrtqp\txssqrtqpo"
    "\010xssqrtsp\007xssubdp\007xssubqp\010xssubqpo\007xssubsp\010xstdivdp\t"
    "xstsqrtdp\txststdcdp\txststdcqp\txststdcsp\010xsxexpdp\010xsxexpqp\010x"
    "sxsigdp\010xsxsigqp\007xvabsdp\007xvabssp\007xvadddp\007xvaddsp\txvcmpe"
    "qdp\txvcmpeqsp\txvcmpgedp\txvcmpgesp\txvcmpgtdp\txvcmpgtsp\txvcpsgndp\t"
    "xvcpsgnsp\010xvcvdpsp\nxvcvdpsxds\nxvcvdpsxws\nxvcvdpuxds\nxvcvdpuxws\010"
    "xvcvhpsp\010xvcvspdp\010xvcvsphp\nxvcvspsxds\nxvcvspsxws\nxvcvspuxds\nx"
    "vcvspuxws\txvcvsxddp\txvcvsxdsp\txvcvsxwdp\txvcvsxwsp\txvcvuxddp\txvcvu"
    "xdsp\txvcvuxwdp\txvcvuxwsp\007xvdivdp\007xvdivsp\010xviexpdp\010xviexps"
    "p\txvmaddadp\txvmaddasp\txvmaddmdp\txvmaddmsp\007xvmaxdp\007xvmaxsp\007"
    "xvmindp\007xvminsp\007xvmovdp\007xvmovsp\txvmsubadp\txvmsubasp\txvmsubm"
    "dp\txvmsubmsp\007xvmuldp\007xvmulsp\010xvnabsdp\010xvnabssp\007xvnegdp\007"
    "xvnegsp\nxvnmaddadp\nxvnmaddasp\nxvnmaddmdp\nxvnmaddmsp\nxvnmsubadp\nxv"
    "nmsubasp\nxvnmsubmdp\nxvnmsubmsp\006xvrdpi\007xvrdpic\007xvrdpim\007xvr"
    "dpip\007xvrdpiz\006xvredp\006xvresp\006xvrspi\007xvrspic\007xvrspim\007"
    "xvrspip\007xvrspiz\nxvrsqrtedp\nxvrsqrtesp\010xvsqrtdp\010xvsqrtsp\007x"
    "vsubdp\007xvsubsp\010xvtdivdp\010xvtdivsp\txvtsqrtdp\txvtsqrtsp\txvtstd"
    "cdp\txvtstdcsp\010xvxexpdp\010xvxexpsp\010xvxsigdp\010xvxsigsp\005xxbrd"
    "\005xxbrh\005xxbrq\005xxbrw\013xxextractuw\txxinsertw\006xxland\007xxla"
    "ndc\006xxleqv\007xxlnand\006xxlnor\005xxlor\006xxlorc\006xxlxor\007xxmr"
    "ghd\007xxmrghw\007xxmrgld\007xxmrglw\006xxperm\010xxpermdi\007xxpermr\005"
    "xxsel\007xxsldwi\007xxspltd\010xxspltib\007xxspltw\007xxswapd";

namespace {
  struct MatchEntry {
    uint16_t Mnemonic;
    uint16_t Opcode;
    uint16_t ConvertFn;
    uint8_t RequiredFeatures;
    uint8_t Classes[6];
    StringRef getMnemonic() const {
      return StringRef(MnemonicTable + Mnemonic + 1,
                       MnemonicTable[Mnemonic]);
    }
  };

  // Predicate for searching for an opcode.
  struct LessOpcode {
    bool operator()(const MatchEntry &LHS, StringRef RHS) {
      return LHS.getMnemonic() < RHS;
    }
    bool operator()(StringRef LHS, const MatchEntry &RHS) {
      return LHS < RHS.getMnemonic();
    }
    bool operator()(const MatchEntry &LHS, const MatchEntry &RHS) {
      return LHS.getMnemonic() < RHS.getMnemonic();
    }
  };
} // end anonymous namespace.

static const MatchEntry MatchTable0[] = {
  { 0 /* add */, PPC::ADD8TLS_, Convert__RegG8RC1_0__RegG8RC1_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_TLSReg }, },
  { 0 /* add */, PPC::ADD4, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 0 /* add */, PPC::ADD4o, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 4 /* addc */, PPC::ADDC, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 4 /* addc */, PPC::ADDCo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9 /* adde */, PPC::ADDE, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9 /* adde */, PPC::ADDEo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 14 /* addi */, PPC::ADDI, Convert__RegGPRC1_0__RegGPRCNoR01_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRCNoR0, MCK_S16Imm }, },
  { 19 /* addic */, PPC::ADDIC, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 19 /* addic */, PPC::ADDICo, Convert__RegGPRC1_1__RegGPRC1_2__S16Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 25 /* addis */, PPC::ADDIS, Convert__RegGPRC1_0__RegGPRCNoR01_1__S17Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRCNoR0, MCK_S17Imm }, },
  { 31 /* addme */, PPC::ADDME, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 31 /* addme */, PPC::ADDMEo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 37 /* addpcis */, PPC::ADDPCIS, Convert__RegG8RC1_0__Imm1_1, 0, { MCK_RegG8RC, MCK_Imm }, },
  { 45 /* addze */, PPC::ADDZE, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 45 /* addze */, PPC::ADDZEo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 51 /* and */, PPC::AND, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 51 /* and */, PPC::ANDo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 55 /* andc */, PPC::ANDC, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 55 /* andc */, PPC::ANDCo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 60 /* andi */, PPC::ANDIo, Convert__RegGPRC1_1__RegGPRC1_2__U16Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 65 /* andis */, PPC::ANDISo, Convert__RegGPRC1_1__RegGPRC1_2__U16Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 71 /* attn */, PPC::ATTN, Convert_NoOperands, 0, {  }, },
  { 76 /* b */, PPC::B, Convert__DirectBr1_0, 0, { MCK_DirectBr }, },
  { 78 /* ba */, PPC::BA, Convert__DirectBr1_0, 0, { MCK_DirectBr }, },
  { 81 /* bc */, PPC::gBC, Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 81 /* bc */, PPC::gBCat, Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3, 0, { MCK_ATBitsAsHint, MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 84 /* bc+ */, PPC::gBCat, Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 88 /* bc- */, PPC::gBCat, Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 92 /* bca */, PPC::gBCA, Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 92 /* bca */, PPC::gBCAat, Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3, 0, { MCK_ATBitsAsHint, MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 96 /* bca+ */, PPC::gBCAat, Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 101 /* bca- */, PPC::gBCAat, Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 106 /* bcctr */, PPC::gBCCTR, Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0, 0, { MCK_U5Imm, MCK_RegCRBITRC }, },
  { 106 /* bcctr */, PPC::gBCCTR, Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_Imm }, },
  { 112 /* bcctrl */, PPC::gBCCTRL, Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0, 0, { MCK_U5Imm, MCK_RegCRBITRC }, },
  { 112 /* bcctrl */, PPC::gBCCTRL, Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_Imm }, },
  { 119 /* bcdcfn */, PPC::BCDCFNo, Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 126 /* bcdcfsq */, PPC::BCDCFSQo, Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 134 /* bcdcfz */, PPC::BCDCFZo, Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 141 /* bcdcpsgn */, PPC::BCDCPSGNo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 150 /* bcdctn */, PPC::BCDCTNo, Convert__RegVRRC1_1__RegVRRC1_2, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC }, },
  { 157 /* bcdctsq */, PPC::BCDCTSQo, Convert__RegVRRC1_1__RegVRRC1_2, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC }, },
  { 165 /* bcdctz */, PPC::BCDCTZo, Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 172 /* bcds */, PPC::BCDSo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3__U1Imm1_4, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 177 /* bcdsetsgn */, PPC::BCDSETSGNo, Convert__RegVRRC1_1__RegVRRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 187 /* bcdsr */, PPC::BCDSRo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3__U1Imm1_4, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 193 /* bcdtrunc */, PPC::BCDTRUNCo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3__U1Imm1_4, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm }, },
  { 202 /* bcdus */, PPC::BCDUSo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 208 /* bcdutrunc */, PPC::BCDUTRUNCo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 218 /* bcl */, PPC::gBCL, Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 218 /* bcl */, PPC::gBCLat, Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3, 0, { MCK_ATBitsAsHint, MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 222 /* bcl+ */, PPC::gBCLat, Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 227 /* bcl- */, PPC::gBCLat, Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 232 /* bcla */, PPC::gBCLA, Convert__U5Imm1_0__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 232 /* bcla */, PPC::gBCLAat, Convert__U5Imm1_1__ATBitsAsHint1_0__RegCRBITRC1_2__CondBr1_3, 0, { MCK_ATBitsAsHint, MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 237 /* bcla+ */, PPC::gBCLAat, Convert__U5Imm1_0__imm_95_3__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 243 /* bcla- */, PPC::gBCLAat, Convert__U5Imm1_0__imm_95_2__RegCRBITRC1_1__CondBr1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_CondBr }, },
  { 249 /* bclr */, PPC::gBCLR, Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0, 0, { MCK_U5Imm, MCK_RegCRBITRC }, },
  { 249 /* bclr */, PPC::gBCLR, Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_Imm }, },
  { 254 /* bclrl */, PPC::gBCLRL, Convert__U5Imm1_0__RegCRBITRC1_1__imm_95_0, 0, { MCK_U5Imm, MCK_RegCRBITRC }, },
  { 254 /* bclrl */, PPC::gBCLRL, Convert__U5Imm1_0__RegCRBITRC1_1__Imm1_2, 0, { MCK_U5Imm, MCK_RegCRBITRC, MCK_Imm }, },
  { 260 /* bctr */, PPC::BCTR, Convert_NoOperands, 0, {  }, },
  { 265 /* bctrl */, PPC::BCTRL, Convert_NoOperands, 0, {  }, },
  { 271 /* bdnz */, PPC::BDNZ, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 276 /* bdnz+ */, PPC::BDNZp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 282 /* bdnz- */, PPC::BDNZm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 288 /* bdnza */, PPC::BDNZA, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 294 /* bdnza+ */, PPC::BDNZAp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 301 /* bdnza- */, PPC::BDNZAm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 308 /* bdnzf */, PPC::gBC, Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 314 /* bdnzfa */, PPC::gBCA, Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 321 /* bdnzfl */, PPC::gBCL, Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 328 /* bdnzfla */, PPC::gBCLA, Convert__imm_95_0__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 336 /* bdnzflr */, PPC::gBCLR, Convert__imm_95_0__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 344 /* bdnzflrl */, PPC::gBCLRL, Convert__imm_95_0__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 353 /* bdnzl */, PPC::BDNZL, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 359 /* bdnzl+ */, PPC::BDNZLp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 366 /* bdnzl- */, PPC::BDNZLm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 373 /* bdnzla */, PPC::BDNZLA, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 380 /* bdnzla+ */, PPC::BDNZLAp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 388 /* bdnzla- */, PPC::BDNZLAm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 396 /* bdnzlr */, PPC::BDNZLR, Convert_NoOperands, 0, {  }, },
  { 403 /* bdnzlr+ */, PPC::BDNZLRp, Convert_NoOperands, 0, {  }, },
  { 411 /* bdnzlr- */, PPC::BDNZLRm, Convert_NoOperands, 0, {  }, },
  { 419 /* bdnzlrl */, PPC::BDNZLRL, Convert_NoOperands, 0, {  }, },
  { 427 /* bdnzlrl+ */, PPC::BDNZLRLp, Convert_NoOperands, 0, {  }, },
  { 436 /* bdnzlrl- */, PPC::BDNZLRLm, Convert_NoOperands, 0, {  }, },
  { 445 /* bdnzt */, PPC::gBC, Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 451 /* bdnzta */, PPC::gBCA, Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 458 /* bdnztl */, PPC::gBCL, Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 465 /* bdnztla */, PPC::gBCLA, Convert__imm_95_8__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 473 /* bdnztlr */, PPC::gBCLR, Convert__imm_95_8__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 481 /* bdnztlrl */, PPC::gBCLRL, Convert__imm_95_8__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 490 /* bdz */, PPC::BDZ, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 494 /* bdz+ */, PPC::BDZp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 499 /* bdz- */, PPC::BDZm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 504 /* bdza */, PPC::BDZA, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 509 /* bdza+ */, PPC::BDZAp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 515 /* bdza- */, PPC::BDZAm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 521 /* bdzf */, PPC::gBC, Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 526 /* bdzfa */, PPC::gBCA, Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 532 /* bdzfl */, PPC::gBCL, Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 538 /* bdzfla */, PPC::gBCLA, Convert__imm_95_2__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 545 /* bdzflr */, PPC::gBCLR, Convert__imm_95_2__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 552 /* bdzflrl */, PPC::gBCLRL, Convert__imm_95_2__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 560 /* bdzl */, PPC::BDZL, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 565 /* bdzl+ */, PPC::BDZLp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 571 /* bdzl- */, PPC::BDZLm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 577 /* bdzla */, PPC::BDZLA, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 583 /* bdzla+ */, PPC::BDZLAp, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 590 /* bdzla- */, PPC::BDZLAm, Convert__CondBr1_0, 0, { MCK_CondBr }, },
  { 597 /* bdzlr */, PPC::BDZLR, Convert_NoOperands, 0, {  }, },
  { 603 /* bdzlr+ */, PPC::BDZLRp, Convert_NoOperands, 0, {  }, },
  { 610 /* bdzlr- */, PPC::BDZLRm, Convert_NoOperands, 0, {  }, },
  { 617 /* bdzlrl */, PPC::BDZLRL, Convert_NoOperands, 0, {  }, },
  { 624 /* bdzlrl+ */, PPC::BDZLRLp, Convert_NoOperands, 0, {  }, },
  { 632 /* bdzlrl- */, PPC::BDZLRLm, Convert_NoOperands, 0, {  }, },
  { 640 /* bdzt */, PPC::gBC, Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 645 /* bdzta */, PPC::gBCA, Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 651 /* bdztl */, PPC::gBCL, Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 657 /* bdztla */, PPC::gBCLA, Convert__imm_95_10__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 664 /* bdztlr */, PPC::gBCLR, Convert__imm_95_10__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 671 /* bdztlrl */, PPC::gBCLRL, Convert__imm_95_10__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 679 /* beq */, PPC::BCC, Convert__imm_95_76__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 679 /* beq */, PPC::BCC, Convert__imm_95_76__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 683 /* beq+ */, PPC::BCC, Convert__imm_95_79__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 683 /* beq+ */, PPC::BCC, Convert__imm_95_79__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 688 /* beq- */, PPC::BCC, Convert__imm_95_78__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 688 /* beq- */, PPC::BCC, Convert__imm_95_78__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 693 /* beqa */, PPC::BCCA, Convert__imm_95_76__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 693 /* beqa */, PPC::BCCA, Convert__imm_95_76__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 698 /* beqa+ */, PPC::BCCA, Convert__imm_95_79__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 698 /* beqa+ */, PPC::BCCA, Convert__imm_95_79__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 704 /* beqa- */, PPC::BCCA, Convert__imm_95_78__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 704 /* beqa- */, PPC::BCCA, Convert__imm_95_78__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 710 /* beqctr */, PPC::BCCCTR, Convert__imm_95_76__regCR0, 0, {  }, },
  { 710 /* beqctr */, PPC::BCCCTR, Convert__imm_95_76__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 717 /* beqctr+ */, PPC::BCCCTR, Convert__imm_95_79__regCR0, 0, {  }, },
  { 717 /* beqctr+ */, PPC::BCCCTR, Convert__imm_95_79__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 725 /* beqctr- */, PPC::BCCCTR, Convert__imm_95_78__regCR0, 0, {  }, },
  { 725 /* beqctr- */, PPC::BCCCTR, Convert__imm_95_78__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 733 /* beqctrl */, PPC::BCCCTRL, Convert__imm_95_76__regCR0, 0, {  }, },
  { 733 /* beqctrl */, PPC::BCCCTRL, Convert__imm_95_76__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 741 /* beqctrl+ */, PPC::BCCCTRL, Convert__imm_95_79__regCR0, 0, {  }, },
  { 741 /* beqctrl+ */, PPC::BCCCTRL, Convert__imm_95_79__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 750 /* beqctrl- */, PPC::BCCCTRL, Convert__imm_95_78__regCR0, 0, {  }, },
  { 750 /* beqctrl- */, PPC::BCCCTRL, Convert__imm_95_78__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 759 /* beql */, PPC::BCCL, Convert__imm_95_76__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 759 /* beql */, PPC::BCCL, Convert__imm_95_76__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 764 /* beql+ */, PPC::BCCL, Convert__imm_95_79__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 764 /* beql+ */, PPC::BCCL, Convert__imm_95_79__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 770 /* beql- */, PPC::BCCL, Convert__imm_95_78__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 770 /* beql- */, PPC::BCCL, Convert__imm_95_78__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 776 /* beqla */, PPC::BCCLA, Convert__imm_95_76__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 776 /* beqla */, PPC::BCCLA, Convert__imm_95_76__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 782 /* beqla+ */, PPC::BCCLA, Convert__imm_95_79__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 782 /* beqla+ */, PPC::BCCLA, Convert__imm_95_79__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 789 /* beqla- */, PPC::BCCLA, Convert__imm_95_78__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 789 /* beqla- */, PPC::BCCLA, Convert__imm_95_78__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 796 /* beqlr */, PPC::BCCLR, Convert__imm_95_76__regCR0, 0, {  }, },
  { 796 /* beqlr */, PPC::BCCLR, Convert__imm_95_76__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 802 /* beqlr+ */, PPC::BCCLR, Convert__imm_95_79__regCR0, 0, {  }, },
  { 802 /* beqlr+ */, PPC::BCCLR, Convert__imm_95_79__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 809 /* beqlr- */, PPC::BCCLR, Convert__imm_95_78__regCR0, 0, {  }, },
  { 809 /* beqlr- */, PPC::BCCLR, Convert__imm_95_78__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 816 /* beqlrl */, PPC::BCCLRL, Convert__imm_95_76__regCR0, 0, {  }, },
  { 816 /* beqlrl */, PPC::BCCLRL, Convert__imm_95_76__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 823 /* beqlrl+ */, PPC::BCCLRL, Convert__imm_95_79__regCR0, 0, {  }, },
  { 823 /* beqlrl+ */, PPC::BCCLRL, Convert__imm_95_79__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 831 /* beqlrl- */, PPC::BCCLRL, Convert__imm_95_78__regCR0, 0, {  }, },
  { 831 /* beqlrl- */, PPC::BCCLRL, Convert__imm_95_78__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 839 /* bf */, PPC::gBC, Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 842 /* bf+ */, PPC::gBC, Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 846 /* bf- */, PPC::gBC, Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 850 /* bfa */, PPC::gBCA, Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 854 /* bfa+ */, PPC::gBCA, Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 859 /* bfa- */, PPC::gBCA, Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 864 /* bfctr */, PPC::gBCCTR, Convert__imm_95_4__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 870 /* bfctr+ */, PPC::gBCCTR, Convert__imm_95_7__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 877 /* bfctr- */, PPC::gBCCTR, Convert__imm_95_6__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 884 /* bfctrl */, PPC::gBCCTRL, Convert__imm_95_4__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 891 /* bfctrl+ */, PPC::gBCCTRL, Convert__imm_95_7__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 899 /* bfctrl- */, PPC::gBCCTRL, Convert__imm_95_6__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 907 /* bfl */, PPC::gBCL, Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 911 /* bfl+ */, PPC::gBCL, Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 916 /* bfl- */, PPC::gBCL, Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 921 /* bfla */, PPC::gBCLA, Convert__imm_95_4__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 926 /* bfla+ */, PPC::gBCLA, Convert__imm_95_7__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 932 /* bfla- */, PPC::gBCLA, Convert__imm_95_6__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 938 /* bflr */, PPC::gBCLR, Convert__imm_95_4__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 943 /* bflr+ */, PPC::gBCLR, Convert__imm_95_7__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 949 /* bflr- */, PPC::gBCLR, Convert__imm_95_6__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 955 /* bflrl */, PPC::gBCLRL, Convert__imm_95_4__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 961 /* bflrl+ */, PPC::gBCLRL, Convert__imm_95_7__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 968 /* bflrl- */, PPC::gBCLRL, Convert__imm_95_6__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 975 /* bge */, PPC::BCC, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 975 /* bge */, PPC::BCC, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 979 /* bge+ */, PPC::BCC, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 979 /* bge+ */, PPC::BCC, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 984 /* bge- */, PPC::BCC, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 984 /* bge- */, PPC::BCC, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 989 /* bgea */, PPC::BCCA, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 989 /* bgea */, PPC::BCCA, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 994 /* bgea+ */, PPC::BCCA, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 994 /* bgea+ */, PPC::BCCA, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1000 /* bgea- */, PPC::BCCA, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1000 /* bgea- */, PPC::BCCA, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1006 /* bgectr */, PPC::BCCCTR, Convert__imm_95_4__regCR0, 0, {  }, },
  { 1006 /* bgectr */, PPC::BCCCTR, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1013 /* bgectr+ */, PPC::BCCCTR, Convert__imm_95_7__regCR0, 0, {  }, },
  { 1013 /* bgectr+ */, PPC::BCCCTR, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1021 /* bgectr- */, PPC::BCCCTR, Convert__imm_95_6__regCR0, 0, {  }, },
  { 1021 /* bgectr- */, PPC::BCCCTR, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1029 /* bgectrl */, PPC::BCCCTRL, Convert__imm_95_4__regCR0, 0, {  }, },
  { 1029 /* bgectrl */, PPC::BCCCTRL, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1037 /* bgectrl+ */, PPC::BCCCTRL, Convert__imm_95_7__regCR0, 0, {  }, },
  { 1037 /* bgectrl+ */, PPC::BCCCTRL, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1046 /* bgectrl- */, PPC::BCCCTRL, Convert__imm_95_6__regCR0, 0, {  }, },
  { 1046 /* bgectrl- */, PPC::BCCCTRL, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1055 /* bgel */, PPC::BCCL, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1055 /* bgel */, PPC::BCCL, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1060 /* bgel+ */, PPC::BCCL, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1060 /* bgel+ */, PPC::BCCL, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1066 /* bgel- */, PPC::BCCL, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1066 /* bgel- */, PPC::BCCL, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1072 /* bgela */, PPC::BCCLA, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1072 /* bgela */, PPC::BCCLA, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1078 /* bgela+ */, PPC::BCCLA, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1078 /* bgela+ */, PPC::BCCLA, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1085 /* bgela- */, PPC::BCCLA, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1085 /* bgela- */, PPC::BCCLA, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1092 /* bgelr */, PPC::BCCLR, Convert__imm_95_4__regCR0, 0, {  }, },
  { 1092 /* bgelr */, PPC::BCCLR, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1098 /* bgelr+ */, PPC::BCCLR, Convert__imm_95_7__regCR0, 0, {  }, },
  { 1098 /* bgelr+ */, PPC::BCCLR, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1105 /* bgelr- */, PPC::BCCLR, Convert__imm_95_6__regCR0, 0, {  }, },
  { 1105 /* bgelr- */, PPC::BCCLR, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1112 /* bgelrl */, PPC::BCCLRL, Convert__imm_95_4__regCR0, 0, {  }, },
  { 1112 /* bgelrl */, PPC::BCCLRL, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1119 /* bgelrl+ */, PPC::BCCLRL, Convert__imm_95_7__regCR0, 0, {  }, },
  { 1119 /* bgelrl+ */, PPC::BCCLRL, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1127 /* bgelrl- */, PPC::BCCLRL, Convert__imm_95_6__regCR0, 0, {  }, },
  { 1127 /* bgelrl- */, PPC::BCCLRL, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1135 /* bgt */, PPC::BCC, Convert__imm_95_44__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1135 /* bgt */, PPC::BCC, Convert__imm_95_44__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1139 /* bgt+ */, PPC::BCC, Convert__imm_95_47__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1139 /* bgt+ */, PPC::BCC, Convert__imm_95_47__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1144 /* bgt- */, PPC::BCC, Convert__imm_95_46__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1144 /* bgt- */, PPC::BCC, Convert__imm_95_46__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1149 /* bgta */, PPC::BCCA, Convert__imm_95_44__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1149 /* bgta */, PPC::BCCA, Convert__imm_95_44__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1154 /* bgta+ */, PPC::BCCA, Convert__imm_95_47__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1154 /* bgta+ */, PPC::BCCA, Convert__imm_95_47__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1160 /* bgta- */, PPC::BCCA, Convert__imm_95_46__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1160 /* bgta- */, PPC::BCCA, Convert__imm_95_46__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1166 /* bgtctr */, PPC::BCCCTR, Convert__imm_95_44__regCR0, 0, {  }, },
  { 1166 /* bgtctr */, PPC::BCCCTR, Convert__imm_95_44__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1173 /* bgtctr+ */, PPC::BCCCTR, Convert__imm_95_47__regCR0, 0, {  }, },
  { 1173 /* bgtctr+ */, PPC::BCCCTR, Convert__imm_95_47__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1181 /* bgtctr- */, PPC::BCCCTR, Convert__imm_95_46__regCR0, 0, {  }, },
  { 1181 /* bgtctr- */, PPC::BCCCTR, Convert__imm_95_46__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1189 /* bgtctrl */, PPC::BCCCTRL, Convert__imm_95_44__regCR0, 0, {  }, },
  { 1189 /* bgtctrl */, PPC::BCCCTRL, Convert__imm_95_44__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1197 /* bgtctrl+ */, PPC::BCCCTRL, Convert__imm_95_47__regCR0, 0, {  }, },
  { 1197 /* bgtctrl+ */, PPC::BCCCTRL, Convert__imm_95_47__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1206 /* bgtctrl- */, PPC::BCCCTRL, Convert__imm_95_46__regCR0, 0, {  }, },
  { 1206 /* bgtctrl- */, PPC::BCCCTRL, Convert__imm_95_46__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1215 /* bgtl */, PPC::BCCL, Convert__imm_95_44__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1215 /* bgtl */, PPC::BCCL, Convert__imm_95_44__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1220 /* bgtl+ */, PPC::BCCL, Convert__imm_95_47__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1220 /* bgtl+ */, PPC::BCCL, Convert__imm_95_47__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1226 /* bgtl- */, PPC::BCCL, Convert__imm_95_46__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1226 /* bgtl- */, PPC::BCCL, Convert__imm_95_46__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1232 /* bgtla */, PPC::BCCLA, Convert__imm_95_44__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1232 /* bgtla */, PPC::BCCLA, Convert__imm_95_44__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1238 /* bgtla+ */, PPC::BCCLA, Convert__imm_95_47__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1238 /* bgtla+ */, PPC::BCCLA, Convert__imm_95_47__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1245 /* bgtla- */, PPC::BCCLA, Convert__imm_95_46__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1245 /* bgtla- */, PPC::BCCLA, Convert__imm_95_46__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1252 /* bgtlr */, PPC::BCCLR, Convert__imm_95_44__regCR0, 0, {  }, },
  { 1252 /* bgtlr */, PPC::BCCLR, Convert__imm_95_44__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1258 /* bgtlr+ */, PPC::BCCLR, Convert__imm_95_47__regCR0, 0, {  }, },
  { 1258 /* bgtlr+ */, PPC::BCCLR, Convert__imm_95_47__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1265 /* bgtlr- */, PPC::BCCLR, Convert__imm_95_46__regCR0, 0, {  }, },
  { 1265 /* bgtlr- */, PPC::BCCLR, Convert__imm_95_46__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1272 /* bgtlrl */, PPC::BCCLRL, Convert__imm_95_44__regCR0, 0, {  }, },
  { 1272 /* bgtlrl */, PPC::BCCLRL, Convert__imm_95_44__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1279 /* bgtlrl+ */, PPC::BCCLRL, Convert__imm_95_47__regCR0, 0, {  }, },
  { 1279 /* bgtlrl+ */, PPC::BCCLRL, Convert__imm_95_47__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1287 /* bgtlrl- */, PPC::BCCLRL, Convert__imm_95_46__regCR0, 0, {  }, },
  { 1287 /* bgtlrl- */, PPC::BCCLRL, Convert__imm_95_46__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1295 /* bl */, PPC::BL, Convert__DirectBr1_0, 0, { MCK_DirectBr }, },
  { 1295 /* bl */, PPC::BL8_TLS_, Convert__DirectBr1_0__Imm1_1, 0, { MCK_DirectBr, MCK_Imm }, },
  { 1298 /* bla */, PPC::BLA, Convert__DirectBr1_0, 0, { MCK_DirectBr }, },
  { 1302 /* ble */, PPC::BCC, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1302 /* ble */, PPC::BCC, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1306 /* ble+ */, PPC::BCC, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1306 /* ble+ */, PPC::BCC, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1311 /* ble- */, PPC::BCC, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1311 /* ble- */, PPC::BCC, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1316 /* blea */, PPC::BCCA, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1316 /* blea */, PPC::BCCA, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1321 /* blea+ */, PPC::BCCA, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1321 /* blea+ */, PPC::BCCA, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1327 /* blea- */, PPC::BCCA, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1327 /* blea- */, PPC::BCCA, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1333 /* blectr */, PPC::BCCCTR, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1333 /* blectr */, PPC::BCCCTR, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1340 /* blectr+ */, PPC::BCCCTR, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1340 /* blectr+ */, PPC::BCCCTR, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1348 /* blectr- */, PPC::BCCCTR, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1348 /* blectr- */, PPC::BCCCTR, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1356 /* blectrl */, PPC::BCCCTRL, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1356 /* blectrl */, PPC::BCCCTRL, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1364 /* blectrl+ */, PPC::BCCCTRL, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1364 /* blectrl+ */, PPC::BCCCTRL, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1373 /* blectrl- */, PPC::BCCCTRL, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1373 /* blectrl- */, PPC::BCCCTRL, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1382 /* blel */, PPC::BCCL, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1382 /* blel */, PPC::BCCL, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1387 /* blel+ */, PPC::BCCL, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1387 /* blel+ */, PPC::BCCL, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1393 /* blel- */, PPC::BCCL, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1393 /* blel- */, PPC::BCCL, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1399 /* blela */, PPC::BCCLA, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1399 /* blela */, PPC::BCCLA, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1405 /* blela+ */, PPC::BCCLA, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1405 /* blela+ */, PPC::BCCLA, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1412 /* blela- */, PPC::BCCLA, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1412 /* blela- */, PPC::BCCLA, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1419 /* blelr */, PPC::BCCLR, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1419 /* blelr */, PPC::BCCLR, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1425 /* blelr+ */, PPC::BCCLR, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1425 /* blelr+ */, PPC::BCCLR, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1432 /* blelr- */, PPC::BCCLR, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1432 /* blelr- */, PPC::BCCLR, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1439 /* blelrl */, PPC::BCCLRL, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1439 /* blelrl */, PPC::BCCLRL, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1446 /* blelrl+ */, PPC::BCCLRL, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1446 /* blelrl+ */, PPC::BCCLRL, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1454 /* blelrl- */, PPC::BCCLRL, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1454 /* blelrl- */, PPC::BCCLRL, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1462 /* blr */, PPC::BLR, Convert_NoOperands, 0, {  }, },
  { 1466 /* blrl */, PPC::BLRL, Convert_NoOperands, 0, {  }, },
  { 1471 /* blt */, PPC::BCC, Convert__imm_95_12__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1471 /* blt */, PPC::BCC, Convert__imm_95_12__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1475 /* blt+ */, PPC::BCC, Convert__imm_95_15__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1475 /* blt+ */, PPC::BCC, Convert__imm_95_15__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1480 /* blt- */, PPC::BCC, Convert__imm_95_14__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1480 /* blt- */, PPC::BCC, Convert__imm_95_14__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1485 /* blta */, PPC::BCCA, Convert__imm_95_12__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1485 /* blta */, PPC::BCCA, Convert__imm_95_12__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1490 /* blta+ */, PPC::BCCA, Convert__imm_95_15__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1490 /* blta+ */, PPC::BCCA, Convert__imm_95_15__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1496 /* blta- */, PPC::BCCA, Convert__imm_95_14__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1496 /* blta- */, PPC::BCCA, Convert__imm_95_14__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1502 /* bltctr */, PPC::BCCCTR, Convert__imm_95_12__regCR0, 0, {  }, },
  { 1502 /* bltctr */, PPC::BCCCTR, Convert__imm_95_12__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1509 /* bltctr+ */, PPC::BCCCTR, Convert__imm_95_15__regCR0, 0, {  }, },
  { 1509 /* bltctr+ */, PPC::BCCCTR, Convert__imm_95_15__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1517 /* bltctr- */, PPC::BCCCTR, Convert__imm_95_14__regCR0, 0, {  }, },
  { 1517 /* bltctr- */, PPC::BCCCTR, Convert__imm_95_14__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1525 /* bltctrl */, PPC::BCCCTRL, Convert__imm_95_12__regCR0, 0, {  }, },
  { 1525 /* bltctrl */, PPC::BCCCTRL, Convert__imm_95_12__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1533 /* bltctrl+ */, PPC::BCCCTRL, Convert__imm_95_15__regCR0, 0, {  }, },
  { 1533 /* bltctrl+ */, PPC::BCCCTRL, Convert__imm_95_15__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1542 /* bltctrl- */, PPC::BCCCTRL, Convert__imm_95_14__regCR0, 0, {  }, },
  { 1542 /* bltctrl- */, PPC::BCCCTRL, Convert__imm_95_14__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1551 /* bltl */, PPC::BCCL, Convert__imm_95_12__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1551 /* bltl */, PPC::BCCL, Convert__imm_95_12__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1556 /* bltl+ */, PPC::BCCL, Convert__imm_95_15__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1556 /* bltl+ */, PPC::BCCL, Convert__imm_95_15__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1562 /* bltl- */, PPC::BCCL, Convert__imm_95_14__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1562 /* bltl- */, PPC::BCCL, Convert__imm_95_14__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1568 /* bltla */, PPC::BCCLA, Convert__imm_95_12__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1568 /* bltla */, PPC::BCCLA, Convert__imm_95_12__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1574 /* bltla+ */, PPC::BCCLA, Convert__imm_95_15__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1574 /* bltla+ */, PPC::BCCLA, Convert__imm_95_15__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1581 /* bltla- */, PPC::BCCLA, Convert__imm_95_14__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1581 /* bltla- */, PPC::BCCLA, Convert__imm_95_14__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1588 /* bltlr */, PPC::BCCLR, Convert__imm_95_12__regCR0, 0, {  }, },
  { 1588 /* bltlr */, PPC::BCCLR, Convert__imm_95_12__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1594 /* bltlr+ */, PPC::BCCLR, Convert__imm_95_15__regCR0, 0, {  }, },
  { 1594 /* bltlr+ */, PPC::BCCLR, Convert__imm_95_15__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1601 /* bltlr- */, PPC::BCCLR, Convert__imm_95_14__regCR0, 0, {  }, },
  { 1601 /* bltlr- */, PPC::BCCLR, Convert__imm_95_14__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1608 /* bltlrl */, PPC::BCCLRL, Convert__imm_95_12__regCR0, 0, {  }, },
  { 1608 /* bltlrl */, PPC::BCCLRL, Convert__imm_95_12__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1615 /* bltlrl+ */, PPC::BCCLRL, Convert__imm_95_15__regCR0, 0, {  }, },
  { 1615 /* bltlrl+ */, PPC::BCCLRL, Convert__imm_95_15__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1623 /* bltlrl- */, PPC::BCCLRL, Convert__imm_95_14__regCR0, 0, {  }, },
  { 1623 /* bltlrl- */, PPC::BCCLRL, Convert__imm_95_14__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1631 /* bne */, PPC::BCC, Convert__imm_95_68__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1631 /* bne */, PPC::BCC, Convert__imm_95_68__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1635 /* bne+ */, PPC::BCC, Convert__imm_95_71__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1635 /* bne+ */, PPC::BCC, Convert__imm_95_71__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1640 /* bne- */, PPC::BCC, Convert__imm_95_70__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1640 /* bne- */, PPC::BCC, Convert__imm_95_70__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1645 /* bnea */, PPC::BCCA, Convert__imm_95_68__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1645 /* bnea */, PPC::BCCA, Convert__imm_95_68__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1650 /* bnea+ */, PPC::BCCA, Convert__imm_95_71__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1650 /* bnea+ */, PPC::BCCA, Convert__imm_95_71__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1656 /* bnea- */, PPC::BCCA, Convert__imm_95_70__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1656 /* bnea- */, PPC::BCCA, Convert__imm_95_70__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1662 /* bnectr */, PPC::BCCCTR, Convert__imm_95_68__regCR0, 0, {  }, },
  { 1662 /* bnectr */, PPC::BCCCTR, Convert__imm_95_68__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1669 /* bnectr+ */, PPC::BCCCTR, Convert__imm_95_71__regCR0, 0, {  }, },
  { 1669 /* bnectr+ */, PPC::BCCCTR, Convert__imm_95_71__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1677 /* bnectr- */, PPC::BCCCTR, Convert__imm_95_70__regCR0, 0, {  }, },
  { 1677 /* bnectr- */, PPC::BCCCTR, Convert__imm_95_70__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1685 /* bnectrl */, PPC::BCCCTRL, Convert__imm_95_68__regCR0, 0, {  }, },
  { 1685 /* bnectrl */, PPC::BCCCTRL, Convert__imm_95_68__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1693 /* bnectrl+ */, PPC::BCCCTRL, Convert__imm_95_71__regCR0, 0, {  }, },
  { 1693 /* bnectrl+ */, PPC::BCCCTRL, Convert__imm_95_71__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1702 /* bnectrl- */, PPC::BCCCTRL, Convert__imm_95_70__regCR0, 0, {  }, },
  { 1702 /* bnectrl- */, PPC::BCCCTRL, Convert__imm_95_70__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1711 /* bnel */, PPC::BCCL, Convert__imm_95_68__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1711 /* bnel */, PPC::BCCL, Convert__imm_95_68__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1716 /* bnel+ */, PPC::BCCL, Convert__imm_95_71__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1716 /* bnel+ */, PPC::BCCL, Convert__imm_95_71__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1722 /* bnel- */, PPC::BCCL, Convert__imm_95_70__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1722 /* bnel- */, PPC::BCCL, Convert__imm_95_70__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1728 /* bnela */, PPC::BCCLA, Convert__imm_95_68__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1728 /* bnela */, PPC::BCCLA, Convert__imm_95_68__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1734 /* bnela+ */, PPC::BCCLA, Convert__imm_95_71__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1734 /* bnela+ */, PPC::BCCLA, Convert__imm_95_71__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1741 /* bnela- */, PPC::BCCLA, Convert__imm_95_70__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1741 /* bnela- */, PPC::BCCLA, Convert__imm_95_70__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1748 /* bnelr */, PPC::BCCLR, Convert__imm_95_68__regCR0, 0, {  }, },
  { 1748 /* bnelr */, PPC::BCCLR, Convert__imm_95_68__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1754 /* bnelr+ */, PPC::BCCLR, Convert__imm_95_71__regCR0, 0, {  }, },
  { 1754 /* bnelr+ */, PPC::BCCLR, Convert__imm_95_71__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1761 /* bnelr- */, PPC::BCCLR, Convert__imm_95_70__regCR0, 0, {  }, },
  { 1761 /* bnelr- */, PPC::BCCLR, Convert__imm_95_70__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1768 /* bnelrl */, PPC::BCCLRL, Convert__imm_95_68__regCR0, 0, {  }, },
  { 1768 /* bnelrl */, PPC::BCCLRL, Convert__imm_95_68__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1775 /* bnelrl+ */, PPC::BCCLRL, Convert__imm_95_71__regCR0, 0, {  }, },
  { 1775 /* bnelrl+ */, PPC::BCCLRL, Convert__imm_95_71__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1783 /* bnelrl- */, PPC::BCCLRL, Convert__imm_95_70__regCR0, 0, {  }, },
  { 1783 /* bnelrl- */, PPC::BCCLRL, Convert__imm_95_70__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1791 /* bng */, PPC::BCC, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1791 /* bng */, PPC::BCC, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1795 /* bng+ */, PPC::BCC, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1795 /* bng+ */, PPC::BCC, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1800 /* bng- */, PPC::BCC, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1800 /* bng- */, PPC::BCC, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1805 /* bnga */, PPC::BCCA, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1805 /* bnga */, PPC::BCCA, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1810 /* bnga+ */, PPC::BCCA, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1810 /* bnga+ */, PPC::BCCA, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1816 /* bnga- */, PPC::BCCA, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1816 /* bnga- */, PPC::BCCA, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1822 /* bngctr */, PPC::BCCCTR, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1822 /* bngctr */, PPC::BCCCTR, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1829 /* bngctr+ */, PPC::BCCCTR, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1829 /* bngctr+ */, PPC::BCCCTR, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1837 /* bngctr- */, PPC::BCCCTR, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1837 /* bngctr- */, PPC::BCCCTR, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1845 /* bngctrl */, PPC::BCCCTRL, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1845 /* bngctrl */, PPC::BCCCTRL, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1853 /* bngctrl+ */, PPC::BCCCTRL, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1853 /* bngctrl+ */, PPC::BCCCTRL, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1862 /* bngctrl- */, PPC::BCCCTRL, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1862 /* bngctrl- */, PPC::BCCCTRL, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1871 /* bngl */, PPC::BCCL, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1871 /* bngl */, PPC::BCCL, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1876 /* bngl+ */, PPC::BCCL, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1876 /* bngl+ */, PPC::BCCL, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1882 /* bngl- */, PPC::BCCL, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1882 /* bngl- */, PPC::BCCL, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1888 /* bngla */, PPC::BCCLA, Convert__imm_95_36__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1888 /* bngla */, PPC::BCCLA, Convert__imm_95_36__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1894 /* bngla+ */, PPC::BCCLA, Convert__imm_95_39__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1894 /* bngla+ */, PPC::BCCLA, Convert__imm_95_39__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1901 /* bngla- */, PPC::BCCLA, Convert__imm_95_38__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1901 /* bngla- */, PPC::BCCLA, Convert__imm_95_38__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1908 /* bnglr */, PPC::BCCLR, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1908 /* bnglr */, PPC::BCCLR, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1914 /* bnglr+ */, PPC::BCCLR, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1914 /* bnglr+ */, PPC::BCCLR, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1921 /* bnglr- */, PPC::BCCLR, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1921 /* bnglr- */, PPC::BCCLR, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1928 /* bnglrl */, PPC::BCCLRL, Convert__imm_95_36__regCR0, 0, {  }, },
  { 1928 /* bnglrl */, PPC::BCCLRL, Convert__imm_95_36__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1935 /* bnglrl+ */, PPC::BCCLRL, Convert__imm_95_39__regCR0, 0, {  }, },
  { 1935 /* bnglrl+ */, PPC::BCCLRL, Convert__imm_95_39__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1943 /* bnglrl- */, PPC::BCCLRL, Convert__imm_95_38__regCR0, 0, {  }, },
  { 1943 /* bnglrl- */, PPC::BCCLRL, Convert__imm_95_38__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1951 /* bnl */, PPC::BCC, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1951 /* bnl */, PPC::BCC, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1955 /* bnl+ */, PPC::BCC, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1955 /* bnl+ */, PPC::BCC, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1960 /* bnl- */, PPC::BCC, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1960 /* bnl- */, PPC::BCC, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1965 /* bnla */, PPC::BCCA, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1965 /* bnla */, PPC::BCCA, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1970 /* bnla+ */, PPC::BCCA, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1970 /* bnla+ */, PPC::BCCA, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1976 /* bnla- */, PPC::BCCA, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 1976 /* bnla- */, PPC::BCCA, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 1982 /* bnlctr */, PPC::BCCCTR, Convert__imm_95_4__regCR0, 0, {  }, },
  { 1982 /* bnlctr */, PPC::BCCCTR, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1989 /* bnlctr+ */, PPC::BCCCTR, Convert__imm_95_7__regCR0, 0, {  }, },
  { 1989 /* bnlctr+ */, PPC::BCCCTR, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 1997 /* bnlctr- */, PPC::BCCCTR, Convert__imm_95_6__regCR0, 0, {  }, },
  { 1997 /* bnlctr- */, PPC::BCCCTR, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2005 /* bnlctrl */, PPC::BCCCTRL, Convert__imm_95_4__regCR0, 0, {  }, },
  { 2005 /* bnlctrl */, PPC::BCCCTRL, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2013 /* bnlctrl+ */, PPC::BCCCTRL, Convert__imm_95_7__regCR0, 0, {  }, },
  { 2013 /* bnlctrl+ */, PPC::BCCCTRL, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2022 /* bnlctrl- */, PPC::BCCCTRL, Convert__imm_95_6__regCR0, 0, {  }, },
  { 2022 /* bnlctrl- */, PPC::BCCCTRL, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2031 /* bnll */, PPC::BCCL, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2031 /* bnll */, PPC::BCCL, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2036 /* bnll+ */, PPC::BCCL, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2036 /* bnll+ */, PPC::BCCL, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2042 /* bnll- */, PPC::BCCL, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2042 /* bnll- */, PPC::BCCL, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2048 /* bnlla */, PPC::BCCLA, Convert__imm_95_4__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2048 /* bnlla */, PPC::BCCLA, Convert__imm_95_4__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2054 /* bnlla+ */, PPC::BCCLA, Convert__imm_95_7__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2054 /* bnlla+ */, PPC::BCCLA, Convert__imm_95_7__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2061 /* bnlla- */, PPC::BCCLA, Convert__imm_95_6__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2061 /* bnlla- */, PPC::BCCLA, Convert__imm_95_6__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2068 /* bnllr */, PPC::BCCLR, Convert__imm_95_4__regCR0, 0, {  }, },
  { 2068 /* bnllr */, PPC::BCCLR, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2074 /* bnllr+ */, PPC::BCCLR, Convert__imm_95_7__regCR0, 0, {  }, },
  { 2074 /* bnllr+ */, PPC::BCCLR, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2081 /* bnllr- */, PPC::BCCLR, Convert__imm_95_6__regCR0, 0, {  }, },
  { 2081 /* bnllr- */, PPC::BCCLR, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2088 /* bnllrl */, PPC::BCCLRL, Convert__imm_95_4__regCR0, 0, {  }, },
  { 2088 /* bnllrl */, PPC::BCCLRL, Convert__imm_95_4__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2095 /* bnllrl+ */, PPC::BCCLRL, Convert__imm_95_7__regCR0, 0, {  }, },
  { 2095 /* bnllrl+ */, PPC::BCCLRL, Convert__imm_95_7__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2103 /* bnllrl- */, PPC::BCCLRL, Convert__imm_95_6__regCR0, 0, {  }, },
  { 2103 /* bnllrl- */, PPC::BCCLRL, Convert__imm_95_6__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2111 /* bns */, PPC::BCC, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2111 /* bns */, PPC::BCC, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2115 /* bns+ */, PPC::BCC, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2115 /* bns+ */, PPC::BCC, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2120 /* bns- */, PPC::BCC, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2120 /* bns- */, PPC::BCC, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2125 /* bnsa */, PPC::BCCA, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2125 /* bnsa */, PPC::BCCA, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2130 /* bnsa+ */, PPC::BCCA, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2130 /* bnsa+ */, PPC::BCCA, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2136 /* bnsa- */, PPC::BCCA, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2136 /* bnsa- */, PPC::BCCA, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2142 /* bnsctr */, PPC::BCCCTR, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2142 /* bnsctr */, PPC::BCCCTR, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2149 /* bnsctr+ */, PPC::BCCCTR, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2149 /* bnsctr+ */, PPC::BCCCTR, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2157 /* bnsctr- */, PPC::BCCCTR, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2157 /* bnsctr- */, PPC::BCCCTR, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2165 /* bnsctrl */, PPC::BCCCTRL, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2165 /* bnsctrl */, PPC::BCCCTRL, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2173 /* bnsctrl+ */, PPC::BCCCTRL, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2173 /* bnsctrl+ */, PPC::BCCCTRL, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2182 /* bnsctrl- */, PPC::BCCCTRL, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2182 /* bnsctrl- */, PPC::BCCCTRL, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2191 /* bnsl */, PPC::BCCL, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2191 /* bnsl */, PPC::BCCL, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2196 /* bnsl+ */, PPC::BCCL, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2196 /* bnsl+ */, PPC::BCCL, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2202 /* bnsl- */, PPC::BCCL, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2202 /* bnsl- */, PPC::BCCL, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2208 /* bnsla */, PPC::BCCLA, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2208 /* bnsla */, PPC::BCCLA, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2214 /* bnsla+ */, PPC::BCCLA, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2214 /* bnsla+ */, PPC::BCCLA, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2221 /* bnsla- */, PPC::BCCLA, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2221 /* bnsla- */, PPC::BCCLA, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2228 /* bnslr */, PPC::BCCLR, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2228 /* bnslr */, PPC::BCCLR, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2234 /* bnslr+ */, PPC::BCCLR, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2234 /* bnslr+ */, PPC::BCCLR, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2241 /* bnslr- */, PPC::BCCLR, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2241 /* bnslr- */, PPC::BCCLR, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2248 /* bnslrl */, PPC::BCCLRL, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2248 /* bnslrl */, PPC::BCCLRL, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2255 /* bnslrl+ */, PPC::BCCLRL, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2255 /* bnslrl+ */, PPC::BCCLRL, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2263 /* bnslrl- */, PPC::BCCLRL, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2263 /* bnslrl- */, PPC::BCCLRL, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2271 /* bnu */, PPC::BCC, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2271 /* bnu */, PPC::BCC, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2275 /* bnu+ */, PPC::BCC, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2275 /* bnu+ */, PPC::BCC, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2280 /* bnu- */, PPC::BCC, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2280 /* bnu- */, PPC::BCC, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2285 /* bnua */, PPC::BCCA, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2285 /* bnua */, PPC::BCCA, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2290 /* bnua+ */, PPC::BCCA, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2290 /* bnua+ */, PPC::BCCA, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2296 /* bnua- */, PPC::BCCA, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2296 /* bnua- */, PPC::BCCA, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2302 /* bnuctr */, PPC::BCCCTR, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2302 /* bnuctr */, PPC::BCCCTR, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2309 /* bnuctr+ */, PPC::BCCCTR, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2309 /* bnuctr+ */, PPC::BCCCTR, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2317 /* bnuctr- */, PPC::BCCCTR, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2317 /* bnuctr- */, PPC::BCCCTR, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2325 /* bnuctrl */, PPC::BCCCTRL, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2325 /* bnuctrl */, PPC::BCCCTRL, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2333 /* bnuctrl+ */, PPC::BCCCTRL, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2333 /* bnuctrl+ */, PPC::BCCCTRL, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2342 /* bnuctrl- */, PPC::BCCCTRL, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2342 /* bnuctrl- */, PPC::BCCCTRL, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2351 /* bnul */, PPC::BCCL, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2351 /* bnul */, PPC::BCCL, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2356 /* bnul+ */, PPC::BCCL, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2356 /* bnul+ */, PPC::BCCL, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2362 /* bnul- */, PPC::BCCL, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2362 /* bnul- */, PPC::BCCL, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2368 /* bnula */, PPC::BCCLA, Convert__imm_95_100__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2368 /* bnula */, PPC::BCCLA, Convert__imm_95_100__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2374 /* bnula+ */, PPC::BCCLA, Convert__imm_95_103__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2374 /* bnula+ */, PPC::BCCLA, Convert__imm_95_103__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2381 /* bnula- */, PPC::BCCLA, Convert__imm_95_102__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2381 /* bnula- */, PPC::BCCLA, Convert__imm_95_102__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2388 /* bnulr */, PPC::BCCLR, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2388 /* bnulr */, PPC::BCCLR, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2394 /* bnulr+ */, PPC::BCCLR, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2394 /* bnulr+ */, PPC::BCCLR, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2401 /* bnulr- */, PPC::BCCLR, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2401 /* bnulr- */, PPC::BCCLR, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2408 /* bnulrl */, PPC::BCCLRL, Convert__imm_95_100__regCR0, 0, {  }, },
  { 2408 /* bnulrl */, PPC::BCCLRL, Convert__imm_95_100__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2415 /* bnulrl+ */, PPC::BCCLRL, Convert__imm_95_103__regCR0, 0, {  }, },
  { 2415 /* bnulrl+ */, PPC::BCCLRL, Convert__imm_95_103__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2423 /* bnulrl- */, PPC::BCCLRL, Convert__imm_95_102__regCR0, 0, {  }, },
  { 2423 /* bnulrl- */, PPC::BCCLRL, Convert__imm_95_102__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2431 /* bpermd */, PPC::BPERMD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2438 /* brinc */, PPC::BRINC, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 2444 /* bso */, PPC::BCC, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2444 /* bso */, PPC::BCC, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2448 /* bso+ */, PPC::BCC, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2448 /* bso+ */, PPC::BCC, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2453 /* bso- */, PPC::BCC, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2453 /* bso- */, PPC::BCC, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2458 /* bsoa */, PPC::BCCA, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2458 /* bsoa */, PPC::BCCA, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2463 /* bsoa+ */, PPC::BCCA, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2463 /* bsoa+ */, PPC::BCCA, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2469 /* bsoa- */, PPC::BCCA, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2469 /* bsoa- */, PPC::BCCA, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2475 /* bsoctr */, PPC::BCCCTR, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2475 /* bsoctr */, PPC::BCCCTR, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2482 /* bsoctr+ */, PPC::BCCCTR, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2482 /* bsoctr+ */, PPC::BCCCTR, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2490 /* bsoctr- */, PPC::BCCCTR, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2490 /* bsoctr- */, PPC::BCCCTR, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2498 /* bsoctrl */, PPC::BCCCTRL, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2498 /* bsoctrl */, PPC::BCCCTRL, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2506 /* bsoctrl+ */, PPC::BCCCTRL, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2506 /* bsoctrl+ */, PPC::BCCCTRL, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2515 /* bsoctrl- */, PPC::BCCCTRL, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2515 /* bsoctrl- */, PPC::BCCCTRL, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2524 /* bsol */, PPC::BCCL, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2524 /* bsol */, PPC::BCCL, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2529 /* bsol+ */, PPC::BCCL, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2529 /* bsol+ */, PPC::BCCL, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2535 /* bsol- */, PPC::BCCL, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2535 /* bsol- */, PPC::BCCL, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2541 /* bsola */, PPC::BCCLA, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2541 /* bsola */, PPC::BCCLA, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2547 /* bsola+ */, PPC::BCCLA, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2547 /* bsola+ */, PPC::BCCLA, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2554 /* bsola- */, PPC::BCCLA, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2554 /* bsola- */, PPC::BCCLA, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2561 /* bsolr */, PPC::BCCLR, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2561 /* bsolr */, PPC::BCCLR, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2567 /* bsolr+ */, PPC::BCCLR, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2567 /* bsolr+ */, PPC::BCCLR, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2574 /* bsolr- */, PPC::BCCLR, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2574 /* bsolr- */, PPC::BCCLR, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2581 /* bsolrl */, PPC::BCCLRL, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2581 /* bsolrl */, PPC::BCCLRL, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2588 /* bsolrl+ */, PPC::BCCLRL, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2588 /* bsolrl+ */, PPC::BCCLRL, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2596 /* bsolrl- */, PPC::BCCLRL, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2596 /* bsolrl- */, PPC::BCCLRL, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2604 /* bt */, PPC::gBC, Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2607 /* bt+ */, PPC::gBC, Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2611 /* bt- */, PPC::gBC, Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2615 /* bta */, PPC::gBCA, Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2619 /* bta+ */, PPC::gBCA, Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2624 /* bta- */, PPC::gBCA, Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2629 /* btctr */, PPC::gBCCTR, Convert__imm_95_12__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2635 /* btctr+ */, PPC::gBCCTR, Convert__imm_95_15__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2642 /* btctr- */, PPC::gBCCTR, Convert__imm_95_14__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2649 /* btctrl */, PPC::gBCCTRL, Convert__imm_95_12__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2656 /* btctrl+ */, PPC::gBCCTRL, Convert__imm_95_15__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2664 /* btctrl- */, PPC::gBCCTRL, Convert__imm_95_14__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2672 /* btl */, PPC::gBCL, Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2676 /* btl+ */, PPC::gBCL, Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2681 /* btl- */, PPC::gBCL, Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2686 /* btla */, PPC::gBCLA, Convert__imm_95_12__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2691 /* btla+ */, PPC::gBCLA, Convert__imm_95_15__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2697 /* btla- */, PPC::gBCLA, Convert__imm_95_14__RegCRBITRC1_0__CondBr1_1, 0, { MCK_RegCRBITRC, MCK_CondBr }, },
  { 2703 /* btlr */, PPC::gBCLR, Convert__imm_95_12__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2708 /* btlr+ */, PPC::gBCLR, Convert__imm_95_15__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2714 /* btlr- */, PPC::gBCLR, Convert__imm_95_14__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2720 /* btlrl */, PPC::gBCLRL, Convert__imm_95_12__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2726 /* btlrl+ */, PPC::gBCLRL, Convert__imm_95_15__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2733 /* btlrl- */, PPC::gBCLRL, Convert__imm_95_14__RegCRBITRC1_0__imm_95_0, 0, { MCK_RegCRBITRC }, },
  { 2740 /* bun */, PPC::BCC, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2740 /* bun */, PPC::BCC, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2744 /* bun+ */, PPC::BCC, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2744 /* bun+ */, PPC::BCC, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2749 /* bun- */, PPC::BCC, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2749 /* bun- */, PPC::BCC, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2754 /* buna */, PPC::BCCA, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2754 /* buna */, PPC::BCCA, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2759 /* buna+ */, PPC::BCCA, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2759 /* buna+ */, PPC::BCCA, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2765 /* buna- */, PPC::BCCA, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2765 /* buna- */, PPC::BCCA, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2771 /* bunctr */, PPC::BCCCTR, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2771 /* bunctr */, PPC::BCCCTR, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2778 /* bunctr+ */, PPC::BCCCTR, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2778 /* bunctr+ */, PPC::BCCCTR, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2786 /* bunctr- */, PPC::BCCCTR, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2786 /* bunctr- */, PPC::BCCCTR, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2794 /* bunctrl */, PPC::BCCCTRL, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2794 /* bunctrl */, PPC::BCCCTRL, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2802 /* bunctrl+ */, PPC::BCCCTRL, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2802 /* bunctrl+ */, PPC::BCCCTRL, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2811 /* bunctrl- */, PPC::BCCCTRL, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2811 /* bunctrl- */, PPC::BCCCTRL, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2820 /* bunl */, PPC::BCCL, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2820 /* bunl */, PPC::BCCL, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2825 /* bunl+ */, PPC::BCCL, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2825 /* bunl+ */, PPC::BCCL, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2831 /* bunl- */, PPC::BCCL, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2831 /* bunl- */, PPC::BCCL, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2837 /* bunla */, PPC::BCCLA, Convert__imm_95_108__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2837 /* bunla */, PPC::BCCLA, Convert__imm_95_108__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2843 /* bunla+ */, PPC::BCCLA, Convert__imm_95_111__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2843 /* bunla+ */, PPC::BCCLA, Convert__imm_95_111__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2850 /* bunla- */, PPC::BCCLA, Convert__imm_95_110__regCR0__CondBr1_0, 0, { MCK_CondBr }, },
  { 2850 /* bunla- */, PPC::BCCLA, Convert__imm_95_110__RegCRRC1_0__CondBr1_1, 0, { MCK_RegCRRC, MCK_CondBr }, },
  { 2857 /* bunlr */, PPC::BCCLR, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2857 /* bunlr */, PPC::BCCLR, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2863 /* bunlr+ */, PPC::BCCLR, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2863 /* bunlr+ */, PPC::BCCLR, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2870 /* bunlr- */, PPC::BCCLR, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2870 /* bunlr- */, PPC::BCCLR, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2877 /* bunlrl */, PPC::BCCLRL, Convert__imm_95_108__regCR0, 0, {  }, },
  { 2877 /* bunlrl */, PPC::BCCLRL, Convert__imm_95_108__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2884 /* bunlrl+ */, PPC::BCCLRL, Convert__imm_95_111__regCR0, 0, {  }, },
  { 2884 /* bunlrl+ */, PPC::BCCLRL, Convert__imm_95_111__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2892 /* bunlrl- */, PPC::BCCLRL, Convert__imm_95_110__regCR0, 0, {  }, },
  { 2892 /* bunlrl- */, PPC::BCCLRL, Convert__imm_95_110__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 2900 /* clrbhrb */, PPC::CLRBHRB, Convert_NoOperands, 0, {  }, },
  { 2908 /* clrldi */, PPC::RLDICL, Convert__RegG8RC1_0__RegG8RC1_1__imm_95_0__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 2908 /* clrldi */, PPC::RLDICL_32_64, Convert__RegG8RC1_0__RegGPRC1_1__imm_95_0__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegGPRC, MCK_U6Imm }, },
  { 2908 /* clrldi */, PPC::RLDICLo, Convert__RegG8RC1_1__RegG8RC1_2__imm_95_0__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 2915 /* clrlsldi */, PPC::CLRLSLDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 2915 /* clrlsldi */, PPC::CLRLSLDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 2924 /* clrlslwi */, PPC::CLRLSLWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 2924 /* clrlslwi */, PPC::CLRLSLWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 2933 /* clrlwi */, PPC::RLWINM, Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0__U5Imm1_2__imm_95_31, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 2933 /* clrlwi */, PPC::RLWINMo, Convert__RegGPRC1_1__RegGPRC1_2__imm_95_0__U5Imm1_3__imm_95_31, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 2940 /* clrrdi */, PPC::CLRRDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 2940 /* clrrdi */, PPC::CLRRDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 2947 /* clrrwi */, PPC::CLRRWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 2947 /* clrrwi */, PPC::CLRRWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 2954 /* cmp */, PPC::CMPW, Convert__RegCRRC1_0__RegGPRC1_2__RegGPRC1_3, 0, { MCK_RegCRRC, MCK_0, MCK_RegGPRC, MCK_RegGPRC }, },
  { 2954 /* cmp */, PPC::CMPD, Convert__RegCRRC1_0__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegCRRC, MCK_1, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2958 /* cmpb */, PPC::CMPB, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 2963 /* cmpd */, PPC::CMPD, Convert__regCR0__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 2963 /* cmpd */, PPC::CMPD, Convert__RegCRRC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegCRRC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2968 /* cmpdi */, PPC::CMPDI, Convert__regCR0__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 2968 /* cmpdi */, PPC::CMPDI, Convert__RegCRRC1_0__RegG8RC1_1__S16Imm1_2, 0, { MCK_RegCRRC, MCK_RegG8RC, MCK_S16Imm }, },
  { 2974 /* cmpeqb */, PPC::CMPEQB, Convert__RegCRBITRC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegCRBITRC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2981 /* cmpi */, PPC::CMPWI, Convert__RegCRRC1_0__RegGPRC1_2__S16Imm1_3, 0, { MCK_RegCRRC, MCK_0, MCK_RegGPRC, MCK_S16Imm }, },
  { 2981 /* cmpi */, PPC::CMPDI, Convert__RegCRRC1_0__RegG8RC1_2__S16Imm1_3, 0, { MCK_RegCRRC, MCK_1, MCK_RegG8RC, MCK_S16Imm }, },
  { 2986 /* cmpl */, PPC::CMPLW, Convert__RegCRRC1_0__RegGPRC1_2__RegGPRC1_3, 0, { MCK_RegCRRC, MCK_0, MCK_RegGPRC, MCK_RegGPRC }, },
  { 2986 /* cmpl */, PPC::CMPLD, Convert__RegCRRC1_0__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegCRRC, MCK_1, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2991 /* cmpld */, PPC::CMPLD, Convert__regCR0__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 2991 /* cmpld */, PPC::CMPLD, Convert__RegCRRC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegCRRC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 2997 /* cmpldi */, PPC::CMPLDI, Convert__regCR0__RegG8RC1_0__U16Imm1_1, 0, { MCK_RegG8RC, MCK_U16Imm }, },
  { 2997 /* cmpldi */, PPC::CMPLDI, Convert__RegCRRC1_0__RegG8RC1_1__U16Imm1_2, 0, { MCK_RegCRRC, MCK_RegG8RC, MCK_U16Imm }, },
  { 3004 /* cmpli */, PPC::CMPLWI, Convert__RegCRRC1_0__RegGPRC1_2__U16Imm1_3, 0, { MCK_RegCRRC, MCK_0, MCK_RegGPRC, MCK_U16Imm }, },
  { 3004 /* cmpli */, PPC::CMPLDI, Convert__RegCRRC1_0__RegG8RC1_2__U16Imm1_3, 0, { MCK_RegCRRC, MCK_1, MCK_RegG8RC, MCK_U16Imm }, },
  { 3010 /* cmplw */, PPC::CMPLW, Convert__regCR0__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3010 /* cmplw */, PPC::CMPLW, Convert__RegCRRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegCRRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3016 /* cmplwi */, PPC::CMPLWI, Convert__regCR0__RegGPRC1_0__U16Imm1_1, 0, { MCK_RegGPRC, MCK_U16Imm }, },
  { 3016 /* cmplwi */, PPC::CMPLWI, Convert__RegCRRC1_0__RegGPRC1_1__U16Imm1_2, 0, { MCK_RegCRRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 3023 /* cmprb */, PPC::CMPRB, Convert__RegCRBITRC1_0__U1Imm1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegCRBITRC, MCK_U1Imm, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3029 /* cmpw */, PPC::CMPW, Convert__regCR0__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3029 /* cmpw */, PPC::CMPW, Convert__RegCRRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegCRRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3034 /* cmpwi */, PPC::CMPWI, Convert__regCR0__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 3034 /* cmpwi */, PPC::CMPWI, Convert__RegCRRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegCRRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 3040 /* cntlzd */, PPC::CNTLZD, Convert__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 3040 /* cntlzd */, PPC::CNTLZDo, Convert__RegG8RC1_1__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3047 /* cntlzw */, PPC::CNTLZW, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3047 /* cntlzw */, PPC::CNTLZW, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3047 /* cntlzw */, PPC::CNTLZWo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3047 /* cntlzw */, PPC::CNTLZWo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3054 /* cnttzd */, PPC::CNTTZD, Convert__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 3054 /* cnttzd */, PPC::CNTTZDo, Convert__RegG8RC1_1__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3061 /* cnttzw */, PPC::CNTTZW, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3061 /* cnttzw */, PPC::CNTTZWo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3068 /* copy */, PPC::CP_COPYx, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3068 /* copy */, PPC::CP_COPY, Convert__RegGPRC1_0__RegGPRC1_1__U1Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U1Imm }, },
  { 3073 /* copy_first */, PPC::CP_COPY_FIRST, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3084 /* cp_abort */, PPC::CP_ABORT, Convert_NoOperands, 0, {  }, },
  { 3093 /* crand */, PPC::CRAND, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3099 /* crandc */, PPC::CRANDC, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3106 /* crclr */, PPC::CRXOR, Convert__RegCRBITRC1_0__RegCRBITRC1_0__RegCRBITRC1_0, 0, { MCK_RegCRBITRC }, },
  { 3112 /* creqv */, PPC::CREQV, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3118 /* crmove */, PPC::CROR, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_1, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3125 /* crnand */, PPC::CRNAND, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3132 /* crnor */, PPC::CRNOR, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3138 /* crnot */, PPC::CRNOR, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_1, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3144 /* cror */, PPC::CROR, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3149 /* crorc */, PPC::CRORC, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3155 /* crset */, PPC::CREQV, Convert__RegCRBITRC1_0__RegCRBITRC1_0__RegCRBITRC1_0, 0, { MCK_RegCRBITRC }, },
  { 3161 /* crxor */, PPC::CRXOR, Convert__RegCRBITRC1_0__RegCRBITRC1_1__RegCRBITRC1_2, 0, { MCK_RegCRBITRC, MCK_RegCRBITRC, MCK_RegCRBITRC }, },
  { 3167 /* darn */, PPC::DARN, Convert__RegG8RC1_0__Imm1_1, 0, { MCK_RegG8RC, MCK_Imm }, },
  { 3172 /* dcba */, PPC::DCBA, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3177 /* dcbf */, PPC::DCBFx, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3177 /* dcbf */, PPC::DCBF, Convert__U5Imm1_2__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3182 /* dcbfep */, PPC::DCBFEP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3189 /* dcbfl */, PPC::DCBFL, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3195 /* dcbflp */, PPC::DCBFLP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3202 /* dcbi */, PPC::DCBI, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3207 /* dcbst */, PPC::DCBST, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3213 /* dcbstep */, PPC::DCBSTEP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3221 /* dcbt */, PPC::DCBTx, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3221 /* dcbt */, PPC::DCBT, Convert__U5Imm1_2__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3226 /* dcbtct */, PPC::DCBTCT, Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3233 /* dcbtds */, PPC::DCBTDS, Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3240 /* dcbtep */, PPC::DCBTEP, Convert__RegGxRCNoR01_1__RegGxRC1_2__U5Imm1_0, 0, { MCK_U5Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3247 /* dcbtst */, PPC::DCBTSTx, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3247 /* dcbtst */, PPC::DCBTST, Convert__U5Imm1_2__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3254 /* dcbtstct */, PPC::DCBTSTCT, Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3263 /* dcbtstds */, PPC::DCBTSTDS, Convert__RegGxRCNoR01_0__RegGxRC1_1__U5Imm1_2, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_U5Imm }, },
  { 3272 /* dcbtstep */, PPC::DCBTSTEP, Convert__RegGxRCNoR01_1__RegGxRC1_2__U5Imm1_0, 0, { MCK_U5Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3281 /* dcbtstt */, PPC::DCBTSTT, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3289 /* dcbtt */, PPC::DCBTT, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3295 /* dcbz */, PPC::DCBZ, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3300 /* dcbzep */, PPC::DCBZEP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3307 /* dcbzl */, PPC::DCBZL, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3313 /* dcbzlep */, PPC::DCBZLEP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 3321 /* dccci */, PPC::DCCCI, Convert__regR0__regR0, 0, {  }, },
  { 3321 /* dccci */, PPC::DCCCI, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 3327 /* dci */, PPC::DCCCI, Convert__regR0__regR0, 0, { MCK_0 }, },
  { 3331 /* divd */, PPC::DIVD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3331 /* divd */, PPC::DIVDo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3336 /* divde */, PPC::DIVDE, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3336 /* divde */, PPC::DIVDEo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3342 /* divdeu */, PPC::DIVDEU, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3342 /* divdeu */, PPC::DIVDEUo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3349 /* divdu */, PPC::DIVDU, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3349 /* divdu */, PPC::DIVDUo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 3355 /* divw */, PPC::DIVW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3355 /* divw */, PPC::DIVWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3360 /* divwe */, PPC::DIVWE, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3360 /* divwe */, PPC::DIVWEo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3366 /* divweu */, PPC::DIVWEU, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3366 /* divweu */, PPC::DIVWEUo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3373 /* divwu */, PPC::DIVWU, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3373 /* divwu */, PPC::DIVWUo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3379 /* dss */, PPC::DSS, Convert__U5Imm1_0, 0, { MCK_U5Imm }, },
  { 3383 /* dssall */, PPC::DSSALL, Convert_NoOperands, 0, {  }, },
  { 3390 /* dst */, PPC::DST, Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 3394 /* dstst */, PPC::DSTST, Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 3400 /* dststt */, PPC::DSTSTT, Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 3407 /* dstt */, PPC::DSTT, Convert__U5Imm1_2__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 3412 /* efdabs */, PPC::EFDABS, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3419 /* efdadd */, PPC::EFDADD, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3426 /* efdcfs */, PPC::EFDCFS, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3433 /* efdcfsf */, PPC::EFDCFSF, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3441 /* efdcfsi */, PPC::EFDCFSI, Convert__RegSPERC1_0__RegGPRC1_1, 0, { MCK_RegSPERC, MCK_RegGPRC }, },
  { 3449 /* efdcfsid */, PPC::EFDCFSID, Convert__RegSPERC1_0__RegGPRC1_1, 0, { MCK_RegSPERC, MCK_RegGPRC }, },
  { 3458 /* efdcfuf */, PPC::EFDCFUF, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3466 /* efdcfui */, PPC::EFDCFUI, Convert__RegSPERC1_0__RegGPRC1_1, 0, { MCK_RegSPERC, MCK_RegGPRC }, },
  { 3474 /* efdcfuid */, PPC::EFDCFUID, Convert__RegSPERC1_0__RegGPRC1_1, 0, { MCK_RegSPERC, MCK_RegGPRC }, },
  { 3483 /* efdcmpeq */, PPC::EFDCMPEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3492 /* efdcmpgt */, PPC::EFDCMPGT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3501 /* efdcmplt */, PPC::EFDCMPLT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3510 /* efdctsf */, PPC::EFDCTSF, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3518 /* efdctsi */, PPC::EFDCTSI, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3526 /* efdctsidz */, PPC::EFDCTSIDZ, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3536 /* efdctsiz */, PPC::EFDCTSIZ, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3545 /* efdctuf */, PPC::EFDCTUF, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3553 /* efdctui */, PPC::EFDCTUI, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3561 /* efdctuidz */, PPC::EFDCTUIDZ, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3571 /* efdctuiz */, PPC::EFDCTUIZ, Convert__RegGPRC1_0__RegSPERC1_1, 0, { MCK_RegGPRC, MCK_RegSPERC }, },
  { 3580 /* efddiv */, PPC::EFDDIV, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3587 /* efdmul */, PPC::EFDMUL, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3594 /* efdnabs */, PPC::EFDNABS, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3602 /* efdneg */, PPC::EFDNEG, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3609 /* efdsub */, PPC::EFDSUB, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3616 /* efdtsteq */, PPC::EFDTSTEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3625 /* efdtstgt */, PPC::EFDTSTGT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3634 /* efdtstlt */, PPC::EFDTSTLT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3643 /* efsabs */, PPC::EFSABS, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3650 /* efsadd */, PPC::EFSADD, Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3657 /* efscfd */, PPC::EFSCFD, Convert__RegSPE4RC1_0__RegSPERC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPERC }, },
  { 3664 /* efscfsf */, PPC::EFSCFSF, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3672 /* efscfsi */, PPC::EFSCFSI, Convert__RegSPE4RC1_0__RegGPRC1_1, 0, { MCK_RegSPE4RC, MCK_RegGPRC }, },
  { 3680 /* efscfuf */, PPC::EFSCFUF, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3688 /* efscfui */, PPC::EFSCFUI, Convert__RegSPE4RC1_0__RegGPRC1_1, 0, { MCK_RegSPE4RC, MCK_RegGPRC }, },
  { 3696 /* efscmpeq */, PPC::EFSCMPEQ, Convert__RegCRRC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegCRRC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3705 /* efscmpgt */, PPC::EFSCMPGT, Convert__RegCRRC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegCRRC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3714 /* efscmplt */, PPC::EFSCMPLT, Convert__RegCRRC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegCRRC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3723 /* efsctsf */, PPC::EFSCTSF, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3731 /* efsctsi */, PPC::EFSCTSI, Convert__RegGPRC1_0__RegSPE4RC1_1, 0, { MCK_RegGPRC, MCK_RegSPE4RC }, },
  { 3739 /* efsctsiz */, PPC::EFSCTSIZ, Convert__RegGPRC1_0__RegSPE4RC1_1, 0, { MCK_RegGPRC, MCK_RegSPE4RC }, },
  { 3748 /* efsctuf */, PPC::EFSCTUF, Convert__RegSPERC1_0__RegSPE4RC1_1, 0, { MCK_RegSPERC, MCK_RegSPE4RC }, },
  { 3756 /* efsctui */, PPC::EFSCTUI, Convert__RegGPRC1_0__RegSPE4RC1_1, 0, { MCK_RegGPRC, MCK_RegSPE4RC }, },
  { 3764 /* efsctuiz */, PPC::EFSCTUIZ, Convert__RegGPRC1_0__RegSPE4RC1_1, 0, { MCK_RegGPRC, MCK_RegSPE4RC }, },
  { 3773 /* efsdiv */, PPC::EFSDIV, Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3780 /* efsmul */, PPC::EFSMUL, Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3787 /* efsnabs */, PPC::EFSNABS, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3795 /* efsneg */, PPC::EFSNEG, Convert__RegSPE4RC1_0__RegSPE4RC1_1, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3802 /* efssub */, PPC::EFSSUB, Convert__RegSPE4RC1_0__RegSPE4RC1_1__RegSPE4RC1_2, 0, { MCK_RegSPE4RC, MCK_RegSPE4RC, MCK_RegSPE4RC }, },
  { 3809 /* efststeq */, PPC::EFSTSTEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3818 /* efststgt */, PPC::EFSTSTGT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3827 /* efststlt */, PPC::EFSTSTLT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3836 /* eieio */, PPC::EnforceIEIO, Convert_NoOperands, 0, {  }, },
  { 3842 /* eqv */, PPC::EQV, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3842 /* eqv */, PPC::EQVo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 3846 /* evabs */, PPC::EVABS, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3852 /* evaddiw */, PPC::EVADDIW, Convert__RegSPERC1_0__RegSPERC1_2__U5Imm1_1, 0, { MCK_RegSPERC, MCK_U5Imm, MCK_RegSPERC }, },
  { 3860 /* evaddsmiaaw */, PPC::EVADDSMIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3872 /* evaddssiaaw */, PPC::EVADDSSIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3884 /* evaddumiaaw */, PPC::EVADDUMIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3896 /* evaddusiaaw */, PPC::EVADDUSIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3908 /* evaddw */, PPC::EVADDW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3915 /* evand */, PPC::EVAND, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3921 /* evandc */, PPC::EVANDC, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3928 /* evcmpeq */, PPC::EVCMPEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3936 /* evcmpgts */, PPC::EVCMPGTS, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3945 /* evcmpgtu */, PPC::EVCMPGTU, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3954 /* evcmplts */, PPC::EVCMPLTS, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3963 /* evcmpltu */, PPC::EVCMPLTU, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3972 /* evcntlsw */, PPC::EVCNTLSW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3981 /* evcntlzw */, PPC::EVCNTLZW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 3990 /* evdivws */, PPC::EVDIVWS, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 3998 /* evdivwu */, PPC::EVDIVWU, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4006 /* eveqv */, PPC::EVEQV, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4012 /* evextsb */, PPC::EVEXTSB, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4020 /* evextsh */, PPC::EVEXTSH, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4028 /* evfsabs */, PPC::EVFSABS, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4036 /* evfsadd */, PPC::EVFSADD, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4044 /* evfscfsf */, PPC::EVFSCFSF, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4053 /* evfscfsi */, PPC::EVFSCFSI, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4062 /* evfscfuf */, PPC::EVFSCFUF, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4071 /* evfscfui */, PPC::EVFSCFUI, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4080 /* evfscmpeq */, PPC::EVFSCMPEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4090 /* evfscmpgt */, PPC::EVFSCMPGT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4100 /* evfscmplt */, PPC::EVFSCMPLT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4110 /* evfsctsf */, PPC::EVFSCTSF, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4110 /* evfsctsf */, PPC::EVFSCTUF, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4119 /* evfsctsi */, PPC::EVFSCTSI, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4128 /* evfsctsiz */, PPC::EVFSCTSIZ, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4128 /* evfsctsiz */, PPC::EVFSCTUIZ, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4138 /* evfsctui */, PPC::EVFSCTUI, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4147 /* evfsdiv */, PPC::EVFSDIV, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4155 /* evfsmul */, PPC::EVFSMUL, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4163 /* evfsnabs */, PPC::EVFSNABS, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4172 /* evfsneg */, PPC::EVFSNEG, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 4180 /* evfssub */, PPC::EVFSSUB, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4188 /* evfststeq */, PPC::EVFSTSTEQ, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4198 /* evfststgt */, PPC::EVFSTSTGT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4208 /* evfststlt */, PPC::EVFSTSTLT, Convert__RegCRRC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegCRRC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4218 /* evldd */, PPC::EVLDD, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 4224 /* evlddx */, PPC::EVLDDX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4231 /* evldh */, PPC::EVLDH, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 4237 /* evldhx */, PPC::EVLDHX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4244 /* evldw */, PPC::EVLDW, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 4250 /* evldwx */, PPC::EVLDWX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4257 /* evlhhesplat */, PPC::EVLHHESPLAT, Convert__RegSPERC1_0__DispSPE21_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE2, MCK_RegGxRCNoR0 }, },
  { 4269 /* evlhhesplatx */, PPC::EVLHHESPLATX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4282 /* evlhhossplat */, PPC::EVLHHOSSPLAT, Convert__RegSPERC1_0__DispSPE21_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE2, MCK_RegGxRCNoR0 }, },
  { 4295 /* evlhhossplatx */, PPC::EVLHHOSSPLATX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4309 /* evlhhousplat */, PPC::EVLHHOUSPLAT, Convert__RegSPERC1_0__DispSPE21_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE2, MCK_RegGxRCNoR0 }, },
  { 4322 /* evlhhousplatx */, PPC::EVLHHOUSPLATX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4336 /* evlwhe */, PPC::EVLWHE, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 4343 /* evlwhex */, PPC::EVLWHEX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4351 /* evlwhos */, PPC::EVLWHOS, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 4359 /* evlwhosx */, PPC::EVLWHOSX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4368 /* evlwhou */, PPC::EVLWHOU, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 4376 /* evlwhoux */, PPC::EVLWHOUX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4385 /* evlwhsplat */, PPC::EVLWHSPLAT, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 4396 /* evlwhsplatx */, PPC::EVLWHSPLATX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4408 /* evlwwsplat */, PPC::EVLWWSPLAT, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 4419 /* evlwwsplatx */, PPC::EVLWWSPLATX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 4431 /* evmergehi */, PPC::EVMERGEHI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4441 /* evmergehilo */, PPC::EVMERGEHILO, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4453 /* evmergelo */, PPC::EVMERGELO, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4463 /* evmergelohi */, PPC::EVMERGELOHI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4475 /* evmhegsmfaa */, PPC::EVMHEGSMFAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4487 /* evmhegsmfan */, PPC::EVMHEGSMFAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4499 /* evmhegsmiaa */, PPC::EVMHEGSMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4511 /* evmhegsmian */, PPC::EVMHEGSMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4523 /* evmhegumiaa */, PPC::EVMHEGUMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4535 /* evmhegumian */, PPC::EVMHEGUMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4547 /* evmhesmf */, PPC::EVMHESMF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4556 /* evmhesmfa */, PPC::EVMHESMFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4566 /* evmhesmfaaw */, PPC::EVMHESMFAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4578 /* evmhesmfanw */, PPC::EVMHESMFANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4590 /* evmhesmi */, PPC::EVMHESMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4599 /* evmhesmia */, PPC::EVMHESMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4609 /* evmhesmiaaw */, PPC::EVMHESMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4621 /* evmhesmianw */, PPC::EVMHESMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4633 /* evmhessf */, PPC::EVMHESSF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4642 /* evmhessfa */, PPC::EVMHESSFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4652 /* evmhessfaaw */, PPC::EVMHESSFAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4664 /* evmhessfanw */, PPC::EVMHESSFANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4676 /* evmhessiaaw */, PPC::EVMHESSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4688 /* evmhessianw */, PPC::EVMHESSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4700 /* evmheumi */, PPC::EVMHEUMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4709 /* evmheumia */, PPC::EVMHEUMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4719 /* evmheumiaaw */, PPC::EVMHEUMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4731 /* evmheumianw */, PPC::EVMHEUMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4743 /* evmheusiaaw */, PPC::EVMHEUSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4755 /* evmheusianw */, PPC::EVMHEUSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4767 /* evmhogsmfaa */, PPC::EVMHOGSMFAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4779 /* evmhogsmfan */, PPC::EVMHOGSMFAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4791 /* evmhogsmiaa */, PPC::EVMHOGSMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4803 /* evmhogsmian */, PPC::EVMHOGSMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4815 /* evmhogumiaa */, PPC::EVMHOGUMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4827 /* evmhogumian */, PPC::EVMHOGUMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4839 /* evmhosmf */, PPC::EVMHOSMF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4848 /* evmhosmfa */, PPC::EVMHOSMFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4858 /* evmhosmfaaw */, PPC::EVMHOSMFAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4870 /* evmhosmfanw */, PPC::EVMHOSMFANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4882 /* evmhosmi */, PPC::EVMHOSMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4891 /* evmhosmia */, PPC::EVMHOSMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4901 /* evmhosmiaaw */, PPC::EVMHOSMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4913 /* evmhosmianw */, PPC::EVMHOSMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4925 /* evmhossf */, PPC::EVMHOSSF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4934 /* evmhossfa */, PPC::EVMHOSSFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4944 /* evmhossfaaw */, PPC::EVMHOSSFAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4956 /* evmhossfanw */, PPC::EVMHOSSFANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4968 /* evmhossiaaw */, PPC::EVMHOSSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4980 /* evmhossianw */, PPC::EVMHOSSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 4992 /* evmhoumi */, PPC::EVMHOUMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5001 /* evmhoumia */, PPC::EVMHOUMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5011 /* evmhoumiaaw */, PPC::EVMHOUMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5023 /* evmhoumianw */, PPC::EVMHOUMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5035 /* evmhousiaaw */, PPC::EVMHOUSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5047 /* evmhousianw */, PPC::EVMHOUSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5059 /* evmra */, PPC::EVMRA, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5065 /* evmwhsmf */, PPC::EVMWHSMF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5074 /* evmwhsmfa */, PPC::EVMWHSMFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5084 /* evmwhsmi */, PPC::EVMWHSMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5093 /* evmwhsmia */, PPC::EVMWHSMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5103 /* evmwhssf */, PPC::EVMWHSSF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5112 /* evmwhssfa */, PPC::EVMWHSSFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5122 /* evmwhumi */, PPC::EVMWHUMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5131 /* evmwhumia */, PPC::EVMWHUMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5141 /* evmwlsmiaaw */, PPC::EVMWLSMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5153 /* evmwlsmianw */, PPC::EVMWLSMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5165 /* evmwlssiaaw */, PPC::EVMWLSSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5177 /* evmwlssianw */, PPC::EVMWLSSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5189 /* evmwlumi */, PPC::EVMWLUMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5198 /* evmwlumia */, PPC::EVMWLUMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5208 /* evmwlumiaaw */, PPC::EVMWLUMIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5220 /* evmwlumianw */, PPC::EVMWLUMIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5232 /* evmwlusiaaw */, PPC::EVMWLUSIAAW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5244 /* evmwlusianw */, PPC::EVMWLUSIANW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5256 /* evmwsmf */, PPC::EVMWSMF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5264 /* evmwsmfa */, PPC::EVMWSMFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5273 /* evmwsmfaa */, PPC::EVMWSMFAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5283 /* evmwsmfan */, PPC::EVMWSMFAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5293 /* evmwsmi */, PPC::EVMWSMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5301 /* evmwsmia */, PPC::EVMWSMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5310 /* evmwsmiaa */, PPC::EVMWSMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5320 /* evmwsmian */, PPC::EVMWSMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5330 /* evmwssf */, PPC::EVMWSSF, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5338 /* evmwssfa */, PPC::EVMWSSFA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5347 /* evmwssfaa */, PPC::EVMWSSFAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5357 /* evmwssfan */, PPC::EVMWSSFAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5367 /* evmwumi */, PPC::EVMWUMI, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5375 /* evmwumia */, PPC::EVMWUMIA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5384 /* evmwumiaa */, PPC::EVMWUMIAA, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5394 /* evmwumian */, PPC::EVMWUMIAN, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5404 /* evnand */, PPC::EVNAND, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5411 /* evneg */, PPC::EVNEG, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5417 /* evnor */, PPC::EVNOR, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5423 /* evor */, PPC::EVOR, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5428 /* evorc */, PPC::EVORC, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5434 /* evrlw */, PPC::EVRLW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5440 /* evrlwi */, PPC::EVRLWI, Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_U5Imm }, },
  { 5447 /* evrndw */, PPC::EVRNDW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5454 /* evsel */, PPC::EVSEL, Convert__RegSPERC1_1__RegSPERC1_2__RegSPERC1_3__imm_95_0, 0, { MCK_crD, MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5460 /* evslw */, PPC::EVSLW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5466 /* evslwi */, PPC::EVSLWI, Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_U5Imm }, },
  { 5473 /* evsplatfi */, PPC::EVSPLATFI, Convert__RegSPERC1_0__S5Imm1_1, 0, { MCK_RegSPERC, MCK_S5Imm }, },
  { 5483 /* evsplati */, PPC::EVSPLATI, Convert__RegSPERC1_0__S5Imm1_1, 0, { MCK_RegSPERC, MCK_S5Imm }, },
  { 5492 /* evsrwis */, PPC::EVSRWIS, Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_U5Imm }, },
  { 5500 /* evsrwiu */, PPC::EVSRWIU, Convert__RegSPERC1_0__RegSPERC1_1__U5Imm1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_U5Imm }, },
  { 5508 /* evsrws */, PPC::EVSRWS, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5515 /* evsrwu */, PPC::EVSRWU, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5522 /* evstdd */, PPC::EVSTDD, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 5529 /* evstddx */, PPC::EVSTDDX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5537 /* evstdh */, PPC::EVSTDH, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 5544 /* evstdhx */, PPC::EVSTDHX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5552 /* evstdw */, PPC::EVSTDW, Convert__RegSPERC1_0__DispSPE81_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE8, MCK_RegGxRCNoR0 }, },
  { 5559 /* evstdwx */, PPC::EVSTDWX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5567 /* evstwhe */, PPC::EVSTWHE, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 5575 /* evstwhex */, PPC::EVSTWHEX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5584 /* evstwho */, PPC::EVSTWHO, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 5592 /* evstwhox */, PPC::EVSTWHOX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5601 /* evstwwe */, PPC::EVSTWWE, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 5609 /* evstwwex */, PPC::EVSTWWEX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5618 /* evstwwo */, PPC::EVSTWWO, Convert__RegSPERC1_0__DispSPE41_1__RegGxRCNoR01_2, 0, { MCK_RegSPERC, MCK_DispSPE4, MCK_RegGxRCNoR0 }, },
  { 5626 /* evstwwox */, PPC::EVSTWWOX, Convert__RegSPERC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPERC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 5635 /* evsubfsmiaaw */, PPC::EVSUBFSMIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5648 /* evsubfssiaaw */, PPC::EVSUBFSSIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5661 /* evsubfumiaaw */, PPC::EVSUBFUMIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5674 /* evsubfusiaaw */, PPC::EVSUBFUSIAAW, Convert__RegSPERC1_0__RegSPERC1_1, 0, { MCK_RegSPERC, MCK_RegSPERC }, },
  { 5687 /* evsubfw */, PPC::EVSUBFW, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5695 /* evsubifw */, PPC::EVSUBIFW, Convert__RegSPERC1_0__U5Imm1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_U5Imm, MCK_RegSPERC }, },
  { 5704 /* evxor */, PPC::EVXOR, Convert__RegSPERC1_0__RegSPERC1_1__RegSPERC1_2, 0, { MCK_RegSPERC, MCK_RegSPERC, MCK_RegSPERC }, },
  { 5710 /* extldi */, PPC::EXTLDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 5710 /* extldi */, PPC::EXTLDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 5717 /* extlwi */, PPC::EXTLWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 5717 /* extlwi */, PPC::EXTLWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 5724 /* extrdi */, PPC::EXTRDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 5724 /* extrdi */, PPC::EXTRDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 5731 /* extrwi */, PPC::EXTRWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 5731 /* extrwi */, PPC::EXTRWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 5738 /* extsb */, PPC::EXTSB, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 5738 /* extsb */, PPC::EXTSBo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 5744 /* extsh */, PPC::EXTSH, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 5744 /* extsh */, PPC::EXTSHo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 5750 /* extsw */, PPC::EXTSW, Convert__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 5750 /* extsw */, PPC::EXTSWo, Convert__RegG8RC1_1__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC }, },
  { 5756 /* extswsli */, PPC::EXTSWSLI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 5756 /* extswsli */, PPC::EXTSWSLIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 5765 /* fabs */, PPC::FABSS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5765 /* fabs */, PPC::FABSSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5770 /* fadd */, PPC::FADD, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5770 /* fadd */, PPC::FADDo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5775 /* fadds */, PPC::FADDS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5775 /* fadds */, PPC::FADDSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5781 /* fcfid */, PPC::FCFID, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5781 /* fcfid */, PPC::FCFIDo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5787 /* fcfids */, PPC::FCFIDS, Convert__RegF4RC1_0__RegF8RC1_1, 0, { MCK_RegF4RC, MCK_RegF8RC }, },
  { 5787 /* fcfids */, PPC::FCFIDSo, Convert__RegF4RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF8RC }, },
  { 5794 /* fcfidu */, PPC::FCFIDU, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5794 /* fcfidu */, PPC::FCFIDUo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5801 /* fcfidus */, PPC::FCFIDUS, Convert__RegF4RC1_0__RegF8RC1_1, 0, { MCK_RegF4RC, MCK_RegF8RC }, },
  { 5801 /* fcfidus */, PPC::FCFIDUSo, Convert__RegF4RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF8RC }, },
  { 5809 /* fcmpu */, PPC::FCMPUS, Convert__RegCRRC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegCRRC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5815 /* fcpsgn */, PPC::FCPSGNS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5815 /* fcpsgn */, PPC::FCPSGNSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5822 /* fctid */, PPC::FCTID, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5822 /* fctid */, PPC::FCTIDo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5828 /* fctidu */, PPC::FCTIDU, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5828 /* fctidu */, PPC::FCTIDUo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5835 /* fctiduz */, PPC::FCTIDUZ, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5835 /* fctiduz */, PPC::FCTIDUZo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5843 /* fctidz */, PPC::FCTIDZ, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5843 /* fctidz */, PPC::FCTIDZo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5850 /* fctiw */, PPC::FCTIW, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5850 /* fctiw */, PPC::FCTIWo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5856 /* fctiwu */, PPC::FCTIWU, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5856 /* fctiwu */, PPC::FCTIWUo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5863 /* fctiwuz */, PPC::FCTIWUZ, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5863 /* fctiwuz */, PPC::FCTIWUZo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5871 /* fctiwz */, PPC::FCTIWZ, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5871 /* fctiwz */, PPC::FCTIWZo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5878 /* fdiv */, PPC::FDIV, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5878 /* fdiv */, PPC::FDIVo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5883 /* fdivs */, PPC::FDIVS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5883 /* fdivs */, PPC::FDIVSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5889 /* fmadd */, PPC::FMADD, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5889 /* fmadd */, PPC::FMADDo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5895 /* fmadds */, PPC::FMADDS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5895 /* fmadds */, PPC::FMADDSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5902 /* fmr */, PPC::FMR, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5902 /* fmr */, PPC::FMRo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5906 /* fmsub */, PPC::FMSUB, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5906 /* fmsub */, PPC::FMSUBo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5912 /* fmsubs */, PPC::FMSUBS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5912 /* fmsubs */, PPC::FMSUBSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5919 /* fmul */, PPC::FMUL, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5919 /* fmul */, PPC::FMULo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5924 /* fmuls */, PPC::FMULS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5924 /* fmuls */, PPC::FMULSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5930 /* fnabs */, PPC::FNABSS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5930 /* fnabs */, PPC::FNABSSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5936 /* fneg */, PPC::FNEGS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5936 /* fneg */, PPC::FNEGSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5941 /* fnmadd */, PPC::FNMADD, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5941 /* fnmadd */, PPC::FNMADDo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5948 /* fnmadds */, PPC::FNMADDS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5948 /* fnmadds */, PPC::FNMADDSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5956 /* fnmsub */, PPC::FNMSUB, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5956 /* fnmsub */, PPC::FNMSUBo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3__RegF8RC1_4, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5963 /* fnmsubs */, PPC::FNMSUBS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5963 /* fnmsubs */, PPC::FNMSUBSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3__RegF4RC1_4, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5971 /* fre */, PPC::FRE, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 5971 /* fre */, PPC::FREo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 5975 /* fres */, PPC::FRES, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5975 /* fres */, PPC::FRESo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5980 /* frim */, PPC::FRIMS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5980 /* frim */, PPC::FRIMSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5985 /* frin */, PPC::FRINS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5985 /* frin */, PPC::FRINSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5990 /* frip */, PPC::FRIPS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5990 /* frip */, PPC::FRIPSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 5995 /* friz */, PPC::FRIZS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 5995 /* friz */, PPC::FRIZSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6000 /* frsp */, PPC::FRSP, Convert__RegF4RC1_0__RegF8RC1_1, 0, { MCK_RegF4RC, MCK_RegF8RC }, },
  { 6000 /* frsp */, PPC::FRSPo, Convert__RegF4RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF8RC }, },
  { 6005 /* frsqrte */, PPC::FRSQRTE, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 6005 /* frsqrte */, PPC::FRSQRTEo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 6013 /* frsqrtes */, PPC::FRSQRTES, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 6013 /* frsqrtes */, PPC::FRSQRTESo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6022 /* fsel */, PPC::FSELS, Convert__RegF4RC1_0__RegF8RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK_RegF4RC, MCK_RegF8RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6022 /* fsel */, PPC::FSELSo, Convert__RegF4RC1_1__RegF8RC1_2__RegF4RC1_3__RegF4RC1_4, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF8RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6027 /* fsqrt */, PPC::FSQRT, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 6027 /* fsqrt */, PPC::FSQRTo, Convert__RegF8RC1_1__RegF8RC1_2, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC }, },
  { 6033 /* fsqrts */, PPC::FSQRTS, Convert__RegF4RC1_0__RegF4RC1_1, 0, { MCK_RegF4RC, MCK_RegF4RC }, },
  { 6033 /* fsqrts */, PPC::FSQRTSo, Convert__RegF4RC1_1__RegF4RC1_2, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6040 /* fsub */, PPC::FSUB, Convert__RegF8RC1_0__RegF8RC1_1__RegF8RC1_2, 0, { MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 6040 /* fsub */, PPC::FSUBo, Convert__RegF8RC1_1__RegF8RC1_2__RegF8RC1_3, 0, { MCK__DOT_, MCK_RegF8RC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 6045 /* fsubs */, PPC::FSUBS, Convert__RegF4RC1_0__RegF4RC1_1__RegF4RC1_2, 0, { MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6045 /* fsubs */, PPC::FSUBSo, Convert__RegF4RC1_1__RegF4RC1_2__RegF4RC1_3, 0, { MCK__DOT_, MCK_RegF4RC, MCK_RegF4RC, MCK_RegF4RC }, },
  { 6051 /* ftdiv */, PPC::FTDIV, Convert__RegCRRC1_0__RegF8RC1_1__RegF8RC1_2, 0, { MCK_RegCRRC, MCK_RegF8RC, MCK_RegF8RC }, },
  { 6057 /* ftsqrt */, PPC::FTSQRT, Convert__RegCRRC1_0__RegF8RC1_1, 0, { MCK_RegCRRC, MCK_RegF8RC }, },
  { 6064 /* hrfid */, PPC::HRFID, Convert_NoOperands, 0, {  }, },
  { 6070 /* icbi */, PPC::ICBI, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6075 /* icbiep */, PPC::ICBIEP, Convert__RegGxRCNoR01_0__RegGxRC1_1, 0, { MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6082 /* icblc */, PPC::ICBLC, Convert__U4Imm1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_U4Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6088 /* icblq */, PPC::ICBLQ, Convert__U4Imm1_1__RegGxRCNoR01_2__RegGxRC1_3, 0, { MCK__DOT_, MCK_U4Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6094 /* icbt */, PPC::ICBT, Convert__U4Imm1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_U4Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6099 /* icbtls */, PPC::ICBTLS, Convert__U4Imm1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_U4Imm, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6106 /* iccci */, PPC::ICCCI, Convert__regR0__regR0, 0, {  }, },
  { 6106 /* iccci */, PPC::ICCCI, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 6112 /* ici */, PPC::ICCCI, Convert__regR0__regR0, 0, { MCK_0 }, },
  { 6116 /* inslwi */, PPC::INSLWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 6116 /* inslwi */, PPC::INSLWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 6123 /* insrdi */, PPC::INSRDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 6123 /* insrdi */, PPC::INSRDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 6130 /* insrwi */, PPC::INSRWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 6130 /* insrwi */, PPC::INSRWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 6137 /* isel */, PPC::ISEL, Convert__RegGPRC1_0__RegGPRCNoR01_1__RegGPRC1_2__RegCRBITRC1_3, 0, { MCK_RegGPRC, MCK_RegGPRCNoR0, MCK_RegGPRC, MCK_RegCRBITRC }, },
  { 6142 /* isync */, PPC::ISYNC, Convert_NoOperands, 0, {  }, },
  { 6148 /* la */, PPC::LAx, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6151 /* lbarx */, PPC::LBARX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6151 /* lbarx */, PPC::LBARXL, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_1 }, },
  { 6157 /* lbepx */, PPC::LBEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6163 /* lbz */, PPC::LBZ, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6167 /* lbzcix */, PPC::LBZCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 6174 /* lbzu */, PPC::LBZU, Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6179 /* lbzux */, PPC::LBZUX, Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6185 /* lbzx */, PPC::LBZXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 6185 /* lbzx */, PPC::LBZX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6190 /* ld */, PPC::LD, Convert__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegG8RC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 6193 /* ldarx */, PPC::LDARX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6193 /* ldarx */, PPC::LDARXL, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_1 }, },
  { 6199 /* ldat */, PPC::LDAT, Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm }, },
  { 6204 /* ldbrx */, PPC::LDBRX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6210 /* ldcix */, PPC::LDCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 6216 /* ldmx */, PPC::LDMX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6221 /* ldu */, PPC::LDU, Convert__RegG8RC1_0__imm_95_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegG8RC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 6225 /* ldux */, PPC::LDUX, Convert__RegG8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6230 /* ldx */, PPC::LDX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6230 /* ldx */, PPC::LDXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 6234 /* lfd */, PPC::LFD, Convert__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF8RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6238 /* lfdepx */, PPC::LFDEPX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6245 /* lfdu */, PPC::LFDU, Convert__RegF8RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF8RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6250 /* lfdux */, PPC::LFDUX, Convert__RegF8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6256 /* lfdx */, PPC::LFDX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6261 /* lfiwax */, PPC::LFIWAX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6268 /* lfiwzx */, PPC::LFIWZX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6275 /* lfs */, PPC::LFS, Convert__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6279 /* lfsu */, PPC::LFSU, Convert__RegF4RC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6284 /* lfsux */, PPC::LFSUX, Convert__RegF4RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6290 /* lfsx */, PPC::LFSX, Convert__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6295 /* lha */, PPC::LHA, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6299 /* lharx */, PPC::LHARX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6299 /* lharx */, PPC::LHARXL, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_1 }, },
  { 6305 /* lhau */, PPC::LHAU, Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6310 /* lhaux */, PPC::LHAUX, Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6316 /* lhax */, PPC::LHAX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6321 /* lhbrx */, PPC::LHBRX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6327 /* lhepx */, PPC::LHEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6333 /* lhz */, PPC::LHZ, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6337 /* lhzcix */, PPC::LHZCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 6344 /* lhzu */, PPC::LHZU, Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6349 /* lhzux */, PPC::LHZUX, Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6355 /* lhzx */, PPC::LHZXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 6355 /* lhzx */, PPC::LHZX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6360 /* li */, PPC::LI, Convert__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 6363 /* lis */, PPC::LIS, Convert__RegGPRC1_0__S17Imm1_1, 0, { MCK_RegGPRC, MCK_S17Imm }, },
  { 6367 /* lmw */, PPC::LMW, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6371 /* lnia */, PPC::ADDPCIS, Convert__RegG8RC1_0__imm_95_0, 0, { MCK_RegG8RC }, },
  { 6376 /* lswi */, PPC::LSWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 6381 /* lvebx */, PPC::LVEBX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6387 /* lvehx */, PPC::LVEHX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6393 /* lvewx */, PPC::LVEWX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6399 /* lvsl */, PPC::LVSL, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6404 /* lvsr */, PPC::LVSR, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6409 /* lvx */, PPC::LVX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6413 /* lvxl */, PPC::LVXL, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6418 /* lwa */, PPC::LWA, Convert__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegG8RC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 6422 /* lwarx */, PPC::LWARX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6422 /* lwarx */, PPC::LWARXL, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC, MCK_1 }, },
  { 6428 /* lwat */, PPC::LWAT, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 6433 /* lwaux */, PPC::LWAUX, Convert__RegG8RC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6439 /* lwax */, PPC::LWAX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6444 /* lwbrx */, PPC::LWBRX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6450 /* lwepx */, PPC::LWEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6456 /* lwsync */, PPC::SYNC, Convert__imm_95_1, 0, {  }, },
  { 6463 /* lwz */, PPC::LWZ, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6463 /* lwz */, PPC::SPELWZ, Convert__RegSPE4RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegSPE4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6467 /* lwzcix */, PPC::LWZCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 6474 /* lwzu */, PPC::LWZU, Convert__RegGPRC1_0__imm_95_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 6479 /* lwzux */, PPC::LWZUX, Convert__RegGPRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6485 /* lwzx */, PPC::LWZXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 6485 /* lwzx */, PPC::LWZX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6485 /* lwzx */, PPC::SPELWZX, Convert__RegSPE4RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPE4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6490 /* lxsd */, PPC::LXSD, Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegVFRC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 6495 /* lxsdx */, PPC::LXSDX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6501 /* lxsibzx */, PPC::LXSIBZX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6509 /* lxsihzx */, PPC::LXSIHZX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6517 /* lxsiwax */, PPC::LXSIWAX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6525 /* lxsiwzx */, PPC::LXSIWZX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6533 /* lxssp */, PPC::LXSSP, Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegVFRC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 6539 /* lxsspx */, PPC::LXSSPX, Convert__RegVSSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6546 /* lxv */, PPC::LXV, Convert__RegVSRC1_0__DispRIX161_1__RegGxRCNoR01_2, 0, { MCK_RegVSRC, MCK_DispRIX16, MCK_RegGxRCNoR0 }, },
  { 6550 /* lxvb16x */, PPC::LXVB16X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6558 /* lxvd2x */, PPC::LXVD2X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6565 /* lxvdsx */, PPC::LXVDSX, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6572 /* lxvh8x */, PPC::LXVH8X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6579 /* lxvl */, PPC::LXVL, Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_Imm, MCK_RegG8RC }, },
  { 6584 /* lxvll */, PPC::LXVLL, Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_Imm, MCK_RegG8RC }, },
  { 6590 /* lxvw4x */, PPC::LXVW4X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6597 /* lxvwsx */, PPC::LXVWSX, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6604 /* lxvx */, PPC::LXVX, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 6609 /* maddhd */, PPC::MADDHD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 6616 /* maddhdu */, PPC::MADDHDU, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 6624 /* maddld */, PPC::MADDLD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 6631 /* mbar */, PPC::MBAR, Convert__imm_95_0, 0, {  }, },
  { 6631 /* mbar */, PPC::MBAR, Convert__U5Imm1_0, 0, { MCK_U5Imm }, },
  { 6636 /* mcrf */, PPC::MCRF, Convert__RegCRRC1_0__RegCRRC1_1, 0, { MCK_RegCRRC, MCK_RegCRRC }, },
  { 6641 /* mcrfs */, PPC::MCRFS, Convert__RegCRRC1_0__RegCRRC1_1, 0, { MCK_RegCRRC, MCK_RegCRRC }, },
  { 6647 /* mcrxrx */, PPC::MCRXRX, Convert__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 6654 /* mfamr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_29, 0, { MCK_RegGPRC }, },
  { 6660 /* mfasr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_280, 0, { MCK_RegGPRC }, },
  { 6666 /* mfbhrbe */, PPC::MFBHRBE, Convert__RegGPRC1_0__U10Imm1_1__imm_95_0, 0, { MCK_RegGPRC, MCK_U10Imm }, },
  { 6674 /* mfbr0 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_128, 0, { MCK_RegGPRC }, },
  { 6680 /* mfbr1 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_129, 0, { MCK_RegGPRC }, },
  { 6686 /* mfbr2 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_130, 0, { MCK_RegGPRC }, },
  { 6692 /* mfbr3 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_131, 0, { MCK_RegGPRC }, },
  { 6698 /* mfbr4 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_132, 0, { MCK_RegGPRC }, },
  { 6704 /* mfbr5 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_133, 0, { MCK_RegGPRC }, },
  { 6710 /* mfbr6 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_134, 0, { MCK_RegGPRC }, },
  { 6716 /* mfbr7 */, PPC::MFDCR, Convert__RegGPRC1_0__imm_95_135, 0, { MCK_RegGPRC }, },
  { 6722 /* mfcfar */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_28, 0, { MCK_RegGPRC }, },
  { 6729 /* mfcr */, PPC::MFCR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 6734 /* mfctr */, PPC::MFCTR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 6740 /* mfdar */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_19, 0, { MCK_RegGPRC }, },
  { 6746 /* mfdbatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_537, 0, { MCK_RegGPRC, MCK_0 }, },
  { 6746 /* mfdbatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_539, 0, { MCK_RegGPRC, MCK_1 }, },
  { 6746 /* mfdbatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_541, 0, { MCK_RegGPRC, MCK_2 }, },
  { 6746 /* mfdbatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_543, 0, { MCK_RegGPRC, MCK_3 }, },
  { 6754 /* mfdbatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_536, 0, { MCK_RegGPRC, MCK_0 }, },
  { 6754 /* mfdbatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_538, 0, { MCK_RegGPRC, MCK_1 }, },
  { 6754 /* mfdbatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_540, 0, { MCK_RegGPRC, MCK_2 }, },
  { 6754 /* mfdbatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_542, 0, { MCK_RegGPRC, MCK_3 }, },
  { 6762 /* mfdccr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_1018, 0, { MCK_RegGPRC }, },
  { 6769 /* mfdcr */, PPC::MFDCR, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 6775 /* mfdear */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_981, 0, { MCK_RegGPRC }, },
  { 6782 /* mfdec */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_22, 0, { MCK_RegGPRC }, },
  { 6782 /* mfdec */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_22, 0, { MCK_RegGPRC }, },
  { 6788 /* mfdscr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_17, 0, { MCK_RegGPRC }, },
  { 6795 /* mfdsisr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_18, 0, { MCK_RegGPRC }, },
  { 6803 /* mfesr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_980, 0, { MCK_RegGPRC }, },
  { 6809 /* mffprd */, PPC::MFVSRD, Convert__RegG8RC1_0__RegF8RC1_1, 0, { MCK_RegG8RC, MCK_RegF8RC }, },
  { 6816 /* mffs */, PPC::MFFS, Convert__RegF8RC1_0, 0, { MCK_RegF8RC }, },
  { 6816 /* mffs */, PPC::MFFSo, Convert__RegF8RC1_1, 0, { MCK__DOT_, MCK_RegF8RC }, },
  { 6821 /* mffscdrn */, PPC::MFFSCDRN, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 6830 /* mffscdrni */, PPC::MFFSCDRNI, Convert__RegF8RC1_0__U3Imm1_1, 0, { MCK_RegF8RC, MCK_U3Imm }, },
  { 6840 /* mffsce */, PPC::MFFSCE, Convert__RegF8RC1_0, 0, { MCK_RegF8RC }, },
  { 6847 /* mffscrn */, PPC::MFFSCRN, Convert__RegF8RC1_0__RegF8RC1_1, 0, { MCK_RegF8RC, MCK_RegF8RC }, },
  { 6855 /* mffscrni */, PPC::MFFSCRNI, Convert__RegF8RC1_0__U2Imm1_1, 0, { MCK_RegF8RC, MCK_U2Imm }, },
  { 6864 /* mffsl */, PPC::MFFSL, Convert__RegF8RC1_0, 0, { MCK_RegF8RC }, },
  { 6870 /* mfibatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_529, 0, { MCK_RegGPRC, MCK_0 }, },
  { 6870 /* mfibatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_531, 0, { MCK_RegGPRC, MCK_1 }, },
  { 6870 /* mfibatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_533, 0, { MCK_RegGPRC, MCK_2 }, },
  { 6870 /* mfibatl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_535, 0, { MCK_RegGPRC, MCK_3 }, },
  { 6878 /* mfibatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_528, 0, { MCK_RegGPRC, MCK_0 }, },
  { 6878 /* mfibatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_530, 0, { MCK_RegGPRC, MCK_1 }, },
  { 6878 /* mfibatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_532, 0, { MCK_RegGPRC, MCK_2 }, },
  { 6878 /* mfibatu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_534, 0, { MCK_RegGPRC, MCK_3 }, },
  { 6886 /* mficcr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_1019, 0, { MCK_RegGPRC }, },
  { 6893 /* mflr */, PPC::MFLR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 6898 /* mfmsr */, PPC::MFMSR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 6904 /* mfocrf */, PPC::MFOCRF, Convert__RegGPRC1_0__CRBitMask1_1, 0, { MCK_RegGPRC, MCK_CRBitMask }, },
  { 6911 /* mfpid */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_48, 0, { MCK_RegGPRC }, },
  { 6917 /* mfpmr */, PPC::MFPMR, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 6923 /* mfpvr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_287, 0, { MCK_RegGPRC }, },
  { 6929 /* mfrtcl */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_5, 0, { MCK_RegGPRC }, },
  { 6936 /* mfrtcu */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_4, 0, { MCK_RegGPRC }, },
  { 6943 /* mfsdr1 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_25, 0, { MCK_RegGPRC }, },
  { 6943 /* mfsdr1 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_25, 0, { MCK_RegGPRC }, },
  { 6950 /* mfspefscr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_512, 0, { MCK_RegGPRC }, },
  { 6960 /* mfspr */, PPC::MFSPR, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_272, 0, { MCK_RegGPRC, MCK_0 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_273, 0, { MCK_RegGPRC, MCK_1 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_274, 0, { MCK_RegGPRC, MCK_2 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_275, 0, { MCK_RegGPRC, MCK_3 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_260, 0, { MCK_RegGPRC, MCK_4 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_261, 0, { MCK_RegGPRC, MCK_5 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_262, 0, { MCK_RegGPRC, MCK_6 }, },
  { 6966 /* mfsprg */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_263, 0, { MCK_RegGPRC, MCK_7 }, },
  { 6973 /* mfsprg0 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_272, 0, { MCK_RegGPRC }, },
  { 6981 /* mfsprg1 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_273, 0, { MCK_RegGPRC }, },
  { 6989 /* mfsprg2 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_274, 0, { MCK_RegGPRC }, },
  { 6997 /* mfsprg3 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_275, 0, { MCK_RegGPRC }, },
  { 7005 /* mfsprg4 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_260, 0, { MCK_RegGPRC }, },
  { 7013 /* mfsprg5 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_261, 0, { MCK_RegGPRC }, },
  { 7021 /* mfsprg6 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_262, 0, { MCK_RegGPRC }, },
  { 7029 /* mfsprg7 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_263, 0, { MCK_RegGPRC }, },
  { 7037 /* mfsr */, PPC::MFSR, Convert__RegGPRC1_0__U4Imm1_1, 0, { MCK_RegGPRC, MCK_U4Imm }, },
  { 7042 /* mfsrin */, PPC::MFSRIN, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7049 /* mfsrr0 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_26, 0, { MCK_RegGPRC }, },
  { 7049 /* mfsrr0 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_26, 0, { MCK_RegGPRC }, },
  { 7056 /* mfsrr1 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_27, 0, { MCK_RegGPRC }, },
  { 7056 /* mfsrr1 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_27, 0, { MCK_RegGPRC }, },
  { 7063 /* mfsrr2 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_990, 0, { MCK_RegGPRC }, },
  { 7070 /* mfsrr3 */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_991, 0, { MCK_RegGPRC }, },
  { 7077 /* mftb */, PPC::MFTB, Convert__RegGPRC1_0__imm_95_268, 0, { MCK_RegGPRC }, },
  { 7077 /* mftb */, PPC::MFTB, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 7082 /* mftbhi */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_988, 0, { MCK_RegGPRC }, },
  { 7089 /* mftbl */, PPC::MFTB, Convert__RegGPRC1_0__imm_95_268, 0, { MCK_RegGPRC }, },
  { 7095 /* mftblo */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_989, 0, { MCK_RegGPRC }, },
  { 7102 /* mftbu */, PPC::MFTB, Convert__RegGPRC1_0__imm_95_269, 0, { MCK_RegGPRC }, },
  { 7108 /* mftcr */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_986, 0, { MCK_RegGPRC }, },
  { 7114 /* mfvrd */, PPC::MFVRD, Convert__RegG8RC1_0__RegVRRC1_1, 0, { MCK_RegG8RC, MCK_RegVRRC }, },
  { 7120 /* mfvrsave */, PPC::MFVRSAVE, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7129 /* mfvscr */, PPC::MFVSCR, Convert__RegVRRC1_0, 0, { MCK_RegVRRC }, },
  { 7136 /* mfvsrd */, PPC::MFVSRD, Convert__RegG8RC1_0__RegVSFRC1_1, 0, { MCK_RegG8RC, MCK_RegVSFRC }, },
  { 7143 /* mfvsrld */, PPC::MFVSRLD, Convert__RegG8RC1_0__RegVSRC1_1, 0, { MCK_RegG8RC, MCK_RegVSRC }, },
  { 7151 /* mfvsrwz */, PPC::MFVSRWZ, Convert__RegGPRC1_0__RegVSFRC1_1, 0, { MCK_RegGPRC, MCK_RegVSFRC }, },
  { 7159 /* mfxer */, PPC::MFSPR, Convert__RegGPRC1_0__imm_95_1, 0, { MCK_RegGPRC }, },
  { 7165 /* modsd */, PPC::MODSD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7171 /* modsw */, PPC::MODSW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7177 /* modud */, PPC::MODUD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7183 /* moduw */, PPC::MODUW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7189 /* mr */, PPC::OR8, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 7189 /* mr */, PPC::OR8o, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7192 /* msgsync */, PPC::MSGSYNC, Convert_NoOperands, 0, {  }, },
  { 7200 /* msync */, PPC::SYNC, Convert__imm_95_0, 0, {  }, },
  { 7206 /* mtamr */, PPC::MTSPR, Convert__imm_95_29__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7212 /* mtasr */, PPC::MTSPR, Convert__imm_95_280__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7212 /* mtasr */, PPC::MTSPR, Convert__imm_95_280__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7218 /* mtbr0 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_128, 0, { MCK_RegGPRC }, },
  { 7224 /* mtbr1 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_129, 0, { MCK_RegGPRC }, },
  { 7230 /* mtbr2 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_130, 0, { MCK_RegGPRC }, },
  { 7236 /* mtbr3 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_131, 0, { MCK_RegGPRC }, },
  { 7242 /* mtbr4 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_132, 0, { MCK_RegGPRC }, },
  { 7248 /* mtbr5 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_133, 0, { MCK_RegGPRC }, },
  { 7254 /* mtbr6 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_134, 0, { MCK_RegGPRC }, },
  { 7260 /* mtbr7 */, PPC::MTDCR, Convert__RegGPRC1_0__imm_95_135, 0, { MCK_RegGPRC }, },
  { 7266 /* mtcfar */, PPC::MTSPR, Convert__imm_95_28__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7273 /* mtcr */, PPC::MTCRF8, Convert__imm_95_255__RegG8RC1_0, 0, { MCK_RegG8RC }, },
  { 7278 /* mtcrf */, PPC::MTCRF, Convert__Imm1_0__RegGPRC1_1, 0, { MCK_Imm, MCK_RegGPRC }, },
  { 7284 /* mtctr */, PPC::MTCTR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7290 /* mtdar */, PPC::MTSPR, Convert__imm_95_19__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7296 /* mtdbatl */, PPC::MTSPR, Convert__imm_95_537__RegGPRC1_1, 0, { MCK_0, MCK_RegGPRC }, },
  { 7296 /* mtdbatl */, PPC::MTSPR, Convert__imm_95_539__RegGPRC1_1, 0, { MCK_1, MCK_RegGPRC }, },
  { 7296 /* mtdbatl */, PPC::MTSPR, Convert__imm_95_541__RegGPRC1_1, 0, { MCK_2, MCK_RegGPRC }, },
  { 7296 /* mtdbatl */, PPC::MTSPR, Convert__imm_95_543__RegGPRC1_1, 0, { MCK_3, MCK_RegGPRC }, },
  { 7304 /* mtdbatu */, PPC::MTSPR, Convert__imm_95_536__RegGPRC1_1, 0, { MCK_0, MCK_RegGPRC }, },
  { 7304 /* mtdbatu */, PPC::MTSPR, Convert__imm_95_538__RegGPRC1_1, 0, { MCK_1, MCK_RegGPRC }, },
  { 7304 /* mtdbatu */, PPC::MTSPR, Convert__imm_95_540__RegGPRC1_1, 0, { MCK_2, MCK_RegGPRC }, },
  { 7304 /* mtdbatu */, PPC::MTSPR, Convert__imm_95_542__RegGPRC1_1, 0, { MCK_3, MCK_RegGPRC }, },
  { 7312 /* mtdccr */, PPC::MTSPR, Convert__imm_95_1018__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7319 /* mtdcr */, PPC::MTDCR, Convert__RegGPRC1_1__Imm1_0, 0, { MCK_Imm, MCK_RegGPRC }, },
  { 7325 /* mtdear */, PPC::MTSPR, Convert__imm_95_981__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7332 /* mtdec */, PPC::MTSPR, Convert__imm_95_22__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7332 /* mtdec */, PPC::MTSPR, Convert__imm_95_22__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7338 /* mtdscr */, PPC::MTSPR, Convert__imm_95_17__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7345 /* mtdsisr */, PPC::MTSPR, Convert__imm_95_18__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7353 /* mtesr */, PPC::MTSPR, Convert__imm_95_980__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7359 /* mtfsb0 */, PPC::MTFSB0, Convert__U5Imm1_0, 0, { MCK_U5Imm }, },
  { 7366 /* mtfsb1 */, PPC::MTFSB1, Convert__U5Imm1_0, 0, { MCK_U5Imm }, },
  { 7373 /* mtfsf */, PPC::MTFSF, Convert__Imm1_0__RegF8RC1_1__imm_95_0__imm_95_0, 0, { MCK_Imm, MCK_RegF8RC }, },
  { 7373 /* mtfsf */, PPC::MTFSFo, Convert__Imm1_1__RegF8RC1_2__imm_95_0__imm_95_0, 0, { MCK__DOT_, MCK_Imm, MCK_RegF8RC }, },
  { 7373 /* mtfsf */, PPC::MTFSF, Convert__Imm1_0__RegF8RC1_1__Imm1_2__Imm1_3, 0, { MCK_Imm, MCK_RegF8RC, MCK_Imm, MCK_Imm }, },
  { 7373 /* mtfsf */, PPC::MTFSFo, Convert__Imm1_1__RegF8RC1_2__Imm1_3__Imm1_4, 0, { MCK__DOT_, MCK_Imm, MCK_RegF8RC, MCK_Imm, MCK_Imm }, },
  { 7379 /* mtfsfi */, PPC::MTFSFI, Convert__RegCRRC1_0__Imm1_1__imm_95_0, 0, { MCK_RegCRRC, MCK_Imm }, },
  { 7379 /* mtfsfi */, PPC::MTFSFIo, Convert__RegCRRC1_1__Imm1_2__imm_95_0, 0, { MCK__DOT_, MCK_RegCRRC, MCK_Imm }, },
  { 7379 /* mtfsfi */, PPC::MTFSFI, Convert__RegCRRC1_0__Imm1_1__Imm1_2, 0, { MCK_RegCRRC, MCK_Imm, MCK_Imm }, },
  { 7379 /* mtfsfi */, PPC::MTFSFIo, Convert__RegCRRC1_1__Imm1_2__Imm1_3, 0, { MCK__DOT_, MCK_RegCRRC, MCK_Imm, MCK_Imm }, },
  { 7386 /* mtibatl */, PPC::MTSPR, Convert__imm_95_529__RegGPRC1_1, 0, { MCK_0, MCK_RegGPRC }, },
  { 7386 /* mtibatl */, PPC::MTSPR, Convert__imm_95_531__RegGPRC1_1, 0, { MCK_1, MCK_RegGPRC }, },
  { 7386 /* mtibatl */, PPC::MTSPR, Convert__imm_95_533__RegGPRC1_1, 0, { MCK_2, MCK_RegGPRC }, },
  { 7386 /* mtibatl */, PPC::MTSPR, Convert__imm_95_535__RegGPRC1_1, 0, { MCK_3, MCK_RegGPRC }, },
  { 7394 /* mtibatu */, PPC::MTSPR, Convert__imm_95_528__RegGPRC1_1, 0, { MCK_0, MCK_RegGPRC }, },
  { 7394 /* mtibatu */, PPC::MTSPR, Convert__imm_95_530__RegGPRC1_1, 0, { MCK_1, MCK_RegGPRC }, },
  { 7394 /* mtibatu */, PPC::MTSPR, Convert__imm_95_532__RegGPRC1_1, 0, { MCK_2, MCK_RegGPRC }, },
  { 7394 /* mtibatu */, PPC::MTSPR, Convert__imm_95_534__RegGPRC1_1, 0, { MCK_3, MCK_RegGPRC }, },
  { 7402 /* mticcr */, PPC::MTSPR, Convert__imm_95_1019__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7409 /* mtlr */, PPC::MTLR, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7414 /* mtmsr */, PPC::MTMSR, Convert__RegGPRC1_0__imm_95_0, 0, { MCK_RegGPRC }, },
  { 7414 /* mtmsr */, PPC::MTMSR, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 7420 /* mtmsrd */, PPC::MTMSRD, Convert__RegGPRC1_0__imm_95_0, 0, { MCK_RegGPRC }, },
  { 7420 /* mtmsrd */, PPC::MTMSRD, Convert__RegGPRC1_0__Imm1_1, 0, { MCK_RegGPRC, MCK_Imm }, },
  { 7427 /* mtocrf */, PPC::MTOCRF, Convert__CRBitMask1_0__RegGPRC1_1, 0, { MCK_CRBitMask, MCK_RegGPRC }, },
  { 7434 /* mtpid */, PPC::MTSPR, Convert__imm_95_48__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7440 /* mtpmr */, PPC::MTPMR, Convert__Imm1_0__RegGPRC1_1, 0, { MCK_Imm, MCK_RegGPRC }, },
  { 7446 /* mtsdr1 */, PPC::MTSPR, Convert__imm_95_25__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7446 /* mtsdr1 */, PPC::MTSPR, Convert__imm_95_25__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7453 /* mtspefscr */, PPC::MTSPR, Convert__imm_95_512__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7463 /* mtspr */, PPC::MTSPR, Convert__Imm1_0__RegGPRC1_1, 0, { MCK_Imm, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_272__RegGPRC1_1, 0, { MCK_0, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_273__RegGPRC1_1, 0, { MCK_1, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_274__RegGPRC1_1, 0, { MCK_2, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_275__RegGPRC1_1, 0, { MCK_3, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_260__RegGPRC1_1, 0, { MCK_4, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_261__RegGPRC1_1, 0, { MCK_5, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_262__RegGPRC1_1, 0, { MCK_6, MCK_RegGPRC }, },
  { 7469 /* mtsprg */, PPC::MTSPR, Convert__imm_95_263__RegGPRC1_1, 0, { MCK_7, MCK_RegGPRC }, },
  { 7476 /* mtsprg0 */, PPC::MTSPR, Convert__imm_95_272__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7484 /* mtsprg1 */, PPC::MTSPR, Convert__imm_95_273__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7492 /* mtsprg2 */, PPC::MTSPR, Convert__imm_95_274__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7500 /* mtsprg3 */, PPC::MTSPR, Convert__imm_95_275__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7508 /* mtsprg4 */, PPC::MTSPR, Convert__imm_95_260__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7516 /* mtsprg5 */, PPC::MTSPR, Convert__imm_95_261__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7524 /* mtsprg6 */, PPC::MTSPR, Convert__imm_95_262__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7532 /* mtsprg7 */, PPC::MTSPR, Convert__imm_95_263__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7540 /* mtsr */, PPC::MTSR, Convert__RegGPRC1_1__U4Imm1_0, 0, { MCK_U4Imm, MCK_RegGPRC }, },
  { 7545 /* mtsrin */, PPC::MTSRIN, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7552 /* mtsrr0 */, PPC::MTSPR, Convert__imm_95_26__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7552 /* mtsrr0 */, PPC::MTSPR, Convert__imm_95_26__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7559 /* mtsrr1 */, PPC::MTSPR, Convert__imm_95_27__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7559 /* mtsrr1 */, PPC::MTSPR, Convert__imm_95_27__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7566 /* mtsrr2 */, PPC::MTSPR, Convert__imm_95_990__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7573 /* mtsrr3 */, PPC::MTSPR, Convert__imm_95_991__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7580 /* mttbhi */, PPC::MTSPR, Convert__imm_95_988__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7587 /* mttbl */, PPC::MTSPR, Convert__imm_95_284__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7593 /* mttblo */, PPC::MTSPR, Convert__imm_95_989__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7600 /* mttbu */, PPC::MTSPR, Convert__imm_95_285__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7606 /* mttcr */, PPC::MTSPR, Convert__imm_95_986__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7612 /* mtvrsave */, PPC::MTVRSAVE, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7621 /* mtvscr */, PPC::MTVSCR, Convert__RegVRRC1_0, 0, { MCK_RegVRRC }, },
  { 7628 /* mtvsrd */, PPC::MTVSRD, Convert__RegVSFRC1_0__RegG8RC1_1, 0, { MCK_RegVSFRC, MCK_RegG8RC }, },
  { 7635 /* mtvsrdd */, PPC::MTVSRDD, Convert__RegVSRC1_0__RegG8RCNoX01_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_RegG8RCNoX0, MCK_RegG8RC }, },
  { 7643 /* mtvsrwa */, PPC::MTVSRWA, Convert__RegVSFRC1_0__RegGPRC1_1, 0, { MCK_RegVSFRC, MCK_RegGPRC }, },
  { 7651 /* mtvsrws */, PPC::MTVSRWS, Convert__RegVSRC1_0__RegGPRC1_1, 0, { MCK_RegVSRC, MCK_RegGPRC }, },
  { 7659 /* mtvsrwz */, PPC::MTVSRWZ, Convert__RegVSFRC1_0__RegGPRC1_1, 0, { MCK_RegVSFRC, MCK_RegGPRC }, },
  { 7667 /* mtxer */, PPC::MTSPR, Convert__imm_95_1__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 7673 /* mulhd */, PPC::MULHD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7673 /* mulhd */, PPC::MULHDo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7679 /* mulhdu */, PPC::MULHDU, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7679 /* mulhdu */, PPC::MULHDUo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7686 /* mulhw */, PPC::MULHW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7686 /* mulhw */, PPC::MULHWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7692 /* mulhwu */, PPC::MULHWU, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7692 /* mulhwu */, PPC::MULHWUo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7699 /* mulld */, PPC::MULLD, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7699 /* mulld */, PPC::MULLDo, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7705 /* mulli */, PPC::MULLI, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 7711 /* mullw */, PPC::MULLW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7711 /* mullw */, PPC::MULLWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7717 /* nand */, PPC::NAND, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7717 /* nand */, PPC::NANDo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7722 /* nap */, PPC::NAP, Convert_NoOperands, 0, {  }, },
  { 7726 /* neg */, PPC::NEG, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7726 /* neg */, PPC::NEGo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7730 /* nop */, PPC::NOP, Convert_NoOperands, 0, {  }, },
  { 7734 /* nor */, PPC::NOR, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7734 /* nor */, PPC::NORo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7738 /* not */, PPC::NOR8, Convert__RegG8RC1_0__RegG8RC1_1__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 7738 /* not */, PPC::NOR8o, Convert__RegG8RC1_1__RegG8RC1_2__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC }, },
  { 7742 /* or */, PPC::OR, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7742 /* or */, PPC::ORo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7745 /* orc */, PPC::ORC, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7745 /* orc */, PPC::ORCo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 7749 /* ori */, PPC::ORI, Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 7753 /* oris */, PPC::ORIS, Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 7758 /* paste */, PPC::CP_PASTEx, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7758 /* paste */, PPC::CP_PASTE, Convert__RegGPRC1_0__RegGPRC1_1__U1Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U1Imm }, },
  { 7758 /* paste */, PPC::CP_PASTEo, Convert__RegGPRC1_1__RegGPRC1_2__U1Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U1Imm }, },
  { 7764 /* paste_last */, PPC::CP_PASTE_LAST, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7775 /* popcntb */, PPC::POPCNTB, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7783 /* popcntd */, PPC::POPCNTD, Convert__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 7791 /* popcntw */, PPC::POPCNTW, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 7799 /* ptesync */, PPC::SYNC, Convert__imm_95_2, 0, {  }, },
  { 7807 /* qvaligni */, PPC::QVALIGNI, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__U2Imm1_3, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_U2Imm }, },
  { 7816 /* qvesplati */, PPC::QVESPLATI, Convert__RegQFRC1_0__RegQFRC1_1__U2Imm1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_U2Imm }, },
  { 7826 /* qvfabs */, PPC::QVFABS, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7833 /* qvfadd */, PPC::QVFADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 7840 /* qvfadds */, PPC::QVFADDSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 7848 /* qvfand */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_1, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 7855 /* qvfandc */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_4, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 7863 /* qvfcfid */, PPC::QVFCFID, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7871 /* qvfcfids */, PPC::QVFCFIDS, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7880 /* qvfcfidu */, PPC::QVFCFIDU, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7889 /* qvfcfidus */, PPC::QVFCFIDUS, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7899 /* qvfclr */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_0, 0, { MCK_RegQBRC }, },
  { 7906 /* qvfcmpeq */, PPC::QVFCMPEQb, Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQBRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 7915 /* qvfcmpgt */, PPC::QVFCMPGTb, Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQBRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 7924 /* qvfcmplt */, PPC::QVFCMPLTb, Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQBRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 7933 /* qvfcpsgn */, PPC::QVFCPSGN, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 7942 /* qvfctfb */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_5, 0, { MCK_RegQBRC, MCK_RegQBRC }, },
  { 7950 /* qvfctid */, PPC::QVFCTID, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7958 /* qvfctidu */, PPC::QVFCTIDU, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7967 /* qvfctiduz */, PPC::QVFCTIDUZ, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7977 /* qvfctidz */, PPC::QVFCTIDZ, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7986 /* qvfctiw */, PPC::QVFCTIW, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 7994 /* qvfctiwu */, PPC::QVFCTIWU, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8003 /* qvfctiwuz */, PPC::QVFCTIWUZ, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8013 /* qvfctiwz */, PPC::QVFCTIWZ, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8022 /* qvfequ */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_9, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8029 /* qvflogical */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__U12Imm1_3, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC, MCK_U12Imm }, },
  { 8040 /* qvfmadd */, PPC::QVFMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8048 /* qvfmadds */, PPC::QVFMADDSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8057 /* qvfmr */, PPC::QVFMR, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8063 /* qvfmsub */, PPC::QVFMSUB, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8071 /* qvfmsubs */, PPC::QVFMSUBSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8080 /* qvfmul */, PPC::QVFMUL, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8087 /* qvfmuls */, PPC::QVFMULSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8095 /* qvfnabs */, PPC::QVFNABS, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8103 /* qvfnand */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_14, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8111 /* qvfneg */, PPC::QVFNEG, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8118 /* qvfnmadd */, PPC::QVFNMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8127 /* qvfnmadds */, PPC::QVFNMADDSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8137 /* qvfnmsub */, PPC::QVFNMSUB, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8146 /* qvfnmsubs */, PPC::QVFNMSUBSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_3__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8156 /* qvfnor */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_8, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8163 /* qvfnot */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_1__imm_95_10, 0, { MCK_RegQBRC, MCK_RegQBRC }, },
  { 8170 /* qvfor */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_7, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8176 /* qvforc */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_13, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8183 /* qvfperm */, PPC::QVFPERM, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2__RegQFRC1_3, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8191 /* qvfre */, PPC::QVFRE, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8197 /* qvfres */, PPC::QVFRES, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8204 /* qvfrim */, PPC::QVFRIM, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8211 /* qvfrin */, PPC::QVFRIN, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8218 /* qvfrip */, PPC::QVFRIP, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8225 /* qvfriz */, PPC::QVFRIZ, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8232 /* qvfrsp */, PPC::QVFRSPs, Convert__RegQSRC1_0__RegQFRC1_1, 0, { MCK_RegQSRC, MCK_RegQFRC }, },
  { 8239 /* qvfrsqrte */, PPC::QVFRSQRTE, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8249 /* qvfrsqrtes */, PPC::QVFRSQRTES, Convert__RegQFRC1_0__RegQFRC1_1, 0, { MCK_RegQFRC, MCK_RegQFRC }, },
  { 8260 /* qvfsel */, PPC::QVFSELb, Convert__RegQFRC1_0__RegQBRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQBRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8267 /* qvfset */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_0__RegQBRC1_0__imm_95_15, 0, { MCK_RegQBRC }, },
  { 8274 /* qvfsub */, PPC::QVFSUB, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8281 /* qvfsubs */, PPC::QVFSUBSs, Convert__RegQSRC1_0__RegQSRC1_1__RegQSRC1_2, 0, { MCK_RegQSRC, MCK_RegQSRC, MCK_RegQSRC }, },
  { 8289 /* qvftstnan */, PPC::QVFTSTNANb, Convert__RegQBRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQBRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8299 /* qvfxmadd */, PPC::QVFXMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8308 /* qvfxmadds */, PPC::QVFXMADDS, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8318 /* qvfxmul */, PPC::QVFXMUL, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8326 /* qvfxmuls */, PPC::QVFXMULS, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8335 /* qvfxor */, PPC::QVFLOGICALb, Convert__RegQBRC1_0__RegQBRC1_1__RegQBRC1_2__imm_95_6, 0, { MCK_RegQBRC, MCK_RegQBRC, MCK_RegQBRC }, },
  { 8342 /* qvfxxcpnmadd */, PPC::QVFXXCPNMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8355 /* qvfxxcpnmadds */, PPC::QVFXXCPNMADDS, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8369 /* qvfxxmadd */, PPC::QVFXXMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8379 /* qvfxxmadds */, PPC::QVFXXMADDS, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8390 /* qvfxxnpmadd */, PPC::QVFXXNPMADD, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8402 /* qvfxxnpmadds */, PPC::QVFXXNPMADDS, Convert__RegQFRC1_0__RegQFRC1_1__RegQFRC1_3__RegQFRC1_2, 0, { MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC, MCK_RegQFRC }, },
  { 8415 /* qvgpci */, PPC::QVGPCI, Convert__RegQFRC1_0__U12Imm1_1, 0, { MCK_RegQFRC, MCK_U12Imm }, },
  { 8422 /* qvlfcdux */, PPC::QVLFCDUX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8431 /* qvlfcduxa */, PPC::QVLFCDUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8441 /* qvlfcdx */, PPC::QVLFCDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8449 /* qvlfcdxa */, PPC::QVLFCDXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8458 /* qvlfcsux */, PPC::QVLFCSUX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8467 /* qvlfcsuxa */, PPC::QVLFCSUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8477 /* qvlfcsx */, PPC::QVLFCSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8485 /* qvlfcsxa */, PPC::QVLFCSXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8494 /* qvlfdux */, PPC::QVLFDUX, Convert__RegQFRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8502 /* qvlfduxa */, PPC::QVLFDUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8511 /* qvlfdx */, PPC::QVLFDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8518 /* qvlfdxa */, PPC::QVLFDXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8526 /* qvlfiwax */, PPC::QVLFIWAX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8535 /* qvlfiwaxa */, PPC::QVLFIWAXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8545 /* qvlfiwzx */, PPC::QVLFIWZX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8554 /* qvlfiwzxa */, PPC::QVLFIWZXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8564 /* qvlfsux */, PPC::QVLFSUX, Convert__RegQSRC1_0__imm_95_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8572 /* qvlfsuxa */, PPC::QVLFSUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8581 /* qvlfsx */, PPC::QVLFSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8588 /* qvlfsxa */, PPC::QVLFSXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8596 /* qvlpcldx */, PPC::QVLPCLDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8605 /* qvlpclsx */, PPC::QVLPCLSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8614 /* qvlpcrdx */, PPC::QVLPCRDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8623 /* qvlpcrsx */, PPC::QVLPCRSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8632 /* qvstfcdux */, PPC::QVSTFCDUX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8642 /* qvstfcduxa */, PPC::QVSTFCDUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8653 /* qvstfcduxi */, PPC::QVSTFCDUXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8664 /* qvstfcduxia */, PPC::QVSTFCDUXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8676 /* qvstfcdx */, PPC::QVSTFCDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8685 /* qvstfcdxa */, PPC::QVSTFCDXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8695 /* qvstfcdxi */, PPC::QVSTFCDXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8705 /* qvstfcdxia */, PPC::QVSTFCDXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8716 /* qvstfcsux */, PPC::QVSTFCSUX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8726 /* qvstfcsuxa */, PPC::QVSTFCSUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8737 /* qvstfcsuxi */, PPC::QVSTFCSUXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8748 /* qvstfcsuxia */, PPC::QVSTFCSUXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8760 /* qvstfcsx */, PPC::QVSTFCSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8769 /* qvstfcsxa */, PPC::QVSTFCSXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8779 /* qvstfcsxi */, PPC::QVSTFCSXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8789 /* qvstfcsxia */, PPC::QVSTFCSXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8800 /* qvstfdux */, PPC::QVSTFDUX, Convert__imm_95_0__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8809 /* qvstfduxa */, PPC::QVSTFDUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8819 /* qvstfduxi */, PPC::QVSTFDUXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8829 /* qvstfduxia */, PPC::QVSTFDUXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8840 /* qvstfdx */, PPC::QVSTFDX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8848 /* qvstfdxa */, PPC::QVSTFDXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8857 /* qvstfdxi */, PPC::QVSTFDXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8866 /* qvstfdxia */, PPC::QVSTFDXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8876 /* qvstfiwx */, PPC::QVSTFIWX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8885 /* qvstfiwxa */, PPC::QVSTFIWXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8895 /* qvstfsux */, PPC::QVSTFSUX, Convert__imm_95_0__RegQSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8904 /* qvstfsuxa */, PPC::QVSTFSUXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8914 /* qvstfsuxi */, PPC::QVSTFSUXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8924 /* qvstfsuxia */, PPC::QVSTFSUXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8935 /* qvstfsx */, PPC::QVSTFSX, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8943 /* qvstfsxa */, PPC::QVSTFSXA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8952 /* qvstfsxi */, PPC::QVSTFSXI, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8961 /* qvstfsxia */, PPC::QVSTFSXIA, Convert__RegQFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegQFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 8971 /* rfci */, PPC::RFCI, Convert_NoOperands, 0, {  }, },
  { 8976 /* rfdi */, PPC::RFDI, Convert_NoOperands, 0, {  }, },
  { 8981 /* rfebb */, PPC::RFEBB, Convert__U1Imm1_0, 0, { MCK_U1Imm }, },
  { 8987 /* rfi */, PPC::RFI, Convert_NoOperands, 0, {  }, },
  { 8991 /* rfid */, PPC::RFID, Convert_NoOperands, 0, {  }, },
  { 8996 /* rfmci */, PPC::RFMCI, Convert_NoOperands, 0, {  }, },
  { 9002 /* rldcl */, PPC::RLDCL, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC, MCK_U6Imm }, },
  { 9002 /* rldcl */, PPC::RLDCLo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC, MCK_U6Imm }, },
  { 9008 /* rldcr */, PPC::RLDCR, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC, MCK_U6Imm }, },
  { 9008 /* rldcr */, PPC::RLDCRo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC, MCK_U6Imm }, },
  { 9014 /* rldic */, PPC::RLDIC, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9014 /* rldic */, PPC::RLDICo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9020 /* rldicl */, PPC::RLDICL, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9020 /* rldicl */, PPC::RLDICLo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9027 /* rldicr */, PPC::RLDICR, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9027 /* rldicr */, PPC::RLDICRo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9034 /* rldimi */, PPC::RLDIMI, Convert__RegG8RC1_0__Tie0_1_1__RegG8RC1_1__U6Imm1_2__U6Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9034 /* rldimi */, PPC::RLDIMIo, Convert__RegG8RC1_1__Tie0_1_1__RegG8RC1_2__U6Imm1_3__U6Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm, MCK_U6Imm }, },
  { 9041 /* rlwimi */, PPC::RLWIMIbm, Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2__Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9041 /* rlwimi */, PPC::RLWIMIobm, Convert__RegG8RC1_1__RegG8RC1_2__U5Imm1_3__Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9041 /* rlwimi */, PPC::RLWIMI, Convert__RegGPRC1_0__Tie0_1_1__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm, MCK_U5Imm }, },
  { 9041 /* rlwimi */, PPC::RLWIMIo, Convert__RegGPRC1_1__Tie0_1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm, MCK_U5Imm }, },
  { 9048 /* rlwinm */, PPC::RLWINMbm, Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2__Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9048 /* rlwinm */, PPC::RLWINMobm, Convert__RegG8RC1_1__RegG8RC1_2__U5Imm1_3__Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9048 /* rlwinm */, PPC::RLWINM, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__U5Imm1_3__U5Imm1_4, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm, MCK_U5Imm }, },
  { 9048 /* rlwinm */, PPC::RLWINMo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4__U5Imm1_5, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm, MCK_U5Imm }, },
  { 9055 /* rlwnm */, PPC::RLWNMbm, Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2__Imm1_3, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9055 /* rlwnm */, PPC::RLWNMobm, Convert__RegG8RC1_1__RegG8RC1_2__U5Imm1_3__Imm1_4, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm, MCK_Imm }, },
  { 9055 /* rlwnm */, PPC::RLWNM, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__U5Imm1_4, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 9055 /* rlwnm */, PPC::RLWNMo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__U5Imm1_4__U5Imm1_5, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm, MCK_U5Imm }, },
  { 9061 /* rotld */, PPC::RLDCL, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2__imm_95_0, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9061 /* rotld */, PPC::RLDCLo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3__imm_95_0, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9067 /* rotldi */, PPC::RLDICL, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2__imm_95_0, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9067 /* rotldi */, PPC::RLDICLo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3__imm_95_0, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9074 /* rotlw */, PPC::RLWNM, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2__imm_95_0__imm_95_31, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9074 /* rotlw */, PPC::RLWNMo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3__imm_95_0__imm_95_31, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9080 /* rotlwi */, PPC::RLWINM, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2__imm_95_0__imm_95_31, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9080 /* rotlwi */, PPC::RLWINMo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3__imm_95_0__imm_95_31, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9087 /* rotrdi */, PPC::ROTRDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9087 /* rotrdi */, PPC::ROTRDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9094 /* rotrwi */, PPC::ROTRWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9094 /* rotrwi */, PPC::ROTRWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9101 /* sc */, PPC::SC, Convert__imm_95_0, 0, {  }, },
  { 9101 /* sc */, PPC::SC, Convert__Imm1_0, 0, { MCK_Imm }, },
  { 9104 /* setb */, PPC::SETB, Convert__RegG8RC1_0__RegCRRC1_1, 0, { MCK_RegG8RC, MCK_RegCRRC }, },
  { 9109 /* slbia */, PPC::SLBIA, Convert_NoOperands, 0, {  }, },
  { 9115 /* slbie */, PPC::SLBIE, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 9121 /* slbieg */, PPC::SLBIEG, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9128 /* slbmfee */, PPC::SLBMFEE, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9136 /* slbmfev */, PPC::SLBMFEV, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9144 /* slbmte */, PPC::SLBMTE, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9151 /* slbsync */, PPC::SLBSYNC, Convert_NoOperands, 0, {  }, },
  { 9159 /* sld */, PPC::SLD, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9159 /* sld */, PPC::SLDo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9163 /* sldi */, PPC::SLDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9163 /* sldi */, PPC::SLDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9168 /* slw */, PPC::SLW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9168 /* slw */, PPC::SLWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9172 /* slwi */, PPC::SLWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9172 /* slwi */, PPC::SLWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9177 /* srad */, PPC::SRAD, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9177 /* srad */, PPC::SRADo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9182 /* sradi */, PPC::SRADI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9182 /* sradi */, PPC::SRADIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9188 /* sraw */, PPC::SRAW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9188 /* sraw */, PPC::SRAWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9193 /* srawi */, PPC::SRAWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9193 /* srawi */, PPC::SRAWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9199 /* srd */, PPC::SRD, Convert__RegG8RC1_0__RegG8RC1_1__RegGPRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9199 /* srd */, PPC::SRDo, Convert__RegG8RC1_1__RegG8RC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegGPRC }, },
  { 9203 /* srdi */, PPC::SRDI, Convert__RegG8RC1_0__RegG8RC1_1__U6Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9203 /* srdi */, PPC::SRDIo, Convert__RegG8RC1_1__RegG8RC1_2__U6Imm1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_U6Imm }, },
  { 9208 /* srw */, PPC::SRW, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9208 /* srw */, PPC::SRWo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9212 /* srwi */, PPC::SRWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9212 /* srwi */, PPC::SRWIo, Convert__RegGPRC1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9217 /* stb */, PPC::STB, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9221 /* stbcix */, PPC::STBCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9228 /* stbcx */, PPC::STBCX, Convert__RegGPRC1_1__RegGxRCNoR01_2__RegGxRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9234 /* stbepx */, PPC::STBEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9241 /* stbu */, PPC::STBU, Convert__imm_95_0__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9246 /* stbux */, PPC::STBUX, Convert__imm_95_0__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9252 /* stbx */, PPC::STBXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 9252 /* stbx */, PPC::STBX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9257 /* std */, PPC::STD, Convert__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegG8RC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 9261 /* stdat */, PPC::STDAT, Convert__RegG8RC1_0__RegG8RC1_1__U5Imm1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_U5Imm }, },
  { 9267 /* stdbrx */, PPC::STDBRX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9274 /* stdcix */, PPC::STDCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9281 /* stdcx */, PPC::STDCX, Convert__RegG8RC1_1__RegGxRCNoR01_2__RegGxRC1_3, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9287 /* stdu */, PPC::STDU, Convert__imm_95_0__RegG8RC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegG8RC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 9292 /* stdux */, PPC::STDUX, Convert__imm_95_0__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9298 /* stdx */, PPC::STDX, Convert__RegG8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9298 /* stdx */, PPC::STDXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 9303 /* stfd */, PPC::STFD, Convert__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF8RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9308 /* stfdepx */, PPC::STFDEPX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9316 /* stfdu */, PPC::STFDU, Convert__imm_95_0__RegF8RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF8RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9322 /* stfdux */, PPC::STFDUX, Convert__imm_95_0__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9329 /* stfdx */, PPC::STFDX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9335 /* stfiwx */, PPC::STFIWX, Convert__RegF8RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF8RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9342 /* stfs */, PPC::STFS, Convert__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9347 /* stfsu */, PPC::STFSU, Convert__imm_95_0__RegF4RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegF4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9353 /* stfsux */, PPC::STFSUX, Convert__imm_95_0__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9360 /* stfsx */, PPC::STFSX, Convert__RegF4RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegF4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9366 /* sth */, PPC::STH, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9370 /* sthbrx */, PPC::STHBRX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9377 /* sthcix */, PPC::STHCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9384 /* sthcx */, PPC::STHCX, Convert__RegGPRC1_1__RegGxRCNoR01_2__RegGxRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9390 /* sthepx */, PPC::STHEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9397 /* sthu */, PPC::STHU, Convert__imm_95_0__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9402 /* sthux */, PPC::STHUX, Convert__imm_95_0__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9408 /* sthx */, PPC::STHXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 9408 /* sthx */, PPC::STHX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9413 /* stmw */, PPC::STMW, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9418 /* stop */, PPC::STOP, Convert_NoOperands, 0, {  }, },
  { 9423 /* stswi */, PPC::STSWI, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9429 /* stvebx */, PPC::STVEBX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9436 /* stvehx */, PPC::STVEHX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9443 /* stvewx */, PPC::STVEWX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9450 /* stvx */, PPC::STVX, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9455 /* stvxl */, PPC::STVXL, Convert__RegVRRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVRRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9461 /* stw */, PPC::STW, Convert__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9461 /* stw */, PPC::SPESTW, Convert__RegSPE4RC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegSPE4RC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9465 /* stwat */, PPC::STWAT, Convert__RegGPRC1_0__RegGPRC1_1__U5Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U5Imm }, },
  { 9471 /* stwbrx */, PPC::STWBRX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9478 /* stwcix */, PPC::STWCIX, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9485 /* stwcx */, PPC::STWCX, Convert__RegGPRC1_1__RegGxRCNoR01_2__RegGxRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9491 /* stwepx */, PPC::STWEPX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9498 /* stwu */, PPC::STWU, Convert__imm_95_0__RegGPRC1_0__DispRI1_1__RegGxRCNoR01_2, 0, { MCK_RegGPRC, MCK_DispRI, MCK_RegGxRCNoR0 }, },
  { 9503 /* stwux */, PPC::STWUX, Convert__imm_95_0__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9509 /* stwx */, PPC::STWXTLS_, Convert__RegG8RC1_0__RegGxRCNoR01_1__TLSReg1_2, 0, { MCK_RegG8RC, MCK_RegGxRCNoR0, MCK_TLSReg }, },
  { 9509 /* stwx */, PPC::STWX, Convert__RegGPRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegGPRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9509 /* stwx */, PPC::SPESTWX, Convert__RegSPE4RC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegSPE4RC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9514 /* stxsd */, PPC::STXSD, Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegVFRC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 9520 /* stxsdx */, PPC::STXSDX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9527 /* stxsibx */, PPC::STXSIBX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9535 /* stxsihx */, PPC::STXSIHX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9543 /* stxsiwx */, PPC::STXSIWX, Convert__RegVSFRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSFRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9551 /* stxssp */, PPC::STXSSP, Convert__RegVFRC1_0__DispRIX1_1__RegGxRCNoR01_2, 0, { MCK_RegVFRC, MCK_DispRIX, MCK_RegGxRCNoR0 }, },
  { 9558 /* stxsspx */, PPC::STXSSPX, Convert__RegVSSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9566 /* stxv */, PPC::STXV, Convert__RegVSRC1_0__DispRIX161_1__RegGxRCNoR01_2, 0, { MCK_RegVSRC, MCK_DispRIX16, MCK_RegGxRCNoR0 }, },
  { 9571 /* stxvb16x */, PPC::STXVB16X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9580 /* stxvd2x */, PPC::STXVD2X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9588 /* stxvh8x */, PPC::STXVH8X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9596 /* stxvl */, PPC::STXVL, Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_Imm, MCK_RegG8RC }, },
  { 9602 /* stxvll */, PPC::STXVLL, Convert__RegVSRC1_0__Imm1_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_Imm, MCK_RegG8RC }, },
  { 9609 /* stxvw4x */, PPC::STXVW4X, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9617 /* stxvx */, PPC::STXVX, Convert__RegVSRC1_0__RegGxRCNoR01_1__RegGxRC1_2, 0, { MCK_RegVSRC, MCK_RegGxRCNoR0, MCK_RegGxRC }, },
  { 9623 /* sub */, PPC::SUBF8, Convert__RegG8RC1_0__RegG8RC1_2__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 9623 /* sub */, PPC::SUBF8o, Convert__RegG8RC1_1__RegG8RC1_3__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 9627 /* subc */, PPC::SUBFC8, Convert__RegG8RC1_0__RegG8RC1_2__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 9627 /* subc */, PPC::SUBFC8o, Convert__RegG8RC1_1__RegG8RC1_3__RegG8RC1_2, 0, { MCK__DOT_, MCK_RegG8RC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 9632 /* subf */, PPC::SUBF, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9632 /* subf */, PPC::SUBFo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9637 /* subfc */, PPC::SUBFC, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9637 /* subfc */, PPC::SUBFCo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9643 /* subfe */, PPC::SUBFE, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9643 /* subfe */, PPC::SUBFEo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9649 /* subfic */, PPC::SUBFIC, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 9656 /* subfme */, PPC::SUBFME, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9656 /* subfme */, PPC::SUBFMEo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9663 /* subfze */, PPC::SUBFZE, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9663 /* subfze */, PPC::SUBFZEo, Convert__RegGPRC1_1__RegGPRC1_2, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9670 /* subi */, PPC::SUBI, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 9675 /* subic */, PPC::SUBIC, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 9675 /* subic */, PPC::SUBICo, Convert__RegGPRC1_1__RegGPRC1_2__S16Imm1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 9681 /* subis */, PPC::SUBIS, Convert__RegGPRC1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_S16Imm }, },
  { 9687 /* subpcis */, PPC::SUBPCIS, Convert__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9695 /* sync */, PPC::SYNC, Convert__imm_95_0, 0, {  }, },
  { 9695 /* sync */, PPC::SYNC, Convert__Imm1_0, 0, { MCK_Imm }, },
  { 9700 /* tabort */, PPC::TABORT, Convert__imm_95_0__RegGPRC1_1, 0, { MCK__DOT_, MCK_RegGPRC }, },
  { 9707 /* tabortdc */, PPC::TABORTDC, Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_U5Imm, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9716 /* tabortdci */, PPC::TABORTDCI, Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_U5Imm, MCK_RegGPRC, MCK_U5Imm }, },
  { 9726 /* tabortwc */, PPC::TABORTWC, Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_U5Imm, MCK_RegGPRC, MCK_RegGPRC }, },
  { 9735 /* tabortwci */, PPC::TABORTWCI, Convert__imm_95_0__U5Imm1_1__RegGPRC1_2__U5Imm1_3, 0, { MCK__DOT_, MCK_U5Imm, MCK_RegGPRC, MCK_U5Imm }, },
  { 9745 /* tbegin */, PPC::TBEGIN, Convert__imm_95_0__U1Imm1_1, 0, { MCK__DOT_, MCK_U1Imm }, },
  { 9752 /* tcheck */, PPC::TCHECK, Convert__RegCRRC1_0, 0, { MCK_RegCRRC }, },
  { 9759 /* td */, PPC::TD, Convert__U5Imm1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_U5Imm, MCK_RegG8RC, MCK_RegG8RC }, },
  { 9762 /* tdeq */, PPC::TD, Convert__imm_95_4__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9767 /* tdeqi */, PPC::TDI, Convert__imm_95_4__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9773 /* tdge */, PPC::TD, Convert__imm_95_12__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9778 /* tdgei */, PPC::TDI, Convert__imm_95_12__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9784 /* tdgt */, PPC::TD, Convert__imm_95_8__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9789 /* tdgti */, PPC::TDI, Convert__imm_95_8__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9795 /* tdi */, PPC::TDI, Convert__U5Imm1_0__RegG8RC1_1__S16Imm1_2, 0, { MCK_U5Imm, MCK_RegG8RC, MCK_S16Imm }, },
  { 9799 /* tdle */, PPC::TD, Convert__imm_95_20__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9804 /* tdlei */, PPC::TDI, Convert__imm_95_20__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9810 /* tdlge */, PPC::TD, Convert__imm_95_5__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9816 /* tdlgei */, PPC::TDI, Convert__imm_95_5__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9823 /* tdlgt */, PPC::TD, Convert__imm_95_1__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9829 /* tdlgti */, PPC::TDI, Convert__imm_95_1__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9836 /* tdlle */, PPC::TD, Convert__imm_95_6__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9842 /* tdllei */, PPC::TDI, Convert__imm_95_6__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9849 /* tdllt */, PPC::TD, Convert__imm_95_2__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9855 /* tdllti */, PPC::TDI, Convert__imm_95_2__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9862 /* tdlng */, PPC::TD, Convert__imm_95_6__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9868 /* tdlngi */, PPC::TDI, Convert__imm_95_6__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9875 /* tdlnl */, PPC::TD, Convert__imm_95_5__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9881 /* tdlnli */, PPC::TDI, Convert__imm_95_5__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9888 /* tdlt */, PPC::TD, Convert__imm_95_16__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9893 /* tdlti */, PPC::TDI, Convert__imm_95_16__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9899 /* tdne */, PPC::TD, Convert__imm_95_24__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9904 /* tdnei */, PPC::TDI, Convert__imm_95_24__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9910 /* tdng */, PPC::TD, Convert__imm_95_20__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9915 /* tdngi */, PPC::TDI, Convert__imm_95_20__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9921 /* tdnl */, PPC::TD, Convert__imm_95_12__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9926 /* tdnli */, PPC::TDI, Convert__imm_95_12__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9932 /* tdu */, PPC::TD, Convert__imm_95_31__RegG8RC1_0__RegG8RC1_1, 0, { MCK_RegG8RC, MCK_RegG8RC }, },
  { 9936 /* tdui */, PPC::TDI, Convert__imm_95_31__RegG8RC1_0__S16Imm1_1, 0, { MCK_RegG8RC, MCK_S16Imm }, },
  { 9941 /* tend */, PPC::TEND, Convert__imm_95_0__U1Imm1_1, 0, { MCK__DOT_, MCK_U1Imm }, },
  { 9946 /* tlbia */, PPC::TLBIA, Convert_NoOperands, 0, {  }, },
  { 9952 /* tlbie */, PPC::TLBIE, Convert__regR0__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 9952 /* tlbie */, PPC::TLBIE, Convert__RegGPRC1_1__RegGPRC1_0, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9958 /* tlbiel */, PPC::TLBIEL, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 9965 /* tlbivax */, PPC::TLBIVAX, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9973 /* tlbld */, PPC::TLBLD, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 9979 /* tlbli */, PPC::TLBLI, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 9985 /* tlbre */, PPC::TLBRE, Convert_NoOperands, 0, {  }, },
  { 9985 /* tlbre */, PPC::TLBRE2, Convert__RegGPRC1_0__RegGPRC1_1__Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_Imm }, },
  { 9991 /* tlbrehi */, PPC::TLBRE2, Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 9999 /* tlbrelo */, PPC::TLBRE2, Convert__RegGPRC1_0__RegGPRC1_1__imm_95_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10007 /* tlbsx */, PPC::TLBSX, Convert__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10007 /* tlbsx */, PPC::TLBSX2, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 10007 /* tlbsx */, PPC::TLBSX2D, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 10013 /* tlbsync */, PPC::TLBSYNC, Convert_NoOperands, 0, {  }, },
  { 10021 /* tlbwe */, PPC::TLBWE, Convert_NoOperands, 0, {  }, },
  { 10021 /* tlbwe */, PPC::TLBWE2, Convert__RegGPRC1_0__RegGPRC1_1__Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_Imm }, },
  { 10027 /* tlbwehi */, PPC::TLBWE2, Convert__RegGPRC1_0__RegGPRC1_1__imm_95_0, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10035 /* tlbwelo */, PPC::TLBWE2, Convert__RegGPRC1_0__RegGPRC1_1__imm_95_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10043 /* trap */, PPC::TRAP, Convert_NoOperands, 0, {  }, },
  { 10048 /* trechkpt */, PPC::TRECHKPT, Convert__imm_95_0, 0, { MCK__DOT_ }, },
  { 10057 /* treclaim */, PPC::TRECLAIM, Convert__imm_95_0__RegGPRC1_1, 0, { MCK__DOT_, MCK_RegGPRC }, },
  { 10066 /* tsr */, PPC::TSR, Convert__imm_95_0__U1Imm1_1, 0, { MCK__DOT_, MCK_U1Imm }, },
  { 10070 /* tw */, PPC::TW, Convert__U5Imm1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_U5Imm, MCK_RegGPRC, MCK_RegGPRC }, },
  { 10073 /* tweq */, PPC::TW, Convert__imm_95_4__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10078 /* tweqi */, PPC::TWI, Convert__imm_95_4__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10084 /* twge */, PPC::TW, Convert__imm_95_12__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10089 /* twgei */, PPC::TWI, Convert__imm_95_12__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10095 /* twgt */, PPC::TW, Convert__imm_95_8__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10100 /* twgti */, PPC::TWI, Convert__imm_95_8__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10106 /* twi */, PPC::TWI, Convert__U5Imm1_0__RegGPRC1_1__S16Imm1_2, 0, { MCK_U5Imm, MCK_RegGPRC, MCK_S16Imm }, },
  { 10110 /* twle */, PPC::TW, Convert__imm_95_20__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10115 /* twlei */, PPC::TWI, Convert__imm_95_20__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10121 /* twlge */, PPC::TW, Convert__imm_95_5__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10127 /* twlgei */, PPC::TWI, Convert__imm_95_5__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10134 /* twlgt */, PPC::TW, Convert__imm_95_1__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10140 /* twlgti */, PPC::TWI, Convert__imm_95_1__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10147 /* twlle */, PPC::TW, Convert__imm_95_6__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10153 /* twllei */, PPC::TWI, Convert__imm_95_6__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10160 /* twllt */, PPC::TW, Convert__imm_95_2__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10166 /* twllti */, PPC::TWI, Convert__imm_95_2__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10173 /* twlng */, PPC::TW, Convert__imm_95_6__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10179 /* twlngi */, PPC::TWI, Convert__imm_95_6__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10186 /* twlnl */, PPC::TW, Convert__imm_95_5__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10192 /* twlnli */, PPC::TWI, Convert__imm_95_5__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10199 /* twlt */, PPC::TW, Convert__imm_95_16__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10204 /* twlti */, PPC::TWI, Convert__imm_95_16__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10210 /* twne */, PPC::TW, Convert__imm_95_24__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10215 /* twnei */, PPC::TWI, Convert__imm_95_24__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10221 /* twng */, PPC::TW, Convert__imm_95_20__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10226 /* twngi */, PPC::TWI, Convert__imm_95_20__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10232 /* twnl */, PPC::TW, Convert__imm_95_12__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10237 /* twnli */, PPC::TWI, Convert__imm_95_12__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10243 /* twu */, PPC::TW, Convert__imm_95_31__RegGPRC1_0__RegGPRC1_1, 0, { MCK_RegGPRC, MCK_RegGPRC }, },
  { 10247 /* twui */, PPC::TWI, Convert__imm_95_31__RegGPRC1_0__S16Imm1_1, 0, { MCK_RegGPRC, MCK_S16Imm }, },
  { 10252 /* vabsdub */, PPC::VABSDUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10260 /* vabsduh */, PPC::VABSDUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10268 /* vabsduw */, PPC::VABSDUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10276 /* vaddcuq */, PPC::VADDCUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10284 /* vaddcuw */, PPC::VADDCUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10292 /* vaddecuq */, PPC::VADDECUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10301 /* vaddeuqm */, PPC::VADDEUQM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10310 /* vaddfp */, PPC::VADDFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10317 /* vaddsbs */, PPC::VADDSBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10325 /* vaddshs */, PPC::VADDSHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10333 /* vaddsws */, PPC::VADDSWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10341 /* vaddubm */, PPC::VADDUBM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10349 /* vaddubs */, PPC::VADDUBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10357 /* vaddudm */, PPC::VADDUDM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10365 /* vadduhm */, PPC::VADDUHM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10373 /* vadduhs */, PPC::VADDUHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10381 /* vadduqm */, PPC::VADDUQM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10389 /* vadduwm */, PPC::VADDUWM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10397 /* vadduws */, PPC::VADDUWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10405 /* vand */, PPC::VAND, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10410 /* vandc */, PPC::VANDC, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10416 /* vavgsb */, PPC::VAVGSB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10423 /* vavgsh */, PPC::VAVGSH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10430 /* vavgsw */, PPC::VAVGSW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10437 /* vavgub */, PPC::VAVGUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10444 /* vavguh */, PPC::VAVGUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10451 /* vavguw */, PPC::VAVGUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10458 /* vbpermd */, PPC::VBPERMD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10466 /* vbpermq */, PPC::VBPERMQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10474 /* vcfsx */, PPC::VCFSX, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 10480 /* vcfux */, PPC::VCFUX, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 10486 /* vcipher */, PPC::VCIPHER, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10494 /* vcipherlast */, PPC::VCIPHERLAST, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10506 /* vclzb */, PPC::VCLZB, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10512 /* vclzd */, PPC::VCLZD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10518 /* vclzh */, PPC::VCLZH, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10524 /* vclzlsbb */, PPC::VCLZLSBB, Convert__RegGPRC1_0__RegVRRC1_1, 0, { MCK_RegGPRC, MCK_RegVRRC }, },
  { 10533 /* vclzw */, PPC::VCLZW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10539 /* vcmpbfp */, PPC::VCMPBFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10539 /* vcmpbfp */, PPC::VCMPBFPo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10547 /* vcmpeqfp */, PPC::VCMPEQFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10547 /* vcmpeqfp */, PPC::VCMPEQFPo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10556 /* vcmpequb */, PPC::VCMPEQUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10556 /* vcmpequb */, PPC::VCMPEQUBo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10565 /* vcmpequd */, PPC::VCMPEQUD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10565 /* vcmpequd */, PPC::VCMPEQUDo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10574 /* vcmpequh */, PPC::VCMPEQUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10574 /* vcmpequh */, PPC::VCMPEQUHo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10583 /* vcmpequw */, PPC::VCMPEQUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10583 /* vcmpequw */, PPC::VCMPEQUWo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10592 /* vcmpgefp */, PPC::VCMPGEFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10592 /* vcmpgefp */, PPC::VCMPGEFPo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10601 /* vcmpgtfp */, PPC::VCMPGTFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10601 /* vcmpgtfp */, PPC::VCMPGTFPo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10610 /* vcmpgtsb */, PPC::VCMPGTSB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10610 /* vcmpgtsb */, PPC::VCMPGTSBo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10619 /* vcmpgtsd */, PPC::VCMPGTSD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10619 /* vcmpgtsd */, PPC::VCMPGTSDo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10628 /* vcmpgtsh */, PPC::VCMPGTSH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10628 /* vcmpgtsh */, PPC::VCMPGTSHo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10637 /* vcmpgtsw */, PPC::VCMPGTSW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10637 /* vcmpgtsw */, PPC::VCMPGTSWo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10646 /* vcmpgtub */, PPC::VCMPGTUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10646 /* vcmpgtub */, PPC::VCMPGTUBo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10655 /* vcmpgtud */, PPC::VCMPGTUD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10655 /* vcmpgtud */, PPC::VCMPGTUDo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10664 /* vcmpgtuh */, PPC::VCMPGTUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10664 /* vcmpgtuh */, PPC::VCMPGTUHo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10673 /* vcmpgtuw */, PPC::VCMPGTUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10673 /* vcmpgtuw */, PPC::VCMPGTUWo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10682 /* vcmpneb */, PPC::VCMPNEB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10682 /* vcmpneb */, PPC::VCMPNEBo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10690 /* vcmpneh */, PPC::VCMPNEH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10690 /* vcmpneh */, PPC::VCMPNEHo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10698 /* vcmpnew */, PPC::VCMPNEW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10698 /* vcmpnew */, PPC::VCMPNEWo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10706 /* vcmpnezb */, PPC::VCMPNEZB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10706 /* vcmpnezb */, PPC::VCMPNEZBo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10715 /* vcmpnezh */, PPC::VCMPNEZH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10715 /* vcmpnezh */, PPC::VCMPNEZHo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10724 /* vcmpnezw */, PPC::VCMPNEZW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10724 /* vcmpnezw */, PPC::VCMPNEZWo, Convert__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK__DOT_, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10733 /* vctsxs */, PPC::VCTSXS, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 10740 /* vctuxs */, PPC::VCTUXS, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 10747 /* vctzb */, PPC::VCTZB, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10753 /* vctzd */, PPC::VCTZD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10759 /* vctzh */, PPC::VCTZH, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10765 /* vctzlsbb */, PPC::VCTZLSBB, Convert__RegGPRC1_0__RegVRRC1_1, 0, { MCK_RegGPRC, MCK_RegVRRC }, },
  { 10774 /* vctzw */, PPC::VCTZW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10780 /* veqv */, PPC::VEQV, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10785 /* vexptefp */, PPC::VEXPTEFP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10794 /* vextractd */, PPC::VEXTRACTD, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10804 /* vextractub */, PPC::VEXTRACTUB, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10815 /* vextractuh */, PPC::VEXTRACTUH, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10826 /* vextractuw */, PPC::VEXTRACTUW, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10837 /* vextsb2d */, PPC::VEXTSB2D, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10846 /* vextsb2w */, PPC::VEXTSB2W, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10855 /* vextsh2d */, PPC::VEXTSH2D, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10864 /* vextsh2w */, PPC::VEXTSH2W, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10873 /* vextsw2d */, PPC::VEXTSW2D, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10882 /* vextublx */, PPC::VEXTUBLX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10891 /* vextubrx */, PPC::VEXTUBRX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10900 /* vextuhlx */, PPC::VEXTUHLX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10909 /* vextuhrx */, PPC::VEXTUHRX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10918 /* vextuwlx */, PPC::VEXTUWLX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10927 /* vextuwrx */, PPC::VEXTUWRX, Convert__RegG8RC1_0__RegG8RC1_1__RegVRRC1_2, 0, { MCK_RegG8RC, MCK_RegG8RC, MCK_RegVRRC }, },
  { 10936 /* vgbbd */, PPC::VGBBD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10942 /* vinsertb */, PPC::VINSERTB, Convert__RegVRRC1_0__Tie0_1_1__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10951 /* vinsertd */, PPC::VINSERTD, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10960 /* vinserth */, PPC::VINSERTH, Convert__RegVRRC1_0__Tie0_1_1__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10969 /* vinsertw */, PPC::VINSERTW, Convert__RegVRRC1_0__U4Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 10978 /* vlogefp */, PPC::VLOGEFP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 10986 /* vmaddfp */, PPC::VMADDFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 10994 /* vmaxfp */, PPC::VMAXFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11001 /* vmaxsb */, PPC::VMAXSB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11008 /* vmaxsd */, PPC::VMAXSD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11015 /* vmaxsh */, PPC::VMAXSH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11022 /* vmaxsw */, PPC::VMAXSW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11029 /* vmaxub */, PPC::VMAXUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11036 /* vmaxud */, PPC::VMAXUD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11043 /* vmaxuh */, PPC::VMAXUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11050 /* vmaxuw */, PPC::VMAXUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11057 /* vmhaddshs */, PPC::VMHADDSHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11067 /* vmhraddshs */, PPC::VMHRADDSHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11078 /* vminfp */, PPC::VMINFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11085 /* vminsb */, PPC::VMINSB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11092 /* vminsd */, PPC::VMINSD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11099 /* vminsh */, PPC::VMINSH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11106 /* vminsw */, PPC::VMINSW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11113 /* vminub */, PPC::VMINUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11120 /* vminud */, PPC::VMINUD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11127 /* vminuh */, PPC::VMINUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11134 /* vminuw */, PPC::VMINUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11141 /* vmladduhm */, PPC::VMLADDUHM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11151 /* vmr */, PPC::VOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11155 /* vmrgew */, PPC::VMRGEW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11162 /* vmrghb */, PPC::VMRGHB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11169 /* vmrghh */, PPC::VMRGHH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11176 /* vmrghw */, PPC::VMRGHW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11183 /* vmrglb */, PPC::VMRGLB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11190 /* vmrglh */, PPC::VMRGLH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11197 /* vmrglw */, PPC::VMRGLW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11204 /* vmrgow */, PPC::VMRGOW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11211 /* vmsummbm */, PPC::VMSUMMBM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11220 /* vmsumshm */, PPC::VMSUMSHM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11229 /* vmsumshs */, PPC::VMSUMSHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11238 /* vmsumubm */, PPC::VMSUMUBM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11247 /* vmsumuhm */, PPC::VMSUMUHM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11256 /* vmsumuhs */, PPC::VMSUMUHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11265 /* vmul10cuq */, PPC::VMUL10CUQ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11275 /* vmul10ecuq */, PPC::VMUL10ECUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11286 /* vmul10euq */, PPC::VMUL10EUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11296 /* vmul10uq */, PPC::VMUL10UQ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11305 /* vmulesb */, PPC::VMULESB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11313 /* vmulesh */, PPC::VMULESH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11321 /* vmulesw */, PPC::VMULESW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11329 /* vmuleub */, PPC::VMULEUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11337 /* vmuleuh */, PPC::VMULEUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11345 /* vmuleuw */, PPC::VMULEUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11353 /* vmulosb */, PPC::VMULOSB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11361 /* vmulosh */, PPC::VMULOSH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11369 /* vmulosw */, PPC::VMULOSW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11377 /* vmuloub */, PPC::VMULOUB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11385 /* vmulouh */, PPC::VMULOUH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11393 /* vmulouw */, PPC::VMULOUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11401 /* vmuluwm */, PPC::VMULUWM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11409 /* vnand */, PPC::VNAND, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11415 /* vncipher */, PPC::VNCIPHER, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11424 /* vncipherlast */, PPC::VNCIPHERLAST, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11437 /* vnegd */, PPC::VNEGD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11443 /* vnegw */, PPC::VNEGW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11449 /* vnmsubfp */, PPC::VNMSUBFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11458 /* vnor */, PPC::VNOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11463 /* vnot */, PPC::VNOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11468 /* vor */, PPC::VOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11472 /* vorc */, PPC::VORC, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11477 /* vperm */, PPC::VPERM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11483 /* vpermr */, PPC::VPERMR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11490 /* vpermxor */, PPC::VPERMXOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11499 /* vpkpx */, PPC::VPKPX, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11505 /* vpksdss */, PPC::VPKSDSS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11513 /* vpksdus */, PPC::VPKSDUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11521 /* vpkshss */, PPC::VPKSHSS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11529 /* vpkshus */, PPC::VPKSHUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11537 /* vpkswss */, PPC::VPKSWSS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11545 /* vpkswus */, PPC::VPKSWUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11553 /* vpkudum */, PPC::VPKUDUM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11561 /* vpkudus */, PPC::VPKUDUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11569 /* vpkuhum */, PPC::VPKUHUM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11577 /* vpkuhus */, PPC::VPKUHUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11585 /* vpkuwum */, PPC::VPKUWUM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11593 /* vpkuwus */, PPC::VPKUWUS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11601 /* vpmsumb */, PPC::VPMSUMB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11609 /* vpmsumd */, PPC::VPMSUMD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11617 /* vpmsumh */, PPC::VPMSUMH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11625 /* vpmsumw */, PPC::VPMSUMW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11633 /* vpopcntb */, PPC::VPOPCNTB, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11642 /* vpopcntd */, PPC::VPOPCNTD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11651 /* vpopcnth */, PPC::VPOPCNTH, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11660 /* vpopcntw */, PPC::VPOPCNTW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11669 /* vprtybd */, PPC::VPRTYBD, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11677 /* vprtybq */, PPC::VPRTYBQ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11685 /* vprtybw */, PPC::VPRTYBW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11693 /* vrefp */, PPC::VREFP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11699 /* vrfim */, PPC::VRFIM, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11705 /* vrfin */, PPC::VRFIN, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11711 /* vrfip */, PPC::VRFIP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11717 /* vrfiz */, PPC::VRFIZ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11723 /* vrlb */, PPC::VRLB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11728 /* vrld */, PPC::VRLD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11733 /* vrldmi */, PPC::VRLDMI, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__Tie0_1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11740 /* vrldnm */, PPC::VRLDNM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11747 /* vrlh */, PPC::VRLH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11752 /* vrlw */, PPC::VRLW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11757 /* vrlwmi */, PPC::VRLWMI, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__Tie0_1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11764 /* vrlwnm */, PPC::VRLWNM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11771 /* vrsqrtefp */, PPC::VRSQRTEFP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11781 /* vsbox */, PPC::VSBOX, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 11787 /* vsel */, PPC::VSEL, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11792 /* vshasigmad */, PPC::VSHASIGMAD, Convert__RegVRRC1_0__RegVRRC1_1__U1Imm1_2__U4Imm1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm, MCK_U4Imm }, },
  { 11803 /* vshasigmaw */, PPC::VSHASIGMAW, Convert__RegVRRC1_0__RegVRRC1_1__U1Imm1_2__U4Imm1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U1Imm, MCK_U4Imm }, },
  { 11814 /* vsl */, PPC::VSL, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11818 /* vslb */, PPC::VSLB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11823 /* vsld */, PPC::VSLD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11828 /* vsldoi */, PPC::VSLDOI, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__U4Imm1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_U4Imm }, },
  { 11835 /* vslh */, PPC::VSLH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11840 /* vslo */, PPC::VSLO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11845 /* vslv */, PPC::VSLV, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11850 /* vslw */, PPC::VSLW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11855 /* vspltb */, PPC::VSPLTB, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 11862 /* vsplth */, PPC::VSPLTH, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 11869 /* vspltisb */, PPC::VSPLTISB, Convert__RegVRRC1_0__S5Imm1_1, 0, { MCK_RegVRRC, MCK_S5Imm }, },
  { 11878 /* vspltish */, PPC::VSPLTISH, Convert__RegVRRC1_0__S5Imm1_1, 0, { MCK_RegVRRC, MCK_S5Imm }, },
  { 11887 /* vspltisw */, PPC::VSPLTISW, Convert__RegVRRC1_0__S5Imm1_1, 0, { MCK_RegVRRC, MCK_S5Imm }, },
  { 11896 /* vspltw */, PPC::VSPLTW, Convert__RegVRRC1_0__U5Imm1_2__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_U5Imm }, },
  { 11903 /* vsr */, PPC::VSR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11907 /* vsrab */, PPC::VSRAB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11913 /* vsrad */, PPC::VSRAD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11919 /* vsrah */, PPC::VSRAH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11925 /* vsraw */, PPC::VSRAW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11931 /* vsrb */, PPC::VSRB, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11936 /* vsrd */, PPC::VSRD, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11941 /* vsrh */, PPC::VSRH, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11946 /* vsro */, PPC::VSRO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11951 /* vsrv */, PPC::VSRV, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11956 /* vsrw */, PPC::VSRW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11961 /* vsubcuq */, PPC::VSUBCUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11969 /* vsubcuw */, PPC::VSUBCUW, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11977 /* vsubecuq */, PPC::VSUBECUQ, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11986 /* vsubeuqm */, PPC::VSUBEUQM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2__RegVRRC1_3, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 11995 /* vsubfp */, PPC::VSUBFP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12002 /* vsubsbs */, PPC::VSUBSBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12010 /* vsubshs */, PPC::VSUBSHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12018 /* vsubsws */, PPC::VSUBSWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12026 /* vsububm */, PPC::VSUBUBM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12034 /* vsububs */, PPC::VSUBUBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12042 /* vsubudm */, PPC::VSUBUDM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12050 /* vsubuhm */, PPC::VSUBUHM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12058 /* vsubuhs */, PPC::VSUBUHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12066 /* vsubuqm */, PPC::VSUBUQM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12074 /* vsubuwm */, PPC::VSUBUWM, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12082 /* vsubuws */, PPC::VSUBUWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12090 /* vsum2sws */, PPC::VSUM2SWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12099 /* vsum4sbs */, PPC::VSUM4SBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12108 /* vsum4shs */, PPC::VSUM4SHS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12117 /* vsum4ubs */, PPC::VSUM4UBS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12126 /* vsumsws */, PPC::VSUMSWS, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12134 /* vupkhpx */, PPC::VUPKHPX, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12142 /* vupkhsb */, PPC::VUPKHSB, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12150 /* vupkhsh */, PPC::VUPKHSH, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12158 /* vupkhsw */, PPC::VUPKHSW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12166 /* vupklpx */, PPC::VUPKLPX, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12174 /* vupklsb */, PPC::VUPKLSB, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12182 /* vupklsh */, PPC::VUPKLSH, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12190 /* vupklsw */, PPC::VUPKLSW, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12198 /* vxor */, PPC::VXOR, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12203 /* wait */, PPC::WAIT, Convert__imm_95_0, 0, {  }, },
  { 12203 /* wait */, PPC::WAIT, Convert__Imm1_0, 0, { MCK_Imm }, },
  { 12208 /* waitimpl */, PPC::WAIT, Convert__imm_95_2, 0, {  }, },
  { 12217 /* waitrsv */, PPC::WAIT, Convert__imm_95_1, 0, {  }, },
  { 12225 /* wrtee */, PPC::WRTEE, Convert__RegGPRC1_0, 0, { MCK_RegGPRC }, },
  { 12231 /* wrteei */, PPC::WRTEEI, Convert__Imm1_0, 0, { MCK_Imm }, },
  { 12238 /* xnop */, PPC::XORI, Convert__regR0__regR0__imm_95_0, 0, {  }, },
  { 12243 /* xor */, PPC::XOR, Convert__RegGPRC1_0__RegGPRC1_1__RegGPRC1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 12243 /* xor */, PPC::XORo, Convert__RegGPRC1_1__RegGPRC1_2__RegGPRC1_3, 0, { MCK__DOT_, MCK_RegGPRC, MCK_RegGPRC, MCK_RegGPRC }, },
  { 12247 /* xori */, PPC::XORI, Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 12252 /* xoris */, PPC::XORIS, Convert__RegGPRC1_0__RegGPRC1_1__U16Imm1_2, 0, { MCK_RegGPRC, MCK_RegGPRC, MCK_U16Imm }, },
  { 12258 /* xsabsdp */, PPC::XSABSDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12266 /* xsabsqp */, PPC::XSABSQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12274 /* xsadddp */, PPC::XSADDDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12282 /* xsaddqp */, PPC::XSADDQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12290 /* xsaddqpo */, PPC::XSADDQPO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12299 /* xsaddsp */, PPC::XSADDSP, Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12307 /* xscmpeqdp */, PPC::XSCMPEQDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12317 /* xscmpexpdp */, PPC::XSCMPEXPDP, Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12328 /* xscmpexpqp */, PPC::XSCMPEXPQP, Convert__RegCRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegCRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12339 /* xscmpgedp */, PPC::XSCMPGEDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12349 /* xscmpgtdp */, PPC::XSCMPGTDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12359 /* xscmpodp */, PPC::XSCMPODP, Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12368 /* xscmpoqp */, PPC::XSCMPOQP, Convert__RegCRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegCRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12377 /* xscmpudp */, PPC::XSCMPUDP, Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12386 /* xscmpuqp */, PPC::XSCMPUQP, Convert__RegCRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegCRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12395 /* xscpsgndp */, PPC::XSCPSGNDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12405 /* xscpsgnqp */, PPC::XSCPSGNQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12415 /* xscvdphp */, PPC::XSCVDPHP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12424 /* xscvdpqp */, PPC::XSCVDPQP, Convert__RegVRRC1_0__RegVFRC1_1, 0, { MCK_RegVRRC, MCK_RegVFRC }, },
  { 12433 /* xscvdpsp */, PPC::XSCVDPSP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12442 /* xscvdpspn */, PPC::XSCVDPSPN, Convert__RegVSRC1_0__RegVSSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSSRC }, },
  { 12452 /* xscvdpsxds */, PPC::XSCVDPSXDS, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12463 /* xscvdpsxws */, PPC::XSCVDPSXWS, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12474 /* xscvdpuxds */, PPC::XSCVDPUXDS, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12485 /* xscvdpuxws */, PPC::XSCVDPUXWS, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12496 /* xscvhpdp */, PPC::XSCVHPDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12505 /* xscvqpdp */, PPC::XSCVQPDP, Convert__RegVFRC1_0__RegVRRC1_1, 0, { MCK_RegVFRC, MCK_RegVRRC }, },
  { 12514 /* xscvqpdpo */, PPC::XSCVQPDPO, Convert__RegVFRC1_0__RegVRRC1_1, 0, { MCK_RegVFRC, MCK_RegVRRC }, },
  { 12524 /* xscvqpsdz */, PPC::XSCVQPSDZ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12534 /* xscvqpswz */, PPC::XSCVQPSWZ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12544 /* xscvqpudz */, PPC::XSCVQPUDZ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12554 /* xscvqpuwz */, PPC::XSCVQPUWZ, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12564 /* xscvsdqp */, PPC::XSCVSDQP, Convert__RegVRRC1_0__RegVFRC1_1, 0, { MCK_RegVRRC, MCK_RegVFRC }, },
  { 12573 /* xscvspdp */, PPC::XSCVSPDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12582 /* xscvspdpn */, PPC::XSCVSPDPN, Convert__RegVSSRC1_0__RegVSRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSRC }, },
  { 12592 /* xscvsxddp */, PPC::XSCVSXDDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12602 /* xscvsxdsp */, PPC::XSCVSXDSP, Convert__RegVSSRC1_0__RegVSFRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSFRC }, },
  { 12612 /* xscvudqp */, PPC::XSCVUDQP, Convert__RegVRRC1_0__RegVFRC1_1, 0, { MCK_RegVRRC, MCK_RegVFRC }, },
  { 12621 /* xscvuxddp */, PPC::XSCVUXDDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12631 /* xscvuxdsp */, PPC::XSCVUXDSP, Convert__RegVSSRC1_0__RegVSFRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSFRC }, },
  { 12641 /* xsdivdp */, PPC::XSDIVDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12649 /* xsdivqp */, PPC::XSDIVQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12657 /* xsdivqpo */, PPC::XSDIVQPO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12666 /* xsdivsp */, PPC::XSDIVSP, Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12674 /* xsiexpdp */, PPC::XSIEXPDP, Convert__RegVSRC1_0__RegG8RC1_1__RegG8RC1_2, 0, { MCK_RegVSRC, MCK_RegG8RC, MCK_RegG8RC }, },
  { 12683 /* xsiexpqp */, PPC::XSIEXPQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVSFRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVSFRC }, },
  { 12692 /* xsmaddadp */, PPC::XSMADDADP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12702 /* xsmaddasp */, PPC::XSMADDASP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12712 /* xsmaddmdp */, PPC::XSMADDMDP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12722 /* xsmaddmsp */, PPC::XSMADDMSP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12732 /* xsmaddqp */, PPC::XSMADDQP, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12741 /* xsmaddqpo */, PPC::XSMADDQPO, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12751 /* xsmaxcdp */, PPC::XSMAXCDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12760 /* xsmaxdp */, PPC::XSMAXDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12768 /* xsmaxjdp */, PPC::XSMAXJDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12777 /* xsmincdp */, PPC::XSMINCDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12786 /* xsmindp */, PPC::XSMINDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12794 /* xsminjdp */, PPC::XSMINJDP, Convert__RegVSRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12803 /* xsmsubadp */, PPC::XSMSUBADP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12813 /* xsmsubasp */, PPC::XSMSUBASP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12823 /* xsmsubmdp */, PPC::XSMSUBMDP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12833 /* xsmsubmsp */, PPC::XSMSUBMSP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12843 /* xsmsubqp */, PPC::XSMSUBQP, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12852 /* xsmsubqpo */, PPC::XSMSUBQPO, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12862 /* xsmuldp */, PPC::XSMULDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12870 /* xsmulqp */, PPC::XSMULQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12878 /* xsmulqpo */, PPC::XSMULQPO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12887 /* xsmulsp */, PPC::XSMULSP, Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12895 /* xsnabsdp */, PPC::XSNABSDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12904 /* xsnabsqp */, PPC::XSNABSQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12913 /* xsnegdp */, PPC::XSNEGDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12921 /* xsnegqp */, PPC::XSNEGQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 12929 /* xsnmaddadp */, PPC::XSNMADDADP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12940 /* xsnmaddasp */, PPC::XSNMADDASP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12951 /* xsnmaddmdp */, PPC::XSNMADDMDP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 12962 /* xsnmaddmsp */, PPC::XSNMADDMSP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 12973 /* xsnmaddqp */, PPC::XSNMADDQP, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12983 /* xsnmaddqpo */, PPC::XSNMADDQPO, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 12994 /* xsnmsubadp */, PPC::XSNMSUBADP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13005 /* xsnmsubasp */, PPC::XSNMSUBASP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13016 /* xsnmsubmdp */, PPC::XSNMSUBMDP, Convert__RegVSFRC1_0__Tie0_1_1__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13027 /* xsnmsubmsp */, PPC::XSNMSUBMSP, Convert__RegVSSRC1_0__Tie0_1_1__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13038 /* xsnmsubqp */, PPC::XSNMSUBQP, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 13048 /* xsnmsubqpo */, PPC::XSNMSUBQPO, Convert__RegVRRC1_0__Tie0_1_1__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 13059 /* xsrdpi */, PPC::XSRDPI, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13066 /* xsrdpic */, PPC::XSRDPIC, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13074 /* xsrdpim */, PPC::XSRDPIM, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13082 /* xsrdpip */, PPC::XSRDPIP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13090 /* xsrdpiz */, PPC::XSRDPIZ, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13098 /* xsredp */, PPC::XSREDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13105 /* xsresp */, PPC::XSRESP, Convert__RegVSSRC1_0__RegVSSRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13112 /* xsrqpi */, PPC::XSRQPI, Convert__RegVRRC1_1__U1Imm1_0__RegVRRC1_2__U2Imm1_3, 0, { MCK_U1Imm, MCK_RegVRRC, MCK_RegVRRC, MCK_U2Imm }, },
  { 13119 /* xsrqpix */, PPC::XSRQPIX, Convert__RegVRRC1_1__U1Imm1_0__RegVRRC1_2__U2Imm1_3, 0, { MCK_U1Imm, MCK_RegVRRC, MCK_RegVRRC, MCK_U2Imm }, },
  { 13127 /* xsrqpxp */, PPC::XSRQPXP, Convert__RegVRRC1_1__U1Imm1_0__RegVRRC1_2__U2Imm1_3, 0, { MCK_U1Imm, MCK_RegVRRC, MCK_RegVRRC, MCK_U2Imm }, },
  { 13135 /* xsrsp */, PPC::XSRSP, Convert__RegVSSRC1_0__RegVSFRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSFRC }, },
  { 13141 /* xsrsqrtedp */, PPC::XSRSQRTEDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13152 /* xsrsqrtesp */, PPC::XSRSQRTESP, Convert__RegVSSRC1_0__RegVSSRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13163 /* xssqrtdp */, PPC::XSSQRTDP, Convert__RegVSFRC1_0__RegVSFRC1_1, 0, { MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13172 /* xssqrtqp */, PPC::XSSQRTQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 13181 /* xssqrtqpo */, PPC::XSSQRTQPO, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 13191 /* xssqrtsp */, PPC::XSSQRTSP, Convert__RegVSSRC1_0__RegVSSRC1_1, 0, { MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13200 /* xssubdp */, PPC::XSSUBDP, Convert__RegVSFRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegVSFRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13208 /* xssubqp */, PPC::XSSUBQP, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 13216 /* xssubqpo */, PPC::XSSUBQPO, Convert__RegVRRC1_0__RegVRRC1_1__RegVRRC1_2, 0, { MCK_RegVRRC, MCK_RegVRRC, MCK_RegVRRC }, },
  { 13225 /* xssubsp */, PPC::XSSUBSP, Convert__RegVSSRC1_0__RegVSSRC1_1__RegVSSRC1_2, 0, { MCK_RegVSSRC, MCK_RegVSSRC, MCK_RegVSSRC }, },
  { 13233 /* xstdivdp */, PPC::XSTDIVDP, Convert__RegCRRC1_0__RegVSFRC1_1__RegVSFRC1_2, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_RegVSFRC }, },
  { 13242 /* xstsqrtdp */, PPC::XSTSQRTDP, Convert__RegCRRC1_0__RegVSFRC1_1, 0, { MCK_RegCRRC, MCK_RegVSFRC }, },
  { 13252 /* xststdcdp */, PPC::XSTSTDCDP, Convert__RegCRRC1_0__U7Imm1_2__RegVSFRC1_1, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_U7Imm }, },
  { 13262 /* xststdcqp */, PPC::XSTSTDCQP, Convert__RegCRRC1_0__U7Imm1_2__RegVRRC1_1, 0, { MCK_RegCRRC, MCK_RegVRRC, MCK_U7Imm }, },
  { 13272 /* xststdcsp */, PPC::XSTSTDCSP, Convert__RegCRRC1_0__U7Imm1_2__RegVSFRC1_1, 0, { MCK_RegCRRC, MCK_RegVSFRC, MCK_U7Imm }, },
  { 13282 /* xsxexpdp */, PPC::XSXEXPDP, Convert__RegG8RC1_0__RegVSFRC1_1, 0, { MCK_RegG8RC, MCK_RegVSFRC }, },
  { 13291 /* xsxexpqp */, PPC::XSXEXPQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 13300 /* xsxsigdp */, PPC::XSXSIGDP, Convert__RegG8RC1_0__RegVSFRC1_1, 0, { MCK_RegG8RC, MCK_RegVSFRC }, },
  { 13309 /* xsxsigqp */, PPC::XSXSIGQP, Convert__RegVRRC1_0__RegVRRC1_1, 0, { MCK_RegVRRC, MCK_RegVRRC }, },
  { 13318 /* xvabsdp */, PPC::XVABSDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13326 /* xvabssp */, PPC::XVABSSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13334 /* xvadddp */, PPC::XVADDDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13342 /* xvaddsp */, PPC::XVADDSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13350 /* xvcmpeqdp */, PPC::XVCMPEQDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13350 /* xvcmpeqdp */, PPC::XVCMPEQDPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13360 /* xvcmpeqsp */, PPC::XVCMPEQSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13360 /* xvcmpeqsp */, PPC::XVCMPEQSPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13370 /* xvcmpgedp */, PPC::XVCMPGEDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13370 /* xvcmpgedp */, PPC::XVCMPGEDPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13380 /* xvcmpgesp */, PPC::XVCMPGESP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13380 /* xvcmpgesp */, PPC::XVCMPGESPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13390 /* xvcmpgtdp */, PPC::XVCMPGTDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13390 /* xvcmpgtdp */, PPC::XVCMPGTDPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13400 /* xvcmpgtsp */, PPC::XVCMPGTSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13400 /* xvcmpgtsp */, PPC::XVCMPGTSPo, Convert__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK__DOT_, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13410 /* xvcpsgndp */, PPC::XVCPSGNDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13420 /* xvcpsgnsp */, PPC::XVCPSGNSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13430 /* xvcvdpsp */, PPC::XVCVDPSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13439 /* xvcvdpsxds */, PPC::XVCVDPSXDS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13450 /* xvcvdpsxws */, PPC::XVCVDPSXWS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13461 /* xvcvdpuxds */, PPC::XVCVDPUXDS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13472 /* xvcvdpuxws */, PPC::XVCVDPUXWS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13483 /* xvcvhpsp */, PPC::XVCVHPSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13492 /* xvcvspdp */, PPC::XVCVSPDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13501 /* xvcvsphp */, PPC::XVCVSPHP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13510 /* xvcvspsxds */, PPC::XVCVSPSXDS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13521 /* xvcvspsxws */, PPC::XVCVSPSXWS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13532 /* xvcvspuxds */, PPC::XVCVSPUXDS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13543 /* xvcvspuxws */, PPC::XVCVSPUXWS, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13554 /* xvcvsxddp */, PPC::XVCVSXDDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13564 /* xvcvsxdsp */, PPC::XVCVSXDSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13574 /* xvcvsxwdp */, PPC::XVCVSXWDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13584 /* xvcvsxwsp */, PPC::XVCVSXWSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13594 /* xvcvuxddp */, PPC::XVCVUXDDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13604 /* xvcvuxdsp */, PPC::XVCVUXDSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13614 /* xvcvuxwdp */, PPC::XVCVUXWDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13624 /* xvcvuxwsp */, PPC::XVCVUXWSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13634 /* xvdivdp */, PPC::XVDIVDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13642 /* xvdivsp */, PPC::XVDIVSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13650 /* xviexpdp */, PPC::XVIEXPDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13659 /* xviexpsp */, PPC::XVIEXPSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13668 /* xvmaddadp */, PPC::XVMADDADP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13678 /* xvmaddasp */, PPC::XVMADDASP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13688 /* xvmaddmdp */, PPC::XVMADDMDP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13698 /* xvmaddmsp */, PPC::XVMADDMSP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13708 /* xvmaxdp */, PPC::XVMAXDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13716 /* xvmaxsp */, PPC::XVMAXSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13724 /* xvmindp */, PPC::XVMINDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13732 /* xvminsp */, PPC::XVMINSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13740 /* xvmovdp */, PPC::XVCPSGNDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13748 /* xvmovsp */, PPC::XVCPSGNSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13756 /* xvmsubadp */, PPC::XVMSUBADP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13766 /* xvmsubasp */, PPC::XVMSUBASP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13776 /* xvmsubmdp */, PPC::XVMSUBMDP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13786 /* xvmsubmsp */, PPC::XVMSUBMSP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13796 /* xvmuldp */, PPC::XVMULDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13804 /* xvmulsp */, PPC::XVMULSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13812 /* xvnabsdp */, PPC::XVNABSDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13821 /* xvnabssp */, PPC::XVNABSSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13830 /* xvnegdp */, PPC::XVNEGDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13838 /* xvnegsp */, PPC::XVNEGSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13846 /* xvnmaddadp */, PPC::XVNMADDADP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13857 /* xvnmaddasp */, PPC::XVNMADDASP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13868 /* xvnmaddmdp */, PPC::XVNMADDMDP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13879 /* xvnmaddmsp */, PPC::XVNMADDMSP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13890 /* xvnmsubadp */, PPC::XVNMSUBADP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13901 /* xvnmsubasp */, PPC::XVNMSUBASP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13912 /* xvnmsubmdp */, PPC::XVNMSUBMDP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13923 /* xvnmsubmsp */, PPC::XVNMSUBMSP, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 13934 /* xvrdpi */, PPC::XVRDPI, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13941 /* xvrdpic */, PPC::XVRDPIC, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13949 /* xvrdpim */, PPC::XVRDPIM, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13957 /* xvrdpip */, PPC::XVRDPIP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13965 /* xvrdpiz */, PPC::XVRDPIZ, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13973 /* xvredp */, PPC::XVREDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13980 /* xvresp */, PPC::XVRESP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13987 /* xvrspi */, PPC::XVRSPI, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 13994 /* xvrspic */, PPC::XVRSPIC, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14002 /* xvrspim */, PPC::XVRSPIM, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14010 /* xvrspip */, PPC::XVRSPIP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14018 /* xvrspiz */, PPC::XVRSPIZ, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14026 /* xvrsqrtedp */, PPC::XVRSQRTEDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14037 /* xvrsqrtesp */, PPC::XVRSQRTESP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14048 /* xvsqrtdp */, PPC::XVSQRTDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14057 /* xvsqrtsp */, PPC::XVSQRTSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14066 /* xvsubdp */, PPC::XVSUBDP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14074 /* xvsubsp */, PPC::XVSUBSP, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14082 /* xvtdivdp */, PPC::XVTDIVDP, Convert__RegCRRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegCRRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14091 /* xvtdivsp */, PPC::XVTDIVSP, Convert__RegCRRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegCRRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14100 /* xvtsqrtdp */, PPC::XVTSQRTDP, Convert__RegCRRC1_0__RegVSRC1_1, 0, { MCK_RegCRRC, MCK_RegVSRC }, },
  { 14110 /* xvtsqrtsp */, PPC::XVTSQRTSP, Convert__RegCRRC1_0__RegVSRC1_1, 0, { MCK_RegCRRC, MCK_RegVSRC }, },
  { 14120 /* xvtstdcdp */, PPC::XVTSTDCDP, Convert__RegVSRC1_0__U7Imm1_2__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_U7Imm }, },
  { 14130 /* xvtstdcsp */, PPC::XVTSTDCSP, Convert__RegVSRC1_0__U7Imm1_2__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_U7Imm }, },
  { 14140 /* xvxexpdp */, PPC::XVXEXPDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14149 /* xvxexpsp */, PPC::XVXEXPSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14158 /* xvxsigdp */, PPC::XVXSIGDP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14167 /* xvxsigsp */, PPC::XVXSIGSP, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14176 /* xxbrd */, PPC::XXBRD, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14182 /* xxbrh */, PPC::XXBRH, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14188 /* xxbrq */, PPC::XXBRQ, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14194 /* xxbrw */, PPC::XXBRW, Convert__RegVSRC1_0__RegVSRC1_1, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
  { 14200 /* xxextractuw */, PPC::XXEXTRACTUW, Convert__RegVSFRC1_0__RegVSRC1_1__U4Imm1_2, 0, { MCK_RegVSFRC, MCK_RegVSRC, MCK_U4Imm }, },
  { 14212 /* xxinsertw */, PPC::XXINSERTW, Convert__RegVSRC1_0__Tie0_1_1__RegVSRC1_1__U4Imm1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_U4Imm }, },
  { 14222 /* xxland */, PPC::XXLAND, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14229 /* xxlandc */, PPC::XXLANDC, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14237 /* xxleqv */, PPC::XXLEQV, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14244 /* xxlnand */, PPC::XXLNAND, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14252 /* xxlnor */, PPC::XXLNOR, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14259 /* xxlor */, PPC::XXLOR, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14265 /* xxlorc */, PPC::XXLORC, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14272 /* xxlxor */, PPC::XXLXOR, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14279 /* xxmrghd */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_0, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14287 /* xxmrghw */, PPC::XXMRGHW, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14295 /* xxmrgld */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__imm_95_3, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14303 /* xxmrglw */, PPC::XXMRGLW, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14311 /* xxperm */, PPC::XXPERM, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14318 /* xxpermdi */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__U2Imm1_3, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC, MCK_U2Imm }, },
  { 14327 /* xxpermr */, PPC::XXPERMR, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14335 /* xxsel */, PPC::XXSEL, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__RegVSRC1_3, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC }, },
  { 14341 /* xxsldwi */, PPC::XXSLDWI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_2__U2Imm1_3, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_RegVSRC, MCK_U2Imm }, },
  { 14349 /* xxspltd */, PPC::XXPERMDIs, Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_0, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_0 }, },
  { 14349 /* xxspltd */, PPC::XXPERMDIs, Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_3, 0, { MCK_RegVSRC, MCK_RegVSFRC, MCK_1 }, },
  { 14349 /* xxspltd */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_0, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_0 }, },
  { 14349 /* xxspltd */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_3, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_1 }, },
  { 14357 /* xxspltib */, PPC::XXSPLTIB, Convert__RegVSRC1_0__U8Imm1_1, 0, { MCK_RegVSRC, MCK_U8Imm }, },
  { 14366 /* xxspltw */, PPC::XXSPLTW, Convert__RegVSRC1_0__RegVSRC1_1__U2Imm1_2, 0, { MCK_RegVSRC, MCK_RegVSRC, MCK_U2Imm }, },
  { 14374 /* xxswapd */, PPC::XXPERMDIs, Convert__RegVSRC1_0__RegVSFRC1_1__imm_95_2, 0, { MCK_RegVSRC, MCK_RegVSFRC }, },
  { 14374 /* xxswapd */, PPC::XXPERMDI, Convert__RegVSRC1_0__RegVSRC1_1__RegVSRC1_1__imm_95_2, 0, { MCK_RegVSRC, MCK_RegVSRC }, },
};

#include "llvm/Support/Debug.h"
#include "llvm/Support/Format.h"

unsigned PPCAsmParser::
MatchInstructionImpl(const OperandVector &Operands,
                     MCInst &Inst,
                     uint64_t &ErrorInfo,
                     bool matchingInlineAsm, unsigned VariantID) {
  // Eliminate obvious mismatches.
  if (Operands.size() > 7) {
    ErrorInfo = 7;
    return Match_InvalidOperand;
  }

  // Get the current feature set.
  uint64_t AvailableFeatures = getAvailableFeatures();

  // Get the instruction mnemonic, which is the first token.
  StringRef Mnemonic = ((PPCOperand&)*Operands[0]).getToken();

  // Process all MnemonicAliases to remap the mnemonic.
  applyMnemonicAliases(Mnemonic, AvailableFeatures, VariantID);

  // Some state to try to produce better error messages.
  bool HadMatchOtherThanFeatures = false;
  bool HadMatchOtherThanPredicate = false;
  unsigned RetCode = Match_InvalidOperand;
  uint64_t MissingFeatures = ~0ULL;
  // Set ErrorInfo to the operand that mismatches if it is
  // wrong for all instances of the instruction.
  ErrorInfo = ~0ULL;
  // Find the appropriate table for this asm variant.
  const MatchEntry *Start, *End;
  switch (VariantID) {
  default: llvm_unreachable("invalid variant!");
  case 0: Start = std::begin(MatchTable0); End = std::end(MatchTable0); break;
  }
  // Search the table.
  auto MnemonicRange = std::equal_range(Start, End, Mnemonic, LessOpcode());

  DEBUG_WITH_TYPE("asm-matcher", dbgs() << "AsmMatcher: found " <<
  std::distance(MnemonicRange.first, MnemonicRange.second) << 
  " encodings with mnemonic '" << Mnemonic << "'\n");

  // Return a more specific error code if no mnemonics match.
  if (MnemonicRange.first == MnemonicRange.second)
    return Match_MnemonicFail;

  for (const MatchEntry *it = MnemonicRange.first, *ie = MnemonicRange.second;
       it != ie; ++it) {
    bool HasRequiredFeatures =
      (AvailableFeatures & it->RequiredFeatures) == it->RequiredFeatures;
    DEBUG_WITH_TYPE("asm-matcher", dbgs() << "Trying to match opcode "
                                          << MII.getName(it->Opcode) << "\n");
    // equal_range guarantees that instruction mnemonic matches.
    assert(Mnemonic == it->getMnemonic());
    bool OperandsValid = true;
    for (unsigned FormalIdx = 0, ActualIdx = 1; FormalIdx != 6; ++FormalIdx) {
      auto Formal = static_cast<MatchClassKind>(it->Classes[FormalIdx]);
      DEBUG_WITH_TYPE("asm-matcher",
                      dbgs() << "  Matching formal operand class " << getMatchClassName(Formal)
                             << " against actual operand at index " << ActualIdx);
      if (ActualIdx < Operands.size())
        DEBUG_WITH_TYPE("asm-matcher", dbgs() << " (";
                        Operands[ActualIdx]->print(dbgs()); dbgs() << "): ");
      else
        DEBUG_WITH_TYPE("asm-matcher", dbgs() << ": ");
      if (ActualIdx >= Operands.size()) {
        DEBUG_WITH_TYPE("asm-matcher", dbgs() << "actual operand index out of range ");
        OperandsValid = (Formal == InvalidMatchClass) || isSubclass(Formal, OptionalMatchClass);
        if (!OperandsValid) ErrorInfo = ActualIdx;
        break;
      }
      MCParsedAsmOperand &Actual = *Operands[ActualIdx];
      unsigned Diag = validateOperandClass(Actual, Formal);
      if (Diag == Match_Success) {
        DEBUG_WITH_TYPE("asm-matcher",
                        dbgs() << "match success using generic matcher\n");
        ++ActualIdx;
        continue;
      }
      // If the generic handler indicates an invalid operand
      // failure, check for a special case.
      if (Diag != Match_Success) {
        unsigned TargetDiag = validateTargetOperandClass(Actual, Formal);
        if (TargetDiag == Match_Success) {
          DEBUG_WITH_TYPE("asm-matcher",
                          dbgs() << "match success using target matcher\n");
          ++ActualIdx;
          continue;
        }
        // If the target matcher returned a specific error code use
        // that, else use the one from the generic matcher.
        if (TargetDiag != Match_InvalidOperand && HasRequiredFeatures)
          Diag = TargetDiag;
      }
      // If current formal operand wasn't matched and it is optional
      // then try to match next formal operand
      if (Diag == Match_InvalidOperand && isSubclass(Formal, OptionalMatchClass)) {
        DEBUG_WITH_TYPE("asm-matcher", dbgs() << "ignoring optional operand\n");
        continue;
      }
      // If this operand is broken for all of the instances of this
      // mnemonic, keep track of it so we can report loc info.
      // If we already had a match that only failed due to a
      // target predicate, that diagnostic is preferred.
      if (!HadMatchOtherThanPredicate &&
          (it == MnemonicRange.first || ErrorInfo <= ActualIdx)) {
        if (HasRequiredFeatures && (ErrorInfo != ActualIdx || Diag != Match_InvalidOperand))
          RetCode = Diag;
        ErrorInfo = ActualIdx;
      }
      // Otherwise, just reject this instance of the mnemonic.
      OperandsValid = false;
      break;
    }

    if (!OperandsValid) {
      DEBUG_WITH_TYPE("asm-matcher", dbgs() << "Opcode result: multiple "
                                               "operand mismatches, ignoring "
                                               "this opcode\n");
      continue;
    }
    if (!HasRequiredFeatures) {
      HadMatchOtherThanFeatures = true;
      uint64_t NewMissingFeatures = it->RequiredFeatures & ~AvailableFeatures;
      DEBUG_WITH_TYPE("asm-matcher", dbgs() << "Missing target features: "
                                            << format_hex(NewMissingFeatures, 18)
                                            << "\n");
      if (countPopulation(NewMissingFeatures) <=
          countPopulation(MissingFeatures))
        MissingFeatures = NewMissingFeatures;
      continue;
    }

    Inst.clear();

    Inst.setOpcode(it->Opcode);
    // We have a potential match but have not rendered the operands.
    // Check the target predicate to handle any context sensitive
    // constraints.
    // For example, Ties that are referenced multiple times must be
    // checked here to ensure the input is the same for each match
    // constraints. If we leave it any later the ties will have been
    // canonicalized
    unsigned MatchResult;
    if ((MatchResult = checkEarlyTargetMatchPredicate(Inst, Operands)) != Match_Success) {
      Inst.clear();
      DEBUG_WITH_TYPE(
          "asm-matcher",
          dbgs() << "Early target match predicate failed with diag code "
                 << MatchResult << "\n");
      RetCode = MatchResult;
      HadMatchOtherThanPredicate = true;
      continue;
    }

    if (matchingInlineAsm) {
      convertToMapAndConstraints(it->ConvertFn, Operands);
      if (!checkAsmTiedOperandConstraints(*this, it->ConvertFn, Operands, ErrorInfo))
        return Match_InvalidTiedOperand;

      return Match_Success;
    }

    // We have selected a definite instruction, convert the parsed
    // operands into the appropriate MCInst.
    convertToMCInst(it->ConvertFn, Inst, it->Opcode, Operands);

    // We have a potential match. Check the target predicate to
    // handle any context sensitive constraints.
    if ((MatchResult = checkTargetMatchPredicate(Inst)) != Match_Success) {
      DEBUG_WITH_TYPE("asm-matcher",
                      dbgs() << "Target match predicate failed with diag code "
                             << MatchResult << "\n");
      Inst.clear();
      RetCode = MatchResult;
      HadMatchOtherThanPredicate = true;
      continue;
    }

    std::string Info;
    if (!getParser().getTargetParser().
        getTargetOptions().MCNoDeprecatedWarn &&
        MII.get(Inst.getOpcode()).getDeprecatedInfo(Inst, getSTI(), Info)) {
      SMLoc Loc = ((PPCOperand&)*Operands[0]).getStartLoc();
      getParser().Warning(Loc, Info, None);
    }
    if (!checkAsmTiedOperandConstraints(*this, it->ConvertFn, Operands, ErrorInfo))
      return Match_InvalidTiedOperand;

    DEBUG_WITH_TYPE(
        "asm-matcher",
        dbgs() << "Opcode result: complete match, selecting this opcode\n");
    return Match_Success;
  }

  // Okay, we had no match.  Try to return a useful error code.
  if (HadMatchOtherThanPredicate || !HadMatchOtherThanFeatures)
    return RetCode;

  // Missing feature matches return which features were missing
  ErrorInfo = MissingFeatures;
  return Match_MissingFeature;
}

#endif // GET_MATCHER_IMPLEMENTATION


#ifdef GET_MNEMONIC_SPELL_CHECKER
#undef GET_MNEMONIC_SPELL_CHECKER

static std::string PPCMnemonicSpellCheck(StringRef S, uint64_t FBS, unsigned VariantID) {
  const unsigned MaxEditDist = 2;
  std::vector<StringRef> Candidates;
  StringRef Prev = "";

  // Find the appropriate table for this asm variant.
  const MatchEntry *Start, *End;
  switch (VariantID) {
  default: llvm_unreachable("invalid variant!");
  case 0: Start = std::begin(MatchTable0); End = std::end(MatchTable0); break;
  }

  for (auto I = Start; I < End; I++) {
    // Ignore unsupported instructions.
    if ((FBS & I->RequiredFeatures) != I->RequiredFeatures)
      continue;

    StringRef T = I->getMnemonic();
    // Avoid recomputing the edit distance for the same string.
    if (T.equals(Prev))
      continue;

    Prev = T;
    unsigned Dist = S.edit_distance(T, false, MaxEditDist);
    if (Dist <= MaxEditDist)
      Candidates.push_back(T);
  }

  if (Candidates.empty())
    return "";

  std::string Res = ", did you mean: ";
  unsigned i = 0;
  for( ; i < Candidates.size() - 1; i++)
    Res += Candidates[i].str() + ", ";
  return Res + Candidates[i].str() + "?";
}

#endif // GET_MNEMONIC_SPELL_CHECKER

