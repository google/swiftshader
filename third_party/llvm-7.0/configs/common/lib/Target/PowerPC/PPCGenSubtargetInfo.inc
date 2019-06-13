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
namespace PPC {
enum {
  DeprecatedDST = 0,
  Directive32 = 1,
  Directive64 = 2,
  Directive440 = 3,
  Directive601 = 4,
  Directive602 = 5,
  Directive603 = 6,
  Directive604 = 7,
  Directive620 = 8,
  Directive750 = 9,
  Directive970 = 10,
  Directive7400 = 11,
  DirectiveA2 = 12,
  DirectiveE500 = 13,
  DirectiveE500mc = 14,
  DirectiveE5500 = 15,
  DirectivePwr3 = 16,
  DirectivePwr4 = 17,
  DirectivePwr5 = 18,
  DirectivePwr5x = 19,
  DirectivePwr6 = 20,
  DirectivePwr6x = 21,
  DirectivePwr7 = 22,
  DirectivePwr8 = 23,
  DirectivePwr9 = 24,
  Feature64Bit = 25,
  Feature64BitRegs = 26,
  FeatureAltivec = 27,
  FeatureBPERMD = 28,
  FeatureBookE = 29,
  FeatureCMPB = 30,
  FeatureCRBits = 31,
  FeatureDirectMove = 32,
  FeatureE500 = 33,
  FeatureExtDiv = 34,
  FeatureFCPSGN = 35,
  FeatureFPCVT = 36,
  FeatureFPRND = 37,
  FeatureFPU = 38,
  FeatureFRE = 39,
  FeatureFRES = 40,
  FeatureFRSQRTE = 41,
  FeatureFRSQRTES = 42,
  FeatureFSqrt = 43,
  FeatureFloat128 = 44,
  FeatureFusion = 45,
  FeatureHTM = 46,
  FeatureHardFloat = 47,
  FeatureICBT = 48,
  FeatureISA3_0 = 49,
  FeatureISEL = 50,
  FeatureInvariantFunctionDescriptors = 51,
  FeatureLDBRX = 52,
  FeatureLFIWAX = 53,
  FeatureLongCall = 54,
  FeatureMFOCRF = 55,
  FeatureMFTB = 56,
  FeatureMSYNC = 57,
  FeatureP8Altivec = 58,
  FeatureP8Crypto = 59,
  FeatureP8Vector = 60,
  FeatureP9Altivec = 61,
  FeatureP9Vector = 62,
  FeaturePOPCNTD = 63,
  FeaturePPC4xx = 64,
  FeaturePPC6xx = 65,
  FeaturePartwordAtomic = 66,
  FeatureQPX = 67,
  FeatureRecipPrec = 68,
  FeatureSPE = 69,
  FeatureSTFIWX = 70,
  FeatureSecurePlt = 71,
  FeatureSlowPOPCNTD = 72,
  FeatureVSX = 73,
};
} // end namespace PPC
} // end namespace llvm

#endif // GET_SUBTARGETINFO_ENUM


#ifdef GET_SUBTARGETINFO_MC_DESC
#undef GET_SUBTARGETINFO_MC_DESC

namespace llvm {
// Sorted (by key) array of values for CPU features.
extern const llvm::SubtargetFeatureKV PPCFeatureKV[] = {
  { "64bit", "Enable 64-bit instructions", { PPC::Feature64Bit }, { } },
  { "64bitregs", "Enable 64-bit registers usage for ppc32 [beta]", { PPC::Feature64BitRegs }, { } },
  { "altivec", "Enable Altivec instructions", { PPC::FeatureAltivec }, { PPC::FeatureFPU } },
  { "booke", "Enable Book E instructions", { PPC::FeatureBookE }, { PPC::FeatureICBT } },
  { "bpermd", "Enable the bpermd instruction", { PPC::FeatureBPERMD }, { } },
  { "cmpb", "Enable the cmpb instruction", { PPC::FeatureCMPB }, { } },
  { "crbits", "Use condition-register bits individually", { PPC::FeatureCRBits }, { } },
  { "crypto", "Enable POWER8 Crypto instructions", { PPC::FeatureP8Crypto }, { PPC::FeatureP8Altivec } },
  { "direct-move", "Enable Power8 direct move instructions", { PPC::FeatureDirectMove }, { PPC::FeatureVSX } },
  { "e500", "Enable E500/E500mc instructions", { PPC::FeatureE500 }, { } },
  { "extdiv", "Enable extended divide instructions", { PPC::FeatureExtDiv }, { } },
  { "fcpsgn", "Enable the fcpsgn instruction", { PPC::FeatureFCPSGN }, { PPC::FeatureFPU } },
  { "float128", "Enable the __float128 data type for IEEE-754R Binary128.", { PPC::FeatureFloat128 }, { PPC::FeatureVSX } },
  { "fpcvt", "Enable fc[ft]* (unsigned and single-precision) and lfiwzx instructions", { PPC::FeatureFPCVT }, { PPC::FeatureFPU } },
  { "fprnd", "Enable the fri[mnpz] instructions", { PPC::FeatureFPRND }, { PPC::FeatureFPU } },
  { "fpu", "Enable classic FPU instructions", { PPC::FeatureFPU }, { PPC::FeatureHardFloat } },
  { "fre", "Enable the fre instruction", { PPC::FeatureFRE }, { PPC::FeatureFPU } },
  { "fres", "Enable the fres instruction", { PPC::FeatureFRES }, { PPC::FeatureFPU } },
  { "frsqrte", "Enable the frsqrte instruction", { PPC::FeatureFRSQRTE }, { PPC::FeatureFPU } },
  { "frsqrtes", "Enable the frsqrtes instruction", { PPC::FeatureFRSQRTES }, { PPC::FeatureFPU } },
  { "fsqrt", "Enable the fsqrt instruction", { PPC::FeatureFSqrt }, { PPC::FeatureFPU } },
  { "fusion", "Target supports add/load integer fusion.", { PPC::FeatureFusion }, { } },
  { "hard-float", "Enable floating-point instructions", { PPC::FeatureHardFloat }, { } },
  { "htm", "Enable Hardware Transactional Memory instructions", { PPC::FeatureHTM }, { } },
  { "icbt", "Enable icbt instruction", { PPC::FeatureICBT }, { } },
  { "invariant-function-descriptors", "Assume function descriptors are invariant", { PPC::FeatureInvariantFunctionDescriptors }, { } },
  { "isa-v30-instructions", "Enable instructions added in ISA 3.0.", { PPC::FeatureISA3_0 }, { } },
  { "isel", "Enable the isel instruction", { PPC::FeatureISEL }, { } },
  { "ldbrx", "Enable the ldbrx instruction", { PPC::FeatureLDBRX }, { } },
  { "lfiwax", "Enable the lfiwax instruction", { PPC::FeatureLFIWAX }, { PPC::FeatureFPU } },
  { "longcall", "Always use indirect calls", { PPC::FeatureLongCall }, { } },
  { "mfocrf", "Enable the MFOCRF instruction", { PPC::FeatureMFOCRF }, { } },
  { "msync", "Has only the msync instruction instead of sync", { PPC::FeatureMSYNC }, { PPC::FeatureBookE } },
  { "partword-atomics", "Enable l[bh]arx and st[bh]cx.", { PPC::FeaturePartwordAtomic }, { } },
  { "popcntd", "Enable the popcnt[dw] instructions", { PPC::FeaturePOPCNTD }, { } },
  { "power8-altivec", "Enable POWER8 Altivec instructions", { PPC::FeatureP8Altivec }, { PPC::FeatureAltivec } },
  { "power8-vector", "Enable POWER8 vector instructions", { PPC::FeatureP8Vector }, { PPC::FeatureVSX, PPC::FeatureP8Altivec } },
  { "power9-altivec", "Enable POWER9 Altivec instructions", { PPC::FeatureP9Altivec }, { PPC::FeatureISA3_0, PPC::FeatureP8Altivec } },
  { "power9-vector", "Enable POWER9 vector instructions", { PPC::FeatureP9Vector }, { PPC::FeatureISA3_0, PPC::FeatureP8Vector, PPC::FeatureP9Altivec } },
  { "ppc4xx", "Enable PPC 4xx instructions", { PPC::FeaturePPC4xx }, { } },
  { "ppc6xx", "Enable PPC 6xx instructions", { PPC::FeaturePPC6xx }, { } },
  { "qpx", "Enable QPX instructions", { PPC::FeatureQPX }, { PPC::FeatureFPU } },
  { "recipprec", "Assume higher precision reciprocal estimates", { PPC::FeatureRecipPrec }, { } },
  { "secure-plt", "Enable secure plt mode", { PPC::FeatureSecurePlt }, { } },
  { "slow-popcntd", "Has slow popcnt[dw] instructions", { PPC::FeatureSlowPOPCNTD }, { } },
  { "spe", "Enable SPE instructions", { PPC::FeatureSPE }, { PPC::FeatureHardFloat } },
  { "stfiwx", "Enable the stfiwx instruction", { PPC::FeatureSTFIWX }, { PPC::FeatureFPU } },
  { "vsx", "Enable VSX instructions", { PPC::FeatureVSX }, { PPC::FeatureAltivec } },
};

// Sorted (by key) array of values for CPU subtype.
extern const llvm::SubtargetFeatureKV PPCSubTypeKV[] = {
  { "440", "Select the 440 processor", { PPC::Directive440, PPC::FeatureISEL, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureMSYNC, PPC::FeatureMFTB }, { } },
  { "450", "Select the 450 processor", { PPC::Directive440, PPC::FeatureISEL, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureMSYNC, PPC::FeatureMFTB }, { } },
  { "601", "Select the 601 processor", { PPC::Directive601, PPC::FeatureFPU }, { } },
  { "602", "Select the 602 processor", { PPC::Directive602, PPC::FeatureFPU, PPC::FeatureMFTB }, { } },
  { "603", "Select the 603 processor", { PPC::Directive603, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "603e", "Select the 603e processor", { PPC::Directive603, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "603ev", "Select the 603ev processor", { PPC::Directive603, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "604", "Select the 604 processor", { PPC::Directive604, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "604e", "Select the 604e processor", { PPC::Directive604, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "620", "Select the 620 processor", { PPC::Directive620, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "7400", "Select the 7400 processor", { PPC::Directive7400, PPC::FeatureAltivec, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "7450", "Select the 7450 processor", { PPC::Directive7400, PPC::FeatureAltivec, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "750", "Select the 750 processor", { PPC::Directive750, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "970", "Select the 970 processor", { PPC::Directive970, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureSTFIWX, PPC::Feature64Bit, PPC::FeatureMFTB }, { } },
  { "a2", "Select the a2 processor", { PPC::DirectiveA2, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeatureSlowPOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureMFTB }, { } },
  { "a2q", "Select the a2q processor", { PPC::DirectiveA2, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeatureSlowPOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureQPX, PPC::FeatureMFTB }, { } },
  { "e500", "Select the e500 processor", { PPC::DirectiveE500, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureISEL, PPC::FeatureMFTB }, { } },
  { "e500mc", "Select the e500mc processor", { PPC::DirectiveE500mc, PPC::FeatureSTFIWX, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureISEL, PPC::FeatureMFTB }, { } },
  { "e5500", "Select the e5500 processor", { PPC::DirectiveE5500, PPC::FeatureMFOCRF, PPC::Feature64Bit, PPC::FeatureSTFIWX, PPC::FeatureICBT, PPC::FeatureBookE, PPC::FeatureISEL, PPC::FeatureMFTB }, { } },
  { "g3", "Select the g3 processor", { PPC::Directive750, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "g4", "Select the g4 processor", { PPC::Directive7400, PPC::FeatureAltivec, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "g4+", "Select the g4+ processor", { PPC::Directive7400, PPC::FeatureAltivec, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFTB }, { } },
  { "g5", "Select the g5 processor", { PPC::Directive970, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureSTFIWX, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::Feature64Bit, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "generic", "Select the generic processor", { PPC::Directive32, PPC::FeatureHardFloat, PPC::FeatureMFTB }, { } },
  { "ppc", "Select the ppc processor", { PPC::Directive32, PPC::FeatureHardFloat, PPC::FeatureMFTB }, { } },
  { "ppc32", "Select the ppc32 processor", { PPC::Directive32, PPC::FeatureHardFloat, PPC::FeatureMFTB }, { } },
  { "ppc64", "Select the ppc64 processor", { PPC::Directive64, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureSTFIWX, PPC::Feature64Bit, PPC::FeatureMFTB }, { } },
  { "ppc64le", "Select the ppc64le processor", { PPC::DirectivePwr7, PPC::FeatureAltivec, PPC::FeatureVSX, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeaturePOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureBPERMD, PPC::FeatureExtDiv, PPC::FeatureMFTB, PPC::DeprecatedDST, PPC::DirectivePwr8, PPC::FeatureP8Altivec, PPC::FeatureP8Vector, PPC::FeatureP8Crypto, PPC::FeatureHTM, PPC::FeatureDirectMove, PPC::FeatureICBT, PPC::FeaturePartwordAtomic, PPC::FeatureFusion }, { } },
  { "pwr3", "Select the pwr3 processor", { PPC::DirectivePwr3, PPC::FeatureAltivec, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureMFOCRF, PPC::FeatureSTFIWX, PPC::Feature64Bit }, { } },
  { "pwr4", "Select the pwr4 processor", { PPC::DirectivePwr4, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureSTFIWX, PPC::Feature64Bit, PPC::FeatureMFTB }, { } },
  { "pwr5", "Select the pwr5 processor", { PPC::DirectivePwr5, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureSTFIWX, PPC::Feature64Bit, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "pwr5x", "Select the pwr5x processor", { PPC::DirectivePwr5x, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureSTFIWX, PPC::FeatureFPRND, PPC::Feature64Bit, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "pwr6", "Select the pwr6 processor", { PPC::DirectivePwr6, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureCMPB, PPC::FeatureFPRND, PPC::Feature64Bit, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "pwr6x", "Select the pwr6x processor", { PPC::DirectivePwr5x, PPC::FeatureAltivec, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureCMPB, PPC::FeatureFPRND, PPC::Feature64Bit, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "pwr7", "Select the pwr7 processor", { PPC::DirectivePwr7, PPC::FeatureAltivec, PPC::FeatureVSX, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeaturePOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureBPERMD, PPC::FeatureExtDiv, PPC::FeatureMFTB, PPC::DeprecatedDST }, { } },
  { "pwr8", "Select the pwr8 processor", { PPC::DirectivePwr7, PPC::FeatureAltivec, PPC::FeatureVSX, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeaturePOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureBPERMD, PPC::FeatureExtDiv, PPC::FeatureMFTB, PPC::DeprecatedDST, PPC::DirectivePwr8, PPC::FeatureP8Altivec, PPC::FeatureP8Vector, PPC::FeatureP8Crypto, PPC::FeatureHTM, PPC::FeatureDirectMove, PPC::FeatureICBT, PPC::FeaturePartwordAtomic, PPC::FeatureFusion }, { } },
  { "pwr9", "Select the pwr9 processor", { PPC::DirectivePwr7, PPC::FeatureAltivec, PPC::FeatureVSX, PPC::FeatureMFOCRF, PPC::FeatureFCPSGN, PPC::FeatureFSqrt, PPC::FeatureFRE, PPC::FeatureFRES, PPC::FeatureFRSQRTE, PPC::FeatureFRSQRTES, PPC::FeatureRecipPrec, PPC::FeatureSTFIWX, PPC::FeatureLFIWAX, PPC::FeatureFPRND, PPC::FeatureFPCVT, PPC::FeatureISEL, PPC::FeaturePOPCNTD, PPC::FeatureCMPB, PPC::FeatureLDBRX, PPC::Feature64Bit, PPC::FeatureBPERMD, PPC::FeatureExtDiv, PPC::FeatureMFTB, PPC::DeprecatedDST, PPC::DirectivePwr8, PPC::FeatureP8Altivec, PPC::FeatureP8Vector, PPC::FeatureP8Crypto, PPC::FeatureHTM, PPC::FeatureDirectMove, PPC::FeatureICBT, PPC::FeaturePartwordAtomic, PPC::FeatureFusion, PPC::DirectivePwr9, PPC::FeatureP9Altivec, PPC::FeatureP9Vector, PPC::FeatureISA3_0 }, { } },
};

#ifdef DBGFIELD
#error "<target>GenSubtargetInfo.inc requires a DBGFIELD macro"
#endif
#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
#define DBGFIELD(x) x,
#else
#define DBGFIELD(x)
#endif

// Functional units for "PPC440Itineraries"
namespace PPC440ItinerariesFU {
  const unsigned P440_DISS1 = 1 << 0;
  const unsigned P440_DISS2 = 1 << 1;
  const unsigned P440_FRACC = 1 << 2;
  const unsigned P440_IRACC = 1 << 3;
  const unsigned P440_IEXE1 = 1 << 4;
  const unsigned P440_IEXE2 = 1 << 5;
  const unsigned P440_IWB = 1 << 6;
  const unsigned P440_LRACC = 1 << 7;
  const unsigned P440_JEXE1 = 1 << 8;
  const unsigned P440_JEXE2 = 1 << 9;
  const unsigned P440_JWB = 1 << 10;
  const unsigned P440_AGEN = 1 << 11;
  const unsigned P440_CRD = 1 << 12;
  const unsigned P440_LWB = 1 << 13;
  const unsigned P440_FEXE1 = 1 << 14;
  const unsigned P440_FEXE2 = 1 << 15;
  const unsigned P440_FEXE3 = 1 << 16;
  const unsigned P440_FEXE4 = 1 << 17;
  const unsigned P440_FEXE5 = 1 << 18;
  const unsigned P440_FEXE6 = 1 << 19;
  const unsigned P440_FWB = 1 << 20;
  const unsigned P440_LWARX_Hold = 1 << 21;
} // end namespace PPC440ItinerariesFU

// Pipeline forwarding paths for itineraries "PPC440Itineraries"
namespace PPC440ItinerariesBypass {
  const unsigned NoBypass = 0;
  const unsigned P440_GPR_Bypass = 1 << 0;
  const unsigned P440_FPR_Bypass = 1 << 1;
} // end namespace PPC440ItinerariesBypass

// Functional units for "G3Itineraries"
namespace G3ItinerariesFU {
  const unsigned G3_IU1 = 1 << 0;
  const unsigned G3_IU2 = 1 << 1;
  const unsigned G3_FPU1 = 1 << 2;
  const unsigned G3_BPU = 1 << 3;
  const unsigned G3_SRU = 1 << 4;
  const unsigned G3_SLU = 1 << 5;
} // end namespace G3ItinerariesFU

// Functional units for "G4Itineraries"
namespace G4ItinerariesFU {
  const unsigned G4_IU1 = 1 << 0;
  const unsigned G4_IU2 = 1 << 1;
  const unsigned G4_SLU = 1 << 2;
  const unsigned G4_SRU = 1 << 3;
  const unsigned G4_BPU = 1 << 4;
  const unsigned G4_FPU1 = 1 << 5;
  const unsigned G4_VIU1 = 1 << 6;
  const unsigned G4_VIU2 = 1 << 7;
  const unsigned G4_VPU = 1 << 8;
  const unsigned G4_VFPU = 1 << 9;
} // end namespace G4ItinerariesFU

// Functional units for "G4PlusItineraries"
namespace G4PlusItinerariesFU {
  const unsigned G4P_IU1 = 1 << 0;
  const unsigned G4P_IU2 = 1 << 1;
  const unsigned G4P_IU3 = 1 << 2;
  const unsigned G4P_IU4 = 1 << 3;
  const unsigned G4P_BPU = 1 << 4;
  const unsigned G4P_SLU = 1 << 5;
  const unsigned G4P_FPU1 = 1 << 6;
  const unsigned G4P_VFPU = 1 << 7;
  const unsigned G4P_VIU1 = 1 << 8;
  const unsigned G4P_VIU2 = 1 << 9;
  const unsigned G4P_VPU = 1 << 10;
} // end namespace G4PlusItinerariesFU

// Functional units for "G5Itineraries"
namespace G5ItinerariesFU {
  const unsigned G5_IU1 = 1 << 0;
  const unsigned G5_IU2 = 1 << 1;
  const unsigned G5_SLU = 1 << 2;
  const unsigned G5_BPU = 1 << 3;
  const unsigned G5_FPU1 = 1 << 4;
  const unsigned G5_FPU2 = 1 << 5;
  const unsigned G5_VFPU = 1 << 6;
  const unsigned G5_VIU1 = 1 << 7;
  const unsigned G5_VIU2 = 1 << 8;
  const unsigned G5_VPU = 1 << 9;
} // end namespace G5ItinerariesFU

// Functional units for "PPCA2Itineraries"
namespace PPCA2ItinerariesFU {
  const unsigned A2_XU = 1 << 0;
  const unsigned A2_FU = 1 << 1;
} // end namespace PPCA2ItinerariesFU

// Functional units for "PPCE500Itineraries"
namespace PPCE500ItinerariesFU {
  const unsigned E500_DIS0 = 1 << 0;
  const unsigned E500_DIS1 = 1 << 1;
  const unsigned E500_SU0 = 1 << 2;
  const unsigned E500_SU1 = 1 << 3;
  const unsigned E500_BU = 1 << 4;
  const unsigned E500_MU = 1 << 5;
  const unsigned E500_LSU_0 = 1 << 6;
} // end namespace PPCE500ItinerariesFU

// Pipeline forwarding paths for itineraries "PPCE500Itineraries"
namespace PPCE500ItinerariesBypass {
  const unsigned NoBypass = 0;
  const unsigned E500_CR_Bypass = 1 << 0;
  const unsigned E500_GPR_Bypass = 1 << 1;
  const unsigned E500_DivBypass = 1 << 2;
} // end namespace PPCE500ItinerariesBypass

// Functional units for "PPCE500mcItineraries"
namespace PPCE500mcItinerariesFU {
  const unsigned E500mc_DIS0 = 1 << 0;
  const unsigned E500mc_DIS1 = 1 << 1;
  const unsigned E500mc_SFX0 = 1 << 2;
  const unsigned E500mc_SFX1 = 1 << 3;
  const unsigned E500mc_BU = 1 << 4;
  const unsigned E500mc_CFX_DivBypass = 1 << 5;
  const unsigned E500mc_CFX_0 = 1 << 6;
  const unsigned E500mc_LSU_0 = 1 << 7;
  const unsigned E500mc_FPU_0 = 1 << 8;
} // end namespace PPCE500mcItinerariesFU

// Pipeline forwarding paths for itineraries "PPCE500mcItineraries"
namespace PPCE500mcItinerariesBypass {
  const unsigned NoBypass = 0;
  const unsigned E500mc_CR_Bypass = 1 << 0;
  const unsigned E500mc_GPR_Bypass = 1 << 1;
  const unsigned E500mc_FPR_Bypass = 1 << 2;
} // end namespace PPCE500mcItinerariesBypass

// Functional units for "PPCE5500Itineraries"
namespace PPCE5500ItinerariesFU {
  const unsigned E5500_DIS0 = 1 << 0;
  const unsigned E5500_DIS1 = 1 << 1;
  const unsigned E5500_SFX0 = 1 << 2;
  const unsigned E5500_SFX1 = 1 << 3;
  const unsigned E5500_BU = 1 << 4;
  const unsigned E5500_CFX_DivBypass = 1 << 5;
  const unsigned E5500_CFX_0 = 1 << 6;
  const unsigned E5500_CFX_1 = 1 << 7;
  const unsigned E5500_LSU_0 = 1 << 8;
  const unsigned E5500_FPU_0 = 1 << 9;
} // end namespace PPCE5500ItinerariesFU

// Pipeline forwarding paths for itineraries "PPCE5500Itineraries"
namespace PPCE5500ItinerariesBypass {
  const unsigned NoBypass = 0;
  const unsigned E5500_CR_Bypass = 1 << 0;
  const unsigned E5500_GPR_Bypass = 1 << 1;
  const unsigned E5500_FPR_Bypass = 1 << 2;
} // end namespace PPCE5500ItinerariesBypass

// Functional units for "P8Itineraries"
namespace P8ItinerariesFU {
  const unsigned P8_DU1 = 1 << 0;
  const unsigned P8_DU2 = 1 << 1;
  const unsigned P8_DU3 = 1 << 2;
  const unsigned P8_DU4 = 1 << 3;
  const unsigned P8_DU5 = 1 << 4;
  const unsigned P8_DU6 = 1 << 5;
  const unsigned P8_DU7 = 1 << 6;
  const unsigned P8_DU8 = 1 << 7;
  const unsigned P8_LU1 = 1 << 8;
  const unsigned P8_LU2 = 1 << 9;
  const unsigned P8_LSU1 = 1 << 10;
  const unsigned P8_LSU2 = 1 << 11;
  const unsigned P8_FXU1 = 1 << 12;
  const unsigned P8_FXU2 = 1 << 13;
  const unsigned P8_FPU1 = 1 << 14;
  const unsigned P8_FPU2 = 1 << 15;
  const unsigned P8_CRU = 1 << 16;
  const unsigned P8_BRU = 1 << 17;
} // end namespace P8ItinerariesFU

// Functional units for "P7Itineraries"
namespace P7ItinerariesFU {
  const unsigned P7_DU1 = 1 << 0;
  const unsigned P7_DU2 = 1 << 1;
  const unsigned P7_DU3 = 1 << 2;
  const unsigned P7_DU4 = 1 << 3;
  const unsigned P7_DU5 = 1 << 4;
  const unsigned P7_DU6 = 1 << 5;
  const unsigned P7_LS1 = 1 << 6;
  const unsigned P7_LS2 = 1 << 7;
  const unsigned P7_FX1 = 1 << 8;
  const unsigned P7_FX2 = 1 << 9;
  const unsigned P7_VS1 = 1 << 10;
  const unsigned P7_VS2 = 1 << 11;
  const unsigned P7_CRU = 1 << 12;
  const unsigned P7_BRU = 1 << 13;
} // end namespace P7ItinerariesFU

extern const llvm::InstrStage PPCStages[] = {
  { 0, 0, 0, llvm::InstrStage::Required }, // No itinerary
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 3, PPC440ItinerariesFU::P440_AGEN, 1, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPC440ItinerariesFU::P440_CRD, 1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 1-5
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC | PPC440ItinerariesFU::P440_LRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE1 | PPC440ItinerariesFU::P440_JEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE2 | PPC440ItinerariesFU::P440_JEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IWB | PPC440ItinerariesFU::P440_JWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 6-10
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 11-15
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_AGEN, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_CRD, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 16-20
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_AGEN, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_CRD, -1, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPC440ItinerariesFU::P440_LWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 21-25
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 33, PPC440ItinerariesFU::P440_IWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 26-30
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE3, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE4, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE5, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE6, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 31-39
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE3, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE4, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE5, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE6, -1, (llvm::InstrStage::ReservationKinds)0 },   { 25, PPC440ItinerariesFU::P440_FWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 40-48
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE3, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE4, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE5, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE6, -1, (llvm::InstrStage::ReservationKinds)0 },   { 13, PPC440ItinerariesFU::P440_FWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 49-57
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FRACC, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LRACC, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_AGEN, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_JEXE1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_FEXE2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_CRD, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_JEXE2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 6, PPC440ItinerariesFU::P440_FEXE3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 6, PPC440ItinerariesFU::P440_LWB, 0, (llvm::InstrStage::ReservationKinds)0 },   { 6, PPC440ItinerariesFU::P440_JWB, 0, (llvm::InstrStage::ReservationKinds)0 },   { 6, PPC440ItinerariesFU::P440_IWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 58-73
  { 1, PPC440ItinerariesFU::P440_DISS1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPC440ItinerariesFU::P440_LWARX_Hold, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_AGEN, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_CRD, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_LWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 74-80
  { 1, PPC440ItinerariesFU::P440_DISS1 | PPC440ItinerariesFU::P440_DISS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IRACC, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE1, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPC440ItinerariesFU::P440_IEXE2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 3, PPC440ItinerariesFU::P440_IWB, -1, (llvm::InstrStage::ReservationKinds)0 }, // 81-85
  { 3, G3ItinerariesFU::G3_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 86
  { 1, G3ItinerariesFU::G3_IU1 | G3ItinerariesFU::G3_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 87
  { 1, G3ItinerariesFU::G3_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 88
  { 1, G3ItinerariesFU::G3_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 89
  { 2, G3ItinerariesFU::G3_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 90
  { 19, G3ItinerariesFU::G3_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 91
  { 1, G3ItinerariesFU::G3_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 92
  { 31, G3ItinerariesFU::G3_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 93
  { 17, G3ItinerariesFU::G3_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 94
  { 2, G3ItinerariesFU::G3_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 95
  { 2, G3ItinerariesFU::G3_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 96
  { 34, G3ItinerariesFU::G3_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 97
  { 3, G3ItinerariesFU::G3_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 98
  { 3, G3ItinerariesFU::G3_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 99
  { 5, G3ItinerariesFU::G3_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 100
  { 6, G3ItinerariesFU::G3_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 101
  { 3, G3ItinerariesFU::G3_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 102
  { 8, G3ItinerariesFU::G3_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 103
  { 2, G3ItinerariesFU::G3_IU1 | G3ItinerariesFU::G3_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 104
  { 8, G4ItinerariesFU::G4_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 105
  { 1, G4ItinerariesFU::G4_IU1 | G4ItinerariesFU::G4_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 106
  { 1, G4ItinerariesFU::G4_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 107
  { 4, G4ItinerariesFU::G4_VFPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 108
  { 1, G4ItinerariesFU::G4_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 109
  { 2, G4ItinerariesFU::G4_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 110
  { 19, G4ItinerariesFU::G4_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 111
  { 1, G4ItinerariesFU::G4_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 112
  { 31, G4ItinerariesFU::G4_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 113
  { 1, G4ItinerariesFU::G4_VIU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 114
  { 3, G4ItinerariesFU::G4_VIU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 115
  { 17, G4ItinerariesFU::G4_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 116
  { 2, G4ItinerariesFU::G4_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 117
  { 3, G4ItinerariesFU::G4_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 118
  { 34, G4ItinerariesFU::G4_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 119
  { 3, G4ItinerariesFU::G4_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 120
  { 3, G4ItinerariesFU::G4_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 121
  { 5, G4ItinerariesFU::G4_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 122
  { 6, G4ItinerariesFU::G4_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 123
  { 3, G4ItinerariesFU::G4_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 124
  { 1, G4ItinerariesFU::G4_VPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 125
  { 5, G4ItinerariesFU::G4_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 126
  { 8, G4ItinerariesFU::G4_SRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 127
  { 2, G4ItinerariesFU::G4_IU1 | G4ItinerariesFU::G4_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 128
  { 35, G4PlusItinerariesFU::G4P_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 129
  { 1, G4PlusItinerariesFU::G4P_IU1 | G4PlusItinerariesFU::G4P_IU2 | G4PlusItinerariesFU::G4P_IU3 | G4PlusItinerariesFU::G4P_IU4, -1, (llvm::InstrStage::ReservationKinds)0 }, // 130
  { 1, G4PlusItinerariesFU::G4P_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 131
  { 4, G4PlusItinerariesFU::G4P_VFPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 132
  { 2, G4PlusItinerariesFU::G4P_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 133
  { 3, G4PlusItinerariesFU::G4P_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 134
  { 23, G4PlusItinerariesFU::G4P_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 135
  { 5, G4PlusItinerariesFU::G4P_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 136
  { 35, G4PlusItinerariesFU::G4P_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 137
  { 1, G4PlusItinerariesFU::G4P_VIU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 138
  { 4, G4PlusItinerariesFU::G4P_VIU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 139
  { 21, G4PlusItinerariesFU::G4P_FPU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 140
  { 3, G4PlusItinerariesFU::G4P_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 141
  { 0, G4PlusItinerariesFU::G4P_IU1 | G4PlusItinerariesFU::G4P_IU2 | G4PlusItinerariesFU::G4P_IU3 | G4PlusItinerariesFU::G4P_IU4, -1, (llvm::InstrStage::ReservationKinds)0 }, // 142
  { 4, G4PlusItinerariesFU::G4P_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 143
  { 37, G4PlusItinerariesFU::G4P_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 144
  { 4, G4PlusItinerariesFU::G4P_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 145
  { 5, G4PlusItinerariesFU::G4P_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 146
  { 2, G4PlusItinerariesFU::G4P_VPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 147
  { 2, G4PlusItinerariesFU::G4P_IU1 | G4PlusItinerariesFU::G4P_IU2 | G4PlusItinerariesFU::G4P_IU3 | G4PlusItinerariesFU::G4P_IU4, -1, (llvm::InstrStage::ReservationKinds)0 }, // 148
  { 2, G4PlusItinerariesFU::G4P_VFPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 149
  { 35, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 150
  { 2, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 151
  { 1, G5ItinerariesFU::G5_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 152
  { 8, G5ItinerariesFU::G5_VFPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 153
  { 3, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 154
  { 4, G5ItinerariesFU::G5_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 155
  { 3, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 156
  { 68, G5ItinerariesFU::G5_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 157
  { 36, G5ItinerariesFU::G5_IU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 158
  { 6, G5ItinerariesFU::G5_FPU1 | G5ItinerariesFU::G5_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 159
  { 33, G5ItinerariesFU::G5_FPU1 | G5ItinerariesFU::G5_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 160
  { 8, G5ItinerariesFU::G5_FPU1 | G5ItinerariesFU::G5_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 161
  { 2, G5ItinerariesFU::G5_VIU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 162
  { 5, G5ItinerariesFU::G5_VIU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 163
  { 40, G5ItinerariesFU::G5_FPU1 | G5ItinerariesFU::G5_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 164
  { 40, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 165
  { 11, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 166
  { 5, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 167
  { 64, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 168
  { 7, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 169
  { 2, G5ItinerariesFU::G5_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 170
  { 3, G5ItinerariesFU::G5_BPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 171
  { 2, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 172
  { 3, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 173
  { 6, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 174
  { 10, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 175
  { 8, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 176
  { 5, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 177
  { 4, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 178
  { 3, G5ItinerariesFU::G5_VPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 179
  { 4, G5ItinerariesFU::G5_SLU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 180
  { 1, G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 181
  { 1, G5ItinerariesFU::G5_IU1 | G5ItinerariesFU::G5_IU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 182
  { 2, G5ItinerariesFU::G5_VFPU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 183
  { 1, PPCA2ItinerariesFU::A2_XU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 184
  { 1, PPCA2ItinerariesFU::A2_FU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 185
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 186-187
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_SU0 | PPCE500ItinerariesFU::E500_SU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 188-189
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_BU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 190-191
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_MU, 0, (llvm::InstrStage::ReservationKinds)0 },   { 14, PPCE500ItinerariesFU::E500_MU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 192-194
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 6, PPCE500ItinerariesFU::E500_MU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 195-196
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 32, PPCE500ItinerariesFU::E500_MU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 197-198
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE500ItinerariesFU::E500_MU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 199-200
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_SU0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 201-202
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 29, PPCE500ItinerariesFU::E500_MU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 203-204
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 3, PPCE500ItinerariesFU::E500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 205-206
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_SU0 | PPCE500ItinerariesFU::E500_SU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 207-209
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 5, PPCE500ItinerariesFU::E500_SU0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 210-211
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE500ItinerariesFU::E500_SU0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 212-213
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE500ItinerariesFU::E500_SU0 | PPCE500ItinerariesFU::E500_SU1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 214-215
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500ItinerariesFU::E500_LSU_0, 0, (llvm::InstrStage::ReservationKinds)0 }, // 216-217
  { 1, PPCE500ItinerariesFU::E500_DIS0 | PPCE500ItinerariesFU::E500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE500ItinerariesFU::E500_SU0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 218-219
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 220-221
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_SFX0 | PPCE500mcItinerariesFU::E500mc_SFX1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 222-223
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_BU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 224-225
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_CFX_0, 0, (llvm::InstrStage::ReservationKinds)0 },   { 14, PPCE500mcItinerariesFU::E500mc_CFX_DivBypass, -1, (llvm::InstrStage::ReservationKinds)0 }, // 226-228
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE500mcItinerariesFU::E500mc_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 229-230
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 68, PPCE500mcItinerariesFU::E500mc_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 231-232
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE500mcItinerariesFU::E500mc_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 233-234
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 38, PPCE500mcItinerariesFU::E500mc_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 235-236
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 3, PPCE500mcItinerariesFU::E500mc_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 237-238
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_SFX0 | PPCE500mcItinerariesFU::E500mc_SFX1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 239-241
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 5, PPCE500mcItinerariesFU::E500mc_SFX0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 242-243
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 8, PPCE500mcItinerariesFU::E500mc_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 244-245
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE500mcItinerariesFU::E500mc_SFX0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 246-247
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE500mcItinerariesFU::E500mc_SFX0 | PPCE500mcItinerariesFU::E500mc_SFX1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 248-249
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_SFX0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 250-251
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_CFX_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 252-253
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE500mcItinerariesFU::E500mc_LSU_0, 0, (llvm::InstrStage::ReservationKinds)0 }, // 254-255
  { 1, PPCE500mcItinerariesFU::E500mc_DIS0 | PPCE500mcItinerariesFU::E500mc_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE500mcItinerariesFU::E500mc_SFX0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 256-257
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 258-259
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_SFX0 | PPCE5500ItinerariesFU::E5500_SFX1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 260-261
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_BU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 262-263
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_0, 0, (llvm::InstrStage::ReservationKinds)0 },   { 26, PPCE5500ItinerariesFU::E5500_CFX_DivBypass, -1, (llvm::InstrStage::ReservationKinds)0 }, // 264-266
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_0, 0, (llvm::InstrStage::ReservationKinds)0 },   { 16, PPCE5500ItinerariesFU::E5500_CFX_DivBypass, -1, (llvm::InstrStage::ReservationKinds)0 }, // 267-269
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 270-271
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 31, PPCE5500ItinerariesFU::E5500_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 272-273
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 16, PPCE5500ItinerariesFU::E5500_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 274-275
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 3, PPCE5500ItinerariesFU::E5500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 276-277
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_SFX0 | PPCE5500ItinerariesFU::E5500_SFX1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 278-280
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE5500ItinerariesFU::E5500_LSU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 281-282
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_0, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE5500ItinerariesFU::E5500_CFX_1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 283-285
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 286-287
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 5, PPCE5500ItinerariesFU::E5500_CFX_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 288-289
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 4, PPCE5500ItinerariesFU::E5500_CFX_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 290-291
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 7, PPCE5500ItinerariesFU::E5500_FPU_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 292-293
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE5500ItinerariesFU::E5500_CFX_0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 294-295
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_0, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_CFX_1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 296-298
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE5500ItinerariesFU::E5500_SFX0 | PPCE5500ItinerariesFU::E5500_SFX1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 299-300
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, PPCE5500ItinerariesFU::E5500_LSU_0, 0, (llvm::InstrStage::ReservationKinds)0 }, // 301-302
  { 1, PPCE5500ItinerariesFU::E5500_DIS0 | PPCE5500ItinerariesFU::E5500_DIS1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 2, PPCE5500ItinerariesFU::E5500_SFX0, -1, (llvm::InstrStage::ReservationKinds)0 }, // 303-304
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2 | P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 305-306
  { 1, P8ItinerariesFU::P8_DU7 | P8ItinerariesFU::P8_DU8, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_BRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 307-308
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FPU1 | P8ItinerariesFU::P8_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 309-310
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 311-312
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_CRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 313-314
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 315-316
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 23, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 317-318
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 15, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 319-320
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FPU1 | P8ItinerariesFU::P8_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 321-322
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 323-325
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_BRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 326-328
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 329-333
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 334-337
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU5, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 338-344
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 345-346
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 347-350
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 351-354
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 355-359
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU5, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 360-368
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2 | P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 369-372
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_CRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 373-374
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 375-376
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FPU1 | P8ItinerariesFU::P8_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 377-378
  { 1, P8ItinerariesFU::P8_DU1 | P8ItinerariesFU::P8_DU2 | P8ItinerariesFU::P8_DU3 | P8ItinerariesFU::P8_DU4 | P8ItinerariesFU::P8_DU5 | P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FPU1 | P8ItinerariesFU::P8_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 379-381
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FPU1 | P8ItinerariesFU::P8_FPU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 382-386
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU5, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 387-394
  { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2 | P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2 | P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2 | P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 395-397
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LU1 | P8ItinerariesFU::P8_LU2 | P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 398-401
  { 1, P8ItinerariesFU::P8_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU5, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_LSU1 | P8ItinerariesFU::P8_LSU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P8ItinerariesFU::P8_FXU1 | P8ItinerariesFU::P8_FXU2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 402-410
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2 | P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 411-412
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 413-414
  { 1, P7ItinerariesFU::P7_DU5 | P7ItinerariesFU::P7_DU6, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_BRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 415-416
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS1 | P7ItinerariesFU::P7_VS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 417-418
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_CRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 419-420
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 421-422
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 68, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 423-425
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 36, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 426-428
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS1 | P7ItinerariesFU::P7_VS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 429-430
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 431-432
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 433-435
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_BRU, -1, (llvm::InstrStage::ReservationKinds)0 }, // 436-438
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 439-443
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 444-447
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 448-454
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 455-458
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 459-463
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 464-471
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_CRU, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 472-477
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1, -1, (llvm::InstrStage::ReservationKinds)0 }, // 478-479
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 480-481
  { 1, P7ItinerariesFU::P7_DU1 | P7ItinerariesFU::P7_DU2 | P7ItinerariesFU::P7_DU3 | P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS1 | P7ItinerariesFU::P7_VS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 482-484
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_VS1 | P7ItinerariesFU::P7_VS2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 485-489
  { 1, P7ItinerariesFU::P7_DU1, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU3, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_DU4, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_LS1 | P7ItinerariesFU::P7_LS2, 0, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 },   { 1, P7ItinerariesFU::P7_FX1 | P7ItinerariesFU::P7_FX2, -1, (llvm::InstrStage::ReservationKinds)0 }, // 490-496
  { 0, 0, 0, llvm::InstrStage::Required } // End stages
};
extern const unsigned PPCOperandCycles[] = {
  0, // No itinerary
  2,   0,   0, // 1-3
  2,   0,   0, // 4-6
  4,   0, // 7-8
  2,   0,   0, // 9-11
  4,   0,   0, // 12-14
  1,   1, // 15-16
  5,   1,   1, // 17-19
  36,   0,   0, // 20-22
  6,   0,   0, // 23-25
  31,   0,   0, // 26-28
  6,   0,   0, // 29-31
  6,   0,   0, // 32-34
  1,   1,   1, // 35-37
  19,   0,   0, // 38-40
  6,   0,   0,   0, // 41-44
  4,   1,   1, // 45-47
  2,   0,   0,   0, // 48-51
  4,   1,   1, // 52-54
  5,   2,   1,   1, // 55-58
  5,   2,   1,   1, // 59-62
  5,   1,   1, // 63-65
  5,   2,   1,   1, // 66-69
  5,   2,   1,   1, // 70-73
  4,   1,   1, // 74-76
  4,   1,   1, // 77-79
  4,   1,   1, // 80-82
  4,   1,   1, // 83-85
  4,   0,   0, // 86-88
  4,   0,   0, // 89-91
  4,   0, // 92-93
  6,   0, // 94-95
  3,   0,   0, // 96-98
  3,   0, // 99-100
  2,   0, // 101-102
  6,   0, // 103-104
  6,   0, // 105-106
  3,   0,   0, // 107-109
  2,   0, // 110-111
  5,   0, // 112-113
  4,   0,   0, // 114-116
  4,   0,   0, // 117-119
  4,   0,   0, // 120-122
  1,   1,   1, // 123-125
  2,   1,   1,   1, // 126-129
  4,   0, // 130-131
  2,   0,   0, // 132-134
  2,   0,   0, // 135-137
  4,   1,   1, // 138-140
  2,   1,   1,   1, // 141-144
  4,   1,   1, // 145-147
  4,   1,   1, // 148-150
  2,   1,   1,   1, // 151-154
  2,   1,   1,   1, // 155-158
  2,   0, // 159-160
  6,   0, // 161-162
  3,   0,   0, // 163-165
  3,   0,   0, // 166-168
  2,   0,   0, // 169-171
  2,   0, // 172-173
  6,   0,   0, // 174-176
  6,   0,   0, // 177-179
  2,   0,   0, // 180-182
  2,   0,   0, // 183-185
  2,   0,   0, // 186-188
  2,   0,   0, // 189-191
  4,   0,   0, // 192-194
  4,   0,   0, // 195-197
  4,   0, // 198-199
  6,   0, // 200-201
  5,   1,   1, // 202-204
  2,   0,   0, // 205-207
  4,   0,   0, // 208-210
  4,   0,   0, // 211-213
  2,   0,   0, // 214-216
  6,   0,   0, // 217-219
  2,   0,   0, // 220-222
  2,   0,   0,   0, // 223-226
  6,   0, // 227-228
  1,   1,   1, // 229-231
  5,   1,   1, // 232-234
  6,   0,   0, // 235-237
  4,   0,   0, // 238-240
  4,   0,   0, // 241-243
  4,   0,   0, // 244-246
  6,   0,   0, // 247-249
  6,   0,   0, // 250-252
  6,   0,   0,   0, // 253-256
  6,   0,   0, // 257-259
  4,   0,   0, // 260-262
  4,   0,   0, // 263-265
  6,   0,   0, // 266-268
  6,   0,   0, // 269-271
  6,   0,   0,   0, // 272-275
  5,   1,   1, // 276-278
  6,   0,   0, // 279-281
  5,   1,   1, // 282-284
  5,   1,   1, // 285-287
  5,   1,   1, // 288-290
  1,   1, // 291-292
  4,   1,   1, // 293-295
  5,   1,   1, // 296-298
  4,   1,   1, // 299-301
  4,   1,   1, // 302-304
  5,   1,   1, // 305-307
  5,   2,   1,   1, // 308-311
  5,   2,   1,   1, // 312-315
  6,   0, // 316-317
  4,   1,   1, // 318-320
  4,   1,   1, // 321-323
  4,   1,   1, // 324-326
  6,   0, // 327-328
  5,   1,   1, // 329-331
  5,   1,   1, // 332-334
  5,   1,   1, // 335-337
  5,   1,   1, // 338-340
  4,   1,   1, // 341-343
  4,   1,   1, // 344-346
  5,   1,   1, // 347-349
  1,   1,   1, // 350-352
  1,   1,   1, // 353-355
  4,   1,   1, // 356-358
  5,   1,   1, // 359-361
  4,   1,   1, // 362-364
  1,   1,   1, // 365-367
  1,   1,   1, // 368-370
  5,   1,   1, // 371-373
  6,   0, // 374-375
  2,   0,   0, // 376-378
  6,   0, // 379-380
  6,   0, // 381-382
  3,   0, // 383-384
  2,   0, // 385-386
  6,   0, // 387-388
  36,   0,   0, // 389-391
  36,   0,   0, // 392-394
  36,   0,   0, // 395-397
  36,   0,   0, // 398-400
  4,   0,   0, // 401-403
  2,   0,   0, // 404-406
  6,   0,   0, // 407-409
  4,   0,   0, // 410-412
  3,   0,   0, // 413-415
  3,   0,   0, // 416-418
  2,   0,   0, // 419-421
  2,   0,   0, // 422-424
  2,   0,   0, // 425-427
  3,   0,   0, // 428-430
  4,   0, // 431-432
  31,   0,   0, // 433-435
  31,   0,   0, // 436-438
  31,   0,   0, // 439-441
  19,   0,   0, // 442-444
  19,   0,   0, // 445-447
  19,   0,   0, // 448-450
  19,   0,   0, // 451-453
  31,   0,   0, // 454-456
  5,   2,   1,   1, // 457-460
  5,   2,   1,   1, // 461-464
  2,   1,   1,   1, // 465-468
  2,   1,   1,   1, // 469-472
  2,   1,   1,   1, // 473-476
  2,   1,   1,   1, // 477-480
  5,   2,   1,   1, // 481-484
  5,   2,   1,   1, // 485-488
  5,   2,   1,   1, // 489-492
  5,   2,   1,   1, // 493-496
  4,   0, // 497-498
  4,   0, // 499-500
  5,   1,   1, // 501-503
  1,   1,   1, // 504-506
  2,   0,   0, // 507-509
  5,   1,   1, // 510-512
  1,   1,   1, // 513-515
  4,   0, // 516-517
  4,   0, // 518-519
  5,   1,   1, // 520-522
  1,   1,   1, // 523-525
  5,   1,   1, // 526-528
  1,   1,   1, // 529-531
  2,   0, // 532-533
  5,   0, // 534-535
  6,   0, // 536-537
  6,   0, // 538-539
  2,   0,   0, // 540-542
  2,   0, // 543-544
  1,   1, // 545-546
  6, // 547
  1,   0,   0, // 548-550
  2,   0,   0, // 551-553
  6,   0,   0, // 554-556
  2,   0,   0, // 557-559
  1,   0,   0, // 560-562
  1,   0,   0, // 563-565
  6,   0,   0, // 566-568
  71,   0,   0, // 569-571
  39,   0,   0, // 572-574
  6,   0,   0, // 575-577
  72,   0,   0, // 578-580
  5,   0,   0, // 581-583
  6,   0,   0, // 584-586
  0,   0,   0, // 587-589
  2,   0,   0, // 590-592
  59,   0,   0, // 593-595
  6,   0,   0,   0, // 596-599
  69,   0,   0, // 600-602
  65,   0,   0, // 603-605
  16,   0,   0, // 606-608
  2,   0,   0,   0, // 609-612
  16, // 613
  82,   0,   0, // 614-616
  6,   8,   0,   0, // 617-620
  6,   8,   0,   0, // 621-624
  6,   0,   0, // 625-627
  6,   0,   0, // 628-630
  7,   0,   0, // 631-633
  7,   9,   0,   0, // 634-637
  7,   9,   0,   0, // 638-641
  6,   0,   0, // 642-644
  6,   8,   0,   0, // 645-648
  6,   8,   0,   0, // 649-652
  5,   0,   0, // 653-655
  1,   0,   0, // 656-658
  6,   0, // 659-660
  6,   0, // 661-662
  4,   0, // 663-664
  1,   0, // 665-666
  4,   0, // 667-668
  6,   0, // 669-670
  16,   0, // 671-672
  5,   0,   0, // 673-675
  5,   0,   0, // 676-678
  6,   0,   0, // 679-681
  0,   0,   0, // 682-684
  2,   0,   0,   0, // 685-688
  16, // 689
  2,   0,   0, // 690-692
  2,   0,   0, // 693-695
  2,   0,   0, // 696-698
  82,   0,   0, // 699-701
  2,   0,   0,   0, // 702-705
  0,   0,   0, // 706-708
  82,   0,   0, // 709-711
  2,   0,   0,   0, // 712-715
  2,   0,   0,   0, // 716-719
  2,   0, // 720-721
  2,   0, // 722-723
  6,   0, // 724-725
  2,   0,   0, // 726-728
  2,   0, // 729-730
  2,   0, // 731-732
  5,   0,   0, // 733-735
  5,   0,   0, // 736-738
  2,   0,   0, // 739-741
  2,   0,   0, // 742-744
  2,   0,   0, // 745-747
  2,   0,   0, // 748-750
  2,   0,   0, // 751-753
  1,   0,   0, // 754-756
  5,   0,   0, // 757-759
  1,   0,   0, // 760-762
  6,   0,   0, // 763-765
  6,   0, // 766-767
  6,   0,   0, // 768-770
  2,   0,   0, // 771-773
  2,   0,   0, // 774-776
  2,   0,   0, // 777-779
  1,   0, // 780-781
  1,   0,   0, // 782-784
  1,   0,   0, // 785-787
  2,   0,   0, // 788-790
  6,   0,   0, // 791-793
  2,   0,   0, // 794-796
  2,   0,   0,   0, // 797-800
  6,   0, // 801-802
  0,   0,   0, // 803-805
  6,   0,   0, // 806-808
  5,   0,   0, // 809-811
  5,   0,   0, // 812-814
  5,   0,   0, // 815-817
  6,   0,   0, // 818-820
  6,   0,   0, // 821-823
  6,   0,   0, // 824-826
  6,   0,   0,   0, // 827-830
  6,   0,   0, // 831-833
  5,   0,   0, // 834-836
  5,   0,   0, // 837-839
  6,   0,   0, // 840-842
  6,   0,   0, // 843-845
  6,   0,   0,   0, // 846-849
  6,   0,   0, // 850-852
  5,   0,   0, // 853-855
  6,   0,   0, // 856-858
  6,   0,   0, // 859-861
  7,   0,   0, // 862-864
  1,   0,   0, // 865-867
  16,   0,   0, // 868-870
  6,   0,   0, // 871-873
  82,   0,   0, // 874-876
  16, // 877
  6, // 878
  7,   0,   0, // 879-881
  6,   8,   0,   0, // 882-885
  6,   8,   0,   0, // 886-889
  6,   0, // 890-891
  82,   0,   0, // 892-894
  82,   0,   0, // 895-897
  6,   0,   0, // 898-900
  6,   0, // 901-902
  7,   0,   0, // 903-905
  7,   0,   0, // 906-908
  7,   0,   0, // 909-911
  7,   0,   0, // 912-914
  6,   8,   0,   0, // 915-918
  6,   8,   0,   0, // 919-922
  7,   0,   0, // 923-925
  0,   0,   0, // 926-928
  0,   0,   0, // 929-931
  0,   0,   0, // 932-934
  6,   0,   0, // 935-937
  0,   0,   0, // 938-940
  0,   0,   0, // 941-943
  6,   0,   0, // 944-946
  6,   0, // 947-948
  2,   0,   0, // 949-951
  4,   0, // 952-953
  6,   0, // 954-955
  4,   0, // 956-957
  16,   0, // 958-959
  6,   0, // 960-961
  39,   0,   0, // 962-964
  39,   0,   0, // 965-967
  71,   0,   0, // 968-970
  71,   0,   0, // 971-973
  39,   0,   0, // 974-976
  71,   0,   0, // 977-979
  39,   0,   0, // 980-982
  71,   0,   0, // 983-985
  1,   0,   0, // 986-988
  2,   0,   0, // 989-991
  6,   0,   0, // 992-994
  5,   0,   0, // 995-997
  2,   0,   0, // 998-1000
  2,   0,   0, // 1001-1003
  2,   0,   0, // 1004-1006
  2,   0,   0, // 1007-1009
  2,   0,   0, // 1010-1012
  6,   0, // 1013-1014
  2,   0,   0, // 1015-1017
  2,   0,   0, // 1018-1020
  72,   0,   0, // 1021-1023
  72,   0,   0, // 1024-1026
  69,   0,   0, // 1027-1029
  69,   0,   0, // 1030-1032
  69,   0,   0, // 1033-1035
  65,   0,   0, // 1036-1038
  69,   0,   0, // 1039-1041
  65,   0,   0, // 1042-1044
  65,   0,   0, // 1045-1047
  65,   0,   0, // 1048-1050
  72,   0,   0, // 1051-1053
  59,   0,   0, // 1054-1056
  59,   0,   0, // 1057-1059
  59,   0,   0, // 1060-1062
  59,   0,   0, // 1063-1065
  72,   0,   0, // 1066-1068
  7,   9,   0,   0, // 1069-1072
  7,   9,   0,   0, // 1073-1076
  2,   0,   0,   0, // 1077-1080
  2,   0,   0,   0, // 1081-1084
  2,   0,   0,   0, // 1085-1088
  2,   0,   0,   0, // 1089-1092
  6,   8,   0,   0, // 1093-1096
  6,   8,   0,   0, // 1097-1100
  6,   0,   0, // 1101-1103
  6,   0,   0, // 1104-1106
  7,   9,   0,   0, // 1107-1110
  7,   9,   0,   0, // 1111-1114
  6,   0,   0, // 1115-1117
  6,   0,   0, // 1118-1120
  6,   0,   0, // 1121-1123
  0,   0,   0, // 1124-1126
  1,   0,   0, // 1127-1129
  6,   0,   0, // 1130-1132
  0,   0,   0, // 1133-1135
  6,   0,   0, // 1136-1138
  16, // 1139
  6,   0,   0, // 1140-1142
  0,   0,   0, // 1143-1145
  7,   0,   0, // 1146-1148
  0,   0,   0, // 1149-1151
  6,   0, // 1152-1153
  6,   0, // 1154-1155
  1,   0,   0, // 1156-1158
  16,   0, // 1159-1160
  6, // 1161
  1,   0,   0, // 1162-1164
  4,   1,   1, // 1165-1167
  4,   1,   1, // 1168-1170
  4,   1, // 1171-1172
  5,   1,   1, // 1173-1175
  4,   1,   1, // 1176-1178
  6,   1, // 1179-1180
  6,   1, // 1181-1182
  17,   1,   1, // 1183-1185
  9,   1,   1, // 1186-1188
  35,   1,   1, // 1189-1191
  7,   1,   1, // 1192-1194
  4,   1,   1, // 1195-1197
  7,   1,   1, // 1198-1200
  6,   1, // 1201-1202
  32,   1,   1, // 1203-1205
  6,   1, // 1206-1207
  4,   1,   1,   1, // 1208-1211
  6,   1,   1, // 1212-1214
  6,   1, // 1215-1216
  6,   1, // 1217-1218
  6,   1, // 1219-1220
  6,   1, // 1221-1222
  6,   1, // 1223-1224
  7,   1, // 1225-1226
  4,   1, // 1227-1228
  4,   1,   1, // 1229-1231
  8,   1, // 1232-1233
  4,   1, // 1234-1235
  7,   1, // 1236-1237
  8,   1, // 1238-1239
  7,   1, // 1240-1241
  7,   1, // 1242-1243
  7,   1, // 1244-1245
  4,   1, // 1246-1247
  5,   1, // 1248-1249
  4,   1, // 1250-1251
  5,   1, // 1252-1253
  7,   1,   1, // 1254-1256
  7,   1,   1, // 1257-1259
  7,   1,   1, // 1260-1262
  4,   1,   1, // 1263-1265
  4,   1,   1, // 1266-1268
  6,   1, // 1269-1270
  6,   1, // 1271-1272
  5,   1, // 1273-1274
  4,   1,   1, // 1275-1277
  4,   1, // 1278-1279
  5,   1,   1, // 1280-1282
  5,   1, // 1283-1284
  4,   1,   1, // 1285-1287
  4,   1,   1, // 1288-1290
  5,   1,   1, // 1291-1293
  4,   1,   1, // 1294-1296
  4,   1,   1, // 1297-1299
  4,   1, // 1300-1301
  4,   1,   1, // 1302-1304
  4,   1, // 1305-1306
  4,   1, // 1307-1308
  6,   1, // 1309-1310
  4,   1,   1, // 1311-1313
  8,   1, // 1314-1315
  4,   1,   1, // 1316-1318
  4,   1,   1, // 1319-1321
  4,   1,   1, // 1322-1324
  4,   1,   1, // 1325-1327
  4,   1,   1,   1, // 1328-1331
  4,   1, // 1332-1333
  6,   1, // 1334-1335
  6,   1, // 1336-1337
  4,   1,   1, // 1338-1340
  4,   1,   1, // 1341-1343
  7,   1,   1, // 1344-1346
  7,   1,   1, // 1347-1349
  7,   1,   1, // 1350-1352
  7,   1,   1, // 1353-1355
  7,   1,   1, // 1356-1358
  6,   1, // 1359-1360
  4,   1,   1, // 1361-1363
  6,   1, // 1364-1365
  6,   1, // 1366-1367
  6,   1, // 1368-1369
  6,   1, // 1370-1371
  6,   1, // 1372-1373
  6,   1,   1, // 1374-1376
  7,   1, // 1377-1378
  6,   1, // 1379-1380
  6,   1, // 1381-1382
  4,   1, // 1383-1384
  6,   1, // 1385-1386
  6,   1, // 1387-1388
  4,   1, // 1389-1390
  6,   1, // 1391-1392
  6,   1, // 1393-1394
  6,   1, // 1395-1396
  6,   1, // 1397-1398
  7,   1, // 1399-1400
  6,   1, // 1401-1402
  6,   1, // 1403-1404
  4,   1, // 1405-1406
  4,   1,   1, // 1407-1409
  7,   1, // 1410-1411
  4,   1, // 1412-1413
  7,   1, // 1414-1415
  4,   1, // 1416-1417
  7,   1, // 1418-1419
  5,   1, // 1420-1421
  4,   1, // 1422-1423
  17,   1,   1, // 1424-1426
  17,   1,   1, // 1427-1429
  17,   1,   1, // 1430-1432
  17,   1,   1, // 1433-1435
  4,   1,   1, // 1436-1438
  4,   1,   1, // 1439-1441
  4,   1, // 1442-1443
  4,   1,   1, // 1444-1446
  4,   1,   1, // 1447-1449
  4,   1,   1, // 1450-1452
  8,   1, // 1453-1454
  35,   1,   1, // 1455-1457
  35,   1,   1, // 1458-1460
  35,   1,   1, // 1461-1463
  32,   1,   1, // 1464-1466
  32,   1,   1, // 1467-1469
  32,   1,   1, // 1470-1472
  32,   1,   1, // 1473-1475
  35,   1,   1, // 1476-1478
  6,   1, // 1479-1480
  6,   1, // 1481-1482
  6,   1, // 1483-1484
  4,   1, // 1485-1486
  4,   1, // 1487-1488
  6,   1, // 1489-1490
  6,   1, // 1491-1492
  4,   1,   1, // 1493-1495
  4,   1,   1, // 1496-1498
  7,   1,   1, // 1499-1501
  6,   1, // 1502-1503
  6,   1, // 1504-1505
  4,   1, // 1506-1507
  6,   1, // 1508-1509
  6,   1, // 1510-1511
  7,   1, // 1512-1513
  5,   1, // 1514-1515
  4,   1, // 1516-1517
  4,   1, // 1518-1519
  4,   1,   1, // 1520-1522
  5,   1, // 1523-1524
  6,   1, // 1525-1526
  4,   1,   1, // 1527-1529
  4,   1,   1, // 1530-1532
  4,   1, // 1533-1534
  5,   1,   1, // 1535-1537
  4,   1,   1, // 1538-1540
  6,   1, // 1541-1542
  6,   1, // 1543-1544
  17,   1,   1, // 1545-1547
  13,   1,   1, // 1548-1550
  71,   1,   1, // 1551-1553
  11,   1,   1, // 1554-1556
  11,   1,   1, // 1557-1559
  6,   1, // 1560-1561
  41,   1,   1, // 1562-1564
  13,   1,   1,   1, // 1565-1568
  6,   1, // 1569-1570
  4,   1,   1,   1, // 1571-1574
  6,   1,   1, // 1575-1577
  6,   1, // 1578-1579
  6,   1, // 1580-1581
  7,   1,   1, // 1582-1584
  7,   1,   1, // 1585-1587
  7,   1,   1, // 1588-1590
  6,   1, // 1591-1592
  6,   1, // 1593-1594
  6,   1, // 1595-1596
  7,   1, // 1597-1598
  4,   1, // 1599-1600
  4,   1,   1, // 1601-1603
  8,   1, // 1604-1605
  4,   1, // 1606-1607
  11, // 1608
  7,   1, // 1609-1610
  8,   1, // 1611-1612
  7,   1, // 1613-1614
  7,   1, // 1615-1616
  7,   1, // 1617-1618
  4,   1, // 1619-1620
  11,   1,   1, // 1621-1623
  5,   1, // 1624-1625
  4,   1, // 1626-1627
  5,   1, // 1628-1629
  7,   1,   1, // 1630-1632
  7,   1,   1, // 1633-1635
  7,   1,   1, // 1636-1638
  6,   1,   1, // 1639-1641
  6,   1,   1, // 1642-1644
  4,   1,   1, // 1645-1647
  4,   1,   1, // 1648-1650
  6,   1, // 1651-1652
  6,   1, // 1653-1654
  5,   1, // 1655-1656
  4,   1, // 1657-1658
  11,   1,   1, // 1659-1661
  11, // 1662
  5,   1,   1, // 1663-1665
  5,   1, // 1666-1667
  11,   1,   1, // 1668-1670
  11,   1,   1, // 1671-1673
  4,   1,   1, // 1674-1676
  5,   1,   1, // 1677-1679
  4,   1,   1, // 1680-1682
  4,   1,   1, // 1683-1685
  4,   1, // 1686-1687
  4,   1,   1, // 1688-1690
  4,   1, // 1691-1692
  4,   1, // 1693-1694
  6,   1, // 1695-1696
  4,   1,   1, // 1697-1699
  8,   1, // 1700-1701
  4,   1,   1, // 1702-1704
  4,   1,   1, // 1705-1707
  4,   1,   1, // 1708-1710
  11,   1,   1, // 1711-1713
  4,   1,   1, // 1714-1716
  4,   1,   1,   1, // 1717-1720
  4,   1, // 1721-1722
  6,   1, // 1723-1724
  6,   1, // 1725-1726
  11,   1,   1, // 1727-1729
  7,   1,   1, // 1730-1732
  7,   1,   1, // 1733-1735
  7,   1,   1, // 1736-1738
  11,   1,   1, // 1739-1741
  13,   1,   1, // 1742-1744
  13,   1,   1,   1, // 1745-1748
  11,   1,   1, // 1749-1751
  7,   1,   1, // 1752-1754
  7,   1,   1, // 1755-1757
  11,   1,   1, // 1758-1760
  13,   1,   1, // 1761-1763
  13,   1,   1,   1, // 1764-1767
  6,   1, // 1768-1769
  11,   1,   1, // 1770-1772
  6,   1, // 1773-1774
  6,   1, // 1775-1776
  7,   1,   1, // 1777-1779
  6,   1, // 1780-1781
  6,   1, // 1782-1783
  6,   1, // 1784-1785
  6,   1,   1, // 1786-1788
  7,   1, // 1789-1790
  7,   1,   1, // 1791-1793
  6,   1, // 1794-1795
  6,   1, // 1796-1797
  4,   1, // 1798-1799
  6,   1, // 1800-1801
  6,   1, // 1802-1803
  4,   1, // 1804-1805
  7,   1,   1, // 1806-1808
  7,   1,   1, // 1809-1811
  7,   1,   1, // 1812-1814
  7,   1,   1, // 1815-1817
  6,   1, // 1818-1819
  6,   1, // 1820-1821
  7,   1,   1, // 1822-1824
  6,   1,   1, // 1825-1827
  6,   1, // 1828-1829
  6,   1, // 1830-1831
  7,   1, // 1832-1833
  6,   1, // 1834-1835
  6,   1,   1, // 1836-1838
  6,   1, // 1839-1840
  4,   1, // 1841-1842
  4,   1,   1, // 1843-1845
  7,   1, // 1846-1847
  4,   1, // 1848-1849
  7,   1, // 1850-1851
  4,   1, // 1852-1853
  7,   1, // 1854-1855
  5,   1, // 1856-1857
  4,   1, // 1858-1859
  17,   1,   1, // 1860-1862
  17,   1,   1, // 1863-1865
  17,   1,   1, // 1866-1868
  17,   1,   1, // 1869-1871
  4,   1,   1, // 1872-1874
  4,   1,   1, // 1875-1877
  11,   1,   1, // 1878-1880
  4,   1, // 1881-1882
  11, // 1883
  11,   1,   1, // 1884-1886
  4,   1,   1, // 1887-1889
  4,   1,   1, // 1890-1892
  4,   1,   1, // 1893-1895
  11, // 1896
  8,   1, // 1897-1898
  71,   1,   1, // 1899-1901
  71,   1,   1, // 1902-1904
  71,   1,   1, // 1905-1907
  41,   1,   1, // 1908-1910
  41,   1,   1, // 1911-1913
  41,   1,   1, // 1914-1916
  41,   1,   1, // 1917-1919
  71,   1,   1, // 1920-1922
  7,   1,   1, // 1923-1925
  7,   1,   1, // 1926-1928
  6,   1,   1, // 1929-1931
  6,   1, // 1932-1933
  6,   1, // 1934-1935
  6,   1, // 1936-1937
  7,   1,   1, // 1938-1940
  7,   1,   1, // 1941-1943
  4,   1, // 1944-1945
  4,   1, // 1946-1947
  6,   1, // 1948-1949
  6,   1, // 1950-1951
  4,   1,   1, // 1952-1954
  6,   1, // 1955-1956
  6,   1, // 1957-1958
  4,   1, // 1959-1960
  6,   1, // 1961-1962
  6,   1, // 1963-1964
  7,   1,   1, // 1965-1967
  6,   1,   1, // 1968-1970
  7,   1, // 1971-1972
  5,   1, // 1973-1974
  4,   1, // 1975-1976
  4,   1, // 1977-1978
  4,   1,   1, // 1979-1981
  5,   1, // 1982-1983
  6,   1, // 1984-1985
  5,   2,   2, // 1986-1988
  5,   2,   2, // 1989-1991
  5,   2, // 1992-1993
  6,   2,   2, // 1994-1996
  5,   2,   2, // 1997-1999
  7,   2, // 2000-2001
  7,   2, // 2002-2003
  7,   2, // 2004-2005
  30,   2,   2, // 2006-2008
  20,   2,   2, // 2009-2011
  11,   2,   2, // 2012-2014
  39,   2,   2, // 2015-2017
  11,   2,   2, // 2018-2020
  11,   2,   2, // 2021-2023
  7,   2, // 2024-2025
  5,   2,   2, // 2026-2028
  24,   2,   2, // 2029-2031
  11,   2,   2,   2, // 2032-2035
  7,   2, // 2036-2037
  5,   2,   2,   2, // 2038-2041
  7,   2,   2, // 2042-2044
  7,   2, // 2045-2046
  7,   2, // 2047-2048
  7,   2, // 2049-2050
  7,   2, // 2051-2052
  7,   2, // 2053-2054
  8,   2,   2, // 2055-2057
  8,   2,   2, // 2058-2060
  8,   2,   2, // 2061-2063
  7,   2, // 2064-2065
  7,   2, // 2066-2067
  7,   2, // 2068-2069
  8,   2, // 2070-2071
  9,   2,   2, // 2072-2074
  5,   2, // 2075-2076
  5,   2,   2, // 2077-2079
  9,   2, // 2080-2081
  5, // 2082
  11, // 2083
  9,   2, // 2084-2085
  8,   2, // 2086-2087
  8,   2, // 2088-2089
  5, // 2090
  11,   2,   2, // 2091-2093
  6,   2, // 2094-2095
  5, // 2096
  8,   2,   2, // 2097-2099
  8,   2,   2, // 2100-2102
  8,   2,   2, // 2103-2105
  7,   2,   2, // 2106-2108
  7,   2,   2, // 2109-2111
  6,   2,   2, // 2112-2114
  5,   2,   2, // 2115-2117
  6,   2,   2, // 2118-2120
  7,   2, // 2121-2122
  7,   2, // 2123-2124
  7,   2, // 2125-2126
  7,   2, // 2127-2128
  7,   2, // 2129-2130
  7,   2, // 2131-2132
  6,   2, // 2133-2134
  5, // 2135
  11,   2,   2, // 2136-2138
  11, // 2139
  6,   2,   2, // 2140-2142
  6,   2, // 2143-2144
  11,   2,   2, // 2145-2147
  11,   2,   2, // 2148-2150
  5,   2,   2, // 2151-2153
  6,   2,   2, // 2154-2156
  5,   2,   2, // 2157-2159
  6,   2,   2, // 2160-2162
  5,   2,   2, // 2163-2165
  5,   2,   2, // 2166-2168
  5,   2, // 2169-2170
  5,   2,   2, // 2171-2173
  5,   2, // 2174-2175
  5, // 2176
  7,   2, // 2177-2178
  6,   2,   2, // 2179-2181
  5,   2,   2, // 2182-2184
  5,   2,   2, // 2185-2187
  9,   2, // 2188-2189
  5,   2,   2, // 2190-2192
  5,   2,   2, // 2193-2195
  5,   2,   2, // 2196-2198
  11,   2,   2, // 2199-2201
  6,   2,   2, // 2202-2204
  5,   2,   2,   2, // 2205-2208
  5, // 2209
  7,   2, // 2210-2211
  7,   2, // 2212-2213
  11,   2,   2, // 2214-2216
  9,   2,   2, // 2217-2219
  8,   2,   2, // 2220-2222
  8,   2,   2, // 2223-2225
  8,   2,   2, // 2226-2228
  11,   2,   2, // 2229-2231
  11,   2,   2, // 2232-2234
  11,   2,   2,   2, // 2235-2238
  11,   2,   2, // 2239-2241
  8,   2,   2, // 2242-2244
  8,   2,   2, // 2245-2247
  9,   2,   2, // 2248-2250
  11,   2,   2, // 2251-2253
  11,   2,   2, // 2254-2256
  11,   2,   2,   2, // 2257-2260
  7,   2, // 2261-2262
  11,   2,   2, // 2263-2265
  7,   2, // 2266-2267
  7,   2, // 2268-2269
  8,   2,   2, // 2270-2272
  7,   2, // 2273-2274
  7,   2, // 2275-2276
  7,   2, // 2277-2278
  7,   2,   2, // 2279-2281
  7,   2, // 2282-2283
  7,   2, // 2284-2285
  8,   2, // 2286-2287
  8,   2,   2, // 2288-2290
  7,   2, // 2291-2292
  7,   2, // 2293-2294
  5, // 2295
  7,   2, // 2296-2297
  7,   2, // 2298-2299
  7,   2, // 2300-2301
  7,   2, // 2302-2303
  5, // 2304
  8,   2,   2, // 2305-2307
  8,   2,   2, // 2308-2310
  8,   2,   2, // 2311-2313
  8,   2,   2, // 2314-2316
  7,   2, // 2317-2318
  7,   2, // 2319-2320
  8,   2,   2, // 2321-2323
  7,   2,   2, // 2324-2326
  7,   2, // 2327-2328
  7,   2, // 2329-2330
  7,   2, // 2331-2332
  8,   2, // 2333-2334
  7,   2, // 2335-2336
  7,   2,   2, // 2337-2339
  7,   2, // 2340-2341
  5, // 2342
  5,   2,   2, // 2343-2345
  8,   2, // 2346-2347
  5, // 2348
  8,   2, // 2349-2350
  5, // 2351
  6,   2, // 2352-2353
  5, // 2354
  20,   2,   2, // 2355-2357
  20,   2,   2, // 2358-2360
  30,   2,   2, // 2361-2363
  30,   2,   2, // 2364-2366
  20,   2,   2, // 2367-2369
  30,   2,   2, // 2370-2372
  20,   2,   2, // 2373-2375
  30,   2,   2, // 2376-2378
  5,   2,   2, // 2379-2381
  5,   2,   2, // 2382-2384
  11,   2,   2, // 2385-2387
  5,   2, // 2388-2389
  11, // 2390
  11,   2,   2, // 2391-2393
  6,   2,   2, // 2394-2396
  5,   2,   2, // 2397-2399
  5,   2,   2, // 2400-2402
  5,   2,   2, // 2403-2405
  6,   2,   2, // 2406-2408
  11, // 2409
  9,   2, // 2410-2411
  5,   2,   2, // 2412-2414
  6,   2,   2, // 2415-2417
  39,   2,   2, // 2418-2420
  39,   2,   2, // 2421-2423
  39,   2,   2, // 2424-2426
  24,   2,   2, // 2427-2429
  24,   2,   2, // 2430-2432
  24,   2,   2, // 2433-2435
  24,   2,   2, // 2436-2438
  39,   2,   2, // 2439-2441
  8,   2,   2, // 2442-2444
  8,   2,   2, // 2445-2447
  7,   2,   2, // 2448-2450
  7,   2, // 2451-2452
  7,   2, // 2453-2454
  7,   2, // 2455-2456
  7,   2, // 2457-2458
  7,   2, // 2459-2460
  7,   2, // 2461-2462
  7,   2, // 2463-2464
  8,   2,   2, // 2465-2467
  8,   2,   2, // 2468-2470
  5,   2, // 2471-2472
  5,   2, // 2473-2474
  7,   2, // 2475-2476
  7,   2, // 2477-2478
  5,   2,   2, // 2479-2481
  7,   2, // 2482-2483
  7,   2, // 2484-2485
  5,   2, // 2486-2487
  7,   2, // 2488-2489
  7,   2, // 2490-2491
  8,   2,   2, // 2492-2494
  7,   2,   2, // 2495-2497
  5, // 2498
  5, // 2499
  5,   2,   2, // 2500-2502
  6,   2, // 2503-2504
  7,   2, // 2505-2506
  1,   1,   1, // 2507-2509
  1,   1,   1, // 2510-2512
  3,   1,   1, // 2513-2515
  6,   1,   1, // 2516-2518
  1,   1,   1, // 2519-2521
  3,   1,   1, // 2522-2524
  2,   1,   1, // 2525-2527
  2,   1,   1, // 2528-2530
  23,   1,   1, // 2531-2533
  15,   1,   1, // 2534-2536
  5,   1,   1, // 2537-2539
  33,   1,   1, // 2540-2542
  8,   1,   1, // 2543-2545
  5,   1,   1, // 2546-2548
  2,   1,   1, // 2549-2551
  7,   1,   1, // 2552-2554
  1,   1,   1, // 2555-2557
  27,   1,   1, // 2558-2560
  5,   1,   1,   1, // 2561-2564
  44,   1,   1, // 2565-2567
  32,   1,   1, // 2568-2570
  1,   1,   1,   1, // 2571-2574
  3,   1,   1, // 2575-2577
  2,   2,   1,   1, // 2578-2581
  3,   3,   1,   1, // 2582-2585
  3,   1,   1, // 2586-2588
  2,   2,   1,   1, // 2589-2592
  3,   3,   1,   1, // 2593-2596
  3,   1,   1, // 2597-2599
  3,   3,   1,   1, // 2600-2603
  3,   3,   1,   1, // 2604-2607
  3,   1,   1, // 2608-2610
  4,   4,   1,   1, // 2611-2614
  4,   4,   1,   1, // 2615-2618
  2,   1,   1, // 2619-2621
  3,   1,   1, // 2622-2624
  3,   1,   1, // 2625-2627
  3,   1, // 2628-2629
  6,   1, // 2630-2631
  3,   1, // 2632-2633
  4,   1, // 2634-2635
  4,   1,   1, // 2636-2638
  4,   1,   1, // 2639-2641
  4,   1,   1, // 2642-2644
  3,   1,   1, // 2645-2647
  1,   1,   1, // 2648-2650
  2,   1,   1,   1, // 2651-2654
  1,   1,   1, // 2655-2657
  1,   1,   1, // 2658-2660
  1,   1,   1, // 2661-2663
  1,   1,   1, // 2664-2666
  1,   1,   1, // 2667-2669
  2,   1,   1,   1, // 2670-2673
  2,   1,   1,   1, // 2674-2677
  1,   1, // 2678-2679
  1,   1, // 2680-2681
  6,   1,   1, // 2682-2684
  2,   1,   1, // 2685-2687
  6,   1,   1, // 2688-2690
  3,   1,   1, // 2691-2693
  4,   1, // 2694-2695
  1,   1,   1, // 2696-2698
  1,   1, // 2699-2700
  1,   1, // 2701-2702
  8,   1,   1, // 2703-2705
  6,   1,   1, // 2706-2708
  6,   1,   1, // 2709-2711
  8,   1,   1, // 2712-2714
  1,   1,   1, // 2715-2717
  1,   1,   1, // 2718-2720
  2,   1,   1, // 2721-2723
  1,   1,   1, // 2724-2726
  1,   1,   1, // 2727-2729
  1,   1,   1, // 2730-2732
  3,   1,   1, // 2733-2735
  3,   1, // 2736-2737
  6,   1,   1, // 2738-2740
  3,   1,   1, // 2741-2743
  4,   1, // 2744-2745
  2,   1,   1, // 2746-2748
  1,   1,   1, // 2749-2751
  1,   1,   1, // 2752-2754
  3,   1, // 2755-2756
  3,   1, // 2757-2758
  3,   1,   1, // 2759-2761
  1,   1,   1, // 2762-2764
  5,   1,   1, // 2765-2767
  1,   1,   1, // 2768-2770
  1,   1,   1,   1, // 2771-2774
  6,   1,   1, // 2775-2777
  4,   1, // 2778-2779
  1,   1,   1, // 2780-2782
  2,   1,   1, // 2783-2785
  6,   1,   1, // 2786-2788
  2,   1,   1, // 2789-2791
  6,   1,   1, // 2792-2794
  8,   1,   1, // 2795-2797
  6,   1,   1, // 2798-2800
  2,   1,   1, // 2801-2803
  4,   1,   1, // 2804-2806
  4,   1,   1, // 2807-2809
  4,   1,   1, // 2810-2812
  5,   1,   1, // 2813-2815
  5,   1,   1, // 2816-2818
  5,   1,   1,   1, // 2819-2822
  6,   1,   1, // 2823-2825
  5,   1,   1, // 2826-2828
  4,   1,   1, // 2829-2831
  4,   1,   1, // 2832-2834
  5,   1,   1, // 2835-2837
  5,   1,   1, // 2838-2840
  5,   1,   1,   1, // 2841-2844
  6,   1,   1, // 2845-2847
  2,   1,   1, // 2848-2850
  3,   1,   1, // 2851-2853
  6,   1,   1, // 2854-2856
  2,   1,   1, // 2857-2859
  8,   1,   1, // 2860-2862
  6,   1,   1, // 2863-2865
  6,   1,   1, // 2866-2868
  6,   1,   1, // 2869-2871
  6,   1,   1, // 2872-2874
  6,   1,   1, // 2875-2877
  6,   1,   1, // 2878-2880
  2,   1,   1, // 2881-2883
  2,   1,   1, // 2884-2886
  3,   1,   1, // 2887-2889
  2,   1,   1, // 2890-2892
  3,   1,   1, // 2893-2895
  2,   1,   1, // 2896-2898
  3,   1,   1, // 2899-2901
  2,   1,   1, // 2902-2904
  3,   1,   1, // 2905-2907
  2,   2,   1,   1, // 2908-2911
  3,   3,   1,   1, // 2912-2915
  4,   1, // 2916-2917
  1,   1,   1, // 2918-2920
  1,   1,   1, // 2921-2923
  2,   1,   1, // 2924-2926
  3,   1,   1, // 2927-2929
  3,   1,   1, // 2930-2932
  4,   1, // 2933-2934
  3,   1,   1, // 2935-2937
  3,   1,   1, // 2938-2940
  3,   1,   1, // 2941-2943
  3,   1,   1, // 2944-2946
  4,   4,   1,   1, // 2947-2950
  4,   4,   1,   1, // 2951-2954
  3,   1,   1, // 2955-2957
  1,   1,   1, // 2958-2960
  1,   1,   1, // 2961-2963
  2,   1,   1, // 2964-2966
  2,   1,   1, // 2967-2969
  1,   1,   1, // 2970-2972
  1,   1,   1, // 2973-2975
  2,   1,   1, // 2976-2978
  4,   1, // 2979-2980
  1,   1,   1, // 2981-2983
  4,   1, // 2984-2985
  15,   1,   1, // 2986-2988
  15,   1,   1, // 2989-2991
  23,   1,   1, // 2992-2994
  23,   1,   1, // 2995-2997
  15,   1,   1, // 2998-3000
  23,   1,   1, // 3001-3003
  15,   1,   1, // 3004-3006
  23,   1,   1, // 3007-3009
  3,   1, // 3010-3011
  1,   1,   1, // 3012-3014
  5,   1,   1, // 3015-3017
  3,   1,   1, // 3018-3020
  1,   1,   1, // 3021-3023
  1,   1,   1, // 3024-3026
  1,   1,   1, // 3027-3029
  1,   1,   1, // 3030-3032
  6,   1, // 3033-3034
  1,   1,   1, // 3035-3037
  33,   1,   1, // 3038-3040
  33,   1,   1, // 3041-3043
  44,   1,   1, // 3044-3046
  44,   1,   1, // 3047-3049
  44,   1,   1, // 3050-3052
  32,   1,   1, // 3053-3055
  44,   1,   1, // 3056-3058
  32,   1,   1, // 3059-3061
  32,   1,   1, // 3062-3064
  32,   1,   1, // 3065-3067
  33,   1,   1, // 3068-3070
  27,   1,   1, // 3071-3073
  27,   1,   1, // 3074-3076
  27,   1,   1, // 3077-3079
  27,   1,   1, // 3080-3082
  33,   1,   1, // 3083-3085
  3,   3,   1,   1, // 3086-3089
  3,   3,   1,   1, // 3090-3093
  2,   1,   1,   1, // 3094-3097
  2,   1,   1,   1, // 3098-3101
  2,   1,   1,   1, // 3102-3105
  2,   2,   1,   1, // 3106-3109
  3,   3,   1,   1, // 3110-3113
  2,   2,   1,   1, // 3114-3117
  3,   3,   1,   1, // 3118-3121
  3,   3,   1,   1, // 3122-3125
  3,   3,   1,   1, // 3126-3129
  6,   1,   1, // 3130-3132
  3,   1,   1, // 3133-3135
  3,   1,   1, // 3136-3138
  2,   1,   1, // 3139-3141
  1,   1,   1, // 3142-3144
  1,   1,   1, // 3145-3147
  2,   1,   1, // 3148-3150
  7,   1,   1, // 3151-3153
  2,   1,   1, // 3154-3156
  1,   1,   1, // 3157-3159
  3,   1,   1, // 3160-3162
  2,   1,   1, // 3163-3165
  1,   1,   1, // 3166-3168
  3,   1,   1, // 3169-3171
  1,   1,   1, // 3172-3174
  4,   1, // 3175-3176
  1,   1,   1, // 3177-3179
  1,   1,   1, // 3180-3182
  1,   1,   1, // 3183-3185
  3,   1,   1, // 3186-3188
  6,   1,   1, // 3189-3191
  1,   1,   1, // 3192-3194
  3,   1,   1, // 3195-3197
  2,   1,   1, // 3198-3200
  2,   1,   1, // 3201-3203
  68,   1,   1, // 3204-3206
  36,   1,   1, // 3207-3209
  5,   1,   1, // 3210-3212
  33,   1,   1, // 3213-3215
  8,   1,   1, // 3216-3218
  5,   1,   1, // 3219-3221
  2,   1,   1, // 3222-3224
  7,   1,   1, // 3225-3227
  1,   1,   1, // 3228-3230
  27,   1,   1, // 3231-3233
  5,   1,   1,   1, // 3234-3237
  44,   1,   1, // 3238-3240
  32,   1,   1, // 3241-3243
  1,   1,   1,   1, // 3244-3247
  3,   1,   1, // 3248-3250
  2,   2,   1,   1, // 3251-3254
  3,   3,   1,   1, // 3255-3258
  3,   1,   1, // 3259-3261
  2,   2,   1,   1, // 3262-3265
  3,   3,   1,   1, // 3266-3269
  3,   1,   1, // 3270-3272
  3,   3,   1,   1, // 3273-3276
  3,   3,   1,   1, // 3277-3280
  3,   1,   1, // 3281-3283
  4,   4,   1,   1, // 3284-3287
  4,   4,   1,   1, // 3288-3291
  2,   1,   1, // 3292-3294
  3,   1,   1, // 3295-3297
  3,   1,   1, // 3298-3300
  3,   1, // 3301-3302
  6,   1, // 3303-3304
  3,   1, // 3305-3306
  4,   1, // 3307-3308
  4,   1,   1, // 3309-3311
  4,   1,   1, // 3312-3314
  4,   1,   1, // 3315-3317
  3,   1,   1, // 3318-3320
  1,   1,   1, // 3321-3323
  2,   1,   1,   1, // 3324-3327
  1,   1,   1, // 3328-3330
  1,   1,   1, // 3331-3333
  1,   1,   1, // 3334-3336
  1,   1,   1, // 3337-3339
  1,   1,   1, // 3340-3342
  1,   1,   1, // 3343-3345
  2,   1,   1,   1, // 3346-3349
  2,   1,   1,   1, // 3350-3353
  1,   1, // 3354-3355
  1,   1, // 3356-3357
  6,   1,   1, // 3358-3360
  2,   1,   1, // 3361-3363
  6,   1,   1, // 3364-3366
  3,   1,   1, // 3367-3369
  4,   1, // 3370-3371
  1,   1,   1, // 3372-3374
  1,   1, // 3375-3376
  1,   1, // 3377-3378
  8,   1,   1, // 3379-3381
  6,   1,   1, // 3382-3384
  6,   1,   1, // 3385-3387
  8,   1,   1, // 3388-3390
  1,   1,   1, // 3391-3393
  1,   1,   1, // 3394-3396
  2,   1,   1, // 3397-3399
  1,   1,   1, // 3400-3402
  1,   1,   1, // 3403-3405
  1,   1,   1, // 3406-3408
  3,   1,   1, // 3409-3411
  3,   1, // 3412-3413
  6,   1,   1, // 3414-3416
  3,   1,   1, // 3417-3419
  4,   1, // 3420-3421
  2,   1,   1, // 3422-3424
  1,   1,   1, // 3425-3427
  1,   1,   1, // 3428-3430
  3,   1, // 3431-3432
  3,   1, // 3433-3434
  3,   1,   1, // 3435-3437
  1,   1,   1, // 3438-3440
  5,   1,   1, // 3441-3443
  1,   1,   1, // 3444-3446
  1,   1,   1,   1, // 3447-3450
  6,   1,   1, // 3451-3453
  4,   1, // 3454-3455
  1,   1,   1, // 3456-3458
  2,   1,   1, // 3459-3461
  6,   1,   1, // 3462-3464
  2,   1,   1, // 3465-3467
  6,   1,   1, // 3468-3470
  8,   1,   1, // 3471-3473
  6,   1,   1, // 3474-3476
  2,   1,   1, // 3477-3479
  4,   1,   1, // 3480-3482
  4,   1,   1, // 3483-3485
  4,   1,   1, // 3486-3488
  5,   1,   1, // 3489-3491
  5,   1,   1, // 3492-3494
  5,   1,   1,   1, // 3495-3498
  6,   1,   1, // 3499-3501
  5,   1,   1, // 3502-3504
  4,   1,   1, // 3505-3507
  4,   1,   1, // 3508-3510
  5,   1,   1, // 3511-3513
  5,   1,   1, // 3514-3516
  5,   1,   1,   1, // 3517-3520
  6,   1,   1, // 3521-3523
  2,   1,   1, // 3524-3526
  3,   1,   1, // 3527-3529
  6,   1,   1, // 3530-3532
  2,   1,   1, // 3533-3535
  8,   1,   1, // 3536-3538
  6,   1,   1, // 3539-3541
  6,   1,   1, // 3542-3544
  6,   1,   1, // 3545-3547
  6,   1,   1, // 3548-3550
  6,   1,   1, // 3551-3553
  6,   1,   1, // 3554-3556
  2,   1,   1, // 3557-3559
  2,   1,   1, // 3560-3562
  3,   1,   1, // 3563-3565
  2,   1,   1, // 3566-3568
  3,   1,   1, // 3569-3571
  2,   1,   1, // 3572-3574
  3,   1,   1, // 3575-3577
  2,   1,   1, // 3578-3580
  3,   1,   1, // 3581-3583
  2,   2,   1,   1, // 3584-3587
  3,   3,   1,   1, // 3588-3591
  4,   1, // 3592-3593
  1,   1,   1, // 3594-3596
  1,   1,   1, // 3597-3599
  2,   1,   1, // 3600-3602
  3,   1,   1, // 3603-3605
  3,   1,   1, // 3606-3608
  4,   1, // 3609-3610
  3,   1,   1, // 3611-3613
  3,   1,   1, // 3614-3616
  3,   1,   1, // 3617-3619
  3,   1,   1, // 3620-3622
  4,   4,   1,   1, // 3623-3626
  4,   4,   1,   1, // 3627-3630
  3,   1,   1, // 3631-3633
  1,   1,   1, // 3634-3636
  1,   1,   1, // 3637-3639
  1,   1,   1, // 3640-3642
  2,   1,   1, // 3643-3645
  2,   1,   1, // 3646-3648
  1,   1,   1, // 3649-3651
  1,   1,   1, // 3652-3654
  2,   1,   1, // 3655-3657
  4,   1, // 3658-3659
  1,   1,   1, // 3660-3662
  4,   1, // 3663-3664
  36,   1,   1, // 3665-3667
  36,   1,   1, // 3668-3670
  68,   1,   1, // 3671-3673
  68,   1,   1, // 3674-3676
  36,   1,   1, // 3677-3679
  68,   1,   1, // 3680-3682
  36,   1,   1, // 3683-3685
  68,   1,   1, // 3686-3688
  3,   1, // 3689-3690
  1,   1,   1, // 3691-3693
  5,   1,   1, // 3694-3696
  3,   1,   1, // 3697-3699
  1,   1,   1, // 3700-3702
  1,   1,   1, // 3703-3705
  1,   1,   1, // 3706-3708
  1,   1,   1, // 3709-3711
  6,   1, // 3712-3713
  1,   1,   1, // 3714-3716
  33,   1,   1, // 3717-3719
  33,   1,   1, // 3720-3722
  44,   1,   1, // 3723-3725
  44,   1,   1, // 3726-3728
  44,   1,   1, // 3729-3731
  32,   1,   1, // 3732-3734
  44,   1,   1, // 3735-3737
  32,   1,   1, // 3738-3740
  32,   1,   1, // 3741-3743
  32,   1,   1, // 3744-3746
  33,   1,   1, // 3747-3749
  27,   1,   1, // 3750-3752
  27,   1,   1, // 3753-3755
  27,   1,   1, // 3756-3758
  27,   1,   1, // 3759-3761
  33,   1,   1, // 3762-3764
  3,   3,   1,   1, // 3765-3768
  3,   3,   1,   1, // 3769-3772
  2,   1,   1,   1, // 3773-3776
  2,   1,   1,   1, // 3777-3780
  2,   1,   1,   1, // 3781-3784
  2,   2,   1,   1, // 3785-3788
  3,   3,   1,   1, // 3789-3792
  2,   2,   1,   1, // 3793-3796
  3,   3,   1,   1, // 3797-3800
  3,   3,   1,   1, // 3801-3804
  3,   3,   1,   1, // 3805-3808
  6,   1,   1, // 3809-3811
  3,   1,   1, // 3812-3814
  3,   1,   1, // 3815-3817
  2,   1,   1, // 3818-3820
  1,   1,   1, // 3821-3823
  1,   1,   1, // 3824-3826
  2,   1,   1, // 3827-3829
  7,   1,   1, // 3830-3832
  2,   1,   1, // 3833-3835
  1,   1,   1, // 3836-3838
  3,   1,   1, // 3839-3841
  2,   1,   1, // 3842-3844
  1,   1,   1, // 3845-3847
  3,   1,   1, // 3848-3850
  1,   1,   1, // 3851-3853
  4,   1, // 3854-3855
  1,   1,   1, // 3856-3858
  0 // End operand cycles
};
extern const unsigned PPCForwardingPaths[] = {
 0, // No itinerary
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 1-3
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 4-6
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 7-8
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 9-11
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 12-14
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 15-16
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 17-19
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 20-22
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 23-25
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 26-28
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 29-31
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 32-34
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 35-37
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 38-40
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 41-44
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 45-47
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::NoBypass, // 48-51
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 52-54
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 55-58
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 59-62
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 63-65
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 66-69
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 70-73
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 74-76
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 77-79
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 80-82
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 83-85
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 86-88
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 89-91
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 92-93
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 94-95
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 96-98
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 99-100
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 101-102
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 103-104
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 105-106
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 107-109
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 110-111
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 112-113
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 114-116
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 117-119
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 120-122
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 123-125
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 126-129
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 130-131
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 132-134
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 135-137
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 138-140
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 141-144
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 145-147
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 148-150
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 151-154
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 155-158
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 159-160
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 161-162
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 163-165
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 166-168
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 169-171
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 172-173
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 174-176
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 177-179
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 180-182
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 183-185
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 186-188
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 189-191
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 192-194
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 195-197
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 198-199
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 200-201
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 202-204
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 205-207
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 208-210
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 211-213
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 214-216
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 217-219
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 220-222
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::NoBypass, // 223-226
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 227-228
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 229-231
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 232-234
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 235-237
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 238-240
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 241-243
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 244-246
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 247-249
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 250-252
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 253-256
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 257-259
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 260-262
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 263-265
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 266-268
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 269-271
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 272-275
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 276-278
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 279-281
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 282-284
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 285-287
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 288-290
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 291-292
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 293-295
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 296-298
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 299-301
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 302-304
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 305-307
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 308-311
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 312-315
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 316-317
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 318-320
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 321-323
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 324-326
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 327-328
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 329-331
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 332-334
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 335-337
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 338-340
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 341-343
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 344-346
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 347-349
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 350-352
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 353-355
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 356-358
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 359-361
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 362-364
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 365-367
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 368-370
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 371-373
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 374-375
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 376-378
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 379-380
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 381-382
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 383-384
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 385-386
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 387-388
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 389-391
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 392-394
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 395-397
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 398-400
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 401-403
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 404-406
PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 407-409
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 410-412
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 413-415
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 416-418
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 419-421
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 422-424
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 425-427
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 428-430
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 431-432
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 433-435
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 436-438
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 439-441
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 442-444
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 445-447
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 448-450
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 451-453
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_FPR_Bypass, PPC440ItinerariesBypass::P440_FPR_Bypass, // 454-456
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 457-460
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 461-464
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 465-468
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 469-472
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 473-476
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 477-480
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 481-484
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0,  0, // 485-488
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 489-492
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 493-496
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 497-498
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 499-500
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 501-503
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 504-506
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 507-509
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 510-512
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 513-515
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 516-517
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 518-519
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 520-522
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass,  0, // 523-525
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 526-528
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 529-531
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 532-533
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 534-535
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 536-537
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 538-539
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 540-542
PPC440ItinerariesBypass::P440_GPR_Bypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 543-544
PPC440ItinerariesBypass::NoBypass, PPC440ItinerariesBypass::P440_GPR_Bypass, // 545-546
 0, // 547
 0,  0,  0, // 548-550
 0,  0,  0, // 551-553
 0,  0,  0, // 554-556
 0,  0,  0, // 557-559
 0,  0,  0, // 560-562
 0,  0,  0, // 563-565
 0,  0,  0, // 566-568
 0,  0,  0, // 569-571
 0,  0,  0, // 572-574
 0,  0,  0, // 575-577
 0,  0,  0, // 578-580
 0,  0,  0, // 581-583
 0,  0,  0, // 584-586
 0,  0,  0, // 587-589
 0,  0,  0, // 590-592
 0,  0,  0, // 593-595
 0,  0,  0,  0, // 596-599
 0,  0,  0, // 600-602
 0,  0,  0, // 603-605
 0,  0,  0, // 606-608
 0,  0,  0,  0, // 609-612
 0, // 613
 0,  0,  0, // 614-616
 0,  0,  0,  0, // 617-620
 0,  0,  0,  0, // 621-624
 0,  0,  0, // 625-627
 0,  0,  0, // 628-630
 0,  0,  0, // 631-633
 0,  0,  0,  0, // 634-637
 0,  0,  0,  0, // 638-641
 0,  0,  0, // 642-644
 0,  0,  0,  0, // 645-648
 0,  0,  0,  0, // 649-652
 0,  0,  0, // 653-655
 0,  0,  0, // 656-658
 0,  0, // 659-660
 0,  0, // 661-662
 0,  0, // 663-664
 0,  0, // 665-666
 0,  0, // 667-668
 0,  0, // 669-670
 0,  0, // 671-672
 0,  0,  0, // 673-675
 0,  0,  0, // 676-678
 0,  0,  0, // 679-681
 0,  0,  0, // 682-684
 0,  0,  0,  0, // 685-688
 0, // 689
 0,  0,  0, // 690-692
 0,  0,  0, // 693-695
 0,  0,  0, // 696-698
 0,  0,  0, // 699-701
 0,  0,  0,  0, // 702-705
 0,  0,  0, // 706-708
 0,  0,  0, // 709-711
 0,  0,  0,  0, // 712-715
 0,  0,  0,  0, // 716-719
 0,  0, // 720-721
 0,  0, // 722-723
 0,  0, // 724-725
 0,  0,  0, // 726-728
 0,  0, // 729-730
 0,  0, // 731-732
 0,  0,  0, // 733-735
 0,  0,  0, // 736-738
 0,  0,  0, // 739-741
 0,  0,  0, // 742-744
 0,  0,  0, // 745-747
 0,  0,  0, // 748-750
 0,  0,  0, // 751-753
 0,  0,  0, // 754-756
 0,  0,  0, // 757-759
 0,  0,  0, // 760-762
 0,  0,  0, // 763-765
 0,  0, // 766-767
 0,  0,  0, // 768-770
 0,  0,  0, // 771-773
 0,  0,  0, // 774-776
 0,  0,  0, // 777-779
 0,  0, // 780-781
 0,  0,  0, // 782-784
 0,  0,  0, // 785-787
 0,  0,  0, // 788-790
 0,  0,  0, // 791-793
 0,  0,  0, // 794-796
 0,  0,  0,  0, // 797-800
 0,  0, // 801-802
 0,  0,  0, // 803-805
 0,  0,  0, // 806-808
 0,  0,  0, // 809-811
 0,  0,  0, // 812-814
 0,  0,  0, // 815-817
 0,  0,  0, // 818-820
 0,  0,  0, // 821-823
 0,  0,  0, // 824-826
 0,  0,  0,  0, // 827-830
 0,  0,  0, // 831-833
 0,  0,  0, // 834-836
 0,  0,  0, // 837-839
 0,  0,  0, // 840-842
 0,  0,  0, // 843-845
 0,  0,  0,  0, // 846-849
 0,  0,  0, // 850-852
 0,  0,  0, // 853-855
 0,  0,  0, // 856-858
 0,  0,  0, // 859-861
 0,  0,  0, // 862-864
 0,  0,  0, // 865-867
 0,  0,  0, // 868-870
 0,  0,  0, // 871-873
 0,  0,  0, // 874-876
 0, // 877
 0, // 878
 0,  0,  0, // 879-881
 0,  0,  0,  0, // 882-885
 0,  0,  0,  0, // 886-889
 0,  0, // 890-891
 0,  0,  0, // 892-894
 0,  0,  0, // 895-897
 0,  0,  0, // 898-900
 0,  0, // 901-902
 0,  0,  0, // 903-905
 0,  0,  0, // 906-908
 0,  0,  0, // 909-911
 0,  0,  0, // 912-914
 0,  0,  0,  0, // 915-918
 0,  0,  0,  0, // 919-922
 0,  0,  0, // 923-925
 0,  0,  0, // 926-928
 0,  0,  0, // 929-931
 0,  0,  0, // 932-934
 0,  0,  0, // 935-937
 0,  0,  0, // 938-940
 0,  0,  0, // 941-943
 0,  0,  0, // 944-946
 0,  0, // 947-948
 0,  0,  0, // 949-951
 0,  0, // 952-953
 0,  0, // 954-955
 0,  0, // 956-957
 0,  0, // 958-959
 0,  0, // 960-961
 0,  0,  0, // 962-964
 0,  0,  0, // 965-967
 0,  0,  0, // 968-970
 0,  0,  0, // 971-973
 0,  0,  0, // 974-976
 0,  0,  0, // 977-979
 0,  0,  0, // 980-982
 0,  0,  0, // 983-985
 0,  0,  0, // 986-988
 0,  0,  0, // 989-991
 0,  0,  0, // 992-994
 0,  0,  0, // 995-997
 0,  0,  0, // 998-1000
 0,  0,  0, // 1001-1003
 0,  0,  0, // 1004-1006
 0,  0,  0, // 1007-1009
 0,  0,  0, // 1010-1012
 0,  0, // 1013-1014
 0,  0,  0, // 1015-1017
 0,  0,  0, // 1018-1020
 0,  0,  0, // 1021-1023
 0,  0,  0, // 1024-1026
 0,  0,  0, // 1027-1029
 0,  0,  0, // 1030-1032
 0,  0,  0, // 1033-1035
 0,  0,  0, // 1036-1038
 0,  0,  0, // 1039-1041
 0,  0,  0, // 1042-1044
 0,  0,  0, // 1045-1047
 0,  0,  0, // 1048-1050
 0,  0,  0, // 1051-1053
 0,  0,  0, // 1054-1056
 0,  0,  0, // 1057-1059
 0,  0,  0, // 1060-1062
 0,  0,  0, // 1063-1065
 0,  0,  0, // 1066-1068
 0,  0,  0,  0, // 1069-1072
 0,  0,  0,  0, // 1073-1076
 0,  0,  0,  0, // 1077-1080
 0,  0,  0,  0, // 1081-1084
 0,  0,  0,  0, // 1085-1088
 0,  0,  0,  0, // 1089-1092
 0,  0,  0,  0, // 1093-1096
 0,  0,  0,  0, // 1097-1100
 0,  0,  0, // 1101-1103
 0,  0,  0, // 1104-1106
 0,  0,  0,  0, // 1107-1110
 0,  0,  0,  0, // 1111-1114
 0,  0,  0, // 1115-1117
 0,  0,  0, // 1118-1120
 0,  0,  0, // 1121-1123
 0,  0,  0, // 1124-1126
 0,  0,  0, // 1127-1129
 0,  0,  0, // 1130-1132
 0,  0,  0, // 1133-1135
 0,  0,  0, // 1136-1138
 0, // 1139
 0,  0,  0, // 1140-1142
 0,  0,  0, // 1143-1145
 0,  0,  0, // 1146-1148
 0,  0,  0, // 1149-1151
 0,  0, // 1152-1153
 0,  0, // 1154-1155
 0,  0,  0, // 1156-1158
 0,  0, // 1159-1160
 0, // 1161
 0,  0,  0, // 1162-1164
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1165-1167
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1168-1170
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1171-1172
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1173-1175
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1176-1178
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1179-1180
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1181-1182
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1183-1185
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1186-1188
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1189-1191
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1192-1194
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1195-1197
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1198-1200
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1201-1202
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1203-1205
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1206-1207
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1208-1211
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1212-1214
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1215-1216
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1217-1218
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1219-1220
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1221-1222
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1223-1224
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1225-1226
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1227-1228
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass,  0, // 1229-1231
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1232-1233
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1234-1235
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1236-1237
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1238-1239
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1240-1241
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1242-1243
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1244-1245
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1246-1247
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1248-1249
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1250-1251
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1252-1253
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1254-1256
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1257-1259
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1260-1262
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1263-1265
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1266-1268
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1269-1270
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1271-1272
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1273-1274
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1275-1277
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1278-1279
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1280-1282
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1283-1284
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1285-1287
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1288-1290
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1291-1293
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1294-1296
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1297-1299
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1300-1301
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass,  0, // 1302-1304
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1305-1306
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1307-1308
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1309-1310
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1311-1313
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1314-1315
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass,  0, // 1316-1318
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1319-1321
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1322-1324
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1325-1327
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1328-1331
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1332-1333
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1334-1335
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1336-1337
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1338-1340
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1341-1343
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1344-1346
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1347-1349
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1350-1352
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1353-1355
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1356-1358
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1359-1360
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1361-1363
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1364-1365
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1366-1367
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1368-1369
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1370-1371
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1372-1373
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1374-1376
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1377-1378
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1379-1380
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1381-1382
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1383-1384
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1385-1386
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1387-1388
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1389-1390
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1391-1392
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1393-1394
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1395-1396
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1397-1398
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1399-1400
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1401-1402
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1403-1404
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1405-1406
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1407-1409
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1410-1411
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1412-1413
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1414-1415
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1416-1417
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1418-1419
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1420-1421
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1422-1423
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1424-1426
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1427-1429
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1430-1432
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1433-1435
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass,  0, // 1436-1438
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1439-1441
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1442-1443
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1444-1446
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1447-1449
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1450-1452
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1453-1454
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1455-1457
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1458-1460
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1461-1463
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1464-1466
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1467-1469
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1470-1472
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1473-1475
PPCE500ItinerariesBypass::E500_DivBypass,  0,  0, // 1476-1478
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1479-1480
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1481-1482
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1483-1484
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1485-1486
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1487-1488
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1489-1490
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1491-1492
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1493-1495
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1496-1498
PPCE500ItinerariesBypass::NoBypass,  0,  0, // 1499-1501
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1502-1503
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1504-1505
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1506-1507
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1508-1509
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1510-1511
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1512-1513
PPCE500ItinerariesBypass::NoBypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1514-1515
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_CR_Bypass, // 1516-1517
PPCE500ItinerariesBypass::E500_CR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1518-1519
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1520-1522
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1523-1524
PPCE500ItinerariesBypass::E500_GPR_Bypass, PPCE500ItinerariesBypass::E500_GPR_Bypass, // 1525-1526
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1527-1529
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1530-1532
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1533-1534
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1535-1537
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1538-1540
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1541-1542
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1543-1544
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1545-1547
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1548-1550
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1551-1553
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1554-1556
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1557-1559
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1560-1561
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1562-1564
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1565-1568
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1569-1570
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1571-1574
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1575-1577
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1578-1579
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1580-1581
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1582-1584
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1585-1587
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1588-1590
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1591-1592
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1593-1594
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1595-1596
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1597-1598
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1599-1600
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass,  0, // 1601-1603
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1604-1605
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1606-1607
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1608
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1609-1610
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1611-1612
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1613-1614
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1615-1616
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1617-1618
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1619-1620
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, // 1621-1623
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1624-1625
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1626-1627
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1628-1629
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1630-1632
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1633-1635
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1636-1638
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1639-1641
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1642-1644
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1645-1647
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1648-1650
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1651-1652
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1653-1654
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1655-1656
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1657-1658
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, // 1659-1661
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1662
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1663-1665
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1666-1667
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1668-1670
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1671-1673
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1674-1676
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1677-1679
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1680-1682
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1683-1685
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1686-1687
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass,  0, // 1688-1690
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1691-1692
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1693-1694
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1695-1696
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1697-1699
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1700-1701
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass,  0, // 1702-1704
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1705-1707
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1708-1710
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1711-1713
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1714-1716
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1717-1720
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1721-1722
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1723-1724
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1725-1726
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1727-1729
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1730-1732
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1733-1735
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1736-1738
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1739-1741
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1742-1744
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1745-1748
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1749-1751
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1752-1754
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1755-1757
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1758-1760
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1761-1763
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1764-1767
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1768-1769
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1770-1772
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1773-1774
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1775-1776
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1777-1779
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1780-1781
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1782-1783
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1784-1785
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1786-1788
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1789-1790
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1791-1793
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1794-1795
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1796-1797
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1798-1799
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1800-1801
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1802-1803
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1804-1805
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1806-1808
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1809-1811
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1812-1814
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1815-1817
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1818-1819
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1820-1821
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1822-1824
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1825-1827
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1828-1829
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1830-1831
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1832-1833
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1834-1835
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1836-1838
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1839-1840
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1841-1842
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1843-1845
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1846-1847
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1848-1849
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1850-1851
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1852-1853
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1854-1855
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1856-1857
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1858-1859
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1860-1862
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1863-1865
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1866-1868
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1869-1871
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass,  0, // 1872-1874
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1875-1877
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1878-1880
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1881-1882
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1883
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::NoBypass, // 1884-1886
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1887-1889
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1890-1892
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1893-1895
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1896
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1897-1898
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1899-1901
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1902-1904
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1905-1907
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1908-1910
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1911-1913
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1914-1916
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1917-1919
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, // 1920-1922
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1923-1925
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1926-1928
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1929-1931
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1932-1933
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1934-1935
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1936-1937
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1938-1940
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1941-1943
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1944-1945
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1946-1947
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1948-1949
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1950-1951
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1952-1954
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1955-1956
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1957-1958
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1959-1960
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1961-1962
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1963-1964
PPCE500mcItinerariesBypass::E500mc_FPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1965-1967
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1968-1970
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1971-1972
PPCE500mcItinerariesBypass::NoBypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1973-1974
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_CR_Bypass, // 1975-1976
PPCE500mcItinerariesBypass::E500mc_CR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1977-1978
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1979-1981
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1982-1983
PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, PPCE500mcItinerariesBypass::E500mc_GPR_Bypass, // 1984-1985
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 1986-1988
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 1989-1991
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 1992-1993
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 1994-1996
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 1997-1999
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2000-2001
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2002-2003
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2004-2005
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2006-2008
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2009-2011
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2012-2014
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2015-2017
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2018-2020
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2021-2023
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2024-2025
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2026-2028
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2029-2031
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2032-2035
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2036-2037
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2038-2041
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2042-2044
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2045-2046
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2047-2048
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2049-2050
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2051-2052
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2053-2054
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2055-2057
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2058-2060
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2061-2063
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2064-2065
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2066-2067
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2068-2069
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2070-2071
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2072-2074
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2075-2076
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass,  0, // 2077-2079
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2080-2081
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2082
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2083
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2084-2085
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2086-2087
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2088-2089
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2090
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, // 2091-2093
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2094-2095
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2096
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2097-2099
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2100-2102
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2103-2105
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2106-2108
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2109-2111
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2112-2114
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2115-2117
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2118-2120
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2121-2122
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2123-2124
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2125-2126
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2127-2128
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2129-2130
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2131-2132
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2133-2134
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2135
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, // 2136-2138
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2139
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2140-2142
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2143-2144
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2145-2147
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2148-2150
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2151-2153
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2154-2156
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2157-2159
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2160-2162
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2163-2165
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2166-2168
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2169-2170
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass,  0, // 2171-2173
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2174-2175
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2176
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2177-2178
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2179-2181
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2182-2184
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2185-2187
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2188-2189
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass,  0, // 2190-2192
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2193-2195
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2196-2198
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2199-2201
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2202-2204
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2205-2208
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2209
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2210-2211
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2212-2213
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2214-2216
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2217-2219
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2220-2222
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2223-2225
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2226-2228
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2229-2231
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2232-2234
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2235-2238
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2239-2241
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2242-2244
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2245-2247
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2248-2250
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2251-2253
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2254-2256
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2257-2260
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2261-2262
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2263-2265
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2266-2267
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2268-2269
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2270-2272
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2273-2274
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2275-2276
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2277-2278
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2279-2281
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2282-2283
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2284-2285
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2286-2287
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2288-2290
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2291-2292
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2293-2294
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2295
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2296-2297
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2298-2299
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2300-2301
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2302-2303
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2304
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2305-2307
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2308-2310
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2311-2313
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2314-2316
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2317-2318
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2319-2320
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2321-2323
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2324-2326
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2327-2328
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2329-2330
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2331-2332
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2333-2334
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2335-2336
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2337-2339
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2340-2341
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2342
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2343-2345
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2346-2347
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2348
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2349-2350
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2351
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2352-2353
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2354
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2355-2357
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2358-2360
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2361-2363
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2364-2366
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2367-2369
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2370-2372
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2373-2375
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2376-2378
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass,  0, // 2379-2381
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2382-2384
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2385-2387
PPCE5500ItinerariesBypass::E5500_CR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2388-2389
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2390
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::NoBypass, // 2391-2393
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2394-2396
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2397-2399
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2400-2402
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2403-2405
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2406-2408
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2409
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_CR_Bypass, // 2410-2411
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2412-2414
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2415-2417
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2418-2420
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2421-2423
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2424-2426
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2427-2429
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2430-2432
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2433-2435
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2436-2438
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_FPR_Bypass, // 2439-2441
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2442-2444
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2445-2447
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2448-2450
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2451-2452
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2453-2454
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2455-2456
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2457-2458
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2459-2460
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2461-2462
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2463-2464
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2465-2467
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2468-2470
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2471-2472
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2473-2474
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2475-2476
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2477-2478
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2479-2481
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2482-2483
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2484-2485
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2486-2487
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2488-2489
PPCE5500ItinerariesBypass::NoBypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2490-2491
PPCE5500ItinerariesBypass::E5500_FPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2492-2494
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2495-2497
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2498
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2499
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2500-2502
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2503-2504
PPCE5500ItinerariesBypass::E5500_GPR_Bypass, PPCE5500ItinerariesBypass::E5500_GPR_Bypass, // 2505-2506
 0,  0,  0, // 2507-2509
 0,  0,  0, // 2510-2512
 0,  0,  0, // 2513-2515
 0,  0,  0, // 2516-2518
 0,  0,  0, // 2519-2521
 0,  0,  0, // 2522-2524
 0,  0,  0, // 2525-2527
 0,  0,  0, // 2528-2530
 0,  0,  0, // 2531-2533
 0,  0,  0, // 2534-2536
 0,  0,  0, // 2537-2539
 0,  0,  0, // 2540-2542
 0,  0,  0, // 2543-2545
 0,  0,  0, // 2546-2548
 0,  0,  0, // 2549-2551
 0,  0,  0, // 2552-2554
 0,  0,  0, // 2555-2557
 0,  0,  0, // 2558-2560
 0,  0,  0,  0, // 2561-2564
 0,  0,  0, // 2565-2567
 0,  0,  0, // 2568-2570
 0,  0,  0,  0, // 2571-2574
 0,  0,  0, // 2575-2577
 0,  0,  0,  0, // 2578-2581
 0,  0,  0,  0, // 2582-2585
 0,  0,  0, // 2586-2588
 0,  0,  0,  0, // 2589-2592
 0,  0,  0,  0, // 2593-2596
 0,  0,  0, // 2597-2599
 0,  0,  0,  0, // 2600-2603
 0,  0,  0,  0, // 2604-2607
 0,  0,  0, // 2608-2610
 0,  0,  0,  0, // 2611-2614
 0,  0,  0,  0, // 2615-2618
 0,  0,  0, // 2619-2621
 0,  0,  0, // 2622-2624
 0,  0,  0, // 2625-2627
 0,  0, // 2628-2629
 0,  0, // 2630-2631
 0,  0, // 2632-2633
 0,  0, // 2634-2635
 0,  0,  0, // 2636-2638
 0,  0,  0, // 2639-2641
 0,  0,  0, // 2642-2644
 0,  0,  0, // 2645-2647
 0,  0,  0, // 2648-2650
 0,  0,  0,  0, // 2651-2654
 0,  0,  0, // 2655-2657
 0,  0,  0, // 2658-2660
 0,  0,  0, // 2661-2663
 0,  0,  0, // 2664-2666
 0,  0,  0, // 2667-2669
 0,  0,  0,  0, // 2670-2673
 0,  0,  0,  0, // 2674-2677
 0,  0, // 2678-2679
 0,  0, // 2680-2681
 0,  0,  0, // 2682-2684
 0,  0,  0, // 2685-2687
 0,  0,  0, // 2688-2690
 0,  0,  0, // 2691-2693
 0,  0, // 2694-2695
 0,  0,  0, // 2696-2698
 0,  0, // 2699-2700
 0,  0, // 2701-2702
 0,  0,  0, // 2703-2705
 0,  0,  0, // 2706-2708
 0,  0,  0, // 2709-2711
 0,  0,  0, // 2712-2714
 0,  0,  0, // 2715-2717
 0,  0,  0, // 2718-2720
 0,  0,  0, // 2721-2723
 0,  0,  0, // 2724-2726
 0,  0,  0, // 2727-2729
 0,  0,  0, // 2730-2732
 0,  0,  0, // 2733-2735
 0,  0, // 2736-2737
 0,  0,  0, // 2738-2740
 0,  0,  0, // 2741-2743
 0,  0, // 2744-2745
 0,  0,  0, // 2746-2748
 0,  0,  0, // 2749-2751
 0,  0,  0, // 2752-2754
 0,  0, // 2755-2756
 0,  0, // 2757-2758
 0,  0,  0, // 2759-2761
 0,  0,  0, // 2762-2764
 0,  0,  0, // 2765-2767
 0,  0,  0, // 2768-2770
 0,  0,  0,  0, // 2771-2774
 0,  0,  0, // 2775-2777
 0,  0, // 2778-2779
 0,  0,  0, // 2780-2782
 0,  0,  0, // 2783-2785
 0,  0,  0, // 2786-2788
 0,  0,  0, // 2789-2791
 0,  0,  0, // 2792-2794
 0,  0,  0, // 2795-2797
 0,  0,  0, // 2798-2800
 0,  0,  0, // 2801-2803
 0,  0,  0, // 2804-2806
 0,  0,  0, // 2807-2809
 0,  0,  0, // 2810-2812
 0,  0,  0, // 2813-2815
 0,  0,  0, // 2816-2818
 0,  0,  0,  0, // 2819-2822
 0,  0,  0, // 2823-2825
 0,  0,  0, // 2826-2828
 0,  0,  0, // 2829-2831
 0,  0,  0, // 2832-2834
 0,  0,  0, // 2835-2837
 0,  0,  0, // 2838-2840
 0,  0,  0,  0, // 2841-2844
 0,  0,  0, // 2845-2847
 0,  0,  0, // 2848-2850
 0,  0,  0, // 2851-2853
 0,  0,  0, // 2854-2856
 0,  0,  0, // 2857-2859
 0,  0,  0, // 2860-2862
 0,  0,  0, // 2863-2865
 0,  0,  0, // 2866-2868
 0,  0,  0, // 2869-2871
 0,  0,  0, // 2872-2874
 0,  0,  0, // 2875-2877
 0,  0,  0, // 2878-2880
 0,  0,  0, // 2881-2883
 0,  0,  0, // 2884-2886
 0,  0,  0, // 2887-2889
 0,  0,  0, // 2890-2892
 0,  0,  0, // 2893-2895
 0,  0,  0, // 2896-2898
 0,  0,  0, // 2899-2901
 0,  0,  0, // 2902-2904
 0,  0,  0, // 2905-2907
 0,  0,  0,  0, // 2908-2911
 0,  0,  0,  0, // 2912-2915
 0,  0, // 2916-2917
 0,  0,  0, // 2918-2920
 0,  0,  0, // 2921-2923
 0,  0,  0, // 2924-2926
 0,  0,  0, // 2927-2929
 0,  0,  0, // 2930-2932
 0,  0, // 2933-2934
 0,  0,  0, // 2935-2937
 0,  0,  0, // 2938-2940
 0,  0,  0, // 2941-2943
 0,  0,  0, // 2944-2946
 0,  0,  0,  0, // 2947-2950
 0,  0,  0,  0, // 2951-2954
 0,  0,  0, // 2955-2957
 0,  0,  0, // 2958-2960
 0,  0,  0, // 2961-2963
 0,  0,  0, // 2964-2966
 0,  0,  0, // 2967-2969
 0,  0,  0, // 2970-2972
 0,  0,  0, // 2973-2975
 0,  0,  0, // 2976-2978
 0,  0, // 2979-2980
 0,  0,  0, // 2981-2983
 0,  0, // 2984-2985
 0,  0,  0, // 2986-2988
 0,  0,  0, // 2989-2991
 0,  0,  0, // 2992-2994
 0,  0,  0, // 2995-2997
 0,  0,  0, // 2998-3000
 0,  0,  0, // 3001-3003
 0,  0,  0, // 3004-3006
 0,  0,  0, // 3007-3009
 0,  0, // 3010-3011
 0,  0,  0, // 3012-3014
 0,  0,  0, // 3015-3017
 0,  0,  0, // 3018-3020
 0,  0,  0, // 3021-3023
 0,  0,  0, // 3024-3026
 0,  0,  0, // 3027-3029
 0,  0,  0, // 3030-3032
 0,  0, // 3033-3034
 0,  0,  0, // 3035-3037
 0,  0,  0, // 3038-3040
 0,  0,  0, // 3041-3043
 0,  0,  0, // 3044-3046
 0,  0,  0, // 3047-3049
 0,  0,  0, // 3050-3052
 0,  0,  0, // 3053-3055
 0,  0,  0, // 3056-3058
 0,  0,  0, // 3059-3061
 0,  0,  0, // 3062-3064
 0,  0,  0, // 3065-3067
 0,  0,  0, // 3068-3070
 0,  0,  0, // 3071-3073
 0,  0,  0, // 3074-3076
 0,  0,  0, // 3077-3079
 0,  0,  0, // 3080-3082
 0,  0,  0, // 3083-3085
 0,  0,  0,  0, // 3086-3089
 0,  0,  0,  0, // 3090-3093
 0,  0,  0,  0, // 3094-3097
 0,  0,  0,  0, // 3098-3101
 0,  0,  0,  0, // 3102-3105
 0,  0,  0,  0, // 3106-3109
 0,  0,  0,  0, // 3110-3113
 0,  0,  0,  0, // 3114-3117
 0,  0,  0,  0, // 3118-3121
 0,  0,  0,  0, // 3122-3125
 0,  0,  0,  0, // 3126-3129
 0,  0,  0, // 3130-3132
 0,  0,  0, // 3133-3135
 0,  0,  0, // 3136-3138
 0,  0,  0, // 3139-3141
 0,  0,  0, // 3142-3144
 0,  0,  0, // 3145-3147
 0,  0,  0, // 3148-3150
 0,  0,  0, // 3151-3153
 0,  0,  0, // 3154-3156
 0,  0,  0, // 3157-3159
 0,  0,  0, // 3160-3162
 0,  0,  0, // 3163-3165
 0,  0,  0, // 3166-3168
 0,  0,  0, // 3169-3171
 0,  0,  0, // 3172-3174
 0,  0, // 3175-3176
 0,  0,  0, // 3177-3179
 0,  0,  0, // 3180-3182
 0,  0,  0, // 3183-3185
 0,  0,  0, // 3186-3188
 0,  0,  0, // 3189-3191
 0,  0,  0, // 3192-3194
 0,  0,  0, // 3195-3197
 0,  0,  0, // 3198-3200
 0,  0,  0, // 3201-3203
 0,  0,  0, // 3204-3206
 0,  0,  0, // 3207-3209
 0,  0,  0, // 3210-3212
 0,  0,  0, // 3213-3215
 0,  0,  0, // 3216-3218
 0,  0,  0, // 3219-3221
 0,  0,  0, // 3222-3224
 0,  0,  0, // 3225-3227
 0,  0,  0, // 3228-3230
 0,  0,  0, // 3231-3233
 0,  0,  0,  0, // 3234-3237
 0,  0,  0, // 3238-3240
 0,  0,  0, // 3241-3243
 0,  0,  0,  0, // 3244-3247
 0,  0,  0, // 3248-3250
 0,  0,  0,  0, // 3251-3254
 0,  0,  0,  0, // 3255-3258
 0,  0,  0, // 3259-3261
 0,  0,  0,  0, // 3262-3265
 0,  0,  0,  0, // 3266-3269
 0,  0,  0, // 3270-3272
 0,  0,  0,  0, // 3273-3276
 0,  0,  0,  0, // 3277-3280
 0,  0,  0, // 3281-3283
 0,  0,  0,  0, // 3284-3287
 0,  0,  0,  0, // 3288-3291
 0,  0,  0, // 3292-3294
 0,  0,  0, // 3295-3297
 0,  0,  0, // 3298-3300
 0,  0, // 3301-3302
 0,  0, // 3303-3304
 0,  0, // 3305-3306
 0,  0, // 3307-3308
 0,  0,  0, // 3309-3311
 0,  0,  0, // 3312-3314
 0,  0,  0, // 3315-3317
 0,  0,  0, // 3318-3320
 0,  0,  0, // 3321-3323
 0,  0,  0,  0, // 3324-3327
 0,  0,  0, // 3328-3330
 0,  0,  0, // 3331-3333
 0,  0,  0, // 3334-3336
 0,  0,  0, // 3337-3339
 0,  0,  0, // 3340-3342
 0,  0,  0, // 3343-3345
 0,  0,  0,  0, // 3346-3349
 0,  0,  0,  0, // 3350-3353
 0,  0, // 3354-3355
 0,  0, // 3356-3357
 0,  0,  0, // 3358-3360
 0,  0,  0, // 3361-3363
 0,  0,  0, // 3364-3366
 0,  0,  0, // 3367-3369
 0,  0, // 3370-3371
 0,  0,  0, // 3372-3374
 0,  0, // 3375-3376
 0,  0, // 3377-3378
 0,  0,  0, // 3379-3381
 0,  0,  0, // 3382-3384
 0,  0,  0, // 3385-3387
 0,  0,  0, // 3388-3390
 0,  0,  0, // 3391-3393
 0,  0,  0, // 3394-3396
 0,  0,  0, // 3397-3399
 0,  0,  0, // 3400-3402
 0,  0,  0, // 3403-3405
 0,  0,  0, // 3406-3408
 0,  0,  0, // 3409-3411
 0,  0, // 3412-3413
 0,  0,  0, // 3414-3416
 0,  0,  0, // 3417-3419
 0,  0, // 3420-3421
 0,  0,  0, // 3422-3424
 0,  0,  0, // 3425-3427
 0,  0,  0, // 3428-3430
 0,  0, // 3431-3432
 0,  0, // 3433-3434
 0,  0,  0, // 3435-3437
 0,  0,  0, // 3438-3440
 0,  0,  0, // 3441-3443
 0,  0,  0, // 3444-3446
 0,  0,  0,  0, // 3447-3450
 0,  0,  0, // 3451-3453
 0,  0, // 3454-3455
 0,  0,  0, // 3456-3458
 0,  0,  0, // 3459-3461
 0,  0,  0, // 3462-3464
 0,  0,  0, // 3465-3467
 0,  0,  0, // 3468-3470
 0,  0,  0, // 3471-3473
 0,  0,  0, // 3474-3476
 0,  0,  0, // 3477-3479
 0,  0,  0, // 3480-3482
 0,  0,  0, // 3483-3485
 0,  0,  0, // 3486-3488
 0,  0,  0, // 3489-3491
 0,  0,  0, // 3492-3494
 0,  0,  0,  0, // 3495-3498
 0,  0,  0, // 3499-3501
 0,  0,  0, // 3502-3504
 0,  0,  0, // 3505-3507
 0,  0,  0, // 3508-3510
 0,  0,  0, // 3511-3513
 0,  0,  0, // 3514-3516
 0,  0,  0,  0, // 3517-3520
 0,  0,  0, // 3521-3523
 0,  0,  0, // 3524-3526
 0,  0,  0, // 3527-3529
 0,  0,  0, // 3530-3532
 0,  0,  0, // 3533-3535
 0,  0,  0, // 3536-3538
 0,  0,  0, // 3539-3541
 0,  0,  0, // 3542-3544
 0,  0,  0, // 3545-3547
 0,  0,  0, // 3548-3550
 0,  0,  0, // 3551-3553
 0,  0,  0, // 3554-3556
 0,  0,  0, // 3557-3559
 0,  0,  0, // 3560-3562
 0,  0,  0, // 3563-3565
 0,  0,  0, // 3566-3568
 0,  0,  0, // 3569-3571
 0,  0,  0, // 3572-3574
 0,  0,  0, // 3575-3577
 0,  0,  0, // 3578-3580
 0,  0,  0, // 3581-3583
 0,  0,  0,  0, // 3584-3587
 0,  0,  0,  0, // 3588-3591
 0,  0, // 3592-3593
 0,  0,  0, // 3594-3596
 0,  0,  0, // 3597-3599
 0,  0,  0, // 3600-3602
 0,  0,  0, // 3603-3605
 0,  0,  0, // 3606-3608
 0,  0, // 3609-3610
 0,  0,  0, // 3611-3613
 0,  0,  0, // 3614-3616
 0,  0,  0, // 3617-3619
 0,  0,  0, // 3620-3622
 0,  0,  0,  0, // 3623-3626
 0,  0,  0,  0, // 3627-3630
 0,  0,  0, // 3631-3633
 0,  0,  0, // 3634-3636
 0,  0,  0, // 3637-3639
 0,  0,  0, // 3640-3642
 0,  0,  0, // 3643-3645
 0,  0,  0, // 3646-3648
 0,  0,  0, // 3649-3651
 0,  0,  0, // 3652-3654
 0,  0,  0, // 3655-3657
 0,  0, // 3658-3659
 0,  0,  0, // 3660-3662
 0,  0, // 3663-3664
 0,  0,  0, // 3665-3667
 0,  0,  0, // 3668-3670
 0,  0,  0, // 3671-3673
 0,  0,  0, // 3674-3676
 0,  0,  0, // 3677-3679
 0,  0,  0, // 3680-3682
 0,  0,  0, // 3683-3685
 0,  0,  0, // 3686-3688
 0,  0, // 3689-3690
 0,  0,  0, // 3691-3693
 0,  0,  0, // 3694-3696
 0,  0,  0, // 3697-3699
 0,  0,  0, // 3700-3702
 0,  0,  0, // 3703-3705
 0,  0,  0, // 3706-3708
 0,  0,  0, // 3709-3711
 0,  0, // 3712-3713
 0,  0,  0, // 3714-3716
 0,  0,  0, // 3717-3719
 0,  0,  0, // 3720-3722
 0,  0,  0, // 3723-3725
 0,  0,  0, // 3726-3728
 0,  0,  0, // 3729-3731
 0,  0,  0, // 3732-3734
 0,  0,  0, // 3735-3737
 0,  0,  0, // 3738-3740
 0,  0,  0, // 3741-3743
 0,  0,  0, // 3744-3746
 0,  0,  0, // 3747-3749
 0,  0,  0, // 3750-3752
 0,  0,  0, // 3753-3755
 0,  0,  0, // 3756-3758
 0,  0,  0, // 3759-3761
 0,  0,  0, // 3762-3764
 0,  0,  0,  0, // 3765-3768
 0,  0,  0,  0, // 3769-3772
 0,  0,  0,  0, // 3773-3776
 0,  0,  0,  0, // 3777-3780
 0,  0,  0,  0, // 3781-3784
 0,  0,  0,  0, // 3785-3788
 0,  0,  0,  0, // 3789-3792
 0,  0,  0,  0, // 3793-3796
 0,  0,  0,  0, // 3797-3800
 0,  0,  0,  0, // 3801-3804
 0,  0,  0,  0, // 3805-3808
 0,  0,  0, // 3809-3811
 0,  0,  0, // 3812-3814
 0,  0,  0, // 3815-3817
 0,  0,  0, // 3818-3820
 0,  0,  0, // 3821-3823
 0,  0,  0, // 3824-3826
 0,  0,  0, // 3827-3829
 0,  0,  0, // 3830-3832
 0,  0,  0, // 3833-3835
 0,  0,  0, // 3836-3838
 0,  0,  0, // 3839-3841
 0,  0,  0, // 3842-3844
 0,  0,  0, // 3845-3847
 0,  0,  0, // 3848-3850
 0,  0,  0, // 3851-3853
 0,  0, // 3854-3855
 0,  0,  0, // 3856-3858
 0 // End bypass tables
};

static const llvm::InstrItinerary PPC440Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 1, 6, 0, 0 }, // 1 IIC_LdStSync
  { 1, 6, 11, 1, 4 }, // 2 IIC_IntSimple
  { 1, 6, 11, 4, 7 }, // 3 IIC_IntGeneral
  { 1, 11, 16, 7, 9 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 6, 11, 9, 12 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 11, 16, 12, 15 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 16, 21, 15, 17 }, // 12 IIC_LdStDCBF
  { 1, 21, 26, 17, 20 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 26, 31, 20, 23 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 31, 40, 23, 26 }, // 17 IIC_FPAddSub
  { 1, 40, 49, 26, 29 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 31, 40, 29, 32 }, // 20 IIC_FPCompare
  { 1, 31, 40, 32, 35 }, // 21 IIC_FPGeneral
  { 0, 0, 0, 0, 0 }, // 22 IIC_VecGeneral
  { 0, 0, 0, 0, 0 }, // 23 IIC_VecComplex
  { 1, 21, 26, 35, 38 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 49, 58, 38, 41 }, // 26 IIC_FPDivS
  { 1, 31, 40, 41, 45 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 16, 21, 45, 48 }, // 30 IIC_LdStICBI
  { 1, 6, 11, 48, 52 }, // 31 IIC_IntISEL
  { 1, 58, 74, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 74, 81, 52, 55 }, // 33 IIC_LdStLWARX
  { 1, 21, 26, 55, 59 }, // 34 IIC_LdStLoadUpd
  { 1, 21, 26, 59, 63 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 21, 26, 63, 66 }, // 39 IIC_LdStLFD
  { 1, 16, 21, 66, 70 }, // 40 IIC_LdStLFDU
  { 1, 16, 21, 70, 74 }, // 41 IIC_LdStLFDUX
  { 1, 16, 21, 74, 77 }, // 42 IIC_LdStLHA
  { 1, 16, 21, 77, 80 }, // 43 IIC_LdStLHAU
  { 1, 16, 21, 80, 83 }, // 44 IIC_LdStLHAUX
  { 1, 16, 21, 83, 86 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 11, 16, 86, 89 }, // 48 IIC_BrMCR
  { 1, 11, 16, 89, 92 }, // 49 IIC_BrMCRX
  { 1, 11, 16, 92, 94 }, // 50 IIC_SprMFCR
  { 1, 81, 86, 94, 96 }, // 51 IIC_SprMFSPR
  { 1, 11, 16, 96, 99 }, // 52 IIC_IntMFFS
  { 1, 11, 16, 99, 101 }, // 53 IIC_SprMFMSR
  { 0, 0, 0, 0, 0 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 1, 11, 16, 101, 103 }, // 56 IIC_SprMFSR
  { 1, 81, 86, 103, 105 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 81, 86, 105, 107 }, // 59 IIC_SprMTSPR
  { 1, 11, 16, 107, 110 }, // 60 IIC_IntMTFSB0
  { 1, 11, 16, 110, 112 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 1, 81, 86, 112, 114 }, // 64 IIC_SprMTSR
  { 1, 11, 16, 114, 117 }, // 65 IIC_IntMulHW
  { 1, 11, 16, 117, 120 }, // 66 IIC_IntMulHWU
  { 1, 11, 16, 120, 123 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 1, 16, 21, 123, 126 }, // 69 IIC_LdStSTFD
  { 1, 16, 21, 126, 130 }, // 70 IIC_LdStSTFDU
  { 1, 11, 16, 130, 132 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 6, 11, 132, 135 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 6, 11, 135, 138 }, // 82 IIC_IntShift
  { 1, 74, 81, 138, 141 }, // 83 IIC_LdStSTWCX
  { 1, 21, 26, 141, 145 }, // 84 IIC_LdStStoreUpd
  { 1, 21, 26, 145, 148 }, // 85 IIC_LdStSTD
  { 1, 74, 81, 148, 151 }, // 86 IIC_LdStSTDCX
  { 1, 21, 26, 151, 155 }, // 87 IIC_LdStSTDU
  { 1, 21, 26, 155, 159 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 11, 16, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 11, 16, 159, 161 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 0, 0, 0, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 81, 86, 161, 163 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 11, 16, 163, 166 }, // 101 MTFSB0_MTFSB1
  { 1, 11, 16, 166, 169 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 6, 11, 169, 172 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 11, 16, 172, 174 }, // 105 TW_TWI
  { 1, 31, 40, 174, 177 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 31, 40, 177, 180 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 6, 11, 180, 183 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 0, 0, 0, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 6, 11, 183, 186 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 6, 11, 186, 189 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 6, 11, 189, 192 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 11, 16, 192, 195 }, // 118 MCRF
  { 1, 11, 16, 195, 198 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 11, 16, 198, 200 }, // 121 RFEBB
  { 1, 81, 86, 200, 202 }, // 122 TBEGIN_TRECHKPT
  { 1, 21, 26, 202, 205 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 6, 11, 205, 208 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 0, 0, 0, 0, 0 }, // 127 MFOCRF_MFOCRF8
  { 1, 11, 16, 208, 211 }, // 128 MTOCRF_MTOCRF8
  { 1, 11, 16, 211, 214 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 6, 11, 214, 217 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 31, 40, 217, 220 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 6, 11, 220, 223 }, // 132 SRAW_SRAWI
  { 1, 6, 11, 223, 227 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 81, 86, 227, 229 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 21, 26, 229, 232 }, // 136 MFVSCR
  { 1, 21, 26, 232, 235 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 0, 0, 0, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 31, 40, 235, 238 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 0, 0, 0, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 11, 16, 238, 241 }, // 145 MULHD_MULHW_MULLW
  { 1, 11, 16, 241, 244 }, // 146 MULHDU_MULHWU
  { 1, 11, 16, 244, 247 }, // 147 MULLI_MULLI8
  { 1, 31, 40, 247, 250 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 31, 40, 250, 253 }, // 149 FADD_FSUB
  { 1, 31, 40, 253, 257 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 31, 40, 257, 260 }, // 152 FSELDo_FSELSo
  { 1, 11, 16, 260, 263 }, // 153 MULHDUo_MULHWUo
  { 1, 11, 16, 263, 266 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 31, 40, 266, 269 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 31, 40, 269, 272 }, // 157 FADDo_FSUBo
  { 1, 31, 40, 272, 276 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 21, 26, 276, 279 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 0, 0, 0, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 31, 40, 279, 282 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 21, 26, 282, 285 }, // 171 LXVL_LXVLL
  { 1, 21, 26, 285, 288 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 21, 26, 288, 291 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 16, 21, 291, 293 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 16, 21, 293, 296 }, // 178 ICBI_ICBIEP
  { 1, 21, 26, 296, 299 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 74, 81, 299, 302 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 58, 74, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 11, 16, 0, 0 }, // 186 TLBSYNC
  { 1, 1, 6, 0, 0 }, // 187 SYNC
  { 1, 16, 21, 302, 305 }, // 188 LMW
  { 1, 21, 26, 305, 308 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 21, 26, 308, 312 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 21, 26, 312, 316 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 81, 86, 316, 318 }, // 198 TEND
  { 1, 74, 81, 318, 321 }, // 199 STBCX_STHCX_STWCX
  { 1, 74, 81, 321, 324 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 16, 21, 324, 327 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 81, 86, 327, 329 }, // 205 TCHECK
  { 1, 21, 26, 329, 332 }, // 206 LFIWAX
  { 1, 21, 26, 332, 335 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 21, 26, 335, 338 }, // 209 LFSX_LFS
  { 1, 21, 26, 338, 341 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 16, 21, 341, 344 }, // 212 LHAU_LHAU8
  { 1, 16, 21, 344, 347 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 21, 26, 347, 350 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 16, 21, 350, 353 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 21, 26, 353, 356 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 21, 26, 356, 359 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 21, 26, 359, 362 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 16, 21, 362, 365 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 21, 26, 365, 368 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 16, 21, 368, 371 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 21, 26, 371, 374 }, // 225 STXVL_STXVLL
  { 1, 81, 86, 374, 376 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 6, 11, 376, 379 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 81, 86, 379, 381 }, // 230 MFTB_MFTB8
  { 1, 81, 86, 381, 383 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 11, 16, 383, 385 }, // 232 MFMSR
  { 1, 11, 16, 385, 387 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 81, 86, 387, 389 }, // 235 MTSPR_MTSPR8
  { 1, 26, 31, 389, 392 }, // 236 DIVW_DIVWU_MODSW
  { 1, 26, 31, 392, 395 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 26, 31, 395, 398 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 26, 31, 398, 401 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 11, 16, 401, 404 }, // 244 MTCRF_MTCRF8
  { 1, 6, 11, 404, 407 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 31, 40, 407, 410 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 11, 16, 410, 413 }, // 247 MCRFS
  { 1, 11, 16, 413, 416 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 11, 16, 416, 419 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 6, 11, 419, 422 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 6, 11, 422, 425 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 6, 11, 425, 428 }, // 254 SRAWIo_SRAWo
  { 1, 11, 16, 428, 431 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 11, 16, 431, 433 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 40, 49, 433, 436 }, // 259 FDIV
  { 1, 40, 49, 436, 439 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 40, 49, 439, 442 }, // 269 XSDIVDP
  { 1, 49, 58, 442, 445 }, // 270 FDIVS
  { 1, 49, 58, 445, 448 }, // 271 FDIVSo
  { 1, 49, 58, 448, 451 }, // 272 XSDIVSP
  { 1, 49, 58, 451, 454 }, // 273 XVDIVSP
  { 1, 40, 49, 454, 457 }, // 274 XVDIVDP
  { 1, 16, 21, 457, 461 }, // 275 LFSU
  { 1, 16, 21, 461, 465 }, // 276 LFSUX
  { 1, 16, 21, 465, 469 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 21, 26, 469, 473 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 21, 26, 473, 477 }, // 279 STDU
  { 1, 21, 26, 477, 481 }, // 280 STDUX
  { 1, 21, 26, 481, 485 }, // 281 LBZU_LBZU8
  { 1, 21, 26, 485, 489 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 1, 16, 21, 489, 493 }, // 285 LFDU
  { 1, 16, 21, 493, 497 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 11, 16, 497, 499 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 11, 16, 499, 501 }, // 289 ADDPCIS
  { 1, 21, 26, 501, 504 }, // 290 LDAT_LWAT
  { 1, 21, 26, 504, 507 }, // 291 STDAT_STWAT
  { 1, 6, 11, 507, 510 }, // 292 BRINC
  { 0, 0, 0, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 0, 0, 0, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 21, 26, 510, 513 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 21, 26, 513, 516 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 11, 16, 516, 518 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 1, 11, 16, 518, 520 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 21, 26, 520, 523 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 21, 26, 523, 526 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 21, 26, 526, 529 }, // 302 LFDEPX
  { 1, 16, 21, 529, 532 }, // 303 STFDEPX
  { 1, 11, 16, 532, 534 }, // 304 MFSR_MFSRIN
  { 1, 81, 86, 534, 536 }, // 305 MTSR_MTSRIN
  { 1, 81, 86, 536, 538 }, // 306 MFDCR
  { 1, 81, 86, 538, 540 }, // 307 MTDCR
  { 1, 6, 11, 540, 543 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 11, 16, 543, 545 }, // 310 WRTEE_WRTEEI
  { 1, 1, 6, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 16, 21, 545, 547 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary G3Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 86, 87, 0, 0 }, // 1 IIC_LdStSync
  { 1, 87, 88, 0, 0 }, // 2 IIC_IntSimple
  { 1, 87, 88, 0, 0 }, // 3 IIC_IntGeneral
  { 1, 88, 89, 0, 0 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 87, 88, 0, 0 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 89, 90, 0, 0 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 86, 87, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 90, 91, 0, 0 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 91, 92, 0, 0 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 92, 93, 0, 0 }, // 17 IIC_FPAddSub
  { 1, 93, 94, 0, 0 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 92, 93, 0, 0 }, // 20 IIC_FPCompare
  { 1, 92, 93, 0, 0 }, // 21 IIC_FPGeneral
  { 0, 0, 0, 0, 0 }, // 22 IIC_VecGeneral
  { 0, 0, 0, 0, 0 }, // 23 IIC_VecComplex
  { 1, 90, 91, 0, 0 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 94, 95, 0, 0 }, // 26 IIC_FPDivS
  { 1, 95, 96, 0, 0 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 86, 87, 0, 0 }, // 30 IIC_LdStICBI
  { 0, 0, 0, 0, 0 }, // 31 IIC_IntISEL
  { 1, 96, 97, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 86, 87, 0, 0 }, // 33 IIC_LdStLWARX
  { 1, 90, 91, 0, 0 }, // 34 IIC_LdStLoadUpd
  { 1, 90, 91, 0, 0 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 90, 91, 0, 0 }, // 39 IIC_LdStLFD
  { 1, 90, 91, 0, 0 }, // 40 IIC_LdStLFDU
  { 1, 90, 91, 0, 0 }, // 41 IIC_LdStLFDUX
  { 1, 90, 91, 0, 0 }, // 42 IIC_LdStLHA
  { 1, 90, 91, 0, 0 }, // 43 IIC_LdStLHAU
  { 1, 90, 91, 0, 0 }, // 44 IIC_LdStLHAUX
  { 1, 97, 98, 0, 0 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 89, 90, 0, 0 }, // 48 IIC_BrMCR
  { 1, 89, 90, 0, 0 }, // 49 IIC_BrMCRX
  { 1, 89, 90, 0, 0 }, // 50 IIC_SprMFCR
  { 1, 98, 99, 0, 0 }, // 51 IIC_SprMFSPR
  { 1, 92, 93, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 89, 90, 0, 0 }, // 53 IIC_SprMFMSR
  { 0, 0, 0, 0, 0 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 1, 98, 99, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 98, 99, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 96, 97, 0, 0 }, // 59 IIC_SprMTSPR
  { 1, 99, 100, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 89, 90, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 1, 96, 97, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 100, 101, 0, 0 }, // 65 IIC_IntMulHW
  { 1, 101, 102, 0, 0 }, // 66 IIC_IntMulHWU
  { 1, 102, 103, 0, 0 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 1, 90, 91, 0, 0 }, // 69 IIC_LdStSTFD
  { 1, 90, 91, 0, 0 }, // 70 IIC_LdStSTFDU
  { 1, 96, 97, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 87, 88, 0, 0 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 87, 88, 0, 0 }, // 82 IIC_IntShift
  { 1, 103, 104, 0, 0 }, // 83 IIC_LdStSTWCX
  { 1, 90, 91, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 0, 0, 0, 0, 0 }, // 85 IIC_LdStSTD
  { 0, 0, 0, 0, 0 }, // 86 IIC_LdStSTDCX
  { 0, 0, 0, 0, 0 }, // 87 IIC_LdStSTDU
  { 0, 0, 0, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 98, 99, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 104, 105, 0, 0 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 0, 0, 0, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 96, 97, 0, 0 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 99, 100, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 1, 92, 93, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 87, 88, 0, 0 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 104, 105, 0, 0 }, // 105 TW_TWI
  { 1, 92, 93, 0, 0 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 92, 93, 0, 0 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 87, 88, 0, 0 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 0, 0, 0, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 87, 88, 0, 0 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 87, 88, 0, 0 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 87, 88, 0, 0 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 89, 90, 0, 0 }, // 118 MCRF
  { 1, 89, 90, 0, 0 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 88, 89, 0, 0 }, // 121 RFEBB
  { 1, 96, 97, 0, 0 }, // 122 TBEGIN_TRECHKPT
  { 1, 90, 91, 0, 0 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 87, 88, 0, 0 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 0, 0, 0, 0, 0 }, // 127 MFOCRF_MFOCRF8
  { 1, 89, 90, 0, 0 }, // 128 MTOCRF_MTOCRF8
  { 1, 89, 90, 0, 0 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 87, 88, 0, 0 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 92, 93, 0, 0 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 87, 88, 0, 0 }, // 132 SRAW_SRAWI
  { 0, 0, 0, 0, 0 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 96, 97, 0, 0 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 90, 91, 0, 0 }, // 136 MFVSCR
  { 1, 90, 91, 0, 0 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 0, 0, 0, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 92, 93, 0, 0 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 0, 0, 0, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 100, 101, 0, 0 }, // 145 MULHD_MULHW_MULLW
  { 1, 101, 102, 0, 0 }, // 146 MULHDU_MULHWU
  { 1, 102, 103, 0, 0 }, // 147 MULLI_MULLI8
  { 1, 92, 93, 0, 0 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 92, 93, 0, 0 }, // 149 FADD_FSUB
  { 1, 95, 96, 0, 0 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 92, 93, 0, 0 }, // 152 FSELDo_FSELSo
  { 1, 101, 102, 0, 0 }, // 153 MULHDUo_MULHWUo
  { 1, 100, 101, 0, 0 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 92, 93, 0, 0 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 92, 93, 0, 0 }, // 157 FADDo_FSUBo
  { 1, 95, 96, 0, 0 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 90, 91, 0, 0 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 0, 0, 0, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 92, 93, 0, 0 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 90, 91, 0, 0 }, // 171 LXVL_LXVLL
  { 1, 90, 91, 0, 0 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 90, 91, 0, 0 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 86, 87, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 86, 87, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 90, 91, 0, 0 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 86, 87, 0, 0 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 96, 97, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 98, 99, 0, 0 }, // 186 TLBSYNC
  { 1, 86, 87, 0, 0 }, // 187 SYNC
  { 1, 97, 98, 0, 0 }, // 188 LMW
  { 1, 90, 91, 0, 0 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 90, 91, 0, 0 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 90, 91, 0, 0 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 96, 97, 0, 0 }, // 198 TEND
  { 1, 103, 104, 0, 0 }, // 199 STBCX_STHCX_STWCX
  { 0, 0, 0, 0, 0 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 90, 91, 0, 0 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 96, 97, 0, 0 }, // 205 TCHECK
  { 1, 90, 91, 0, 0 }, // 206 LFIWAX
  { 1, 90, 91, 0, 0 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 90, 91, 0, 0 }, // 209 LFSX_LFS
  { 1, 90, 91, 0, 0 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 90, 91, 0, 0 }, // 212 LHAU_LHAU8
  { 1, 90, 91, 0, 0 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 90, 91, 0, 0 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 90, 91, 0, 0 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 90, 91, 0, 0 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 0, 0, 0, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 90, 91, 0, 0 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 97, 98, 0, 0 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 90, 91, 0, 0 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 90, 91, 0, 0 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 90, 91, 0, 0 }, // 225 STXVL_STXVLL
  { 1, 96, 97, 0, 0 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 87, 88, 0, 0 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 98, 99, 0, 0 }, // 230 MFTB_MFTB8
  { 1, 98, 99, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 89, 90, 0, 0 }, // 232 MFMSR
  { 1, 89, 90, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 96, 97, 0, 0 }, // 235 MTSPR_MTSPR8
  { 1, 91, 92, 0, 0 }, // 236 DIVW_DIVWU_MODSW
  { 1, 91, 92, 0, 0 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 91, 92, 0, 0 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 91, 92, 0, 0 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 89, 90, 0, 0 }, // 244 MTCRF_MTCRF8
  { 1, 87, 88, 0, 0 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 92, 93, 0, 0 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 89, 90, 0, 0 }, // 247 MCRFS
  { 1, 92, 93, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 99, 100, 0, 0 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 87, 88, 0, 0 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 87, 88, 0, 0 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 87, 88, 0, 0 }, // 254 SRAWIo_SRAWo
  { 1, 92, 93, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 89, 90, 0, 0 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 93, 94, 0, 0 }, // 259 FDIV
  { 1, 93, 94, 0, 0 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 93, 94, 0, 0 }, // 269 XSDIVDP
  { 1, 94, 95, 0, 0 }, // 270 FDIVS
  { 1, 94, 95, 0, 0 }, // 271 FDIVSo
  { 1, 94, 95, 0, 0 }, // 272 XSDIVSP
  { 1, 94, 95, 0, 0 }, // 273 XVDIVSP
  { 1, 93, 94, 0, 0 }, // 274 XVDIVDP
  { 1, 90, 91, 0, 0 }, // 275 LFSU
  { 1, 90, 91, 0, 0 }, // 276 LFSUX
  { 1, 90, 91, 0, 0 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 90, 91, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 0, 0, 0, 0, 0 }, // 279 STDU
  { 0, 0, 0, 0, 0 }, // 280 STDUX
  { 1, 90, 91, 0, 0 }, // 281 LBZU_LBZU8
  { 1, 90, 91, 0, 0 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 1, 90, 91, 0, 0 }, // 285 LFDU
  { 1, 90, 91, 0, 0 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 88, 89, 0, 0 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 88, 89, 0, 0 }, // 289 ADDPCIS
  { 1, 90, 91, 0, 0 }, // 290 LDAT_LWAT
  { 1, 90, 91, 0, 0 }, // 291 STDAT_STWAT
  { 1, 87, 88, 0, 0 }, // 292 BRINC
  { 0, 0, 0, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 0, 0, 0, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 90, 91, 0, 0 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 90, 91, 0, 0 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 88, 89, 0, 0 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 1, 96, 97, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 90, 91, 0, 0 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 90, 91, 0, 0 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 90, 91, 0, 0 }, // 302 LFDEPX
  { 1, 90, 91, 0, 0 }, // 303 STFDEPX
  { 1, 98, 99, 0, 0 }, // 304 MFSR_MFSRIN
  { 1, 96, 97, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 98, 99, 0, 0 }, // 306 MFDCR
  { 1, 96, 97, 0, 0 }, // 307 MTDCR
  { 1, 87, 88, 0, 0 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 89, 90, 0, 0 }, // 310 WRTEE_WRTEEI
  { 1, 86, 87, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 86, 87, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary G4Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 105, 106, 0, 0 }, // 1 IIC_LdStSync
  { 1, 106, 107, 0, 0 }, // 2 IIC_IntSimple
  { 1, 106, 107, 0, 0 }, // 3 IIC_IntGeneral
  { 1, 107, 108, 0, 0 }, // 4 IIC_BrB
  { 1, 108, 109, 0, 0 }, // 5 IIC_VecFP
  { 1, 106, 107, 0, 0 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 109, 110, 0, 0 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 110, 111, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 110, 111, 0, 0 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 111, 112, 0, 0 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 112, 113, 0, 0 }, // 17 IIC_FPAddSub
  { 1, 113, 114, 0, 0 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 112, 113, 0, 0 }, // 20 IIC_FPCompare
  { 1, 112, 113, 0, 0 }, // 21 IIC_FPGeneral
  { 1, 114, 115, 0, 0 }, // 22 IIC_VecGeneral
  { 1, 115, 116, 0, 0 }, // 23 IIC_VecComplex
  { 1, 110, 111, 0, 0 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 116, 117, 0, 0 }, // 26 IIC_FPDivS
  { 1, 112, 113, 0, 0 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 110, 111, 0, 0 }, // 30 IIC_LdStICBI
  { 0, 0, 0, 0, 0 }, // 31 IIC_IntISEL
  { 1, 117, 118, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 118, 119, 0, 0 }, // 33 IIC_LdStLWARX
  { 1, 110, 111, 0, 0 }, // 34 IIC_LdStLoadUpd
  { 1, 110, 111, 0, 0 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 110, 111, 0, 0 }, // 39 IIC_LdStLFD
  { 1, 110, 111, 0, 0 }, // 40 IIC_LdStLFDU
  { 1, 110, 111, 0, 0 }, // 41 IIC_LdStLFDUX
  { 1, 110, 111, 0, 0 }, // 42 IIC_LdStLHA
  { 1, 110, 111, 0, 0 }, // 43 IIC_LdStLHAU
  { 1, 110, 111, 0, 0 }, // 44 IIC_LdStLHAUX
  { 1, 119, 120, 0, 0 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 109, 110, 0, 0 }, // 48 IIC_BrMCR
  { 1, 109, 110, 0, 0 }, // 49 IIC_BrMCRX
  { 1, 109, 110, 0, 0 }, // 50 IIC_SprMFCR
  { 1, 120, 121, 0, 0 }, // 51 IIC_SprMFSPR
  { 1, 121, 122, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 109, 110, 0, 0 }, // 53 IIC_SprMFMSR
  { 0, 0, 0, 0, 0 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 1, 120, 121, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 109, 110, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 117, 118, 0, 0 }, // 59 IIC_SprMTSPR
  { 1, 121, 122, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 109, 110, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 1, 117, 118, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 122, 123, 0, 0 }, // 65 IIC_IntMulHW
  { 1, 123, 124, 0, 0 }, // 66 IIC_IntMulHWU
  { 1, 124, 125, 0, 0 }, // 67 IIC_IntMulLI
  { 1, 125, 126, 0, 0 }, // 68 IIC_VecPerm
  { 1, 110, 111, 0, 0 }, // 69 IIC_LdStSTFD
  { 1, 110, 111, 0, 0 }, // 70 IIC_LdStSTFDU
  { 1, 117, 118, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 106, 107, 0, 0 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 106, 107, 0, 0 }, // 82 IIC_IntShift
  { 1, 126, 127, 0, 0 }, // 83 IIC_LdStSTWCX
  { 1, 110, 111, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 0, 0, 0, 0, 0 }, // 85 IIC_LdStSTD
  { 0, 0, 0, 0, 0 }, // 86 IIC_LdStSTDCX
  { 0, 0, 0, 0, 0 }, // 87 IIC_LdStSTDU
  { 0, 0, 0, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 127, 128, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 128, 129, 0, 0 }, // 95 IIC_IntTrapW
  { 1, 114, 115, 0, 0 }, // 96 IIC_VecFPCompare
  { 1, 114, 115, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 1, 108, 109, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 1, 125, 126, 0, 0 }, // 99 XXSEL
  { 1, 117, 118, 0, 0 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 121, 122, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 1, 121, 122, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 106, 107, 0, 0 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 128, 129, 0, 0 }, // 105 TW_TWI
  { 1, 112, 113, 0, 0 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 1, 108, 109, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 1, 108, 109, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 112, 113, 0, 0 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 106, 107, 0, 0 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 114, 115, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 106, 107, 0, 0 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 106, 107, 0, 0 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 106, 107, 0, 0 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 109, 110, 0, 0 }, // 118 MCRF
  { 1, 109, 110, 0, 0 }, // 119 MCRXRX
  { 1, 108, 109, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 107, 108, 0, 0 }, // 121 RFEBB
  { 1, 117, 118, 0, 0 }, // 122 TBEGIN_TRECHKPT
  { 1, 110, 111, 0, 0 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 106, 107, 0, 0 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 0, 0, 0, 0, 0 }, // 127 MFOCRF_MFOCRF8
  { 1, 109, 110, 0, 0 }, // 128 MTOCRF_MTOCRF8
  { 1, 109, 110, 0, 0 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 106, 107, 0, 0 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 112, 113, 0, 0 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 106, 107, 0, 0 }, // 132 SRAW_SRAWI
  { 0, 0, 0, 0, 0 }, // 133 ISEL_ISEL8
  { 1, 108, 109, 0, 0 }, // 134 XSIEXPDP
  { 1, 117, 118, 0, 0 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 110, 111, 0, 0 }, // 136 MFVSCR
  { 1, 110, 111, 0, 0 }, // 137 MTVSCR
  { 1, 114, 115, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 114, 115, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 1, 108, 109, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 112, 113, 0, 0 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 1, 108, 109, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 114, 115, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 122, 123, 0, 0 }, // 145 MULHD_MULHW_MULLW
  { 1, 123, 124, 0, 0 }, // 146 MULHDU_MULHWU
  { 1, 124, 125, 0, 0 }, // 147 MULLI_MULLI8
  { 1, 112, 113, 0, 0 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 112, 113, 0, 0 }, // 149 FADD_FSUB
  { 1, 112, 113, 0, 0 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 1, 108, 109, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 112, 113, 0, 0 }, // 152 FSELDo_FSELSo
  { 1, 123, 124, 0, 0 }, // 153 MULHDUo_MULHWUo
  { 1, 122, 123, 0, 0 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 112, 113, 0, 0 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 112, 113, 0, 0 }, // 157 FADDo_FSUBo
  { 1, 112, 113, 0, 0 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 1, 108, 109, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 110, 111, 0, 0 }, // 160 LVSL_LVSR
  { 1, 125, 126, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 1, 108, 109, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 114, 115, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 112, 113, 0, 0 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 1, 108, 109, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 1, 108, 109, 0, 0 }, // 166 BCDCTSQo
  { 1, 108, 109, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 1, 108, 109, 0, 0 }, // 168 BCDCFSQo
  { 1, 108, 109, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 1, 108, 109, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 110, 111, 0, 0 }, // 171 LXVL_LXVLL
  { 1, 110, 111, 0, 0 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 110, 111, 0, 0 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 110, 111, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 110, 111, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 110, 111, 0, 0 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 118, 119, 0, 0 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 117, 118, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 127, 128, 0, 0 }, // 186 TLBSYNC
  { 1, 105, 106, 0, 0 }, // 187 SYNC
  { 1, 119, 120, 0, 0 }, // 188 LMW
  { 1, 110, 111, 0, 0 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 110, 111, 0, 0 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 110, 111, 0, 0 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 117, 118, 0, 0 }, // 198 TEND
  { 1, 126, 127, 0, 0 }, // 199 STBCX_STHCX_STWCX
  { 0, 0, 0, 0, 0 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 110, 111, 0, 0 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 117, 118, 0, 0 }, // 205 TCHECK
  { 1, 110, 111, 0, 0 }, // 206 LFIWAX
  { 1, 110, 111, 0, 0 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 110, 111, 0, 0 }, // 209 LFSX_LFS
  { 1, 110, 111, 0, 0 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 110, 111, 0, 0 }, // 212 LHAU_LHAU8
  { 1, 110, 111, 0, 0 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 110, 111, 0, 0 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 110, 111, 0, 0 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 110, 111, 0, 0 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 0, 0, 0, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 110, 111, 0, 0 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 119, 120, 0, 0 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 110, 111, 0, 0 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 110, 111, 0, 0 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 110, 111, 0, 0 }, // 225 STXVL_STXVLL
  { 1, 117, 118, 0, 0 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 106, 107, 0, 0 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 109, 110, 0, 0 }, // 230 MFTB_MFTB8
  { 1, 120, 121, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 109, 110, 0, 0 }, // 232 MFMSR
  { 1, 109, 110, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 117, 118, 0, 0 }, // 235 MTSPR_MTSPR8
  { 1, 111, 112, 0, 0 }, // 236 DIVW_DIVWU_MODSW
  { 1, 111, 112, 0, 0 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 111, 112, 0, 0 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 111, 112, 0, 0 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 109, 110, 0, 0 }, // 244 MTCRF_MTCRF8
  { 1, 106, 107, 0, 0 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 112, 113, 0, 0 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 109, 110, 0, 0 }, // 247 MCRFS
  { 1, 121, 122, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 121, 122, 0, 0 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 106, 107, 0, 0 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 106, 107, 0, 0 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 106, 107, 0, 0 }, // 254 SRAWIo_SRAWo
  { 1, 121, 122, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 109, 110, 0, 0 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 113, 114, 0, 0 }, // 259 FDIV
  { 1, 113, 114, 0, 0 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 113, 114, 0, 0 }, // 269 XSDIVDP
  { 1, 116, 117, 0, 0 }, // 270 FDIVS
  { 1, 116, 117, 0, 0 }, // 271 FDIVSo
  { 1, 116, 117, 0, 0 }, // 272 XSDIVSP
  { 1, 116, 117, 0, 0 }, // 273 XVDIVSP
  { 1, 113, 114, 0, 0 }, // 274 XVDIVDP
  { 1, 110, 111, 0, 0 }, // 275 LFSU
  { 1, 110, 111, 0, 0 }, // 276 LFSUX
  { 1, 110, 111, 0, 0 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 110, 111, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 0, 0, 0, 0, 0 }, // 279 STDU
  { 0, 0, 0, 0, 0 }, // 280 STDUX
  { 1, 110, 111, 0, 0 }, // 281 LBZU_LBZU8
  { 1, 110, 111, 0, 0 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 1, 110, 111, 0, 0 }, // 285 LFDU
  { 1, 110, 111, 0, 0 }, // 286 LFDUX
  { 1, 108, 109, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 107, 108, 0, 0 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 107, 108, 0, 0 }, // 289 ADDPCIS
  { 1, 110, 111, 0, 0 }, // 290 LDAT_LWAT
  { 1, 110, 111, 0, 0 }, // 291 STDAT_STWAT
  { 1, 106, 107, 0, 0 }, // 292 BRINC
  { 1, 114, 115, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 115, 116, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 110, 111, 0, 0 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 110, 111, 0, 0 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 107, 108, 0, 0 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 1, 117, 118, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 110, 111, 0, 0 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 110, 111, 0, 0 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 110, 111, 0, 0 }, // 302 LFDEPX
  { 1, 110, 111, 0, 0 }, // 303 STFDEPX
  { 1, 120, 121, 0, 0 }, // 304 MFSR_MFSRIN
  { 1, 117, 118, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 120, 121, 0, 0 }, // 306 MFDCR
  { 1, 117, 118, 0, 0 }, // 307 MTDCR
  { 1, 106, 107, 0, 0 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 109, 110, 0, 0 }, // 310 WRTEE_WRTEEI
  { 1, 105, 106, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 110, 111, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary G4PlusItineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 129, 130, 0, 0 }, // 1 IIC_LdStSync
  { 1, 130, 131, 0, 0 }, // 2 IIC_IntSimple
  { 1, 130, 131, 0, 0 }, // 3 IIC_IntGeneral
  { 1, 131, 132, 0, 0 }, // 4 IIC_BrB
  { 1, 132, 133, 0, 0 }, // 5 IIC_VecFP
  { 1, 130, 131, 0, 0 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 133, 134, 0, 0 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 134, 135, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 134, 135, 0, 0 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 135, 136, 0, 0 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 136, 137, 0, 0 }, // 17 IIC_FPAddSub
  { 1, 137, 138, 0, 0 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 136, 137, 0, 0 }, // 20 IIC_FPCompare
  { 1, 136, 137, 0, 0 }, // 21 IIC_FPGeneral
  { 1, 138, 139, 0, 0 }, // 22 IIC_VecGeneral
  { 1, 139, 140, 0, 0 }, // 23 IIC_VecComplex
  { 1, 134, 135, 0, 0 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 140, 141, 0, 0 }, // 26 IIC_FPDivS
  { 1, 136, 137, 0, 0 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 141, 142, 0, 0 }, // 30 IIC_LdStICBI
  { 0, 0, 0, 0, 0 }, // 31 IIC_IntISEL
  { 1, 142, 143, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 134, 135, 0, 0 }, // 33 IIC_LdStLWARX
  { 1, 134, 135, 0, 0 }, // 34 IIC_LdStLoadUpd
  { 1, 134, 135, 0, 0 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 143, 144, 0, 0 }, // 39 IIC_LdStLFD
  { 1, 143, 144, 0, 0 }, // 40 IIC_LdStLFDU
  { 1, 143, 144, 0, 0 }, // 41 IIC_LdStLFDUX
  { 1, 134, 135, 0, 0 }, // 42 IIC_LdStLHA
  { 1, 134, 135, 0, 0 }, // 43 IIC_LdStLHAU
  { 1, 134, 135, 0, 0 }, // 44 IIC_LdStLHAUX
  { 1, 144, 145, 0, 0 }, // 45 IIC_LdStLMW
  { 1, 134, 135, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 133, 134, 0, 0 }, // 48 IIC_BrMCR
  { 1, 133, 134, 0, 0 }, // 49 IIC_BrMCRX
  { 1, 133, 134, 0, 0 }, // 50 IIC_SprMFCR
  { 1, 145, 146, 0, 0 }, // 51 IIC_SprMFSPR
  { 1, 136, 137, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 141, 142, 0, 0 }, // 53 IIC_SprMFMSR
  { 0, 0, 0, 0, 0 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 1, 145, 146, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 146, 147, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 133, 134, 0, 0 }, // 59 IIC_SprMTSPR
  { 1, 136, 137, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 133, 134, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 1, 133, 134, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 145, 146, 0, 0 }, // 65 IIC_IntMulHW
  { 1, 145, 146, 0, 0 }, // 66 IIC_IntMulHWU
  { 1, 141, 142, 0, 0 }, // 67 IIC_IntMulLI
  { 1, 147, 148, 0, 0 }, // 68 IIC_VecPerm
  { 1, 134, 135, 0, 0 }, // 69 IIC_LdStSTFD
  { 1, 134, 135, 0, 0 }, // 70 IIC_LdStSTFDU
  { 1, 130, 131, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 130, 131, 0, 0 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 148, 149, 0, 0 }, // 82 IIC_IntShift
  { 1, 134, 135, 0, 0 }, // 83 IIC_LdStSTWCX
  { 1, 134, 135, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 1, 134, 135, 0, 0 }, // 85 IIC_LdStSTD
  { 1, 134, 135, 0, 0 }, // 86 IIC_LdStSTDCX
  { 1, 134, 135, 0, 0 }, // 87 IIC_LdStSTDU
  { 1, 134, 135, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 134, 135, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 148, 149, 0, 0 }, // 95 IIC_IntTrapW
  { 1, 149, 150, 0, 0 }, // 96 IIC_VecFPCompare
  { 1, 138, 139, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 1, 132, 133, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 1, 147, 148, 0, 0 }, // 99 XXSEL
  { 1, 133, 134, 0, 0 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 136, 137, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 1, 136, 137, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 130, 131, 0, 0 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 148, 149, 0, 0 }, // 105 TW_TWI
  { 1, 136, 137, 0, 0 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 1, 132, 133, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 1, 132, 133, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 136, 137, 0, 0 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 130, 131, 0, 0 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 138, 139, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 130, 131, 0, 0 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 130, 131, 0, 0 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 130, 131, 0, 0 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 133, 134, 0, 0 }, // 118 MCRF
  { 1, 133, 134, 0, 0 }, // 119 MCRXRX
  { 1, 132, 133, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 131, 132, 0, 0 }, // 121 RFEBB
  { 1, 133, 134, 0, 0 }, // 122 TBEGIN_TRECHKPT
  { 1, 134, 135, 0, 0 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 130, 131, 0, 0 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 0, 0, 0, 0, 0 }, // 127 MFOCRF_MFOCRF8
  { 1, 133, 134, 0, 0 }, // 128 MTOCRF_MTOCRF8
  { 1, 133, 134, 0, 0 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 130, 131, 0, 0 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 136, 137, 0, 0 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 148, 149, 0, 0 }, // 132 SRAW_SRAWI
  { 0, 0, 0, 0, 0 }, // 133 ISEL_ISEL8
  { 1, 132, 133, 0, 0 }, // 134 XSIEXPDP
  { 1, 133, 134, 0, 0 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 134, 135, 0, 0 }, // 136 MFVSCR
  { 1, 134, 135, 0, 0 }, // 137 MTVSCR
  { 1, 149, 150, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 138, 139, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 1, 132, 133, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 136, 137, 0, 0 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 1, 132, 133, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 138, 139, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 145, 146, 0, 0 }, // 145 MULHD_MULHW_MULLW
  { 1, 145, 146, 0, 0 }, // 146 MULHDU_MULHWU
  { 1, 141, 142, 0, 0 }, // 147 MULLI_MULLI8
  { 1, 136, 137, 0, 0 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 136, 137, 0, 0 }, // 149 FADD_FSUB
  { 1, 136, 137, 0, 0 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 1, 132, 133, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 136, 137, 0, 0 }, // 152 FSELDo_FSELSo
  { 1, 145, 146, 0, 0 }, // 153 MULHDUo_MULHWUo
  { 1, 145, 146, 0, 0 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 136, 137, 0, 0 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 136, 137, 0, 0 }, // 157 FADDo_FSUBo
  { 1, 136, 137, 0, 0 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 1, 132, 133, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 134, 135, 0, 0 }, // 160 LVSL_LVSR
  { 1, 147, 148, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 1, 132, 133, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 138, 139, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 136, 137, 0, 0 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 1, 132, 133, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 1, 132, 133, 0, 0 }, // 166 BCDCTSQo
  { 1, 132, 133, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 1, 132, 133, 0, 0 }, // 168 BCDCFSQo
  { 1, 132, 133, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 1, 132, 133, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 134, 135, 0, 0 }, // 171 LXVL_LXVLL
  { 1, 134, 135, 0, 0 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 143, 144, 0, 0 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 134, 135, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 141, 142, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 134, 135, 0, 0 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 134, 135, 0, 0 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 142, 143, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 134, 135, 0, 0 }, // 186 TLBSYNC
  { 1, 129, 130, 0, 0 }, // 187 SYNC
  { 1, 144, 145, 0, 0 }, // 188 LMW
  { 1, 143, 144, 0, 0 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 134, 135, 0, 0 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 134, 135, 0, 0 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 133, 134, 0, 0 }, // 198 TEND
  { 1, 134, 135, 0, 0 }, // 199 STBCX_STHCX_STWCX
  { 1, 134, 135, 0, 0 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 134, 135, 0, 0 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 1, 134, 135, 0, 0 }, // 204 LWA_LWA_32
  { 1, 133, 134, 0, 0 }, // 205 TCHECK
  { 1, 143, 144, 0, 0 }, // 206 LFIWAX
  { 1, 143, 144, 0, 0 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 143, 144, 0, 0 }, // 209 LFSX_LFS
  { 1, 143, 144, 0, 0 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 134, 135, 0, 0 }, // 212 LHAU_LHAU8
  { 1, 134, 135, 0, 0 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 143, 144, 0, 0 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 134, 135, 0, 0 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 134, 135, 0, 0 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 134, 135, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 134, 135, 0, 0 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 144, 145, 0, 0 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 134, 135, 0, 0 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 134, 135, 0, 0 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 134, 135, 0, 0 }, // 225 STXVL_STXVLL
  { 1, 133, 134, 0, 0 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 130, 131, 0, 0 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 146, 147, 0, 0 }, // 230 MFTB_MFTB8
  { 1, 145, 146, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 141, 142, 0, 0 }, // 232 MFMSR
  { 1, 133, 134, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 133, 134, 0, 0 }, // 235 MTSPR_MTSPR8
  { 1, 135, 136, 0, 0 }, // 236 DIVW_DIVWU_MODSW
  { 1, 135, 136, 0, 0 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 135, 136, 0, 0 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 135, 136, 0, 0 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 133, 134, 0, 0 }, // 244 MTCRF_MTCRF8
  { 1, 130, 131, 0, 0 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 136, 137, 0, 0 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 133, 134, 0, 0 }, // 247 MCRFS
  { 1, 136, 137, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 136, 137, 0, 0 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 130, 131, 0, 0 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 130, 131, 0, 0 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 148, 149, 0, 0 }, // 254 SRAWIo_SRAWo
  { 1, 136, 137, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 133, 134, 0, 0 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 137, 138, 0, 0 }, // 259 FDIV
  { 1, 137, 138, 0, 0 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 137, 138, 0, 0 }, // 269 XSDIVDP
  { 1, 140, 141, 0, 0 }, // 270 FDIVS
  { 1, 140, 141, 0, 0 }, // 271 FDIVSo
  { 1, 140, 141, 0, 0 }, // 272 XSDIVSP
  { 1, 140, 141, 0, 0 }, // 273 XVDIVSP
  { 1, 137, 138, 0, 0 }, // 274 XVDIVDP
  { 1, 143, 144, 0, 0 }, // 275 LFSU
  { 1, 143, 144, 0, 0 }, // 276 LFSUX
  { 1, 134, 135, 0, 0 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 134, 135, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 134, 135, 0, 0 }, // 279 STDU
  { 1, 134, 135, 0, 0 }, // 280 STDUX
  { 1, 134, 135, 0, 0 }, // 281 LBZU_LBZU8
  { 1, 134, 135, 0, 0 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 1, 143, 144, 0, 0 }, // 285 LFDU
  { 1, 143, 144, 0, 0 }, // 286 LFDUX
  { 1, 132, 133, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 131, 132, 0, 0 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 131, 132, 0, 0 }, // 289 ADDPCIS
  { 1, 134, 135, 0, 0 }, // 290 LDAT_LWAT
  { 1, 134, 135, 0, 0 }, // 291 STDAT_STWAT
  { 1, 130, 131, 0, 0 }, // 292 BRINC
  { 1, 138, 139, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 139, 140, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 134, 135, 0, 0 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 134, 135, 0, 0 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 131, 132, 0, 0 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 1, 130, 131, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 134, 135, 0, 0 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 134, 135, 0, 0 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 143, 144, 0, 0 }, // 302 LFDEPX
  { 1, 134, 135, 0, 0 }, // 303 STFDEPX
  { 1, 145, 146, 0, 0 }, // 304 MFSR_MFSRIN
  { 1, 133, 134, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 145, 146, 0, 0 }, // 306 MFDCR
  { 1, 133, 134, 0, 0 }, // 307 MTDCR
  { 1, 130, 131, 0, 0 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 133, 134, 0, 0 }, // 310 WRTEE_WRTEEI
  { 1, 129, 130, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 134, 135, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary G5Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 150, 151, 0, 0 }, // 1 IIC_LdStSync
  { 1, 151, 152, 0, 0 }, // 2 IIC_IntSimple
  { 1, 151, 152, 0, 0 }, // 3 IIC_IntGeneral
  { 1, 152, 153, 0, 0 }, // 4 IIC_BrB
  { 1, 153, 154, 0, 0 }, // 5 IIC_VecFP
  { 1, 154, 155, 0, 0 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 155, 156, 0, 0 }, // 10 IIC_BrCR
  { 1, 156, 157, 0, 0 }, // 11 IIC_LdStLD
  { 1, 156, 157, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 156, 157, 0, 0 }, // 13 IIC_LdStLoad
  { 1, 157, 158, 0, 0 }, // 14 IIC_IntDivD
  { 1, 158, 159, 0, 0 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 159, 160, 0, 0 }, // 17 IIC_FPAddSub
  { 1, 160, 161, 0, 0 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 161, 162, 0, 0 }, // 20 IIC_FPCompare
  { 1, 159, 160, 0, 0 }, // 21 IIC_FPGeneral
  { 1, 162, 163, 0, 0 }, // 22 IIC_VecGeneral
  { 1, 163, 164, 0, 0 }, // 23 IIC_VecComplex
  { 1, 156, 157, 0, 0 }, // 24 IIC_LdStStore
  { 1, 151, 152, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 160, 161, 0, 0 }, // 26 IIC_FPDivS
  { 1, 159, 160, 0, 0 }, // 27 IIC_FPFused
  { 1, 164, 165, 0, 0 }, // 28 IIC_FPSqrtD
  { 1, 164, 165, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 165, 166, 0, 0 }, // 30 IIC_LdStICBI
  { 0, 0, 0, 0, 0 }, // 31 IIC_IntISEL
  { 1, 165, 166, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 166, 167, 0, 0 }, // 33 IIC_LdStLWARX
  { 1, 156, 157, 0, 0 }, // 34 IIC_LdStLoadUpd
  { 1, 156, 157, 0, 0 }, // 35 IIC_LdStLoadUpdX
  { 1, 166, 167, 0, 0 }, // 36 IIC_LdStLDARX
  { 1, 156, 157, 0, 0 }, // 37 IIC_LdStLDU
  { 1, 156, 157, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 156, 157, 0, 0 }, // 39 IIC_LdStLFD
  { 1, 167, 168, 0, 0 }, // 40 IIC_LdStLFDU
  { 1, 167, 168, 0, 0 }, // 41 IIC_LdStLFDUX
  { 1, 167, 168, 0, 0 }, // 42 IIC_LdStLHA
  { 1, 167, 168, 0, 0 }, // 43 IIC_LdStLHAU
  { 1, 167, 168, 0, 0 }, // 44 IIC_LdStLHAUX
  { 1, 168, 169, 0, 0 }, // 45 IIC_LdStLMW
  { 1, 167, 168, 0, 0 }, // 46 IIC_LdStLWA
  { 1, 169, 170, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 170, 171, 0, 0 }, // 48 IIC_BrMCR
  { 1, 171, 172, 0, 0 }, // 49 IIC_BrMCRX
  { 1, 172, 173, 0, 0 }, // 50 IIC_SprMFCR
  { 1, 173, 174, 0, 0 }, // 51 IIC_SprMFSPR
  { 1, 174, 175, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 173, 174, 0, 0 }, // 53 IIC_SprMFMSR
  { 1, 172, 173, 0, 0 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 1, 156, 157, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 175, 176, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 176, 177, 0, 0 }, // 59 IIC_SprMTSPR
  { 1, 159, 160, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 156, 157, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 1, 156, 157, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 177, 178, 0, 0 }, // 65 IIC_IntMulHW
  { 1, 177, 178, 0, 0 }, // 66 IIC_IntMulHWU
  { 1, 178, 179, 0, 0 }, // 67 IIC_IntMulLI
  { 1, 179, 180, 0, 0 }, // 68 IIC_VecPerm
  { 1, 180, 181, 0, 0 }, // 69 IIC_LdStSTFD
  { 1, 180, 181, 0, 0 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 1, 181, 182, 0, 0 }, // 72 IIC_IntRFID
  { 1, 151, 152, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 178, 179, 0, 0 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 151, 152, 0, 0 }, // 82 IIC_IntShift
  { 1, 166, 167, 0, 0 }, // 83 IIC_LdStSTWCX
  { 1, 156, 157, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 1, 156, 157, 0, 0 }, // 85 IIC_LdStSTD
  { 1, 166, 167, 0, 0 }, // 86 IIC_LdStSTDCX
  { 1, 156, 157, 0, 0 }, // 87 IIC_LdStSTDU
  { 1, 156, 157, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 1, 182, 183, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 156, 157, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 182, 183, 0, 0 }, // 95 IIC_IntTrapW
  { 1, 183, 184, 0, 0 }, // 96 IIC_VecFPCompare
  { 1, 162, 163, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 1, 153, 154, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 1, 179, 180, 0, 0 }, // 99 XXSEL
  { 1, 176, 177, 0, 0 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 159, 160, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 1, 174, 175, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 154, 155, 0, 0 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 1, 182, 183, 0, 0 }, // 104 TD_TDI
  { 1, 182, 183, 0, 0 }, // 105 TW_TWI
  { 1, 161, 162, 0, 0 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 1, 153, 154, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 1, 153, 154, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 161, 162, 0, 0 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 151, 152, 0, 0 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 1, 151, 152, 0, 0 }, // 111 SLD_SRD_SRAD
  { 1, 151, 152, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 162, 163, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 154, 155, 0, 0 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 151, 152, 0, 0 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 151, 152, 0, 0 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 170, 171, 0, 0 }, // 118 MCRF
  { 1, 171, 172, 0, 0 }, // 119 MCRXRX
  { 1, 153, 154, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 152, 153, 0, 0 }, // 121 RFEBB
  { 1, 176, 177, 0, 0 }, // 122 TBEGIN_TRECHKPT
  { 1, 156, 157, 0, 0 }, // 123 WAIT
  { 1, 151, 152, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 178, 179, 0, 0 }, // 125 RLWIMI_RLWIMI8
  { 1, 151, 152, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 172, 173, 0, 0 }, // 127 MFOCRF_MFOCRF8
  { 1, 171, 172, 0, 0 }, // 128 MTOCRF_MTOCRF8
  { 1, 155, 156, 0, 0 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 151, 152, 0, 0 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 159, 160, 0, 0 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 151, 152, 0, 0 }, // 132 SRAW_SRAWI
  { 0, 0, 0, 0, 0 }, // 133 ISEL_ISEL8
  { 1, 153, 154, 0, 0 }, // 134 XSIEXPDP
  { 1, 176, 177, 0, 0 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 156, 157, 0, 0 }, // 136 MFVSCR
  { 1, 156, 157, 0, 0 }, // 137 MTVSCR
  { 1, 183, 184, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 162, 163, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 1, 153, 154, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 161, 162, 0, 0 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 1, 153, 154, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 162, 163, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 1, 169, 170, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 177, 178, 0, 0 }, // 145 MULHD_MULHW_MULLW
  { 1, 177, 178, 0, 0 }, // 146 MULHDU_MULHWU
  { 1, 178, 179, 0, 0 }, // 147 MULLI_MULLI8
  { 1, 159, 160, 0, 0 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 159, 160, 0, 0 }, // 149 FADD_FSUB
  { 1, 159, 160, 0, 0 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 1, 153, 154, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 159, 160, 0, 0 }, // 152 FSELDo_FSELSo
  { 1, 177, 178, 0, 0 }, // 153 MULHDUo_MULHWUo
  { 1, 177, 178, 0, 0 }, // 154 MULHDo_MULHWo_MULLWo
  { 1, 169, 170, 0, 0 }, // 155 MULLDo
  { 1, 159, 160, 0, 0 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 159, 160, 0, 0 }, // 157 FADDo_FSUBo
  { 1, 159, 160, 0, 0 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 1, 153, 154, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 156, 157, 0, 0 }, // 160 LVSL_LVSR
  { 1, 179, 180, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 1, 153, 154, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 162, 163, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 161, 162, 0, 0 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 1, 153, 154, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 1, 153, 154, 0, 0 }, // 166 BCDCTSQo
  { 1, 153, 154, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 1, 153, 154, 0, 0 }, // 168 BCDCFSQo
  { 1, 153, 154, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 1, 153, 154, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 156, 157, 0, 0 }, // 171 LXVL_LXVLL
  { 1, 156, 157, 0, 0 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 156, 157, 0, 0 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 156, 157, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 165, 166, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 156, 157, 0, 0 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 166, 167, 0, 0 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 1, 156, 157, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 1, 166, 167, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 165, 166, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 156, 157, 0, 0 }, // 186 TLBSYNC
  { 1, 150, 151, 0, 0 }, // 187 SYNC
  { 1, 168, 169, 0, 0 }, // 188 LMW
  { 1, 156, 157, 0, 0 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 156, 157, 0, 0 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 156, 157, 0, 0 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 176, 177, 0, 0 }, // 198 TEND
  { 1, 166, 167, 0, 0 }, // 199 STBCX_STHCX_STWCX
  { 1, 166, 167, 0, 0 }, // 200 STDCX
  { 1, 156, 157, 0, 0 }, // 201 LDMX
  { 1, 167, 168, 0, 0 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 1, 167, 168, 0, 0 }, // 204 LWA_LWA_32
  { 1, 176, 177, 0, 0 }, // 205 TCHECK
  { 1, 156, 157, 0, 0 }, // 206 LFIWAX
  { 1, 156, 157, 0, 0 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 156, 157, 0, 0 }, // 209 LFSX_LFS
  { 1, 156, 157, 0, 0 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 167, 168, 0, 0 }, // 212 LHAU_LHAU8
  { 1, 167, 168, 0, 0 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 156, 157, 0, 0 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 180, 181, 0, 0 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 156, 157, 0, 0 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 156, 157, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 156, 157, 0, 0 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 168, 169, 0, 0 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 156, 157, 0, 0 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 180, 181, 0, 0 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 156, 157, 0, 0 }, // 225 STXVL_STXVLL
  { 1, 176, 177, 0, 0 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 151, 152, 0, 0 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 175, 176, 0, 0 }, // 230 MFTB_MFTB8
  { 1, 173, 174, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 173, 174, 0, 0 }, // 232 MFMSR
  { 1, 156, 157, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 176, 177, 0, 0 }, // 235 MTSPR_MTSPR8
  { 1, 158, 159, 0, 0 }, // 236 DIVW_DIVWU_MODSW
  { 1, 158, 159, 0, 0 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 1, 157, 158, 0, 0 }, // 238 DIVD_DIVDU
  { 1, 157, 158, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 158, 159, 0, 0 }, // 240 DIVWUo_DIVWo
  { 1, 157, 158, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 158, 159, 0, 0 }, // 242 DIVWEo_DIVWEUo
  { 1, 157, 158, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 171, 172, 0, 0 }, // 244 MTCRF_MTCRF8
  { 1, 151, 152, 0, 0 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 159, 160, 0, 0 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 170, 171, 0, 0 }, // 247 MCRFS
  { 1, 174, 175, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 159, 160, 0, 0 }, // 249 MTFSFb
  { 1, 151, 152, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 1, 151, 152, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 178, 179, 0, 0 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 151, 152, 0, 0 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 151, 152, 0, 0 }, // 254 SRAWIo_SRAWo
  { 1, 174, 175, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 172, 173, 0, 0 }, // 256 MFCR_MFCR8
  { 1, 151, 152, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 1, 151, 152, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 160, 161, 0, 0 }, // 259 FDIV
  { 1, 160, 161, 0, 0 }, // 260 FDIVo
  { 1, 164, 165, 0, 0 }, // 261 XSSQRTDP
  { 1, 164, 165, 0, 0 }, // 262 FSQRT
  { 1, 164, 165, 0, 0 }, // 263 XVSQRTDP
  { 1, 164, 165, 0, 0 }, // 264 XVSQRTSP
  { 1, 164, 165, 0, 0 }, // 265 FSQRTo
  { 1, 164, 165, 0, 0 }, // 266 XSSQRTSP
  { 1, 164, 165, 0, 0 }, // 267 FSQRTS
  { 1, 164, 165, 0, 0 }, // 268 FSQRTSo
  { 1, 160, 161, 0, 0 }, // 269 XSDIVDP
  { 1, 160, 161, 0, 0 }, // 270 FDIVS
  { 1, 160, 161, 0, 0 }, // 271 FDIVSo
  { 1, 160, 161, 0, 0 }, // 272 XSDIVSP
  { 1, 160, 161, 0, 0 }, // 273 XVDIVSP
  { 1, 160, 161, 0, 0 }, // 274 XVDIVDP
  { 1, 167, 168, 0, 0 }, // 275 LFSU
  { 1, 167, 168, 0, 0 }, // 276 LFSUX
  { 1, 180, 181, 0, 0 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 156, 157, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 156, 157, 0, 0 }, // 279 STDU
  { 1, 156, 157, 0, 0 }, // 280 STDUX
  { 1, 156, 157, 0, 0 }, // 281 LBZU_LBZU8
  { 1, 156, 157, 0, 0 }, // 282 LBZUX_LBZUX8
  { 1, 156, 157, 0, 0 }, // 283 LDU
  { 1, 156, 157, 0, 0 }, // 284 LDUX
  { 1, 167, 168, 0, 0 }, // 285 LFDU
  { 1, 167, 168, 0, 0 }, // 286 LFDUX
  { 1, 153, 154, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 152, 153, 0, 0 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 152, 153, 0, 0 }, // 289 ADDPCIS
  { 1, 156, 157, 0, 0 }, // 290 LDAT_LWAT
  { 1, 156, 157, 0, 0 }, // 291 STDAT_STWAT
  { 1, 151, 152, 0, 0 }, // 292 BRINC
  { 1, 162, 163, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 163, 164, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 156, 157, 0, 0 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 156, 157, 0, 0 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 152, 153, 0, 0 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 1, 181, 182, 0, 0 }, // 299 RFID
  { 1, 156, 157, 0, 0 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 156, 157, 0, 0 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 156, 157, 0, 0 }, // 302 LFDEPX
  { 1, 180, 181, 0, 0 }, // 303 STFDEPX
  { 1, 156, 157, 0, 0 }, // 304 MFSR_MFSRIN
  { 1, 156, 157, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 173, 174, 0, 0 }, // 306 MFDCR
  { 1, 176, 177, 0, 0 }, // 307 MTDCR
  { 1, 151, 152, 0, 0 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 156, 157, 0, 0 }, // 310 WRTEE_WRTEEI
  { 1, 150, 151, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 156, 157, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary PPCA2Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 184, 185, 547, 548 }, // 1 IIC_LdStSync
  { 1, 184, 185, 548, 551 }, // 2 IIC_IntSimple
  { 1, 184, 185, 551, 554 }, // 3 IIC_IntGeneral
  { 1, 184, 185, 554, 557 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 184, 185, 557, 560 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 184, 185, 560, 563 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 184, 185, 563, 566 }, // 12 IIC_LdStDCBF
  { 1, 184, 185, 566, 569 }, // 13 IIC_LdStLoad
  { 1, 184, 185, 569, 572 }, // 14 IIC_IntDivD
  { 1, 184, 185, 572, 575 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 185, 186, 575, 578 }, // 17 IIC_FPAddSub
  { 1, 185, 186, 578, 581 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 185, 186, 581, 584 }, // 20 IIC_FPCompare
  { 1, 185, 186, 584, 587 }, // 21 IIC_FPGeneral
  { 0, 0, 0, 0, 0 }, // 22 IIC_VecGeneral
  { 0, 0, 0, 0, 0 }, // 23 IIC_VecComplex
  { 1, 184, 185, 587, 590 }, // 24 IIC_LdStStore
  { 1, 184, 185, 590, 593 }, // 25 IIC_IntRotateDI
  { 1, 185, 186, 593, 596 }, // 26 IIC_FPDivS
  { 1, 185, 186, 596, 600 }, // 27 IIC_FPFused
  { 1, 185, 186, 600, 603 }, // 28 IIC_FPSqrtD
  { 1, 185, 186, 603, 606 }, // 29 IIC_FPSqrtS
  { 1, 184, 185, 606, 609 }, // 30 IIC_LdStICBI
  { 1, 184, 185, 609, 613 }, // 31 IIC_IntISEL
  { 1, 184, 185, 613, 614 }, // 32 IIC_SprISYNC
  { 1, 184, 185, 614, 617 }, // 33 IIC_LdStLWARX
  { 1, 184, 185, 617, 621 }, // 34 IIC_LdStLoadUpd
  { 1, 184, 185, 621, 625 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 1, 184, 185, 625, 628 }, // 37 IIC_LdStLDU
  { 1, 184, 185, 628, 631 }, // 38 IIC_LdStLDUX
  { 1, 184, 185, 631, 634 }, // 39 IIC_LdStLFD
  { 1, 184, 185, 634, 638 }, // 40 IIC_LdStLFDU
  { 1, 184, 185, 638, 642 }, // 41 IIC_LdStLFDUX
  { 1, 184, 185, 642, 645 }, // 42 IIC_LdStLHA
  { 1, 184, 185, 645, 649 }, // 43 IIC_LdStLHAU
  { 1, 184, 185, 649, 653 }, // 44 IIC_LdStLHAUX
  { 0, 0, 0, 0, 0 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 184, 185, 653, 656 }, // 48 IIC_BrMCR
  { 1, 184, 185, 656, 659 }, // 49 IIC_BrMCRX
  { 1, 184, 185, 659, 661 }, // 50 IIC_SprMFCR
  { 1, 184, 185, 661, 663 }, // 51 IIC_SprMFSPR
  { 0, 0, 0, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 184, 185, 663, 665 }, // 53 IIC_SprMFMSR
  { 1, 184, 185, 665, 667 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 0, 0, 0, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 184, 185, 667, 669 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 184, 185, 669, 671 }, // 59 IIC_SprMTSPR
  { 0, 0, 0, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 184, 185, 671, 673 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 0, 0, 0, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 184, 185, 673, 676 }, // 65 IIC_IntMulHW
  { 1, 184, 185, 676, 679 }, // 66 IIC_IntMulHWU
  { 1, 184, 185, 679, 682 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 1, 184, 185, 682, 685 }, // 69 IIC_LdStSTFD
  { 1, 184, 185, 685, 689 }, // 70 IIC_LdStSTFDU
  { 1, 184, 185, 689, 690 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 1, 184, 185, 690, 693 }, // 73 IIC_IntRotateD
  { 1, 184, 185, 693, 696 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 184, 185, 696, 699 }, // 82 IIC_IntShift
  { 1, 184, 185, 699, 702 }, // 83 IIC_LdStSTWCX
  { 1, 184, 185, 702, 706 }, // 84 IIC_LdStStoreUpd
  { 1, 184, 185, 706, 709 }, // 85 IIC_LdStSTD
  { 1, 184, 185, 709, 712 }, // 86 IIC_LdStSTDCX
  { 1, 184, 185, 712, 716 }, // 87 IIC_LdStSTDU
  { 1, 184, 185, 716, 720 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 1, 184, 185, 720, 722 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 0, 0, 0, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 184, 185, 722, 724 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 0, 0, 0, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 184, 185, 724, 726 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 0, 0, 0, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 0, 0, 0, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 184, 185, 726, 729 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 1, 184, 185, 729, 731 }, // 104 TD_TDI
  { 1, 184, 185, 731, 733 }, // 105 TW_TWI
  { 1, 185, 186, 733, 736 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 185, 186, 736, 739 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 184, 185, 739, 742 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 1, 184, 185, 742, 745 }, // 111 SLD_SRD_SRAD
  { 1, 184, 185, 745, 748 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 0, 0, 0, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 184, 185, 748, 751 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 184, 185, 751, 754 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 184, 185, 754, 757 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 184, 185, 757, 760 }, // 118 MCRF
  { 1, 184, 185, 760, 763 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 184, 185, 763, 766 }, // 121 RFEBB
  { 1, 184, 185, 766, 768 }, // 122 TBEGIN_TRECHKPT
  { 1, 184, 185, 768, 771 }, // 123 WAIT
  { 1, 184, 185, 771, 774 }, // 124 RLDCL_RLDCR
  { 1, 184, 185, 774, 777 }, // 125 RLWIMI_RLWIMI8
  { 1, 184, 185, 777, 780 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 184, 185, 780, 782 }, // 127 MFOCRF_MFOCRF8
  { 1, 184, 185, 782, 785 }, // 128 MTOCRF_MTOCRF8
  { 1, 184, 185, 785, 788 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 184, 185, 788, 791 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 185, 186, 791, 794 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 184, 185, 794, 797 }, // 132 SRAW_SRAWI
  { 1, 184, 185, 797, 801 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 184, 185, 801, 803 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 184, 185, 803, 806 }, // 136 MFVSCR
  { 1, 184, 185, 806, 809 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 0, 0, 0, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 185, 186, 809, 812 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 0, 0, 0, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 184, 185, 812, 815 }, // 145 MULHD_MULHW_MULLW
  { 1, 184, 185, 815, 818 }, // 146 MULHDU_MULHWU
  { 1, 184, 185, 818, 821 }, // 147 MULLI_MULLI8
  { 1, 185, 186, 821, 824 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 185, 186, 824, 827 }, // 149 FADD_FSUB
  { 1, 185, 186, 827, 831 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 185, 186, 831, 834 }, // 152 FSELDo_FSELSo
  { 1, 184, 185, 834, 837 }, // 153 MULHDUo_MULHWUo
  { 1, 184, 185, 837, 840 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 185, 186, 840, 843 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 185, 186, 843, 846 }, // 157 FADDo_FSUBo
  { 1, 185, 186, 846, 850 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 184, 185, 850, 853 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 0, 0, 0, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 185, 186, 853, 856 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 184, 185, 856, 859 }, // 171 LXVL_LXVLL
  { 1, 184, 185, 859, 862 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 184, 185, 862, 865 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 184, 185, 865, 868 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 184, 185, 868, 871 }, // 178 ICBI_ICBIEP
  { 1, 184, 185, 871, 874 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 184, 185, 874, 877 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 1, 184, 185, 877, 878 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 0, 0, 0, 0, 0 }, // 186 TLBSYNC
  { 1, 184, 185, 878, 879 }, // 187 SYNC
  { 0, 0, 0, 0, 0 }, // 188 LMW
  { 1, 184, 185, 879, 882 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 184, 185, 882, 886 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 184, 185, 886, 890 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 184, 185, 890, 892 }, // 198 TEND
  { 1, 184, 185, 892, 895 }, // 199 STBCX_STHCX_STWCX
  { 1, 184, 185, 895, 898 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 184, 185, 898, 901 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 184, 185, 901, 903 }, // 205 TCHECK
  { 1, 184, 185, 903, 906 }, // 206 LFIWAX
  { 1, 184, 185, 906, 909 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 184, 185, 909, 912 }, // 209 LFSX_LFS
  { 1, 184, 185, 912, 915 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 184, 185, 915, 919 }, // 212 LHAU_LHAU8
  { 1, 184, 185, 919, 923 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 184, 185, 923, 926 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 184, 185, 926, 929 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 184, 185, 929, 932 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 184, 185, 932, 935 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 184, 185, 935, 938 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 0, 0, 0, 0, 0 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 184, 185, 938, 941 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 184, 185, 941, 944 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 184, 185, 944, 947 }, // 225 STXVL_STXVLL
  { 1, 184, 185, 947, 949 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 184, 185, 949, 952 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 1, 184, 185, 952, 954 }, // 230 MFTB_MFTB8
  { 1, 184, 185, 954, 956 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 184, 185, 956, 958 }, // 232 MFMSR
  { 1, 184, 185, 958, 960 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 184, 185, 960, 962 }, // 235 MTSPR_MTSPR8
  { 1, 184, 185, 962, 965 }, // 236 DIVW_DIVWU_MODSW
  { 1, 184, 185, 965, 968 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 1, 184, 185, 968, 971 }, // 238 DIVD_DIVDU
  { 1, 184, 185, 971, 974 }, // 239 DIVDE_DIVDEU
  { 1, 184, 185, 974, 977 }, // 240 DIVWUo_DIVWo
  { 1, 184, 185, 977, 980 }, // 241 DIVDo_DIVDUo
  { 1, 184, 185, 980, 983 }, // 242 DIVWEo_DIVWEUo
  { 1, 184, 185, 983, 986 }, // 243 DIVDEo_DIVDEUo
  { 1, 184, 185, 986, 989 }, // 244 MTCRF_MTCRF8
  { 1, 184, 185, 989, 992 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 185, 186, 992, 995 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 184, 185, 995, 998 }, // 247 MCRFS
  { 0, 0, 0, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 0, 0, 0, 0, 0 }, // 249 MTFSFb
  { 1, 184, 185, 998, 1001 }, // 250 RLDCLo_RLDCRo
  { 1, 184, 185, 1001, 1004 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 184, 185, 1004, 1007 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 184, 185, 1007, 1010 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 184, 185, 1010, 1013 }, // 254 SRAWIo_SRAWo
  { 0, 0, 0, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 184, 185, 1013, 1015 }, // 256 MFCR_MFCR8
  { 1, 184, 185, 1015, 1018 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 1, 184, 185, 1018, 1021 }, // 258 SRADo_SLDo_SRDo
  { 1, 185, 186, 1021, 1024 }, // 259 FDIV
  { 1, 185, 186, 1024, 1027 }, // 260 FDIVo
  { 1, 185, 186, 1027, 1030 }, // 261 XSSQRTDP
  { 1, 185, 186, 1030, 1033 }, // 262 FSQRT
  { 1, 185, 186, 1033, 1036 }, // 263 XVSQRTDP
  { 1, 185, 186, 1036, 1039 }, // 264 XVSQRTSP
  { 1, 185, 186, 1039, 1042 }, // 265 FSQRTo
  { 1, 185, 186, 1042, 1045 }, // 266 XSSQRTSP
  { 1, 185, 186, 1045, 1048 }, // 267 FSQRTS
  { 1, 185, 186, 1048, 1051 }, // 268 FSQRTSo
  { 1, 185, 186, 1051, 1054 }, // 269 XSDIVDP
  { 1, 185, 186, 1054, 1057 }, // 270 FDIVS
  { 1, 185, 186, 1057, 1060 }, // 271 FDIVSo
  { 1, 185, 186, 1060, 1063 }, // 272 XSDIVSP
  { 1, 185, 186, 1063, 1066 }, // 273 XVDIVSP
  { 1, 185, 186, 1066, 1069 }, // 274 XVDIVDP
  { 1, 184, 185, 1069, 1073 }, // 275 LFSU
  { 1, 184, 185, 1073, 1077 }, // 276 LFSUX
  { 1, 184, 185, 1077, 1081 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 1, 184, 185, 1081, 1085 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 184, 185, 1085, 1089 }, // 279 STDU
  { 1, 184, 185, 1089, 1093 }, // 280 STDUX
  { 1, 184, 185, 1093, 1097 }, // 281 LBZU_LBZU8
  { 1, 184, 185, 1097, 1101 }, // 282 LBZUX_LBZUX8
  { 1, 184, 185, 1101, 1104 }, // 283 LDU
  { 1, 184, 185, 1104, 1107 }, // 284 LDUX
  { 1, 184, 185, 1107, 1111 }, // 285 LFDU
  { 1, 184, 185, 1111, 1115 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 184, 185, 1115, 1118 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 184, 185, 1118, 1121 }, // 289 ADDPCIS
  { 1, 184, 185, 1121, 1124 }, // 290 LDAT_LWAT
  { 1, 184, 185, 1124, 1127 }, // 291 STDAT_STWAT
  { 1, 184, 185, 1127, 1130 }, // 292 BRINC
  { 0, 0, 0, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 0, 0, 0, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 184, 185, 1130, 1133 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 184, 185, 1133, 1136 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 184, 185, 1136, 1139 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 1, 184, 185, 1139, 1140 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 184, 185, 1140, 1143 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 184, 185, 1143, 1146 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 184, 185, 1146, 1149 }, // 302 LFDEPX
  { 1, 184, 185, 1149, 1152 }, // 303 STFDEPX
  { 0, 0, 0, 0, 0 }, // 304 MFSR_MFSRIN
  { 0, 0, 0, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 184, 185, 1152, 1154 }, // 306 MFDCR
  { 1, 184, 185, 1154, 1156 }, // 307 MTDCR
  { 1, 184, 185, 1156, 1159 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 184, 185, 1159, 1161 }, // 310 WRTEE_WRTEEI
  { 1, 184, 185, 1161, 1162 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 184, 185, 1162, 1165 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary PPCE500Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 186, 188, 0, 0 }, // 1 IIC_LdStSync
  { 1, 188, 190, 1165, 1168 }, // 2 IIC_IntSimple
  { 1, 188, 190, 1168, 1171 }, // 3 IIC_IntGeneral
  { 1, 190, 192, 1171, 1173 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 188, 190, 1173, 1176 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 190, 192, 1176, 1179 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 186, 188, 1179, 1181 }, // 12 IIC_LdStDCBF
  { 1, 186, 188, 1181, 1183 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 192, 195, 1183, 1186 }, // 15 IIC_IntDivW
  { 1, 195, 197, 1186, 1189 }, // 16 IIC_FPDGeneral
  { 0, 0, 0, 0, 0 }, // 17 IIC_FPAddSub
  { 1, 197, 199, 1189, 1192 }, // 18 IIC_FPDivD
  { 1, 199, 201, 1192, 1195 }, // 19 IIC_FPSGeneral
  { 0, 0, 0, 0, 0 }, // 20 IIC_FPCompare
  { 0, 0, 0, 0, 0 }, // 21 IIC_FPGeneral
  { 1, 201, 203, 1195, 1198 }, // 22 IIC_VecGeneral
  { 1, 199, 201, 1198, 1201 }, // 23 IIC_VecComplex
  { 1, 186, 188, 1201, 1203 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 203, 205, 1203, 1206 }, // 26 IIC_FPDivS
  { 0, 0, 0, 0, 0 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 186, 188, 1206, 1208 }, // 30 IIC_LdStICBI
  { 1, 188, 190, 1208, 1212 }, // 31 IIC_IntISEL
  { 0, 0, 0, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 205, 207, 1212, 1215 }, // 33 IIC_LdStLWARX
  { 2, 207, 210, 1215, 1217 }, // 34 IIC_LdStLoadUpd
  { 2, 207, 210, 1217, 1219 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 0, 0, 0, 0, 0 }, // 39 IIC_LdStLFD
  { 0, 0, 0, 0, 0 }, // 40 IIC_LdStLFDU
  { 0, 0, 0, 0, 0 }, // 41 IIC_LdStLFDUX
  { 1, 186, 188, 1219, 1221 }, // 42 IIC_LdStLHA
  { 1, 207, 210, 1221, 1223 }, // 43 IIC_LdStLHAU
  { 1, 207, 210, 1223, 1225 }, // 44 IIC_LdStLHAUX
  { 1, 186, 188, 1225, 1227 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 190, 192, 1227, 1229 }, // 48 IIC_BrMCR
  { 1, 188, 190, 1229, 1232 }, // 49 IIC_BrMCRX
  { 1, 210, 212, 1232, 1234 }, // 50 IIC_SprMFCR
  { 1, 188, 190, 1234, 1236 }, // 51 IIC_SprMFSPR
  { 0, 0, 0, 0, 0 }, // 52 IIC_IntMFFS
  { 1, 212, 214, 1236, 1238 }, // 53 IIC_SprMFMSR
  { 1, 210, 212, 1238, 1240 }, // 54 IIC_SprMFCRF
  { 1, 212, 214, 1240, 1242 }, // 55 IIC_SprMFPMR
  { 1, 212, 214, 1242, 1244 }, // 56 IIC_SprMFSR
  { 1, 212, 214, 1244, 1246 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 188, 190, 1246, 1248 }, // 59 IIC_SprMTSPR
  { 0, 0, 0, 0, 0 }, // 60 IIC_IntMTFSB0
  { 1, 214, 216, 1248, 1250 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 1, 201, 203, 1250, 1252 }, // 63 IIC_SprMTPMR
  { 1, 201, 203, 1252, 1254 }, // 64 IIC_SprMTSR
  { 1, 199, 201, 1254, 1257 }, // 65 IIC_IntMulHW
  { 1, 199, 201, 1257, 1260 }, // 66 IIC_IntMulHWU
  { 1, 199, 201, 1260, 1263 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 0, 0, 0, 0, 0 }, // 69 IIC_LdStSTFD
  { 0, 0, 0, 0, 0 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 188, 190, 1263, 1266 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 188, 190, 1266, 1269 }, // 82 IIC_IntShift
  { 1, 186, 188, 1269, 1271 }, // 83 IIC_LdStSTWCX
  { 2, 207, 210, 1271, 1273 }, // 84 IIC_LdStStoreUpd
  { 0, 0, 0, 0, 0 }, // 85 IIC_LdStSTD
  { 0, 0, 0, 0, 0 }, // 86 IIC_LdStSTDCX
  { 0, 0, 0, 0, 0 }, // 87 IIC_LdStSTDU
  { 0, 0, 0, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 216, 218, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 218, 220, 1273, 1275 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 1, 201, 203, 1275, 1278 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 188, 190, 1278, 1280 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 0, 0, 0, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 0, 0, 0, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 188, 190, 1280, 1283 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 218, 220, 1283, 1285 }, // 105 TW_TWI
  { 0, 0, 0, 0, 0 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 0, 0, 0, 0, 0 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 188, 190, 1285, 1288 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 201, 203, 1288, 1291 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 188, 190, 1291, 1294 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 188, 190, 1294, 1297 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 188, 190, 1297, 1300 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 190, 192, 1300, 1302 }, // 118 MCRF
  { 1, 188, 190, 1302, 1305 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 190, 192, 1305, 1307 }, // 121 RFEBB
  { 1, 188, 190, 1307, 1309 }, // 122 TBEGIN_TRECHKPT
  { 1, 186, 188, 1309, 1311 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 188, 190, 1311, 1314 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 210, 212, 1314, 1316 }, // 127 MFOCRF_MFOCRF8
  { 1, 188, 190, 1316, 1319 }, // 128 MTOCRF_MTOCRF8
  { 1, 190, 192, 1319, 1322 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 188, 190, 1322, 1325 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 0, 0, 0, 0, 0 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 188, 190, 1325, 1328 }, // 132 SRAW_SRAWI
  { 1, 188, 190, 1328, 1332 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 188, 190, 1332, 1334 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 186, 188, 1334, 1336 }, // 136 MFVSCR
  { 1, 186, 188, 1336, 1338 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 201, 203, 1338, 1341 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 0, 0, 0, 0, 0 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 201, 203, 1341, 1344 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 199, 201, 1344, 1347 }, // 145 MULHD_MULHW_MULLW
  { 1, 199, 201, 1347, 1350 }, // 146 MULHDU_MULHWU
  { 1, 199, 201, 1350, 1353 }, // 147 MULLI_MULLI8
  { 0, 0, 0, 0, 0 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 0, 0, 0, 0, 0 }, // 149 FADD_FSUB
  { 0, 0, 0, 0, 0 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 0, 0, 0, 0, 0 }, // 152 FSELDo_FSELSo
  { 1, 199, 201, 1353, 1356 }, // 153 MULHDUo_MULHWUo
  { 1, 199, 201, 1356, 1359 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 0, 0, 0, 0, 0 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 0, 0, 0, 0, 0 }, // 157 FADDo_FSUBo
  { 0, 0, 0, 0, 0 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 186, 188, 1359, 1361 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 201, 203, 1361, 1364 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 0, 0, 0, 0, 0 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 186, 188, 1364, 1366 }, // 171 LXVL_LXVLL
  { 1, 186, 188, 1366, 1368 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 0, 0, 0, 0, 0 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 186, 188, 1368, 1370 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 186, 188, 1370, 1372 }, // 178 ICBI_ICBIEP
  { 1, 186, 188, 1372, 1374 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 205, 207, 1374, 1377 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 0, 0, 0, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 216, 218, 0, 0 }, // 186 TLBSYNC
  { 1, 186, 188, 0, 0 }, // 187 SYNC
  { 1, 186, 188, 1377, 1379 }, // 188 LMW
  { 0, 0, 0, 0, 0 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 2, 207, 210, 1379, 1381 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 2, 207, 210, 1381, 1383 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 188, 190, 1383, 1385 }, // 198 TEND
  { 1, 186, 188, 1385, 1387 }, // 199 STBCX_STHCX_STWCX
  { 0, 0, 0, 0, 0 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 186, 188, 1387, 1389 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 188, 190, 1389, 1391 }, // 205 TCHECK
  { 0, 0, 0, 0, 0 }, // 206 LFIWAX
  { 0, 0, 0, 0, 0 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 0, 0, 0, 0, 0 }, // 209 LFSX_LFS
  { 0, 0, 0, 0, 0 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 207, 210, 1391, 1393 }, // 212 LHAU_LHAU8
  { 1, 207, 210, 1393, 1395 }, // 213 LHAUX_LHAUX8_LWAUX
  { 0, 0, 0, 0, 0 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 0, 0, 0, 0, 0 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 186, 188, 1395, 1397 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 0, 0, 0, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 186, 188, 1397, 1399 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 186, 188, 1399, 1401 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 186, 188, 1401, 1403 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 0, 0, 0, 0, 0 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 186, 188, 1403, 1405 }, // 225 STXVL_STXVLL
  { 1, 188, 190, 1405, 1407 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 188, 190, 1407, 1410 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 1, 212, 214, 1410, 1412 }, // 228 MFPMR
  { 1, 201, 203, 1412, 1414 }, // 229 MTPMR
  { 1, 212, 214, 1414, 1416 }, // 230 MFTB_MFTB8
  { 1, 188, 190, 1416, 1418 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 212, 214, 1418, 1420 }, // 232 MFMSR
  { 1, 214, 216, 1420, 1422 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 188, 190, 1422, 1424 }, // 235 MTSPR_MTSPR8
  { 1, 192, 195, 1424, 1427 }, // 236 DIVW_DIVWU_MODSW
  { 1, 192, 195, 1427, 1430 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 192, 195, 1430, 1433 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 192, 195, 1433, 1436 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 188, 190, 1436, 1439 }, // 244 MTCRF_MTCRF8
  { 1, 188, 190, 1439, 1442 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 0, 0, 0, 0, 0 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 190, 192, 1442, 1444 }, // 247 MCRFS
  { 0, 0, 0, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 0, 0, 0, 0, 0 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 188, 190, 1444, 1447 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 188, 190, 1447, 1450 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 188, 190, 1450, 1453 }, // 254 SRAWIo_SRAWo
  { 0, 0, 0, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 210, 212, 1453, 1455 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 197, 199, 1455, 1458 }, // 259 FDIV
  { 1, 197, 199, 1458, 1461 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 197, 199, 1461, 1464 }, // 269 XSDIVDP
  { 1, 203, 205, 1464, 1467 }, // 270 FDIVS
  { 1, 203, 205, 1467, 1470 }, // 271 FDIVSo
  { 1, 203, 205, 1470, 1473 }, // 272 XSDIVSP
  { 1, 203, 205, 1473, 1476 }, // 273 XVDIVSP
  { 1, 197, 199, 1476, 1479 }, // 274 XVDIVDP
  { 0, 0, 0, 0, 0 }, // 275 LFSU
  { 0, 0, 0, 0, 0 }, // 276 LFSUX
  { 0, 0, 0, 0, 0 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 2, 207, 210, 1479, 1481 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 0, 0, 0, 0, 0 }, // 279 STDU
  { 0, 0, 0, 0, 0 }, // 280 STDUX
  { 2, 207, 210, 1481, 1483 }, // 281 LBZU_LBZU8
  { 2, 207, 210, 1483, 1485 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 0, 0, 0, 0, 0 }, // 285 LFDU
  { 0, 0, 0, 0, 0 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 190, 192, 1485, 1487 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 190, 192, 1487, 1489 }, // 289 ADDPCIS
  { 1, 186, 188, 1489, 1491 }, // 290 LDAT_LWAT
  { 1, 186, 188, 1491, 1493 }, // 291 STDAT_STWAT
  { 1, 188, 190, 1493, 1496 }, // 292 BRINC
  { 1, 201, 203, 1496, 1499 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 199, 201, 1499, 1502 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 186, 188, 1502, 1504 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 186, 188, 1504, 1506 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 190, 192, 1506, 1508 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 186, 188, 1508, 1510 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 186, 188, 1510, 1512 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 0, 0, 0, 0, 0 }, // 302 LFDEPX
  { 0, 0, 0, 0, 0 }, // 303 STFDEPX
  { 1, 212, 214, 1512, 1514 }, // 304 MFSR_MFSRIN
  { 1, 201, 203, 1514, 1516 }, // 305 MTSR_MTSRIN
  { 1, 188, 190, 1516, 1518 }, // 306 MFDCR
  { 1, 188, 190, 1518, 1520 }, // 307 MTDCR
  { 1, 188, 190, 1520, 1523 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 214, 216, 1523, 1525 }, // 310 WRTEE_WRTEEI
  { 1, 186, 188, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 186, 188, 1525, 1527 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary PPCE500mcItineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 220, 222, 0, 0 }, // 1 IIC_LdStSync
  { 1, 222, 224, 1527, 1530 }, // 2 IIC_IntSimple
  { 1, 222, 224, 1530, 1533 }, // 3 IIC_IntGeneral
  { 1, 224, 226, 1533, 1535 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 222, 224, 1535, 1538 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 224, 226, 1538, 1541 }, // 10 IIC_BrCR
  { 0, 0, 0, 0, 0 }, // 11 IIC_LdStLD
  { 1, 220, 222, 1541, 1543 }, // 12 IIC_LdStDCBF
  { 1, 220, 222, 1543, 1545 }, // 13 IIC_LdStLoad
  { 0, 0, 0, 0, 0 }, // 14 IIC_IntDivD
  { 1, 226, 229, 1545, 1548 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 229, 231, 1548, 1551 }, // 17 IIC_FPAddSub
  { 1, 231, 233, 1551, 1554 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 233, 235, 1554, 1557 }, // 20 IIC_FPCompare
  { 1, 233, 235, 1557, 1560 }, // 21 IIC_FPGeneral
  { 0, 0, 0, 0, 0 }, // 22 IIC_VecGeneral
  { 0, 0, 0, 0, 0 }, // 23 IIC_VecComplex
  { 1, 220, 222, 1560, 1562 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 235, 237, 1562, 1565 }, // 26 IIC_FPDivS
  { 1, 229, 231, 1565, 1569 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 220, 222, 1569, 1571 }, // 30 IIC_LdStICBI
  { 1, 222, 224, 1571, 1575 }, // 31 IIC_IntISEL
  { 0, 0, 0, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 237, 239, 1575, 1578 }, // 33 IIC_LdStLWARX
  { 2, 239, 242, 1578, 1580 }, // 34 IIC_LdStLoadUpd
  { 2, 239, 242, 1580, 1582 }, // 35 IIC_LdStLoadUpdX
  { 0, 0, 0, 0, 0 }, // 36 IIC_LdStLDARX
  { 0, 0, 0, 0, 0 }, // 37 IIC_LdStLDU
  { 0, 0, 0, 0, 0 }, // 38 IIC_LdStLDUX
  { 1, 220, 222, 1582, 1585 }, // 39 IIC_LdStLFD
  { 2, 239, 242, 1585, 1588 }, // 40 IIC_LdStLFDU
  { 2, 239, 242, 1588, 1591 }, // 41 IIC_LdStLFDUX
  { 1, 220, 222, 1591, 1593 }, // 42 IIC_LdStLHA
  { 1, 239, 242, 1593, 1595 }, // 43 IIC_LdStLHAU
  { 1, 239, 242, 1595, 1597 }, // 44 IIC_LdStLHAUX
  { 1, 220, 222, 1597, 1599 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 224, 226, 1599, 1601 }, // 48 IIC_BrMCR
  { 1, 222, 224, 1601, 1604 }, // 49 IIC_BrMCRX
  { 1, 242, 244, 1604, 1606 }, // 50 IIC_SprMFCR
  { 1, 222, 224, 1606, 1608 }, // 51 IIC_SprMFSPR
  { 1, 244, 246, 1608, 1609 }, // 52 IIC_IntMFFS
  { 1, 246, 248, 1609, 1611 }, // 53 IIC_SprMFMSR
  { 1, 242, 244, 1611, 1613 }, // 54 IIC_SprMFCRF
  { 1, 246, 248, 1613, 1615 }, // 55 IIC_SprMFPMR
  { 1, 246, 248, 1615, 1617 }, // 56 IIC_SprMFSR
  { 1, 246, 248, 1617, 1619 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 222, 224, 1619, 1621 }, // 59 IIC_SprMTSPR
  { 1, 244, 246, 1621, 1624 }, // 60 IIC_IntMTFSB0
  { 1, 248, 250, 1624, 1626 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 1, 250, 252, 1626, 1628 }, // 63 IIC_SprMTPMR
  { 1, 250, 252, 1628, 1630 }, // 64 IIC_SprMTSR
  { 1, 252, 254, 1630, 1633 }, // 65 IIC_IntMulHW
  { 1, 252, 254, 1633, 1636 }, // 66 IIC_IntMulHWU
  { 1, 252, 254, 1636, 1639 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 1, 220, 222, 1639, 1642 }, // 69 IIC_LdStSTFD
  { 2, 239, 242, 1642, 1645 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 0, 0, 0, 0, 0 }, // 73 IIC_IntRotateD
  { 1, 222, 224, 1645, 1648 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 222, 224, 1648, 1651 }, // 82 IIC_IntShift
  { 1, 220, 222, 1651, 1653 }, // 83 IIC_LdStSTWCX
  { 2, 239, 242, 1653, 1655 }, // 84 IIC_LdStStoreUpd
  { 0, 0, 0, 0, 0 }, // 85 IIC_LdStSTD
  { 0, 0, 0, 0, 0 }, // 86 IIC_LdStSTDCX
  { 0, 0, 0, 0, 0 }, // 87 IIC_LdStSTDU
  { 0, 0, 0, 0, 0 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 254, 256, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 256, 258, 1655, 1657 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 0, 0, 0, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 222, 224, 1657, 1659 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 244, 246, 1659, 1662 }, // 101 MTFSB0_MTFSB1
  { 1, 244, 246, 1662, 1663 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 222, 224, 1663, 1666 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 256, 258, 1666, 1668 }, // 105 TW_TWI
  { 1, 233, 235, 1668, 1671 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 233, 235, 1671, 1674 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 222, 224, 1674, 1677 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 0, 0, 0, 0, 0 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 0, 0, 0, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 222, 224, 1677, 1680 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 222, 224, 1680, 1683 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 222, 224, 1683, 1686 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 224, 226, 1686, 1688 }, // 118 MCRF
  { 1, 222, 224, 1688, 1691 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 224, 226, 1691, 1693 }, // 121 RFEBB
  { 1, 222, 224, 1693, 1695 }, // 122 TBEGIN_TRECHKPT
  { 1, 220, 222, 1695, 1697 }, // 123 WAIT
  { 0, 0, 0, 0, 0 }, // 124 RLDCL_RLDCR
  { 1, 222, 224, 1697, 1700 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 242, 244, 1700, 1702 }, // 127 MFOCRF_MFOCRF8
  { 1, 222, 224, 1702, 1705 }, // 128 MTOCRF_MTOCRF8
  { 1, 224, 226, 1705, 1708 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 222, 224, 1708, 1711 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 233, 235, 1711, 1714 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 222, 224, 1714, 1717 }, // 132 SRAW_SRAWI
  { 1, 222, 224, 1717, 1721 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 222, 224, 1721, 1723 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 220, 222, 1723, 1725 }, // 136 MFVSCR
  { 1, 220, 222, 1725, 1727 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 0, 0, 0, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 233, 235, 1727, 1730 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 0, 0, 0, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 252, 254, 1730, 1733 }, // 145 MULHD_MULHW_MULLW
  { 1, 252, 254, 1733, 1736 }, // 146 MULHDU_MULHWU
  { 1, 252, 254, 1736, 1739 }, // 147 MULLI_MULLI8
  { 1, 233, 235, 1739, 1742 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 229, 231, 1742, 1745 }, // 149 FADD_FSUB
  { 1, 229, 231, 1745, 1749 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 233, 235, 1749, 1752 }, // 152 FSELDo_FSELSo
  { 1, 252, 254, 1752, 1755 }, // 153 MULHDUo_MULHWUo
  { 1, 252, 254, 1755, 1758 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 233, 235, 1758, 1761 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 229, 231, 1761, 1764 }, // 157 FADDo_FSUBo
  { 1, 229, 231, 1764, 1768 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 220, 222, 1768, 1770 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 0, 0, 0, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 233, 235, 1770, 1773 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 220, 222, 1773, 1775 }, // 171 LXVL_LXVLL
  { 1, 220, 222, 1775, 1777 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 220, 222, 1777, 1780 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 220, 222, 1780, 1782 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 220, 222, 1782, 1784 }, // 178 ICBI_ICBIEP
  { 1, 220, 222, 1784, 1786 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 237, 239, 1786, 1789 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 0, 0, 0, 0, 0 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 0, 0, 0, 0, 0 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 0, 0, 0, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 254, 256, 0, 0 }, // 186 TLBSYNC
  { 1, 220, 222, 0, 0 }, // 187 SYNC
  { 1, 220, 222, 1789, 1791 }, // 188 LMW
  { 1, 220, 222, 1791, 1794 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 2, 239, 242, 1794, 1796 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 2, 239, 242, 1796, 1798 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 222, 224, 1798, 1800 }, // 198 TEND
  { 1, 220, 222, 1800, 1802 }, // 199 STBCX_STHCX_STWCX
  { 0, 0, 0, 0, 0 }, // 200 STDCX
  { 0, 0, 0, 0, 0 }, // 201 LDMX
  { 1, 220, 222, 1802, 1804 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 222, 224, 1804, 1806 }, // 205 TCHECK
  { 1, 220, 222, 1806, 1809 }, // 206 LFIWAX
  { 1, 220, 222, 1809, 1812 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 220, 222, 1812, 1815 }, // 209 LFSX_LFS
  { 1, 220, 222, 1815, 1818 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 239, 242, 1818, 1820 }, // 212 LHAU_LHAU8
  { 1, 239, 242, 1820, 1822 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 220, 222, 1822, 1825 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 220, 222, 1825, 1828 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 220, 222, 1828, 1830 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 0, 0, 0, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 220, 222, 1830, 1832 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 220, 222, 1832, 1834 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 220, 222, 1834, 1836 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 220, 222, 1836, 1839 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 220, 222, 1839, 1841 }, // 225 STXVL_STXVLL
  { 1, 222, 224, 1841, 1843 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 222, 224, 1843, 1846 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 1, 246, 248, 1846, 1848 }, // 228 MFPMR
  { 1, 250, 252, 1848, 1850 }, // 229 MTPMR
  { 1, 246, 248, 1850, 1852 }, // 230 MFTB_MFTB8
  { 1, 222, 224, 1852, 1854 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 1, 246, 248, 1854, 1856 }, // 232 MFMSR
  { 1, 248, 250, 1856, 1858 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 222, 224, 1858, 1860 }, // 235 MTSPR_MTSPR8
  { 1, 226, 229, 1860, 1863 }, // 236 DIVW_DIVWU_MODSW
  { 1, 226, 229, 1863, 1866 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 0, 0, 0, 0, 0 }, // 238 DIVD_DIVDU
  { 0, 0, 0, 0, 0 }, // 239 DIVDE_DIVDEU
  { 1, 226, 229, 1866, 1869 }, // 240 DIVWUo_DIVWo
  { 0, 0, 0, 0, 0 }, // 241 DIVDo_DIVDUo
  { 1, 226, 229, 1869, 1872 }, // 242 DIVWEo_DIVWEUo
  { 0, 0, 0, 0, 0 }, // 243 DIVDEo_DIVDEUo
  { 1, 222, 224, 1872, 1875 }, // 244 MTCRF_MTCRF8
  { 1, 222, 224, 1875, 1878 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 233, 235, 1878, 1881 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 224, 226, 1881, 1883 }, // 247 MCRFS
  { 1, 244, 246, 1883, 1884 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 244, 246, 1884, 1887 }, // 249 MTFSFb
  { 0, 0, 0, 0, 0 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 222, 224, 1887, 1890 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 222, 224, 1890, 1893 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 222, 224, 1893, 1896 }, // 254 SRAWIo_SRAWo
  { 1, 244, 246, 1896, 1897 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 242, 244, 1897, 1899 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 0, 0, 0, 0, 0 }, // 258 SRADo_SLDo_SRDo
  { 1, 231, 233, 1899, 1902 }, // 259 FDIV
  { 1, 231, 233, 1902, 1905 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 231, 233, 1905, 1908 }, // 269 XSDIVDP
  { 1, 235, 237, 1908, 1911 }, // 270 FDIVS
  { 1, 235, 237, 1911, 1914 }, // 271 FDIVSo
  { 1, 235, 237, 1914, 1917 }, // 272 XSDIVSP
  { 1, 235, 237, 1917, 1920 }, // 273 XVDIVSP
  { 1, 231, 233, 1920, 1923 }, // 274 XVDIVDP
  { 2, 239, 242, 1923, 1926 }, // 275 LFSU
  { 2, 239, 242, 1926, 1929 }, // 276 LFSUX
  { 2, 239, 242, 1929, 1932 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 2, 239, 242, 1932, 1934 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 0, 0, 0, 0, 0 }, // 279 STDU
  { 0, 0, 0, 0, 0 }, // 280 STDUX
  { 2, 239, 242, 1934, 1936 }, // 281 LBZU_LBZU8
  { 2, 239, 242, 1936, 1938 }, // 282 LBZUX_LBZUX8
  { 0, 0, 0, 0, 0 }, // 283 LDU
  { 0, 0, 0, 0, 0 }, // 284 LDUX
  { 2, 239, 242, 1938, 1941 }, // 285 LFDU
  { 2, 239, 242, 1941, 1944 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 224, 226, 1944, 1946 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 224, 226, 1946, 1948 }, // 289 ADDPCIS
  { 1, 220, 222, 1948, 1950 }, // 290 LDAT_LWAT
  { 1, 220, 222, 1950, 1952 }, // 291 STDAT_STWAT
  { 1, 222, 224, 1952, 1955 }, // 292 BRINC
  { 0, 0, 0, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 0, 0, 0, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 220, 222, 1955, 1957 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 220, 222, 1957, 1959 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 224, 226, 1959, 1961 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 220, 222, 1961, 1963 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 220, 222, 1963, 1965 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 220, 222, 1965, 1968 }, // 302 LFDEPX
  { 1, 220, 222, 1968, 1971 }, // 303 STFDEPX
  { 1, 246, 248, 1971, 1973 }, // 304 MFSR_MFSRIN
  { 1, 250, 252, 1973, 1975 }, // 305 MTSR_MTSRIN
  { 1, 222, 224, 1975, 1977 }, // 306 MFDCR
  { 1, 222, 224, 1977, 1979 }, // 307 MTDCR
  { 1, 222, 224, 1979, 1982 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 248, 250, 1982, 1984 }, // 310 WRTEE_WRTEEI
  { 1, 220, 222, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 220, 222, 1984, 1986 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary PPCE5500Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 1, 258, 260, 0, 0 }, // 1 IIC_LdStSync
  { 1, 260, 262, 1986, 1989 }, // 2 IIC_IntSimple
  { 1, 260, 262, 1989, 1992 }, // 3 IIC_IntGeneral
  { 1, 262, 264, 1992, 1994 }, // 4 IIC_BrB
  { 0, 0, 0, 0, 0 }, // 5 IIC_VecFP
  { 1, 260, 262, 1994, 1997 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 262, 264, 1997, 2000 }, // 10 IIC_BrCR
  { 1, 258, 260, 2000, 2002 }, // 11 IIC_LdStLD
  { 1, 258, 260, 2002, 2004 }, // 12 IIC_LdStDCBF
  { 1, 258, 260, 2004, 2006 }, // 13 IIC_LdStLoad
  { 1, 264, 267, 2006, 2009 }, // 14 IIC_IntDivD
  { 1, 267, 270, 2009, 2012 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 270, 272, 2012, 2015 }, // 17 IIC_FPAddSub
  { 1, 272, 274, 2015, 2018 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 270, 272, 2018, 2021 }, // 20 IIC_FPCompare
  { 1, 270, 272, 2021, 2024 }, // 21 IIC_FPGeneral
  { 0, 0, 0, 0, 0 }, // 22 IIC_VecGeneral
  { 0, 0, 0, 0, 0 }, // 23 IIC_VecComplex
  { 1, 258, 260, 2024, 2026 }, // 24 IIC_LdStStore
  { 1, 260, 262, 2026, 2029 }, // 25 IIC_IntRotateDI
  { 1, 274, 276, 2029, 2032 }, // 26 IIC_FPDivS
  { 1, 270, 272, 2032, 2036 }, // 27 IIC_FPFused
  { 0, 0, 0, 0, 0 }, // 28 IIC_FPSqrtD
  { 0, 0, 0, 0, 0 }, // 29 IIC_FPSqrtS
  { 1, 258, 260, 2036, 2038 }, // 30 IIC_LdStICBI
  { 1, 260, 262, 2038, 2042 }, // 31 IIC_IntISEL
  { 0, 0, 0, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 276, 278, 2042, 2045 }, // 33 IIC_LdStLWARX
  { 2, 278, 281, 2045, 2047 }, // 34 IIC_LdStLoadUpd
  { 2, 278, 281, 2047, 2049 }, // 35 IIC_LdStLoadUpdX
  { 1, 276, 278, 2049, 2051 }, // 36 IIC_LdStLDARX
  { 2, 278, 281, 2051, 2053 }, // 37 IIC_LdStLDU
  { 2, 278, 281, 2053, 2055 }, // 38 IIC_LdStLDUX
  { 2, 258, 260, 2055, 2058 }, // 39 IIC_LdStLFD
  { 2, 278, 281, 2058, 2061 }, // 40 IIC_LdStLFDU
  { 2, 278, 281, 2061, 2064 }, // 41 IIC_LdStLFDUX
  { 1, 258, 260, 2064, 2066 }, // 42 IIC_LdStLHA
  { 2, 278, 281, 2066, 2068 }, // 43 IIC_LdStLHAU
  { 2, 278, 281, 2068, 2070 }, // 44 IIC_LdStLHAUX
  { 1, 281, 283, 2070, 2072 }, // 45 IIC_LdStLMW
  { 0, 0, 0, 0, 0 }, // 46 IIC_LdStLWA
  { 1, 283, 286, 2072, 2075 }, // 47 IIC_IntMulHD
  { 1, 262, 264, 2075, 2077 }, // 48 IIC_BrMCR
  { 1, 286, 288, 2077, 2080 }, // 49 IIC_BrMCRX
  { 1, 288, 290, 2080, 2082 }, // 50 IIC_SprMFCR
  { 1, 286, 288, 2082, 2083 }, // 51 IIC_SprMFSPR
  { 1, 270, 272, 2083, 2084 }, // 52 IIC_IntMFFS
  { 0, 0, 0, 0, 0 }, // 53 IIC_SprMFMSR
  { 1, 288, 290, 2084, 2086 }, // 54 IIC_SprMFCRF
  { 1, 290, 292, 2086, 2088 }, // 55 IIC_SprMFPMR
  { 0, 0, 0, 0, 0 }, // 56 IIC_SprMFSR
  { 1, 290, 292, 2088, 2090 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 286, 288, 2090, 2091 }, // 59 IIC_SprMTSPR
  { 1, 292, 294, 2091, 2094 }, // 60 IIC_IntMTFSB0
  { 1, 294, 296, 2094, 2096 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 1, 286, 288, 2096, 2097 }, // 63 IIC_SprMTPMR
  { 0, 0, 0, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 296, 299, 2097, 2100 }, // 65 IIC_IntMulHW
  { 1, 296, 299, 2100, 2103 }, // 66 IIC_IntMulHWU
  { 1, 283, 286, 2103, 2106 }, // 67 IIC_IntMulLI
  { 0, 0, 0, 0, 0 }, // 68 IIC_VecPerm
  { 1, 258, 260, 2106, 2109 }, // 69 IIC_LdStSTFD
  { 2, 278, 281, 2109, 2112 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 1, 299, 301, 2112, 2115 }, // 73 IIC_IntRotateD
  { 1, 260, 262, 2115, 2118 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 299, 301, 2118, 2121 }, // 82 IIC_IntShift
  { 1, 258, 260, 2121, 2123 }, // 83 IIC_LdStSTWCX
  { 2, 278, 281, 2123, 2125 }, // 84 IIC_LdStStoreUpd
  { 1, 258, 260, 2125, 2127 }, // 85 IIC_LdStSTD
  { 1, 258, 260, 2127, 2129 }, // 86 IIC_LdStSTDCX
  { 2, 278, 281, 2129, 2131 }, // 87 IIC_LdStSTDU
  { 2, 278, 281, 2131, 2133 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 0, 0, 0, 0, 0 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 1, 301, 303, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 303, 305, 2133, 2135 }, // 95 IIC_IntTrapW
  { 0, 0, 0, 0, 0 }, // 96 IIC_VecFPCompare
  { 0, 0, 0, 0, 0 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 0, 0, 0, 0, 0 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 0, 0, 0, 0, 0 }, // 99 XXSEL
  { 1, 286, 288, 2135, 2136 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 1, 292, 294, 2136, 2139 }, // 101 MTFSB0_MTFSB1
  { 1, 270, 272, 2139, 2140 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 260, 262, 2140, 2143 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 0, 0, 0, 0, 0 }, // 104 TD_TDI
  { 1, 303, 305, 2143, 2145 }, // 105 TW_TWI
  { 1, 270, 272, 2145, 2148 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 0, 0, 0, 0, 0 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 0, 0, 0, 0, 0 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 270, 272, 2148, 2151 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 260, 262, 2151, 2154 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 1, 299, 301, 2154, 2157 }, // 111 SLD_SRD_SRAD
  { 1, 260, 262, 2157, 2160 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 0, 0, 0, 0, 0 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 260, 262, 2160, 2163 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 260, 262, 2163, 2166 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 260, 262, 2166, 2169 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 262, 264, 2169, 2171 }, // 118 MCRF
  { 1, 286, 288, 2171, 2174 }, // 119 MCRXRX
  { 0, 0, 0, 0, 0 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 262, 264, 2174, 2176 }, // 121 RFEBB
  { 1, 286, 288, 2176, 2177 }, // 122 TBEGIN_TRECHKPT
  { 1, 258, 260, 2177, 2179 }, // 123 WAIT
  { 1, 299, 301, 2179, 2182 }, // 124 RLDCL_RLDCR
  { 1, 260, 262, 2182, 2185 }, // 125 RLWIMI_RLWIMI8
  { 1, 260, 262, 2185, 2188 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 288, 290, 2188, 2190 }, // 127 MFOCRF_MFOCRF8
  { 1, 286, 288, 2190, 2193 }, // 128 MTOCRF_MTOCRF8
  { 1, 262, 264, 2193, 2196 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 260, 262, 2196, 2199 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 270, 272, 2199, 2202 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 299, 301, 2202, 2205 }, // 132 SRAW_SRAWI
  { 1, 260, 262, 2205, 2209 }, // 133 ISEL_ISEL8
  { 0, 0, 0, 0, 0 }, // 134 XSIEXPDP
  { 1, 286, 288, 2209, 2210 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 258, 260, 2210, 2212 }, // 136 MFVSCR
  { 1, 258, 260, 2212, 2214 }, // 137 MTVSCR
  { 0, 0, 0, 0, 0 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 0, 0, 0, 0, 0 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 0, 0, 0, 0, 0 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 270, 272, 2214, 2217 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 0, 0, 0, 0, 0 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 0, 0, 0, 0, 0 }, // 143 VSUBFP_VMULUWM
  { 1, 283, 286, 2217, 2220 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 296, 299, 2220, 2223 }, // 145 MULHD_MULHW_MULLW
  { 1, 296, 299, 2223, 2226 }, // 146 MULHDU_MULHWU
  { 1, 283, 286, 2226, 2229 }, // 147 MULLI_MULLI8
  { 1, 270, 272, 2229, 2232 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 270, 272, 2232, 2235 }, // 149 FADD_FSUB
  { 1, 270, 272, 2235, 2239 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 0, 0, 0, 0, 0 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 270, 272, 2239, 2242 }, // 152 FSELDo_FSELSo
  { 1, 296, 299, 2242, 2245 }, // 153 MULHDUo_MULHWUo
  { 1, 296, 299, 2245, 2248 }, // 154 MULHDo_MULHWo_MULLWo
  { 1, 283, 286, 2248, 2251 }, // 155 MULLDo
  { 1, 270, 272, 2251, 2254 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 270, 272, 2254, 2257 }, // 157 FADDo_FSUBo
  { 1, 270, 272, 2257, 2261 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 0, 0, 0, 0, 0 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 258, 260, 2261, 2263 }, // 160 LVSL_LVSR
  { 0, 0, 0, 0, 0 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 0, 0, 0, 0, 0 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 0, 0, 0, 0, 0 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 270, 272, 2263, 2266 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 0, 0, 0, 0, 0 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 0, 0, 0, 0, 0 }, // 166 BCDCTSQo
  { 0, 0, 0, 0, 0 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 0, 0, 0, 0, 0 }, // 168 BCDCFSQo
  { 0, 0, 0, 0, 0 }, // 169 XSDIVQP_XSDIVQPO
  { 0, 0, 0, 0, 0 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 258, 260, 2266, 2268 }, // 171 LXVL_LXVLL
  { 1, 258, 260, 2268, 2270 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 2, 258, 260, 2270, 2273 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 1, 258, 260, 2273, 2275 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 1, 258, 260, 2275, 2277 }, // 178 ICBI_ICBIEP
  { 1, 258, 260, 2277, 2279 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 276, 278, 2279, 2282 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 1, 258, 260, 2282, 2284 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 1, 276, 278, 2284, 2286 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 0, 0, 0, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 1, 301, 303, 0, 0 }, // 186 TLBSYNC
  { 1, 258, 260, 0, 0 }, // 187 SYNC
  { 1, 281, 283, 2286, 2288 }, // 188 LMW
  { 2, 258, 260, 2288, 2291 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 2, 278, 281, 2291, 2293 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 2, 278, 281, 2293, 2295 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 286, 288, 2295, 2296 }, // 198 TEND
  { 1, 258, 260, 2296, 2298 }, // 199 STBCX_STHCX_STWCX
  { 1, 258, 260, 2298, 2300 }, // 200 STDCX
  { 1, 258, 260, 2300, 2302 }, // 201 LDMX
  { 1, 258, 260, 2302, 2304 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 0, 0, 0, 0, 0 }, // 204 LWA_LWA_32
  { 1, 286, 288, 2304, 2305 }, // 205 TCHECK
  { 2, 258, 260, 2305, 2308 }, // 206 LFIWAX
  { 2, 258, 260, 2308, 2311 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 2, 258, 260, 2311, 2314 }, // 209 LFSX_LFS
  { 2, 258, 260, 2314, 2317 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 2, 278, 281, 2317, 2319 }, // 212 LHAU_LHAU8
  { 2, 278, 281, 2319, 2321 }, // 213 LHAUX_LHAUX8_LWAUX
  { 2, 258, 260, 2321, 2324 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 258, 260, 2324, 2327 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 258, 260, 2327, 2329 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 258, 260, 2329, 2331 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 258, 260, 2331, 2333 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 281, 283, 2333, 2335 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 258, 260, 2335, 2337 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 258, 260, 2337, 2340 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 258, 260, 2340, 2342 }, // 225 STXVL_STXVLL
  { 1, 286, 288, 2342, 2343 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 260, 262, 2343, 2346 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 1, 290, 292, 2346, 2348 }, // 228 MFPMR
  { 1, 286, 288, 2348, 2349 }, // 229 MTPMR
  { 1, 290, 292, 2349, 2351 }, // 230 MFTB_MFTB8
  { 1, 286, 288, 2351, 2352 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 0, 0, 0, 0, 0 }, // 232 MFMSR
  { 1, 294, 296, 2352, 2354 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 286, 288, 2354, 2355 }, // 235 MTSPR_MTSPR8
  { 1, 267, 270, 2355, 2358 }, // 236 DIVW_DIVWU_MODSW
  { 1, 267, 270, 2358, 2361 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 1, 264, 267, 2361, 2364 }, // 238 DIVD_DIVDU
  { 1, 264, 267, 2364, 2367 }, // 239 DIVDE_DIVDEU
  { 1, 267, 270, 2367, 2370 }, // 240 DIVWUo_DIVWo
  { 1, 264, 267, 2370, 2373 }, // 241 DIVDo_DIVDUo
  { 1, 267, 270, 2373, 2376 }, // 242 DIVWEo_DIVWEUo
  { 1, 264, 267, 2376, 2379 }, // 243 DIVDEo_DIVDEUo
  { 1, 286, 288, 2379, 2382 }, // 244 MTCRF_MTCRF8
  { 1, 260, 262, 2382, 2385 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 270, 272, 2385, 2388 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 262, 264, 2388, 2390 }, // 247 MCRFS
  { 1, 270, 272, 2390, 2391 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 1, 292, 294, 2391, 2394 }, // 249 MTFSFb
  { 1, 299, 301, 2394, 2397 }, // 250 RLDCLo_RLDCRo
  { 1, 260, 262, 2397, 2400 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 260, 262, 2400, 2403 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 260, 262, 2403, 2406 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 299, 301, 2406, 2409 }, // 254 SRAWIo_SRAWo
  { 1, 270, 272, 2409, 2410 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 288, 290, 2410, 2412 }, // 256 MFCR_MFCR8
  { 1, 260, 262, 2412, 2415 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 1, 299, 301, 2415, 2418 }, // 258 SRADo_SLDo_SRDo
  { 1, 272, 274, 2418, 2421 }, // 259 FDIV
  { 1, 272, 274, 2421, 2424 }, // 260 FDIVo
  { 0, 0, 0, 0, 0 }, // 261 XSSQRTDP
  { 0, 0, 0, 0, 0 }, // 262 FSQRT
  { 0, 0, 0, 0, 0 }, // 263 XVSQRTDP
  { 0, 0, 0, 0, 0 }, // 264 XVSQRTSP
  { 0, 0, 0, 0, 0 }, // 265 FSQRTo
  { 0, 0, 0, 0, 0 }, // 266 XSSQRTSP
  { 0, 0, 0, 0, 0 }, // 267 FSQRTS
  { 0, 0, 0, 0, 0 }, // 268 FSQRTSo
  { 1, 272, 274, 2424, 2427 }, // 269 XSDIVDP
  { 1, 274, 276, 2427, 2430 }, // 270 FDIVS
  { 1, 274, 276, 2430, 2433 }, // 271 FDIVSo
  { 1, 274, 276, 2433, 2436 }, // 272 XSDIVSP
  { 1, 274, 276, 2436, 2439 }, // 273 XVDIVSP
  { 1, 272, 274, 2439, 2442 }, // 274 XVDIVDP
  { 2, 278, 281, 2442, 2445 }, // 275 LFSU
  { 2, 278, 281, 2445, 2448 }, // 276 LFSUX
  { 2, 278, 281, 2448, 2451 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 2, 278, 281, 2451, 2453 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 2, 278, 281, 2453, 2455 }, // 279 STDU
  { 2, 278, 281, 2455, 2457 }, // 280 STDUX
  { 2, 278, 281, 2457, 2459 }, // 281 LBZU_LBZU8
  { 2, 278, 281, 2459, 2461 }, // 282 LBZUX_LBZUX8
  { 2, 278, 281, 2461, 2463 }, // 283 LDU
  { 2, 278, 281, 2463, 2465 }, // 284 LDUX
  { 2, 278, 281, 2465, 2468 }, // 285 LFDU
  { 2, 278, 281, 2468, 2471 }, // 286 LFDUX
  { 0, 0, 0, 0, 0 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 262, 264, 2471, 2473 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 262, 264, 2473, 2475 }, // 289 ADDPCIS
  { 1, 258, 260, 2475, 2477 }, // 290 LDAT_LWAT
  { 1, 258, 260, 2477, 2479 }, // 291 STDAT_STWAT
  { 1, 260, 262, 2479, 2482 }, // 292 BRINC
  { 0, 0, 0, 0, 0 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 0, 0, 0, 0, 0 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 258, 260, 2482, 2484 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 258, 260, 2484, 2486 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 262, 264, 2486, 2488 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 258, 260, 2488, 2490 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 258, 260, 2490, 2492 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 2, 258, 260, 2492, 2495 }, // 302 LFDEPX
  { 1, 258, 260, 2495, 2498 }, // 303 STFDEPX
  { 0, 0, 0, 0, 0 }, // 304 MFSR_MFSRIN
  { 0, 0, 0, 0, 0 }, // 305 MTSR_MTSRIN
  { 1, 286, 288, 2498, 2499 }, // 306 MFDCR
  { 1, 286, 288, 2499, 2500 }, // 307 MTDCR
  { 1, 260, 262, 2500, 2503 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 1, 294, 296, 2503, 2505 }, // 310 WRTEE_WRTEEI
  { 1, 258, 260, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 1, 258, 260, 2505, 2507 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary P8Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 0, 0, 0, 0, 0 }, // 1 IIC_LdStSync
  { 1, 305, 307, 2507, 2510 }, // 2 IIC_IntSimple
  { 1, 305, 307, 2510, 2513 }, // 3 IIC_IntGeneral
  { 1, 307, 309, 2513, 2516 }, // 4 IIC_BrB
  { 1, 309, 311, 2516, 2519 }, // 5 IIC_VecFP
  { 1, 311, 313, 2519, 2522 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 313, 315, 2522, 2525 }, // 10 IIC_BrCR
  { 1, 315, 317, 2525, 2528 }, // 11 IIC_LdStLD
  { 0, 0, 0, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 315, 317, 2528, 2531 }, // 13 IIC_LdStLoad
  { 1, 317, 319, 2531, 2534 }, // 14 IIC_IntDivD
  { 1, 319, 321, 2534, 2537 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 321, 323, 2537, 2540 }, // 17 IIC_FPAddSub
  { 1, 321, 323, 2540, 2543 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 321, 323, 2543, 2546 }, // 20 IIC_FPCompare
  { 1, 321, 323, 2546, 2549 }, // 21 IIC_FPGeneral
  { 1, 309, 311, 2549, 2552 }, // 22 IIC_VecGeneral
  { 1, 309, 311, 2552, 2555 }, // 23 IIC_VecComplex
  { 1, 323, 326, 2555, 2558 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 321, 323, 2558, 2561 }, // 26 IIC_FPDivS
  { 1, 321, 323, 2561, 2565 }, // 27 IIC_FPFused
  { 1, 321, 323, 2565, 2568 }, // 28 IIC_FPSqrtD
  { 1, 321, 323, 2568, 2571 }, // 29 IIC_FPSqrtS
  { 0, 0, 0, 0, 0 }, // 30 IIC_LdStICBI
  { 1, 326, 329, 2571, 2575 }, // 31 IIC_IntISEL
  { 0, 0, 0, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 329, 334, 2575, 2578 }, // 33 IIC_LdStLWARX
  { 1, 334, 338, 2578, 2582 }, // 34 IIC_LdStLoadUpd
  { 1, 334, 338, 2582, 2586 }, // 35 IIC_LdStLoadUpdX
  { 1, 338, 345, 2586, 2589 }, // 36 IIC_LdStLDARX
  { 1, 334, 338, 2589, 2593 }, // 37 IIC_LdStLDU
  { 1, 334, 338, 2593, 2597 }, // 38 IIC_LdStLDUX
  { 1, 345, 347, 2597, 2600 }, // 39 IIC_LdStLFD
  { 1, 347, 351, 2600, 2604 }, // 40 IIC_LdStLFDU
  { 1, 347, 351, 2604, 2608 }, // 41 IIC_LdStLFDUX
  { 1, 351, 355, 2608, 2611 }, // 42 IIC_LdStLHA
  { 1, 355, 360, 2611, 2615 }, // 43 IIC_LdStLHAU
  { 1, 360, 369, 2615, 2619 }, // 44 IIC_LdStLHAUX
  { 1, 315, 317, 2619, 2622 }, // 45 IIC_LdStLMW
  { 1, 369, 373, 2622, 2625 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 373, 375, 2625, 2628 }, // 48 IIC_BrMCR
  { 1, 375, 377, 2628, 2630 }, // 49 IIC_BrMCRX
  { 1, 313, 315, 2630, 2632 }, // 50 IIC_SprMFCR
  { 0, 0, 0, 0, 0 }, // 51 IIC_SprMFSPR
  { 0, 0, 0, 0, 0 }, // 52 IIC_IntMFFS
  { 0, 0, 0, 0, 0 }, // 53 IIC_SprMFMSR
  { 1, 313, 315, 2632, 2634 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 0, 0, 0, 0, 0 }, // 56 IIC_SprMFSR
  { 0, 0, 0, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 375, 377, 2634, 2636 }, // 59 IIC_SprMTSPR
  { 0, 0, 0, 0, 0 }, // 60 IIC_IntMTFSB0
  { 0, 0, 0, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 0, 0, 0, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 311, 313, 2636, 2639 }, // 65 IIC_IntMulHW
  { 1, 311, 313, 2639, 2642 }, // 66 IIC_IntMulHWU
  { 1, 311, 313, 2642, 2645 }, // 67 IIC_IntMulLI
  { 1, 377, 379, 2645, 2648 }, // 68 IIC_VecPerm
  { 1, 379, 382, 2648, 2651 }, // 69 IIC_LdStSTFD
  { 1, 382, 387, 2651, 2655 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 1, 311, 313, 2655, 2658 }, // 73 IIC_IntRotateD
  { 1, 311, 313, 2658, 2661 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 311, 313, 2661, 2664 }, // 82 IIC_IntShift
  { 1, 387, 395, 2664, 2667 }, // 83 IIC_LdStSTWCX
  { 0, 0, 0, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 1, 395, 398, 0, 0 }, // 85 IIC_LdStSTD
  { 1, 387, 395, 2667, 2670 }, // 86 IIC_LdStSTDCX
  { 1, 398, 402, 2670, 2674 }, // 87 IIC_LdStSTDU
  { 1, 402, 411, 2674, 2678 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 1, 311, 313, 2678, 2680 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 0, 0, 0, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 311, 313, 2680, 2682 }, // 95 IIC_IntTrapW
  { 1, 309, 311, 2682, 2685 }, // 96 IIC_VecFPCompare
  { 1, 309, 311, 2685, 2688 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 1, 309, 311, 2688, 2691 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 1, 377, 379, 2691, 2694 }, // 99 XXSEL
  { 1, 375, 377, 2694, 2696 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 0, 0, 0, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 0, 0, 0, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 311, 313, 2696, 2699 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 1, 311, 313, 2699, 2701 }, // 104 TD_TDI
  { 1, 311, 313, 2701, 2703 }, // 105 TW_TWI
  { 1, 321, 323, 2703, 2706 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 1, 309, 311, 2706, 2709 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 1, 309, 311, 2709, 2712 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 321, 323, 2712, 2715 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 305, 307, 2715, 2718 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 1, 311, 313, 2718, 2721 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 309, 311, 2721, 2724 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 311, 313, 2724, 2727 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 305, 307, 2727, 2730 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 305, 307, 2730, 2733 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 373, 375, 2733, 2736 }, // 118 MCRF
  { 1, 375, 377, 2736, 2738 }, // 119 MCRXRX
  { 1, 309, 311, 2738, 2741 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 307, 309, 2741, 2744 }, // 121 RFEBB
  { 1, 375, 377, 2744, 2746 }, // 122 TBEGIN_TRECHKPT
  { 1, 315, 317, 2746, 2749 }, // 123 WAIT
  { 1, 311, 313, 2749, 2752 }, // 124 RLDCL_RLDCR
  { 1, 311, 313, 2752, 2755 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 313, 315, 2755, 2757 }, // 127 MFOCRF_MFOCRF8
  { 1, 375, 377, 2757, 2759 }, // 128 MTOCRF_MTOCRF8
  { 1, 313, 315, 2759, 2762 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 305, 307, 2762, 2765 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 321, 323, 2765, 2768 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 311, 313, 2768, 2771 }, // 132 SRAW_SRAWI
  { 1, 326, 329, 2771, 2775 }, // 133 ISEL_ISEL8
  { 1, 309, 311, 2775, 2778 }, // 134 XSIEXPDP
  { 1, 375, 377, 2778, 2780 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 323, 326, 2780, 2783 }, // 136 MFVSCR
  { 1, 315, 317, 2783, 2786 }, // 137 MTVSCR
  { 1, 309, 311, 2786, 2789 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 309, 311, 2789, 2792 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 1, 309, 311, 2792, 2795 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 321, 323, 2795, 2798 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 1, 309, 311, 2798, 2801 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 309, 311, 2801, 2804 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 311, 313, 2804, 2807 }, // 145 MULHD_MULHW_MULLW
  { 1, 311, 313, 2807, 2810 }, // 146 MULHDU_MULHWU
  { 1, 311, 313, 2810, 2813 }, // 147 MULLI_MULLI8
  { 1, 321, 323, 2813, 2816 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 321, 323, 2816, 2819 }, // 149 FADD_FSUB
  { 1, 321, 323, 2819, 2823 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 1, 309, 311, 2823, 2826 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 321, 323, 2826, 2829 }, // 152 FSELDo_FSELSo
  { 1, 311, 313, 2829, 2832 }, // 153 MULHDUo_MULHWUo
  { 1, 311, 313, 2832, 2835 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 321, 323, 2835, 2838 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 321, 323, 2838, 2841 }, // 157 FADDo_FSUBo
  { 1, 321, 323, 2841, 2845 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 1, 309, 311, 2845, 2848 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 315, 317, 2848, 2851 }, // 160 LVSL_LVSR
  { 1, 377, 379, 2851, 2854 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 1, 309, 311, 2854, 2857 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 309, 311, 2857, 2860 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 321, 323, 2860, 2863 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 1, 309, 311, 2863, 2866 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 1, 309, 311, 2866, 2869 }, // 166 BCDCTSQo
  { 1, 309, 311, 2869, 2872 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 1, 309, 311, 2872, 2875 }, // 168 BCDCFSQo
  { 1, 309, 311, 2875, 2878 }, // 169 XSDIVQP_XSDIVQPO
  { 1, 309, 311, 2878, 2881 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 315, 317, 2881, 2884 }, // 171 LXVL_LXVLL
  { 1, 315, 317, 2884, 2887 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 345, 347, 2887, 2890 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 0, 0, 0, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 0, 0, 0, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 315, 317, 2890, 2893 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 329, 334, 2893, 2896 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 1, 315, 317, 2896, 2899 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 1, 338, 345, 2899, 2902 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 0, 0, 0, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 0, 0, 0, 0, 0 }, // 186 TLBSYNC
  { 0, 0, 0, 0, 0 }, // 187 SYNC
  { 1, 315, 317, 2902, 2905 }, // 188 LMW
  { 1, 345, 347, 2905, 2908 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 334, 338, 2908, 2912 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 334, 338, 2912, 2916 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 375, 377, 2916, 2918 }, // 198 TEND
  { 1, 387, 395, 2918, 2921 }, // 199 STBCX_STHCX_STWCX
  { 1, 387, 395, 2921, 2924 }, // 200 STDCX
  { 1, 315, 317, 2924, 2927 }, // 201 LDMX
  { 1, 351, 355, 2927, 2930 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 1, 369, 373, 2930, 2933 }, // 204 LWA_LWA_32
  { 1, 375, 377, 2933, 2935 }, // 205 TCHECK
  { 1, 345, 347, 2935, 2938 }, // 206 LFIWAX
  { 1, 345, 347, 2938, 2941 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 345, 347, 2941, 2944 }, // 209 LFSX_LFS
  { 1, 345, 347, 2944, 2947 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 355, 360, 2947, 2951 }, // 212 LHAU_LHAU8
  { 1, 360, 369, 2951, 2955 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 345, 347, 2955, 2958 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 379, 382, 2958, 2961 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 323, 326, 2961, 2964 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 395, 398, 0, 0 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 315, 317, 2964, 2967 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 315, 317, 2967, 2970 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 323, 326, 2970, 2973 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 379, 382, 2973, 2976 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 315, 317, 2976, 2979 }, // 225 STXVL_STXVLL
  { 1, 375, 377, 2979, 2981 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 305, 307, 2981, 2984 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 0, 0, 0, 0, 0 }, // 230 MFTB_MFTB8
  { 0, 0, 0, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 0, 0, 0, 0, 0 }, // 232 MFMSR
  { 0, 0, 0, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 375, 377, 2984, 2986 }, // 235 MTSPR_MTSPR8
  { 1, 319, 321, 2986, 2989 }, // 236 DIVW_DIVWU_MODSW
  { 1, 319, 321, 2989, 2992 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 1, 317, 319, 2992, 2995 }, // 238 DIVD_DIVDU
  { 1, 317, 319, 2995, 2998 }, // 239 DIVDE_DIVDEU
  { 1, 319, 321, 2998, 3001 }, // 240 DIVWUo_DIVWo
  { 1, 317, 319, 3001, 3004 }, // 241 DIVDo_DIVDUo
  { 1, 319, 321, 3004, 3007 }, // 242 DIVWEo_DIVWEUo
  { 1, 317, 319, 3007, 3010 }, // 243 DIVDEo_DIVDEUo
  { 1, 375, 377, 3010, 3012 }, // 244 MTCRF_MTCRF8
  { 1, 305, 307, 3012, 3015 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 321, 323, 3015, 3018 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 373, 375, 3018, 3021 }, // 247 MCRFS
  { 0, 0, 0, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 0, 0, 0, 0, 0 }, // 249 MTFSFb
  { 1, 311, 313, 3021, 3024 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 311, 313, 3024, 3027 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 305, 307, 3027, 3030 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 311, 313, 3030, 3033 }, // 254 SRAWIo_SRAWo
  { 0, 0, 0, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 313, 315, 3033, 3035 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 1, 311, 313, 3035, 3038 }, // 258 SRADo_SLDo_SRDo
  { 1, 321, 323, 3038, 3041 }, // 259 FDIV
  { 1, 321, 323, 3041, 3044 }, // 260 FDIVo
  { 1, 321, 323, 3044, 3047 }, // 261 XSSQRTDP
  { 1, 321, 323, 3047, 3050 }, // 262 FSQRT
  { 1, 321, 323, 3050, 3053 }, // 263 XVSQRTDP
  { 1, 321, 323, 3053, 3056 }, // 264 XVSQRTSP
  { 1, 321, 323, 3056, 3059 }, // 265 FSQRTo
  { 1, 321, 323, 3059, 3062 }, // 266 XSSQRTSP
  { 1, 321, 323, 3062, 3065 }, // 267 FSQRTS
  { 1, 321, 323, 3065, 3068 }, // 268 FSQRTSo
  { 1, 321, 323, 3068, 3071 }, // 269 XSDIVDP
  { 1, 321, 323, 3071, 3074 }, // 270 FDIVS
  { 1, 321, 323, 3074, 3077 }, // 271 FDIVSo
  { 1, 321, 323, 3077, 3080 }, // 272 XSDIVSP
  { 1, 321, 323, 3080, 3083 }, // 273 XVDIVSP
  { 1, 321, 323, 3083, 3086 }, // 274 XVDIVDP
  { 1, 347, 351, 3086, 3090 }, // 275 LFSU
  { 1, 347, 351, 3090, 3094 }, // 276 LFSUX
  { 1, 382, 387, 3094, 3098 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 0, 0, 0, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 398, 402, 3098, 3102 }, // 279 STDU
  { 1, 402, 411, 3102, 3106 }, // 280 STDUX
  { 1, 334, 338, 3106, 3110 }, // 281 LBZU_LBZU8
  { 1, 334, 338, 3110, 3114 }, // 282 LBZUX_LBZUX8
  { 1, 334, 338, 3114, 3118 }, // 283 LDU
  { 1, 334, 338, 3118, 3122 }, // 284 LDUX
  { 1, 347, 351, 3122, 3126 }, // 285 LFDU
  { 1, 347, 351, 3126, 3130 }, // 286 LFDUX
  { 1, 309, 311, 3130, 3133 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 307, 309, 3133, 3136 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 307, 309, 3136, 3139 }, // 289 ADDPCIS
  { 1, 315, 317, 3139, 3142 }, // 290 LDAT_LWAT
  { 1, 323, 326, 3142, 3145 }, // 291 STDAT_STWAT
  { 1, 305, 307, 3145, 3148 }, // 292 BRINC
  { 1, 309, 311, 3148, 3151 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 309, 311, 3151, 3154 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 315, 317, 3154, 3157 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 323, 326, 3157, 3160 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 307, 309, 3160, 3163 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 315, 317, 3163, 3166 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 323, 326, 3166, 3169 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 345, 347, 3169, 3172 }, // 302 LFDEPX
  { 1, 379, 382, 3172, 3175 }, // 303 STFDEPX
  { 0, 0, 0, 0, 0 }, // 304 MFSR_MFSRIN
  { 0, 0, 0, 0, 0 }, // 305 MTSR_MTSRIN
  { 0, 0, 0, 0, 0 }, // 306 MFDCR
  { 1, 375, 377, 3175, 3177 }, // 307 MTDCR
  { 1, 305, 307, 3177, 3180 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 0, 0, 0, 0, 0 }, // 310 WRTEE_WRTEEI
  { 0, 0, 0, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 0, 0, 0, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

static const llvm::InstrItinerary P7Itineraries[] = {
  { 0, 0, 0, 0, 0 }, // 0 NoInstrModel
  { 0, 0, 0, 0, 0 }, // 1 IIC_LdStSync
  { 1, 411, 413, 3180, 3183 }, // 2 IIC_IntSimple
  { 1, 413, 415, 3183, 3186 }, // 3 IIC_IntGeneral
  { 1, 415, 417, 3186, 3189 }, // 4 IIC_BrB
  { 1, 417, 419, 3189, 3192 }, // 5 IIC_VecFP
  { 1, 413, 415, 3192, 3195 }, // 6 IIC_IntCompare
  { 0, 0, 0, 0, 0 }, // 7 IIC_SprABORT
  { 0, 0, 0, 0, 0 }, // 8 IIC_LdStCOPY
  { 0, 0, 0, 0, 0 }, // 9 IIC_LdStPASTE
  { 1, 419, 421, 3195, 3198 }, // 10 IIC_BrCR
  { 1, 421, 423, 3198, 3201 }, // 11 IIC_LdStLD
  { 0, 0, 0, 0, 0 }, // 12 IIC_LdStDCBF
  { 1, 421, 423, 3201, 3204 }, // 13 IIC_LdStLoad
  { 1, 423, 426, 3204, 3207 }, // 14 IIC_IntDivD
  { 1, 426, 429, 3207, 3210 }, // 15 IIC_IntDivW
  { 0, 0, 0, 0, 0 }, // 16 IIC_FPDGeneral
  { 1, 429, 431, 3210, 3213 }, // 17 IIC_FPAddSub
  { 1, 429, 431, 3213, 3216 }, // 18 IIC_FPDivD
  { 0, 0, 0, 0, 0 }, // 19 IIC_FPSGeneral
  { 1, 429, 431, 3216, 3219 }, // 20 IIC_FPCompare
  { 1, 429, 431, 3219, 3222 }, // 21 IIC_FPGeneral
  { 1, 431, 433, 3222, 3225 }, // 22 IIC_VecGeneral
  { 1, 431, 433, 3225, 3228 }, // 23 IIC_VecComplex
  { 1, 433, 436, 3228, 3231 }, // 24 IIC_LdStStore
  { 0, 0, 0, 0, 0 }, // 25 IIC_IntRotateDI
  { 1, 429, 431, 3231, 3234 }, // 26 IIC_FPDivS
  { 1, 429, 431, 3234, 3238 }, // 27 IIC_FPFused
  { 1, 429, 431, 3238, 3241 }, // 28 IIC_FPSqrtD
  { 1, 429, 431, 3241, 3244 }, // 29 IIC_FPSqrtS
  { 0, 0, 0, 0, 0 }, // 30 IIC_LdStICBI
  { 1, 436, 439, 3244, 3248 }, // 31 IIC_IntISEL
  { 0, 0, 0, 0, 0 }, // 32 IIC_SprISYNC
  { 1, 439, 444, 3248, 3251 }, // 33 IIC_LdStLWARX
  { 1, 444, 448, 3251, 3255 }, // 34 IIC_LdStLoadUpd
  { 1, 448, 455, 3255, 3259 }, // 35 IIC_LdStLoadUpdX
  { 1, 439, 444, 3259, 3262 }, // 36 IIC_LdStLDARX
  { 1, 444, 448, 3262, 3266 }, // 37 IIC_LdStLDU
  { 1, 448, 455, 3266, 3270 }, // 38 IIC_LdStLDUX
  { 1, 421, 423, 3270, 3273 }, // 39 IIC_LdStLFD
  { 1, 444, 448, 3273, 3277 }, // 40 IIC_LdStLFDU
  { 1, 444, 448, 3277, 3281 }, // 41 IIC_LdStLFDUX
  { 1, 455, 459, 3281, 3284 }, // 42 IIC_LdStLHA
  { 1, 459, 464, 3284, 3288 }, // 43 IIC_LdStLHAU
  { 1, 464, 472, 3288, 3292 }, // 44 IIC_LdStLHAUX
  { 1, 421, 423, 3292, 3295 }, // 45 IIC_LdStLMW
  { 1, 455, 459, 3295, 3298 }, // 46 IIC_LdStLWA
  { 0, 0, 0, 0, 0 }, // 47 IIC_IntMulHD
  { 1, 415, 417, 3298, 3301 }, // 48 IIC_BrMCR
  { 1, 472, 478, 3301, 3303 }, // 49 IIC_BrMCRX
  { 1, 419, 421, 3303, 3305 }, // 50 IIC_SprMFCR
  { 0, 0, 0, 0, 0 }, // 51 IIC_SprMFSPR
  { 0, 0, 0, 0, 0 }, // 52 IIC_IntMFFS
  { 0, 0, 0, 0, 0 }, // 53 IIC_SprMFMSR
  { 1, 419, 421, 3305, 3307 }, // 54 IIC_SprMFCRF
  { 0, 0, 0, 0, 0 }, // 55 IIC_SprMFPMR
  { 0, 0, 0, 0, 0 }, // 56 IIC_SprMFSR
  { 0, 0, 0, 0, 0 }, // 57 IIC_SprMFTB
  { 0, 0, 0, 0, 0 }, // 58 IIC_SprMSGSYNC
  { 1, 478, 480, 3307, 3309 }, // 59 IIC_SprMTSPR
  { 0, 0, 0, 0, 0 }, // 60 IIC_IntMTFSB0
  { 0, 0, 0, 0, 0 }, // 61 IIC_SprMTMSR
  { 0, 0, 0, 0, 0 }, // 62 IIC_SprMTMSRD
  { 0, 0, 0, 0, 0 }, // 63 IIC_SprMTPMR
  { 0, 0, 0, 0, 0 }, // 64 IIC_SprMTSR
  { 1, 413, 415, 3309, 3312 }, // 65 IIC_IntMulHW
  { 1, 413, 415, 3312, 3315 }, // 66 IIC_IntMulHWU
  { 1, 413, 415, 3315, 3318 }, // 67 IIC_IntMulLI
  { 1, 480, 482, 3318, 3321 }, // 68 IIC_VecPerm
  { 1, 482, 485, 3321, 3324 }, // 69 IIC_LdStSTFD
  { 1, 485, 490, 3324, 3328 }, // 70 IIC_LdStSTFDU
  { 0, 0, 0, 0, 0 }, // 71 IIC_SprRFI
  { 0, 0, 0, 0, 0 }, // 72 IIC_IntRFID
  { 1, 413, 415, 3328, 3331 }, // 73 IIC_IntRotateD
  { 1, 413, 415, 3331, 3334 }, // 74 IIC_IntRotate
  { 0, 0, 0, 0, 0 }, // 75 IIC_SprSLBIA
  { 0, 0, 0, 0, 0 }, // 76 IIC_SprSLBIE
  { 0, 0, 0, 0, 0 }, // 77 IIC_SprSLBIEG
  { 0, 0, 0, 0, 0 }, // 78 IIC_SprSLBMFEE
  { 0, 0, 0, 0, 0 }, // 79 IIC_SprSLBMFEV
  { 0, 0, 0, 0, 0 }, // 80 IIC_SprSLBMTE
  { 0, 0, 0, 0, 0 }, // 81 IIC_SprSLBSYNC
  { 1, 413, 415, 3334, 3337 }, // 82 IIC_IntShift
  { 1, 439, 444, 3337, 3340 }, // 83 IIC_LdStSTWCX
  { 0, 0, 0, 0, 0 }, // 84 IIC_LdStStoreUpd
  { 1, 433, 436, 3340, 3343 }, // 85 IIC_LdStSTD
  { 1, 439, 444, 3343, 3346 }, // 86 IIC_LdStSTDCX
  { 1, 459, 464, 3346, 3350 }, // 87 IIC_LdStSTDU
  { 1, 490, 497, 3350, 3354 }, // 88 IIC_LdStSTDUX
  { 0, 0, 0, 0, 0 }, // 89 IIC_SprSTOP
  { 1, 413, 415, 3354, 3356 }, // 90 IIC_IntTrapD
  { 0, 0, 0, 0, 0 }, // 91 IIC_SprTLBIA
  { 0, 0, 0, 0, 0 }, // 92 IIC_SprTLBIE
  { 0, 0, 0, 0, 0 }, // 93 IIC_SprTLBIEL
  { 0, 0, 0, 0, 0 }, // 94 IIC_SprTLBSYNC
  { 1, 413, 415, 3356, 3358 }, // 95 IIC_IntTrapW
  { 1, 417, 419, 3358, 3361 }, // 96 IIC_VecFPCompare
  { 1, 431, 433, 3361, 3364 }, // 97 VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz
  { 1, 417, 419, 3364, 3367 }, // 98 VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP
  { 1, 480, 482, 3367, 3370 }, // 99 XXSEL
  { 1, 478, 480, 3370, 3372 }, // 100 TABORTDC_TABORTDCI_TABORTWC_TABORTWCI
  { 0, 0, 0, 0, 0 }, // 101 MTFSB0_MTFSB1
  { 0, 0, 0, 0, 0 }, // 102 MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI
  { 1, 413, 415, 3372, 3375 }, // 103 CMPRB_CMPRB8_CMPEQB
  { 1, 413, 415, 3375, 3377 }, // 104 TD_TDI
  { 1, 413, 415, 3377, 3379 }, // 105 TW_TWI
  { 1, 429, 431, 3379, 3382 }, // 106 FCMPUD_FCMPUS_FTDIV_FTSQRT
  { 1, 417, 419, 3382, 3385 }, // 107 XSTSTDCDP_XSTSTDCSP
  { 1, 417, 419, 3385, 3388 }, // 108 XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN
  { 1, 429, 431, 3388, 3391 }, // 109 XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP
  { 1, 413, 415, 3391, 3394 }, // 110 CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD
  { 1, 413, 415, 3394, 3397 }, // 111 SLD_SRD_SRAD
  { 0, 0, 0, 0, 0 }, // 112 SRADI_EXTSWSLI_SRADI_32_RLDIC
  { 1, 431, 433, 3397, 3400 }, // 113 MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ
  { 1, 413, 415, 3400, 3403 }, // 114 CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI
  { 1, 413, 415, 3403, 3406 }, // 115 SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA
  { 1, 411, 413, 3406, 3409 }, // 116 ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP
  { 0, 0, 0, 0, 0 }, // 117 ADDIStocHA_ADDItocL_COPY
  { 1, 415, 417, 3409, 3412 }, // 118 MCRF
  { 1, 472, 478, 3412, 3414 }, // 119 MCRXRX
  { 1, 417, 419, 3414, 3417 }, // 120 XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP
  { 1, 415, 417, 3417, 3420 }, // 121 RFEBB
  { 1, 478, 480, 3420, 3422 }, // 122 TBEGIN_TRECHKPT
  { 1, 421, 423, 3422, 3425 }, // 123 WAIT
  { 1, 413, 415, 3425, 3428 }, // 124 RLDCL_RLDCR
  { 1, 413, 415, 3428, 3431 }, // 125 RLWIMI_RLWIMI8
  { 0, 0, 0, 0, 0 }, // 126 RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI
  { 1, 419, 421, 3431, 3433 }, // 127 MFOCRF_MFOCRF8
  { 1, 472, 478, 3433, 3435 }, // 128 MTOCRF_MTOCRF8
  { 1, 419, 421, 3435, 3438 }, // 129 CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR
  { 1, 413, 415, 3438, 3441 }, // 130 SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8
  { 1, 429, 431, 3441, 3444 }, // 131 FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR
  { 1, 413, 415, 3444, 3447 }, // 132 SRAW_SRAWI
  { 1, 436, 439, 3447, 3451 }, // 133 ISEL_ISEL8
  { 1, 417, 419, 3451, 3454 }, // 134 XSIEXPDP
  { 1, 478, 480, 3454, 3456 }, // 135 TRECLAIM_TSR_TABORT
  { 1, 433, 436, 3456, 3459 }, // 136 MFVSCR
  { 1, 421, 423, 3459, 3462 }, // 137 MTVSCR
  { 1, 417, 419, 3462, 3465 }, // 138 VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo
  { 1, 431, 433, 3465, 3468 }, // 139 VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW
  { 1, 417, 419, 3468, 3471 }, // 140 VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP
  { 1, 429, 431, 3471, 3474 }, // 141 XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP
  { 1, 417, 419, 3474, 3477 }, // 142 VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS
  { 1, 431, 433, 3477, 3480 }, // 143 VSUBFP_VMULUWM
  { 0, 0, 0, 0, 0 }, // 144 MADDHD_MADDHDU_MADDLD_MULLD
  { 1, 413, 415, 3480, 3483 }, // 145 MULHD_MULHW_MULLW
  { 1, 413, 415, 3483, 3486 }, // 146 MULHDU_MULHWU
  { 1, 413, 415, 3486, 3489 }, // 147 MULLI_MULLI8
  { 1, 429, 431, 3489, 3492 }, // 148 FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS
  { 1, 429, 431, 3492, 3495 }, // 149 FADD_FSUB
  { 1, 429, 431, 3495, 3499 }, // 150 FMSUB_FMADD_FNMADD_FNMSUB_FMUL
  { 1, 417, 419, 3499, 3502 }, // 151 XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP
  { 1, 429, 431, 3502, 3505 }, // 152 FSELDo_FSELSo
  { 1, 413, 415, 3505, 3508 }, // 153 MULHDUo_MULHWUo
  { 1, 413, 415, 3508, 3511 }, // 154 MULHDo_MULHWo_MULLWo
  { 0, 0, 0, 0, 0 }, // 155 MULLDo
  { 1, 429, 431, 3511, 3514 }, // 156 FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo
  { 1, 429, 431, 3514, 3517 }, // 157 FADDo_FSUBo
  { 1, 429, 431, 3517, 3521 }, // 158 FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo
  { 1, 417, 419, 3521, 3524 }, // 159 XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP
  { 1, 421, 423, 3524, 3527 }, // 160 LVSL_LVSR
  { 1, 480, 482, 3527, 3530 }, // 161 VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs
  { 1, 417, 419, 3530, 3533 }, // 162 V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo
  { 1, 431, 433, 3533, 3536 }, // 163 VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM
  { 1, 429, 431, 3536, 3539 }, // 164 XSCMPEXPQP_XSCMPOQP_XSCMPUQP
  { 1, 417, 419, 3539, 3542 }, // 165 BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO
  { 1, 417, 419, 3542, 3545 }, // 166 BCDCTSQo
  { 1, 417, 419, 3545, 3548 }, // 167 XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO
  { 1, 417, 419, 3548, 3551 }, // 168 BCDCFSQo
  { 1, 417, 419, 3551, 3554 }, // 169 XSDIVQP_XSDIVQPO
  { 1, 417, 419, 3554, 3557 }, // 170 XSSQRTQP_XSSQRTQPO
  { 1, 421, 423, 3557, 3560 }, // 171 LXVL_LXVLL
  { 1, 421, 423, 3560, 3563 }, // 172 LVEBX_LVEHX_LVEWX_LVX_LVXL
  { 1, 421, 423, 3563, 3566 }, // 173 LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD
  { 0, 0, 0, 0, 0 }, // 174 DFLOADf64_XFLOADf64_LIWZX
  { 0, 0, 0, 0, 0 }, // 175 DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP
  { 0, 0, 0, 0, 0 }, // 176 CP_COPY_CP_COPY8
  { 0, 0, 0, 0, 0 }, // 177 CP_PASTE_CP_PASTE8
  { 0, 0, 0, 0, 0 }, // 178 ICBI_ICBIEP
  { 1, 421, 423, 3566, 3569 }, // 179 ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI
  { 1, 439, 444, 3569, 3572 }, // 180 LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL
  { 1, 421, 423, 3572, 3575 }, // 181 LD_LDX_LDXTLS_LDXTLS__DARN
  { 1, 439, 444, 3575, 3578 }, // 182 LDARX_LDARXL
  { 0, 0, 0, 0, 0 }, // 183 CP_ABORT
  { 0, 0, 0, 0, 0 }, // 184 ISYNC
  { 0, 0, 0, 0, 0 }, // 185 MSGSYNC
  { 0, 0, 0, 0, 0 }, // 186 TLBSYNC
  { 0, 0, 0, 0, 0 }, // 187 SYNC
  { 1, 421, 423, 3578, 3581 }, // 188 LMW
  { 1, 421, 423, 3581, 3584 }, // 189 LFIWZX_LFDX_LFD
  { 0, 0, 0, 0, 0 }, // 190 SLBIA
  { 0, 0, 0, 0, 0 }, // 191 SLBIE
  { 0, 0, 0, 0, 0 }, // 192 SLBMFEE
  { 0, 0, 0, 0, 0 }, // 193 SLBMFEV
  { 0, 0, 0, 0, 0 }, // 194 SLBMTE
  { 0, 0, 0, 0, 0 }, // 195 TLBIEL
  { 1, 444, 448, 3584, 3588 }, // 196 LHZU_LHZU8_LWZU_LWZU8
  { 1, 448, 455, 3588, 3592 }, // 197 LHZUX_LHZUX8_LWZUX_LWZUX8
  { 1, 478, 480, 3592, 3594 }, // 198 TEND
  { 1, 439, 444, 3594, 3597 }, // 199 STBCX_STHCX_STWCX
  { 1, 439, 444, 3597, 3600 }, // 200 STDCX
  { 1, 421, 423, 3600, 3603 }, // 201 LDMX
  { 1, 455, 459, 3603, 3606 }, // 202 LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32
  { 0, 0, 0, 0, 0 }, // 203 CP_PASTE8o_CP_PASTEo
  { 1, 455, 459, 3606, 3609 }, // 204 LWA_LWA_32
  { 1, 478, 480, 3609, 3611 }, // 205 TCHECK
  { 1, 421, 423, 3611, 3614 }, // 206 LFIWAX
  { 1, 421, 423, 3614, 3617 }, // 207 LXSIWAX
  { 0, 0, 0, 0, 0 }, // 208 LIWAX
  { 1, 421, 423, 3617, 3620 }, // 209 LFSX_LFS
  { 1, 421, 423, 3620, 3623 }, // 210 LXSSP_LXSSPX
  { 0, 0, 0, 0, 0 }, // 211 XFLOADf32_DFLOADf32
  { 1, 459, 464, 3623, 3627 }, // 212 LHAU_LHAU8
  { 1, 464, 472, 3627, 3631 }, // 213 LHAUX_LHAUX8_LWAUX
  { 1, 421, 423, 3631, 3634 }, // 214 LXVH8X_LXVDSX_LXVW4X
  { 1, 482, 485, 3634, 3637 }, // 215 STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX
  { 1, 433, 436, 3637, 3640 }, // 216 STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32
  { 0, 0, 0, 0, 0 }, // 217 DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX
  { 1, 433, 436, 3640, 3643 }, // 218 STD_STDX_STDXTLS_STDXTLS_
  { 1, 421, 423, 3643, 3646 }, // 219 STBCIX_STDCIX_STHCIX_STWCIX_STSWI
  { 0, 0, 0, 0, 0 }, // 220 SLBIEG
  { 1, 421, 423, 3646, 3649 }, // 221 STMW
  { 0, 0, 0, 0, 0 }, // 222 TLBIE
  { 1, 433, 436, 3649, 3652 }, // 223 STVEBX_STVEHX_STVEWX_STVX_STVXL
  { 1, 482, 485, 3652, 3655 }, // 224 STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX
  { 1, 421, 423, 3655, 3658 }, // 225 STXVL_STXVLL
  { 1, 478, 480, 3658, 3660 }, // 226 MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8
  { 1, 413, 415, 3660, 3663 }, // 227 MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv
  { 0, 0, 0, 0, 0 }, // 228 MFPMR
  { 0, 0, 0, 0, 0 }, // 229 MTPMR
  { 0, 0, 0, 0, 0 }, // 230 MFTB_MFTB8
  { 0, 0, 0, 0, 0 }, // 231 MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8
  { 0, 0, 0, 0, 0 }, // 232 MFMSR
  { 0, 0, 0, 0, 0 }, // 233 MTMSR
  { 0, 0, 0, 0, 0 }, // 234 MTMSRD
  { 1, 478, 480, 3663, 3665 }, // 235 MTSPR_MTSPR8
  { 1, 426, 429, 3665, 3668 }, // 236 DIVW_DIVWU_MODSW
  { 1, 426, 429, 3668, 3671 }, // 237 DIVWE_DIVWEU_MODSD_MODUD_MODUW
  { 1, 423, 426, 3671, 3674 }, // 238 DIVD_DIVDU
  { 1, 423, 426, 3674, 3677 }, // 239 DIVDE_DIVDEU
  { 1, 426, 429, 3677, 3680 }, // 240 DIVWUo_DIVWo
  { 1, 423, 426, 3680, 3683 }, // 241 DIVDo_DIVDUo
  { 1, 426, 429, 3683, 3686 }, // 242 DIVWEo_DIVWEUo
  { 1, 423, 426, 3686, 3689 }, // 243 DIVDEo_DIVDEUo
  { 1, 472, 478, 3689, 3691 }, // 244 MTCRF_MTCRF8
  { 1, 413, 415, 3691, 3694 }, // 245 ADDC8o_ADDCo_SUBFC8o_SUBFCo
  { 1, 429, 431, 3694, 3697 }, // 246 FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo
  { 1, 415, 417, 3697, 3700 }, // 247 MCRFS
  { 0, 0, 0, 0, 0 }, // 248 MTFSF_MTFSFo_MTFSFI_MTFSFIo
  { 0, 0, 0, 0, 0 }, // 249 MTFSFb
  { 1, 413, 415, 3700, 3703 }, // 250 RLDCLo_RLDCRo
  { 0, 0, 0, 0, 0 }, // 251 RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo
  { 1, 413, 415, 3703, 3706 }, // 252 RLWIMI8o_RLWIMIo
  { 1, 413, 415, 3706, 3709 }, // 253 RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo
  { 1, 413, 415, 3709, 3712 }, // 254 SRAWIo_SRAWo
  { 0, 0, 0, 0, 0 }, // 255 MFFS_MFFSCE_MFFSL_MFFSo
  { 1, 419, 421, 3712, 3714 }, // 256 MFCR_MFCR8
  { 0, 0, 0, 0, 0 }, // 257 EXTSWSLIo_SRADIo_RLDICo
  { 1, 413, 415, 3714, 3717 }, // 258 SRADo_SLDo_SRDo
  { 1, 429, 431, 3717, 3720 }, // 259 FDIV
  { 1, 429, 431, 3720, 3723 }, // 260 FDIVo
  { 1, 429, 431, 3723, 3726 }, // 261 XSSQRTDP
  { 1, 429, 431, 3726, 3729 }, // 262 FSQRT
  { 1, 429, 431, 3729, 3732 }, // 263 XVSQRTDP
  { 1, 429, 431, 3732, 3735 }, // 264 XVSQRTSP
  { 1, 429, 431, 3735, 3738 }, // 265 FSQRTo
  { 1, 429, 431, 3738, 3741 }, // 266 XSSQRTSP
  { 1, 429, 431, 3741, 3744 }, // 267 FSQRTS
  { 1, 429, 431, 3744, 3747 }, // 268 FSQRTSo
  { 1, 429, 431, 3747, 3750 }, // 269 XSDIVDP
  { 1, 429, 431, 3750, 3753 }, // 270 FDIVS
  { 1, 429, 431, 3753, 3756 }, // 271 FDIVSo
  { 1, 429, 431, 3756, 3759 }, // 272 XSDIVSP
  { 1, 429, 431, 3759, 3762 }, // 273 XVDIVSP
  { 1, 429, 431, 3762, 3765 }, // 274 XVDIVDP
  { 1, 444, 448, 3765, 3769 }, // 275 LFSU
  { 1, 444, 448, 3769, 3773 }, // 276 LFSUX
  { 1, 485, 490, 3773, 3777 }, // 277 STFDU_STFDUX_STFSU_STFSUX
  { 0, 0, 0, 0, 0 }, // 278 STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8
  { 1, 459, 464, 3777, 3781 }, // 279 STDU
  { 1, 490, 497, 3781, 3785 }, // 280 STDUX
  { 1, 444, 448, 3785, 3789 }, // 281 LBZU_LBZU8
  { 1, 448, 455, 3789, 3793 }, // 282 LBZUX_LBZUX8
  { 1, 444, 448, 3793, 3797 }, // 283 LDU
  { 1, 448, 455, 3797, 3801 }, // 284 LDUX
  { 1, 444, 448, 3801, 3805 }, // 285 LFDU
  { 1, 444, 448, 3805, 3809 }, // 286 LFDUX
  { 1, 417, 419, 3809, 3812 }, // 287 VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX
  { 1, 415, 417, 3812, 3815 }, // 288 BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP
  { 1, 415, 417, 3815, 3818 }, // 289 ADDPCIS
  { 1, 421, 423, 3818, 3821 }, // 290 LDAT_LWAT
  { 1, 433, 436, 3821, 3824 }, // 291 STDAT_STWAT
  { 1, 411, 413, 3824, 3827 }, // 292 BRINC
  { 1, 431, 433, 3827, 3830 }, // 293 EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW
  { 1, 431, 433, 3830, 3833 }, // 294 EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW
  { 1, 421, 423, 3833, 3836 }, // 295 EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX
  { 1, 433, 436, 3836, 3839 }, // 296 EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX
  { 1, 415, 417, 3839, 3842 }, // 297 HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC
  { 0, 0, 0, 0, 0 }, // 298 RFI
  { 0, 0, 0, 0, 0 }, // 299 RFID
  { 1, 421, 423, 3842, 3845 }, // 300 DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI
  { 1, 433, 436, 3845, 3848 }, // 301 ICBLC_STBEPX_STHEPX_STWEPX
  { 1, 421, 423, 3848, 3851 }, // 302 LFDEPX
  { 1, 482, 485, 3851, 3854 }, // 303 STFDEPX
  { 0, 0, 0, 0, 0 }, // 304 MFSR_MFSRIN
  { 0, 0, 0, 0, 0 }, // 305 MTSR_MTSRIN
  { 0, 0, 0, 0, 0 }, // 306 MFDCR
  { 1, 478, 480, 3854, 3856 }, // 307 MTDCR
  { 1, 411, 413, 3856, 3859 }, // 308 NOP_GT_PWR6_NOP_GT_PWR7
  { 0, 0, 0, 0, 0 }, // 309 TLBIA
  { 0, 0, 0, 0, 0 }, // 310 WRTEE_WRTEEI
  { 0, 0, 0, 0, 0 }, // 311 MSYNC
  { 0, 0, 0, 0, 0 }, // 312 SLBSYNC
  { 0, 0, 0, 0, 0 }, // 313 STOP
  { 0, 0, 0, 0, 0 }, // 314 DCBA_DCBI
  { 0, uint16_t(~0U), uint16_t(~0U), uint16_t(~0U), uint16_t(~0U) }// end marker
};

// ===============================================================
// Data tables for the new per-operand machine model.

// {ProcResourceIdx, Cycles}
extern const llvm::MCWriteProcResEntry PPCWriteProcResTable[] = {
  { 0,  0}, // Invalid
  { 1,  2}, // #1
  { 2,  1}, // #2
  { 3,  1}, // #3
  { 7,  3}, // #4
  {13,  2}, // #5
  {14,  1}, // #6
  {15,  1}, // #7
  { 1,  1}, // #8
  { 7,  3}, // #9
  {13,  1}, // #10
  { 1,  1}, // #11
  { 7,  2}, // #12
  {13,  1}, // #13
  { 7,  3}, // #14
  { 9,  2}, // #15
  {10,  1}, // #16
  {11,  1}, // #17
  {13,  2}, // #18
  {14,  1}, // #19
  {15,  1}, // #20
  { 7,  3}, // #21
  { 9,  1}, // #22
  {13,  1}, // #23
  { 1,  1}, // #24
  { 7,  5}, // #25
  { 9,  1}, // #26
  {13,  2}, // #27
  { 7,  2}, // #28
  { 9,  1}, // #29
  {13,  1}, // #30
  { 7,  3}, // #31
  {13,  2}, // #32
  {14,  1}, // #33
  {15,  1}, // #34
  {17,  1}, // #35
  { 6,  1}, // #36
  { 7,  3}, // #37
  {13,  2}, // #38
  {14,  1}, // #39
  {15,  1}, // #40
  { 6, 11}, // #41
  { 7,  3}, // #42
  {13,  2}, // #43
  {14,  1}, // #44
  {15,  1}, // #45
  { 6, 12}, // #46
  { 7,  3}, // #47
  {13,  2}, // #48
  {14,  1}, // #49
  {15,  1}, // #50
  { 6, 25}, // #51
  { 7,  3}, // #52
  {13,  2}, // #53
  {14,  1}, // #54
  {15,  1}, // #55
  { 6, 44}, // #56
  { 7,  3}, // #57
  {13,  2}, // #58
  {14,  1}, // #59
  {15,  1}, // #60
  { 6, 62}, // #61
  { 7,  3}, // #62
  {13,  2}, // #63
  {14,  1}, // #64
  {15,  1}, // #65
  { 7,  2}, // #66
  {12,  1}, // #67
  {16,  1}, // #68
  { 7,  3}, // #69
  {12,  1}, // #70
  {16,  1}, // #71
  { 7,  4}, // #72
  {12,  2}, // #73
  {16,  2}, // #74
  { 1,  1}, // #75
  { 7,  4}, // #76
  {12,  1}, // #77
  {13,  1}, // #78
  {16,  1}, // #79
  { 1,  1}, // #80
  { 7,  5}, // #81
  {12,  1}, // #82
  {13,  2}, // #83
  {16,  1}, // #84
  { 1,  1}, // #85
  { 7,  6}, // #86
  {12,  1}, // #87
  {13,  1}, // #88
  {16,  1}, // #89
  { 1,  2}, // #90
  { 7,  6}, // #91
  {12,  1}, // #92
  {13,  2}, // #93
  {16,  1}, // #94
  { 7,  5}, // #95
  {12,  1}, // #96
  {13,  2}, // #97
  {14,  1}, // #98
  {15,  1}, // #99
  {16,  1}, // #100
  {17,  1}, // #101
  { 7,  3}, // #102
  {12,  1}, // #103
  {13,  1}, // #104
  {16,  1}, // #105
  { 7,  3}, // #106
  {12,  1}, // #107
  {13,  2}, // #108
  {14,  1}, // #109
  {15,  1}, // #110
  {16,  1}, // #111
  { 7,  3}, // #112
  { 8,  1}, // #113
  {13,  2}, // #114
  {14,  1}, // #115
  {15,  1}, // #116
  { 7,  3}, // #117
  { 8,  8}, // #118
  {13,  2}, // #119
  {14,  1}, // #120
  {15,  1}, // #121
  { 1,  1}, // #122
  { 7,  5}, // #123
  { 8,  8}, // #124
  {13,  3}, // #125
  {14,  1}, // #126
  {15,  1}, // #127
  { 1,  2}, // #128
  { 7,  6}, // #129
  {13,  2}, // #130
  { 1,  2}, // #131
  { 7,  4}, // #132
  {13,  2}, // #133
  { 1,  2}, // #134
  { 7,  5}, // #135
  {13,  2}, // #136
  { 1,  3}, // #137
  { 7,  9}, // #138
  {13,  3}, // #139
  { 7,  3}, // #140
  { 9,  8}, // #141
  {13,  1}, // #142
  { 1,  1}, // #143
  { 7,  5}, // #144
  { 9,  8}, // #145
  {13,  2}, // #146
  { 7,  2}, // #147
  { 9, 10}, // #148
  {13,  1}, // #149
  { 7,  3}, // #150
  { 9, 10}, // #151
  {13,  1}, // #152
  { 7,  3}, // #153
  { 9, 20}, // #154
  {13,  2}, // #155
  {14,  1}, // #156
  {15,  1}, // #157
  { 1,  1}, // #158
  { 7,  5}, // #159
  { 9, 10}, // #160
  {13,  2}, // #161
  { 7,  2}, // #162
  { 9,  5}, // #163
  {13,  1}, // #164
  { 7,  3}, // #165
  { 9,  5}, // #166
  {13,  1}, // #167
  { 1,  1}, // #168
  { 7,  5}, // #169
  { 9,  5}, // #170
  {13,  2}, // #171
  { 7,  2}, // #172
  { 9,  8}, // #173
  {13,  1}, // #174
  { 7,  3}, // #175
  { 9, 16}, // #176
  {10,  8}, // #177
  {11,  8}, // #178
  {13,  2}, // #179
  {14,  1}, // #180
  {15,  1}, // #181
  { 1,  2}, // #182
  { 7,  8}, // #183
  {12,  1}, // #184
  {13,  2}, // #185
  {16,  1}, // #186
  { 1,  1}, // #187
  { 7,  5}, // #188
  {12,  1}, // #189
  {13,  1}, // #190
  {16,  1}, // #191
  { 5,  1}, // #192
  { 7,  3}, // #193
  {13,  2}, // #194
  {14,  1}, // #195
  {15,  1}, // #196
  { 4,  1}, // #197
  { 7,  2}, // #198
  { 1,  1}, // #199
  { 4,  1}, // #200
  { 7,  4}, // #201
  {13,  1}, // #202
  { 7, 12}, // #203
  {12,  5}, // #204
  {13,  2}, // #205
  {16,  5}, // #206
  { 7,  7}, // #207
  {12,  3}, // #208
  {13,  1}, // #209
  {16,  3} // #210
}; // PPCWriteProcResTable

// {Cycles, WriteResourceID}
extern const llvm::MCWriteLatencyEntry PPCWriteLatencyTable[] = {
  { 0,  0}, // Invalid
  { 2,  0}, // #1 P9_ALUE_2C
  { 2,  0}, // #2 P9_ALUO_2C_P9_ALU_2C_P9_BR_2C
  { 1,  0}, // #3 IP_EXECE_1C_IP_EXEC_1C_DISP_1C
  { 1,  0}, // #4 IP_EXECO_1C_DISP_1C
  { 1,  0}, // #5 DISP_1C
  { 1,  0}, // #6 DISP_1C
  { 1,  0}, // #7 DISP_1C
  { 3,  0}, // #8 P9_ALU_3C
  { 1,  0}, // #9 IP_EXEC_1C
  { 1,  0}, // #10 DISP_1C
  { 1,  0}, // #11 DISP_1C
  { 1,  0}, // #12 DISP_1C
  { 3,  0}, // #13 P9_ALUE_3C
  { 3,  0}, // #14 P9_ALUO_3C_P9_PM_3C
  { 1,  0}, // #15 IP_EXECE_1C_IP_EXECO_1C
  { 1,  0}, // #16 IP_EXECO_1C_IP_EXECE_1C
  { 1,  0}, // #17 DISP_1C
  { 1,  0}, // #18 DISP_1C
  { 1,  0}, // #19 DISP_1C
  { 7,  0}, // #20 P9_DPE_7C
  { 7,  0}, // #21 P9_DPO_7C_P9_DP_7C_P9_ALU_2C
  { 1,  0}, // #22 IP_EXECE_1C_IP_EXEC_1C
  { 1,  0}, // #23 IP_EXECO_1C_DISP_1C
  { 1,  0}, // #24 DISP_1C
  { 1,  0}, // #25 DISP_1C
  { 1,  0}, // #26 DISP_1C
  { 5,  0}, // #27 P9_DP_5C_P9_LS_5C
  { 1,  0}, // #28 IP_EXEC_1C_IP_AGEN_1C
  { 1,  0}, // #29 DISP_1C
  { 1,  0}, // #30 DISP_1C
  { 1,  0}, // #31 DISP_1C
  { 7,  0}, // #32 P9_DP_7C
  { 3,  0}, // #33 P9_ALU_3C
  { 1,  0}, // #34 IP_EXEC_1C_P9_LS_1C
  { 1,  0}, // #35 IP_EXEC_1C_IP_EXECE_1C
  { 1,  0}, // #36 DISP_1C_IP_AGEN_1C_IP_EXECO_1C
  { 1,  0}, // #37 DISP_1C_IP_AGEN_1C
  { 1,  0}, // #38 DISP_1C
  { 1,  0}, // #39 DISP_1C
  { 1,  0}, // #40 DISP_1C
  { 7,  0}, // #41 P9_ALU_2C
  { 1,  0}, // #42 IP_EXEC_1C_IP_AGEN_1C
  { 1,  0}, // #43 IP_EXEC_1C
  { 1,  0}, // #44 DISP_1C
  { 1,  0}, // #45 DISP_1C
  { 1,  0}, // #46 DISP_1C
  { 1,  0}, // #47 DISP_1C
  { 1,  0}, // #48 DISP_1C
  {10,  0}, // #49 P9_ALU_3C
  { 1,  0}, // #50 IP_EXEC_1C
  { 1,  0}, // #51 IP_EXEC_1C
  { 1,  0}, // #52 DISP_1C
  { 1,  0}, // #53 DISP_1C
  { 1,  0}, // #54 DISP_1C
  { 1,  0}, // #55 DISP_1C
  { 1,  0}, // #56 DISP_1C
  {12,  0}, // #57 P9_DFU_12C_P9_DIV_12C
  { 1,  0}, // #58 IP_EXECE_1C
  { 1,  0}, // #59 IP_EXECO_1C
  { 1,  0}, // #60 DISP_1C
  { 1,  0}, // #61 DISP_1C
  { 1,  0}, // #62 DISP_1C
  {23,  0}, // #63 P9_DFU_23C
  { 1,  0}, // #64 IP_EXECE_1C
  { 1,  0}, // #65 IP_EXECO_1C
  { 1,  0}, // #66 DISP_1C
  { 1,  0}, // #67 DISP_1C
  { 1,  0}, // #68 DISP_1C
  {24,  0}, // #69 P9_DFU_24C_P9_DIV_24C_8
  { 1,  0}, // #70 IP_EXECE_1C_IP_EXECO_1C
  { 1,  0}, // #71 IP_EXECO_1C_IP_EXECE_1C
  { 1,  0}, // #72 DISP_1C
  { 1,  0}, // #73 DISP_1C
  { 1,  0}, // #74 DISP_1C
  {37,  0}, // #75 P9_DFU_37C
  { 1,  0}, // #76 IP_EXECE_1C
  { 1,  0}, // #77 IP_EXECO_1C
  { 1,  0}, // #78 DISP_1C
  { 1,  0}, // #79 DISP_1C
  { 1,  0}, // #80 DISP_1C
  {58,  0}, // #81 P9_DFU_58C
  { 1,  0}, // #82 IP_EXECE_1C
  { 1,  0}, // #83 IP_EXECO_1C
  { 1,  0}, // #84 DISP_1C
  { 1,  0}, // #85 DISP_1C
  { 1,  0}, // #86 DISP_1C
  {76,  0}, // #87 P9_DFU_76C
  { 1,  0}, // #88 IP_EXECE_1C
  { 1,  0}, // #89 IP_EXECO_1C
  { 1,  0}, // #90 DISP_1C
  { 1,  0}, // #91 DISP_1C
  { 1,  0}, // #92 DISP_1C
  { 6,  0}, // #93 P9_LS_6C
  { 1,  0}, // #94 IP_AGEN_1C
  { 1,  0}, // #95 DISP_1C
  { 1,  0}, // #96 DISP_1C
  { 4,  0}, // #97 P9_LS_4C
  { 1,  0}, // #98 IP_AGEN_1C
  { 1,  0}, // #99 DISP_1C
  { 1,  0}, // #100 DISP_1C
  { 4,  0}, // #101 P9_LS_4C
  { 1,  0}, // #102 IP_AGEN_1C
  { 1,  0}, // #103 DISP_1C
  { 1,  0}, // #104 DISP_1C
  { 1,  0}, // #105 DISP_1C
  { 4,  0}, // #106 P9_LS_4C
  { 4,  0}, // #107 P9_LS_4C_P9_ALU_2C
  { 1,  0}, // #108 IP_AGEN_1C_IP_EXEC_1C
  { 1,  0}, // #109 IP_AGEN_1C_IP_EXEC_1C
  { 1,  0}, // #110 DISP_1C
  { 1,  0}, // #111 DISP_1C
  { 1,  0}, // #112 DISP_1C
  { 1,  0}, // #113 DISP_1C
  { 4,  0}, // #114 P9_LS_4C
  { 2,  0}, // #115 P9_ALU_2C
  { 1,  0}, // #116 IP_EXEC_1C_IP_AGEN_1C
  { 1,  0}, // #117 IP_AGEN_1C_IP_EXEC_1C
  { 1,  0}, // #118 DISP_1C
  { 1,  0}, // #119 DISP_1C
  { 1,  0}, // #120 DISP_1C
  { 1,  0}, // #121 DISP_1C
  { 3,  0}, // #122 P9_ALU_2C
  { 1,  0}, // #123 IP_EXEC_1C
  { 1,  0}, // #124 IP_EXEC_1C
  { 1,  0}, // #125 IP_AGEN_1C
  { 1,  0}, // #126 DISP_1C
  { 1,  0}, // #127 DISP_1C
  { 1,  0}, // #128 DISP_1C
  { 1,  0}, // #129 DISP_1C
  { 1,  0}, // #130 DISP_1C
  { 8,  0}, // #131 P9_LS_4C_P9_ALU_3C
  { 1,  0}, // #132 IP_AGEN_1C
  { 1,  0}, // #133 IP_AGEN_1C_IP_EXEC_1C
  { 1,  0}, // #134 DISP_1C
  { 1,  0}, // #135 DISP_1C
  { 1,  0}, // #136 DISP_1C
  { 1,  0}, // #137 DISP_1C
  { 6,  0}, // #138 P9_ALU_2C_P9_CY_6C
  { 1,  0}, // #139 IP_EXEC_1C_IP_EXECO_1C
  { 1,  0}, // #140 IP_AGEN_1C_IP_EXECE_1C
  { 1,  0}, // #141 DISP_1C
  { 1,  0}, // #142 DISP_1C
  { 1,  0}, // #143 DISP_1C
  { 1,  0}, // #144 DISP_1C
  { 6,  0}, // #145 P9_ALU_2C_P9_ALU_3C
  { 1,  0}, // #146 IP_EXEC_1C
  { 1,  0}, // #147 IP_AGEN_1C_IP_EXEC_1C
  { 1,  0}, // #148 DISP_1C
  { 1,  0}, // #149 DISP_1C
  { 1,  0}, // #150 DISP_1C
  { 1,  0}, // #151 DISP_1C
  { 1,  0}, // #152 DISP_1C
  { 1,  0}, // #153 DISP_1C
  { 7,  0}, // #154 P9_ALU_3C
  { 1,  0}, // #155 IP_AGEN_1C
  { 1,  0}, // #156 IP_EXEC_1C
  { 1,  0}, // #157 DISP_1C
  { 1,  0}, // #158 DISP_1C
  { 1,  0}, // #159 DISP_1C
  { 1,  0}, // #160 DISP_1C
  { 1,  0}, // #161 DISP_1C
  { 1,  0}, // #162 DISP_1C
  { 6,  0}, // #163 P9_ALU_2C
  { 2,  0}, // #164 P9_ALU_2C
  { 1,  0}, // #165 IP_AGEN_1C
  { 1,  0}, // #166 IP_EXEC_1C
  { 1,  0}, // #167 IP_EXEC_1C
  { 1,  0}, // #168 DISP_1C
  { 1,  0}, // #169 DISP_1C
  { 1,  0}, // #170 DISP_1C
  { 1,  0}, // #171 DISP_1C
  { 1,  0}, // #172 DISP_1C
  { 1,  0}, // #173 DISP_1C
  { 8,  0}, // #174 P9_PM_3C
  { 1,  0}, // #175 IP_AGEN_1C
  { 1,  0}, // #176 IP_EXECE_1C
  { 1,  0}, // #177 IP_EXECO_1C
  { 1,  0}, // #178 DISP_1C
  { 1,  0}, // #179 DISP_1C
  { 1,  0}, // #180 DISP_1C
  { 1,  0}, // #181 DISP_1C
  { 1,  0}, // #182 DISP_1C
  { 5,  0}, // #183 P9_DIV_5C
  { 1,  0}, // #184 IP_EXECE_1C
  { 1,  0}, // #185 IP_EXECO_1C
  { 1,  0}, // #186 DISP_1C
  { 1,  0}, // #187 DISP_1C
  { 1,  0}, // #188 DISP_1C
  {16,  0}, // #189 P9_DIV_16C_8
  { 1,  0}, // #190 IP_EXECO_1C
  { 1,  0}, // #191 IP_EXECE_1C
  { 1,  0}, // #192 DISP_1C
  { 1,  0}, // #193 DISP_1C
  { 1,  0}, // #194 DISP_1C
  {40,  0}, // #195 P9_DIV_40C_8
  { 1,  0}, // #196 IP_EXECO_1C
  { 1,  0}, // #197 IP_EXECE_1C
  { 1,  0}, // #198 DISP_1C
  { 1,  0}, // #199 DISP_1C
  { 1,  0}, // #200 DISP_1C
  {18,  0}, // #201 P9_ALU_2C
  { 1,  0}, // #202 IP_EXECE_1C
  { 1,  0}, // #203 IP_EXECO_1C
  { 1,  0}, // #204 IP_EXEC_1C
  { 1,  0}, // #205 DISP_1C
  { 1,  0}, // #206 DISP_1C
  { 1,  0}, // #207 DISP_1C
  { 1,  0}, // #208 DISP_1C
  { 1,  0}, // #209 DISP_1C
  {26,  0}, // #210 P9_ALU_2C
  { 1,  0}, // #211 IP_EXECE_1C
  { 1,  0}, // #212 IP_EXECO_1C
  { 1,  0}, // #213 IP_EXEC_1C
  { 1,  0}, // #214 DISP_1C
  { 1,  0}, // #215 DISP_1C
  { 1,  0}, // #216 DISP_1C
  { 1,  0}, // #217 DISP_1C
  { 1,  0}, // #218 DISP_1C
  {42,  0}, // #219 P9_ALU_2C
  { 1,  0}, // #220 IP_EXECE_1C
  { 1,  0}, // #221 IP_EXECO_1C
  { 1,  0}, // #222 IP_EXEC_1C
  { 1,  0}, // #223 DISP_1C
  { 1,  0}, // #224 DISP_1C
  { 1,  0}, // #225 DISP_1C
  { 1,  0}, // #226 DISP_1C
  { 1,  0}, // #227 DISP_1C
  { 2,  0}, // #228 P9_ALU_2C
  { 2,  0}, // #229 P9_ALU_2C
  { 1,  0}, // #230 IP_EXEC_1C
  { 1,  0}, // #231 IP_EXEC_1C
  { 1,  0}, // #232 DISP_1C
  { 1,  0}, // #233 DISP_1C
  { 1,  0}, // #234 DISP_1C
  { 1,  0}, // #235 DISP_1C
  { 1,  0}, // #236 DISP_1C
  { 1,  0}, // #237 DISP_1C
  { 2,  0}, // #238 P9_ALU_2C
  { 3,  0}, // #239 P9_ALU_3C
  { 1,  0}, // #240 IP_EXEC_1C
  { 1,  0}, // #241 IP_EXEC_1C
  { 1,  0}, // #242 DISP_1C
  { 1,  0}, // #243 DISP_1C
  { 1,  0}, // #244 DISP_1C
  { 1,  0}, // #245 DISP_1C
  { 1,  0}, // #246 DISP_1C
  { 3,  0}, // #247 P9_ALU_3C
  { 3,  0}, // #248 P9_ALU_3C
  { 1,  0}, // #249 IP_EXEC_1C
  { 1,  0}, // #250 IP_EXEC_1C
  { 1,  0}, // #251 DISP_1C
  { 1,  0}, // #252 DISP_1C
  { 1,  0}, // #253 DISP_1C
  { 1,  0}, // #254 DISP_1C
  { 3,  0}, // #255 P9_ALU_3C
  { 3,  0}, // #256 P9_ALU_3C
  { 1,  0}, // #257 IP_EXEC_1C
  { 1,  0}, // #258 IP_EXEC_1C
  { 1,  0}, // #259 DISP_1C
  { 1,  0}, // #260 DISP_1C
  { 1,  0}, // #261 DISP_1C
  { 1,  0}, // #262 DISP_1C
  { 1,  0}, // #263 DISP_1C
  { 1,  0}, // #264 DISP_1C
  { 4,  0}, // #265 P9_ALU_2C
  { 1,  0}, // #266 IP_EXEC_1C
  { 1,  0}, // #267 IP_EXEC_1C
  { 1,  0}, // #268 DISP_1C
  { 1,  0}, // #269 DISP_1C
  { 1,  0}, // #270 DISP_1C
  { 1,  0}, // #271 DISP_1C
  { 1,  0}, // #272 DISP_1C
  { 6,  0}, // #273 P9_ALU_2C
  { 1,  0}, // #274 IP_EXEC_1C
  { 1,  0}, // #275 IP_EXEC_1C
  { 1,  0}, // #276 IP_EXEC_1C
  { 1,  0}, // #277 DISP_1C
  { 1,  0}, // #278 DISP_1C
  { 1,  0}, // #279 DISP_1C
  { 1,  0}, // #280 DISP_1C
  { 1,  0}, // #281 DISP_1C
  { 1,  0}, // #282 DISP_1C
  { 1,  0}, // #283 DISP_1C
  { 1,  0}, // #284 DISP_1C
  { 1,  0}, // #285 DISP_1C
  {33,  0}, // #286 P9_DP_33C_8
  { 1,  0}, // #287 IP_EXEC_1C
  { 1,  0}, // #288 DISP_1C
  { 1,  0}, // #289 DISP_1C
  { 1,  0}, // #290 DISP_1C
  {36,  0}, // #291 P9_ALU_3C_P9_DP_36C_10
  { 1,  0}, // #292 IP_EXEC_1C
  { 1,  0}, // #293 IP_EXEC_1C_DISP_1C
  { 1,  0}, // #294 DISP_1C
  { 1,  0}, // #295 DISP_1C
  { 1,  0}, // #296 DISP_1C
  { 1,  0}, // #297 DISP_1C
  { 1,  0}, // #298 DISP_1C
  {36,  0}, // #299 P9_DPE_36C_10
  {36,  0}, // #300 P9_DPO_36C_10
  { 1,  0}, // #301 IP_EXECE_1C
  { 1,  0}, // #302 IP_EXECO_1C
  { 1,  0}, // #303 DISP_1C
  { 1,  0}, // #304 DISP_1C
  { 1,  0}, // #305 DISP_1C
  {27,  0}, // #306 P9_DPE_27C_10
  {27,  0}, // #307 P9_DPO_27C_10
  { 1,  0}, // #308 IP_EXECE_1C
  { 1,  0}, // #309 IP_EXECO_1C
  { 1,  0}, // #310 DISP_1C
  { 1,  0}, // #311 DISP_1C
  { 1,  0}, // #312 DISP_1C
  {39,  0}, // #313 P9_ALU_3C
  { 1,  0}, // #314 IP_EXEC_1C
  { 1,  0}, // #315 IP_EXEC_1C
  { 1,  0}, // #316 DISP_1C
  { 1,  0}, // #317 DISP_1C
  { 1,  0}, // #318 DISP_1C
  { 1,  0}, // #319 DISP_1C
  { 1,  0}, // #320 DISP_1C
  {22,  0}, // #321 P9_DP_26C_5_P9_DP_22C_5
  { 1,  0}, // #322 IP_EXEC_1C
  { 1,  0}, // #323 DISP_1C
  { 1,  0}, // #324 DISP_1C
  {22,  0}, // #325 P9_DP_26C_5_P9_DP_22C_5
  { 1,  0}, // #326 IP_EXEC_1C
  { 1,  0}, // #327 DISP_1C
  { 1,  0}, // #328 DISP_1C
  { 1,  0}, // #329 DISP_1C
  {25,  0}, // #330 P9_ALU_3C
  { 1,  0}, // #331 IP_EXEC_1C
  { 1,  0}, // #332 IP_EXEC_1C
  { 1,  0}, // #333 DISP_1C
  { 1,  0}, // #334 DISP_1C
  { 1,  0}, // #335 DISP_1C
  { 1,  0}, // #336 DISP_1C
  { 1,  0}, // #337 DISP_1C
  {24,  0}, // #338 P9_DPE_24C_8
  {24,  0}, // #339 P9_DPO_24C_8
  { 1,  0}, // #340 IP_EXECE_1C
  { 1,  0}, // #341 IP_EXECO_1C
  { 1,  0}, // #342 DISP_1C
  { 1,  0}, // #343 DISP_1C
  { 1,  0}, // #344 DISP_1C
  {33,  0}, // #345 P9_DPE_33C_8
  {33,  0}, // #346 P9_DPO_33C_8
  { 1,  0}, // #347 IP_EXECE_1C
  { 1,  0}, // #348 IP_EXECO_1C
  { 1,  0}, // #349 DISP_1C
  { 1,  0}, // #350 DISP_1C
  { 1,  0}, // #351 DISP_1C
  { 7,  0}, // #352 P9_ALU_3C
  { 2,  0}, // #353 P9_ALU_2C
  { 1,  0}, // #354 IP_AGEN_1C
  { 1,  0}, // #355 IP_EXEC_1C
  { 1,  0}, // #356 IP_EXEC_1C
  { 1,  0}, // #357 DISP_1C
  { 1,  0}, // #358 DISP_1C
  { 1,  0}, // #359 DISP_1C
  { 1,  0}, // #360 DISP_1C
  { 1,  0}, // #361 DISP_1C
  { 1,  0}, // #362 DISP_1C
  { 1,  0}, // #363 DISP_1C
  { 1,  0}, // #364 DISP_1C
  { 1,  0}, // #365 P9_LS_1C
  { 2,  0}, // #366 P9_ALU_2C
  { 1,  0}, // #367 IP_AGEN_1C
  { 1,  0}, // #368 IP_EXEC_1C
  { 1,  0}, // #369 IP_EXEC_1C
  { 1,  0}, // #370 DISP_1C
  { 1,  0}, // #371 DISP_1C
  { 1,  0}, // #372 DISP_1C
  { 1,  0}, // #373 DISP_1C
  { 1,  0}, // #374 DISP_1C
  { 4,  0}, // #375 P9_LS_4C
  { 2,  0}, // #376 P9_ALU_2C
  { 1,  0}, // #377 IP_AGEN_1C
  { 1,  0}, // #378 IP_EXEC_1C
  { 1,  0}, // #379 DISP_1C
  { 1,  0}, // #380 DISP_1C
  { 1,  0}, // #381 DISP_1C
  { 1,  0}, // #382 DISP_1C
  { 1,  0}, // #383 DISP_1C
  { 1,  0}, // #384 P9_LS_1C
  { 1,  0}, // #385 P9_LS_1C
  { 4,  0}, // #386 P9_LS_4C
  { 4,  0}, // #387 P9_LS_4C
  { 4,  0}, // #388 P9_LS_4C
  { 1,  0}, // #389 IP_EXEC_1C
  { 1,  0}, // #390 IP_EXEC_1C
  { 1,  0}, // #391 IP_AGEN_1C
  { 1,  0}, // #392 IP_AGEN_1C
  { 1,  0}, // #393 IP_AGEN_1C
  { 1,  0}, // #394 IP_AGEN_1C
  { 1,  0}, // #395 IP_AGEN_1C
  { 1,  0}, // #396 DISP_1C
  { 1,  0}, // #397 DISP_1C
  { 1,  0}, // #398 DISP_1C
  { 1,  0}, // #399 DISP_1C
  { 1,  0}, // #400 DISP_1C
  { 1,  0}, // #401 DISP_1C
  { 1,  0}, // #402 DISP_1C
  { 1,  0}, // #403 DISP_1C
  { 1,  0}, // #404 DISP_1C
  { 1,  0}, // #405 DISP_1C
  { 1,  0}, // #406 DISP_1C
  { 1,  0}, // #407 DISP_1C
  { 1,  0}, // #408 P9_LS_1C
  { 4,  0}, // #409 P9_LS_4C
  { 4,  0}, // #410 P9_LS_4C
  { 1,  0}, // #411 IP_EXEC_1C
  { 1,  0}, // #412 IP_AGEN_1C
  { 1,  0}, // #413 IP_AGEN_1C
  { 1,  0}, // #414 IP_AGEN_1C
  { 1,  0}, // #415 DISP_1C
  { 1,  0}, // #416 DISP_1C
  { 1,  0}, // #417 DISP_1C
  { 1,  0}, // #418 DISP_1C
  { 1,  0}, // #419 DISP_1C
  { 1,  0}, // #420 DISP_1C
  { 1,  0} // #421 DISP_1C
}; // PPCWriteLatencyTable

// {UseIdx, WriteResourceID, Cycles}
extern const llvm::MCReadAdvanceEntry PPCReadAdvanceTable[] = {
  {0,  0,  0}, // Invalid
}; // PPCReadAdvanceTable

// {Name, NumMicroOps, BeginGroup, EndGroup, WriteProcResIdx,#, WriteLatencyIdx,#, ReadAdvanceIdx,#}
static const llvm::MCSchedClassDesc P9ModelSchedClasses[] = {
  {DBGFIELD("InvalidSchedClass")  16383, false, false,  0, 0,  0, 0,  0, 0},
  {DBGFIELD("IIC_LdStSync")       0, false, false,  0, 0,  0, 0,  0, 0}, // #1
  {DBGFIELD("IIC_IntSimple")      0, false, false,  0, 0,  0, 0,  0, 0}, // #2
  {DBGFIELD("IIC_IntGeneral")     0, false, false,  0, 0,  0, 0,  0, 0}, // #3
  {DBGFIELD("IIC_BrB")            0, false, false,  0, 0,  0, 0,  0, 0}, // #4
  {DBGFIELD("IIC_VecFP")          0, false, false,  0, 0,  0, 0,  0, 0}, // #5
  {DBGFIELD("IIC_IntCompare")     0, false, false,  0, 0,  0, 0,  0, 0}, // #6
  {DBGFIELD("IIC_SprABORT")       0, false, false,  0, 0,  0, 0,  0, 0}, // #7
  {DBGFIELD("IIC_LdStCOPY")       0, false, false,  0, 0,  0, 0,  0, 0}, // #8
  {DBGFIELD("IIC_LdStPASTE")      0, false, false,  0, 0,  0, 0,  0, 0}, // #9
  {DBGFIELD("IIC_BrCR")           0, false, false,  0, 0,  0, 0,  0, 0}, // #10
  {DBGFIELD("IIC_LdStLD")         0, false, false,  0, 0,  0, 0,  0, 0}, // #11
  {DBGFIELD("IIC_LdStDCBF")       0, false, false,  0, 0,  0, 0,  0, 0}, // #12
  {DBGFIELD("IIC_LdStLoad")       0, false, false,  0, 0,  0, 0,  0, 0}, // #13
  {DBGFIELD("IIC_IntDivD")        0, false, false,  0, 0,  0, 0,  0, 0}, // #14
  {DBGFIELD("IIC_IntDivW")        0, false, false,  0, 0,  0, 0,  0, 0}, // #15
  {DBGFIELD("IIC_FPDGeneral")     0, false, false,  0, 0,  0, 0,  0, 0}, // #16
  {DBGFIELD("IIC_FPAddSub")       0, false, false,  0, 0,  0, 0,  0, 0}, // #17
  {DBGFIELD("IIC_FPDivD")         0, false, false,  0, 0,  0, 0,  0, 0}, // #18
  {DBGFIELD("IIC_FPSGeneral")     0, false, false,  0, 0,  0, 0,  0, 0}, // #19
  {DBGFIELD("IIC_FPCompare")      0, false, false,  0, 0,  0, 0,  0, 0}, // #20
  {DBGFIELD("IIC_FPGeneral")      0, false, false,  0, 0,  0, 0,  0, 0}, // #21
  {DBGFIELD("IIC_VecGeneral")     0, false, false,  0, 0,  0, 0,  0, 0}, // #22
  {DBGFIELD("IIC_VecComplex")     0, false, false,  0, 0,  0, 0,  0, 0}, // #23
  {DBGFIELD("IIC_LdStStore")      0, false, false,  0, 0,  0, 0,  0, 0}, // #24
  {DBGFIELD("IIC_IntRotateDI")    0, false, false,  0, 0,  0, 0,  0, 0}, // #25
  {DBGFIELD("IIC_FPDivS")         0, false, false,  0, 0,  0, 0,  0, 0}, // #26
  {DBGFIELD("IIC_FPFused")        0, false, false,  0, 0,  0, 0,  0, 0}, // #27
  {DBGFIELD("IIC_FPSqrtD")        0, false, false,  0, 0,  0, 0,  0, 0}, // #28
  {DBGFIELD("IIC_FPSqrtS")        0, false, false,  0, 0,  0, 0,  0, 0}, // #29
  {DBGFIELD("IIC_LdStICBI")       0, false, false,  0, 0,  0, 0,  0, 0}, // #30
  {DBGFIELD("IIC_IntISEL")        0, false, false,  0, 0,  0, 0,  0, 0}, // #31
  {DBGFIELD("IIC_SprISYNC")       0, false, false,  0, 0,  0, 0,  0, 0}, // #32
  {DBGFIELD("IIC_LdStLWARX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #33
  {DBGFIELD("IIC_LdStLoadUpd")    0, false, false,  0, 0,  0, 0,  0, 0}, // #34
  {DBGFIELD("IIC_LdStLoadUpdX")   0, false, false,  0, 0,  0, 0,  0, 0}, // #35
  {DBGFIELD("IIC_LdStLDARX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #36
  {DBGFIELD("IIC_LdStLDU")        0, false, false,  0, 0,  0, 0,  0, 0}, // #37
  {DBGFIELD("IIC_LdStLDUX")       0, false, false,  0, 0,  0, 0,  0, 0}, // #38
  {DBGFIELD("IIC_LdStLFD")        0, false, false,  0, 0,  0, 0,  0, 0}, // #39
  {DBGFIELD("IIC_LdStLFDU")       0, false, false,  0, 0,  0, 0,  0, 0}, // #40
  {DBGFIELD("IIC_LdStLFDUX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #41
  {DBGFIELD("IIC_LdStLHA")        0, false, false,  0, 0,  0, 0,  0, 0}, // #42
  {DBGFIELD("IIC_LdStLHAU")       0, false, false,  0, 0,  0, 0,  0, 0}, // #43
  {DBGFIELD("IIC_LdStLHAUX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #44
  {DBGFIELD("IIC_LdStLMW")        0, false, false,  0, 0,  0, 0,  0, 0}, // #45
  {DBGFIELD("IIC_LdStLWA")        0, false, false,  0, 0,  0, 0,  0, 0}, // #46
  {DBGFIELD("IIC_IntMulHD")       0, false, false,  0, 0,  0, 0,  0, 0}, // #47
  {DBGFIELD("IIC_BrMCR")          0, false, false,  0, 0,  0, 0,  0, 0}, // #48
  {DBGFIELD("IIC_BrMCRX")         0, false, false,  0, 0,  0, 0,  0, 0}, // #49
  {DBGFIELD("IIC_SprMFCR")        0, false, false,  0, 0,  0, 0,  0, 0}, // #50
  {DBGFIELD("IIC_SprMFSPR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #51
  {DBGFIELD("IIC_IntMFFS")        0, false, false,  0, 0,  0, 0,  0, 0}, // #52
  {DBGFIELD("IIC_SprMFMSR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #53
  {DBGFIELD("IIC_SprMFCRF")       0, false, false,  0, 0,  0, 0,  0, 0}, // #54
  {DBGFIELD("IIC_SprMFPMR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #55
  {DBGFIELD("IIC_SprMFSR")        0, false, false,  0, 0,  0, 0,  0, 0}, // #56
  {DBGFIELD("IIC_SprMFTB")        0, false, false,  0, 0,  0, 0,  0, 0}, // #57
  {DBGFIELD("IIC_SprMSGSYNC")     0, false, false,  0, 0,  0, 0,  0, 0}, // #58
  {DBGFIELD("IIC_SprMTSPR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #59
  {DBGFIELD("IIC_IntMTFSB0")      0, false, false,  0, 0,  0, 0,  0, 0}, // #60
  {DBGFIELD("IIC_SprMTMSR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #61
  {DBGFIELD("IIC_SprMTMSRD")      0, false, false,  0, 0,  0, 0,  0, 0}, // #62
  {DBGFIELD("IIC_SprMTPMR")       0, false, false,  0, 0,  0, 0,  0, 0}, // #63
  {DBGFIELD("IIC_SprMTSR")        0, false, false,  0, 0,  0, 0,  0, 0}, // #64
  {DBGFIELD("IIC_IntMulHW")       0, false, false,  0, 0,  0, 0,  0, 0}, // #65
  {DBGFIELD("IIC_IntMulHWU")      0, false, false,  0, 0,  0, 0,  0, 0}, // #66
  {DBGFIELD("IIC_IntMulLI")       0, false, false,  0, 0,  0, 0,  0, 0}, // #67
  {DBGFIELD("IIC_VecPerm")        0, false, false,  0, 0,  0, 0,  0, 0}, // #68
  {DBGFIELD("IIC_LdStSTFD")       0, false, false,  0, 0,  0, 0,  0, 0}, // #69
  {DBGFIELD("IIC_LdStSTFDU")      0, false, false,  0, 0,  0, 0,  0, 0}, // #70
  {DBGFIELD("IIC_SprRFI")         0, false, false,  0, 0,  0, 0,  0, 0}, // #71
  {DBGFIELD("IIC_IntRFID")        0, false, false,  0, 0,  0, 0,  0, 0}, // #72
  {DBGFIELD("IIC_IntRotateD")     0, false, false,  0, 0,  0, 0,  0, 0}, // #73
  {DBGFIELD("IIC_IntRotate")      0, false, false,  0, 0,  0, 0,  0, 0}, // #74
  {DBGFIELD("IIC_SprSLBIA")       0, false, false,  0, 0,  0, 0,  0, 0}, // #75
  {DBGFIELD("IIC_SprSLBIE")       0, false, false,  0, 0,  0, 0,  0, 0}, // #76
  {DBGFIELD("IIC_SprSLBIEG")      0, false, false,  0, 0,  0, 0,  0, 0}, // #77
  {DBGFIELD("IIC_SprSLBMFEE")     0, false, false,  0, 0,  0, 0,  0, 0}, // #78
  {DBGFIELD("IIC_SprSLBMFEV")     0, false, false,  0, 0,  0, 0,  0, 0}, // #79
  {DBGFIELD("IIC_SprSLBMTE")      0, false, false,  0, 0,  0, 0,  0, 0}, // #80
  {DBGFIELD("IIC_SprSLBSYNC")     0, false, false,  0, 0,  0, 0,  0, 0}, // #81
  {DBGFIELD("IIC_IntShift")       0, false, false,  0, 0,  0, 0,  0, 0}, // #82
  {DBGFIELD("IIC_LdStSTWCX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #83
  {DBGFIELD("IIC_LdStStoreUpd")   0, false, false,  0, 0,  0, 0,  0, 0}, // #84
  {DBGFIELD("IIC_LdStSTD")        0, false, false,  0, 0,  0, 0,  0, 0}, // #85
  {DBGFIELD("IIC_LdStSTDCX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #86
  {DBGFIELD("IIC_LdStSTDU")       0, false, false,  0, 0,  0, 0,  0, 0}, // #87
  {DBGFIELD("IIC_LdStSTDUX")      0, false, false,  0, 0,  0, 0,  0, 0}, // #88
  {DBGFIELD("IIC_SprSTOP")        0, false, false,  0, 0,  0, 0,  0, 0}, // #89
  {DBGFIELD("IIC_IntTrapD")       0, false, false,  0, 0,  0, 0,  0, 0}, // #90
  {DBGFIELD("IIC_SprTLBIA")       0, false, false,  0, 0,  0, 0,  0, 0}, // #91
  {DBGFIELD("IIC_SprTLBIE")       0, false, false,  0, 0,  0, 0,  0, 0}, // #92
  {DBGFIELD("IIC_SprTLBIEL")      0, false, false,  0, 0,  0, 0,  0, 0}, // #93
  {DBGFIELD("IIC_SprTLBSYNC")     0, false, false,  0, 0,  0, 0,  0, 0}, // #94
  {DBGFIELD("IIC_IntTrapW")       0, false, false,  0, 0,  0, 0,  0, 0}, // #95
  {DBGFIELD("IIC_VecFPCompare")   0, false, false,  0, 0,  0, 0,  0, 0}, // #96
  {DBGFIELD("VADDUBM_VADDUDM_VADDUHM_VADDUWM_VEXTSB2D_VEXTSB2Ds_VEXTSB2W_VEXTSB2Ws_VEXTSH2D_VEXTSH2Ds_VEXTSH2W_VEXTSH2Ws_VEXTSW2D_VEXTSW2Ds_VSLD_VSRD_VSUBUBM_VSUBUDM_VSUBUHM_VSUBUWM_VPOPCNTB_VPOPCNTH_VSRAD_MTVSRDD_VEQV_VNAND_VNEGD_VNEGW_VORC_XXLAND_XXLANDC_XXLEQV_XXLNAND_XXLNOR_XXLOR_XXLORf_XXLORC_XXLXOR_XXLXORdpz_XXLXORspz_XXLXORz") 2, false, false,  1, 7,  1, 7,  0, 0}, // #97
  {DBGFIELD("VAND_VANDC_V_SET0_V_SET0B_V_SET0H_VSLB_VSLH_VSLW_VSRB_VSRH_VSRW_VRLB_VRLD_VRLH_VRLW_VSRAB_VSRAH_VSRAW_XVABSDP_XVABSSP_XVNABSDP_XVNABSSP_XVCPSGNDP_XVCPSGNSP_XVIEXPDP_XVIEXPSP_XVXEXPDP_XVXEXPSP_VRLDMI_VRLDNM_VRLWMI_VRLWNM_VMRGEW_VMRGOW_VNOR_VOR_VSEL_VXOR_XVNEGDP_XVNEGSP_XSABSQP_XSCPSGNQP_XSIEXPQP_XSNABSQP_XSNEGQP_XSXEXPQP") 2, false, false,  1, 7,  1, 7,  0, 0}, // #98
  {DBGFIELD("XXSEL")              2, false, false,  1, 7,  1, 7,  0, 0}, // #99
  {DBGFIELD("TABORTDC_TABORTDCI_TABORTWC_TABORTWCI") 1, false, false,  8, 3,  8, 5,  0, 0}, // #100
  {DBGFIELD("MTFSB0_MTFSB1")      1, false, false,  8, 3,  8, 5,  0, 0}, // #101
  {DBGFIELD("MFFSCDRN_MFFSCDRNI_MFFSCRN_MFFSCRNI") 1, false, false,  8, 3,  8, 5,  0, 0}, // #102
  {DBGFIELD("CMPRB_CMPRB8_CMPEQB") 1, false, false,  8, 3,  8, 5,  0, 0}, // #103
  {DBGFIELD("TD_TDI")             1, false, false,  8, 3,  8, 5,  0, 0}, // #104
  {DBGFIELD("TW_TWI")             1, false, false,  8, 3,  8, 5,  0, 0}, // #105
  {DBGFIELD("FCMPUD_FCMPUS_FTDIV_FTSQRT") 1, false, false,  8, 3,  8, 5,  0, 0}, // #106
  {DBGFIELD("XSTSTDCDP_XSTSTDCSP") 1, false, false,  8, 3,  8, 5,  0, 0}, // #107
  {DBGFIELD("XSMAXCDP_XSMAXDP_XSMAXJDP_XSMINCDP_XSMINDP_XSMINJDP_XSXSIGDP_XSCVSPDPN") 1, false, false, 11, 3,  8, 4,  0, 0}, // #108
  {DBGFIELD("XSCMPEQDP_XSCMPEXPDP_XSCMPGEDP_XSCMPGTDP_XSCMPODP_XSCMPUDP_XSTDIVDP_XSTSQRTDP") 1, false, false, 11, 3,  8, 4,  0, 0}, // #109
  {DBGFIELD("CNTLZD_CNTLZDo_CNTLZW_CNTLZW8_CNTLZW8o_CNTLZWo_CNTTZD_CNTTZDo_CNTTZW_CNTTZW8_CNTTZW8o_CNTTZWo_POPCNTD_POPCNTW_CMPB_CMPB8_SETB_BPERMD") 1, false, false, 11, 3,  8, 4,  0, 0}, // #110
  {DBGFIELD("SLD_SRD_SRAD")       1, false, false, 11, 3,  2, 4,  0, 0}, // #111
  {DBGFIELD("SRADI_EXTSWSLI_SRADI_32_RLDIC") 1, false, false, 11, 3,  2, 4,  0, 0}, // #112
  {DBGFIELD("MFVRD_MFVSRD_MTVSRD_MTVSRWA_MTVSRWZ_MFVSRWZ") 1, false, false, 11, 3,  2, 4,  0, 0}, // #113
  {DBGFIELD("CMPLW_CMPLWI_CMPW_CMPWI_CMPD_CMPDI_CMPLD_CMPLDI") 1, false, false, 11, 3,  2, 4,  0, 0}, // #114
  {DBGFIELD("SUBFC_SUBFC8_SUBFIC_SUBFIC8_ANDISo_ANDISo8_ANDIo_ANDIo8_ADDC_ADDC8_ADDIC_ADDIC8_ADDICo_ADDE_ADDE8_ADDE8o_ADDEo_ADDME_ADDME8_ADDME8o_ADDMEo_ADDZE_ADDZE8_ADDZE8o_ADDZEo_SUBF_SUBF8_SUBF8o_SUBFE_SUBFE8_SUBFE8o_SUBFEo_SUBFME_SUBFME8_SUBFME8o_SUBFMEo_SUBFZE_SUBFZE8_SUBFZE8o_SUBFZEo_SUBFo_POPCNTB_LA") 1, false, false, 11, 3,  2, 4,  0, 0}, // #115
  {DBGFIELD("ADD4_ADD4o_ADD8_ADD8o_NEG_NEG8_NEG8o_NEGo_ADDI_ADDI8_ADDIS_ADDIS8_LI_LI8_LIS_LIS8_OR_OR8_OR8o_ORI_ORI8_ORIS_ORIS8_ORo_XOR_XOR8_XOR8o_XORI_XORI8_XORIS_XORIS8_XORo_NAND_NAND8_NAND8o_NANDo_AND_AND8_AND8o_ANDC_ANDC8_ANDC8o_ANDCo_ANDo_NOR_NOR8_NOR8o_NORo_ORC_ORC8_ORC8o_ORCo_EQV_EQV8_EQV8o_EQVo_EXTSB_EXTSB8_EXTSB8_32_64_EXTSB8o_EXTSBo_EXTSH_EXTSH8_EXTSH8_32_64_EXTSH8o_EXTSHo_EXTSW_EXTSW_32_EXTSW_32_64_EXTSW_32_64o_EXTSWo_ADD4TLS_ADD8TLS_ADD8TLS__NOP") 1, false, false, 11, 3,  2, 4,  0, 0}, // #116
  {DBGFIELD("ADDIStocHA_ADDItocL_COPY") 1, false, false, 11, 3,  2, 4,  0, 0}, // #117
  {DBGFIELD("MCRF")               1, false, false, 11, 3,  2, 4,  0, 0}, // #118
  {DBGFIELD("MCRXRX")             1, false, false, 11, 3,  2, 4,  0, 0}, // #119
  {DBGFIELD("XSNABSDP_XSXEXPDP_XSABSDP_XSNEGDP_XSCPSGNDP") 1, false, false, 11, 3,  2, 4,  0, 0}, // #120
  {DBGFIELD("RFEBB")              1, false, false, 11, 3,  2, 4,  0, 0}, // #121
  {DBGFIELD("TBEGIN_TRECHKPT")    1, false, false, 11, 3,  2, 4,  0, 0}, // #122
  {DBGFIELD("WAIT")               1, false, false, 11, 3,  2, 4,  0, 0}, // #123
  {DBGFIELD("RLDCL_RLDCR")        1, false, false,  8, 3,  2, 5,  0, 0}, // #124
  {DBGFIELD("RLWIMI_RLWIMI8")     1, false, false,  8, 3,  2, 5,  0, 0}, // #125
  {DBGFIELD("RLDICL_RLDICL_32_RLDICL_32_64_RLDICR_RLDICR_32_RLDIMI") 1, false, false,  8, 3,  2, 5,  0, 0}, // #126
  {DBGFIELD("MFOCRF_MFOCRF8")     1, false, false,  8, 3,  2, 5,  0, 0}, // #127
  {DBGFIELD("MTOCRF_MTOCRF8")     1, false, false,  8, 3,  2, 5,  0, 0}, // #128
  {DBGFIELD("CR6SET_CR6UNSET_CRSET_CRUNSET_CRAND_CRANDC_CRNAND_CRNOR_CROR_CRORC_CREQV_CRXOR") 1, false, false,  8, 3,  2, 5,  0, 0}, // #129
  {DBGFIELD("SLW_SLW8_SRW_SRW8_RLWINM_RLWINM8_RLWNM_RLWNM8") 1, false, false,  8, 3,  2, 5,  0, 0}, // #130
  {DBGFIELD("FABSD_FABSS_FNABSD_FNABSS_FNEGD_FNEGS_FCPSGND_FCPSGNS_FMR") 1, false, false,  8, 3,  2, 5,  0, 0}, // #131
  {DBGFIELD("SRAW_SRAWI")         1, false, false,  8, 3,  2, 5,  0, 0}, // #132
  {DBGFIELD("ISEL_ISEL8")         1, false, false,  8, 3,  2, 5,  0, 0}, // #133
  {DBGFIELD("XSIEXPDP")           1, false, false,  8, 3,  2, 5,  0, 0}, // #134
  {DBGFIELD("TRECLAIM_TSR_TABORT") 1, false, false,  8, 3,  2, 5,  0, 0}, // #135
  {DBGFIELD("MFVSCR")             2, false, false,  1, 7, 13, 7,  0, 0}, // #136
  {DBGFIELD("MTVSCR")             2, false, false,  1, 7, 13, 7,  0, 0}, // #137
  {DBGFIELD("VCMPNEZB_VCMPNEZH_VCMPNEZW_VCMPEQUB_VCMPEQUD_VCMPEQUH_VCMPEQUW_VCMPNEB_VCMPNEH_VCMPNEW_VCMPEQFP_VCMPEQFPo_VCMPGEFP_VCMPGEFPo_VCMPGTFP_VCMPGTFPo_VCMPBFP_VCMPBFPo_VCMPGTSB_VCMPGTSBo_VCMPGTSD_VCMPGTSDo_VCMPGTSH_VCMPGTSHo_VCMPGTSW_VCMPGTSWo_VCMPGTUB_VCMPGTUBo_VCMPGTUD_VCMPGTUDo_VCMPGTUH_VCMPGTUHo_VCMPGTUW_VCMPGTUWo_VCMPNEBo_VCMPNEHo_VCMPNEWo_VCMPNEZBo_VCMPNEZHo_VCMPNEZWo_VCMPEQUBo_VCMPEQUDo_VCMPEQUHo_VCMPEQUWo_XVCMPEQDP_XVCMPEQDPo_XVCMPEQSP_XVCMPEQSPo_XVCMPGEDP_XVCMPGEDPo_XVCMPGESP_XVCMPGESPo_XVCMPGTDP_XVCMPGTDPo_XVCMPGTSP_XVCMPGTSPo") 2, false, false,  1, 7, 13, 7,  0, 0}, // #138
  {DBGFIELD("VABSDUB_VABSDUH_VABSDUW_VCLZB_VCLZD_VCLZH_VCLZW_VCTZB_VCTZD_VCTZH_VCTZW_VPOPCNTW_VPOPCNTD_VPRTYBD_VPRTYBW") 2, false, false,  1, 7, 13, 7,  0, 0}, // #139
  {DBGFIELD("VADDUBS_VADDUHS_VADDUWS_VAVGSB_VAVGSH_VAVGSW_VAVGUB_VAVGUH_VAVGUW_VADDSBS_VADDSHS_VADDSWS_VMAXFP_VMINFP_VMAXSB_VMAXSD_VMAXSH_VMAXSW_VMAXUB_VMAXUD_VMAXUH_VMAXUW_VMINSB_VMINSD_VMINSH_VMINSW_VMINUB_VMINUD_VMINUH_VMINUW_VBPERMD_VADDCUW_VSHASIGMAD_VSHASIGMAW_VSUBSBS_VSUBSHS_VSUBSWS_VSUBUBS_VSUBUHS_VSUBUWS_VSUBCUW_XVMAXDP_XVMAXSP_XVMINDP_XVMINSP_XVTSTDCDP_XVTSTDCSP_XVXSIGDP_XVXSIGSP") 2, false, false,  1, 7, 13, 7,  0, 0}, // #140
  {DBGFIELD("XVTDIVDP_XVTDIVSP_XVTSQRTDP_XVTSQRTSP") 2, false, false,  1, 7, 13, 7,  0, 0}, // #141
  {DBGFIELD("VADDFP_VCTSXS_VCTSXS_0_VCTUXS_VCTUXS_0_VEXPTEFP_VLOGEFP_VMADDFP_VMHADDSHS_VNMSUBFP_VREFP_VRFIM_VRFIN_VRFIP_VRFIZ_VRSQRTEFP_XVADDDP_XVADDSP_XVCVDPSP_XVCVDPSXDS_XVCVDPSXWS_XVCVDPUXDS_XVCVDPUXWS_XVCVHPSP_XVCVSPDP_XVCVSPHP_XVCVSPSXDS_XVCVSPSXWS_XVCVSPUXDS_XVCVSPUXWS_XVCVSXDDP_XVCVSXDSP_XVCVSXWDP_XVCVSXWSP_XVCVUXDDP_XVCVUXDSP_XVCVUXWDP_XVCVUXWSP_XVMADDADP_XVMADDASP_XVMADDMDP_XVMADDMSP_XVMSUBADP_XVMSUBASP_XVMSUBMDP_XVMSUBMSP_XVMULDP_XVMULSP_XVNMADDADP_XVNMADDASP_XVNMADDMDP_XVNMADDMSP_XVNMSUBADP_XVNMSUBASP_XVNMSUBMDP_XVNMSUBMSP_XVRDPI_XVRDPIC_XVRDPIM_XVRDPIP_XVRDPIZ_XVREDP_XVRESP_XVRSPI_XVRSPIC_XVRSPIM_XVRSPIP_XVRSPIZ_XVRSQRTEDP_XVRSQRTESP_XVSUBDP_XVSUBSP_VCFSX_VCFSX_0_VCFUX_VCFUX_0_VMHRADDSHS_VMLADDUHM_VMSUMMBM_VMSUMSHM_VMSUMSHS_VMSUMUBM_VMSUMUHM_VMSUMUHS_VMULESB_VMULESH_VMULESW_VMULEUB_VMULEUH_VMULEUW_VMULOSB_VMULOSH_VMULOSW_VMULOUB_VMULOUH_VMULOUW_VSUM2SWS_VSUM4SBS_VSUM4SHS_VSUM4UBS_VSUMSWS") 2, false, false, 14, 7, 20, 7,  0, 0}, // #142
  {DBGFIELD("VSUBFP_VMULUWM")     2, false, false, 14, 7, 20, 7,  0, 0}, // #143
  {DBGFIELD("MADDHD_MADDHDU_MADDLD_MULLD") 1, false, false, 21, 3, 27, 5,  0, 0}, // #144
  {DBGFIELD("MULHD_MULHW_MULLW")  1, false, false, 21, 3, 27, 5,  0, 0}, // #145
  {DBGFIELD("MULHDU_MULHWU")      1, false, false, 21, 3, 27, 5,  0, 0}, // #146
  {DBGFIELD("MULLI_MULLI8")       1, false, false, 21, 3, 27, 5,  0, 0}, // #147
  {DBGFIELD("FRSP_FRIMD_FRIMS_FRIND_FRINS_FRIPD_FRIPS_FRIZD_FRIZS_FRE_FRES_FADDS_FMSUBS_FMADDS_FSUBS_FCFID_FCFIDS_FCFIDU_FCFIDUS_FCTID_FCTIDU_FCTIDUZ_FCTIDZ_FCTIW_FCTIWU_FCTIWUZ_FCTIWZ_FRSQRTE_FRSQRTES_FNMADDS_FNMSUBS_FSELD_FSELS_FMULS") 1, false, false, 21, 3, 21, 5,  0, 0}, // #148
  {DBGFIELD("FADD_FSUB")          1, false, false, 21, 3, 21, 5,  0, 0}, // #149
  {DBGFIELD("FMSUB_FMADD_FNMADD_FNMSUB_FMUL") 1, false, false, 21, 3, 21, 5,  0, 0}, // #150
  {DBGFIELD("XSMADDADP_XSMADDASP_XSMADDMDP_XSMADDMSP_XSMSUBADP_XSMSUBASP_XSMSUBMDP_XSMSUBMSP_XSMULDP_XSMULSP_XSNMADDADP_XSNMADDASP_XSNMADDMDP_XSNMADDMSP_XSNMSUBADP_XSNMSUBASP_XSNMSUBMDP_XSNMSUBMSP") 1, false, false, 21, 3, 21, 5,  0, 0}, // #151
  {DBGFIELD("FSELDo_FSELSo")      2, false, false, 24, 4, 32, 9,  0, 0}, // #152
  {DBGFIELD("MULHDUo_MULHWUo")    2, false, false, 24, 4, 41, 8,  0, 0}, // #153
  {DBGFIELD("MULHDo_MULHWo_MULLWo") 2, false, false, 24, 4, 41, 8,  0, 0}, // #154
  {DBGFIELD("MULLDo")             2, false, false, 24, 4, 41, 8,  0, 0}, // #155
  {DBGFIELD("FRIMDo_FRIMSo_FRINDo_FRINSo_FRIPDo_FRIPSo_FRIZDo_FRIZSo_FRESo_FREo_FADDSo_FSUBSo_FMSUBSo_FNMSUBSo_FMADDSo_FNMADDSo_FCFIDSo_FCFIDUSo_FCFIDUo_FCFIDo_FCTIDUZo_FCTIDUo_FCTIDZo_FCTIDo_FCTIWUZo_FCTIWUo_FCTIWZo_FCTIWo_FMULSo_FRSQRTESo_FRSQRTEo_FRSPo") 2, false, false, 24, 4, 49, 8,  0, 0}, // #156
  {DBGFIELD("FADDo_FSUBo")        2, false, false, 24, 4, 49, 8,  0, 0}, // #157
  {DBGFIELD("FMSUBo_FNMSUBo_FMADDo_FNMADDo_FMULo") 2, false, false, 24, 4, 49, 8,  0, 0}, // #158
  {DBGFIELD("XSADDDP_XSADDSP_XSCVDPHP_XSCVDPSP_XSCVDPSXDS_XSCVDPSXDSs_XSCVDPSXWS_XSCVDPUXDS_XSCVDPUXDSs_XSCVDPUXWS_XSCVDPSXWSs_XSCVDPUXWSs_XSCVHPDP_XSCVSPDP_XSCVSXDDP_XSCVSXDSP_XSCVUXDDP_XSCVUXDSP_XSRDPI_XSRDPIC_XSRDPIM_XSRDPIP_XSRDPIZ_XSREDP_XSRESP_XSRSQRTEDP_XSRSQRTESP_XSSUBDP_XSSUBSP_XSCVDPSPN_XSRSP") 1, false, false, 28, 3, 21, 4,  0, 0}, // #159
  {DBGFIELD("LVSL_LVSR")          1, false, false, 31, 5, 14, 6,  0, 0}, // #160
  {DBGFIELD("VSPLTISB_VSPLTISH_VSPLTISW_VSPLTB_VSPLTBs_VSPLTH_VSPLTHs_VSPLTW_XXMRGHW_XXMRGLW_XXPERM_XXPERMR_XXSLDWI_XXSLDWIs_XXSPLTIB_XXSPLTW_XXSPLTWs_XXPERMDI_XXPERMDIs") 1, false, false, 31, 5, 14, 6,  0, 0}, // #161
  {DBGFIELD("V_SETALLONES_V_SETALLONESB_V_SETALLONESH_VBPERMQ_VGBBD_VMRGHB_VMRGHH_VMRGHW_VMRGLB_VMRGLH_VMRGLW_VPERM_VPERMR_VPERMXOR_VPKPX_VPKSDSS_VPKSDUS_VPKSHSS_VPKSHUS_VPKSWSS_VPKSWUS_VPKUDUM_VPKUDUS_VPKUHUM_VPKUHUS_VPKUWUM_VPKUWUS_VSL_VSLDOI_VSLO_VSLV_VSR_VSRO_VSRV_VUPKHPX_VUPKHSB_VUPKHSH_VUPKHSW_VUPKLPX_VUPKLSB_VUPKLSH_VUPKLSW_XXBRD_XXBRH_XXBRQ_XXBRW_XXEXTRACTUW_XXINSERTW_VADDCUQ_VADDECUQ_VADDEUQM_VMUL10CUQ_VMUL10ECUQ_VMUL10EUQ_VMUL10UQ_VSUBCUQ_VSUBECUQ_VSUBEUQM_XSTSTDCQP_XSXSIGQP_BCDCFNo_BCDCFZo_BCDCPSGNo_BCDCTNo_BCDCTZo_BCDSETSGNo_BCDSo_BCDTRUNCo_BCDUSo_BCDUTRUNCo") 1, false, false, 31, 5, 14, 6,  0, 0}, // #162
  {DBGFIELD("VEXTRACTUB_VEXTRACTUH_VEXTRACTUW_VINSERTB_VINSERTD_VINSERTH_VINSERTW_MFVSRLD_MTVSRWS_VCLZLSBB_VCTZLSBB_VEXTRACTD_VEXTUBLX_VEXTUBRX_VEXTUHLX_VEXTUHRX_VEXTUWLX_VEXTUWRX_VPRTYBQ_VADDUQM_VSUBUQM") 1, false, false, 31, 5, 14, 6,  0, 0}, // #163
  {DBGFIELD("XSCMPEXPQP_XSCMPOQP_XSCMPUQP") 1, false, false, 31, 5, 14, 6,  0, 0}, // #164
  {DBGFIELD("BCDSRo_XSADDQP_XSADDQPO_XSCVDPQP_XSCVQPDP_XSCVQPDPO_XSCVQPSDZ_XSCVQPSWZ_XSCVQPUDZ_XSCVQPUWZ_XSCVSDQP_XSCVUDQP_XSRQPI_XSRQPIX_XSRQPXP_XSSUBQP_XSSUBQPO") 1, false, false, 36, 5, 57, 6,  0, 0}, // #165
  {DBGFIELD("BCDCTSQo")           1, false, false, 41, 5, 63, 6,  0, 0}, // #166
  {DBGFIELD("XSMADDQP_XSMADDQPO_XSMSUBQP_XSMSUBQPO_XSMULQP_XSMULQPO_XSNMADDQP_XSNMADDQPO_XSNMSUBQP_XSNMSUBQPO") 1, false, false, 46, 5, 69, 6,  0, 0}, // #167
  {DBGFIELD("BCDCFSQo")           1, false, false, 51, 5, 75, 6,  0, 0}, // #168
  {DBGFIELD("XSDIVQP_XSDIVQPO")   1, false, false, 56, 5, 81, 6,  0, 0}, // #169
  {DBGFIELD("XSSQRTQP_XSSQRTQPO") 1, false, false, 61, 5, 87, 6,  0, 0}, // #170
  {DBGFIELD("LXVL_LXVLL")         1, false, false, 66, 3, 93, 4,  0, 0}, // #171
  {DBGFIELD("LVEBX_LVEHX_LVEWX_LVX_LVXL") 1, false, false, 66, 3, 27, 4,  0, 0}, // #172
  {DBGFIELD("LXSIBZX_LXSIHZX_LXSDX_LXVB16X_LXVD2X_LXVWSX_LXSIWZX_LXV_LXVX_LXSD") 1, false, false, 66, 3, 27, 4,  0, 0}, // #173
  {DBGFIELD("DFLOADf64_XFLOADf64_LIWZX") 1, false, false, 66, 3, 27, 4,  0, 0}, // #174
  {DBGFIELD("DCBF_DCBFEP_DCBST_DCBSTEP_DCBT_DCBTEP_DCBZ_DCBZEP_DCBZL_DCBZLEP_DCBTST_DCBTSTEP") 1, false, false, 66, 3, 97, 4,  0, 0}, // #175
  {DBGFIELD("CP_COPY_CP_COPY8")   1, false, false, 66, 3, 97, 4,  0, 0}, // #176
  {DBGFIELD("CP_PASTE_CP_PASTE8") 1, false, false, 66, 3, 97, 4,  0, 0}, // #177
  {DBGFIELD("ICBI_ICBIEP")        1, false, false, 66, 3, 97, 4,  0, 0}, // #178
  {DBGFIELD("ICBT_ICBTLS_LBZ_LBZ8_LBZCIX_LBZX_LBZX8_LBZXTLS_LBZXTLS__LBZXTLS_32_LDBRX_LDCIX_LHBRX_LHBRX8_LHZ_LHZ8_LHZCIX_LHZX_LHZX8_LHZXTLS_LHZXTLS__LHZXTLS_32_LWBRX_LWBRX8_LWZ_LWZ8_LWZCIX_LWZX_LWZX8_LWZXTLS_LWZXTLS__LWZXTLS_32_EnforceIEIO_LSWI") 1, false, false, 66, 3, 97, 4,  0, 0}, // #179
  {DBGFIELD("LBARX_LBARXL_LHARX_LHARXL_LWARX_LWARXL") 1, false, false, 66, 3, 97, 4,  0, 0}, // #180
  {DBGFIELD("LD_LDX_LDXTLS_LDXTLS__DARN") 1, false, false, 66, 3, 97, 4,  0, 0}, // #181
  {DBGFIELD("LDARX_LDARXL")       1, false, false, 66, 3, 97, 4,  0, 0}, // #182
  {DBGFIELD("CP_ABORT")           1, false, false, 66, 3, 97, 4,  0, 0}, // #183
  {DBGFIELD("ISYNC")              1, false, false, 66, 3, 97, 4,  0, 0}, // #184
  {DBGFIELD("MSGSYNC")            1, false, false, 66, 3, 97, 4,  0, 0}, // #185
  {DBGFIELD("TLBSYNC")            1, false, false, 66, 3, 97, 4,  0, 0}, // #186
  {DBGFIELD("SYNC")               1, false, false, 66, 3, 97, 4,  0, 0}, // #187
  {DBGFIELD("LMW")                1, false, false, 66, 3, 97, 4,  0, 0}, // #188
  {DBGFIELD("LFIWZX_LFDX_LFD")    1, false, false, 69, 3, 101, 5,  0, 0}, // #189
  {DBGFIELD("SLBIA")              2, false, false, 72, 3, 106, 8,  0, 0}, // #190
  {DBGFIELD("SLBIE")              2, false, false, 72, 3, 106, 8,  0, 0}, // #191
  {DBGFIELD("SLBMFEE")            2, false, false, 72, 3, 106, 8,  0, 0}, // #192
  {DBGFIELD("SLBMFEV")            2, false, false, 72, 3, 106, 8,  0, 0}, // #193
  {DBGFIELD("SLBMTE")             2, false, false, 72, 3, 106, 8,  0, 0}, // #194
  {DBGFIELD("TLBIEL")             2, false, false, 72, 3, 106, 8,  0, 0}, // #195
  {DBGFIELD("LHZU_LHZU8_LWZU_LWZU8") 2, false, false, 75, 5, 114, 8,  0, 0}, // #196
  {DBGFIELD("LHZUX_LHZUX8_LWZUX_LWZUX8") 2, false, false, 75, 5, 114, 8,  0, 0}, // #197
  {DBGFIELD("TEND")               2, false, false, 75, 5, 114, 8,  0, 0}, // #198
  {DBGFIELD("STBCX_STHCX_STWCX")  2, false, false, 80, 5, 122, 9,  0, 0}, // #199
  {DBGFIELD("STDCX")              2, false, false, 80, 5, 122, 9,  0, 0}, // #200
  {DBGFIELD("LDMX")               2, false, false, 72, 3, 131, 7,  0, 0}, // #201
  {DBGFIELD("LHA_LHA8_LHAX_LHAX8_LWAX_LWAX_32") 2, false, false, 75, 5, 138, 7,  0, 0}, // #202
  {DBGFIELD("CP_PASTE8o_CP_PASTEo") 2, false, false, 75, 5, 138, 7,  0, 0}, // #203
  {DBGFIELD("LWA_LWA_32")         2, false, false, 75, 5, 138, 7,  0, 0}, // #204
  {DBGFIELD("TCHECK")             2, false, false, 75, 5, 138, 7,  0, 0}, // #205
  {DBGFIELD("LFIWAX")             2, false, false, 85, 5, 145, 9,  0, 0}, // #206
  {DBGFIELD("LXSIWAX")            2, false, false, 75, 5, 41, 7,  0, 0}, // #207
  {DBGFIELD("LIWAX")              2, false, false, 75, 5, 41, 7,  0, 0}, // #208
  {DBGFIELD("LFSX_LFS")           2, false, false, 85, 5, 154, 9,  0, 0}, // #209
  {DBGFIELD("LXSSP_LXSSPX")       2, false, false, 75, 5, 131, 7,  0, 0}, // #210
  {DBGFIELD("XFLOADf32_DFLOADf32") 2, false, false, 75, 5, 131, 7,  0, 0}, // #211
  {DBGFIELD("LHAU_LHAU8")         3, false, false, 90, 5, 163, 11,  0, 0}, // #212
  {DBGFIELD("LHAUX_LHAUX8_LWAUX") 3, false, false, 90, 5, 163, 11,  0, 0}, // #213
  {DBGFIELD("LXVH8X_LXVDSX_LXVW4X") 2, false, false, 95, 7, 174, 9,  0, 0}, // #214
  {DBGFIELD("STFD_STFDX_STFIWX_STFS_STFSX_STXSD_STXSDX_STXSIBX_STXSIBXv_STXSIHX_STXSIHXv_STXSIWX_STXSSP_STXSSPX") 1, false, false, 102, 4, 34, 6,  0, 0}, // #215
  {DBGFIELD("STW_STW8_STDBRX_STHBRX_STWBRX_STB_STB8_STH_STH8_STBX_STBX8_STBXTLS_STBXTLS__STBXTLS_32_STHX_STHX8_STHXTLS_STHXTLS__STHXTLS_32_STWX_STWX8_STWXTLS_STWXTLS__STWXTLS_32") 1, false, false, 102, 4, 34, 6,  0, 0}, // #216
  {DBGFIELD("DFSTOREf32_DFSTOREf64_XFSTOREf32_XFSTOREf64_STIWX") 1, false, false, 102, 4, 34, 6,  0, 0}, // #217
  {DBGFIELD("STD_STDX_STDXTLS_STDXTLS_") 1, false, false, 102, 4, 34, 6,  0, 0}, // #218
  {DBGFIELD("STBCIX_STDCIX_STHCIX_STWCIX_STSWI") 1, false, false, 102, 4, 34, 6,  0, 0}, // #219
  {DBGFIELD("SLBIEG")             1, false, false, 102, 4, 34, 6,  0, 0}, // #220
  {DBGFIELD("STMW")               1, false, false, 102, 4, 34, 6,  0, 0}, // #221
  {DBGFIELD("TLBIE")              1, false, false, 102, 4, 34, 6,  0, 0}, // #222
  {DBGFIELD("STVEBX_STVEHX_STVEWX_STVX_STVXL") 1, false, false, 106, 6, 34, 7,  0, 0}, // #223
  {DBGFIELD("STXV_STXVB16X_STXVD2X_STXVH8X_STXVW4X_STXVX") 1, false, false, 106, 6, 34, 7,  0, 0}, // #224
  {DBGFIELD("STXVL_STXVLL")       1, false, false, 106, 6, 34, 7,  0, 0}, // #225
  {DBGFIELD("MTCTR_MTCTR8_MTCTR8loop_MTCTRloop_MTLR_MTLR8") 1, false, false, 112, 5, 183, 6,  0, 0}, // #226
  {DBGFIELD("MFVRSAVE_MFVRSAVEv_MTVRSAVE_MTVRSAVEv") 1, false, false, 112, 5, 57, 6,  0, 0}, // #227
  {DBGFIELD("MFPMR")              1, false, false, 112, 5, 57, 6,  0, 0}, // #228
  {DBGFIELD("MTPMR")              1, false, false, 112, 5, 57, 6,  0, 0}, // #229
  {DBGFIELD("MFTB_MFTB8")         1, false, false, 112, 5, 57, 6,  0, 0}, // #230
  {DBGFIELD("MFCTR_MFCTR8_MFLR_MFLR8_MFSPR_MFSPR8") 1, false, false, 112, 5, 57, 6,  0, 0}, // #231
  {DBGFIELD("MFMSR")              1, false, false, 112, 5, 57, 6,  0, 0}, // #232
  {DBGFIELD("MTMSR")              1, false, false, 112, 5, 57, 6,  0, 0}, // #233
  {DBGFIELD("MTMSRD")             1, false, false, 112, 5, 57, 6,  0, 0}, // #234
  {DBGFIELD("MTSPR_MTSPR8")       1, false, false, 112, 5, 57, 6,  0, 0}, // #235
  {DBGFIELD("DIVW_DIVWU_MODSW")   1, false, false, 117, 5, 189, 6,  0, 0}, // #236
  {DBGFIELD("DIVWE_DIVWEU_MODSD_MODUD_MODUW") 1, false, false, 117, 5, 69, 6,  0, 0}, // #237
  {DBGFIELD("DIVD_DIVDU")         1, false, false, 117, 5, 69, 6,  0, 0}, // #238
  {DBGFIELD("DIVDE_DIVDEU")       1, false, false, 117, 5, 195, 6,  0, 0}, // #239
  {DBGFIELD("DIVWUo_DIVWo")       2, false, false, 122, 6, 201, 9,  0, 0}, // #240
  {DBGFIELD("DIVDo_DIVDUo")       2, false, false, 122, 6, 210, 9,  0, 0}, // #241
  {DBGFIELD("DIVWEo_DIVWEUo")     2, false, false, 122, 6, 210, 9,  0, 0}, // #242
  {DBGFIELD("DIVDEo_DIVDEUo")     2, false, false, 122, 6, 219, 9,  0, 0}, // #243
  {DBGFIELD("MTCRF_MTCRF8")       2, false, false, 128, 3, 228, 10,  0, 0}, // #244
  {DBGFIELD("ADDC8o_ADDCo_SUBFC8o_SUBFCo") 2, false, false, 131, 3, 228, 8,  0, 0}, // #245
  {DBGFIELD("FABSDo_FABSSo_FNABSDo_FNABSSo_FCPSGNDo_FCPSGNSo_FNEGDo_FNEGSo_FMRo") 2, false, false, 134, 3, 238, 9,  0, 0}, // #246
  {DBGFIELD("MCRFS")              2, false, false, 131, 3, 247, 8,  0, 0}, // #247
  {DBGFIELD("MTFSF_MTFSFo_MTFSFI_MTFSFIo") 2, false, false, 128, 3, 255, 10,  0, 0}, // #248
  {DBGFIELD("MTFSFb")             2, false, false, 128, 3, 255, 10,  0, 0}, // #249
  {DBGFIELD("RLDCLo_RLDCRo")      2, false, false, 134, 3, 265, 8,  0, 0}, // #250
  {DBGFIELD("RLDICLo_RLDICRo_RLDICL_32o_RLDIMIo") 2, false, false, 134, 3, 265, 8,  0, 0}, // #251
  {DBGFIELD("RLWIMI8o_RLWIMIo")   2, false, false, 134, 3, 265, 8,  0, 0}, // #252
  {DBGFIELD("RLWINM8o_RLWINMo_RLWNM8o_RLWNMo_SLW8o_SLWo_SRW8o_SRWo") 2, false, false, 134, 3, 265, 8,  0, 0}, // #253
  {DBGFIELD("SRAWIo_SRAWo")       2, false, false, 134, 3, 265, 8,  0, 0}, // #254
  {DBGFIELD("MFFS_MFFSCE_MFFSL_MFFSo") 2, false, false, 128, 3, 145, 9,  0, 0}, // #255
  {DBGFIELD("MFCR_MFCR8")         3, false, false, 137, 3, 273, 13,  0, 0}, // #256
  {DBGFIELD("EXTSWSLIo_SRADIo_RLDICo") 2, false, false, 131, 3, 107, 7,  0, 0}, // #257
  {DBGFIELD("SRADo_SLDo_SRDo")    2, false, false, 131, 3, 107, 7,  0, 0}, // #258
  {DBGFIELD("FDIV")               1, false, false, 140, 3, 286, 5,  0, 0}, // #259
  {DBGFIELD("FDIVo")              2, false, false, 143, 4, 291, 8,  0, 0}, // #260
  {DBGFIELD("XSSQRTDP")           1, false, false, 147, 3, 291, 4,  0, 0}, // #261
  {DBGFIELD("FSQRT")              1, false, false, 150, 3, 291, 5,  0, 0}, // #262
  {DBGFIELD("XVSQRTDP")           2, false, false, 153, 5, 299, 7,  0, 0}, // #263
  {DBGFIELD("XVSQRTSP")           2, false, false, 153, 5, 306, 7,  0, 0}, // #264
  {DBGFIELD("FSQRTo")             2, false, false, 158, 4, 313, 8,  0, 0}, // #265
  {DBGFIELD("XSSQRTSP")           1, false, false, 162, 3, 321, 4,  0, 0}, // #266
  {DBGFIELD("FSQRTS")             1, false, false, 165, 3, 325, 5,  0, 0}, // #267
  {DBGFIELD("FSQRTSo")            2, false, false, 168, 4, 330, 8,  0, 0}, // #268
  {DBGFIELD("XSDIVDP")            1, false, false, 172, 3, 286, 4,  0, 0}, // #269
  {DBGFIELD("FDIVS")              1, false, false, 165, 3, 325, 5,  0, 0}, // #270
  {DBGFIELD("FDIVSo")             2, false, false, 168, 4, 330, 8,  0, 0}, // #271
  {DBGFIELD("XSDIVSP")            1, false, false, 162, 3, 321, 4,  0, 0}, // #272
  {DBGFIELD("XVDIVSP")            2, false, false, 175, 7, 338, 7,  0, 0}, // #273
  {DBGFIELD("XVDIVDP")            2, false, false, 175, 7, 345, 7,  0, 0}, // #274
  {DBGFIELD("LFSU")               3, false, false, 182, 5, 352, 13,  0, 0}, // #275
  {DBGFIELD("LFSUX")              3, false, false, 182, 5, 352, 13,  0, 0}, // #276
  {DBGFIELD("STFDU_STFDUX_STFSU_STFSUX") 2, false, false, 80, 5, 365, 10,  0, 0}, // #277
  {DBGFIELD("STBU_STBU8_STBUX_STBUX8_STHU_STHU8_STHUX_STHUX8_STWU_STWU8_STWUX_STWUX8") 2, false, false, 80, 5, 365, 10,  0, 0}, // #278
  {DBGFIELD("STDU")               2, false, false, 80, 5, 365, 10,  0, 0}, // #279
  {DBGFIELD("STDUX")              2, false, false, 80, 5, 365, 10,  0, 0}, // #280
  {DBGFIELD("LBZU_LBZU8")         2, false, false, 75, 5, 114, 8,  0, 0}, // #281
  {DBGFIELD("LBZUX_LBZUX8")       2, false, false, 75, 5, 114, 8,  0, 0}, // #282
  {DBGFIELD("LDU")                2, false, false, 75, 5, 114, 8,  0, 0}, // #283
  {DBGFIELD("LDUX")               2, false, false, 75, 5, 114, 8,  0, 0}, // #284
  {DBGFIELD("LFDU")               2, false, false, 187, 5, 375, 9,  0, 0}, // #285
  {DBGFIELD("LFDUX")              2, false, false, 187, 5, 375, 9,  0, 0}, // #286
  {DBGFIELD("VPMSUMB_VPMSUMD_VPMSUMH_VPMSUMW_VCIPHER_VCIPHERLAST_VNCIPHER_VNCIPHERLAST_VSBOX") 1, false, false, 192, 5, 138, 6,  0, 0}, // #287
  {DBGFIELD("BCCCTR_BCCCTR8_BCCCTRL_BCCCTRL8_BCCL_BCCLA_BCCLR_BCCLRL_BCCTR_BCCTR8_BCCTR8n_BCCTRL_BCCTRL8_BCCTRL8n_BCCTRLn_BCCTRn_BDNZ_BDNZ8_BDNZA_BDNZAm_BDNZAp_BDNZm_BDNZp_BDZ_BDZ8_BDZA_BDZAm_BDZAp_BDZm_BDZp_BDNZL_BDNZLA_BDNZLAm_BDNZLAp_BDNZLR_BDNZLR8_BDNZLRL_BDNZLRLm_BDNZLRLp_BDNZLRm_BDNZLRp_BDNZLm_BDNZLp_BDZL_BDZLA_BDZLAm_BDZLAp_BDZLR_BDZLR8_BDZLRL_BDZLRLm_BDZLRLp_BDZLRm_BDZLRp_BDZLm_BDZLp_BL_BL_TLS_BL8_BL8_NOP_BL8_NOP_TLS_BL8_TLS_BL8_TLS__BLA_BLA8_BLA8_NOP_BLR_BLR8_BLRL_TAILB_TAILB8_TAILBA_TAILBA8_TAILBCTR_TAILBCTR8_gBC_gBCA_gBCAat_gBCCTR_gBCCTRL_gBCL_gBCLA_gBCLAat_gBCLR_gBCLRL_gBCLat_gBCat_BCLR_BCLRL_BCLRLn_BCLRn_BCTR_BCTR8_BCTRL_BCTRL8_B_BA_BC_BCC_BCCA_BCL_BCLalways_BCLn_BCTRL8_LDinto_toc_BCn_CTRL_DEP") 1, false, false, 197, 2,  2, 3,  0, 0}, // #288
  {DBGFIELD("ADDPCIS")            2, false, false, 199, 4, 21, 6,  0, 0}, // #289
  {DBGFIELD("LDAT_LWAT")          5, false, false, 203, 4, 384, 24,  0, 0}, // #290
  {DBGFIELD("STDAT_STWAT")        3, false, false, 207, 4, 408, 14,  0, 0}, // #291
  {DBGFIELD("BRINC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #292
  {DBGFIELD("EVABS_EVEQV_EVNAND_EVNEG_EVADDIW_EVADDW_EVAND_EVANDC_EVCMPEQ_EVCMPGTS_EVCMPGTU_EVCMPLTS_EVCMPLTU_EVCNTLSW_EVCNTLZW_EVEXTSB_EVEXTSH_EVMERGEHI_EVMERGEHILO_EVMERGELO_EVMERGELOHI_EVNOR_EVOR_EVORC_EVXOR_EVRLW_EVRLWI_EVRNDW_EVSLW_EVSLWI_EVSPLATFI_EVSPLATI_EVSRWIS_EVSRWIU_EVSRWS_EVSRWU_EVSUBFW_EVSUBIFW") 0, false, false,  0, 0,  0, 0,  0, 0}, // #293
  {DBGFIELD("EVMRA_EVADDSMIAAW_EVADDSSIAAW_EVADDUMIAAW_EVADDUSIAAW_EVDIVWS_EVDIVWU_EVMHEGSMFAA_EVMHEGSMFAN_EVMHEGSMIAA_EVMHEGSMIAN_EVMHEGUMIAA_EVMHEGUMIAN_EVMHESMF_EVMHESMFA_EVMHESMFAAW_EVMHESMFANW_EVMHESMI_EVMHESMIA_EVMHESMIAAW_EVMHESMIANW_EVMHESSF_EVMHESSFA_EVMHESSFAAW_EVMHESSFANW_EVMHESSIAAW_EVMHESSIANW_EVMHEUMI_EVMHEUMIA_EVMHEUMIAAW_EVMHEUMIANW_EVMHEUSIAAW_EVMHEUSIANW_EVMHOGSMFAA_EVMHOGSMFAN_EVMHOGSMIAA_EVMHOGSMIAN_EVMHOGUMIAA_EVMHOGUMIAN_EVMHOSMF_EVMHOSMFA_EVMHOSMFAAW_EVMHOSMFANW_EVMHOSMI_EVMHOSMIA_EVMHOSMIAAW_EVMHOSMIANW_EVMHOSSF_EVMHOSSFA_EVMHOSSFAAW_EVMHOSSFANW_EVMHOSSIAAW_EVMHOSSIANW_EVMHOUMI_EVMHOUMIA_EVMHOUMIAAW_EVMHOUMIANW_EVMHOUSIAAW_EVMHOUSIANW_EVMWHSMF_EVMWHSMFA_EVMWHSMI_EVMWHSMIA_EVMWHSSF_EVMWHSSFA_EVMWHUMI_EVMWHUMIA_EVMWLSMIAAW_EVMWLSMIANW_EVMWLSSIAAW_EVMWLSSIANW_EVMWLUMI_EVMWLUMIA_EVMWLUMIAAW_EVMWLUMIANW_EVMWLUSIAAW_EVMWLUSIANW_EVMWSMF_EVMWSMFA_EVMWSMFAA_EVMWSMFAN_EVMWSMI_EVMWSMIA_EVMWSMIAA_EVMWSMIAN_EVMWSSF_EVMWSSFA_EVMWSSFAA_EVMWSSFAN_EVMWUMI_EVMWUMIA_EVMWUMIAA_EVMWUMIAN_EVSUBFSMIAAW_EVSUBFSSIAAW_EVSUBFUMIAAW_EVSUBFUSIAAW") 0, false, false,  0, 0,  0, 0,  0, 0}, // #294
  {DBGFIELD("EVLDD_EVLDDX_EVLDH_EVLDHX_EVLDW_EVLDWX_EVLHHESPLAT_EVLHHESPLATX_EVLHHOSSPLAT_EVLHHOSSPLATX_EVLHHOUSPLAT_EVLHHOUSPLATX_EVLWHE_EVLWHEX_EVLWHOS_EVLWHOSX_EVLWHOU_EVLWHOUX_EVLWHSPLAT_EVLWHSPLATX_EVLWWSPLAT_EVLWWSPLATX") 0, false, false,  0, 0,  0, 0,  0, 0}, // #295
  {DBGFIELD("EVSTDD_EVSTDDX_EVSTDH_EVSTDHX_EVSTDW_EVSTDWX_EVSTWHE_EVSTWHEX_EVSTWHO_EVSTWHOX_EVSTWWE_EVSTWWEX_EVSTWWO_EVSTWWOX") 0, false, false,  0, 0,  0, 0,  0, 0}, // #296
  {DBGFIELD("HRFID_ATTN_CLRBHRB_MFBHRBE_NAP_RFCI_RFDI_RFMCI_SC") 0, false, false,  0, 0,  0, 0,  0, 0}, // #297
  {DBGFIELD("RFI")                0, false, false,  0, 0,  0, 0,  0, 0}, // #298
  {DBGFIELD("RFID")               0, false, false,  0, 0,  0, 0,  0, 0}, // #299
  {DBGFIELD("DSS_DSSALL_DST_DST64_DSTST_DSTST64_DSTSTT_DSTSTT64_DSTT_DSTT64_ICBLQ_LBEPX_LHEPX_LWEPX_TLBIVAX_TLBLD_TLBLI_TLBRE_TLBRE2_TLBSX_TLBSX2_TLBSX2D_TLBWE_TLBWE2_MBAR_TRAP_DCCCI_ICCCI") 0, false, false,  0, 0,  0, 0,  0, 0}, // #300
  {DBGFIELD("ICBLC_STBEPX_STHEPX_STWEPX") 0, false, false,  0, 0,  0, 0,  0, 0}, // #301
  {DBGFIELD("LFDEPX")             0, false, false,  0, 0,  0, 0,  0, 0}, // #302
  {DBGFIELD("STFDEPX")            0, false, false,  0, 0,  0, 0,  0, 0}, // #303
  {DBGFIELD("MFSR_MFSRIN")        0, false, false,  0, 0,  0, 0,  0, 0}, // #304
  {DBGFIELD("MTSR_MTSRIN")        0, false, false,  0, 0,  0, 0,  0, 0}, // #305
  {DBGFIELD("MFDCR")              0, false, false,  0, 0,  0, 0,  0, 0}, // #306
  {DBGFIELD("MTDCR")              0, false, false,  0, 0,  0, 0,  0, 0}, // #307
  {DBGFIELD("NOP_GT_PWR6_NOP_GT_PWR7") 0, false, false,  0, 0,  0, 0,  0, 0}, // #308
  {DBGFIELD("TLBIA")              0, false, false,  0, 0,  0, 0,  0, 0}, // #309
  {DBGFIELD("WRTEE_WRTEEI")       0, false, false,  0, 0,  0, 0,  0, 0}, // #310
  {DBGFIELD("MSYNC")              0, false, false,  0, 0,  0, 0,  0, 0}, // #311
  {DBGFIELD("SLBSYNC")            0, false, false,  0, 0,  0, 0,  0, 0}, // #312
  {DBGFIELD("STOP")               0, false, false,  0, 0,  0, 0,  0, 0}, // #313
  {DBGFIELD("DCBA_DCBI")          0, false, false,  0, 0,  0, 0,  0, 0}, // #314
}; // P9ModelSchedClasses

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

static const llvm::MCSchedModel PPC440Model = {
  2, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  5, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  1, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  PPC440Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel G3ItinerariesModel = {
  MCSchedModel::DefaultIssueWidth,
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  MCSchedModel::DefaultLoadLatency,
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  2, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  G3Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel G4ItinerariesModel = {
  MCSchedModel::DefaultIssueWidth,
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  MCSchedModel::DefaultLoadLatency,
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  3, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  G4Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel G4PlusItinerariesModel = {
  MCSchedModel::DefaultIssueWidth,
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  MCSchedModel::DefaultLoadLatency,
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  4, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  G4PlusItineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel G5Model = {
  4, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  3, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  16, // MispredictPenalty
  false, // PostRAScheduler
  false, // CompleteModel
  5, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  G5Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel PPCA2Model = {
  1, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  6, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  13, // MispredictPenalty
  false, // PostRAScheduler
  false, // CompleteModel
  6, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  PPCA2Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel PPCE500Model = {
  2, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  5, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  7, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  PPCE500Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel PPCE500mcModel = {
  2, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  5, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  8, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  PPCE500mcItineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel PPCE5500Model = {
  2, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  MCSchedModel::DefaultLoopMicroOpBufferSize,
  6, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  MCSchedModel::DefaultMispredictPenalty,
  false, // PostRAScheduler
  false, // CompleteModel
  9, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  PPCE5500Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel P8Model = {
  8, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  60, // LoopMicroOpBufferSize
  3, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  16, // MispredictPenalty
  false, // PostRAScheduler
  false, // CompleteModel
  10, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  P8Itineraries,
  nullptr // No extra processor descriptor
};

static const llvm::MCSchedModel P7Model = {
  6, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  40, // LoopMicroOpBufferSize
  3, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  16, // MispredictPenalty
  false, // PostRAScheduler
  false, // CompleteModel
  11, // Processor ID
  nullptr, nullptr, 0, 0, // No instruction-level machine model.
  P7Itineraries,
  nullptr // No extra processor descriptor
};

static const unsigned P9ModelProcResourceSubUnits[] = {
  0,  // Invalid
};

// {Name, NumUnits, SuperIdx, IsBuffered, SubUnitsIdxBegin}
static const llvm::MCProcResourceDesc P9ModelProcResources[] = {
  {"InvalidUnit", 0, 0, 0, 0},
  {"ALU",             4, 0, -1, nullptr}, // #1
  {"ALUE",            2, 1, -1, nullptr}, // #2, Super=ALU
  {"ALUO",            2, 1, -1, nullptr}, // #3, Super=ALU
  {"BR",              1, 0, 16, nullptr}, // #4
  {"CY",              1, 0, -1, nullptr}, // #5
  {"DFU",             1, 0, -1, nullptr}, // #6
  {"DISPATCHER",      12, 0, -1, nullptr}, // #7
  {"DIV",             2, 0, -1, nullptr}, // #8
  {"DP",              4, 0, -1, nullptr}, // #9
  {"DPE",             2, 9, -1, nullptr}, // #10, Super=DP
  {"DPO",             2, 9, -1, nullptr}, // #11, Super=DP
  {"IP_AGEN",         4, 0, -1, nullptr}, // #12
  {"IP_EXEC",         4, 0, -1, nullptr}, // #13
  {"IP_EXECE",        2, 13, -1, nullptr}, // #14, Super=IP_EXEC
  {"IP_EXECO",        2, 13, -1, nullptr}, // #15, Super=IP_EXEC
  {"LS",              4, 0, -1, nullptr}, // #16
  {"PM",              2, 0, -1, nullptr}, // #17
};

static const llvm::MCSchedModel P9Model = {
  8, // IssueWidth
  MCSchedModel::DefaultMicroOpBufferSize,
  60, // LoopMicroOpBufferSize
  5, // LoadLatency
  MCSchedModel::DefaultHighLatency,
  16, // MispredictPenalty
  false, // PostRAScheduler
  true, // CompleteModel
  12, // Processor ID
  P9ModelProcResources,
  P9ModelSchedClasses,
  18,
  315,
  nullptr, // No Itinerary
  nullptr // No extra processor descriptor
};

// Sorted (by key) array of itineraries for CPU subtype.
extern const llvm::SubtargetInfoKV PPCProcSchedKV[] = {
  { "440", (const void *)&PPC440Model },
  { "450", (const void *)&PPC440Model },
  { "601", (const void *)&G3ItinerariesModel },
  { "602", (const void *)&G3ItinerariesModel },
  { "603", (const void *)&G3ItinerariesModel },
  { "603e", (const void *)&G3ItinerariesModel },
  { "603ev", (const void *)&G3ItinerariesModel },
  { "604", (const void *)&G3ItinerariesModel },
  { "604e", (const void *)&G3ItinerariesModel },
  { "620", (const void *)&G3ItinerariesModel },
  { "7400", (const void *)&G4ItinerariesModel },
  { "7450", (const void *)&G4PlusItinerariesModel },
  { "750", (const void *)&G4ItinerariesModel },
  { "970", (const void *)&G5Model },
  { "a2", (const void *)&PPCA2Model },
  { "a2q", (const void *)&PPCA2Model },
  { "e500", (const void *)&PPCE500Model },
  { "e500mc", (const void *)&PPCE500mcModel },
  { "e5500", (const void *)&PPCE5500Model },
  { "g3", (const void *)&G3ItinerariesModel },
  { "g4", (const void *)&G4ItinerariesModel },
  { "g4+", (const void *)&G4PlusItinerariesModel },
  { "g5", (const void *)&G5Model },
  { "generic", (const void *)&G3ItinerariesModel },
  { "ppc", (const void *)&G3ItinerariesModel },
  { "ppc32", (const void *)&G3ItinerariesModel },
  { "ppc64", (const void *)&G5Model },
  { "ppc64le", (const void *)&P8Model },
  { "pwr3", (const void *)&G5Model },
  { "pwr4", (const void *)&G5Model },
  { "pwr5", (const void *)&G5Model },
  { "pwr5x", (const void *)&G5Model },
  { "pwr6", (const void *)&G5Model },
  { "pwr6x", (const void *)&G5Model },
  { "pwr7", (const void *)&P7Model },
  { "pwr8", (const void *)&P8Model },
  { "pwr9", (const void *)&P9Model },
};

#undef DBGFIELD
namespace PPC_MC {
unsigned resolveVariantSchedClassImpl(unsigned SchedClass,
    const MCInst *MI, unsigned CPUID) {
  // Don't know how to resolve this scheduling class.
  return 0;
}
} // end of namespace PPC_MC

struct PPCGenMCSubtargetInfo : public MCSubtargetInfo {
  PPCGenMCSubtargetInfo(const Triple &TT, 
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
    return PPC_MC::resolveVariantSchedClassImpl(SchedClass, MI, CPUID); 
  }
};

static inline MCSubtargetInfo *createPPCMCSubtargetInfoImpl(const Triple &TT, StringRef CPU, StringRef FS) {
  return new PPCGenMCSubtargetInfo(TT, CPU, FS, PPCFeatureKV, PPCSubTypeKV, 
                      PPCProcSchedKV, PPCWriteProcResTable, PPCWriteLatencyTable, PPCReadAdvanceTable, 
                      PPCStages, PPCOperandCycles, PPCForwardingPaths);
}

} // end namespace llvm

#endif // GET_SUBTARGETINFO_MC_DESC


#ifdef GET_SUBTARGETINFO_TARGET_DESC
#undef GET_SUBTARGETINFO_TARGET_DESC

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

// ParseSubtargetFeatures - Parses features string setting specified
// subtarget options.
void llvm::PPCSubtarget::ParseSubtargetFeatures(StringRef CPU, StringRef FS) {
  LLVM_DEBUG(dbgs() << "\nFeatures:" << FS);
  LLVM_DEBUG(dbgs() << "\nCPU:" << CPU << "\n\n");
  InitMCProcessorInfo(CPU, FS);
  const FeatureBitset& Bits = getFeatureBits();
  if (Bits[PPC::DeprecatedDST]) DeprecatedDST = true;
  if (Bits[PPC::Directive32] && DarwinDirective < PPC::DIR_32) DarwinDirective = PPC::DIR_32;
  if (Bits[PPC::Directive64] && DarwinDirective < PPC::DIR_64) DarwinDirective = PPC::DIR_64;
  if (Bits[PPC::Directive440] && DarwinDirective < PPC::DIR_440) DarwinDirective = PPC::DIR_440;
  if (Bits[PPC::Directive601] && DarwinDirective < PPC::DIR_601) DarwinDirective = PPC::DIR_601;
  if (Bits[PPC::Directive602] && DarwinDirective < PPC::DIR_602) DarwinDirective = PPC::DIR_602;
  if (Bits[PPC::Directive603] && DarwinDirective < PPC::DIR_603) DarwinDirective = PPC::DIR_603;
  if (Bits[PPC::Directive604] && DarwinDirective < PPC::DIR_603) DarwinDirective = PPC::DIR_603;
  if (Bits[PPC::Directive620] && DarwinDirective < PPC::DIR_603) DarwinDirective = PPC::DIR_603;
  if (Bits[PPC::Directive750] && DarwinDirective < PPC::DIR_750) DarwinDirective = PPC::DIR_750;
  if (Bits[PPC::Directive970] && DarwinDirective < PPC::DIR_970) DarwinDirective = PPC::DIR_970;
  if (Bits[PPC::Directive7400] && DarwinDirective < PPC::DIR_7400) DarwinDirective = PPC::DIR_7400;
  if (Bits[PPC::DirectiveA2] && DarwinDirective < PPC::DIR_A2) DarwinDirective = PPC::DIR_A2;
  if (Bits[PPC::DirectiveE500] && DarwinDirective < PPC::DIR_E500) DarwinDirective = PPC::DIR_E500;
  if (Bits[PPC::DirectiveE500mc] && DarwinDirective < PPC::DIR_E500mc) DarwinDirective = PPC::DIR_E500mc;
  if (Bits[PPC::DirectiveE5500] && DarwinDirective < PPC::DIR_E5500) DarwinDirective = PPC::DIR_E5500;
  if (Bits[PPC::DirectivePwr3] && DarwinDirective < PPC::DIR_PWR3) DarwinDirective = PPC::DIR_PWR3;
  if (Bits[PPC::DirectivePwr4] && DarwinDirective < PPC::DIR_PWR4) DarwinDirective = PPC::DIR_PWR4;
  if (Bits[PPC::DirectivePwr5] && DarwinDirective < PPC::DIR_PWR5) DarwinDirective = PPC::DIR_PWR5;
  if (Bits[PPC::DirectivePwr5x] && DarwinDirective < PPC::DIR_PWR5X) DarwinDirective = PPC::DIR_PWR5X;
  if (Bits[PPC::DirectivePwr6] && DarwinDirective < PPC::DIR_PWR6) DarwinDirective = PPC::DIR_PWR6;
  if (Bits[PPC::DirectivePwr6x] && DarwinDirective < PPC::DIR_PWR6X) DarwinDirective = PPC::DIR_PWR6X;
  if (Bits[PPC::DirectivePwr7] && DarwinDirective < PPC::DIR_PWR7) DarwinDirective = PPC::DIR_PWR7;
  if (Bits[PPC::DirectivePwr8] && DarwinDirective < PPC::DIR_PWR8) DarwinDirective = PPC::DIR_PWR8;
  if (Bits[PPC::DirectivePwr9] && DarwinDirective < PPC::DIR_PWR9) DarwinDirective = PPC::DIR_PWR9;
  if (Bits[PPC::Feature64Bit]) Has64BitSupport = true;
  if (Bits[PPC::Feature64BitRegs]) Use64BitRegs = true;
  if (Bits[PPC::FeatureAltivec]) HasAltivec = true;
  if (Bits[PPC::FeatureBPERMD]) HasBPERMD = true;
  if (Bits[PPC::FeatureBookE]) IsBookE = true;
  if (Bits[PPC::FeatureCMPB]) HasCMPB = true;
  if (Bits[PPC::FeatureCRBits]) UseCRBits = true;
  if (Bits[PPC::FeatureDirectMove]) HasDirectMove = true;
  if (Bits[PPC::FeatureE500]) IsE500 = true;
  if (Bits[PPC::FeatureExtDiv]) HasExtDiv = true;
  if (Bits[PPC::FeatureFCPSGN]) HasFCPSGN = true;
  if (Bits[PPC::FeatureFPCVT]) HasFPCVT = true;
  if (Bits[PPC::FeatureFPRND]) HasFPRND = true;
  if (Bits[PPC::FeatureFPU]) HasFPU = true;
  if (Bits[PPC::FeatureFRE]) HasFRE = true;
  if (Bits[PPC::FeatureFRES]) HasFRES = true;
  if (Bits[PPC::FeatureFRSQRTE]) HasFRSQRTE = true;
  if (Bits[PPC::FeatureFRSQRTES]) HasFRSQRTES = true;
  if (Bits[PPC::FeatureFSqrt]) HasFSQRT = true;
  if (Bits[PPC::FeatureFloat128]) HasFloat128 = true;
  if (Bits[PPC::FeatureFusion]) HasFusion = true;
  if (Bits[PPC::FeatureHTM]) HasHTM = true;
  if (Bits[PPC::FeatureHardFloat]) HasHardFloat = true;
  if (Bits[PPC::FeatureICBT]) HasICBT = true;
  if (Bits[PPC::FeatureISA3_0]) IsISA3_0 = true;
  if (Bits[PPC::FeatureISEL]) HasISEL = true;
  if (Bits[PPC::FeatureInvariantFunctionDescriptors]) HasInvariantFunctionDescriptors = true;
  if (Bits[PPC::FeatureLDBRX]) HasLDBRX = true;
  if (Bits[PPC::FeatureLFIWAX]) HasLFIWAX = true;
  if (Bits[PPC::FeatureLongCall]) UseLongCalls = true;
  if (Bits[PPC::FeatureMFOCRF]) HasMFOCRF = true;
  if (Bits[PPC::FeatureMFTB]) FeatureMFTB = true;
  if (Bits[PPC::FeatureMSYNC]) HasOnlyMSYNC = true;
  if (Bits[PPC::FeatureP8Altivec]) HasP8Altivec = true;
  if (Bits[PPC::FeatureP8Crypto]) HasP8Crypto = true;
  if (Bits[PPC::FeatureP8Vector]) HasP8Vector = true;
  if (Bits[PPC::FeatureP9Altivec]) HasP9Altivec = true;
  if (Bits[PPC::FeatureP9Vector]) HasP9Vector = true;
  if (Bits[PPC::FeaturePOPCNTD] && HasPOPCNTD < POPCNTD_Fast) HasPOPCNTD = POPCNTD_Fast;
  if (Bits[PPC::FeaturePPC4xx]) IsPPC4xx = true;
  if (Bits[PPC::FeaturePPC6xx]) IsPPC6xx = true;
  if (Bits[PPC::FeaturePartwordAtomic]) HasPartwordAtomics = true;
  if (Bits[PPC::FeatureQPX]) HasQPX = true;
  if (Bits[PPC::FeatureRecipPrec]) HasRecipPrec = true;
  if (Bits[PPC::FeatureSPE]) HasSPE = true;
  if (Bits[PPC::FeatureSTFIWX]) HasSTFIWX = true;
  if (Bits[PPC::FeatureSecurePlt]) SecurePlt = true;
  if (Bits[PPC::FeatureSlowPOPCNTD] && HasPOPCNTD < POPCNTD_Slow) HasPOPCNTD = POPCNTD_Slow;
  if (Bits[PPC::FeatureVSX]) HasVSX = true;
}
#endif // GET_SUBTARGETINFO_TARGET_DESC


#ifdef GET_SUBTARGETINFO_HEADER
#undef GET_SUBTARGETINFO_HEADER

namespace llvm {
class DFAPacketizer;
namespace PPC_MC {
unsigned resolveVariantSchedClassImpl(unsigned SchedClass, const MCInst *MI, unsigned CPUID);
}

struct PPCGenSubtargetInfo : public TargetSubtargetInfo {
  explicit PPCGenSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS);
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
extern const llvm::SubtargetFeatureKV PPCFeatureKV[];
extern const llvm::SubtargetFeatureKV PPCSubTypeKV[];
extern const llvm::SubtargetInfoKV PPCProcSchedKV[];
extern const llvm::MCWriteProcResEntry PPCWriteProcResTable[];
extern const llvm::MCWriteLatencyEntry PPCWriteLatencyTable[];
extern const llvm::MCReadAdvanceEntry PPCReadAdvanceTable[];
extern const llvm::InstrStage PPCStages[];
extern const unsigned PPCOperandCycles[];
extern const unsigned PPCForwardingPaths[];
PPCGenSubtargetInfo::PPCGenSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS)
  : TargetSubtargetInfo(TT, CPU, FS, makeArrayRef(PPCFeatureKV, 48), makeArrayRef(PPCSubTypeKV, 37), 
                        PPCProcSchedKV, PPCWriteProcResTable, PPCWriteLatencyTable, PPCReadAdvanceTable, 
                        PPCStages, PPCOperandCycles, PPCForwardingPaths) {}

unsigned PPCGenSubtargetInfo
::resolveSchedClass(unsigned SchedClass, const MachineInstr *MI, const TargetSchedModel *SchedModel) const {
  report_fatal_error("Expected a variant SchedClass");
} // PPCGenSubtargetInfo::resolveSchedClass

unsigned PPCGenSubtargetInfo
::resolveVariantSchedClass(unsigned SchedClass, const MCInst *MI, unsigned CPUID) const {
  return PPC_MC::resolveVariantSchedClassImpl(SchedClass, MI, CPUID);
} // PPCGenSubtargetInfo::resolveVariantSchedClass
} // end namespace llvm

#endif // GET_SUBTARGETINFO_CTOR

