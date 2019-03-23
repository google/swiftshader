/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Target Instruction Enum Values and Descriptors                             *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_INSTRINFO_ENUM
#undef GET_INSTRINFO_ENUM
namespace llvm {

namespace Mips {
  enum {
    PHI	= 0,
    INLINEASM	= 1,
    CFI_INSTRUCTION	= 2,
    EH_LABEL	= 3,
    GC_LABEL	= 4,
    ANNOTATION_LABEL	= 5,
    KILL	= 6,
    EXTRACT_SUBREG	= 7,
    INSERT_SUBREG	= 8,
    IMPLICIT_DEF	= 9,
    SUBREG_TO_REG	= 10,
    COPY_TO_REGCLASS	= 11,
    DBG_VALUE	= 12,
    DBG_LABEL	= 13,
    REG_SEQUENCE	= 14,
    COPY	= 15,
    BUNDLE	= 16,
    LIFETIME_START	= 17,
    LIFETIME_END	= 18,
    STACKMAP	= 19,
    FENTRY_CALL	= 20,
    PATCHPOINT	= 21,
    LOAD_STACK_GUARD	= 22,
    STATEPOINT	= 23,
    LOCAL_ESCAPE	= 24,
    FAULTING_OP	= 25,
    PATCHABLE_OP	= 26,
    PATCHABLE_FUNCTION_ENTER	= 27,
    PATCHABLE_RET	= 28,
    PATCHABLE_FUNCTION_EXIT	= 29,
    PATCHABLE_TAIL_CALL	= 30,
    PATCHABLE_EVENT_CALL	= 31,
    PATCHABLE_TYPED_EVENT_CALL	= 32,
    ICALL_BRANCH_FUNNEL	= 33,
    G_ADD	= 34,
    G_SUB	= 35,
    G_MUL	= 36,
    G_SDIV	= 37,
    G_UDIV	= 38,
    G_SREM	= 39,
    G_UREM	= 40,
    G_AND	= 41,
    G_OR	= 42,
    G_XOR	= 43,
    G_IMPLICIT_DEF	= 44,
    G_PHI	= 45,
    G_FRAME_INDEX	= 46,
    G_GLOBAL_VALUE	= 47,
    G_EXTRACT	= 48,
    G_UNMERGE_VALUES	= 49,
    G_INSERT	= 50,
    G_MERGE_VALUES	= 51,
    G_PTRTOINT	= 52,
    G_INTTOPTR	= 53,
    G_BITCAST	= 54,
    G_LOAD	= 55,
    G_SEXTLOAD	= 56,
    G_ZEXTLOAD	= 57,
    G_STORE	= 58,
    G_ATOMIC_CMPXCHG_WITH_SUCCESS	= 59,
    G_ATOMIC_CMPXCHG	= 60,
    G_ATOMICRMW_XCHG	= 61,
    G_ATOMICRMW_ADD	= 62,
    G_ATOMICRMW_SUB	= 63,
    G_ATOMICRMW_AND	= 64,
    G_ATOMICRMW_NAND	= 65,
    G_ATOMICRMW_OR	= 66,
    G_ATOMICRMW_XOR	= 67,
    G_ATOMICRMW_MAX	= 68,
    G_ATOMICRMW_MIN	= 69,
    G_ATOMICRMW_UMAX	= 70,
    G_ATOMICRMW_UMIN	= 71,
    G_BRCOND	= 72,
    G_BRINDIRECT	= 73,
    G_INTRINSIC	= 74,
    G_INTRINSIC_W_SIDE_EFFECTS	= 75,
    G_ANYEXT	= 76,
    G_TRUNC	= 77,
    G_CONSTANT	= 78,
    G_FCONSTANT	= 79,
    G_VASTART	= 80,
    G_VAARG	= 81,
    G_SEXT	= 82,
    G_ZEXT	= 83,
    G_SHL	= 84,
    G_LSHR	= 85,
    G_ASHR	= 86,
    G_ICMP	= 87,
    G_FCMP	= 88,
    G_SELECT	= 89,
    G_UADDE	= 90,
    G_USUBE	= 91,
    G_SADDO	= 92,
    G_SSUBO	= 93,
    G_UMULO	= 94,
    G_SMULO	= 95,
    G_UMULH	= 96,
    G_SMULH	= 97,
    G_FADD	= 98,
    G_FSUB	= 99,
    G_FMUL	= 100,
    G_FMA	= 101,
    G_FDIV	= 102,
    G_FREM	= 103,
    G_FPOW	= 104,
    G_FEXP	= 105,
    G_FEXP2	= 106,
    G_FLOG	= 107,
    G_FLOG2	= 108,
    G_FNEG	= 109,
    G_FPEXT	= 110,
    G_FPTRUNC	= 111,
    G_FPTOSI	= 112,
    G_FPTOUI	= 113,
    G_SITOFP	= 114,
    G_UITOFP	= 115,
    G_FABS	= 116,
    G_GEP	= 117,
    G_PTR_MASK	= 118,
    G_BR	= 119,
    G_INSERT_VECTOR_ELT	= 120,
    G_EXTRACT_VECTOR_ELT	= 121,
    G_SHUFFLE_VECTOR	= 122,
    G_BSWAP	= 123,
    G_ADDRSPACE_CAST	= 124,
    G_BLOCK_ADDR	= 125,
    ABSMacro	= 126,
    ADJCALLSTACKDOWN	= 127,
    ADJCALLSTACKUP	= 128,
    AND_V_D_PSEUDO	= 129,
    AND_V_H_PSEUDO	= 130,
    AND_V_W_PSEUDO	= 131,
    ATOMIC_CMP_SWAP_I16	= 132,
    ATOMIC_CMP_SWAP_I16_POSTRA	= 133,
    ATOMIC_CMP_SWAP_I32	= 134,
    ATOMIC_CMP_SWAP_I32_POSTRA	= 135,
    ATOMIC_CMP_SWAP_I64	= 136,
    ATOMIC_CMP_SWAP_I64_POSTRA	= 137,
    ATOMIC_CMP_SWAP_I8	= 138,
    ATOMIC_CMP_SWAP_I8_POSTRA	= 139,
    ATOMIC_LOAD_ADD_I16	= 140,
    ATOMIC_LOAD_ADD_I16_POSTRA	= 141,
    ATOMIC_LOAD_ADD_I32	= 142,
    ATOMIC_LOAD_ADD_I32_POSTRA	= 143,
    ATOMIC_LOAD_ADD_I64	= 144,
    ATOMIC_LOAD_ADD_I64_POSTRA	= 145,
    ATOMIC_LOAD_ADD_I8	= 146,
    ATOMIC_LOAD_ADD_I8_POSTRA	= 147,
    ATOMIC_LOAD_AND_I16	= 148,
    ATOMIC_LOAD_AND_I16_POSTRA	= 149,
    ATOMIC_LOAD_AND_I32	= 150,
    ATOMIC_LOAD_AND_I32_POSTRA	= 151,
    ATOMIC_LOAD_AND_I64	= 152,
    ATOMIC_LOAD_AND_I64_POSTRA	= 153,
    ATOMIC_LOAD_AND_I8	= 154,
    ATOMIC_LOAD_AND_I8_POSTRA	= 155,
    ATOMIC_LOAD_NAND_I16	= 156,
    ATOMIC_LOAD_NAND_I16_POSTRA	= 157,
    ATOMIC_LOAD_NAND_I32	= 158,
    ATOMIC_LOAD_NAND_I32_POSTRA	= 159,
    ATOMIC_LOAD_NAND_I64	= 160,
    ATOMIC_LOAD_NAND_I64_POSTRA	= 161,
    ATOMIC_LOAD_NAND_I8	= 162,
    ATOMIC_LOAD_NAND_I8_POSTRA	= 163,
    ATOMIC_LOAD_OR_I16	= 164,
    ATOMIC_LOAD_OR_I16_POSTRA	= 165,
    ATOMIC_LOAD_OR_I32	= 166,
    ATOMIC_LOAD_OR_I32_POSTRA	= 167,
    ATOMIC_LOAD_OR_I64	= 168,
    ATOMIC_LOAD_OR_I64_POSTRA	= 169,
    ATOMIC_LOAD_OR_I8	= 170,
    ATOMIC_LOAD_OR_I8_POSTRA	= 171,
    ATOMIC_LOAD_SUB_I16	= 172,
    ATOMIC_LOAD_SUB_I16_POSTRA	= 173,
    ATOMIC_LOAD_SUB_I32	= 174,
    ATOMIC_LOAD_SUB_I32_POSTRA	= 175,
    ATOMIC_LOAD_SUB_I64	= 176,
    ATOMIC_LOAD_SUB_I64_POSTRA	= 177,
    ATOMIC_LOAD_SUB_I8	= 178,
    ATOMIC_LOAD_SUB_I8_POSTRA	= 179,
    ATOMIC_LOAD_XOR_I16	= 180,
    ATOMIC_LOAD_XOR_I16_POSTRA	= 181,
    ATOMIC_LOAD_XOR_I32	= 182,
    ATOMIC_LOAD_XOR_I32_POSTRA	= 183,
    ATOMIC_LOAD_XOR_I64	= 184,
    ATOMIC_LOAD_XOR_I64_POSTRA	= 185,
    ATOMIC_LOAD_XOR_I8	= 186,
    ATOMIC_LOAD_XOR_I8_POSTRA	= 187,
    ATOMIC_SWAP_I16	= 188,
    ATOMIC_SWAP_I16_POSTRA	= 189,
    ATOMIC_SWAP_I32	= 190,
    ATOMIC_SWAP_I32_POSTRA	= 191,
    ATOMIC_SWAP_I64	= 192,
    ATOMIC_SWAP_I64_POSTRA	= 193,
    ATOMIC_SWAP_I8	= 194,
    ATOMIC_SWAP_I8_POSTRA	= 195,
    B	= 196,
    BAL_BR	= 197,
    BAL_BR_MM	= 198,
    BEQLImmMacro	= 199,
    BGE	= 200,
    BGEImmMacro	= 201,
    BGEL	= 202,
    BGELImmMacro	= 203,
    BGEU	= 204,
    BGEUImmMacro	= 205,
    BGEUL	= 206,
    BGEULImmMacro	= 207,
    BGT	= 208,
    BGTImmMacro	= 209,
    BGTL	= 210,
    BGTLImmMacro	= 211,
    BGTU	= 212,
    BGTUImmMacro	= 213,
    BGTUL	= 214,
    BGTULImmMacro	= 215,
    BLE	= 216,
    BLEImmMacro	= 217,
    BLEL	= 218,
    BLELImmMacro	= 219,
    BLEU	= 220,
    BLEUImmMacro	= 221,
    BLEUL	= 222,
    BLEULImmMacro	= 223,
    BLT	= 224,
    BLTImmMacro	= 225,
    BLTL	= 226,
    BLTLImmMacro	= 227,
    BLTU	= 228,
    BLTUImmMacro	= 229,
    BLTUL	= 230,
    BLTULImmMacro	= 231,
    BNELImmMacro	= 232,
    BPOSGE32_PSEUDO	= 233,
    BSEL_D_PSEUDO	= 234,
    BSEL_FD_PSEUDO	= 235,
    BSEL_FW_PSEUDO	= 236,
    BSEL_H_PSEUDO	= 237,
    BSEL_W_PSEUDO	= 238,
    B_MM	= 239,
    B_MMR6_Pseudo	= 240,
    B_MM_Pseudo	= 241,
    BeqImm	= 242,
    BneImm	= 243,
    BteqzT8CmpX16	= 244,
    BteqzT8CmpiX16	= 245,
    BteqzT8SltX16	= 246,
    BteqzT8SltiX16	= 247,
    BteqzT8SltiuX16	= 248,
    BteqzT8SltuX16	= 249,
    BtnezT8CmpX16	= 250,
    BtnezT8CmpiX16	= 251,
    BtnezT8SltX16	= 252,
    BtnezT8SltiX16	= 253,
    BtnezT8SltiuX16	= 254,
    BtnezT8SltuX16	= 255,
    BuildPairF64	= 256,
    BuildPairF64_64	= 257,
    CFTC1	= 258,
    CONSTPOOL_ENTRY	= 259,
    COPY_FD_PSEUDO	= 260,
    COPY_FW_PSEUDO	= 261,
    CTTC1	= 262,
    Constant32	= 263,
    DMULImmMacro	= 264,
    DMULMacro	= 265,
    DMULOMacro	= 266,
    DMULOUMacro	= 267,
    DROL	= 268,
    DROLImm	= 269,
    DROR	= 270,
    DRORImm	= 271,
    DSDivIMacro	= 272,
    DSDivMacro	= 273,
    DSRemIMacro	= 274,
    DSRemMacro	= 275,
    DUDivIMacro	= 276,
    DUDivMacro	= 277,
    DURemIMacro	= 278,
    DURemMacro	= 279,
    ERet	= 280,
    ExtractElementF64	= 281,
    ExtractElementF64_64	= 282,
    FABS_D	= 283,
    FABS_W	= 284,
    FEXP2_D_1_PSEUDO	= 285,
    FEXP2_W_1_PSEUDO	= 286,
    FILL_FD_PSEUDO	= 287,
    FILL_FW_PSEUDO	= 288,
    GotPrologue16	= 289,
    INSERT_B_VIDX64_PSEUDO	= 290,
    INSERT_B_VIDX_PSEUDO	= 291,
    INSERT_D_VIDX64_PSEUDO	= 292,
    INSERT_D_VIDX_PSEUDO	= 293,
    INSERT_FD_PSEUDO	= 294,
    INSERT_FD_VIDX64_PSEUDO	= 295,
    INSERT_FD_VIDX_PSEUDO	= 296,
    INSERT_FW_PSEUDO	= 297,
    INSERT_FW_VIDX64_PSEUDO	= 298,
    INSERT_FW_VIDX_PSEUDO	= 299,
    INSERT_H_VIDX64_PSEUDO	= 300,
    INSERT_H_VIDX_PSEUDO	= 301,
    INSERT_W_VIDX64_PSEUDO	= 302,
    INSERT_W_VIDX_PSEUDO	= 303,
    JALR64Pseudo	= 304,
    JALRHB64Pseudo	= 305,
    JALRHBPseudo	= 306,
    JALRPseudo	= 307,
    JalOneReg	= 308,
    JalTwoReg	= 309,
    LDMacro	= 310,
    LD_F16	= 311,
    LOAD_ACC128	= 312,
    LOAD_ACC64	= 313,
    LOAD_ACC64DSP	= 314,
    LOAD_CCOND_DSP	= 315,
    LONG_BRANCH_ADDiu	= 316,
    LONG_BRANCH_DADDiu	= 317,
    LONG_BRANCH_LUi	= 318,
    LWM_MM	= 319,
    LoadAddrImm32	= 320,
    LoadAddrImm64	= 321,
    LoadAddrReg32	= 322,
    LoadAddrReg64	= 323,
    LoadImm32	= 324,
    LoadImm64	= 325,
    LoadImmDoubleFGR	= 326,
    LoadImmDoubleFGR_32	= 327,
    LoadImmDoubleGPR	= 328,
    LoadImmSingleFGR	= 329,
    LoadImmSingleGPR	= 330,
    LwConstant32	= 331,
    MFTACX	= 332,
    MFTC0	= 333,
    MFTC1	= 334,
    MFTDSP	= 335,
    MFTGPR	= 336,
    MFTHC1	= 337,
    MFTHI	= 338,
    MFTLO	= 339,
    MIPSeh_return32	= 340,
    MIPSeh_return64	= 341,
    MSA_FP_EXTEND_D_PSEUDO	= 342,
    MSA_FP_EXTEND_W_PSEUDO	= 343,
    MSA_FP_ROUND_D_PSEUDO	= 344,
    MSA_FP_ROUND_W_PSEUDO	= 345,
    MTTACX	= 346,
    MTTC0	= 347,
    MTTC1	= 348,
    MTTDSP	= 349,
    MTTGPR	= 350,
    MTTHC1	= 351,
    MTTHI	= 352,
    MTTLO	= 353,
    MULImmMacro	= 354,
    MULOMacro	= 355,
    MULOUMacro	= 356,
    MultRxRy16	= 357,
    MultRxRyRz16	= 358,
    MultuRxRy16	= 359,
    MultuRxRyRz16	= 360,
    NOP	= 361,
    NORImm	= 362,
    NORImm64	= 363,
    NOR_V_D_PSEUDO	= 364,
    NOR_V_H_PSEUDO	= 365,
    NOR_V_W_PSEUDO	= 366,
    OR_V_D_PSEUDO	= 367,
    OR_V_H_PSEUDO	= 368,
    OR_V_W_PSEUDO	= 369,
    PseudoCMPU_EQ_QB	= 370,
    PseudoCMPU_LE_QB	= 371,
    PseudoCMPU_LT_QB	= 372,
    PseudoCMP_EQ_PH	= 373,
    PseudoCMP_LE_PH	= 374,
    PseudoCMP_LT_PH	= 375,
    PseudoCVT_D32_W	= 376,
    PseudoCVT_D64_L	= 377,
    PseudoCVT_D64_W	= 378,
    PseudoCVT_S_L	= 379,
    PseudoCVT_S_W	= 380,
    PseudoDMULT	= 381,
    PseudoDMULTu	= 382,
    PseudoDSDIV	= 383,
    PseudoDUDIV	= 384,
    PseudoIndirectBranch	= 385,
    PseudoIndirectBranch64	= 386,
    PseudoIndirectBranch64R6	= 387,
    PseudoIndirectBranchR6	= 388,
    PseudoIndirectBranch_MM	= 389,
    PseudoIndirectBranch_MMR6	= 390,
    PseudoIndirectHazardBranch	= 391,
    PseudoIndirectHazardBranch64	= 392,
    PseudoIndrectHazardBranch64R6	= 393,
    PseudoIndrectHazardBranchR6	= 394,
    PseudoMADD	= 395,
    PseudoMADDU	= 396,
    PseudoMFHI	= 397,
    PseudoMFHI64	= 398,
    PseudoMFLO	= 399,
    PseudoMFLO64	= 400,
    PseudoMSUB	= 401,
    PseudoMSUBU	= 402,
    PseudoMTLOHI	= 403,
    PseudoMTLOHI64	= 404,
    PseudoMTLOHI_DSP	= 405,
    PseudoMULT	= 406,
    PseudoMULTu	= 407,
    PseudoPICK_PH	= 408,
    PseudoPICK_QB	= 409,
    PseudoReturn	= 410,
    PseudoReturn64	= 411,
    PseudoSDIV	= 412,
    PseudoSELECTFP_F_D32	= 413,
    PseudoSELECTFP_F_D64	= 414,
    PseudoSELECTFP_F_I	= 415,
    PseudoSELECTFP_F_I64	= 416,
    PseudoSELECTFP_F_S	= 417,
    PseudoSELECTFP_T_D32	= 418,
    PseudoSELECTFP_T_D64	= 419,
    PseudoSELECTFP_T_I	= 420,
    PseudoSELECTFP_T_I64	= 421,
    PseudoSELECTFP_T_S	= 422,
    PseudoSELECT_D32	= 423,
    PseudoSELECT_D64	= 424,
    PseudoSELECT_I	= 425,
    PseudoSELECT_I64	= 426,
    PseudoSELECT_S	= 427,
    PseudoTRUNC_W_D	= 428,
    PseudoTRUNC_W_D32	= 429,
    PseudoTRUNC_W_S	= 430,
    PseudoUDIV	= 431,
    ROL	= 432,
    ROLImm	= 433,
    ROR	= 434,
    RORImm	= 435,
    RetRA	= 436,
    RetRA16	= 437,
    SDIV_MM_Pseudo	= 438,
    SDMacro	= 439,
    SDivIMacro	= 440,
    SDivMacro	= 441,
    SEQIMacro	= 442,
    SEQMacro	= 443,
    SLTImm64	= 444,
    SLTUImm64	= 445,
    SNZ_B_PSEUDO	= 446,
    SNZ_D_PSEUDO	= 447,
    SNZ_H_PSEUDO	= 448,
    SNZ_V_PSEUDO	= 449,
    SNZ_W_PSEUDO	= 450,
    SRemIMacro	= 451,
    SRemMacro	= 452,
    STORE_ACC128	= 453,
    STORE_ACC64	= 454,
    STORE_ACC64DSP	= 455,
    STORE_CCOND_DSP	= 456,
    ST_F16	= 457,
    SWM_MM	= 458,
    SZ_B_PSEUDO	= 459,
    SZ_D_PSEUDO	= 460,
    SZ_H_PSEUDO	= 461,
    SZ_V_PSEUDO	= 462,
    SZ_W_PSEUDO	= 463,
    SelBeqZ	= 464,
    SelBneZ	= 465,
    SelTBteqZCmp	= 466,
    SelTBteqZCmpi	= 467,
    SelTBteqZSlt	= 468,
    SelTBteqZSlti	= 469,
    SelTBteqZSltiu	= 470,
    SelTBteqZSltu	= 471,
    SelTBtneZCmp	= 472,
    SelTBtneZCmpi	= 473,
    SelTBtneZSlt	= 474,
    SelTBtneZSlti	= 475,
    SelTBtneZSltiu	= 476,
    SelTBtneZSltu	= 477,
    SltCCRxRy16	= 478,
    SltiCCRxImmX16	= 479,
    SltiuCCRxImmX16	= 480,
    SltuCCRxRy16	= 481,
    SltuRxRyRz16	= 482,
    TAILCALL	= 483,
    TAILCALL64R6REG	= 484,
    TAILCALLHB64R6REG	= 485,
    TAILCALLHBR6REG	= 486,
    TAILCALLR6REG	= 487,
    TAILCALLREG	= 488,
    TAILCALLREG64	= 489,
    TAILCALLREGHB	= 490,
    TAILCALLREGHB64	= 491,
    TAILCALLREG_MM	= 492,
    TAILCALLREG_MMR6	= 493,
    TAILCALL_MM	= 494,
    TAILCALL_MMR6	= 495,
    TRAP	= 496,
    TRAP_MM	= 497,
    UDIV_MM_Pseudo	= 498,
    UDivIMacro	= 499,
    UDivMacro	= 500,
    URemIMacro	= 501,
    URemMacro	= 502,
    Ulh	= 503,
    Ulhu	= 504,
    Ulw	= 505,
    Ush	= 506,
    Usw	= 507,
    XOR_V_D_PSEUDO	= 508,
    XOR_V_H_PSEUDO	= 509,
    XOR_V_W_PSEUDO	= 510,
    ABSQ_S_PH	= 511,
    ABSQ_S_PH_MM	= 512,
    ABSQ_S_QB	= 513,
    ABSQ_S_QB_MMR2	= 514,
    ABSQ_S_W	= 515,
    ABSQ_S_W_MM	= 516,
    ADD	= 517,
    ADDIUPC	= 518,
    ADDIUPC_MM	= 519,
    ADDIUPC_MMR6	= 520,
    ADDIUR1SP_MM	= 521,
    ADDIUR2_MM	= 522,
    ADDIUS5_MM	= 523,
    ADDIUSP_MM	= 524,
    ADDIU_MMR6	= 525,
    ADDQH_PH	= 526,
    ADDQH_PH_MMR2	= 527,
    ADDQH_R_PH	= 528,
    ADDQH_R_PH_MMR2	= 529,
    ADDQH_R_W	= 530,
    ADDQH_R_W_MMR2	= 531,
    ADDQH_W	= 532,
    ADDQH_W_MMR2	= 533,
    ADDQ_PH	= 534,
    ADDQ_PH_MM	= 535,
    ADDQ_S_PH	= 536,
    ADDQ_S_PH_MM	= 537,
    ADDQ_S_W	= 538,
    ADDQ_S_W_MM	= 539,
    ADDSC	= 540,
    ADDSC_MM	= 541,
    ADDS_A_B	= 542,
    ADDS_A_D	= 543,
    ADDS_A_H	= 544,
    ADDS_A_W	= 545,
    ADDS_S_B	= 546,
    ADDS_S_D	= 547,
    ADDS_S_H	= 548,
    ADDS_S_W	= 549,
    ADDS_U_B	= 550,
    ADDS_U_D	= 551,
    ADDS_U_H	= 552,
    ADDS_U_W	= 553,
    ADDU16_MM	= 554,
    ADDU16_MMR6	= 555,
    ADDUH_QB	= 556,
    ADDUH_QB_MMR2	= 557,
    ADDUH_R_QB	= 558,
    ADDUH_R_QB_MMR2	= 559,
    ADDU_MMR6	= 560,
    ADDU_PH	= 561,
    ADDU_PH_MMR2	= 562,
    ADDU_QB	= 563,
    ADDU_QB_MM	= 564,
    ADDU_S_PH	= 565,
    ADDU_S_PH_MMR2	= 566,
    ADDU_S_QB	= 567,
    ADDU_S_QB_MM	= 568,
    ADDVI_B	= 569,
    ADDVI_D	= 570,
    ADDVI_H	= 571,
    ADDVI_W	= 572,
    ADDV_B	= 573,
    ADDV_D	= 574,
    ADDV_H	= 575,
    ADDV_W	= 576,
    ADDWC	= 577,
    ADDWC_MM	= 578,
    ADD_A_B	= 579,
    ADD_A_D	= 580,
    ADD_A_H	= 581,
    ADD_A_W	= 582,
    ADD_MM	= 583,
    ADD_MMR6	= 584,
    ADDi	= 585,
    ADDi_MM	= 586,
    ADDiu	= 587,
    ADDiu_MM	= 588,
    ADDu	= 589,
    ADDu_MM	= 590,
    ALIGN	= 591,
    ALIGN_MMR6	= 592,
    ALUIPC	= 593,
    ALUIPC_MMR6	= 594,
    AND	= 595,
    AND16_MM	= 596,
    AND16_MMR6	= 597,
    AND64	= 598,
    ANDI16_MM	= 599,
    ANDI16_MMR6	= 600,
    ANDI_B	= 601,
    ANDI_MMR6	= 602,
    AND_MM	= 603,
    AND_MMR6	= 604,
    AND_V	= 605,
    ANDi	= 606,
    ANDi64	= 607,
    ANDi_MM	= 608,
    APPEND	= 609,
    APPEND_MMR2	= 610,
    ASUB_S_B	= 611,
    ASUB_S_D	= 612,
    ASUB_S_H	= 613,
    ASUB_S_W	= 614,
    ASUB_U_B	= 615,
    ASUB_U_D	= 616,
    ASUB_U_H	= 617,
    ASUB_U_W	= 618,
    AUI	= 619,
    AUIPC	= 620,
    AUIPC_MMR6	= 621,
    AUI_MMR6	= 622,
    AVER_S_B	= 623,
    AVER_S_D	= 624,
    AVER_S_H	= 625,
    AVER_S_W	= 626,
    AVER_U_B	= 627,
    AVER_U_D	= 628,
    AVER_U_H	= 629,
    AVER_U_W	= 630,
    AVE_S_B	= 631,
    AVE_S_D	= 632,
    AVE_S_H	= 633,
    AVE_S_W	= 634,
    AVE_U_B	= 635,
    AVE_U_D	= 636,
    AVE_U_H	= 637,
    AVE_U_W	= 638,
    AddiuRxImmX16	= 639,
    AddiuRxPcImmX16	= 640,
    AddiuRxRxImm16	= 641,
    AddiuRxRxImmX16	= 642,
    AddiuRxRyOffMemX16	= 643,
    AddiuSpImm16	= 644,
    AddiuSpImmX16	= 645,
    AdduRxRyRz16	= 646,
    AndRxRxRy16	= 647,
    B16_MM	= 648,
    BADDu	= 649,
    BAL	= 650,
    BALC	= 651,
    BALC_MMR6	= 652,
    BALIGN	= 653,
    BALIGN_MMR2	= 654,
    BBIT0	= 655,
    BBIT032	= 656,
    BBIT1	= 657,
    BBIT132	= 658,
    BC	= 659,
    BC16_MMR6	= 660,
    BC1EQZ	= 661,
    BC1EQZC_MMR6	= 662,
    BC1F	= 663,
    BC1FL	= 664,
    BC1F_MM	= 665,
    BC1NEZ	= 666,
    BC1NEZC_MMR6	= 667,
    BC1T	= 668,
    BC1TL	= 669,
    BC1T_MM	= 670,
    BC2EQZ	= 671,
    BC2EQZC_MMR6	= 672,
    BC2NEZ	= 673,
    BC2NEZC_MMR6	= 674,
    BCLRI_B	= 675,
    BCLRI_D	= 676,
    BCLRI_H	= 677,
    BCLRI_W	= 678,
    BCLR_B	= 679,
    BCLR_D	= 680,
    BCLR_H	= 681,
    BCLR_W	= 682,
    BC_MMR6	= 683,
    BEQ	= 684,
    BEQ64	= 685,
    BEQC	= 686,
    BEQC64	= 687,
    BEQC_MMR6	= 688,
    BEQL	= 689,
    BEQZ16_MM	= 690,
    BEQZALC	= 691,
    BEQZALC_MMR6	= 692,
    BEQZC	= 693,
    BEQZC16_MMR6	= 694,
    BEQZC64	= 695,
    BEQZC_MM	= 696,
    BEQZC_MMR6	= 697,
    BEQ_MM	= 698,
    BGEC	= 699,
    BGEC64	= 700,
    BGEC_MMR6	= 701,
    BGEUC	= 702,
    BGEUC64	= 703,
    BGEUC_MMR6	= 704,
    BGEZ	= 705,
    BGEZ64	= 706,
    BGEZAL	= 707,
    BGEZALC	= 708,
    BGEZALC_MMR6	= 709,
    BGEZALL	= 710,
    BGEZALS_MM	= 711,
    BGEZAL_MM	= 712,
    BGEZC	= 713,
    BGEZC64	= 714,
    BGEZC_MMR6	= 715,
    BGEZL	= 716,
    BGEZ_MM	= 717,
    BGTZ	= 718,
    BGTZ64	= 719,
    BGTZALC	= 720,
    BGTZALC_MMR6	= 721,
    BGTZC	= 722,
    BGTZC64	= 723,
    BGTZC_MMR6	= 724,
    BGTZL	= 725,
    BGTZ_MM	= 726,
    BINSLI_B	= 727,
    BINSLI_D	= 728,
    BINSLI_H	= 729,
    BINSLI_W	= 730,
    BINSL_B	= 731,
    BINSL_D	= 732,
    BINSL_H	= 733,
    BINSL_W	= 734,
    BINSRI_B	= 735,
    BINSRI_D	= 736,
    BINSRI_H	= 737,
    BINSRI_W	= 738,
    BINSR_B	= 739,
    BINSR_D	= 740,
    BINSR_H	= 741,
    BINSR_W	= 742,
    BITREV	= 743,
    BITREV_MM	= 744,
    BITSWAP	= 745,
    BITSWAP_MMR6	= 746,
    BLEZ	= 747,
    BLEZ64	= 748,
    BLEZALC	= 749,
    BLEZALC_MMR6	= 750,
    BLEZC	= 751,
    BLEZC64	= 752,
    BLEZC_MMR6	= 753,
    BLEZL	= 754,
    BLEZ_MM	= 755,
    BLTC	= 756,
    BLTC64	= 757,
    BLTC_MMR6	= 758,
    BLTUC	= 759,
    BLTUC64	= 760,
    BLTUC_MMR6	= 761,
    BLTZ	= 762,
    BLTZ64	= 763,
    BLTZAL	= 764,
    BLTZALC	= 765,
    BLTZALC_MMR6	= 766,
    BLTZALL	= 767,
    BLTZALS_MM	= 768,
    BLTZAL_MM	= 769,
    BLTZC	= 770,
    BLTZC64	= 771,
    BLTZC_MMR6	= 772,
    BLTZL	= 773,
    BLTZ_MM	= 774,
    BMNZI_B	= 775,
    BMNZ_V	= 776,
    BMZI_B	= 777,
    BMZ_V	= 778,
    BNE	= 779,
    BNE64	= 780,
    BNEC	= 781,
    BNEC64	= 782,
    BNEC_MMR6	= 783,
    BNEGI_B	= 784,
    BNEGI_D	= 785,
    BNEGI_H	= 786,
    BNEGI_W	= 787,
    BNEG_B	= 788,
    BNEG_D	= 789,
    BNEG_H	= 790,
    BNEG_W	= 791,
    BNEL	= 792,
    BNEZ16_MM	= 793,
    BNEZALC	= 794,
    BNEZALC_MMR6	= 795,
    BNEZC	= 796,
    BNEZC16_MMR6	= 797,
    BNEZC64	= 798,
    BNEZC_MM	= 799,
    BNEZC_MMR6	= 800,
    BNE_MM	= 801,
    BNVC	= 802,
    BNVC_MMR6	= 803,
    BNZ_B	= 804,
    BNZ_D	= 805,
    BNZ_H	= 806,
    BNZ_V	= 807,
    BNZ_W	= 808,
    BOVC	= 809,
    BOVC_MMR6	= 810,
    BPOSGE32	= 811,
    BPOSGE32C_MMR3	= 812,
    BPOSGE32_MM	= 813,
    BREAK	= 814,
    BREAK16_MM	= 815,
    BREAK16_MMR6	= 816,
    BREAK_MM	= 817,
    BREAK_MMR6	= 818,
    BSELI_B	= 819,
    BSEL_V	= 820,
    BSETI_B	= 821,
    BSETI_D	= 822,
    BSETI_H	= 823,
    BSETI_W	= 824,
    BSET_B	= 825,
    BSET_D	= 826,
    BSET_H	= 827,
    BSET_W	= 828,
    BZ_B	= 829,
    BZ_D	= 830,
    BZ_H	= 831,
    BZ_V	= 832,
    BZ_W	= 833,
    BeqzRxImm16	= 834,
    BeqzRxImmX16	= 835,
    Bimm16	= 836,
    BimmX16	= 837,
    BnezRxImm16	= 838,
    BnezRxImmX16	= 839,
    Break16	= 840,
    Bteqz16	= 841,
    BteqzX16	= 842,
    Btnez16	= 843,
    BtnezX16	= 844,
    CACHE	= 845,
    CACHEE	= 846,
    CACHEE_MM	= 847,
    CACHE_MM	= 848,
    CACHE_MMR6	= 849,
    CACHE_R6	= 850,
    CEIL_L_D64	= 851,
    CEIL_L_D_MMR6	= 852,
    CEIL_L_S	= 853,
    CEIL_L_S_MMR6	= 854,
    CEIL_W_D32	= 855,
    CEIL_W_D64	= 856,
    CEIL_W_D_MMR6	= 857,
    CEIL_W_MM	= 858,
    CEIL_W_S	= 859,
    CEIL_W_S_MM	= 860,
    CEIL_W_S_MMR6	= 861,
    CEQI_B	= 862,
    CEQI_D	= 863,
    CEQI_H	= 864,
    CEQI_W	= 865,
    CEQ_B	= 866,
    CEQ_D	= 867,
    CEQ_H	= 868,
    CEQ_W	= 869,
    CFC1	= 870,
    CFC1_MM	= 871,
    CFC2_MM	= 872,
    CFCMSA	= 873,
    CINS	= 874,
    CINS32	= 875,
    CINS64_32	= 876,
    CINS_i32	= 877,
    CLASS_D	= 878,
    CLASS_D_MMR6	= 879,
    CLASS_S	= 880,
    CLASS_S_MMR6	= 881,
    CLEI_S_B	= 882,
    CLEI_S_D	= 883,
    CLEI_S_H	= 884,
    CLEI_S_W	= 885,
    CLEI_U_B	= 886,
    CLEI_U_D	= 887,
    CLEI_U_H	= 888,
    CLEI_U_W	= 889,
    CLE_S_B	= 890,
    CLE_S_D	= 891,
    CLE_S_H	= 892,
    CLE_S_W	= 893,
    CLE_U_B	= 894,
    CLE_U_D	= 895,
    CLE_U_H	= 896,
    CLE_U_W	= 897,
    CLO	= 898,
    CLO_MM	= 899,
    CLO_MMR6	= 900,
    CLO_R6	= 901,
    CLTI_S_B	= 902,
    CLTI_S_D	= 903,
    CLTI_S_H	= 904,
    CLTI_S_W	= 905,
    CLTI_U_B	= 906,
    CLTI_U_D	= 907,
    CLTI_U_H	= 908,
    CLTI_U_W	= 909,
    CLT_S_B	= 910,
    CLT_S_D	= 911,
    CLT_S_H	= 912,
    CLT_S_W	= 913,
    CLT_U_B	= 914,
    CLT_U_D	= 915,
    CLT_U_H	= 916,
    CLT_U_W	= 917,
    CLZ	= 918,
    CLZ_MM	= 919,
    CLZ_MMR6	= 920,
    CLZ_R6	= 921,
    CMPGDU_EQ_QB	= 922,
    CMPGDU_EQ_QB_MMR2	= 923,
    CMPGDU_LE_QB	= 924,
    CMPGDU_LE_QB_MMR2	= 925,
    CMPGDU_LT_QB	= 926,
    CMPGDU_LT_QB_MMR2	= 927,
    CMPGU_EQ_QB	= 928,
    CMPGU_EQ_QB_MM	= 929,
    CMPGU_LE_QB	= 930,
    CMPGU_LE_QB_MM	= 931,
    CMPGU_LT_QB	= 932,
    CMPGU_LT_QB_MM	= 933,
    CMPU_EQ_QB	= 934,
    CMPU_EQ_QB_MM	= 935,
    CMPU_LE_QB	= 936,
    CMPU_LE_QB_MM	= 937,
    CMPU_LT_QB	= 938,
    CMPU_LT_QB_MM	= 939,
    CMP_AF_D_MMR6	= 940,
    CMP_AF_S_MMR6	= 941,
    CMP_EQ_D	= 942,
    CMP_EQ_D_MMR6	= 943,
    CMP_EQ_PH	= 944,
    CMP_EQ_PH_MM	= 945,
    CMP_EQ_S	= 946,
    CMP_EQ_S_MMR6	= 947,
    CMP_F_D	= 948,
    CMP_F_S	= 949,
    CMP_LE_D	= 950,
    CMP_LE_D_MMR6	= 951,
    CMP_LE_PH	= 952,
    CMP_LE_PH_MM	= 953,
    CMP_LE_S	= 954,
    CMP_LE_S_MMR6	= 955,
    CMP_LT_D	= 956,
    CMP_LT_D_MMR6	= 957,
    CMP_LT_PH	= 958,
    CMP_LT_PH_MM	= 959,
    CMP_LT_S	= 960,
    CMP_LT_S_MMR6	= 961,
    CMP_SAF_D	= 962,
    CMP_SAF_D_MMR6	= 963,
    CMP_SAF_S	= 964,
    CMP_SAF_S_MMR6	= 965,
    CMP_SEQ_D	= 966,
    CMP_SEQ_D_MMR6	= 967,
    CMP_SEQ_S	= 968,
    CMP_SEQ_S_MMR6	= 969,
    CMP_SLE_D	= 970,
    CMP_SLE_D_MMR6	= 971,
    CMP_SLE_S	= 972,
    CMP_SLE_S_MMR6	= 973,
    CMP_SLT_D	= 974,
    CMP_SLT_D_MMR6	= 975,
    CMP_SLT_S	= 976,
    CMP_SLT_S_MMR6	= 977,
    CMP_SUEQ_D	= 978,
    CMP_SUEQ_D_MMR6	= 979,
    CMP_SUEQ_S	= 980,
    CMP_SUEQ_S_MMR6	= 981,
    CMP_SULE_D	= 982,
    CMP_SULE_D_MMR6	= 983,
    CMP_SULE_S	= 984,
    CMP_SULE_S_MMR6	= 985,
    CMP_SULT_D	= 986,
    CMP_SULT_D_MMR6	= 987,
    CMP_SULT_S	= 988,
    CMP_SULT_S_MMR6	= 989,
    CMP_SUN_D	= 990,
    CMP_SUN_D_MMR6	= 991,
    CMP_SUN_S	= 992,
    CMP_SUN_S_MMR6	= 993,
    CMP_UEQ_D	= 994,
    CMP_UEQ_D_MMR6	= 995,
    CMP_UEQ_S	= 996,
    CMP_UEQ_S_MMR6	= 997,
    CMP_ULE_D	= 998,
    CMP_ULE_D_MMR6	= 999,
    CMP_ULE_S	= 1000,
    CMP_ULE_S_MMR6	= 1001,
    CMP_ULT_D	= 1002,
    CMP_ULT_D_MMR6	= 1003,
    CMP_ULT_S	= 1004,
    CMP_ULT_S_MMR6	= 1005,
    CMP_UN_D	= 1006,
    CMP_UN_D_MMR6	= 1007,
    CMP_UN_S	= 1008,
    CMP_UN_S_MMR6	= 1009,
    COPY_S_B	= 1010,
    COPY_S_D	= 1011,
    COPY_S_H	= 1012,
    COPY_S_W	= 1013,
    COPY_U_B	= 1014,
    COPY_U_H	= 1015,
    COPY_U_W	= 1016,
    CRC32B	= 1017,
    CRC32CB	= 1018,
    CRC32CD	= 1019,
    CRC32CH	= 1020,
    CRC32CW	= 1021,
    CRC32D	= 1022,
    CRC32H	= 1023,
    CRC32W	= 1024,
    CTC1	= 1025,
    CTC1_MM	= 1026,
    CTC2_MM	= 1027,
    CTCMSA	= 1028,
    CVT_D32_S	= 1029,
    CVT_D32_S_MM	= 1030,
    CVT_D32_W	= 1031,
    CVT_D32_W_MM	= 1032,
    CVT_D64_L	= 1033,
    CVT_D64_S	= 1034,
    CVT_D64_S_MM	= 1035,
    CVT_D64_W	= 1036,
    CVT_D64_W_MM	= 1037,
    CVT_D_L_MMR6	= 1038,
    CVT_L_D64	= 1039,
    CVT_L_D64_MM	= 1040,
    CVT_L_D_MMR6	= 1041,
    CVT_L_S	= 1042,
    CVT_L_S_MM	= 1043,
    CVT_L_S_MMR6	= 1044,
    CVT_S_D32	= 1045,
    CVT_S_D32_MM	= 1046,
    CVT_S_D64	= 1047,
    CVT_S_D64_MM	= 1048,
    CVT_S_L	= 1049,
    CVT_S_L_MMR6	= 1050,
    CVT_S_W	= 1051,
    CVT_S_W_MM	= 1052,
    CVT_S_W_MMR6	= 1053,
    CVT_W_D32	= 1054,
    CVT_W_D32_MM	= 1055,
    CVT_W_D64	= 1056,
    CVT_W_D64_MM	= 1057,
    CVT_W_S	= 1058,
    CVT_W_S_MM	= 1059,
    CVT_W_S_MMR6	= 1060,
    C_EQ_D32	= 1061,
    C_EQ_D32_MM	= 1062,
    C_EQ_D64	= 1063,
    C_EQ_D64_MM	= 1064,
    C_EQ_S	= 1065,
    C_EQ_S_MM	= 1066,
    C_F_D32	= 1067,
    C_F_D32_MM	= 1068,
    C_F_D64	= 1069,
    C_F_D64_MM	= 1070,
    C_F_S	= 1071,
    C_F_S_MM	= 1072,
    C_LE_D32	= 1073,
    C_LE_D32_MM	= 1074,
    C_LE_D64	= 1075,
    C_LE_D64_MM	= 1076,
    C_LE_S	= 1077,
    C_LE_S_MM	= 1078,
    C_LT_D32	= 1079,
    C_LT_D32_MM	= 1080,
    C_LT_D64	= 1081,
    C_LT_D64_MM	= 1082,
    C_LT_S	= 1083,
    C_LT_S_MM	= 1084,
    C_NGE_D32	= 1085,
    C_NGE_D32_MM	= 1086,
    C_NGE_D64	= 1087,
    C_NGE_D64_MM	= 1088,
    C_NGE_S	= 1089,
    C_NGE_S_MM	= 1090,
    C_NGLE_D32	= 1091,
    C_NGLE_D32_MM	= 1092,
    C_NGLE_D64	= 1093,
    C_NGLE_D64_MM	= 1094,
    C_NGLE_S	= 1095,
    C_NGLE_S_MM	= 1096,
    C_NGL_D32	= 1097,
    C_NGL_D32_MM	= 1098,
    C_NGL_D64	= 1099,
    C_NGL_D64_MM	= 1100,
    C_NGL_S	= 1101,
    C_NGL_S_MM	= 1102,
    C_NGT_D32	= 1103,
    C_NGT_D32_MM	= 1104,
    C_NGT_D64	= 1105,
    C_NGT_D64_MM	= 1106,
    C_NGT_S	= 1107,
    C_NGT_S_MM	= 1108,
    C_OLE_D32	= 1109,
    C_OLE_D32_MM	= 1110,
    C_OLE_D64	= 1111,
    C_OLE_D64_MM	= 1112,
    C_OLE_S	= 1113,
    C_OLE_S_MM	= 1114,
    C_OLT_D32	= 1115,
    C_OLT_D32_MM	= 1116,
    C_OLT_D64	= 1117,
    C_OLT_D64_MM	= 1118,
    C_OLT_S	= 1119,
    C_OLT_S_MM	= 1120,
    C_SEQ_D32	= 1121,
    C_SEQ_D32_MM	= 1122,
    C_SEQ_D64	= 1123,
    C_SEQ_D64_MM	= 1124,
    C_SEQ_S	= 1125,
    C_SEQ_S_MM	= 1126,
    C_SF_D32	= 1127,
    C_SF_D32_MM	= 1128,
    C_SF_D64	= 1129,
    C_SF_D64_MM	= 1130,
    C_SF_S	= 1131,
    C_SF_S_MM	= 1132,
    C_UEQ_D32	= 1133,
    C_UEQ_D32_MM	= 1134,
    C_UEQ_D64	= 1135,
    C_UEQ_D64_MM	= 1136,
    C_UEQ_S	= 1137,
    C_UEQ_S_MM	= 1138,
    C_ULE_D32	= 1139,
    C_ULE_D32_MM	= 1140,
    C_ULE_D64	= 1141,
    C_ULE_D64_MM	= 1142,
    C_ULE_S	= 1143,
    C_ULE_S_MM	= 1144,
    C_ULT_D32	= 1145,
    C_ULT_D32_MM	= 1146,
    C_ULT_D64	= 1147,
    C_ULT_D64_MM	= 1148,
    C_ULT_S	= 1149,
    C_ULT_S_MM	= 1150,
    C_UN_D32	= 1151,
    C_UN_D32_MM	= 1152,
    C_UN_D64	= 1153,
    C_UN_D64_MM	= 1154,
    C_UN_S	= 1155,
    C_UN_S_MM	= 1156,
    CmpRxRy16	= 1157,
    CmpiRxImm16	= 1158,
    CmpiRxImmX16	= 1159,
    DADD	= 1160,
    DADDi	= 1161,
    DADDiu	= 1162,
    DADDu	= 1163,
    DAHI	= 1164,
    DALIGN	= 1165,
    DATI	= 1166,
    DAUI	= 1167,
    DBITSWAP	= 1168,
    DCLO	= 1169,
    DCLO_R6	= 1170,
    DCLZ	= 1171,
    DCLZ_R6	= 1172,
    DDIV	= 1173,
    DDIVU	= 1174,
    DERET	= 1175,
    DERET_MM	= 1176,
    DERET_MMR6	= 1177,
    DEXT	= 1178,
    DEXT64_32	= 1179,
    DEXTM	= 1180,
    DEXTU	= 1181,
    DI	= 1182,
    DINS	= 1183,
    DINSM	= 1184,
    DINSU	= 1185,
    DIV	= 1186,
    DIVU	= 1187,
    DIVU_MMR6	= 1188,
    DIV_MMR6	= 1189,
    DIV_S_B	= 1190,
    DIV_S_D	= 1191,
    DIV_S_H	= 1192,
    DIV_S_W	= 1193,
    DIV_U_B	= 1194,
    DIV_U_D	= 1195,
    DIV_U_H	= 1196,
    DIV_U_W	= 1197,
    DI_MM	= 1198,
    DI_MMR6	= 1199,
    DLSA	= 1200,
    DLSA_R6	= 1201,
    DMFC0	= 1202,
    DMFC1	= 1203,
    DMFC2	= 1204,
    DMFC2_OCTEON	= 1205,
    DMFGC0	= 1206,
    DMOD	= 1207,
    DMODU	= 1208,
    DMT	= 1209,
    DMTC0	= 1210,
    DMTC1	= 1211,
    DMTC2	= 1212,
    DMTC2_OCTEON	= 1213,
    DMTGC0	= 1214,
    DMUH	= 1215,
    DMUHU	= 1216,
    DMUL	= 1217,
    DMULT	= 1218,
    DMULTu	= 1219,
    DMULU	= 1220,
    DMUL_R6	= 1221,
    DOTP_S_D	= 1222,
    DOTP_S_H	= 1223,
    DOTP_S_W	= 1224,
    DOTP_U_D	= 1225,
    DOTP_U_H	= 1226,
    DOTP_U_W	= 1227,
    DPADD_S_D	= 1228,
    DPADD_S_H	= 1229,
    DPADD_S_W	= 1230,
    DPADD_U_D	= 1231,
    DPADD_U_H	= 1232,
    DPADD_U_W	= 1233,
    DPAQX_SA_W_PH	= 1234,
    DPAQX_SA_W_PH_MMR2	= 1235,
    DPAQX_S_W_PH	= 1236,
    DPAQX_S_W_PH_MMR2	= 1237,
    DPAQ_SA_L_W	= 1238,
    DPAQ_SA_L_W_MM	= 1239,
    DPAQ_S_W_PH	= 1240,
    DPAQ_S_W_PH_MM	= 1241,
    DPAU_H_QBL	= 1242,
    DPAU_H_QBL_MM	= 1243,
    DPAU_H_QBR	= 1244,
    DPAU_H_QBR_MM	= 1245,
    DPAX_W_PH	= 1246,
    DPAX_W_PH_MMR2	= 1247,
    DPA_W_PH	= 1248,
    DPA_W_PH_MMR2	= 1249,
    DPOP	= 1250,
    DPSQX_SA_W_PH	= 1251,
    DPSQX_SA_W_PH_MMR2	= 1252,
    DPSQX_S_W_PH	= 1253,
    DPSQX_S_W_PH_MMR2	= 1254,
    DPSQ_SA_L_W	= 1255,
    DPSQ_SA_L_W_MM	= 1256,
    DPSQ_S_W_PH	= 1257,
    DPSQ_S_W_PH_MM	= 1258,
    DPSUB_S_D	= 1259,
    DPSUB_S_H	= 1260,
    DPSUB_S_W	= 1261,
    DPSUB_U_D	= 1262,
    DPSUB_U_H	= 1263,
    DPSUB_U_W	= 1264,
    DPSU_H_QBL	= 1265,
    DPSU_H_QBL_MM	= 1266,
    DPSU_H_QBR	= 1267,
    DPSU_H_QBR_MM	= 1268,
    DPSX_W_PH	= 1269,
    DPSX_W_PH_MMR2	= 1270,
    DPS_W_PH	= 1271,
    DPS_W_PH_MMR2	= 1272,
    DROTR	= 1273,
    DROTR32	= 1274,
    DROTRV	= 1275,
    DSBH	= 1276,
    DSDIV	= 1277,
    DSHD	= 1278,
    DSLL	= 1279,
    DSLL32	= 1280,
    DSLL64_32	= 1281,
    DSLLV	= 1282,
    DSRA	= 1283,
    DSRA32	= 1284,
    DSRAV	= 1285,
    DSRL	= 1286,
    DSRL32	= 1287,
    DSRLV	= 1288,
    DSUB	= 1289,
    DSUBu	= 1290,
    DUDIV	= 1291,
    DVP	= 1292,
    DVPE	= 1293,
    DVP_MMR6	= 1294,
    DivRxRy16	= 1295,
    DivuRxRy16	= 1296,
    EHB	= 1297,
    EHB_MM	= 1298,
    EHB_MMR6	= 1299,
    EI	= 1300,
    EI_MM	= 1301,
    EI_MMR6	= 1302,
    EMT	= 1303,
    ERET	= 1304,
    ERETNC	= 1305,
    ERETNC_MMR6	= 1306,
    ERET_MM	= 1307,
    ERET_MMR6	= 1308,
    EVP	= 1309,
    EVPE	= 1310,
    EVP_MMR6	= 1311,
    EXT	= 1312,
    EXTP	= 1313,
    EXTPDP	= 1314,
    EXTPDPV	= 1315,
    EXTPDPV_MM	= 1316,
    EXTPDP_MM	= 1317,
    EXTPV	= 1318,
    EXTPV_MM	= 1319,
    EXTP_MM	= 1320,
    EXTRV_RS_W	= 1321,
    EXTRV_RS_W_MM	= 1322,
    EXTRV_R_W	= 1323,
    EXTRV_R_W_MM	= 1324,
    EXTRV_S_H	= 1325,
    EXTRV_S_H_MM	= 1326,
    EXTRV_W	= 1327,
    EXTRV_W_MM	= 1328,
    EXTR_RS_W	= 1329,
    EXTR_RS_W_MM	= 1330,
    EXTR_R_W	= 1331,
    EXTR_R_W_MM	= 1332,
    EXTR_S_H	= 1333,
    EXTR_S_H_MM	= 1334,
    EXTR_W	= 1335,
    EXTR_W_MM	= 1336,
    EXTS	= 1337,
    EXTS32	= 1338,
    EXT_MM	= 1339,
    EXT_MMR6	= 1340,
    FABS_D32	= 1341,
    FABS_D32_MM	= 1342,
    FABS_D64	= 1343,
    FABS_D64_MM	= 1344,
    FABS_S	= 1345,
    FABS_S_MM	= 1346,
    FADD_D	= 1347,
    FADD_D32	= 1348,
    FADD_D32_MM	= 1349,
    FADD_D64	= 1350,
    FADD_D64_MM	= 1351,
    FADD_S	= 1352,
    FADD_S_MM	= 1353,
    FADD_S_MMR6	= 1354,
    FADD_W	= 1355,
    FCAF_D	= 1356,
    FCAF_W	= 1357,
    FCEQ_D	= 1358,
    FCEQ_W	= 1359,
    FCLASS_D	= 1360,
    FCLASS_W	= 1361,
    FCLE_D	= 1362,
    FCLE_W	= 1363,
    FCLT_D	= 1364,
    FCLT_W	= 1365,
    FCMP_D32	= 1366,
    FCMP_D32_MM	= 1367,
    FCMP_D64	= 1368,
    FCMP_S32	= 1369,
    FCMP_S32_MM	= 1370,
    FCNE_D	= 1371,
    FCNE_W	= 1372,
    FCOR_D	= 1373,
    FCOR_W	= 1374,
    FCUEQ_D	= 1375,
    FCUEQ_W	= 1376,
    FCULE_D	= 1377,
    FCULE_W	= 1378,
    FCULT_D	= 1379,
    FCULT_W	= 1380,
    FCUNE_D	= 1381,
    FCUNE_W	= 1382,
    FCUN_D	= 1383,
    FCUN_W	= 1384,
    FDIV_D	= 1385,
    FDIV_D32	= 1386,
    FDIV_D32_MM	= 1387,
    FDIV_D64	= 1388,
    FDIV_D64_MM	= 1389,
    FDIV_S	= 1390,
    FDIV_S_MM	= 1391,
    FDIV_S_MMR6	= 1392,
    FDIV_W	= 1393,
    FEXDO_H	= 1394,
    FEXDO_W	= 1395,
    FEXP2_D	= 1396,
    FEXP2_W	= 1397,
    FEXUPL_D	= 1398,
    FEXUPL_W	= 1399,
    FEXUPR_D	= 1400,
    FEXUPR_W	= 1401,
    FFINT_S_D	= 1402,
    FFINT_S_W	= 1403,
    FFINT_U_D	= 1404,
    FFINT_U_W	= 1405,
    FFQL_D	= 1406,
    FFQL_W	= 1407,
    FFQR_D	= 1408,
    FFQR_W	= 1409,
    FILL_B	= 1410,
    FILL_D	= 1411,
    FILL_H	= 1412,
    FILL_W	= 1413,
    FLOG2_D	= 1414,
    FLOG2_W	= 1415,
    FLOOR_L_D64	= 1416,
    FLOOR_L_D_MMR6	= 1417,
    FLOOR_L_S	= 1418,
    FLOOR_L_S_MMR6	= 1419,
    FLOOR_W_D32	= 1420,
    FLOOR_W_D64	= 1421,
    FLOOR_W_D_MMR6	= 1422,
    FLOOR_W_MM	= 1423,
    FLOOR_W_S	= 1424,
    FLOOR_W_S_MM	= 1425,
    FLOOR_W_S_MMR6	= 1426,
    FMADD_D	= 1427,
    FMADD_W	= 1428,
    FMAX_A_D	= 1429,
    FMAX_A_W	= 1430,
    FMAX_D	= 1431,
    FMAX_W	= 1432,
    FMIN_A_D	= 1433,
    FMIN_A_W	= 1434,
    FMIN_D	= 1435,
    FMIN_W	= 1436,
    FMOV_D32	= 1437,
    FMOV_D32_MM	= 1438,
    FMOV_D64	= 1439,
    FMOV_D64_MM	= 1440,
    FMOV_S	= 1441,
    FMOV_S_MM	= 1442,
    FMOV_S_MMR6	= 1443,
    FMSUB_D	= 1444,
    FMSUB_W	= 1445,
    FMUL_D	= 1446,
    FMUL_D32	= 1447,
    FMUL_D32_MM	= 1448,
    FMUL_D64	= 1449,
    FMUL_D64_MM	= 1450,
    FMUL_S	= 1451,
    FMUL_S_MM	= 1452,
    FMUL_S_MMR6	= 1453,
    FMUL_W	= 1454,
    FNEG_D32	= 1455,
    FNEG_D32_MM	= 1456,
    FNEG_D64	= 1457,
    FNEG_D64_MM	= 1458,
    FNEG_S	= 1459,
    FNEG_S_MM	= 1460,
    FNEG_S_MMR6	= 1461,
    FORK	= 1462,
    FRCP_D	= 1463,
    FRCP_W	= 1464,
    FRINT_D	= 1465,
    FRINT_W	= 1466,
    FRSQRT_D	= 1467,
    FRSQRT_W	= 1468,
    FSAF_D	= 1469,
    FSAF_W	= 1470,
    FSEQ_D	= 1471,
    FSEQ_W	= 1472,
    FSLE_D	= 1473,
    FSLE_W	= 1474,
    FSLT_D	= 1475,
    FSLT_W	= 1476,
    FSNE_D	= 1477,
    FSNE_W	= 1478,
    FSOR_D	= 1479,
    FSOR_W	= 1480,
    FSQRT_D	= 1481,
    FSQRT_D32	= 1482,
    FSQRT_D32_MM	= 1483,
    FSQRT_D64	= 1484,
    FSQRT_D64_MM	= 1485,
    FSQRT_S	= 1486,
    FSQRT_S_MM	= 1487,
    FSQRT_W	= 1488,
    FSUB_D	= 1489,
    FSUB_D32	= 1490,
    FSUB_D32_MM	= 1491,
    FSUB_D64	= 1492,
    FSUB_D64_MM	= 1493,
    FSUB_S	= 1494,
    FSUB_S_MM	= 1495,
    FSUB_S_MMR6	= 1496,
    FSUB_W	= 1497,
    FSUEQ_D	= 1498,
    FSUEQ_W	= 1499,
    FSULE_D	= 1500,
    FSULE_W	= 1501,
    FSULT_D	= 1502,
    FSULT_W	= 1503,
    FSUNE_D	= 1504,
    FSUNE_W	= 1505,
    FSUN_D	= 1506,
    FSUN_W	= 1507,
    FTINT_S_D	= 1508,
    FTINT_S_W	= 1509,
    FTINT_U_D	= 1510,
    FTINT_U_W	= 1511,
    FTQ_H	= 1512,
    FTQ_W	= 1513,
    FTRUNC_S_D	= 1514,
    FTRUNC_S_W	= 1515,
    FTRUNC_U_D	= 1516,
    FTRUNC_U_W	= 1517,
    GINVI	= 1518,
    GINVI_MMR6	= 1519,
    GINVT	= 1520,
    GINVT_MMR6	= 1521,
    HADD_S_D	= 1522,
    HADD_S_H	= 1523,
    HADD_S_W	= 1524,
    HADD_U_D	= 1525,
    HADD_U_H	= 1526,
    HADD_U_W	= 1527,
    HSUB_S_D	= 1528,
    HSUB_S_H	= 1529,
    HSUB_S_W	= 1530,
    HSUB_U_D	= 1531,
    HSUB_U_H	= 1532,
    HSUB_U_W	= 1533,
    HYPCALL	= 1534,
    HYPCALL_MM	= 1535,
    ILVEV_B	= 1536,
    ILVEV_D	= 1537,
    ILVEV_H	= 1538,
    ILVEV_W	= 1539,
    ILVL_B	= 1540,
    ILVL_D	= 1541,
    ILVL_H	= 1542,
    ILVL_W	= 1543,
    ILVOD_B	= 1544,
    ILVOD_D	= 1545,
    ILVOD_H	= 1546,
    ILVOD_W	= 1547,
    ILVR_B	= 1548,
    ILVR_D	= 1549,
    ILVR_H	= 1550,
    ILVR_W	= 1551,
    INS	= 1552,
    INSERT_B	= 1553,
    INSERT_D	= 1554,
    INSERT_H	= 1555,
    INSERT_W	= 1556,
    INSV	= 1557,
    INSVE_B	= 1558,
    INSVE_D	= 1559,
    INSVE_H	= 1560,
    INSVE_W	= 1561,
    INSV_MM	= 1562,
    INS_MM	= 1563,
    INS_MMR6	= 1564,
    J	= 1565,
    JAL	= 1566,
    JALR	= 1567,
    JALR16_MM	= 1568,
    JALR64	= 1569,
    JALRC16_MMR6	= 1570,
    JALRC_HB_MMR6	= 1571,
    JALRC_MMR6	= 1572,
    JALRS16_MM	= 1573,
    JALRS_MM	= 1574,
    JALR_HB	= 1575,
    JALR_HB64	= 1576,
    JALR_MM	= 1577,
    JALS_MM	= 1578,
    JALX	= 1579,
    JALX_MM	= 1580,
    JAL_MM	= 1581,
    JIALC	= 1582,
    JIALC64	= 1583,
    JIALC_MMR6	= 1584,
    JIC	= 1585,
    JIC64	= 1586,
    JIC_MMR6	= 1587,
    JR	= 1588,
    JR16_MM	= 1589,
    JR64	= 1590,
    JRADDIUSP	= 1591,
    JRC16_MM	= 1592,
    JRC16_MMR6	= 1593,
    JRCADDIUSP_MMR6	= 1594,
    JR_HB	= 1595,
    JR_HB64	= 1596,
    JR_HB64_R6	= 1597,
    JR_HB_R6	= 1598,
    JR_MM	= 1599,
    J_MM	= 1600,
    Jal16	= 1601,
    JalB16	= 1602,
    JrRa16	= 1603,
    JrcRa16	= 1604,
    JrcRx16	= 1605,
    JumpLinkReg16	= 1606,
    LB	= 1607,
    LB64	= 1608,
    LBE	= 1609,
    LBE_MM	= 1610,
    LBU16_MM	= 1611,
    LBUX	= 1612,
    LBUX_MM	= 1613,
    LBU_MMR6	= 1614,
    LB_MM	= 1615,
    LB_MMR6	= 1616,
    LBu	= 1617,
    LBu64	= 1618,
    LBuE	= 1619,
    LBuE_MM	= 1620,
    LBu_MM	= 1621,
    LD	= 1622,
    LDC1	= 1623,
    LDC164	= 1624,
    LDC1_D64_MMR6	= 1625,
    LDC1_MM	= 1626,
    LDC2	= 1627,
    LDC2_MMR6	= 1628,
    LDC2_R6	= 1629,
    LDC3	= 1630,
    LDI_B	= 1631,
    LDI_D	= 1632,
    LDI_H	= 1633,
    LDI_W	= 1634,
    LDL	= 1635,
    LDPC	= 1636,
    LDR	= 1637,
    LDXC1	= 1638,
    LDXC164	= 1639,
    LD_B	= 1640,
    LD_D	= 1641,
    LD_H	= 1642,
    LD_W	= 1643,
    LEA_ADDiu	= 1644,
    LEA_ADDiu64	= 1645,
    LEA_ADDiu_MM	= 1646,
    LH	= 1647,
    LH64	= 1648,
    LHE	= 1649,
    LHE_MM	= 1650,
    LHU16_MM	= 1651,
    LHX	= 1652,
    LHX_MM	= 1653,
    LH_MM	= 1654,
    LHu	= 1655,
    LHu64	= 1656,
    LHuE	= 1657,
    LHuE_MM	= 1658,
    LHu_MM	= 1659,
    LI16_MM	= 1660,
    LI16_MMR6	= 1661,
    LL	= 1662,
    LL64	= 1663,
    LL64_R6	= 1664,
    LLD	= 1665,
    LLD_R6	= 1666,
    LLE	= 1667,
    LLE_MM	= 1668,
    LL_MM	= 1669,
    LL_MMR6	= 1670,
    LL_R6	= 1671,
    LSA	= 1672,
    LSA_MMR6	= 1673,
    LSA_R6	= 1674,
    LUI_MMR6	= 1675,
    LUXC1	= 1676,
    LUXC164	= 1677,
    LUXC1_MM	= 1678,
    LUi	= 1679,
    LUi64	= 1680,
    LUi_MM	= 1681,
    LW	= 1682,
    LW16_MM	= 1683,
    LW64	= 1684,
    LWC1	= 1685,
    LWC1_MM	= 1686,
    LWC2	= 1687,
    LWC2_MMR6	= 1688,
    LWC2_R6	= 1689,
    LWC3	= 1690,
    LWDSP	= 1691,
    LWDSP_MM	= 1692,
    LWE	= 1693,
    LWE_MM	= 1694,
    LWGP_MM	= 1695,
    LWL	= 1696,
    LWL64	= 1697,
    LWLE	= 1698,
    LWLE_MM	= 1699,
    LWL_MM	= 1700,
    LWM16_MM	= 1701,
    LWM16_MMR6	= 1702,
    LWM32_MM	= 1703,
    LWPC	= 1704,
    LWPC_MMR6	= 1705,
    LWP_MM	= 1706,
    LWR	= 1707,
    LWR64	= 1708,
    LWRE	= 1709,
    LWRE_MM	= 1710,
    LWR_MM	= 1711,
    LWSP_MM	= 1712,
    LWUPC	= 1713,
    LWU_MM	= 1714,
    LWX	= 1715,
    LWXC1	= 1716,
    LWXC1_MM	= 1717,
    LWXS_MM	= 1718,
    LWX_MM	= 1719,
    LW_MM	= 1720,
    LW_MMR6	= 1721,
    LWu	= 1722,
    LbRxRyOffMemX16	= 1723,
    LbuRxRyOffMemX16	= 1724,
    LhRxRyOffMemX16	= 1725,
    LhuRxRyOffMemX16	= 1726,
    LiRxImm16	= 1727,
    LiRxImmAlignX16	= 1728,
    LiRxImmX16	= 1729,
    LwRxPcTcp16	= 1730,
    LwRxPcTcpX16	= 1731,
    LwRxRyOffMemX16	= 1732,
    LwRxSpImmX16	= 1733,
    MADD	= 1734,
    MADDF_D	= 1735,
    MADDF_D_MMR6	= 1736,
    MADDF_S	= 1737,
    MADDF_S_MMR6	= 1738,
    MADDR_Q_H	= 1739,
    MADDR_Q_W	= 1740,
    MADDU	= 1741,
    MADDU_DSP	= 1742,
    MADDU_DSP_MM	= 1743,
    MADDU_MM	= 1744,
    MADDV_B	= 1745,
    MADDV_D	= 1746,
    MADDV_H	= 1747,
    MADDV_W	= 1748,
    MADD_D32	= 1749,
    MADD_D32_MM	= 1750,
    MADD_D64	= 1751,
    MADD_DSP	= 1752,
    MADD_DSP_MM	= 1753,
    MADD_MM	= 1754,
    MADD_Q_H	= 1755,
    MADD_Q_W	= 1756,
    MADD_S	= 1757,
    MADD_S_MM	= 1758,
    MAQ_SA_W_PHL	= 1759,
    MAQ_SA_W_PHL_MM	= 1760,
    MAQ_SA_W_PHR	= 1761,
    MAQ_SA_W_PHR_MM	= 1762,
    MAQ_S_W_PHL	= 1763,
    MAQ_S_W_PHL_MM	= 1764,
    MAQ_S_W_PHR	= 1765,
    MAQ_S_W_PHR_MM	= 1766,
    MAXA_D	= 1767,
    MAXA_D_MMR6	= 1768,
    MAXA_S	= 1769,
    MAXA_S_MMR6	= 1770,
    MAXI_S_B	= 1771,
    MAXI_S_D	= 1772,
    MAXI_S_H	= 1773,
    MAXI_S_W	= 1774,
    MAXI_U_B	= 1775,
    MAXI_U_D	= 1776,
    MAXI_U_H	= 1777,
    MAXI_U_W	= 1778,
    MAX_A_B	= 1779,
    MAX_A_D	= 1780,
    MAX_A_H	= 1781,
    MAX_A_W	= 1782,
    MAX_D	= 1783,
    MAX_D_MMR6	= 1784,
    MAX_S	= 1785,
    MAX_S_B	= 1786,
    MAX_S_D	= 1787,
    MAX_S_H	= 1788,
    MAX_S_MMR6	= 1789,
    MAX_S_W	= 1790,
    MAX_U_B	= 1791,
    MAX_U_D	= 1792,
    MAX_U_H	= 1793,
    MAX_U_W	= 1794,
    MFC0	= 1795,
    MFC0_MMR6	= 1796,
    MFC1	= 1797,
    MFC1_D64	= 1798,
    MFC1_MM	= 1799,
    MFC1_MMR6	= 1800,
    MFC2	= 1801,
    MFC2_MMR6	= 1802,
    MFGC0	= 1803,
    MFGC0_MM	= 1804,
    MFHC0_MMR6	= 1805,
    MFHC1_D32	= 1806,
    MFHC1_D32_MM	= 1807,
    MFHC1_D64	= 1808,
    MFHC1_D64_MM	= 1809,
    MFHC2_MMR6	= 1810,
    MFHGC0	= 1811,
    MFHGC0_MM	= 1812,
    MFHI	= 1813,
    MFHI16_MM	= 1814,
    MFHI64	= 1815,
    MFHI_DSP	= 1816,
    MFHI_DSP_MM	= 1817,
    MFHI_MM	= 1818,
    MFLO	= 1819,
    MFLO16_MM	= 1820,
    MFLO64	= 1821,
    MFLO_DSP	= 1822,
    MFLO_DSP_MM	= 1823,
    MFLO_MM	= 1824,
    MFTR	= 1825,
    MINA_D	= 1826,
    MINA_D_MMR6	= 1827,
    MINA_S	= 1828,
    MINA_S_MMR6	= 1829,
    MINI_S_B	= 1830,
    MINI_S_D	= 1831,
    MINI_S_H	= 1832,
    MINI_S_W	= 1833,
    MINI_U_B	= 1834,
    MINI_U_D	= 1835,
    MINI_U_H	= 1836,
    MINI_U_W	= 1837,
    MIN_A_B	= 1838,
    MIN_A_D	= 1839,
    MIN_A_H	= 1840,
    MIN_A_W	= 1841,
    MIN_D	= 1842,
    MIN_D_MMR6	= 1843,
    MIN_S	= 1844,
    MIN_S_B	= 1845,
    MIN_S_D	= 1846,
    MIN_S_H	= 1847,
    MIN_S_MMR6	= 1848,
    MIN_S_W	= 1849,
    MIN_U_B	= 1850,
    MIN_U_D	= 1851,
    MIN_U_H	= 1852,
    MIN_U_W	= 1853,
    MOD	= 1854,
    MODSUB	= 1855,
    MODSUB_MM	= 1856,
    MODU	= 1857,
    MODU_MMR6	= 1858,
    MOD_MMR6	= 1859,
    MOD_S_B	= 1860,
    MOD_S_D	= 1861,
    MOD_S_H	= 1862,
    MOD_S_W	= 1863,
    MOD_U_B	= 1864,
    MOD_U_D	= 1865,
    MOD_U_H	= 1866,
    MOD_U_W	= 1867,
    MOVE16_MM	= 1868,
    MOVE16_MMR6	= 1869,
    MOVEP_MM	= 1870,
    MOVEP_MMR6	= 1871,
    MOVE_V	= 1872,
    MOVF_D32	= 1873,
    MOVF_D32_MM	= 1874,
    MOVF_D64	= 1875,
    MOVF_I	= 1876,
    MOVF_I64	= 1877,
    MOVF_I_MM	= 1878,
    MOVF_S	= 1879,
    MOVF_S_MM	= 1880,
    MOVN_I64_D64	= 1881,
    MOVN_I64_I	= 1882,
    MOVN_I64_I64	= 1883,
    MOVN_I64_S	= 1884,
    MOVN_I_D32	= 1885,
    MOVN_I_D32_MM	= 1886,
    MOVN_I_D64	= 1887,
    MOVN_I_I	= 1888,
    MOVN_I_I64	= 1889,
    MOVN_I_MM	= 1890,
    MOVN_I_S	= 1891,
    MOVN_I_S_MM	= 1892,
    MOVT_D32	= 1893,
    MOVT_D32_MM	= 1894,
    MOVT_D64	= 1895,
    MOVT_I	= 1896,
    MOVT_I64	= 1897,
    MOVT_I_MM	= 1898,
    MOVT_S	= 1899,
    MOVT_S_MM	= 1900,
    MOVZ_I64_D64	= 1901,
    MOVZ_I64_I	= 1902,
    MOVZ_I64_I64	= 1903,
    MOVZ_I64_S	= 1904,
    MOVZ_I_D32	= 1905,
    MOVZ_I_D32_MM	= 1906,
    MOVZ_I_D64	= 1907,
    MOVZ_I_I	= 1908,
    MOVZ_I_I64	= 1909,
    MOVZ_I_MM	= 1910,
    MOVZ_I_S	= 1911,
    MOVZ_I_S_MM	= 1912,
    MSUB	= 1913,
    MSUBF_D	= 1914,
    MSUBF_D_MMR6	= 1915,
    MSUBF_S	= 1916,
    MSUBF_S_MMR6	= 1917,
    MSUBR_Q_H	= 1918,
    MSUBR_Q_W	= 1919,
    MSUBU	= 1920,
    MSUBU_DSP	= 1921,
    MSUBU_DSP_MM	= 1922,
    MSUBU_MM	= 1923,
    MSUBV_B	= 1924,
    MSUBV_D	= 1925,
    MSUBV_H	= 1926,
    MSUBV_W	= 1927,
    MSUB_D32	= 1928,
    MSUB_D32_MM	= 1929,
    MSUB_D64	= 1930,
    MSUB_DSP	= 1931,
    MSUB_DSP_MM	= 1932,
    MSUB_MM	= 1933,
    MSUB_Q_H	= 1934,
    MSUB_Q_W	= 1935,
    MSUB_S	= 1936,
    MSUB_S_MM	= 1937,
    MTC0	= 1938,
    MTC0_MMR6	= 1939,
    MTC1	= 1940,
    MTC1_D64	= 1941,
    MTC1_MM	= 1942,
    MTC1_MMR6	= 1943,
    MTC2	= 1944,
    MTC2_MMR6	= 1945,
    MTGC0	= 1946,
    MTGC0_MM	= 1947,
    MTHC0_MMR6	= 1948,
    MTHC1_D32	= 1949,
    MTHC1_D32_MM	= 1950,
    MTHC1_D64	= 1951,
    MTHC1_D64_MM	= 1952,
    MTHC2_MMR6	= 1953,
    MTHGC0	= 1954,
    MTHGC0_MM	= 1955,
    MTHI	= 1956,
    MTHI64	= 1957,
    MTHI_DSP	= 1958,
    MTHI_DSP_MM	= 1959,
    MTHI_MM	= 1960,
    MTHLIP	= 1961,
    MTHLIP_MM	= 1962,
    MTLO	= 1963,
    MTLO64	= 1964,
    MTLO_DSP	= 1965,
    MTLO_DSP_MM	= 1966,
    MTLO_MM	= 1967,
    MTM0	= 1968,
    MTM1	= 1969,
    MTM2	= 1970,
    MTP0	= 1971,
    MTP1	= 1972,
    MTP2	= 1973,
    MTTR	= 1974,
    MUH	= 1975,
    MUHU	= 1976,
    MUHU_MMR6	= 1977,
    MUH_MMR6	= 1978,
    MUL	= 1979,
    MULEQ_S_W_PHL	= 1980,
    MULEQ_S_W_PHL_MM	= 1981,
    MULEQ_S_W_PHR	= 1982,
    MULEQ_S_W_PHR_MM	= 1983,
    MULEU_S_PH_QBL	= 1984,
    MULEU_S_PH_QBL_MM	= 1985,
    MULEU_S_PH_QBR	= 1986,
    MULEU_S_PH_QBR_MM	= 1987,
    MULQ_RS_PH	= 1988,
    MULQ_RS_PH_MM	= 1989,
    MULQ_RS_W	= 1990,
    MULQ_RS_W_MMR2	= 1991,
    MULQ_S_PH	= 1992,
    MULQ_S_PH_MMR2	= 1993,
    MULQ_S_W	= 1994,
    MULQ_S_W_MMR2	= 1995,
    MULR_Q_H	= 1996,
    MULR_Q_W	= 1997,
    MULSAQ_S_W_PH	= 1998,
    MULSAQ_S_W_PH_MM	= 1999,
    MULSA_W_PH	= 2000,
    MULSA_W_PH_MMR2	= 2001,
    MULT	= 2002,
    MULTU_DSP	= 2003,
    MULTU_DSP_MM	= 2004,
    MULT_DSP	= 2005,
    MULT_DSP_MM	= 2006,
    MULT_MM	= 2007,
    MULTu	= 2008,
    MULTu_MM	= 2009,
    MULU	= 2010,
    MULU_MMR6	= 2011,
    MULV_B	= 2012,
    MULV_D	= 2013,
    MULV_H	= 2014,
    MULV_W	= 2015,
    MUL_MM	= 2016,
    MUL_MMR6	= 2017,
    MUL_PH	= 2018,
    MUL_PH_MMR2	= 2019,
    MUL_Q_H	= 2020,
    MUL_Q_W	= 2021,
    MUL_R6	= 2022,
    MUL_S_PH	= 2023,
    MUL_S_PH_MMR2	= 2024,
    Mfhi16	= 2025,
    Mflo16	= 2026,
    Move32R16	= 2027,
    MoveR3216	= 2028,
    NLOC_B	= 2029,
    NLOC_D	= 2030,
    NLOC_H	= 2031,
    NLOC_W	= 2032,
    NLZC_B	= 2033,
    NLZC_D	= 2034,
    NLZC_H	= 2035,
    NLZC_W	= 2036,
    NMADD_D32	= 2037,
    NMADD_D32_MM	= 2038,
    NMADD_D64	= 2039,
    NMADD_S	= 2040,
    NMADD_S_MM	= 2041,
    NMSUB_D32	= 2042,
    NMSUB_D32_MM	= 2043,
    NMSUB_D64	= 2044,
    NMSUB_S	= 2045,
    NMSUB_S_MM	= 2046,
    NOR	= 2047,
    NOR64	= 2048,
    NORI_B	= 2049,
    NOR_MM	= 2050,
    NOR_MMR6	= 2051,
    NOR_V	= 2052,
    NOT16_MM	= 2053,
    NOT16_MMR6	= 2054,
    NegRxRy16	= 2055,
    NotRxRy16	= 2056,
    OR	= 2057,
    OR16_MM	= 2058,
    OR16_MMR6	= 2059,
    OR64	= 2060,
    ORI_B	= 2061,
    ORI_MMR6	= 2062,
    OR_MM	= 2063,
    OR_MMR6	= 2064,
    OR_V	= 2065,
    ORi	= 2066,
    ORi64	= 2067,
    ORi_MM	= 2068,
    OrRxRxRy16	= 2069,
    PACKRL_PH	= 2070,
    PACKRL_PH_MM	= 2071,
    PAUSE	= 2072,
    PAUSE_MM	= 2073,
    PAUSE_MMR6	= 2074,
    PCKEV_B	= 2075,
    PCKEV_D	= 2076,
    PCKEV_H	= 2077,
    PCKEV_W	= 2078,
    PCKOD_B	= 2079,
    PCKOD_D	= 2080,
    PCKOD_H	= 2081,
    PCKOD_W	= 2082,
    PCNT_B	= 2083,
    PCNT_D	= 2084,
    PCNT_H	= 2085,
    PCNT_W	= 2086,
    PICK_PH	= 2087,
    PICK_PH_MM	= 2088,
    PICK_QB	= 2089,
    PICK_QB_MM	= 2090,
    POP	= 2091,
    PRECEQU_PH_QBL	= 2092,
    PRECEQU_PH_QBLA	= 2093,
    PRECEQU_PH_QBLA_MM	= 2094,
    PRECEQU_PH_QBL_MM	= 2095,
    PRECEQU_PH_QBR	= 2096,
    PRECEQU_PH_QBRA	= 2097,
    PRECEQU_PH_QBRA_MM	= 2098,
    PRECEQU_PH_QBR_MM	= 2099,
    PRECEQ_W_PHL	= 2100,
    PRECEQ_W_PHL_MM	= 2101,
    PRECEQ_W_PHR	= 2102,
    PRECEQ_W_PHR_MM	= 2103,
    PRECEU_PH_QBL	= 2104,
    PRECEU_PH_QBLA	= 2105,
    PRECEU_PH_QBLA_MM	= 2106,
    PRECEU_PH_QBL_MM	= 2107,
    PRECEU_PH_QBR	= 2108,
    PRECEU_PH_QBRA	= 2109,
    PRECEU_PH_QBRA_MM	= 2110,
    PRECEU_PH_QBR_MM	= 2111,
    PRECRQU_S_QB_PH	= 2112,
    PRECRQU_S_QB_PH_MM	= 2113,
    PRECRQ_PH_W	= 2114,
    PRECRQ_PH_W_MM	= 2115,
    PRECRQ_QB_PH	= 2116,
    PRECRQ_QB_PH_MM	= 2117,
    PRECRQ_RS_PH_W	= 2118,
    PRECRQ_RS_PH_W_MM	= 2119,
    PRECR_QB_PH	= 2120,
    PRECR_QB_PH_MMR2	= 2121,
    PRECR_SRA_PH_W	= 2122,
    PRECR_SRA_PH_W_MMR2	= 2123,
    PRECR_SRA_R_PH_W	= 2124,
    PRECR_SRA_R_PH_W_MMR2	= 2125,
    PREF	= 2126,
    PREFE	= 2127,
    PREFE_MM	= 2128,
    PREFX_MM	= 2129,
    PREF_MM	= 2130,
    PREF_MMR6	= 2131,
    PREF_R6	= 2132,
    PREPEND	= 2133,
    PREPEND_MMR2	= 2134,
    RADDU_W_QB	= 2135,
    RADDU_W_QB_MM	= 2136,
    RDDSP	= 2137,
    RDDSP_MM	= 2138,
    RDHWR	= 2139,
    RDHWR64	= 2140,
    RDHWR_MM	= 2141,
    RDHWR_MMR6	= 2142,
    RDPGPR_MMR6	= 2143,
    RECIP_D32	= 2144,
    RECIP_D32_MM	= 2145,
    RECIP_D64	= 2146,
    RECIP_D64_MM	= 2147,
    RECIP_S	= 2148,
    RECIP_S_MM	= 2149,
    REPLV_PH	= 2150,
    REPLV_PH_MM	= 2151,
    REPLV_QB	= 2152,
    REPLV_QB_MM	= 2153,
    REPL_PH	= 2154,
    REPL_PH_MM	= 2155,
    REPL_QB	= 2156,
    REPL_QB_MM	= 2157,
    RINT_D	= 2158,
    RINT_D_MMR6	= 2159,
    RINT_S	= 2160,
    RINT_S_MMR6	= 2161,
    ROTR	= 2162,
    ROTRV	= 2163,
    ROTRV_MM	= 2164,
    ROTR_MM	= 2165,
    ROUND_L_D64	= 2166,
    ROUND_L_D_MMR6	= 2167,
    ROUND_L_S	= 2168,
    ROUND_L_S_MMR6	= 2169,
    ROUND_W_D32	= 2170,
    ROUND_W_D64	= 2171,
    ROUND_W_D_MMR6	= 2172,
    ROUND_W_MM	= 2173,
    ROUND_W_S	= 2174,
    ROUND_W_S_MM	= 2175,
    ROUND_W_S_MMR6	= 2176,
    RSQRT_D32	= 2177,
    RSQRT_D32_MM	= 2178,
    RSQRT_D64	= 2179,
    RSQRT_D64_MM	= 2180,
    RSQRT_S	= 2181,
    RSQRT_S_MM	= 2182,
    Restore16	= 2183,
    RestoreX16	= 2184,
    SAT_S_B	= 2185,
    SAT_S_D	= 2186,
    SAT_S_H	= 2187,
    SAT_S_W	= 2188,
    SAT_U_B	= 2189,
    SAT_U_D	= 2190,
    SAT_U_H	= 2191,
    SAT_U_W	= 2192,
    SB	= 2193,
    SB16_MM	= 2194,
    SB16_MMR6	= 2195,
    SB64	= 2196,
    SBE	= 2197,
    SBE_MM	= 2198,
    SB_MM	= 2199,
    SB_MMR6	= 2200,
    SC	= 2201,
    SC64	= 2202,
    SC64_R6	= 2203,
    SCD	= 2204,
    SCD_R6	= 2205,
    SCE	= 2206,
    SCE_MM	= 2207,
    SC_MM	= 2208,
    SC_MMR6	= 2209,
    SC_R6	= 2210,
    SD	= 2211,
    SDBBP	= 2212,
    SDBBP16_MM	= 2213,
    SDBBP16_MMR6	= 2214,
    SDBBP_MM	= 2215,
    SDBBP_MMR6	= 2216,
    SDBBP_R6	= 2217,
    SDC1	= 2218,
    SDC164	= 2219,
    SDC1_D64_MMR6	= 2220,
    SDC1_MM	= 2221,
    SDC2	= 2222,
    SDC2_MMR6	= 2223,
    SDC2_R6	= 2224,
    SDC3	= 2225,
    SDIV	= 2226,
    SDIV_MM	= 2227,
    SDL	= 2228,
    SDR	= 2229,
    SDXC1	= 2230,
    SDXC164	= 2231,
    SEB	= 2232,
    SEB64	= 2233,
    SEB_MM	= 2234,
    SEH	= 2235,
    SEH64	= 2236,
    SEH_MM	= 2237,
    SELEQZ	= 2238,
    SELEQZ64	= 2239,
    SELEQZ_D	= 2240,
    SELEQZ_D_MMR6	= 2241,
    SELEQZ_MMR6	= 2242,
    SELEQZ_S	= 2243,
    SELEQZ_S_MMR6	= 2244,
    SELNEZ	= 2245,
    SELNEZ64	= 2246,
    SELNEZ_D	= 2247,
    SELNEZ_D_MMR6	= 2248,
    SELNEZ_MMR6	= 2249,
    SELNEZ_S	= 2250,
    SELNEZ_S_MMR6	= 2251,
    SEL_D	= 2252,
    SEL_D_MMR6	= 2253,
    SEL_S	= 2254,
    SEL_S_MMR6	= 2255,
    SEQ	= 2256,
    SEQi	= 2257,
    SH	= 2258,
    SH16_MM	= 2259,
    SH16_MMR6	= 2260,
    SH64	= 2261,
    SHE	= 2262,
    SHE_MM	= 2263,
    SHF_B	= 2264,
    SHF_H	= 2265,
    SHF_W	= 2266,
    SHILO	= 2267,
    SHILOV	= 2268,
    SHILOV_MM	= 2269,
    SHILO_MM	= 2270,
    SHLLV_PH	= 2271,
    SHLLV_PH_MM	= 2272,
    SHLLV_QB	= 2273,
    SHLLV_QB_MM	= 2274,
    SHLLV_S_PH	= 2275,
    SHLLV_S_PH_MM	= 2276,
    SHLLV_S_W	= 2277,
    SHLLV_S_W_MM	= 2278,
    SHLL_PH	= 2279,
    SHLL_PH_MM	= 2280,
    SHLL_QB	= 2281,
    SHLL_QB_MM	= 2282,
    SHLL_S_PH	= 2283,
    SHLL_S_PH_MM	= 2284,
    SHLL_S_W	= 2285,
    SHLL_S_W_MM	= 2286,
    SHRAV_PH	= 2287,
    SHRAV_PH_MM	= 2288,
    SHRAV_QB	= 2289,
    SHRAV_QB_MMR2	= 2290,
    SHRAV_R_PH	= 2291,
    SHRAV_R_PH_MM	= 2292,
    SHRAV_R_QB	= 2293,
    SHRAV_R_QB_MMR2	= 2294,
    SHRAV_R_W	= 2295,
    SHRAV_R_W_MM	= 2296,
    SHRA_PH	= 2297,
    SHRA_PH_MM	= 2298,
    SHRA_QB	= 2299,
    SHRA_QB_MMR2	= 2300,
    SHRA_R_PH	= 2301,
    SHRA_R_PH_MM	= 2302,
    SHRA_R_QB	= 2303,
    SHRA_R_QB_MMR2	= 2304,
    SHRA_R_W	= 2305,
    SHRA_R_W_MM	= 2306,
    SHRLV_PH	= 2307,
    SHRLV_PH_MMR2	= 2308,
    SHRLV_QB	= 2309,
    SHRLV_QB_MM	= 2310,
    SHRL_PH	= 2311,
    SHRL_PH_MMR2	= 2312,
    SHRL_QB	= 2313,
    SHRL_QB_MM	= 2314,
    SH_MM	= 2315,
    SH_MMR6	= 2316,
    SLDI_B	= 2317,
    SLDI_D	= 2318,
    SLDI_H	= 2319,
    SLDI_W	= 2320,
    SLD_B	= 2321,
    SLD_D	= 2322,
    SLD_H	= 2323,
    SLD_W	= 2324,
    SLL	= 2325,
    SLL16_MM	= 2326,
    SLL16_MMR6	= 2327,
    SLL64_32	= 2328,
    SLL64_64	= 2329,
    SLLI_B	= 2330,
    SLLI_D	= 2331,
    SLLI_H	= 2332,
    SLLI_W	= 2333,
    SLLV	= 2334,
    SLLV_MM	= 2335,
    SLL_B	= 2336,
    SLL_D	= 2337,
    SLL_H	= 2338,
    SLL_MM	= 2339,
    SLL_MMR6	= 2340,
    SLL_W	= 2341,
    SLT	= 2342,
    SLT64	= 2343,
    SLT_MM	= 2344,
    SLTi	= 2345,
    SLTi64	= 2346,
    SLTi_MM	= 2347,
    SLTiu	= 2348,
    SLTiu64	= 2349,
    SLTiu_MM	= 2350,
    SLTu	= 2351,
    SLTu64	= 2352,
    SLTu_MM	= 2353,
    SNE	= 2354,
    SNEi	= 2355,
    SPLATI_B	= 2356,
    SPLATI_D	= 2357,
    SPLATI_H	= 2358,
    SPLATI_W	= 2359,
    SPLAT_B	= 2360,
    SPLAT_D	= 2361,
    SPLAT_H	= 2362,
    SPLAT_W	= 2363,
    SRA	= 2364,
    SRAI_B	= 2365,
    SRAI_D	= 2366,
    SRAI_H	= 2367,
    SRAI_W	= 2368,
    SRARI_B	= 2369,
    SRARI_D	= 2370,
    SRARI_H	= 2371,
    SRARI_W	= 2372,
    SRAR_B	= 2373,
    SRAR_D	= 2374,
    SRAR_H	= 2375,
    SRAR_W	= 2376,
    SRAV	= 2377,
    SRAV_MM	= 2378,
    SRA_B	= 2379,
    SRA_D	= 2380,
    SRA_H	= 2381,
    SRA_MM	= 2382,
    SRA_W	= 2383,
    SRL	= 2384,
    SRL16_MM	= 2385,
    SRL16_MMR6	= 2386,
    SRLI_B	= 2387,
    SRLI_D	= 2388,
    SRLI_H	= 2389,
    SRLI_W	= 2390,
    SRLRI_B	= 2391,
    SRLRI_D	= 2392,
    SRLRI_H	= 2393,
    SRLRI_W	= 2394,
    SRLR_B	= 2395,
    SRLR_D	= 2396,
    SRLR_H	= 2397,
    SRLR_W	= 2398,
    SRLV	= 2399,
    SRLV_MM	= 2400,
    SRL_B	= 2401,
    SRL_D	= 2402,
    SRL_H	= 2403,
    SRL_MM	= 2404,
    SRL_W	= 2405,
    SSNOP	= 2406,
    SSNOP_MM	= 2407,
    SSNOP_MMR6	= 2408,
    ST_B	= 2409,
    ST_D	= 2410,
    ST_H	= 2411,
    ST_W	= 2412,
    SUB	= 2413,
    SUBQH_PH	= 2414,
    SUBQH_PH_MMR2	= 2415,
    SUBQH_R_PH	= 2416,
    SUBQH_R_PH_MMR2	= 2417,
    SUBQH_R_W	= 2418,
    SUBQH_R_W_MMR2	= 2419,
    SUBQH_W	= 2420,
    SUBQH_W_MMR2	= 2421,
    SUBQ_PH	= 2422,
    SUBQ_PH_MM	= 2423,
    SUBQ_S_PH	= 2424,
    SUBQ_S_PH_MM	= 2425,
    SUBQ_S_W	= 2426,
    SUBQ_S_W_MM	= 2427,
    SUBSUS_U_B	= 2428,
    SUBSUS_U_D	= 2429,
    SUBSUS_U_H	= 2430,
    SUBSUS_U_W	= 2431,
    SUBSUU_S_B	= 2432,
    SUBSUU_S_D	= 2433,
    SUBSUU_S_H	= 2434,
    SUBSUU_S_W	= 2435,
    SUBS_S_B	= 2436,
    SUBS_S_D	= 2437,
    SUBS_S_H	= 2438,
    SUBS_S_W	= 2439,
    SUBS_U_B	= 2440,
    SUBS_U_D	= 2441,
    SUBS_U_H	= 2442,
    SUBS_U_W	= 2443,
    SUBU16_MM	= 2444,
    SUBU16_MMR6	= 2445,
    SUBUH_QB	= 2446,
    SUBUH_QB_MMR2	= 2447,
    SUBUH_R_QB	= 2448,
    SUBUH_R_QB_MMR2	= 2449,
    SUBU_MMR6	= 2450,
    SUBU_PH	= 2451,
    SUBU_PH_MMR2	= 2452,
    SUBU_QB	= 2453,
    SUBU_QB_MM	= 2454,
    SUBU_S_PH	= 2455,
    SUBU_S_PH_MMR2	= 2456,
    SUBU_S_QB	= 2457,
    SUBU_S_QB_MM	= 2458,
    SUBVI_B	= 2459,
    SUBVI_D	= 2460,
    SUBVI_H	= 2461,
    SUBVI_W	= 2462,
    SUBV_B	= 2463,
    SUBV_D	= 2464,
    SUBV_H	= 2465,
    SUBV_W	= 2466,
    SUB_MM	= 2467,
    SUB_MMR6	= 2468,
    SUBu	= 2469,
    SUBu_MM	= 2470,
    SUXC1	= 2471,
    SUXC164	= 2472,
    SUXC1_MM	= 2473,
    SW	= 2474,
    SW16_MM	= 2475,
    SW16_MMR6	= 2476,
    SW64	= 2477,
    SWC1	= 2478,
    SWC1_MM	= 2479,
    SWC2	= 2480,
    SWC2_MMR6	= 2481,
    SWC2_R6	= 2482,
    SWC3	= 2483,
    SWDSP	= 2484,
    SWDSP_MM	= 2485,
    SWE	= 2486,
    SWE_MM	= 2487,
    SWL	= 2488,
    SWL64	= 2489,
    SWLE	= 2490,
    SWLE_MM	= 2491,
    SWL_MM	= 2492,
    SWM16_MM	= 2493,
    SWM16_MMR6	= 2494,
    SWM32_MM	= 2495,
    SWP_MM	= 2496,
    SWR	= 2497,
    SWR64	= 2498,
    SWRE	= 2499,
    SWRE_MM	= 2500,
    SWR_MM	= 2501,
    SWSP_MM	= 2502,
    SWSP_MMR6	= 2503,
    SWXC1	= 2504,
    SWXC1_MM	= 2505,
    SW_MM	= 2506,
    SW_MMR6	= 2507,
    SYNC	= 2508,
    SYNCI	= 2509,
    SYNCI_MM	= 2510,
    SYNCI_MMR6	= 2511,
    SYNC_MM	= 2512,
    SYNC_MMR6	= 2513,
    SYSCALL	= 2514,
    SYSCALL_MM	= 2515,
    Save16	= 2516,
    SaveX16	= 2517,
    SbRxRyOffMemX16	= 2518,
    SebRx16	= 2519,
    SehRx16	= 2520,
    ShRxRyOffMemX16	= 2521,
    SllX16	= 2522,
    SllvRxRy16	= 2523,
    SltRxRy16	= 2524,
    SltiRxImm16	= 2525,
    SltiRxImmX16	= 2526,
    SltiuRxImm16	= 2527,
    SltiuRxImmX16	= 2528,
    SltuRxRy16	= 2529,
    SraX16	= 2530,
    SravRxRy16	= 2531,
    SrlX16	= 2532,
    SrlvRxRy16	= 2533,
    SubuRxRyRz16	= 2534,
    SwRxRyOffMemX16	= 2535,
    SwRxSpImmX16	= 2536,
    TEQ	= 2537,
    TEQI	= 2538,
    TEQI_MM	= 2539,
    TEQ_MM	= 2540,
    TGE	= 2541,
    TGEI	= 2542,
    TGEIU	= 2543,
    TGEIU_MM	= 2544,
    TGEI_MM	= 2545,
    TGEU	= 2546,
    TGEU_MM	= 2547,
    TGE_MM	= 2548,
    TLBGINV	= 2549,
    TLBGINVF	= 2550,
    TLBGINVF_MM	= 2551,
    TLBGINV_MM	= 2552,
    TLBGP	= 2553,
    TLBGP_MM	= 2554,
    TLBGR	= 2555,
    TLBGR_MM	= 2556,
    TLBGWI	= 2557,
    TLBGWI_MM	= 2558,
    TLBGWR	= 2559,
    TLBGWR_MM	= 2560,
    TLBINV	= 2561,
    TLBINVF	= 2562,
    TLBINVF_MMR6	= 2563,
    TLBINV_MMR6	= 2564,
    TLBP	= 2565,
    TLBP_MM	= 2566,
    TLBR	= 2567,
    TLBR_MM	= 2568,
    TLBWI	= 2569,
    TLBWI_MM	= 2570,
    TLBWR	= 2571,
    TLBWR_MM	= 2572,
    TLT	= 2573,
    TLTI	= 2574,
    TLTIU_MM	= 2575,
    TLTI_MM	= 2576,
    TLTU	= 2577,
    TLTU_MM	= 2578,
    TLT_MM	= 2579,
    TNE	= 2580,
    TNEI	= 2581,
    TNEI_MM	= 2582,
    TNE_MM	= 2583,
    TRUNC_L_D64	= 2584,
    TRUNC_L_D_MMR6	= 2585,
    TRUNC_L_S	= 2586,
    TRUNC_L_S_MMR6	= 2587,
    TRUNC_W_D32	= 2588,
    TRUNC_W_D64	= 2589,
    TRUNC_W_D_MMR6	= 2590,
    TRUNC_W_MM	= 2591,
    TRUNC_W_S	= 2592,
    TRUNC_W_S_MM	= 2593,
    TRUNC_W_S_MMR6	= 2594,
    TTLTIU	= 2595,
    UDIV	= 2596,
    UDIV_MM	= 2597,
    V3MULU	= 2598,
    VMM0	= 2599,
    VMULU	= 2600,
    VSHF_B	= 2601,
    VSHF_D	= 2602,
    VSHF_H	= 2603,
    VSHF_W	= 2604,
    WAIT	= 2605,
    WAIT_MM	= 2606,
    WAIT_MMR6	= 2607,
    WRDSP	= 2608,
    WRDSP_MM	= 2609,
    WRPGPR_MMR6	= 2610,
    WSBH	= 2611,
    WSBH_MM	= 2612,
    WSBH_MMR6	= 2613,
    XOR	= 2614,
    XOR16_MM	= 2615,
    XOR16_MMR6	= 2616,
    XOR64	= 2617,
    XORI_B	= 2618,
    XORI_MMR6	= 2619,
    XOR_MM	= 2620,
    XOR_MMR6	= 2621,
    XOR_V	= 2622,
    XORi	= 2623,
    XORi64	= 2624,
    XORi_MM	= 2625,
    XorRxRxRy16	= 2626,
    YIELD	= 2627,
    INSTRUCTION_LIST_END = 2628
  };

} // end Mips namespace
} // end llvm namespace
#endif // GET_INSTRINFO_ENUM

#ifdef GET_INSTRINFO_SCHED_ENUM
#undef GET_INSTRINFO_SCHED_ENUM
namespace llvm {

namespace Mips {
namespace Sched {
  enum {
    NoInstrModel	= 0,
    IIPseudo	= 1,
    II_B	= 2,
    II_BCCZAL	= 3,
    II_MTC1	= 4,
    II_MFC1	= 5,
    II_JALR	= 6,
    II_CVT	= 7,
    II_DMULT	= 8,
    II_DMULTU	= 9,
    II_DDIV	= 10,
    II_DDIVU	= 11,
    II_IndirectBranchPseudo	= 12,
    II_MADD	= 13,
    II_MADDU	= 14,
    II_MFHI_MFLO	= 15,
    II_MSUB	= 16,
    II_MSUBU	= 17,
    II_MTHI_MTLO	= 18,
    II_MULT	= 19,
    II_MULTU	= 20,
    II_ReturnPseudo	= 21,
    II_DIV	= 22,
    II_DIVU	= 23,
    II_J	= 24,
    II_JR	= 25,
    II_TRAP	= 26,
    II_ADD	= 27,
    II_ADDIUPC	= 28,
    II_ADDIU	= 29,
    II_ADDU	= 30,
    II_ADDI	= 31,
    II_ALIGN	= 32,
    II_ALUIPC	= 33,
    II_AND	= 34,
    II_ANDI	= 35,
    II_AUI	= 36,
    II_AUIPC	= 37,
    IIM16Alu	= 38,
    II_BADDU	= 39,
    II_BC	= 40,
    II_BALC	= 41,
    II_BBIT	= 42,
    II_BC1CCZ	= 43,
    II_BC1F	= 44,
    II_BC1FL	= 45,
    II_BC1T	= 46,
    II_BC1TL	= 47,
    II_BC2CCZ	= 48,
    II_BCC	= 49,
    II_BCCC	= 50,
    II_BCCZ	= 51,
    II_BCCZC	= 52,
    II_BCCZALS	= 53,
    II_BITSWAP	= 54,
    II_BREAK	= 55,
    II_CACHE	= 56,
    II_CACHEE	= 57,
    II_CEIL	= 58,
    II_CFC1	= 59,
    II_CFC2	= 60,
    II_INS	= 61,
    II_CLASS_D	= 62,
    II_CLASS_S	= 63,
    II_CLO	= 64,
    II_CLZ	= 65,
    II_CMP_CC_D	= 66,
    II_CMP_CC_S	= 67,
    II_CRC32B	= 68,
    II_CRC32CB	= 69,
    II_CRC32CD	= 70,
    II_CRC32CH	= 71,
    II_CRC32CW	= 72,
    II_CRC32D	= 73,
    II_CRC32H	= 74,
    II_CRC32W	= 75,
    II_CTC1	= 76,
    II_CTC2	= 77,
    II_C_CC_D	= 78,
    II_C_CC_S	= 79,
    II_DADD	= 80,
    II_DADDI	= 81,
    II_DADDIU	= 82,
    II_DADDU	= 83,
    II_DAHI	= 84,
    II_DALIGN	= 85,
    II_DATI	= 86,
    II_DAUI	= 87,
    II_DBITSWAP	= 88,
    II_DCLO	= 89,
    II_DCLZ	= 90,
    II_DERET	= 91,
    II_EXT	= 92,
    II_DI	= 93,
    II_DLSA	= 94,
    II_DMFC0	= 95,
    II_DMFC1	= 96,
    II_DMFC2	= 97,
    II_DMFGC0	= 98,
    II_DMOD	= 99,
    II_DMODU	= 100,
    II_DMT	= 101,
    II_DMTC0	= 102,
    II_DMTC1	= 103,
    II_DMTC2	= 104,
    II_DMTGC0	= 105,
    II_DMUH	= 106,
    II_DMUHU	= 107,
    II_DMUL	= 108,
    II_POP	= 109,
    II_DROTR	= 110,
    II_DROTR32	= 111,
    II_DROTRV	= 112,
    II_DSBH	= 113,
    II_DSHD	= 114,
    II_DSLL	= 115,
    II_DSLL32	= 116,
    II_DSLLV	= 117,
    II_DSRA	= 118,
    II_DSRA32	= 119,
    II_DSRAV	= 120,
    II_DSRL	= 121,
    II_DSRL32	= 122,
    II_DSRLV	= 123,
    II_DSUB	= 124,
    II_DSUBU	= 125,
    II_DVP	= 126,
    II_DVPE	= 127,
    II_EHB	= 128,
    II_EI	= 129,
    II_EMT	= 130,
    II_ERET	= 131,
    II_ERETNC	= 132,
    II_EVP	= 133,
    II_EVPE	= 134,
    II_ABS	= 135,
    II_SQRT_D	= 136,
    II_ADD_D	= 137,
    II_ADD_S	= 138,
    II_DIV_D	= 139,
    II_DIV_S	= 140,
    II_FLOOR	= 141,
    II_MOV_D	= 142,
    II_MOV_S	= 143,
    II_MUL_D	= 144,
    II_MUL_S	= 145,
    II_NEG	= 146,
    II_FORK	= 147,
    II_SQRT_S	= 148,
    II_SUB_D	= 149,
    II_SUB_S	= 150,
    II_GINVI	= 151,
    II_GINVT	= 152,
    II_HYPCALL	= 153,
    II_JAL	= 154,
    II_JALR_HB	= 155,
    II_JALRC	= 156,
    II_JALRS	= 157,
    II_JALS	= 158,
    II_JIALC	= 159,
    II_JIC	= 160,
    II_JRADDIUSP	= 161,
    II_JRC	= 162,
    II_JR_HB	= 163,
    II_LB	= 164,
    II_LBE	= 165,
    II_LBU	= 166,
    II_LBUE	= 167,
    II_LD	= 168,
    II_LDC1	= 169,
    II_LDC2	= 170,
    II_LDC3	= 171,
    II_LDL	= 172,
    II_LDPC	= 173,
    II_LDR	= 174,
    II_LDXC1	= 175,
    II_LH	= 176,
    II_LHE	= 177,
    II_LHU	= 178,
    II_LHUE	= 179,
    II_LI	= 180,
    II_LL	= 181,
    II_LLD	= 182,
    II_LLE	= 183,
    II_LSA	= 184,
    II_LUI	= 185,
    II_LUXC1	= 186,
    II_LW	= 187,
    II_LWC1	= 188,
    II_LWC2	= 189,
    II_LWC3	= 190,
    II_LWE	= 191,
    II_LWL	= 192,
    II_LWLE	= 193,
    II_LWM	= 194,
    II_LWPC	= 195,
    II_LWP	= 196,
    II_LWR	= 197,
    II_LWRE	= 198,
    II_LWUPC	= 199,
    II_LWU	= 200,
    II_LWXC1	= 201,
    II_LWXS	= 202,
    II_MADDF_D	= 203,
    II_MADDF_S	= 204,
    II_MADD_D	= 205,
    II_MADD_S	= 206,
    II_MAX_D	= 207,
    II_MAXA_D	= 208,
    II_MAX_S	= 209,
    II_MAXA_S	= 210,
    II_MFC0	= 211,
    II_MFC2	= 212,
    II_MFGC0	= 213,
    II_MFHC0	= 214,
    II_MFHC1	= 215,
    II_MFHGC0	= 216,
    II_MFTR	= 217,
    II_MIN_S	= 218,
    II_MINA_D	= 219,
    II_MIN_D	= 220,
    II_MINA_S	= 221,
    II_MOD	= 222,
    II_MODU	= 223,
    II_MOVE	= 224,
    II_MOVF_D	= 225,
    II_MOVF	= 226,
    II_MOVF_S	= 227,
    II_MOVN_D	= 228,
    II_MOVN	= 229,
    II_MOVN_S	= 230,
    II_MOVT_D	= 231,
    II_MOVT	= 232,
    II_MOVT_S	= 233,
    II_MOVZ_D	= 234,
    II_MOVZ	= 235,
    II_MOVZ_S	= 236,
    II_MSUBF_D	= 237,
    II_MSUBF_S	= 238,
    II_MSUB_D	= 239,
    II_MSUB_S	= 240,
    II_MTC0	= 241,
    II_MTC2	= 242,
    II_MTGC0	= 243,
    II_MTHC0	= 244,
    II_MTHC1	= 245,
    II_MTHGC0	= 246,
    II_MTTR	= 247,
    II_MUH	= 248,
    II_MUHU	= 249,
    II_MUL	= 250,
    II_MULU	= 251,
    II_NMADD_D	= 252,
    II_NMADD_S	= 253,
    II_NMSUB_D	= 254,
    II_NMSUB_S	= 255,
    II_NOR	= 256,
    II_NOT	= 257,
    II_OR	= 258,
    II_ORI	= 259,
    II_PAUSE	= 260,
    II_PREF	= 261,
    II_PREFE	= 262,
    II_RDHWR	= 263,
    II_RDPGPR	= 264,
    II_RECIP_D	= 265,
    II_RECIP_S	= 266,
    II_RINT_D	= 267,
    II_RINT_S	= 268,
    II_ROTR	= 269,
    II_ROTRV	= 270,
    II_ROUND	= 271,
    II_RSQRT_D	= 272,
    II_RSQRT_S	= 273,
    II_RESTORE	= 274,
    II_SB	= 275,
    II_SBE	= 276,
    II_SC	= 277,
    II_SCD	= 278,
    II_SCE	= 279,
    II_SD	= 280,
    II_SDBBP	= 281,
    II_SDC1	= 282,
    II_SDC2	= 283,
    II_SDC3	= 284,
    II_SDL	= 285,
    II_SDR	= 286,
    II_SDXC1	= 287,
    II_SEB	= 288,
    II_SEH	= 289,
    II_SELCCZ	= 290,
    II_SELCCZ_D	= 291,
    II_SELCCZ_S	= 292,
    II_SEL_D	= 293,
    II_SEL_S	= 294,
    II_SEQ_SNE	= 295,
    II_SEQI_SNEI	= 296,
    II_SH	= 297,
    II_SHE	= 298,
    II_SLL	= 299,
    II_SLLV	= 300,
    II_SLT_SLTU	= 301,
    II_SLTI_SLTIU	= 302,
    II_SRA	= 303,
    II_SRAV	= 304,
    II_SRL	= 305,
    II_SRLV	= 306,
    II_SSNOP	= 307,
    II_SUB	= 308,
    II_SUBU	= 309,
    II_SUXC1	= 310,
    II_SW	= 311,
    II_SWC1	= 312,
    II_SWC2	= 313,
    II_SWC3	= 314,
    II_SWE	= 315,
    II_SWL	= 316,
    II_SWLE	= 317,
    II_SWM	= 318,
    II_SWP	= 319,
    II_SWR	= 320,
    II_SWRE	= 321,
    II_SWXC1	= 322,
    II_SYNC	= 323,
    II_SYNCI	= 324,
    II_SYSCALL	= 325,
    II_SAVE	= 326,
    II_TEQ	= 327,
    II_TEQI	= 328,
    II_TGE	= 329,
    II_TGEI	= 330,
    II_TGEIU	= 331,
    II_TGEU	= 332,
    II_TLBGINV	= 333,
    II_TLBGINVF	= 334,
    II_TLBGP	= 335,
    II_TLBGR	= 336,
    II_TLBGWI	= 337,
    II_TLBGWR	= 338,
    II_TLBINV	= 339,
    II_TLBINVF	= 340,
    II_TLBP	= 341,
    II_TLBR	= 342,
    II_TLBWI	= 343,
    II_TLBWR	= 344,
    II_TLT	= 345,
    II_TLTI	= 346,
    II_TTLTIU	= 347,
    II_TLTU	= 348,
    II_TNE	= 349,
    II_TNEI	= 350,
    II_TRUNC	= 351,
    II_WAIT	= 352,
    II_WRPGPR	= 353,
    II_WSBH	= 354,
    II_XOR	= 355,
    II_XORI	= 356,
    II_YIELD	= 357,
    AND	= 358,
    LUi	= 359,
    NOR	= 360,
    OR	= 361,
    SLTi_SLTiu	= 362,
    SUB	= 363,
    SUBu	= 364,
    XOR	= 365,
    B	= 366,
    BAL	= 367,
    BAL_BR_BGEZAL_BGEZALL_BLTZAL_BLTZALL	= 368,
    BEQ_BEQL_BNE_BNEL	= 369,
    BGEZ_BGEZL_BGTZ_BGTZL_BLEZ_BLEZL_BLTZ_BLTZL	= 370,
    BREAK	= 371,
    DERET	= 372,
    ERET	= 373,
    ERETNC	= 374,
    J_TAILCALL	= 375,
    JR_TAILCALLREG_TAILCALLREGHB	= 376,
    JR_HB	= 377,
    PseudoIndirectBranch_PseudoIndirectHazardBranch	= 378,
    PseudoReturn	= 379,
    SDBBP	= 380,
    SSNOP	= 381,
    SYSCALL	= 382,
    TEQ	= 383,
    TEQI	= 384,
    TGE	= 385,
    TGEI	= 386,
    TGEIU	= 387,
    TGEU	= 388,
    TLT	= 389,
    TLTI	= 390,
    TLTU	= 391,
    TNE	= 392,
    TNEI	= 393,
    TRAP	= 394,
    TTLTIU	= 395,
    WAIT	= 396,
    PAUSE	= 397,
    JAL	= 398,
    JALR_JALRHBPseudo_JALRPseudo	= 399,
    JALR_HB	= 400,
    JALX	= 401,
    TLBINV	= 402,
    TLBINVF	= 403,
    TLBP	= 404,
    TLBR	= 405,
    TLBWI	= 406,
    TLBWR	= 407,
    MFC0	= 408,
    MTC0	= 409,
    MFC2	= 410,
    MTC2	= 411,
    LB	= 412,
    LBu	= 413,
    LH	= 414,
    LHu	= 415,
    LW	= 416,
    LL	= 417,
    LWC2	= 418,
    LWC3	= 419,
    LDC2	= 420,
    LDC3	= 421,
    LBE	= 422,
    LBuE	= 423,
    LHE	= 424,
    LHuE	= 425,
    LWE	= 426,
    LLE	= 427,
    LWPC	= 428,
    LWL	= 429,
    LWR	= 430,
    LWLE	= 431,
    LWRE	= 432,
    SB	= 433,
    SH	= 434,
    SW	= 435,
    SWC2	= 436,
    SWC3	= 437,
    SDC2	= 438,
    SDC3	= 439,
    SC	= 440,
    SBE	= 441,
    SHE	= 442,
    SWE	= 443,
    SCE	= 444,
    SWL	= 445,
    SWR	= 446,
    SWLE	= 447,
    SWRE	= 448,
    PREF	= 449,
    PREFE	= 450,
    CACHE	= 451,
    CACHEE	= 452,
    SYNC	= 453,
    SYNCI	= 454,
    CLO	= 455,
    CLZ	= 456,
    DI	= 457,
    EI	= 458,
    MFHI_MFLO_PseudoMFHI_PseudoMFLO	= 459,
    EHB	= 460,
    RDHWR	= 461,
    WSBH	= 462,
    MOVN_I_I	= 463,
    MOVZ_I_I	= 464,
    DIV_PseudoSDIV_SDIV	= 465,
    DIVU_PseudoUDIV_UDIV	= 466,
    MUL	= 467,
    MULT_PseudoMULT	= 468,
    MULTu_PseudoMULTu	= 469,
    MADD_PseudoMADD	= 470,
    MADDU_PseudoMADDU	= 471,
    MSUB_PseudoMSUB	= 472,
    MSUBU_PseudoMSUBU	= 473,
    MTHI_MTLO_PseudoMTLOHI	= 474,
    EXT	= 475,
    INS	= 476,
    ADD	= 477,
    ADDi	= 478,
    ADDiu	= 479,
    ANDi	= 480,
    ORi	= 481,
    ROTR	= 482,
    SEB	= 483,
    SEH	= 484,
    SLT_SLTu	= 485,
    SLL	= 486,
    SRA	= 487,
    SRL	= 488,
    XORi	= 489,
    ADDu	= 490,
    SLLV	= 491,
    SRAV	= 492,
    SRLV	= 493,
    LSA	= 494,
    COPY	= 495,
    VSHF_B_VSHF_D_VSHF_H_VSHF_W	= 496,
    BINSLI_B_BINSLI_D_BINSLI_H_BINSLI_W_BINSL_B_BINSL_D_BINSL_H_BINSL_W	= 497,
    BINSRI_B_BINSRI_D_BINSRI_H_BINSRI_W_BINSR_B_BINSR_D_BINSR_H_BINSR_W	= 498,
    INSERT_B_INSERT_D_INSERT_H_INSERT_W	= 499,
    SLDI_B_SLDI_D_SLDI_H_SLDI_W_SLD_B_SLD_D_SLD_H_SLD_W	= 500,
    BSETI_B_BSETI_D_BSETI_H_BSETI_W_BSET_B_BSET_D_BSET_H_BSET_W	= 501,
    BCLRI_B_BCLRI_D_BCLRI_H_BCLRI_W_BCLR_B_BCLR_D_BCLR_H_BCLR_W	= 502,
    BNEGI_B_BNEGI_D_BNEGI_H_BNEGI_W_BNEG_B_BNEG_D_BNEG_H_BNEG_W	= 503,
    BSELI_B_BSEL_V	= 504,
    BMNZI_B_BMNZ_V_BMZI_B_BMZ_V	= 505,
    PCNT_B_PCNT_D_PCNT_H_PCNT_W	= 506,
    SAT_S_B_SAT_S_D_SAT_S_H_SAT_S_W_SAT_U_B_SAT_U_D_SAT_U_H_SAT_U_W	= 507,
    BNZ_B_BNZ_D_BNZ_H_BNZ_V_BNZ_W_BZ_B_BZ_D_BZ_H_BZ_V_BZ_W	= 508,
    CFCMSA_CTCMSA	= 509,
    FABS_S_FABS_D32_FABS_D64	= 510,
    MOVF_D32_MOVF_D64	= 511,
    MOVF_S	= 512,
    MOVT_D32_MOVT_D64	= 513,
    MOVT_S	= 514,
    FMOV_D32_FMOV_D64	= 515,
    FMOV_S	= 516,
    FNEG_S_FNEG_D32_FNEG_D64	= 517,
    ADD_A_B_ADD_A_D_ADD_A_H_ADD_A_W	= 518,
    ADDS_A_B_ADDS_A_D_ADDS_A_H_ADDS_A_W_ADDS_S_B_ADDS_S_D_ADDS_S_H_ADDS_S_W_ADDS_U_B_ADDS_U_D_ADDS_U_H_ADDS_U_W	= 519,
    ADDVI_B_ADDVI_D_ADDVI_H_ADDVI_W_ADDV_B_ADDV_D_ADDV_H_ADDV_W	= 520,
    ASUB_S_B_ASUB_S_D_ASUB_S_H_ASUB_S_W_ASUB_U_B_ASUB_U_D_ASUB_U_H_ASUB_U_W	= 521,
    AVER_S_B_AVER_S_D_AVER_S_H_AVER_S_W_AVER_U_B_AVER_U_D_AVER_U_H_AVER_U_W_AVE_S_B_AVE_S_D_AVE_S_H_AVE_S_W_AVE_U_B_AVE_U_D_AVE_U_H_AVE_U_W	= 522,
    SHF_B_SHF_H_SHF_W	= 523,
    FILL_B_FILL_D_FILL_H_FILL_W	= 524,
    SPLATI_B_SPLATI_D_SPLATI_H_SPLATI_W_SPLAT_B_SPLAT_D_SPLAT_H_SPLAT_W	= 525,
    MOVE_V	= 526,
    LDI_B_LDI_D_LDI_H_LDI_W	= 527,
    AND_V_NOR_V_OR_V_XOR_V	= 528,
    ANDI_B_NORI_B_ORI_B_XORI_B	= 529,
    FEXP2_D_FEXP2_W	= 530,
    CLTI_S_B_CLTI_S_D_CLTI_S_H_CLTI_S_W_CLTI_U_B_CLTI_U_D_CLTI_U_H_CLTI_U_W_CLT_S_B_CLT_S_D_CLT_S_H_CLT_S_W_CLT_U_B_CLT_U_D_CLT_U_H_CLT_U_W	= 531,
    CLEI_S_B_CLEI_S_D_CLEI_S_H_CLEI_S_W_CLEI_U_B_CLEI_U_D_CLEI_U_H_CLEI_U_W_CLE_S_B_CLE_S_D_CLE_S_H_CLE_S_W_CLE_U_B_CLE_U_D_CLE_U_H_CLE_U_W	= 532,
    CEQI_B_CEQI_D_CEQI_H_CEQI_W_CEQ_B_CEQ_D_CEQ_H_CEQ_W	= 533,
    CMP_UN_D	= 534,
    CMP_UN_S	= 535,
    CMP_UEQ_D	= 536,
    CMP_UEQ_S	= 537,
    CMP_EQ_D	= 538,
    CMP_EQ_S	= 539,
    CMP_LT_D	= 540,
    CMP_LT_S	= 541,
    CMP_ULT_D	= 542,
    CMP_ULT_S	= 543,
    CMP_LE_D	= 544,
    CMP_LE_S	= 545,
    CMP_ULE_D	= 546,
    CMP_ULE_S	= 547,
    FSAF_D_FSAF_W_FSEQ_D_FSEQ_W_FSLE_D_FSLE_W_FSLT_D_FSLT_W_FSNE_D_FSNE_W_FSOR_D_FSOR_W	= 548,
    FSUEQ_D_FSUEQ_W	= 549,
    FSULE_D_FSULE_W	= 550,
    FSULT_D_FSULT_W	= 551,
    FSUNE_D_FSUNE_W	= 552,
    FSUN_D_FSUN_W	= 553,
    FCAF_D_FCAF_W	= 554,
    FCEQ_D_FCEQ_W	= 555,
    FCLE_D_FCLE_W	= 556,
    FCLT_D_FCLT_W	= 557,
    FCNE_D_FCNE_W	= 558,
    FCOR_D_FCOR_W	= 559,
    FCUEQ_D_FCUEQ_W	= 560,
    FCULE_D_FCULE_W	= 561,
    FCULT_D_FCULT_W	= 562,
    FCUNE_D_FCUNE_W	= 563,
    FCUN_D_FCUN_W	= 564,
    FABS_D_FABS_W	= 565,
    FFINT_S_D_FFINT_S_W_FFINT_U_D_FFINT_U_W	= 566,
    FFQL_D_FFQL_W	= 567,
    FFQR_D_FFQR_W	= 568,
    FTINT_S_D_FTINT_S_W_FTINT_U_D_FTINT_U_W	= 569,
    FRINT_D_FRINT_W	= 570,
    FTQ_H_FTQ_W	= 571,
    FTRUNC_S_D_FTRUNC_S_W_FTRUNC_U_D_FTRUNC_U_W	= 572,
    FEXDO_H_FEXDO_W	= 573,
    FEXUPL_D_FEXUPL_W	= 574,
    FEXUPR_D_FEXUPR_W	= 575,
    FCLASS_D_FCLASS_W	= 576,
    FMAX_A_D_FMAX_A_W	= 577,
    FMAX_D_FMAX_W	= 578,
    FMIN_A_D_FMIN_A_W	= 579,
    FMIN_D_FMIN_W	= 580,
    FLOG2_D_FLOG2_W	= 581,
    ILVL_B_ILVL_D_ILVL_H_ILVL_W_ILVR_B_ILVR_D_ILVR_H_ILVR_W	= 582,
    ILVEV_B_ILVEV_D_ILVEV_H_ILVEV_W_ILVOD_B_ILVOD_D_ILVOD_H_ILVOD_W	= 583,
    INSVE_B_INSVE_D_INSVE_H_INSVE_W	= 584,
    SUBS_S_B_SUBS_S_D_SUBS_S_H_SUBS_S_W_SUBS_U_B_SUBS_U_D_SUBS_U_H_SUBS_U_W	= 585,
    SUBSUS_U_B_SUBSUS_U_D_SUBSUS_U_H_SUBSUS_U_W	= 586,
    SUBSUU_S_B_SUBSUU_S_D_SUBSUU_S_H_SUBSUU_S_W	= 587,
    SUBVI_B_SUBVI_D_SUBVI_H_SUBVI_W	= 588,
    SUBV_B_SUBV_D_SUBV_H_SUBV_W	= 589,
    MOD_S_B_MOD_S_D_MOD_S_H_MOD_S_W_MOD_U_B_MOD_U_D_MOD_U_H_MOD_U_W	= 590,
    DIV_S_B_DIV_S_D_DIV_S_H_DIV_S_W_DIV_U_B_DIV_U_D_DIV_U_H_DIV_U_W	= 591,
    HADD_S_D_HADD_S_H_HADD_S_W_HADD_U_D_HADD_U_H_HADD_U_W	= 592,
    HSUB_S_D_HSUB_S_H_HSUB_S_W_HSUB_U_D_HSUB_U_H_HSUB_U_W	= 593,
    MAX_S_B_MAX_S_D_MAX_S_H_MAX_S_W_MIN_S_B_MIN_S_D_MIN_S_H_MIN_S_W	= 594,
    MAX_U_B_MAX_U_D_MAX_U_H_MAX_U_W_MIN_U_B_MIN_U_D_MIN_U_H_MIN_U_W	= 595,
    MAX_A_B_MAX_A_D_MAX_A_H_MAX_A_W_MIN_A_B_MIN_A_D_MIN_A_H_MIN_A_W	= 596,
    MAXI_S_B_MAXI_S_D_MAXI_S_H_MAXI_S_W_MAXI_U_B_MAXI_U_D_MAXI_U_H_MAXI_U_W_MINI_S_B_MINI_S_D_MINI_S_H_MINI_S_W_MINI_U_B_MINI_U_D_MINI_U_H_MINI_U_W	= 597,
    SRAI_B_SRAI_D_SRAI_H_SRAI_W_SRA_B_SRA_D_SRA_H_SRA_W	= 598,
    SRLI_B_SRLI_D_SRLI_H_SRLI_W_SRL_B_SRL_D_SRL_H_SRL_W	= 599,
    SRARI_B_SRARI_D_SRARI_H_SRARI_W_SRAR_B_SRAR_D_SRAR_H_SRAR_W	= 600,
    SRLRI_B_SRLRI_D_SRLRI_H_SRLRI_W_SRLR_B_SRLR_D_SRLR_H_SRLR_W	= 601,
    SLLI_B_SLLI_D_SLLI_H_SLLI_W_SLL_B_SLL_D_SLL_H_SLL_W	= 602,
    PCKEV_B_PCKEV_D_PCKEV_H_PCKEV_W_PCKOD_B_PCKOD_D_PCKOD_H_PCKOD_W	= 603,
    NLOC_B_NLOC_D_NLOC_H_NLOC_W_NLZC_B_NLZC_D_NLZC_H_NLZC_W	= 604,
    FADD_D32_FADD_D64	= 605,
    FADD_S	= 606,
    FMUL_D32_FMUL_D64	= 607,
    FMUL_S	= 608,
    FSUB_D32_FSUB_D64	= 609,
    FSUB_S	= 610,
    TRUNC_L_D64_TRUNC_L_S_TRUNC_W_D32_TRUNC_W_D64_TRUNC_W_S	= 611,
    CVT_D32_S_CVT_D32_W_CVT_D64_L_CVT_D64_S_CVT_D64_W_CVT_L_D64_CVT_L_S_CVT_S_D32_CVT_S_D64_CVT_S_L_CVT_S_W_CVT_W_D32_CVT_W_D64_CVT_W_S	= 612,
    C_EQ_D32_C_EQ_D64_C_F_D32_C_F_D64_C_LE_D32_C_LE_D64_C_LT_D32_C_LT_D64_C_NGE_D32_C_NGE_D64_C_NGLE_D32_C_NGLE_D64_C_NGL_D32_C_NGL_D64_C_NGT_D32_C_NGT_D64_C_OLE_D32_C_OLE_D64_C_OLT_D32_C_OLT_D64_C_SEQ_D32_C_SEQ_D64_C_SF_D32_C_SF_D64_C_UEQ_D32_C_UEQ_D64_C_ULE_D32_C_ULE_D64_C_ULT_D32_C_ULT_D64_C_UN_D32_C_UN_D64	= 613,
    C_EQ_S_C_F_S_C_LE_S_C_LT_S_C_NGE_S_C_NGLE_S_C_NGL_S_C_NGT_S_C_OLE_S_C_OLT_S_C_SEQ_S_C_SF_S_C_UEQ_S_C_ULE_S_C_ULT_S_C_UN_S	= 614,
    FCMP_D32_FCMP_D64	= 615,
    FCMP_S32	= 616,
    PseudoCVT_D32_W_PseudoCVT_D64_L_PseudoCVT_D64_W_PseudoCVT_S_L_PseudoCVT_S_W	= 617,
    FDIV_S	= 618,
    FDIV_D32_FDIV_D64	= 619,
    FSQRT_S	= 620,
    FSQRT_D32_FSQRT_D64	= 621,
    FRCP_D_FRCP_W	= 622,
    FRSQRT_D_FRSQRT_W	= 623,
    RECIP_D32_RECIP_D64	= 624,
    RSQRT_D32_RSQRT_D64	= 625,
    RECIP_S	= 626,
    RSQRT_S	= 627,
    FMADD_D_FMADD_W	= 628,
    FMSUB_D_FMSUB_W	= 629,
    FDIV_W	= 630,
    FDIV_D	= 631,
    FSQRT_W	= 632,
    FSQRT_D	= 633,
    FMUL_D_FMUL_W	= 634,
    FADD_D_FADD_W	= 635,
    FSUB_D_FSUB_W	= 636,
    DPADD_S_D_DPADD_S_H_DPADD_S_W_DPADD_U_D_DPADD_U_H_DPADD_U_W	= 637,
    DPSUB_S_D_DPSUB_S_H_DPSUB_S_W_DPSUB_U_D_DPSUB_U_H_DPSUB_U_W	= 638,
    DOTP_S_D_DOTP_S_H_DOTP_S_W_DOTP_U_D_DOTP_U_H_DOTP_U_W	= 639,
    MSUBV_B_MSUBV_D_MSUBV_H_MSUBV_W	= 640,
    MADDV_B_MADDV_D_MADDV_H_MADDV_W	= 641,
    MULV_B_MULV_D_MULV_H_MULV_W	= 642,
    MADDR_Q_H_MADDR_Q_W	= 643,
    MADD_Q_H_MADD_Q_W	= 644,
    MSUBR_Q_H_MSUBR_Q_W	= 645,
    MSUB_Q_H_MSUB_Q_W	= 646,
    MULR_Q_H_MULR_Q_W	= 647,
    MUL_Q_H_MUL_Q_W	= 648,
    MADD_D32_MADD_D64	= 649,
    MADD_S	= 650,
    MSUB_D32_MSUB_D64	= 651,
    MSUB_S	= 652,
    NMADD_D32_NMADD_D64	= 653,
    NMADD_S	= 654,
    NMSUB_D32_NMSUB_D64	= 655,
    NMSUB_S	= 656,
    CTC1	= 657,
    MTC1_MTC1_D64_BuildPairF64_BuildPairF64_64	= 658,
    MTHC1_D32_MTHC1_D64	= 659,
    COPY_U_B_COPY_U_H_COPY_U_W	= 660,
    COPY_S_B_COPY_S_D_COPY_S_H_COPY_S_W	= 661,
    BC1F	= 662,
    BC1FL	= 663,
    BC1T	= 664,
    BC1TL	= 665,
    CFC1	= 666,
    MFC1_MFC1_D64_ExtractElementF64_ExtractElementF64_64	= 667,
    MFHC1_D32_MFHC1_D64	= 668,
    MOVF_I	= 669,
    MOVT_I	= 670,
    SDC1	= 671,
    SDXC1	= 672,
    SUXC1	= 673,
    SWC1	= 674,
    SWXC1	= 675,
    ST_B_ST_D_ST_H_ST_W	= 676,
    MOVN_I_D32_MOVN_I_D64	= 677,
    MOVN_I_S	= 678,
    MOVZ_I_D32_MOVZ_I_D64	= 679,
    MOVZ_I_S	= 680,
    LDC1	= 681,
    LDXC1	= 682,
    LWC1	= 683,
    LWXC1	= 684,
    LUXC1	= 685,
    LD_B_LD_D_LD_H_LD_W	= 686,
    CEIL_L_D64_CEIL_L_S_CEIL_W_D32_CEIL_W_D64_CEIL_W_S	= 687,
    FLOOR_L_D64_FLOOR_L_S_FLOOR_W_D32_FLOOR_W_D64_FLOOR_W_S	= 688,
    ROUND_L_D64_ROUND_L_S_ROUND_W_D32_ROUND_W_D64_ROUND_W_S	= 689,
    ROTRV	= 690,
    EXTRV_RS_W	= 691,
    EXTRV_R_W	= 692,
    EXTRV_S_H	= 693,
    EXTRV_W	= 694,
    EXTR_RS_W	= 695,
    EXTR_R_W	= 696,
    EXTR_S_H	= 697,
    EXTR_W	= 698,
    INSV	= 699,
    MTHLIP	= 700,
    MTHI_DSP	= 701,
    MTLO_DSP	= 702,
    ABSQ_S_PH	= 703,
    ABSQ_S_W	= 704,
    ADDQ_PH	= 705,
    ADDQ_S_PH	= 706,
    ADDQ_S_W	= 707,
    ADDSC	= 708,
    ADDU_QB	= 709,
    ADDU_S_QB	= 710,
    ADDWC	= 711,
    BITREV	= 712,
    BPOSGE32	= 713,
    CMPGU_EQ_QB	= 714,
    CMPGU_LE_QB	= 715,
    CMPGU_LT_QB	= 716,
    CMPU_EQ_QB	= 717,
    CMPU_LE_QB	= 718,
    CMPU_LT_QB	= 719,
    CMP_EQ_PH	= 720,
    CMP_LE_PH	= 721,
    CMP_LT_PH	= 722,
    DPAQ_SA_L_W	= 723,
    DPAQ_S_W_PH	= 724,
    DPAU_H_QBL	= 725,
    DPAU_H_QBR	= 726,
    DPSQ_SA_L_W	= 727,
    DPSQ_S_W_PH	= 728,
    DPSU_H_QBL	= 729,
    DPSU_H_QBR	= 730,
    EXTPDPV	= 731,
    EXTPDP	= 732,
    EXTPV	= 733,
    EXTP	= 734,
    LBUX	= 735,
    LHX	= 736,
    LWX	= 737,
    MADDU_DSP	= 738,
    MADD_DSP	= 739,
    MAQ_SA_W_PHL	= 740,
    MAQ_SA_W_PHR	= 741,
    MAQ_S_W_PHL	= 742,
    MAQ_S_W_PHR	= 743,
    MFHI_DSP	= 744,
    MFLO_DSP	= 745,
    MODSUB	= 746,
    MSUBU_DSP	= 747,
    MSUB_DSP	= 748,
    MULEQ_S_W_PHL	= 749,
    MULEQ_S_W_PHR	= 750,
    MULEU_S_PH_QBL	= 751,
    MULEU_S_PH_QBR	= 752,
    MULQ_RS_PH	= 753,
    MULSAQ_S_W_PH	= 754,
    MULTU_DSP	= 755,
    MULT_DSP	= 756,
    PACKRL_PH	= 757,
    PICK_PH	= 758,
    PICK_QB	= 759,
    PRECEQU_PH_QBLA	= 760,
    PRECEQU_PH_QBL	= 761,
    PRECEQU_PH_QBRA	= 762,
    PRECEQU_PH_QBR	= 763,
    PRECEQ_W_PHL	= 764,
    PRECEQ_W_PHR	= 765,
    PRECEU_PH_QBLA	= 766,
    PRECEU_PH_QBL	= 767,
    PRECEU_PH_QBRA	= 768,
    PRECEU_PH_QBR	= 769,
    PRECRQU_S_QB_PH	= 770,
    PRECRQ_PH_W	= 771,
    PRECRQ_QB_PH	= 772,
    PRECRQ_RS_PH_W	= 773,
    RADDU_W_QB	= 774,
    RDDSP	= 775,
    REPLV_PH	= 776,
    REPLV_QB	= 777,
    REPL_PH	= 778,
    REPL_QB	= 779,
    SHILOV	= 780,
    SHILO	= 781,
    SHLLV_PH	= 782,
    SHLLV_QB	= 783,
    SHLLV_S_PH	= 784,
    SHLLV_S_W	= 785,
    SHLL_PH	= 786,
    SHLL_QB	= 787,
    SHLL_S_PH	= 788,
    SHLL_S_W	= 789,
    SHRAV_PH	= 790,
    SHRAV_R_PH	= 791,
    SHRAV_R_W	= 792,
    SHRA_PH	= 793,
    SHRA_R_PH	= 794,
    SHRA_R_W	= 795,
    SHRLV_QB	= 796,
    SHRL_QB	= 797,
    SUBQ_PH	= 798,
    SUBQ_S_PH	= 799,
    SUBQ_S_W	= 800,
    SUBU_QB	= 801,
    SUBU_S_QB	= 802,
    WRDSP	= 803,
    ABSQ_S_QB	= 804,
    ADDQH_PH	= 805,
    ADDQH_R_PH	= 806,
    ADDQH_R_W	= 807,
    ADDQH_W	= 808,
    ADDUH_QB	= 809,
    ADDUH_R_QB	= 810,
    ADDU_PH	= 811,
    ADDU_S_PH	= 812,
    APPEND	= 813,
    BALIGN	= 814,
    CMPGDU_EQ_QB	= 815,
    CMPGDU_LE_QB	= 816,
    CMPGDU_LT_QB	= 817,
    DPA_W_PH	= 818,
    DPAQX_SA_W_PH	= 819,
    DPAQX_S_W_PH	= 820,
    DPAX_W_PH	= 821,
    DPS_W_PH	= 822,
    DPSQX_S_W_PH	= 823,
    DPSQX_SA_W_PH	= 824,
    DPSX_W_PH	= 825,
    MUL_PH	= 826,
    MUL_S_PH	= 827,
    MULQ_RS_W	= 828,
    MULQ_S_PH	= 829,
    MULQ_S_W	= 830,
    MULSA_W_PH	= 831,
    PRECR_QB_PH	= 832,
    PRECR_SRA_PH_W	= 833,
    PRECR_SRA_R_PH_W	= 834,
    PREPEND	= 835,
    SHRA_QB	= 836,
    SHRA_R_QB	= 837,
    SHRAV_QB	= 838,
    SHRAV_R_QB	= 839,
    SHRL_PH	= 840,
    SHRLV_PH	= 841,
    SUBQH_PH	= 842,
    SUBQH_R_PH	= 843,
    SUBQH_W	= 844,
    SUBQH_R_W	= 845,
    SUBU_PH	= 846,
    SUBU_S_PH	= 847,
    SUBUH_QB	= 848,
    SUBUH_R_QB	= 849,
    ABSQ_S_PH_MM	= 850,
    ABSQ_S_W_MM	= 851,
    ADDQ_PH_MM	= 852,
    ADDQ_S_PH_MM	= 853,
    ADDQ_S_W_MM	= 854,
    ADDSC_MM	= 855,
    ADDU_QB_MM	= 856,
    ADDU_S_QB_MM	= 857,
    ADDWC_MM	= 858,
    BITREV_MM	= 859,
    BPOSGE32_MM	= 860,
    CMPGU_EQ_QB_MM	= 861,
    CMPGU_LE_QB_MM	= 862,
    CMPGU_LT_QB_MM	= 863,
    CMPU_EQ_QB_MM	= 864,
    CMPU_LE_QB_MM	= 865,
    CMPU_LT_QB_MM	= 866,
    CMP_EQ_PH_MM	= 867,
    CMP_LE_PH_MM	= 868,
    CMP_LT_PH_MM	= 869,
    DPAQ_SA_L_W_MM	= 870,
    DPAQ_S_W_PH_MM	= 871,
    DPAU_H_QBL_MM	= 872,
    DPAU_H_QBR_MM	= 873,
    DPSQ_SA_L_W_MM	= 874,
    DPSQ_S_W_PH_MM	= 875,
    DPSU_H_QBL_MM	= 876,
    DPSU_H_QBR_MM	= 877,
    EXTPDPV_MM	= 878,
    EXTPDP_MM	= 879,
    EXTPV_MM	= 880,
    EXTP_MM	= 881,
    EXTRV_RS_W_MM	= 882,
    EXTRV_R_W_MM	= 883,
    EXTRV_S_H_MM	= 884,
    EXTRV_W_MM	= 885,
    EXTR_RS_W_MM	= 886,
    EXTR_R_W_MM	= 887,
    EXTR_S_H_MM	= 888,
    EXTR_W_MM	= 889,
    INSV_MM	= 890,
    LBUX_MM	= 891,
    LHX_MM	= 892,
    LWX_MM	= 893,
    MADDU_DSP_MM	= 894,
    MADD_DSP_MM	= 895,
    MAQ_SA_W_PHL_MM	= 896,
    MAQ_SA_W_PHR_MM	= 897,
    MAQ_S_W_PHL_MM	= 898,
    MAQ_S_W_PHR_MM	= 899,
    MFHI_DSP_MM	= 900,
    MFLO_DSP_MM	= 901,
    MODSUB_MM	= 902,
    MOVEP_MM	= 903,
    MOVEP_MMR6	= 904,
    MOVN_I_MM	= 905,
    MOVZ_I_MM	= 906,
    MSUBU_DSP_MM	= 907,
    MSUB_DSP_MM	= 908,
    MTHI_DSP_MM	= 909,
    MTHLIP_MM	= 910,
    MTLO_DSP_MM	= 911,
    MULEQ_S_W_PHL_MM	= 912,
    MULEQ_S_W_PHR_MM	= 913,
    MULEU_S_PH_QBL_MM	= 914,
    MULEU_S_PH_QBR_MM	= 915,
    MULQ_RS_PH_MM	= 916,
    MULSAQ_S_W_PH_MM	= 917,
    MULTU_DSP_MM	= 918,
    MULT_DSP_MM	= 919,
    PACKRL_PH_MM	= 920,
    PICK_PH_MM	= 921,
    PICK_QB_MM	= 922,
    PRECEQU_PH_QBLA_MM	= 923,
    PRECEQU_PH_QBL_MM	= 924,
    PRECEQU_PH_QBRA_MM	= 925,
    PRECEQU_PH_QBR_MM	= 926,
    PRECEQ_W_PHL_MM	= 927,
    PRECEQ_W_PHR_MM	= 928,
    PRECEU_PH_QBLA_MM	= 929,
    PRECEU_PH_QBL_MM	= 930,
    PRECEU_PH_QBRA_MM	= 931,
    PRECEU_PH_QBR_MM	= 932,
    PRECRQU_S_QB_PH_MM	= 933,
    PRECRQ_PH_W_MM	= 934,
    PRECRQ_QB_PH_MM	= 935,
    PRECRQ_RS_PH_W_MM	= 936,
    RADDU_W_QB_MM	= 937,
    RDDSP_MM	= 938,
    REPLV_PH_MM	= 939,
    REPLV_QB_MM	= 940,
    REPL_PH_MM	= 941,
    REPL_QB_MM	= 942,
    SHILOV_MM	= 943,
    SHILO_MM	= 944,
    SHLLV_PH_MM	= 945,
    SHLLV_QB_MM	= 946,
    SHLLV_S_PH_MM	= 947,
    SHLLV_S_W_MM	= 948,
    SHLL_PH_MM	= 949,
    SHLL_QB_MM	= 950,
    SHLL_S_PH_MM	= 951,
    SHLL_S_W_MM	= 952,
    SHRAV_PH_MM	= 953,
    SHRAV_R_PH_MM	= 954,
    SHRAV_R_W_MM	= 955,
    SHRA_PH_MM	= 956,
    SHRA_R_PH_MM	= 957,
    SHRA_R_W_MM	= 958,
    SHRLV_QB_MM	= 959,
    SHRL_QB_MM	= 960,
    SUBQ_PH_MM	= 961,
    SUBQ_S_PH_MM	= 962,
    SUBQ_S_W_MM	= 963,
    SUBU_QB_MM	= 964,
    SUBU_S_QB_MM	= 965,
    WRDSP_MM	= 966,
    ABSQ_S_QB_MMR2	= 967,
    ADDQH_PH_MMR2	= 968,
    ADDQH_R_PH_MMR2	= 969,
    ADDQH_R_W_MMR2	= 970,
    ADDQH_W_MMR2	= 971,
    ADDUH_QB_MMR2	= 972,
    ADDUH_R_QB_MMR2	= 973,
    ADDU_PH_MMR2	= 974,
    ADDU_S_PH_MMR2	= 975,
    APPEND_MMR2	= 976,
    BALIGN_MMR2	= 977,
    CMPGDU_EQ_QB_MMR2	= 978,
    CMPGDU_LE_QB_MMR2	= 979,
    CMPGDU_LT_QB_MMR2	= 980,
    DPA_W_PH_MMR2	= 981,
    DPAQX_SA_W_PH_MMR2	= 982,
    DPAQX_S_W_PH_MMR2	= 983,
    DPAX_W_PH_MMR2	= 984,
    DPS_W_PH_MMR2	= 985,
    DPSQX_S_W_PH_MMR2	= 986,
    DPSQX_SA_W_PH_MMR2	= 987,
    DPSX_W_PH_MMR2	= 988,
    MUL_PH_MMR2	= 989,
    MUL_S_PH_MMR2	= 990,
    MULQ_RS_W_MMR2	= 991,
    MULQ_S_PH_MMR2	= 992,
    MULQ_S_W_MMR2	= 993,
    MULSA_W_PH_MMR2	= 994,
    PRECR_QB_PH_MMR2	= 995,
    PRECR_SRA_PH_W_MMR2	= 996,
    PRECR_SRA_R_PH_W_MMR2	= 997,
    PREPEND_MMR2	= 998,
    SHRA_QB_MMR2	= 999,
    SHRA_R_QB_MMR2	= 1000,
    SHRAV_QB_MMR2	= 1001,
    SHRAV_R_QB_MMR2	= 1002,
    SHRL_PH_MMR2	= 1003,
    SHRLV_PH_MMR2	= 1004,
    SUBQH_PH_MMR2	= 1005,
    SUBQH_R_PH_MMR2	= 1006,
    SUBQH_W_MMR2	= 1007,
    SUBQH_R_W_MMR2	= 1008,
    SUBU_PH_MMR2	= 1009,
    SUBU_S_PH_MMR2	= 1010,
    SUBUH_QB_MMR2	= 1011,
    SUBUH_R_QB_MMR2	= 1012,
    BPOSGE32C_MMR3	= 1013,
    SCHED_LIST_END = 1014
  };
} // end Sched namespace
} // end Mips namespace
} // end llvm namespace
#endif // GET_INSTRINFO_SCHED_ENUM

#ifdef GET_INSTRINFO_MC_DESC
#undef GET_INSTRINFO_MC_DESC
namespace llvm {

static const MCPhysReg ImplicitList1[] = { Mips::SP, 0 };
static const MCPhysReg ImplicitList2[] = { Mips::AT, 0 };
static const MCPhysReg ImplicitList3[] = { Mips::RA, 0 };
static const MCPhysReg ImplicitList4[] = { Mips::DSPPos, 0 };
static const MCPhysReg ImplicitList5[] = { Mips::V0, Mips::V1, 0 };
static const MCPhysReg ImplicitList6[] = { Mips::HI0, Mips::LO0, 0 };
static const MCPhysReg ImplicitList7[] = { Mips::T8, 0 };
static const MCPhysReg ImplicitList8[] = { Mips::DSPOutFlag20, 0 };
static const MCPhysReg ImplicitList9[] = { Mips::DSPCarry, 0 };
static const MCPhysReg ImplicitList10[] = { Mips::DSPCCond, 0 };
static const MCPhysReg ImplicitList11[] = { Mips::HI0, Mips::LO0, Mips::P0, Mips::P1, Mips::P2, 0 };
static const MCPhysReg ImplicitList12[] = { Mips::HI0_64, Mips::LO0_64, 0 };
static const MCPhysReg ImplicitList13[] = { Mips::DSPOutFlag16_19, 0 };
static const MCPhysReg ImplicitList14[] = { Mips::DSPEFI, 0 };
static const MCPhysReg ImplicitList15[] = { Mips::DSPPos, Mips::DSPEFI, 0 };
static const MCPhysReg ImplicitList16[] = { Mips::DSPOutFlag23, 0 };
static const MCPhysReg ImplicitList17[] = { Mips::FCC0, 0 };
static const MCPhysReg ImplicitList18[] = { Mips::DSPPos, Mips::DSPSCount, 0 };
static const MCPhysReg ImplicitList19[] = { Mips::AC0, 0 };
static const MCPhysReg ImplicitList20[] = { Mips::AC0_64, 0 };
static const MCPhysReg ImplicitList21[] = { Mips::HI0, 0 };
static const MCPhysReg ImplicitList22[] = { Mips::HI0_64, 0 };
static const MCPhysReg ImplicitList23[] = { Mips::LO0, 0 };
static const MCPhysReg ImplicitList24[] = { Mips::LO0_64, 0 };
static const MCPhysReg ImplicitList25[] = { Mips::MPL0, Mips::P0, Mips::P1, Mips::P2, 0 };
static const MCPhysReg ImplicitList26[] = { Mips::MPL1, Mips::P0, Mips::P1, Mips::P2, 0 };
static const MCPhysReg ImplicitList27[] = { Mips::MPL2, Mips::P0, Mips::P1, Mips::P2, 0 };
static const MCPhysReg ImplicitList28[] = { Mips::P0, 0 };
static const MCPhysReg ImplicitList29[] = { Mips::P1, 0 };
static const MCPhysReg ImplicitList30[] = { Mips::P2, 0 };
static const MCPhysReg ImplicitList31[] = { Mips::DSPOutFlag21, 0 };
static const MCPhysReg ImplicitList32[] = { Mips::DSPOutFlag22, 0 };
static const MCPhysReg ImplicitList33[] = { Mips::P0, Mips::P1, Mips::P2, 0 };
static const MCPhysReg ImplicitList34[] = { Mips::MPL1, Mips::MPL2, Mips::P0, Mips::P1, Mips::P2, 0 };

static const MCOperandInfo OperandInfo2[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo3[] = { { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo4[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo5[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo6[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo7[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo8[] = { { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo9[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo10[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo11[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo12[] = { { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo13[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo14[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo15[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo16[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo17[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, };
static const MCOperandInfo OperandInfo18[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo19[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_2, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo20[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo21[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo22[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, };
static const MCOperandInfo OperandInfo23[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, };
static const MCOperandInfo OperandInfo24[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo25[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, };
static const MCOperandInfo OperandInfo26[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, };
static const MCOperandInfo OperandInfo27[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo28[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_2, 0 }, };
static const MCOperandInfo OperandInfo29[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_2, 0 }, };
static const MCOperandInfo OperandInfo30[] = { { -1, 0, MCOI::OPERAND_GENERIC_0, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_1, 0 }, { -1, 0, MCOI::OPERAND_GENERIC_2, 0 }, };
static const MCOperandInfo OperandInfo31[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo32[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo33[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo34[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo35[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo36[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo37[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo38[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo39[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo40[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo41[] = { { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo42[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo43[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo44[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo45[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo46[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo47[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo48[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo49[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo50[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo51[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo52[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGRCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo53[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo54[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo55[] = { { Mips::FGRCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo56[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo57[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo58[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo59[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo60[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo61[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo62[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo63[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo64[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo65[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo66[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo67[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo68[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo69[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo70[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo71[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo72[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo73[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo74[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo75[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo76[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo77[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo78[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo79[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo80[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo81[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo82[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo83[] = { { Mips::MSA128F16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo84[] = { { Mips::ACC128RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo85[] = { { Mips::ACC64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo86[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo87[] = { { Mips::DSPCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo88[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo89[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo90[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo91[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo92[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo93[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo94[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo95[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo96[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo97[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo98[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo99[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo100[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo101[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::COP0RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo102[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo103[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo104[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128F16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo105[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128F16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo106[] = { { Mips::MSA128F16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo107[] = { { Mips::MSA128F16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo108[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo109[] = { { Mips::COP0RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo110[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo111[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo112[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo113[] = { { Mips::DSPCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo114[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo115[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo116[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo117[] = { { Mips::ACC128RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo118[] = { { Mips::ACC64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo119[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo120[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC128RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo121[] = { { Mips::ACC64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo122[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo123[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo124[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo125[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo126[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo127[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo128[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo129[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo130[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo131[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo132[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo133[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo134[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo135[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo136[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo137[] = { { Mips::GPR32NONZERORegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo138[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo139[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo140[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo141[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo142[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo143[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo144[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo145[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo146[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo147[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo148[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo149[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo150[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo151[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo152[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo153[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo154[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo155[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo156[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo157[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo158[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo159[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo160[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo161[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo162[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsPlusSPRegClassID, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo163[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo164[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo165[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo166[] = { { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo167[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo168[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo169[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo170[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo171[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo172[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo173[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo174[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo175[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo176[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo177[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo178[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo179[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo180[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo181[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_PCREL, 0 }, };
static const MCOperandInfo OperandInfo182[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo183[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo184[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo185[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo186[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo187[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo188[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CCRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo189[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo190[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSACtrlRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo191[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo192[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo193[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo194[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo195[] = { { Mips::FGRCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo196[] = { { Mips::FGRCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo197[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo198[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo199[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo200[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo201[] = { { Mips::CCRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo202[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo203[] = { { Mips::MSACtrlRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo204[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo205[] = { { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo206[] = { { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo207[] = { { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo208[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo209[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo210[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo211[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::COP0RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo212[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo213[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo214[] = { { Mips::COP0RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo215[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo216[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo217[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo218[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo219[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo220[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo221[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo222[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo223[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo224[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo225[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo226[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo227[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo228[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo229[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo230[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo231[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo232[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo233[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo234[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo235[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo236[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo237[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo238[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo239[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo240[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo241[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo242[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo243[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo244[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo245[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo246[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo247[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo248[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo249[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo250[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo251[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo252[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo253[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 1, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo254[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo255[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo256[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo257[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo258[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo259[] = { { Mips::COP3RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo260[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo261[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo262[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo263[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo264[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo265[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo266[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo267[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo268[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo269[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo270[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo271[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo272[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo273[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 3, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo274[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo275[] = { { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { 2, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo276[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo277[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 2, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo278[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo279[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo280[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_IMMEDIATE, 0 }, };
static const MCOperandInfo OperandInfo281[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo282[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo283[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo284[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo285[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo286[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo287[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo288[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo289[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo290[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPRMM16MovePRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16MovePRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo291[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo292[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo293[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo294[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo295[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo296[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FCCRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo297[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo298[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo299[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo300[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo301[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo302[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo303[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo304[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo305[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo306[] = { { Mips::COP2RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo307[] = { { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::AFGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo308[] = { { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo309[] = { { Mips::HI32DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo310[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo311[] = { { Mips::LO32DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo312[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo313[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo314[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo315[] = { { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPRMM16RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo316[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo317[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo318[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo319[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_UNKNOWN, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo320[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::HWRegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo321[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::HWRegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo322[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo323[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo324[] = { { Mips::GPRMM16ZeroRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { 1, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo325[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo326[] = { { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo327[] = { { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGRCCRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::FGR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo328[] = { { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, { Mips::ACC64DSPRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };
static const MCOperandInfo OperandInfo329[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo330[] = { { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::DSPRRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo331[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo332[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo333[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo334[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo335[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo336[] = { { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR64RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { -1, 0, MCOI::OPERAND_UNKNOWN, 0 }, };
static const MCOperandInfo OperandInfo337[] = { { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128BRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo338[] = { { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128DRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo339[] = { { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128HRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo340[] = { { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::MSA128WRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::GPR32RegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, };
static const MCOperandInfo OperandInfo341[] = { { 0, 0|(1<<MCOI::LookupPtrRegClass), MCOI::OPERAND_MEMORY, 0 }, { -1, 0, MCOI::OPERAND_MEMORY, 0 }, };
static const MCOperandInfo OperandInfo342[] = { { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, 0 }, { Mips::CPU16RegsRegClassID, 0, MCOI::OPERAND_REGISTER, ((0 << 16) | (1 << MCOI::TIED_TO)) }, };

extern const MCInstrDesc MipsInsts[] = {
  { 0,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #0 = PHI
  { 1,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1 = INLINEASM
  { 2,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::NotDuplicable), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #2 = CFI_INSTRUCTION
  { 3,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::NotDuplicable), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #3 = EH_LABEL
  { 4,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::NotDuplicable), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #4 = GC_LABEL
  { 5,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::NotDuplicable), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #5 = ANNOTATION_LABEL
  { 6,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #6 = KILL
  { 7,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo4, -1 ,nullptr },  // Inst #7 = EXTRACT_SUBREG
  { 8,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo5, -1 ,nullptr },  // Inst #8 = INSERT_SUBREG
  { 9,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #9 = IMPLICIT_DEF
  { 10,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo6, -1 ,nullptr },  // Inst #10 = SUBREG_TO_REG
  { 11,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo4, -1 ,nullptr },  // Inst #11 = COPY_TO_REGCLASS
  { 12,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #12 = DBG_VALUE
  { 13,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #13 = DBG_LABEL
  { 14,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo7, -1 ,nullptr },  // Inst #14 = REG_SEQUENCE
  { 15,	2,	1,	0,	495,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo7, -1 ,nullptr },  // Inst #15 = COPY
  { 16,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #16 = BUNDLE
  { 17,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #17 = LIFETIME_START
  { 18,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo3, -1 ,nullptr },  // Inst #18 = LIFETIME_END
  { 19,	2,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Call)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo8, -1 ,nullptr },  // Inst #19 = STACKMAP
  { 20,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #20 = FENTRY_CALL
  { 21,	6,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Call)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo9, -1 ,nullptr },  // Inst #21 = PATCHPOINT
  { 22,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo10, -1 ,nullptr },  // Inst #22 = LOAD_STACK_GUARD
  { 23,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Call)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #23 = STATEPOINT
  { 24,	2,	0,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo11, -1 ,nullptr },  // Inst #24 = LOCAL_ESCAPE
  { 25,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #25 = FAULTING_OP
  { 26,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #26 = PATCHABLE_OP
  { 27,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #27 = PATCHABLE_FUNCTION_ENTER
  { 28,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #28 = PATCHABLE_RET
  { 29,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #29 = PATCHABLE_FUNCTION_EXIT
  { 30,	0,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #30 = PATCHABLE_TAIL_CALL
  { 31,	2,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Call)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo11, -1 ,nullptr },  // Inst #31 = PATCHABLE_EVENT_CALL
  { 32,	3,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Call)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo12, -1 ,nullptr },  // Inst #32 = PATCHABLE_TYPED_EVENT_CALL
  { 33,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #33 = ICALL_BRANCH_FUNNEL
  { 34,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #34 = G_ADD
  { 35,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #35 = G_SUB
  { 36,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #36 = G_MUL
  { 37,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #37 = G_SDIV
  { 38,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #38 = G_UDIV
  { 39,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #39 = G_SREM
  { 40,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #40 = G_UREM
  { 41,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #41 = G_AND
  { 42,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #42 = G_OR
  { 43,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #43 = G_XOR
  { 44,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo14, -1 ,nullptr },  // Inst #44 = G_IMPLICIT_DEF
  { 45,	1,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, OperandInfo14, -1 ,nullptr },  // Inst #45 = G_PHI
  { 46,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #46 = G_FRAME_INDEX
  { 47,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #47 = G_GLOBAL_VALUE
  { 48,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo16, -1 ,nullptr },  // Inst #48 = G_EXTRACT
  { 49,	2,	2,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #49 = G_UNMERGE_VALUES
  { 50,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo18, -1 ,nullptr },  // Inst #50 = G_INSERT
  { 51,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #51 = G_MERGE_VALUES
  { 52,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #52 = G_PTRTOINT
  { 53,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #53 = G_INTTOPTR
  { 54,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #54 = G_BITCAST
  { 55,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #55 = G_LOAD
  { 56,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #56 = G_SEXTLOAD
  { 57,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #57 = G_ZEXTLOAD
  { 58,	2,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #58 = G_STORE
  { 59,	5,	2,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo19, -1 ,nullptr },  // Inst #59 = G_ATOMIC_CMPXCHG_WITH_SUCCESS
  { 60,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #60 = G_ATOMIC_CMPXCHG
  { 61,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #61 = G_ATOMICRMW_XCHG
  { 62,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #62 = G_ATOMICRMW_ADD
  { 63,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #63 = G_ATOMICRMW_SUB
  { 64,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #64 = G_ATOMICRMW_AND
  { 65,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #65 = G_ATOMICRMW_NAND
  { 66,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #66 = G_ATOMICRMW_OR
  { 67,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #67 = G_ATOMICRMW_XOR
  { 68,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #68 = G_ATOMICRMW_MAX
  { 69,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #69 = G_ATOMICRMW_MIN
  { 70,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #70 = G_ATOMICRMW_UMAX
  { 71,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo21, -1 ,nullptr },  // Inst #71 = G_ATOMICRMW_UMIN
  { 72,	2,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #72 = G_BRCOND
  { 73,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo14, -1 ,nullptr },  // Inst #73 = G_BRINDIRECT
  { 74,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Variadic), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #74 = G_INTRINSIC
  { 75,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #75 = G_INTRINSIC_W_SIDE_EFFECTS
  { 76,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #76 = G_ANYEXT
  { 77,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #77 = G_TRUNC
  { 78,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #78 = G_CONSTANT
  { 79,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #79 = G_FCONSTANT
  { 80,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo14, -1 ,nullptr },  // Inst #80 = G_VASTART
  { 81,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo16, -1 ,nullptr },  // Inst #81 = G_VAARG
  { 82,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #82 = G_SEXT
  { 83,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #83 = G_ZEXT
  { 84,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #84 = G_SHL
  { 85,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #85 = G_LSHR
  { 86,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #86 = G_ASHR
  { 87,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo22, -1 ,nullptr },  // Inst #87 = G_ICMP
  { 88,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo22, -1 ,nullptr },  // Inst #88 = G_FCMP
  { 89,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #89 = G_SELECT
  { 90,	5,	2,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo23, -1 ,nullptr },  // Inst #90 = G_UADDE
  { 91,	5,	2,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo23, -1 ,nullptr },  // Inst #91 = G_USUBE
  { 92,	4,	2,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #92 = G_SADDO
  { 93,	4,	2,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #93 = G_SSUBO
  { 94,	4,	2,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #94 = G_UMULO
  { 95,	4,	2,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo20, -1 ,nullptr },  // Inst #95 = G_SMULO
  { 96,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #96 = G_UMULH
  { 97,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #97 = G_SMULH
  { 98,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #98 = G_FADD
  { 99,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #99 = G_FSUB
  { 100,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #100 = G_FMUL
  { 101,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo24, -1 ,nullptr },  // Inst #101 = G_FMA
  { 102,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #102 = G_FDIV
  { 103,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #103 = G_FREM
  { 104,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo13, -1 ,nullptr },  // Inst #104 = G_FPOW
  { 105,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #105 = G_FEXP
  { 106,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #106 = G_FEXP2
  { 107,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #107 = G_FLOG
  { 108,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #108 = G_FLOG2
  { 109,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #109 = G_FNEG
  { 110,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #110 = G_FPEXT
  { 111,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #111 = G_FPTRUNC
  { 112,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #112 = G_FPTOSI
  { 113,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #113 = G_FPTOUI
  { 114,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #114 = G_SITOFP
  { 115,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #115 = G_UITOFP
  { 116,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #116 = G_FABS
  { 117,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo26, -1 ,nullptr },  // Inst #117 = G_GEP
  { 118,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo27, -1 ,nullptr },  // Inst #118 = G_PTR_MASK
  { 119,	1,	0,	0,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #119 = G_BR
  { 120,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo28, -1 ,nullptr },  // Inst #120 = G_INSERT_VECTOR_ELT
  { 121,	3,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo29, -1 ,nullptr },  // Inst #121 = G_EXTRACT_VECTOR_ELT
  { 122,	4,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo30, -1 ,nullptr },  // Inst #122 = G_SHUFFLE_VECTOR
  { 123,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo25, -1 ,nullptr },  // Inst #123 = G_BSWAP
  { 124,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo17, -1 ,nullptr },  // Inst #124 = G_ADDRSPACE_CAST
  { 125,	2,	1,	0,	0,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo15, -1 ,nullptr },  // Inst #125 = G_BLOCK_ADDR
  { 126,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #126 = ABSMacro
  { 127,	2,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, OperandInfo8, -1 ,nullptr },  // Inst #127 = ADJCALLSTACKDOWN
  { 128,	2,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, OperandInfo8, -1 ,nullptr },  // Inst #128 = ADJCALLSTACKUP
  { 129,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #129 = AND_V_D_PSEUDO
  { 130,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #130 = AND_V_H_PSEUDO
  { 131,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #131 = AND_V_W_PSEUDO
  { 132,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo35, -1 ,nullptr },  // Inst #132 = ATOMIC_CMP_SWAP_I16
  { 133,	7,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo36, -1 ,nullptr },  // Inst #133 = ATOMIC_CMP_SWAP_I16_POSTRA
  { 134,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo35, -1 ,nullptr },  // Inst #134 = ATOMIC_CMP_SWAP_I32
  { 135,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo35, -1 ,nullptr },  // Inst #135 = ATOMIC_CMP_SWAP_I32_POSTRA
  { 136,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo37, -1 ,nullptr },  // Inst #136 = ATOMIC_CMP_SWAP_I64
  { 137,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo37, -1 ,nullptr },  // Inst #137 = ATOMIC_CMP_SWAP_I64_POSTRA
  { 138,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo35, -1 ,nullptr },  // Inst #138 = ATOMIC_CMP_SWAP_I8
  { 139,	7,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo36, -1 ,nullptr },  // Inst #139 = ATOMIC_CMP_SWAP_I8_POSTRA
  { 140,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #140 = ATOMIC_LOAD_ADD_I16
  { 141,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #141 = ATOMIC_LOAD_ADD_I16_POSTRA
  { 142,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #142 = ATOMIC_LOAD_ADD_I32
  { 143,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #143 = ATOMIC_LOAD_ADD_I32_POSTRA
  { 144,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #144 = ATOMIC_LOAD_ADD_I64
  { 145,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #145 = ATOMIC_LOAD_ADD_I64_POSTRA
  { 146,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #146 = ATOMIC_LOAD_ADD_I8
  { 147,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #147 = ATOMIC_LOAD_ADD_I8_POSTRA
  { 148,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #148 = ATOMIC_LOAD_AND_I16
  { 149,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #149 = ATOMIC_LOAD_AND_I16_POSTRA
  { 150,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #150 = ATOMIC_LOAD_AND_I32
  { 151,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #151 = ATOMIC_LOAD_AND_I32_POSTRA
  { 152,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #152 = ATOMIC_LOAD_AND_I64
  { 153,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #153 = ATOMIC_LOAD_AND_I64_POSTRA
  { 154,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #154 = ATOMIC_LOAD_AND_I8
  { 155,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #155 = ATOMIC_LOAD_AND_I8_POSTRA
  { 156,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #156 = ATOMIC_LOAD_NAND_I16
  { 157,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #157 = ATOMIC_LOAD_NAND_I16_POSTRA
  { 158,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #158 = ATOMIC_LOAD_NAND_I32
  { 159,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #159 = ATOMIC_LOAD_NAND_I32_POSTRA
  { 160,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #160 = ATOMIC_LOAD_NAND_I64
  { 161,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #161 = ATOMIC_LOAD_NAND_I64_POSTRA
  { 162,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #162 = ATOMIC_LOAD_NAND_I8
  { 163,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #163 = ATOMIC_LOAD_NAND_I8_POSTRA
  { 164,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #164 = ATOMIC_LOAD_OR_I16
  { 165,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #165 = ATOMIC_LOAD_OR_I16_POSTRA
  { 166,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #166 = ATOMIC_LOAD_OR_I32
  { 167,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #167 = ATOMIC_LOAD_OR_I32_POSTRA
  { 168,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #168 = ATOMIC_LOAD_OR_I64
  { 169,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #169 = ATOMIC_LOAD_OR_I64_POSTRA
  { 170,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #170 = ATOMIC_LOAD_OR_I8
  { 171,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #171 = ATOMIC_LOAD_OR_I8_POSTRA
  { 172,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #172 = ATOMIC_LOAD_SUB_I16
  { 173,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #173 = ATOMIC_LOAD_SUB_I16_POSTRA
  { 174,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #174 = ATOMIC_LOAD_SUB_I32
  { 175,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #175 = ATOMIC_LOAD_SUB_I32_POSTRA
  { 176,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #176 = ATOMIC_LOAD_SUB_I64
  { 177,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #177 = ATOMIC_LOAD_SUB_I64_POSTRA
  { 178,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #178 = ATOMIC_LOAD_SUB_I8
  { 179,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #179 = ATOMIC_LOAD_SUB_I8_POSTRA
  { 180,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #180 = ATOMIC_LOAD_XOR_I16
  { 181,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #181 = ATOMIC_LOAD_XOR_I16_POSTRA
  { 182,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #182 = ATOMIC_LOAD_XOR_I32
  { 183,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #183 = ATOMIC_LOAD_XOR_I32_POSTRA
  { 184,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #184 = ATOMIC_LOAD_XOR_I64
  { 185,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #185 = ATOMIC_LOAD_XOR_I64_POSTRA
  { 186,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #186 = ATOMIC_LOAD_XOR_I8
  { 187,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #187 = ATOMIC_LOAD_XOR_I8_POSTRA
  { 188,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #188 = ATOMIC_SWAP_I16
  { 189,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #189 = ATOMIC_SWAP_I16_POSTRA
  { 190,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #190 = ATOMIC_SWAP_I32
  { 191,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #191 = ATOMIC_SWAP_I32_POSTRA
  { 192,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #192 = ATOMIC_SWAP_I64
  { 193,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo40, -1 ,nullptr },  // Inst #193 = ATOMIC_SWAP_I64_POSTRA
  { 194,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo38, -1 ,nullptr },  // Inst #194 = ATOMIC_SWAP_I8
  { 195,	6,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo39, -1 ,nullptr },  // Inst #195 = ATOMIC_SWAP_I8_POSTRA
  { 196,	1,	0,	4,	366,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, ImplicitList2, OperandInfo41, -1 ,nullptr },  // Inst #196 = B
  { 197,	1,	0,	4,	368,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, ImplicitList3, OperandInfo41, -1 ,nullptr },  // Inst #197 = BAL_BR
  { 198,	1,	0,	4,	3,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, ImplicitList3, OperandInfo41, -1 ,nullptr },  // Inst #198 = BAL_BR_MM
  { 199,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #199 = BEQLImmMacro
  { 200,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #200 = BGE
  { 201,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #201 = BGEImmMacro
  { 202,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #202 = BGEL
  { 203,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #203 = BGELImmMacro
  { 204,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #204 = BGEU
  { 205,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #205 = BGEUImmMacro
  { 206,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #206 = BGEUL
  { 207,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #207 = BGEULImmMacro
  { 208,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #208 = BGT
  { 209,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #209 = BGTImmMacro
  { 210,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #210 = BGTL
  { 211,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #211 = BGTLImmMacro
  { 212,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #212 = BGTU
  { 213,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #213 = BGTUImmMacro
  { 214,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #214 = BGTUL
  { 215,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #215 = BGTULImmMacro
  { 216,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #216 = BLE
  { 217,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #217 = BLEImmMacro
  { 218,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #218 = BLEL
  { 219,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #219 = BLELImmMacro
  { 220,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #220 = BLEU
  { 221,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #221 = BLEUImmMacro
  { 222,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #222 = BLEUL
  { 223,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #223 = BLEULImmMacro
  { 224,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #224 = BLT
  { 225,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #225 = BLTImmMacro
  { 226,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #226 = BLTL
  { 227,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #227 = BLTLImmMacro
  { 228,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #228 = BLTU
  { 229,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #229 = BLTUImmMacro
  { 230,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo43, -1 ,nullptr },  // Inst #230 = BLTUL
  { 231,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #231 = BLTULImmMacro
  { 232,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #232 = BNELImmMacro
  { 233,	1,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, ImplicitList4, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #233 = BPOSGE32_PSEUDO
  { 234,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #234 = BSEL_D_PSEUDO
  { 235,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #235 = BSEL_FD_PSEUDO
  { 236,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #236 = BSEL_FW_PSEUDO
  { 237,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #237 = BSEL_H_PSEUDO
  { 238,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #238 = BSEL_W_PSEUDO
  { 239,	1,	0,	4,	2,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, ImplicitList2, OperandInfo41, -1 ,nullptr },  // Inst #239 = B_MM
  { 240,	1,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #240 = B_MMR6_Pseudo
  { 241,	1,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #241 = B_MM_Pseudo
  { 242,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #242 = BeqImm
  { 243,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, OperandInfo42, -1 ,nullptr },  // Inst #243 = BneImm
  { 244,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #244 = BteqzT8CmpX16
  { 245,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #245 = BteqzT8CmpiX16
  { 246,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #246 = BteqzT8SltX16
  { 247,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #247 = BteqzT8SltiX16
  { 248,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #248 = BteqzT8SltiuX16
  { 249,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #249 = BteqzT8SltuX16
  { 250,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #250 = BtnezT8CmpX16
  { 251,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #251 = BtnezT8CmpiX16
  { 252,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #252 = BtnezT8SltX16
  { 253,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #253 = BtnezT8SltiX16
  { 254,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo49, -1 ,nullptr },  // Inst #254 = BtnezT8SltiuX16
  { 255,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo48, -1 ,nullptr },  // Inst #255 = BtnezT8SltuX16
  { 256,	3,	1,	4,	658,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo50, -1 ,nullptr },  // Inst #256 = BuildPairF64
  { 257,	3,	1,	4,	658,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo51, -1 ,nullptr },  // Inst #257 = BuildPairF64_64
  { 258,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo52, -1 ,nullptr },  // Inst #258 = CFTC1
  { 259,	3,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::NotDuplicable), 0x0ULL, nullptr, nullptr, OperandInfo4, -1 ,nullptr },  // Inst #259 = CONSTPOOL_ENTRY
  { 260,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo53, -1 ,nullptr },  // Inst #260 = COPY_FD_PSEUDO
  { 261,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo54, -1 ,nullptr },  // Inst #261 = COPY_FW_PSEUDO
  { 262,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo55, -1 ,nullptr },  // Inst #262 = CTTC1
  { 263,	1,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #263 = Constant32
  { 264,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #264 = DMULImmMacro
  { 265,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #265 = DMULMacro
  { 266,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #266 = DMULOMacro
  { 267,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #267 = DMULOUMacro
  { 268,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #268 = DROL
  { 269,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #269 = DROLImm
  { 270,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #270 = DROR
  { 271,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #271 = DRORImm
  { 272,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #272 = DSDivIMacro
  { 273,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #273 = DSDivMacro
  { 274,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #274 = DSRemIMacro
  { 275,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #275 = DSRemMacro
  { 276,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #276 = DUDivIMacro
  { 277,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #277 = DUDivMacro
  { 278,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #278 = DURemIMacro
  { 279,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #279 = DURemMacro
  { 280,	0,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #280 = ERet
  { 281,	3,	1,	4,	667,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo60, -1 ,nullptr },  // Inst #281 = ExtractElementF64
  { 282,	3,	1,	4,	667,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo61, -1 ,nullptr },  // Inst #282 = ExtractElementF64_64
  { 283,	2,	1,	4,	565,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #283 = FABS_D
  { 284,	2,	1,	4,	565,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #284 = FABS_W
  { 285,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #285 = FEXP2_D_1_PSEUDO
  { 286,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #286 = FEXP2_W_1_PSEUDO
  { 287,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo64, -1 ,nullptr },  // Inst #287 = FILL_FD_PSEUDO
  { 288,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo65, -1 ,nullptr },  // Inst #288 = FILL_FW_PSEUDO
  { 289,	4,	2,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo66, -1 ,nullptr },  // Inst #289 = GotPrologue16
  { 290,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo67, -1 ,nullptr },  // Inst #290 = INSERT_B_VIDX64_PSEUDO
  { 291,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo68, -1 ,nullptr },  // Inst #291 = INSERT_B_VIDX_PSEUDO
  { 292,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo69, -1 ,nullptr },  // Inst #292 = INSERT_D_VIDX64_PSEUDO
  { 293,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo70, -1 ,nullptr },  // Inst #293 = INSERT_D_VIDX_PSEUDO
  { 294,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo71, -1 ,nullptr },  // Inst #294 = INSERT_FD_PSEUDO
  { 295,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo72, -1 ,nullptr },  // Inst #295 = INSERT_FD_VIDX64_PSEUDO
  { 296,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo73, -1 ,nullptr },  // Inst #296 = INSERT_FD_VIDX_PSEUDO
  { 297,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo74, -1 ,nullptr },  // Inst #297 = INSERT_FW_PSEUDO
  { 298,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo75, -1 ,nullptr },  // Inst #298 = INSERT_FW_VIDX64_PSEUDO
  { 299,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo76, -1 ,nullptr },  // Inst #299 = INSERT_FW_VIDX_PSEUDO
  { 300,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo77, -1 ,nullptr },  // Inst #300 = INSERT_H_VIDX64_PSEUDO
  { 301,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo78, -1 ,nullptr },  // Inst #301 = INSERT_H_VIDX_PSEUDO
  { 302,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo79, -1 ,nullptr },  // Inst #302 = INSERT_W_VIDX64_PSEUDO
  { 303,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo80, -1 ,nullptr },  // Inst #303 = INSERT_W_VIDX_PSEUDO
  { 304,	1,	0,	4,	6,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x10ULL, nullptr, ImplicitList3, OperandInfo81, -1 ,nullptr },  // Inst #304 = JALR64Pseudo
  { 305,	1,	0,	4,	6,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x10ULL, nullptr, ImplicitList3, OperandInfo81, -1 ,nullptr },  // Inst #305 = JALRHB64Pseudo
  { 306,	1,	0,	4,	399,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x10ULL, nullptr, ImplicitList3, OperandInfo44, -1 ,nullptr },  // Inst #306 = JALRHBPseudo
  { 307,	1,	0,	4,	399,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x10ULL, nullptr, ImplicitList3, OperandInfo44, -1 ,nullptr },  // Inst #307 = JALRPseudo
  { 308,	1,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #308 = JalOneReg
  { 309,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #309 = JalTwoReg
  { 310,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #310 = LDMacro
  { 311,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo83, -1 ,nullptr },  // Inst #311 = LD_F16
  { 312,	3,	1,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo84, -1 ,nullptr },  // Inst #312 = LOAD_ACC128
  { 313,	3,	1,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo85, -1 ,nullptr },  // Inst #313 = LOAD_ACC64
  { 314,	3,	1,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo86, -1 ,nullptr },  // Inst #314 = LOAD_ACC64DSP
  { 315,	3,	1,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo87, -1 ,nullptr },  // Inst #315 = LOAD_CCOND_DSP
  { 316,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo88, -1 ,nullptr },  // Inst #316 = LONG_BRANCH_ADDiu
  { 317,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo89, -1 ,nullptr },  // Inst #317 = LONG_BRANCH_DADDiu
  { 318,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo90, -1 ,nullptr },  // Inst #318 = LONG_BRANCH_LUi
  { 319,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo91, -1 ,nullptr },  // Inst #319 = LWM_MM
  { 320,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo92, -1 ,nullptr },  // Inst #320 = LoadAddrImm32
  { 321,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #321 = LoadAddrImm64
  { 322,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #322 = LoadAddrReg32
  { 323,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #323 = LoadAddrReg64
  { 324,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #324 = LoadImm32
  { 325,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #325 = LoadImm64
  { 326,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo96, -1 ,nullptr },  // Inst #326 = LoadImmDoubleFGR
  { 327,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo97, -1 ,nullptr },  // Inst #327 = LoadImmDoubleFGR_32
  { 328,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #328 = LoadImmDoubleGPR
  { 329,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo98, -1 ,nullptr },  // Inst #329 = LoadImmSingleFGR
  { 330,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #330 = LoadImmSingleGPR
  { 331,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo99, -1 ,nullptr },  // Inst #331 = LwConstant32
  { 332,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #332 = MFTACX
  { 333,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #333 = MFTC0
  { 334,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo102, -1 ,nullptr },  // Inst #334 = MFTC1
  { 335,	1,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #335 = MFTDSP
  { 336,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #336 = MFTGPR
  { 337,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo102, -1 ,nullptr },  // Inst #337 = MFTHC1
  { 338,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #338 = MFTHI
  { 339,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #339 = MFTLO
  { 340,	2,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x10ULL, ImplicitList5, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #340 = MIPSeh_return32
  { 341,	2,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x10ULL, ImplicitList5, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #341 = MIPSeh_return64
  { 342,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo104, -1 ,nullptr },  // Inst #342 = MSA_FP_EXTEND_D_PSEUDO
  { 343,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo105, -1 ,nullptr },  // Inst #343 = MSA_FP_EXTEND_W_PSEUDO
  { 344,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo106, -1 ,nullptr },  // Inst #344 = MSA_FP_ROUND_D_PSEUDO
  { 345,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo107, -1 ,nullptr },  // Inst #345 = MSA_FP_ROUND_W_PSEUDO
  { 346,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo108, -1 ,nullptr },  // Inst #346 = MTTACX
  { 347,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #347 = MTTC0
  { 348,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #348 = MTTC1
  { 349,	1,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #349 = MTTDSP
  { 350,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #350 = MTTGPR
  { 351,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #351 = MTTHC1
  { 352,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo108, -1 ,nullptr },  // Inst #352 = MTTHI
  { 353,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo108, -1 ,nullptr },  // Inst #353 = MTTLO
  { 354,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #354 = MULImmMacro
  { 355,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #355 = MULOMacro
  { 356,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #356 = MULOUMacro
  { 357,	2,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, ImplicitList6, OperandInfo111, -1 ,nullptr },  // Inst #357 = MultRxRy16
  { 358,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, ImplicitList6, OperandInfo112, -1 ,nullptr },  // Inst #358 = MultRxRyRz16
  { 359,	2,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, ImplicitList6, OperandInfo111, -1 ,nullptr },  // Inst #359 = MultuRxRy16
  { 360,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, ImplicitList6, OperandInfo112, -1 ,nullptr },  // Inst #360 = MultuRxRyRz16
  { 361,	0,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #361 = NOP
  { 362,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #362 = NORImm
  { 363,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #363 = NORImm64
  { 364,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #364 = NOR_V_D_PSEUDO
  { 365,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #365 = NOR_V_H_PSEUDO
  { 366,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #366 = NOR_V_W_PSEUDO
  { 367,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #367 = OR_V_D_PSEUDO
  { 368,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #368 = OR_V_H_PSEUDO
  { 369,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #369 = OR_V_W_PSEUDO
  { 370,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #370 = PseudoCMPU_EQ_QB
  { 371,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #371 = PseudoCMPU_LE_QB
  { 372,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #372 = PseudoCMPU_LT_QB
  { 373,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #373 = PseudoCMP_EQ_PH
  { 374,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #374 = PseudoCMP_LE_PH
  { 375,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo113, -1 ,nullptr },  // Inst #375 = PseudoCMP_LT_PH
  { 376,	2,	1,	4,	617,	0|(1ULL<<MCID::Pseudo), 0x4ULL, nullptr, nullptr, OperandInfo114, -1 ,nullptr },  // Inst #376 = PseudoCVT_D32_W
  { 377,	2,	1,	4,	617,	0|(1ULL<<MCID::Pseudo), 0x4ULL, nullptr, nullptr, OperandInfo115, -1 ,nullptr },  // Inst #377 = PseudoCVT_D64_L
  { 378,	2,	1,	4,	617,	0|(1ULL<<MCID::Pseudo), 0x4ULL, nullptr, nullptr, OperandInfo116, -1 ,nullptr },  // Inst #378 = PseudoCVT_D64_W
  { 379,	2,	1,	4,	617,	0|(1ULL<<MCID::Pseudo), 0x4ULL, nullptr, nullptr, OperandInfo115, -1 ,nullptr },  // Inst #379 = PseudoCVT_S_L
  { 380,	2,	1,	4,	617,	0|(1ULL<<MCID::Pseudo), 0x4ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #380 = PseudoCVT_S_W
  { 381,	3,	1,	4,	8,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo117, -1 ,nullptr },  // Inst #381 = PseudoDMULT
  { 382,	3,	1,	4,	9,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo117, -1 ,nullptr },  // Inst #382 = PseudoDMULTu
  { 383,	3,	1,	4,	10,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo117, -1 ,nullptr },  // Inst #383 = PseudoDSDIV
  { 384,	3,	1,	4,	11,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo117, -1 ,nullptr },  // Inst #384 = PseudoDUDIV
  { 385,	1,	0,	4,	378,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #385 = PseudoIndirectBranch
  { 386,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #386 = PseudoIndirectBranch64
  { 387,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #387 = PseudoIndirectBranch64R6
  { 388,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #388 = PseudoIndirectBranchR6
  { 389,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #389 = PseudoIndirectBranch_MM
  { 390,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #390 = PseudoIndirectBranch_MMR6
  { 391,	1,	0,	4,	378,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #391 = PseudoIndirectHazardBranch
  { 392,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #392 = PseudoIndirectHazardBranch64
  { 393,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #393 = PseudoIndrectHazardBranch64R6
  { 394,	1,	0,	4,	12,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #394 = PseudoIndrectHazardBranchR6
  { 395,	4,	1,	4,	470,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo118, -1 ,nullptr },  // Inst #395 = PseudoMADD
  { 396,	4,	1,	4,	471,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo118, -1 ,nullptr },  // Inst #396 = PseudoMADDU
  { 397,	2,	1,	4,	459,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo119, -1 ,nullptr },  // Inst #397 = PseudoMFHI
  { 398,	2,	1,	4,	15,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo120, -1 ,nullptr },  // Inst #398 = PseudoMFHI64
  { 399,	2,	1,	4,	459,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo119, -1 ,nullptr },  // Inst #399 = PseudoMFLO
  { 400,	2,	1,	4,	15,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo120, -1 ,nullptr },  // Inst #400 = PseudoMFLO64
  { 401,	4,	1,	4,	472,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo118, -1 ,nullptr },  // Inst #401 = PseudoMSUB
  { 402,	4,	1,	4,	473,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo118, -1 ,nullptr },  // Inst #402 = PseudoMSUBU
  { 403,	3,	1,	4,	474,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #403 = PseudoMTLOHI
  { 404,	3,	1,	4,	18,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo117, -1 ,nullptr },  // Inst #404 = PseudoMTLOHI64
  { 405,	3,	1,	4,	18,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo122, -1 ,nullptr },  // Inst #405 = PseudoMTLOHI_DSP
  { 406,	3,	1,	4,	468,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #406 = PseudoMULT
  { 407,	3,	1,	4,	469,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #407 = PseudoMULTu
  { 408,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo123, -1 ,nullptr },  // Inst #408 = PseudoPICK_PH
  { 409,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo123, -1 ,nullptr },  // Inst #409 = PseudoPICK_QB
  { 410,	1,	0,	4,	379,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #410 = PseudoReturn
  { 411,	1,	0,	4,	21,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #411 = PseudoReturn64
  { 412,	3,	1,	4,	465,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #412 = PseudoSDIV
  { 413,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo124, -1 ,nullptr },  // Inst #413 = PseudoSELECTFP_F_D32
  { 414,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo125, -1 ,nullptr },  // Inst #414 = PseudoSELECTFP_F_D64
  { 415,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo126, -1 ,nullptr },  // Inst #415 = PseudoSELECTFP_F_I
  { 416,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo127, -1 ,nullptr },  // Inst #416 = PseudoSELECTFP_F_I64
  { 417,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo128, -1 ,nullptr },  // Inst #417 = PseudoSELECTFP_F_S
  { 418,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo124, -1 ,nullptr },  // Inst #418 = PseudoSELECTFP_T_D32
  { 419,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo125, -1 ,nullptr },  // Inst #419 = PseudoSELECTFP_T_D64
  { 420,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo126, -1 ,nullptr },  // Inst #420 = PseudoSELECTFP_T_I
  { 421,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo127, -1 ,nullptr },  // Inst #421 = PseudoSELECTFP_T_I64
  { 422,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo128, -1 ,nullptr },  // Inst #422 = PseudoSELECTFP_T_S
  { 423,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo129, -1 ,nullptr },  // Inst #423 = PseudoSELECT_D32
  { 424,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo130, -1 ,nullptr },  // Inst #424 = PseudoSELECT_D64
  { 425,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo131, -1 ,nullptr },  // Inst #425 = PseudoSELECT_I
  { 426,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo132, -1 ,nullptr },  // Inst #426 = PseudoSELECT_I64
  { 427,	4,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo133, -1 ,nullptr },  // Inst #427 = PseudoSELECT_S
  { 428,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo134, -1 ,nullptr },  // Inst #428 = PseudoTRUNC_W_D
  { 429,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo135, -1 ,nullptr },  // Inst #429 = PseudoTRUNC_W_D32
  { 430,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo136, -1 ,nullptr },  // Inst #430 = PseudoTRUNC_W_S
  { 431,	3,	1,	4,	466,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #431 = PseudoUDIV
  { 432,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #432 = ROL
  { 433,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #433 = ROLImm
  { 434,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #434 = ROR
  { 435,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #435 = RORImm
  { 436,	0,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x10ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #436 = RetRA
  { 437,	0,	0,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #437 = RetRA16
  { 438,	3,	1,	4,	22,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #438 = SDIV_MM_Pseudo
  { 439,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #439 = SDMacro
  { 440,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #440 = SDivIMacro
  { 441,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo137, -1 ,nullptr },  // Inst #441 = SDivMacro
  { 442,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #442 = SEQIMacro
  { 443,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #443 = SEQMacro
  { 444,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #444 = SLTImm64
  { 445,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #445 = SLTUImm64
  { 446,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo138, -1 ,nullptr },  // Inst #446 = SNZ_B_PSEUDO
  { 447,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo139, -1 ,nullptr },  // Inst #447 = SNZ_D_PSEUDO
  { 448,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo140, -1 ,nullptr },  // Inst #448 = SNZ_H_PSEUDO
  { 449,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo138, -1 ,nullptr },  // Inst #449 = SNZ_V_PSEUDO
  { 450,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo141, -1 ,nullptr },  // Inst #450 = SNZ_W_PSEUDO
  { 451,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #451 = SRemIMacro
  { 452,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #452 = SRemMacro
  { 453,	3,	0,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo84, -1 ,nullptr },  // Inst #453 = STORE_ACC128
  { 454,	3,	0,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo85, -1 ,nullptr },  // Inst #454 = STORE_ACC64
  { 455,	3,	0,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo86, -1 ,nullptr },  // Inst #455 = STORE_ACC64DSP
  { 456,	3,	0,	4,	0,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo87, -1 ,nullptr },  // Inst #456 = STORE_CCOND_DSP
  { 457,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo83, -1 ,nullptr },  // Inst #457 = ST_F16
  { 458,	3,	0,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo91, -1 ,nullptr },  // Inst #458 = SWM_MM
  { 459,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo138, -1 ,nullptr },  // Inst #459 = SZ_B_PSEUDO
  { 460,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo139, -1 ,nullptr },  // Inst #460 = SZ_D_PSEUDO
  { 461,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo140, -1 ,nullptr },  // Inst #461 = SZ_H_PSEUDO
  { 462,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo138, -1 ,nullptr },  // Inst #462 = SZ_V_PSEUDO
  { 463,	2,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo141, -1 ,nullptr },  // Inst #463 = SZ_W_PSEUDO
  { 464,	4,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo142, -1 ,nullptr },  // Inst #464 = SelBeqZ
  { 465,	4,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo142, -1 ,nullptr },  // Inst #465 = SelBneZ
  { 466,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #466 = SelTBteqZCmp
  { 467,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #467 = SelTBteqZCmpi
  { 468,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #468 = SelTBteqZSlt
  { 469,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #469 = SelTBteqZSlti
  { 470,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #470 = SelTBteqZSltiu
  { 471,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #471 = SelTBteqZSltu
  { 472,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #472 = SelTBtneZCmp
  { 473,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #473 = SelTBtneZCmpi
  { 474,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #474 = SelTBtneZSlt
  { 475,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #475 = SelTBtneZSlti
  { 476,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo144, -1 ,nullptr },  // Inst #476 = SelTBtneZSltiu
  { 477,	5,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo143, -1 ,nullptr },  // Inst #477 = SelTBtneZSltu
  { 478,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo112, -1 ,nullptr },  // Inst #478 = SltCCRxRy16
  { 479,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo145, -1 ,nullptr },  // Inst #479 = SltiCCRxImmX16
  { 480,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo145, -1 ,nullptr },  // Inst #480 = SltiuCCRxImmX16
  { 481,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter), 0x0ULL, nullptr, nullptr, OperandInfo112, -1 ,nullptr },  // Inst #481 = SltuCCRxRy16
  { 482,	3,	1,	2,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo112, -1 ,nullptr },  // Inst #482 = SltuRxRyRz16
  { 483,	1,	0,	4,	375,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo2, -1 ,nullptr },  // Inst #483 = TAILCALL
  { 484,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo81, -1 ,nullptr },  // Inst #484 = TAILCALL64R6REG
  { 485,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo81, -1 ,nullptr },  // Inst #485 = TAILCALLHB64R6REG
  { 486,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #486 = TAILCALLHBR6REG
  { 487,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #487 = TAILCALLR6REG
  { 488,	1,	0,	4,	376,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #488 = TAILCALLREG
  { 489,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo81, -1 ,nullptr },  // Inst #489 = TAILCALLREG64
  { 490,	1,	0,	4,	376,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #490 = TAILCALLREGHB
  { 491,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo81, -1 ,nullptr },  // Inst #491 = TAILCALLREGHB64
  { 492,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #492 = TAILCALLREG_MM
  { 493,	1,	0,	4,	25,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo44, -1 ,nullptr },  // Inst #493 = TAILCALLREG_MMR6
  { 494,	1,	0,	4,	24,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo2, -1 ,nullptr },  // Inst #494 = TAILCALL_MM
  { 495,	1,	0,	4,	24,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Return)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::ExtraSrcRegAllocReq), 0x10ULL, nullptr, ImplicitList2, OperandInfo2, -1 ,nullptr },  // Inst #495 = TAILCALL_MMR6
  { 496,	0,	0,	4,	394,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #496 = TRAP
  { 497,	0,	0,	4,	26,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x10ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #497 = TRAP_MM
  { 498,	3,	1,	4,	23,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UsesCustomInserter)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo121, -1 ,nullptr },  // Inst #498 = UDIV_MM_Pseudo
  { 499,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #499 = UDivIMacro
  { 500,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #500 = UDivMacro
  { 501,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #501 = URemIMacro
  { 502,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #502 = URemMacro
  { 503,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #503 = Ulh
  { 504,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #504 = Ulhu
  { 505,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #505 = Ulw
  { 506,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #506 = Ush
  { 507,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #507 = Usw
  { 508,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #508 = XOR_V_D_PSEUDO
  { 509,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #509 = XOR_V_H_PSEUDO
  { 510,	3,	1,	4,	1,	0|(1ULL<<MCID::Pseudo), 0x0ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #510 = XOR_V_W_PSEUDO
  { 511,	2,	1,	4,	703,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo146, -1 ,nullptr },  // Inst #511 = ABSQ_S_PH
  { 512,	2,	1,	4,	850,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo146, -1 ,nullptr },  // Inst #512 = ABSQ_S_PH_MM
  { 513,	2,	1,	4,	804,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo146, -1 ,nullptr },  // Inst #513 = ABSQ_S_QB
  { 514,	2,	1,	4,	967,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo146, -1 ,nullptr },  // Inst #514 = ABSQ_S_QB_MMR2
  { 515,	2,	1,	4,	704,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo31, -1 ,nullptr },  // Inst #515 = ABSQ_S_W
  { 516,	2,	1,	4,	851,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo31, -1 ,nullptr },  // Inst #516 = ABSQ_S_W_MM
  { 517,	3,	1,	4,	477,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #517 = ADD
  { 518,	2,	1,	4,	28,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #518 = ADDIUPC
  { 519,	2,	1,	4,	29,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo147, -1 ,nullptr },  // Inst #519 = ADDIUPC_MM
  { 520,	2,	1,	4,	28,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #520 = ADDIUPC_MMR6
  { 521,	2,	1,	2,	29,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo147, -1 ,nullptr },  // Inst #521 = ADDIUR1SP_MM
  { 522,	3,	1,	2,	29,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #522 = ADDIUR2_MM
  { 523,	3,	1,	2,	29,	0, 0x0ULL, nullptr, nullptr, OperandInfo149, -1 ,nullptr },  // Inst #523 = ADDIUS5_MM
  { 524,	1,	0,	2,	29,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #524 = ADDIUSP_MM
  { 525,	3,	1,	4,	29,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #525 = ADDIU_MMR6
  { 526,	3,	1,	4,	805,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #526 = ADDQH_PH
  { 527,	3,	1,	4,	968,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #527 = ADDQH_PH_MMR2
  { 528,	3,	1,	4,	806,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #528 = ADDQH_R_PH
  { 529,	3,	1,	4,	969,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #529 = ADDQH_R_PH_MMR2
  { 530,	3,	1,	4,	807,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #530 = ADDQH_R_W
  { 531,	3,	1,	4,	970,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #531 = ADDQH_R_W_MMR2
  { 532,	3,	1,	4,	808,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #532 = ADDQH_W
  { 533,	3,	1,	4,	971,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #533 = ADDQH_W_MMR2
  { 534,	3,	1,	4,	705,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #534 = ADDQ_PH
  { 535,	3,	1,	4,	852,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #535 = ADDQ_PH_MM
  { 536,	3,	1,	4,	706,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #536 = ADDQ_S_PH
  { 537,	3,	1,	4,	853,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #537 = ADDQ_S_PH_MM
  { 538,	3,	1,	4,	707,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #538 = ADDQ_S_W
  { 539,	3,	1,	4,	854,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #539 = ADDQ_S_W_MM
  { 540,	3,	1,	4,	708,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList9, OperandInfo58, -1 ,nullptr },  // Inst #540 = ADDSC
  { 541,	3,	1,	4,	855,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList9, OperandInfo58, -1 ,nullptr },  // Inst #541 = ADDSC_MM
  { 542,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #542 = ADDS_A_B
  { 543,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #543 = ADDS_A_D
  { 544,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #544 = ADDS_A_H
  { 545,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #545 = ADDS_A_W
  { 546,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #546 = ADDS_S_B
  { 547,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #547 = ADDS_S_D
  { 548,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #548 = ADDS_S_H
  { 549,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #549 = ADDS_S_W
  { 550,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #550 = ADDS_U_B
  { 551,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #551 = ADDS_U_D
  { 552,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #552 = ADDS_U_H
  { 553,	3,	1,	4,	519,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #553 = ADDS_U_W
  { 554,	3,	1,	2,	30,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo152, -1 ,nullptr },  // Inst #554 = ADDU16_MM
  { 555,	3,	1,	2,	30,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo152, -1 ,nullptr },  // Inst #555 = ADDU16_MMR6
  { 556,	3,	1,	4,	809,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #556 = ADDUH_QB
  { 557,	3,	1,	4,	972,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #557 = ADDUH_QB_MMR2
  { 558,	3,	1,	4,	810,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #558 = ADDUH_R_QB
  { 559,	3,	1,	4,	973,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #559 = ADDUH_R_QB_MMR2
  { 560,	3,	1,	4,	30,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #560 = ADDU_MMR6
  { 561,	3,	1,	4,	811,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #561 = ADDU_PH
  { 562,	3,	1,	4,	974,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #562 = ADDU_PH_MMR2
  { 563,	3,	1,	4,	709,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #563 = ADDU_QB
  { 564,	3,	1,	4,	856,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #564 = ADDU_QB_MM
  { 565,	3,	1,	4,	812,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #565 = ADDU_S_PH
  { 566,	3,	1,	4,	975,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #566 = ADDU_S_PH_MMR2
  { 567,	3,	1,	4,	710,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #567 = ADDU_S_QB
  { 568,	3,	1,	4,	857,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #568 = ADDU_S_QB_MM
  { 569,	3,	1,	4,	520,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #569 = ADDVI_B
  { 570,	3,	1,	4,	520,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #570 = ADDVI_D
  { 571,	3,	1,	4,	520,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #571 = ADDVI_H
  { 572,	3,	1,	4,	520,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #572 = ADDVI_W
  { 573,	3,	1,	4,	520,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #573 = ADDV_B
  { 574,	3,	1,	4,	520,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #574 = ADDV_D
  { 575,	3,	1,	4,	520,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #575 = ADDV_H
  { 576,	3,	1,	4,	520,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #576 = ADDV_W
  { 577,	3,	1,	4,	711,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList9, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #577 = ADDWC
  { 578,	3,	1,	4,	858,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList9, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #578 = ADDWC_MM
  { 579,	3,	1,	4,	518,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #579 = ADD_A_B
  { 580,	3,	1,	4,	518,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #580 = ADD_A_D
  { 581,	3,	1,	4,	518,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #581 = ADD_A_H
  { 582,	3,	1,	4,	518,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #582 = ADD_A_W
  { 583,	3,	1,	4,	27,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #583 = ADD_MM
  { 584,	3,	1,	4,	27,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #584 = ADD_MMR6
  { 585,	3,	1,	4,	478,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #585 = ADDi
  { 586,	3,	1,	4,	31,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #586 = ADDi_MM
  { 587,	3,	1,	4,	479,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #587 = ADDiu
  { 588,	3,	1,	4,	29,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #588 = ADDiu_MM
  { 589,	3,	1,	4,	490,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #589 = ADDu
  { 590,	3,	1,	4,	30,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #590 = ADDu_MM
  { 591,	4,	1,	4,	32,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo157, -1 ,nullptr },  // Inst #591 = ALIGN
  { 592,	4,	1,	4,	32,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo157, -1 ,nullptr },  // Inst #592 = ALIGN_MMR6
  { 593,	2,	1,	4,	33,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #593 = ALUIPC
  { 594,	2,	1,	4,	33,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #594 = ALUIPC_MMR6
  { 595,	3,	1,	4,	358,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #595 = AND
  { 596,	3,	1,	2,	34,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #596 = AND16_MM
  { 597,	3,	1,	2,	34,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #597 = AND16_MMR6
  { 598,	3,	1,	4,	34,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #598 = AND64
  { 599,	3,	1,	2,	34,	0, 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #599 = ANDI16_MM
  { 600,	3,	1,	2,	34,	0, 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #600 = ANDI16_MMR6
  { 601,	3,	1,	4,	529,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #601 = ANDI_B
  { 602,	3,	1,	4,	35,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #602 = ANDI_MMR6
  { 603,	3,	1,	4,	34,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #603 = AND_MM
  { 604,	3,	1,	4,	34,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #604 = AND_MMR6
  { 605,	3,	1,	4,	528,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #605 = AND_V
  { 606,	3,	1,	4,	480,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #606 = ANDi
  { 607,	3,	1,	4,	34,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #607 = ANDi64
  { 608,	3,	1,	4,	35,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #608 = ANDi_MM
  { 609,	4,	1,	4,	813,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #609 = APPEND
  { 610,	4,	1,	4,	976,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #610 = APPEND_MMR2
  { 611,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #611 = ASUB_S_B
  { 612,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #612 = ASUB_S_D
  { 613,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #613 = ASUB_S_H
  { 614,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #614 = ASUB_S_W
  { 615,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #615 = ASUB_U_B
  { 616,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #616 = ASUB_U_D
  { 617,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #617 = ASUB_U_H
  { 618,	3,	1,	4,	521,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #618 = ASUB_U_W
  { 619,	3,	1,	4,	36,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #619 = AUI
  { 620,	2,	1,	4,	37,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #620 = AUIPC
  { 621,	2,	1,	4,	37,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #621 = AUIPC_MMR6
  { 622,	3,	1,	4,	36,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #622 = AUI_MMR6
  { 623,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #623 = AVER_S_B
  { 624,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #624 = AVER_S_D
  { 625,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #625 = AVER_S_H
  { 626,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #626 = AVER_S_W
  { 627,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #627 = AVER_U_B
  { 628,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #628 = AVER_U_D
  { 629,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #629 = AVER_U_H
  { 630,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #630 = AVER_U_W
  { 631,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #631 = AVE_S_B
  { 632,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #632 = AVE_S_D
  { 633,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #633 = AVE_S_H
  { 634,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #634 = AVE_S_W
  { 635,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #635 = AVE_U_B
  { 636,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #636 = AVE_U_D
  { 637,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #637 = AVE_U_H
  { 638,	3,	1,	4,	522,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #638 = AVE_U_W
  { 639,	2,	1,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo160, -1 ,nullptr },  // Inst #639 = AddiuRxImmX16
  { 640,	2,	1,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo160, -1 ,nullptr },  // Inst #640 = AddiuRxPcImmX16
  { 641,	3,	1,	2,	38,	0|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo161, -1 ,nullptr },  // Inst #641 = AddiuRxRxImm16
  { 642,	3,	1,	4,	38,	0|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo161, -1 ,nullptr },  // Inst #642 = AddiuRxRxImmX16
  { 643,	3,	1,	4,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo162, -1 ,nullptr },  // Inst #643 = AddiuRxRyOffMemX16
  { 644,	1,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, OperandInfo2, -1 ,nullptr },  // Inst #644 = AddiuSpImm16
  { 645,	1,	0,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, OperandInfo2, -1 ,nullptr },  // Inst #645 = AddiuSpImmX16
  { 646,	3,	1,	2,	38,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo112, -1 ,nullptr },  // Inst #646 = AdduRxRyRz16
  { 647,	3,	1,	2,	38,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #647 = AndRxRxRy16
  { 648,	1,	0,	2,	2,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo41, -1 ,nullptr },  // Inst #648 = B16_MM
  { 649,	3,	1,	4,	39,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #649 = BADDu
  { 650,	1,	0,	4,	367,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo41, -1 ,nullptr },  // Inst #650 = BAL
  { 651,	1,	0,	4,	41,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo41, -1 ,nullptr },  // Inst #651 = BALC
  { 652,	1,	0,	4,	41,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo41, -1 ,nullptr },  // Inst #652 = BALC_MMR6
  { 653,	4,	1,	4,	814,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #653 = BALIGN
  { 654,	4,	1,	4,	977,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #654 = BALIGN_MMR2
  { 655,	3,	0,	4,	42,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x2ULL, nullptr, ImplicitList2, OperandInfo164, -1 ,nullptr },  // Inst #655 = BBIT0
  { 656,	3,	0,	4,	42,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x2ULL, nullptr, ImplicitList2, OperandInfo164, -1 ,nullptr },  // Inst #656 = BBIT032
  { 657,	3,	0,	4,	42,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x2ULL, nullptr, ImplicitList2, OperandInfo164, -1 ,nullptr },  // Inst #657 = BBIT1
  { 658,	3,	0,	4,	42,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x2ULL, nullptr, ImplicitList2, OperandInfo164, -1 ,nullptr },  // Inst #658 = BBIT132
  { 659,	1,	0,	4,	40,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #659 = BC
  { 660,	1,	0,	2,	40,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo41, -1 ,nullptr },  // Inst #660 = BC16_MMR6
  { 661,	2,	0,	4,	43,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo165, -1 ,nullptr },  // Inst #661 = BC1EQZ
  { 662,	2,	0,	4,	43,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo165, -1 ,nullptr },  // Inst #662 = BC1EQZC_MMR6
  { 663,	2,	0,	4,	662,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #663 = BC1F
  { 664,	2,	0,	4,	663,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #664 = BC1FL
  { 665,	2,	0,	4,	44,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #665 = BC1F_MM
  { 666,	2,	0,	4,	43,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo165, -1 ,nullptr },  // Inst #666 = BC1NEZ
  { 667,	2,	0,	4,	43,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo165, -1 ,nullptr },  // Inst #667 = BC1NEZC_MMR6
  { 668,	2,	0,	4,	664,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #668 = BC1T
  { 669,	2,	0,	4,	665,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #669 = BC1TL
  { 670,	2,	0,	4,	46,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x85ULL, nullptr, ImplicitList2, OperandInfo166, -1 ,nullptr },  // Inst #670 = BC1T_MM
  { 671,	2,	0,	4,	48,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo167, -1 ,nullptr },  // Inst #671 = BC2EQZ
  { 672,	2,	0,	4,	48,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo167, -1 ,nullptr },  // Inst #672 = BC2EQZC_MMR6
  { 673,	2,	0,	4,	48,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo167, -1 ,nullptr },  // Inst #673 = BC2NEZ
  { 674,	2,	0,	4,	48,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo167, -1 ,nullptr },  // Inst #674 = BC2NEZC_MMR6
  { 675,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #675 = BCLRI_B
  { 676,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #676 = BCLRI_D
  { 677,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #677 = BCLRI_H
  { 678,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #678 = BCLRI_W
  { 679,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #679 = BCLR_B
  { 680,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #680 = BCLR_D
  { 681,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #681 = BCLR_H
  { 682,	3,	1,	4,	502,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #682 = BCLR_W
  { 683,	1,	0,	4,	40,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x16ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #683 = BC_MMR6
  { 684,	3,	0,	4,	369,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #684 = BEQ
  { 685,	3,	0,	4,	49,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #685 = BEQ64
  { 686,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #686 = BEQC
  { 687,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #687 = BEQC64
  { 688,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #688 = BEQC_MMR6
  { 689,	3,	0,	4,	369,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #689 = BEQL
  { 690,	2,	0,	2,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo169, -1 ,nullptr },  // Inst #690 = BEQZ16_MM
  { 691,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #691 = BEQZALC
  { 692,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #692 = BEQZALC_MMR6
  { 693,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #693 = BEQZC
  { 694,	2,	0,	2,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo169, -1 ,nullptr },  // Inst #694 = BEQZC16_MMR6
  { 695,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #695 = BEQZC64
  { 696,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #696 = BEQZC_MM
  { 697,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #697 = BEQZC_MMR6
  { 698,	3,	0,	4,	49,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #698 = BEQ_MM
  { 699,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #699 = BGEC
  { 700,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #700 = BGEC64
  { 701,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #701 = BGEC_MMR6
  { 702,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #702 = BGEUC
  { 703,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #703 = BGEUC64
  { 704,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #704 = BGEUC_MMR6
  { 705,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #705 = BGEZ
  { 706,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #706 = BGEZ64
  { 707,	2,	0,	4,	368,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #707 = BGEZAL
  { 708,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #708 = BGEZALC
  { 709,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #709 = BGEZALC_MMR6
  { 710,	2,	0,	4,	368,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #710 = BGEZALL
  { 711,	2,	0,	4,	53,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #711 = BGEZALS_MM
  { 712,	2,	0,	4,	3,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #712 = BGEZAL_MM
  { 713,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #713 = BGEZC
  { 714,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #714 = BGEZC64
  { 715,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #715 = BGEZC_MMR6
  { 716,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #716 = BGEZL
  { 717,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #717 = BGEZ_MM
  { 718,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #718 = BGTZ
  { 719,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #719 = BGTZ64
  { 720,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #720 = BGTZALC
  { 721,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #721 = BGTZALC_MMR6
  { 722,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #722 = BGTZC
  { 723,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #723 = BGTZC64
  { 724,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #724 = BGTZC_MMR6
  { 725,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #725 = BGTZL
  { 726,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #726 = BGTZ_MM
  { 727,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #727 = BINSLI_B
  { 728,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo173, -1 ,nullptr },  // Inst #728 = BINSLI_D
  { 729,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo174, -1 ,nullptr },  // Inst #729 = BINSLI_H
  { 730,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo175, -1 ,nullptr },  // Inst #730 = BINSLI_W
  { 731,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #731 = BINSL_B
  { 732,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #732 = BINSL_D
  { 733,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #733 = BINSL_H
  { 734,	4,	1,	4,	497,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #734 = BINSL_W
  { 735,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #735 = BINSRI_B
  { 736,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo173, -1 ,nullptr },  // Inst #736 = BINSRI_D
  { 737,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo174, -1 ,nullptr },  // Inst #737 = BINSRI_H
  { 738,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo175, -1 ,nullptr },  // Inst #738 = BINSRI_W
  { 739,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #739 = BINSR_B
  { 740,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #740 = BINSR_D
  { 741,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #741 = BINSR_H
  { 742,	4,	1,	4,	498,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #742 = BINSR_W
  { 743,	2,	1,	4,	712,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #743 = BITREV
  { 744,	2,	1,	4,	859,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #744 = BITREV_MM
  { 745,	2,	1,	4,	54,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #745 = BITSWAP
  { 746,	2,	1,	4,	54,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #746 = BITSWAP_MMR6
  { 747,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #747 = BLEZ
  { 748,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #748 = BLEZ64
  { 749,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #749 = BLEZALC
  { 750,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #750 = BLEZALC_MMR6
  { 751,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #751 = BLEZC
  { 752,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #752 = BLEZC64
  { 753,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #753 = BLEZC_MMR6
  { 754,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #754 = BLEZL
  { 755,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #755 = BLEZ_MM
  { 756,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #756 = BLTC
  { 757,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #757 = BLTC64
  { 758,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #758 = BLTC_MMR6
  { 759,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #759 = BLTUC
  { 760,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #760 = BLTUC64
  { 761,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #761 = BLTUC_MMR6
  { 762,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #762 = BLTZ
  { 763,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #763 = BLTZ64
  { 764,	2,	0,	4,	368,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #764 = BLTZAL
  { 765,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #765 = BLTZALC
  { 766,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #766 = BLTZALC_MMR6
  { 767,	2,	0,	4,	368,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #767 = BLTZALL
  { 768,	2,	0,	4,	53,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #768 = BLTZALS_MM
  { 769,	2,	0,	4,	3,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #769 = BLTZAL_MM
  { 770,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #770 = BLTZC
  { 771,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #771 = BLTZC64
  { 772,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #772 = BLTZC_MMR6
  { 773,	2,	0,	4,	370,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #773 = BLTZL
  { 774,	2,	0,	4,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #774 = BLTZ_MM
  { 775,	4,	1,	4,	505,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #775 = BMNZI_B
  { 776,	4,	1,	4,	505,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #776 = BMNZ_V
  { 777,	4,	1,	4,	505,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #777 = BMZI_B
  { 778,	4,	1,	4,	505,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #778 = BMZ_V
  { 779,	3,	0,	4,	369,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #779 = BNE
  { 780,	3,	0,	4,	49,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #780 = BNE64
  { 781,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #781 = BNEC
  { 782,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo168, -1 ,nullptr },  // Inst #782 = BNEC64
  { 783,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #783 = BNEC_MMR6
  { 784,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #784 = BNEGI_B
  { 785,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #785 = BNEGI_D
  { 786,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #786 = BNEGI_H
  { 787,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #787 = BNEGI_W
  { 788,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #788 = BNEG_B
  { 789,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #789 = BNEG_D
  { 790,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #790 = BNEG_H
  { 791,	3,	1,	4,	503,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #791 = BNEG_W
  { 792,	3,	0,	4,	369,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #792 = BNEL
  { 793,	2,	0,	2,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo169, -1 ,nullptr },  // Inst #793 = BNEZ16_MM
  { 794,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #794 = BNEZALC
  { 795,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo170, -1 ,nullptr },  // Inst #795 = BNEZALC_MMR6
  { 796,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #796 = BNEZC
  { 797,	2,	0,	2,	51,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList2, OperandInfo169, -1 ,nullptr },  // Inst #797 = BNEZC16_MMR6
  { 798,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo171, -1 ,nullptr },  // Inst #798 = BNEZC64
  { 799,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #799 = BNEZC_MM
  { 800,	2,	0,	4,	52,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x36ULL, nullptr, ImplicitList2, OperandInfo170, -1 ,nullptr },  // Inst #800 = BNEZC_MMR6
  { 801,	3,	0,	4,	49,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x12ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #801 = BNE_MM
  { 802,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #802 = BNVC
  { 803,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #803 = BNVC_MMR6
  { 804,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo177, -1 ,nullptr },  // Inst #804 = BNZ_B
  { 805,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo178, -1 ,nullptr },  // Inst #805 = BNZ_D
  { 806,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo179, -1 ,nullptr },  // Inst #806 = BNZ_H
  { 807,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo177, -1 ,nullptr },  // Inst #807 = BNZ_V
  { 808,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo180, -1 ,nullptr },  // Inst #808 = BNZ_W
  { 809,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x36ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #809 = BOVC
  { 810,	3,	0,	4,	50,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo43, -1 ,nullptr },  // Inst #810 = BOVC_MMR6
  { 811,	1,	0,	4,	713,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #811 = BPOSGE32
  { 812,	1,	0,	4,	1013,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #812 = BPOSGE32C_MMR3
  { 813,	1,	0,	4,	860,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #813 = BPOSGE32_MM
  { 814,	2,	0,	4,	371,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo7, -1 ,nullptr },  // Inst #814 = BREAK
  { 815,	1,	0,	2,	55,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #815 = BREAK16_MM
  { 816,	1,	0,	2,	55,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #816 = BREAK16_MMR6
  { 817,	2,	0,	4,	55,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo7, -1 ,nullptr },  // Inst #817 = BREAK_MM
  { 818,	2,	0,	4,	55,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo7, -1 ,nullptr },  // Inst #818 = BREAK_MMR6
  { 819,	4,	1,	4,	504,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #819 = BSELI_B
  { 820,	4,	1,	4,	504,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #820 = BSEL_V
  { 821,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #821 = BSETI_B
  { 822,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #822 = BSETI_D
  { 823,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #823 = BSETI_H
  { 824,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #824 = BSETI_W
  { 825,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #825 = BSET_B
  { 826,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #826 = BSET_D
  { 827,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #827 = BSET_H
  { 828,	3,	1,	4,	501,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #828 = BSET_W
  { 829,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo177, -1 ,nullptr },  // Inst #829 = BZ_B
  { 830,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo178, -1 ,nullptr },  // Inst #830 = BZ_D
  { 831,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo179, -1 ,nullptr },  // Inst #831 = BZ_H
  { 832,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo177, -1 ,nullptr },  // Inst #832 = BZ_V
  { 833,	2,	0,	4,	508,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo180, -1 ,nullptr },  // Inst #833 = BZ_W
  { 834,	2,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo181, -1 ,nullptr },  // Inst #834 = BeqzRxImm16
  { 835,	2,	0,	4,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo181, -1 ,nullptr },  // Inst #835 = BeqzRxImmX16
  { 836,	1,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #836 = Bimm16
  { 837,	1,	0,	4,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo41, -1 ,nullptr },  // Inst #837 = BimmX16
  { 838,	2,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo181, -1 ,nullptr },  // Inst #838 = BnezRxImm16
  { 839,	2,	0,	4,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo181, -1 ,nullptr },  // Inst #839 = BnezRxImmX16
  { 840,	0,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #840 = Break16
  { 841,	1,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList7, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #841 = Bteqz16
  { 842,	1,	0,	4,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList7, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #842 = BteqzX16
  { 843,	1,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList7, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #843 = Btnez16
  { 844,	1,	0,	4,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList7, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #844 = BtnezX16
  { 845,	3,	0,	4,	451,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #845 = CACHE
  { 846,	3,	0,	4,	452,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #846 = CACHEE
  { 847,	3,	0,	4,	57,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #847 = CACHEE_MM
  { 848,	3,	0,	4,	56,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #848 = CACHE_MM
  { 849,	3,	0,	4,	56,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #849 = CACHE_MMR6
  { 850,	3,	0,	4,	56,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #850 = CACHE_R6
  { 851,	2,	1,	4,	687,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #851 = CEIL_L_D64
  { 852,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #852 = CEIL_L_D_MMR6
  { 853,	2,	1,	4,	687,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #853 = CEIL_L_S
  { 854,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #854 = CEIL_L_S_MMR6
  { 855,	2,	1,	4,	687,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #855 = CEIL_W_D32
  { 856,	2,	1,	4,	687,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #856 = CEIL_W_D64
  { 857,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #857 = CEIL_W_D_MMR6
  { 858,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #858 = CEIL_W_MM
  { 859,	2,	1,	4,	687,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #859 = CEIL_W_S
  { 860,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #860 = CEIL_W_S_MM
  { 861,	2,	1,	4,	58,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #861 = CEIL_W_S_MMR6
  { 862,	3,	1,	4,	533,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #862 = CEQI_B
  { 863,	3,	1,	4,	533,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #863 = CEQI_D
  { 864,	3,	1,	4,	533,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #864 = CEQI_H
  { 865,	3,	1,	4,	533,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #865 = CEQI_W
  { 866,	3,	1,	4,	533,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #866 = CEQ_B
  { 867,	3,	1,	4,	533,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #867 = CEQ_D
  { 868,	3,	1,	4,	533,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #868 = CEQ_H
  { 869,	3,	1,	4,	533,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #869 = CEQ_W
  { 870,	2,	1,	4,	666,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo188, -1 ,nullptr },  // Inst #870 = CFC1
  { 871,	2,	1,	4,	59,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo188, -1 ,nullptr },  // Inst #871 = CFC1_MM
  { 872,	2,	1,	4,	60,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo189, -1 ,nullptr },  // Inst #872 = CFC2_MM
  { 873,	2,	1,	4,	509,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo190, -1 ,nullptr },  // Inst #873 = CFCMSA
  { 874,	4,	1,	4,	61,	0, 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #874 = CINS
  { 875,	4,	1,	4,	61,	0, 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #875 = CINS32
  { 876,	4,	1,	4,	61,	0, 0x1ULL, nullptr, nullptr, OperandInfo192, -1 ,nullptr },  // Inst #876 = CINS64_32
  { 877,	4,	1,	4,	61,	0, 0x1ULL, nullptr, nullptr, OperandInfo193, -1 ,nullptr },  // Inst #877 = CINS_i32
  { 878,	2,	1,	4,	62,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #878 = CLASS_D
  { 879,	2,	1,	4,	63,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #879 = CLASS_D_MMR6
  { 880,	2,	1,	4,	63,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #880 = CLASS_S
  { 881,	2,	1,	4,	63,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #881 = CLASS_S_MMR6
  { 882,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #882 = CLEI_S_B
  { 883,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #883 = CLEI_S_D
  { 884,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #884 = CLEI_S_H
  { 885,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #885 = CLEI_S_W
  { 886,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #886 = CLEI_U_B
  { 887,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #887 = CLEI_U_D
  { 888,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #888 = CLEI_U_H
  { 889,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #889 = CLEI_U_W
  { 890,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #890 = CLE_S_B
  { 891,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #891 = CLE_S_D
  { 892,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #892 = CLE_S_H
  { 893,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #893 = CLE_S_W
  { 894,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #894 = CLE_U_B
  { 895,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #895 = CLE_U_D
  { 896,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #896 = CLE_U_H
  { 897,	3,	1,	4,	532,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #897 = CLE_U_W
  { 898,	2,	1,	4,	455,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #898 = CLO
  { 899,	2,	1,	4,	64,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #899 = CLO_MM
  { 900,	2,	1,	4,	64,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #900 = CLO_MMR6
  { 901,	2,	1,	4,	64,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #901 = CLO_R6
  { 902,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #902 = CLTI_S_B
  { 903,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #903 = CLTI_S_D
  { 904,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #904 = CLTI_S_H
  { 905,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #905 = CLTI_S_W
  { 906,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #906 = CLTI_U_B
  { 907,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #907 = CLTI_U_D
  { 908,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #908 = CLTI_U_H
  { 909,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #909 = CLTI_U_W
  { 910,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #910 = CLT_S_B
  { 911,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #911 = CLT_S_D
  { 912,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #912 = CLT_S_H
  { 913,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #913 = CLT_S_W
  { 914,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #914 = CLT_U_B
  { 915,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #915 = CLT_U_D
  { 916,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #916 = CLT_U_H
  { 917,	3,	1,	4,	531,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #917 = CLT_U_W
  { 918,	2,	1,	4,	456,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #918 = CLZ
  { 919,	2,	1,	4,	65,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #919 = CLZ_MM
  { 920,	2,	1,	4,	65,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #920 = CLZ_MMR6
  { 921,	2,	1,	4,	65,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #921 = CLZ_R6
  { 922,	3,	1,	4,	815,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #922 = CMPGDU_EQ_QB
  { 923,	3,	1,	4,	978,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #923 = CMPGDU_EQ_QB_MMR2
  { 924,	3,	1,	4,	816,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #924 = CMPGDU_LE_QB
  { 925,	3,	1,	4,	979,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #925 = CMPGDU_LE_QB_MMR2
  { 926,	3,	1,	4,	817,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #926 = CMPGDU_LT_QB
  { 927,	3,	1,	4,	980,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo194, -1 ,nullptr },  // Inst #927 = CMPGDU_LT_QB_MMR2
  { 928,	3,	1,	4,	714,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #928 = CMPGU_EQ_QB
  { 929,	3,	1,	4,	861,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #929 = CMPGU_EQ_QB_MM
  { 930,	3,	1,	4,	715,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #930 = CMPGU_LE_QB
  { 931,	3,	1,	4,	862,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #931 = CMPGU_LE_QB_MM
  { 932,	3,	1,	4,	716,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #932 = CMPGU_LT_QB
  { 933,	3,	1,	4,	863,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo194, -1 ,nullptr },  // Inst #933 = CMPGU_LT_QB_MM
  { 934,	2,	0,	4,	717,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #934 = CMPU_EQ_QB
  { 935,	2,	0,	4,	864,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #935 = CMPU_EQ_QB_MM
  { 936,	2,	0,	4,	718,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #936 = CMPU_LE_QB
  { 937,	2,	0,	4,	865,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #937 = CMPU_LE_QB_MM
  { 938,	2,	0,	4,	719,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #938 = CMPU_LT_QB
  { 939,	2,	0,	4,	866,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #939 = CMPU_LT_QB_MM
  { 940,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #940 = CMP_AF_D_MMR6
  { 941,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #941 = CMP_AF_S_MMR6
  { 942,	3,	1,	4,	538,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #942 = CMP_EQ_D
  { 943,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #943 = CMP_EQ_D_MMR6
  { 944,	2,	0,	4,	720,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #944 = CMP_EQ_PH
  { 945,	2,	0,	4,	867,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #945 = CMP_EQ_PH_MM
  { 946,	3,	1,	4,	539,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #946 = CMP_EQ_S
  { 947,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #947 = CMP_EQ_S_MMR6
  { 948,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #948 = CMP_F_D
  { 949,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #949 = CMP_F_S
  { 950,	3,	1,	4,	544,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #950 = CMP_LE_D
  { 951,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #951 = CMP_LE_D_MMR6
  { 952,	2,	0,	4,	721,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #952 = CMP_LE_PH
  { 953,	2,	0,	4,	868,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #953 = CMP_LE_PH_MM
  { 954,	3,	1,	4,	545,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #954 = CMP_LE_S
  { 955,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #955 = CMP_LE_S_MMR6
  { 956,	3,	1,	4,	540,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #956 = CMP_LT_D
  { 957,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #957 = CMP_LT_D_MMR6
  { 958,	2,	0,	4,	722,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #958 = CMP_LT_PH
  { 959,	2,	0,	4,	869,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList10, OperandInfo146, -1 ,nullptr },  // Inst #959 = CMP_LT_PH_MM
  { 960,	3,	1,	4,	541,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #960 = CMP_LT_S
  { 961,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #961 = CMP_LT_S_MMR6
  { 962,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #962 = CMP_SAF_D
  { 963,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #963 = CMP_SAF_D_MMR6
  { 964,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #964 = CMP_SAF_S
  { 965,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #965 = CMP_SAF_S_MMR6
  { 966,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #966 = CMP_SEQ_D
  { 967,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #967 = CMP_SEQ_D_MMR6
  { 968,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #968 = CMP_SEQ_S
  { 969,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #969 = CMP_SEQ_S_MMR6
  { 970,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #970 = CMP_SLE_D
  { 971,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #971 = CMP_SLE_D_MMR6
  { 972,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #972 = CMP_SLE_S
  { 973,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #973 = CMP_SLE_S_MMR6
  { 974,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #974 = CMP_SLT_D
  { 975,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #975 = CMP_SLT_D_MMR6
  { 976,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #976 = CMP_SLT_S
  { 977,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #977 = CMP_SLT_S_MMR6
  { 978,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #978 = CMP_SUEQ_D
  { 979,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #979 = CMP_SUEQ_D_MMR6
  { 980,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #980 = CMP_SUEQ_S
  { 981,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #981 = CMP_SUEQ_S_MMR6
  { 982,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #982 = CMP_SULE_D
  { 983,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #983 = CMP_SULE_D_MMR6
  { 984,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #984 = CMP_SULE_S
  { 985,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #985 = CMP_SULE_S_MMR6
  { 986,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #986 = CMP_SULT_D
  { 987,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #987 = CMP_SULT_D_MMR6
  { 988,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #988 = CMP_SULT_S
  { 989,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #989 = CMP_SULT_S_MMR6
  { 990,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #990 = CMP_SUN_D
  { 991,	3,	1,	4,	66,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #991 = CMP_SUN_D_MMR6
  { 992,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #992 = CMP_SUN_S
  { 993,	3,	1,	4,	67,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #993 = CMP_SUN_S_MMR6
  { 994,	3,	1,	4,	536,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #994 = CMP_UEQ_D
  { 995,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #995 = CMP_UEQ_D_MMR6
  { 996,	3,	1,	4,	537,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #996 = CMP_UEQ_S
  { 997,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #997 = CMP_UEQ_S_MMR6
  { 998,	3,	1,	4,	546,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #998 = CMP_ULE_D
  { 999,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #999 = CMP_ULE_D_MMR6
  { 1000,	3,	1,	4,	547,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1000 = CMP_ULE_S
  { 1001,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1001 = CMP_ULE_S_MMR6
  { 1002,	3,	1,	4,	542,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #1002 = CMP_ULT_D
  { 1003,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #1003 = CMP_ULT_D_MMR6
  { 1004,	3,	1,	4,	543,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1004 = CMP_ULT_S
  { 1005,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1005 = CMP_ULT_S_MMR6
  { 1006,	3,	1,	4,	534,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #1006 = CMP_UN_D
  { 1007,	3,	1,	4,	66,	0, 0x16ULL, nullptr, nullptr, OperandInfo195, -1 ,nullptr },  // Inst #1007 = CMP_UN_D_MMR6
  { 1008,	3,	1,	4,	535,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1008 = CMP_UN_S
  { 1009,	3,	1,	4,	67,	0, 0x16ULL, nullptr, nullptr, OperandInfo196, -1 ,nullptr },  // Inst #1009 = CMP_UN_S_MMR6
  { 1010,	3,	1,	4,	661,	0, 0x6ULL, nullptr, nullptr, OperandInfo197, -1 ,nullptr },  // Inst #1010 = COPY_S_B
  { 1011,	3,	1,	4,	661,	0, 0x6ULL, nullptr, nullptr, OperandInfo198, -1 ,nullptr },  // Inst #1011 = COPY_S_D
  { 1012,	3,	1,	4,	661,	0, 0x6ULL, nullptr, nullptr, OperandInfo199, -1 ,nullptr },  // Inst #1012 = COPY_S_H
  { 1013,	3,	1,	4,	661,	0, 0x6ULL, nullptr, nullptr, OperandInfo200, -1 ,nullptr },  // Inst #1013 = COPY_S_W
  { 1014,	3,	1,	4,	660,	0, 0x6ULL, nullptr, nullptr, OperandInfo197, -1 ,nullptr },  // Inst #1014 = COPY_U_B
  { 1015,	3,	1,	4,	660,	0, 0x6ULL, nullptr, nullptr, OperandInfo199, -1 ,nullptr },  // Inst #1015 = COPY_U_H
  { 1016,	3,	1,	4,	660,	0, 0x6ULL, nullptr, nullptr, OperandInfo200, -1 ,nullptr },  // Inst #1016 = COPY_U_W
  { 1017,	3,	1,	4,	68,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1017 = CRC32B
  { 1018,	3,	1,	4,	69,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1018 = CRC32CB
  { 1019,	3,	1,	4,	70,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1019 = CRC32CD
  { 1020,	3,	1,	4,	71,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1020 = CRC32CH
  { 1021,	3,	1,	4,	72,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1021 = CRC32CW
  { 1022,	3,	1,	4,	73,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1022 = CRC32D
  { 1023,	3,	1,	4,	74,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1023 = CRC32H
  { 1024,	3,	1,	4,	75,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1024 = CRC32W
  { 1025,	2,	1,	4,	657,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo201, -1 ,nullptr },  // Inst #1025 = CTC1
  { 1026,	2,	1,	4,	76,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo201, -1 ,nullptr },  // Inst #1026 = CTC1_MM
  { 1027,	2,	1,	4,	77,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo202, -1 ,nullptr },  // Inst #1027 = CTC2_MM
  { 1028,	2,	0,	4,	509,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo203, -1 ,nullptr },  // Inst #1028 = CTCMSA
  { 1029,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo204, -1 ,nullptr },  // Inst #1029 = CVT_D32_S
  { 1030,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo204, -1 ,nullptr },  // Inst #1030 = CVT_D32_S_MM
  { 1031,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo204, -1 ,nullptr },  // Inst #1031 = CVT_D32_W
  { 1032,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo204, -1 ,nullptr },  // Inst #1032 = CVT_D32_W_MM
  { 1033,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1033 = CVT_D64_L
  { 1034,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1034 = CVT_D64_S
  { 1035,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1035 = CVT_D64_S_MM
  { 1036,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1036 = CVT_D64_W
  { 1037,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1037 = CVT_D64_W_MM
  { 1038,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1038 = CVT_D_L_MMR6
  { 1039,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1039 = CVT_L_D64
  { 1040,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1040 = CVT_L_D64_MM
  { 1041,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1041 = CVT_L_D_MMR6
  { 1042,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1042 = CVT_L_S
  { 1043,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1043 = CVT_L_S_MM
  { 1044,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1044 = CVT_L_S_MMR6
  { 1045,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1045 = CVT_S_D32
  { 1046,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1046 = CVT_S_D32_MM
  { 1047,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1047 = CVT_S_D64
  { 1048,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1048 = CVT_S_D64_MM
  { 1049,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1049 = CVT_S_L
  { 1050,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1050 = CVT_S_L_MMR6
  { 1051,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1051 = CVT_S_W
  { 1052,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1052 = CVT_S_W_MM
  { 1053,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1053 = CVT_S_W_MMR6
  { 1054,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1054 = CVT_W_D32
  { 1055,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1055 = CVT_W_D32_MM
  { 1056,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1056 = CVT_W_D64
  { 1057,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1057 = CVT_W_D64_MM
  { 1058,	2,	1,	4,	612,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1058 = CVT_W_S
  { 1059,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1059 = CVT_W_S_MM
  { 1060,	2,	1,	4,	7,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1060 = CVT_W_S_MMR6
  { 1061,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1061 = C_EQ_D32
  { 1062,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1062 = C_EQ_D32_MM
  { 1063,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1063 = C_EQ_D64
  { 1064,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1064 = C_EQ_D64_MM
  { 1065,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1065 = C_EQ_S
  { 1066,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1066 = C_EQ_S_MM
  { 1067,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1067 = C_F_D32
  { 1068,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1068 = C_F_D32_MM
  { 1069,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1069 = C_F_D64
  { 1070,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1070 = C_F_D64_MM
  { 1071,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1071 = C_F_S
  { 1072,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1072 = C_F_S_MM
  { 1073,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1073 = C_LE_D32
  { 1074,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1074 = C_LE_D32_MM
  { 1075,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1075 = C_LE_D64
  { 1076,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1076 = C_LE_D64_MM
  { 1077,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1077 = C_LE_S
  { 1078,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1078 = C_LE_S_MM
  { 1079,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1079 = C_LT_D32
  { 1080,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1080 = C_LT_D32_MM
  { 1081,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1081 = C_LT_D64
  { 1082,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1082 = C_LT_D64_MM
  { 1083,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1083 = C_LT_S
  { 1084,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1084 = C_LT_S_MM
  { 1085,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1085 = C_NGE_D32
  { 1086,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1086 = C_NGE_D32_MM
  { 1087,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1087 = C_NGE_D64
  { 1088,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1088 = C_NGE_D64_MM
  { 1089,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1089 = C_NGE_S
  { 1090,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1090 = C_NGE_S_MM
  { 1091,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1091 = C_NGLE_D32
  { 1092,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1092 = C_NGLE_D32_MM
  { 1093,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1093 = C_NGLE_D64
  { 1094,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1094 = C_NGLE_D64_MM
  { 1095,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1095 = C_NGLE_S
  { 1096,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1096 = C_NGLE_S_MM
  { 1097,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1097 = C_NGL_D32
  { 1098,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1098 = C_NGL_D32_MM
  { 1099,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1099 = C_NGL_D64
  { 1100,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1100 = C_NGL_D64_MM
  { 1101,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1101 = C_NGL_S
  { 1102,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1102 = C_NGL_S_MM
  { 1103,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1103 = C_NGT_D32
  { 1104,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1104 = C_NGT_D32_MM
  { 1105,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1105 = C_NGT_D64
  { 1106,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1106 = C_NGT_D64_MM
  { 1107,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1107 = C_NGT_S
  { 1108,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1108 = C_NGT_S_MM
  { 1109,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1109 = C_OLE_D32
  { 1110,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1110 = C_OLE_D32_MM
  { 1111,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1111 = C_OLE_D64
  { 1112,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1112 = C_OLE_D64_MM
  { 1113,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1113 = C_OLE_S
  { 1114,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1114 = C_OLE_S_MM
  { 1115,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1115 = C_OLT_D32
  { 1116,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1116 = C_OLT_D32_MM
  { 1117,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1117 = C_OLT_D64
  { 1118,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1118 = C_OLT_D64_MM
  { 1119,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1119 = C_OLT_S
  { 1120,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1120 = C_OLT_S_MM
  { 1121,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1121 = C_SEQ_D32
  { 1122,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1122 = C_SEQ_D32_MM
  { 1123,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1123 = C_SEQ_D64
  { 1124,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1124 = C_SEQ_D64_MM
  { 1125,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1125 = C_SEQ_S
  { 1126,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1126 = C_SEQ_S_MM
  { 1127,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1127 = C_SF_D32
  { 1128,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1128 = C_SF_D32_MM
  { 1129,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1129 = C_SF_D64
  { 1130,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1130 = C_SF_D64_MM
  { 1131,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1131 = C_SF_S
  { 1132,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1132 = C_SF_S_MM
  { 1133,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1133 = C_UEQ_D32
  { 1134,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1134 = C_UEQ_D32_MM
  { 1135,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1135 = C_UEQ_D64
  { 1136,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1136 = C_UEQ_D64_MM
  { 1137,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1137 = C_UEQ_S
  { 1138,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1138 = C_UEQ_S_MM
  { 1139,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1139 = C_ULE_D32
  { 1140,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1140 = C_ULE_D32_MM
  { 1141,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1141 = C_ULE_D64
  { 1142,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1142 = C_ULE_D64_MM
  { 1143,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1143 = C_ULE_S
  { 1144,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1144 = C_ULE_S_MM
  { 1145,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1145 = C_ULT_D32
  { 1146,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1146 = C_ULT_D32_MM
  { 1147,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1147 = C_ULT_D64
  { 1148,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1148 = C_ULT_D64_MM
  { 1149,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1149 = C_ULT_S
  { 1150,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1150 = C_ULT_S_MM
  { 1151,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1151 = C_UN_D32
  { 1152,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo205, -1 ,nullptr },  // Inst #1152 = C_UN_D32_MM
  { 1153,	3,	1,	4,	613,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1153 = C_UN_D64
  { 1154,	3,	1,	4,	78,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo206, -1 ,nullptr },  // Inst #1154 = C_UN_D64_MM
  { 1155,	3,	1,	4,	614,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1155 = C_UN_S
  { 1156,	3,	1,	4,	79,	0|(1ULL<<MCID::Compare)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x84ULL, nullptr, nullptr, OperandInfo207, -1 ,nullptr },  // Inst #1156 = C_UN_S_MM
  { 1157,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo111, -1 ,nullptr },  // Inst #1157 = CmpRxRy16
  { 1158,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #1158 = CmpiRxImm16
  { 1159,	2,	0,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #1159 = CmpiRxImmX16
  { 1160,	3,	1,	4,	80,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1160 = DADD
  { 1161,	3,	1,	4,	81,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1161 = DADDi
  { 1162,	3,	1,	4,	82,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1162 = DADDiu
  { 1163,	3,	1,	4,	83,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1163 = DADDu
  { 1164,	3,	1,	4,	84,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo208, -1 ,nullptr },  // Inst #1164 = DAHI
  { 1165,	4,	1,	4,	85,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo209, -1 ,nullptr },  // Inst #1165 = DALIGN
  { 1166,	3,	1,	4,	86,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo208, -1 ,nullptr },  // Inst #1166 = DATI
  { 1167,	3,	1,	4,	87,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1167 = DAUI
  { 1168,	2,	1,	4,	88,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1168 = DBITSWAP
  { 1169,	2,	1,	4,	89,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1169 = DCLO
  { 1170,	2,	1,	4,	89,	0, 0x6ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1170 = DCLO_R6
  { 1171,	2,	1,	4,	90,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1171 = DCLZ
  { 1172,	2,	1,	4,	90,	0, 0x6ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1172 = DCLZ_R6
  { 1173,	3,	1,	4,	10,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1173 = DDIV
  { 1174,	3,	1,	4,	11,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1174 = DDIVU
  { 1175,	0,	0,	4,	372,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1175 = DERET
  { 1176,	0,	0,	4,	91,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1176 = DERET_MM
  { 1177,	0,	0,	4,	91,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1177 = DERET_MMR6
  { 1178,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #1178 = DEXT
  { 1179,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo192, -1 ,nullptr },  // Inst #1179 = DEXT64_32
  { 1180,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #1180 = DEXTM
  { 1181,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #1181 = DEXTU
  { 1182,	1,	1,	4,	457,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1182 = DI
  { 1183,	5,	1,	4,	61,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo210, -1 ,nullptr },  // Inst #1183 = DINS
  { 1184,	5,	1,	4,	61,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo210, -1 ,nullptr },  // Inst #1184 = DINSM
  { 1185,	5,	1,	4,	61,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo210, -1 ,nullptr },  // Inst #1185 = DINSU
  { 1186,	3,	1,	4,	465,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1186 = DIV
  { 1187,	3,	1,	4,	466,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1187 = DIVU
  { 1188,	3,	1,	4,	23,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1188 = DIVU_MMR6
  { 1189,	3,	1,	4,	22,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1189 = DIV_MMR6
  { 1190,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1190 = DIV_S_B
  { 1191,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1191 = DIV_S_D
  { 1192,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1192 = DIV_S_H
  { 1193,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1193 = DIV_S_W
  { 1194,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1194 = DIV_U_B
  { 1195,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1195 = DIV_U_D
  { 1196,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1196 = DIV_U_H
  { 1197,	3,	1,	4,	591,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1197 = DIV_U_W
  { 1198,	1,	1,	4,	93,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1198 = DI_MM
  { 1199,	1,	1,	4,	93,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1199 = DI_MMR6
  { 1200,	4,	1,	4,	94,	0, 0x6ULL, nullptr, nullptr, OperandInfo209, -1 ,nullptr },  // Inst #1200 = DLSA
  { 1201,	4,	1,	4,	94,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo209, -1 ,nullptr },  // Inst #1201 = DLSA_R6
  { 1202,	3,	1,	4,	95,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo211, -1 ,nullptr },  // Inst #1202 = DMFC0
  { 1203,	2,	1,	4,	96,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo212, -1 ,nullptr },  // Inst #1203 = DMFC1
  { 1204,	3,	1,	4,	97,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo213, -1 ,nullptr },  // Inst #1204 = DMFC2
  { 1205,	2,	2,	4,	97,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #1205 = DMFC2_OCTEON
  { 1206,	3,	1,	4,	98,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo211, -1 ,nullptr },  // Inst #1206 = DMFGC0
  { 1207,	3,	1,	4,	99,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1207 = DMOD
  { 1208,	3,	1,	4,	100,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1208 = DMODU
  { 1209,	1,	1,	4,	101,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1209 = DMT
  { 1210,	3,	1,	4,	102,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo214, -1 ,nullptr },  // Inst #1210 = DMTC0
  { 1211,	2,	1,	4,	103,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo115, -1 ,nullptr },  // Inst #1211 = DMTC1
  { 1212,	3,	1,	4,	104,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo215, -1 ,nullptr },  // Inst #1212 = DMTC2
  { 1213,	2,	2,	4,	104,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #1213 = DMTC2_OCTEON
  { 1214,	3,	1,	4,	105,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo214, -1 ,nullptr },  // Inst #1214 = DMTGC0
  { 1215,	3,	1,	4,	106,	0, 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1215 = DMUH
  { 1216,	3,	1,	4,	107,	0, 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1216 = DMUHU
  { 1217,	3,	1,	4,	108,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, ImplicitList11, OperandInfo57, -1 ,nullptr },  // Inst #1217 = DMUL
  { 1218,	2,	0,	4,	8,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList12, OperandInfo103, -1 ,nullptr },  // Inst #1218 = DMULT
  { 1219,	2,	0,	4,	9,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList12, OperandInfo103, -1 ,nullptr },  // Inst #1219 = DMULTu
  { 1220,	3,	1,	4,	108,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1220 = DMULU
  { 1221,	3,	1,	4,	108,	0, 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1221 = DMUL_R6
  { 1222,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1222 = DOTP_S_D
  { 1223,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1223 = DOTP_S_H
  { 1224,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1224 = DOTP_S_W
  { 1225,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1225 = DOTP_U_D
  { 1226,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1226 = DOTP_U_H
  { 1227,	3,	1,	4,	639,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1227 = DOTP_U_W
  { 1228,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo219, -1 ,nullptr },  // Inst #1228 = DPADD_S_D
  { 1229,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo220, -1 ,nullptr },  // Inst #1229 = DPADD_S_H
  { 1230,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo221, -1 ,nullptr },  // Inst #1230 = DPADD_S_W
  { 1231,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo219, -1 ,nullptr },  // Inst #1231 = DPADD_U_D
  { 1232,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo220, -1 ,nullptr },  // Inst #1232 = DPADD_U_H
  { 1233,	4,	1,	4,	637,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo221, -1 ,nullptr },  // Inst #1233 = DPADD_U_W
  { 1234,	4,	1,	4,	819,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1234 = DPAQX_SA_W_PH
  { 1235,	4,	1,	4,	982,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1235 = DPAQX_SA_W_PH_MMR2
  { 1236,	4,	1,	4,	820,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1236 = DPAQX_S_W_PH
  { 1237,	4,	1,	4,	983,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1237 = DPAQX_S_W_PH_MMR2
  { 1238,	4,	1,	4,	723,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1238 = DPAQ_SA_L_W
  { 1239,	4,	1,	4,	870,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1239 = DPAQ_SA_L_W_MM
  { 1240,	4,	1,	4,	724,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1240 = DPAQ_S_W_PH
  { 1241,	4,	1,	4,	871,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1241 = DPAQ_S_W_PH_MM
  { 1242,	4,	1,	4,	725,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1242 = DPAU_H_QBL
  { 1243,	4,	1,	4,	872,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1243 = DPAU_H_QBL_MM
  { 1244,	4,	1,	4,	726,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1244 = DPAU_H_QBR
  { 1245,	4,	1,	4,	873,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1245 = DPAU_H_QBR_MM
  { 1246,	4,	1,	4,	821,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1246 = DPAX_W_PH
  { 1247,	4,	1,	4,	984,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1247 = DPAX_W_PH_MMR2
  { 1248,	4,	1,	4,	818,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1248 = DPA_W_PH
  { 1249,	4,	1,	4,	981,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1249 = DPA_W_PH_MMR2
  { 1250,	2,	1,	4,	109,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1250 = DPOP
  { 1251,	4,	1,	4,	824,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1251 = DPSQX_SA_W_PH
  { 1252,	4,	1,	4,	987,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1252 = DPSQX_SA_W_PH_MMR2
  { 1253,	4,	1,	4,	823,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1253 = DPSQX_S_W_PH
  { 1254,	4,	1,	4,	986,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1254 = DPSQX_S_W_PH_MMR2
  { 1255,	4,	1,	4,	727,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1255 = DPSQ_SA_L_W
  { 1256,	4,	1,	4,	874,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1256 = DPSQ_SA_L_W_MM
  { 1257,	4,	1,	4,	728,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1257 = DPSQ_S_W_PH
  { 1258,	4,	1,	4,	875,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1258 = DPSQ_S_W_PH_MM
  { 1259,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo219, -1 ,nullptr },  // Inst #1259 = DPSUB_S_D
  { 1260,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo220, -1 ,nullptr },  // Inst #1260 = DPSUB_S_H
  { 1261,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo221, -1 ,nullptr },  // Inst #1261 = DPSUB_S_W
  { 1262,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo219, -1 ,nullptr },  // Inst #1262 = DPSUB_U_D
  { 1263,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo220, -1 ,nullptr },  // Inst #1263 = DPSUB_U_H
  { 1264,	4,	1,	4,	638,	0, 0x6ULL, nullptr, nullptr, OperandInfo221, -1 ,nullptr },  // Inst #1264 = DPSUB_U_W
  { 1265,	4,	1,	4,	729,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1265 = DPSU_H_QBL
  { 1266,	4,	1,	4,	876,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1266 = DPSU_H_QBL_MM
  { 1267,	4,	1,	4,	730,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1267 = DPSU_H_QBR
  { 1268,	4,	1,	4,	877,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1268 = DPSU_H_QBR_MM
  { 1269,	4,	1,	4,	825,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1269 = DPSX_W_PH
  { 1270,	4,	1,	4,	988,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1270 = DPSX_W_PH_MMR2
  { 1271,	4,	1,	4,	822,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1271 = DPS_W_PH
  { 1272,	4,	1,	4,	985,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1272 = DPS_W_PH_MMR2
  { 1273,	3,	1,	4,	110,	0, 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1273 = DROTR
  { 1274,	3,	1,	4,	111,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1274 = DROTR32
  { 1275,	3,	1,	4,	112,	0, 0x1ULL, nullptr, nullptr, OperandInfo223, -1 ,nullptr },  // Inst #1275 = DROTRV
  { 1276,	2,	1,	4,	113,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1276 = DSBH
  { 1277,	2,	0,	4,	10,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList12, OperandInfo103, -1 ,nullptr },  // Inst #1277 = DSDIV
  { 1278,	2,	1,	4,	114,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1278 = DSHD
  { 1279,	3,	1,	4,	115,	0, 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1279 = DSLL
  { 1280,	3,	1,	4,	116,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1280 = DSLL32
  { 1281,	2,	1,	4,	115,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo224, -1 ,nullptr },  // Inst #1281 = DSLL64_32
  { 1282,	3,	1,	4,	117,	0, 0x1ULL, nullptr, nullptr, OperandInfo223, -1 ,nullptr },  // Inst #1282 = DSLLV
  { 1283,	3,	1,	4,	118,	0, 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1283 = DSRA
  { 1284,	3,	1,	4,	119,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1284 = DSRA32
  { 1285,	3,	1,	4,	120,	0, 0x1ULL, nullptr, nullptr, OperandInfo223, -1 ,nullptr },  // Inst #1285 = DSRAV
  { 1286,	3,	1,	4,	121,	0, 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1286 = DSRL
  { 1287,	3,	1,	4,	122,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #1287 = DSRL32
  { 1288,	3,	1,	4,	123,	0, 0x1ULL, nullptr, nullptr, OperandInfo223, -1 ,nullptr },  // Inst #1288 = DSRLV
  { 1289,	3,	1,	4,	124,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1289 = DSUB
  { 1290,	3,	1,	4,	125,	0|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #1290 = DSUBu
  { 1291,	2,	0,	4,	11,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList12, OperandInfo103, -1 ,nullptr },  // Inst #1291 = DUDIV
  { 1292,	1,	1,	4,	126,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1292 = DVP
  { 1293,	1,	1,	4,	127,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1293 = DVPE
  { 1294,	1,	1,	4,	126,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1294 = DVP_MMR6
  { 1295,	2,	0,	2,	38,	0, 0x0ULL, nullptr, ImplicitList6, OperandInfo111, -1 ,nullptr },  // Inst #1295 = DivRxRy16
  { 1296,	2,	0,	2,	38,	0, 0x0ULL, nullptr, ImplicitList6, OperandInfo111, -1 ,nullptr },  // Inst #1296 = DivuRxRy16
  { 1297,	0,	0,	4,	460,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1297 = EHB
  { 1298,	0,	0,	4,	128,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1298 = EHB_MM
  { 1299,	0,	0,	4,	128,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1299 = EHB_MMR6
  { 1300,	1,	1,	4,	458,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1300 = EI
  { 1301,	1,	1,	4,	129,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1301 = EI_MM
  { 1302,	1,	1,	4,	129,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1302 = EI_MMR6
  { 1303,	1,	1,	4,	130,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1303 = EMT
  { 1304,	0,	0,	4,	373,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1304 = ERET
  { 1305,	0,	0,	4,	374,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1305 = ERETNC
  { 1306,	0,	0,	4,	132,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1306 = ERETNC_MMR6
  { 1307,	0,	0,	4,	131,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1307 = ERET_MM
  { 1308,	0,	0,	4,	131,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1308 = ERET_MMR6
  { 1309,	1,	1,	4,	133,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1309 = EVP
  { 1310,	1,	1,	4,	134,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1310 = EVPE
  { 1311,	1,	1,	4,	133,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1311 = EVP_MMR6
  { 1312,	4,	1,	4,	475,	0, 0x1ULL, nullptr, nullptr, OperandInfo193, -1 ,nullptr },  // Inst #1312 = EXT
  { 1313,	3,	1,	4,	734,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList14, OperandInfo225, -1 ,nullptr },  // Inst #1313 = EXTP
  { 1314,	3,	1,	4,	732,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList15, OperandInfo225, -1 ,nullptr },  // Inst #1314 = EXTPDP
  { 1315,	3,	1,	4,	731,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList15, OperandInfo226, -1 ,nullptr },  // Inst #1315 = EXTPDPV
  { 1316,	3,	1,	4,	878,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList15, OperandInfo226, -1 ,nullptr },  // Inst #1316 = EXTPDPV_MM
  { 1317,	3,	1,	4,	879,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList15, OperandInfo225, -1 ,nullptr },  // Inst #1317 = EXTPDP_MM
  { 1318,	3,	1,	4,	733,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList14, OperandInfo226, -1 ,nullptr },  // Inst #1318 = EXTPV
  { 1319,	3,	1,	4,	880,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList14, OperandInfo226, -1 ,nullptr },  // Inst #1319 = EXTPV_MM
  { 1320,	3,	1,	4,	881,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, ImplicitList4, ImplicitList14, OperandInfo225, -1 ,nullptr },  // Inst #1320 = EXTP_MM
  { 1321,	3,	1,	4,	691,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1321 = EXTRV_RS_W
  { 1322,	3,	1,	4,	882,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1322 = EXTRV_RS_W_MM
  { 1323,	3,	1,	4,	692,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1323 = EXTRV_R_W
  { 1324,	3,	1,	4,	883,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1324 = EXTRV_R_W_MM
  { 1325,	3,	1,	4,	693,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1325 = EXTRV_S_H
  { 1326,	3,	1,	4,	884,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1326 = EXTRV_S_H_MM
  { 1327,	3,	1,	4,	694,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1327 = EXTRV_W
  { 1328,	3,	1,	4,	885,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo226, -1 ,nullptr },  // Inst #1328 = EXTRV_W_MM
  { 1329,	3,	1,	4,	695,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1329 = EXTR_RS_W
  { 1330,	3,	1,	4,	886,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1330 = EXTR_RS_W_MM
  { 1331,	3,	1,	4,	696,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1331 = EXTR_R_W
  { 1332,	3,	1,	4,	887,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1332 = EXTR_R_W_MM
  { 1333,	3,	1,	4,	697,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1333 = EXTR_S_H
  { 1334,	3,	1,	4,	888,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1334 = EXTR_S_H_MM
  { 1335,	3,	1,	4,	698,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1335 = EXTR_W
  { 1336,	3,	1,	4,	889,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList16, OperandInfo225, -1 ,nullptr },  // Inst #1336 = EXTR_W_MM
  { 1337,	4,	1,	4,	92,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #1337 = EXTS
  { 1338,	4,	1,	4,	92,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo191, -1 ,nullptr },  // Inst #1338 = EXTS32
  { 1339,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo193, -1 ,nullptr },  // Inst #1339 = EXT_MM
  { 1340,	4,	1,	4,	92,	0, 0x1ULL, nullptr, nullptr, OperandInfo193, -1 ,nullptr },  // Inst #1340 = EXT_MMR6
  { 1341,	2,	1,	4,	510,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1341 = FABS_D32
  { 1342,	2,	1,	4,	136,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1342 = FABS_D32_MM
  { 1343,	2,	1,	4,	510,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1343 = FABS_D64
  { 1344,	2,	1,	4,	136,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1344 = FABS_D64_MM
  { 1345,	2,	1,	4,	510,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1345 = FABS_S
  { 1346,	2,	1,	4,	135,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1346 = FABS_S_MM
  { 1347,	3,	1,	4,	635,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1347 = FADD_D
  { 1348,	3,	1,	4,	605,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1348 = FADD_D32
  { 1349,	3,	1,	4,	137,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1349 = FADD_D32_MM
  { 1350,	3,	1,	4,	605,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1350 = FADD_D64
  { 1351,	3,	1,	4,	137,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1351 = FADD_D64_MM
  { 1352,	3,	1,	4,	606,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1352 = FADD_S
  { 1353,	3,	1,	4,	138,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1353 = FADD_S_MM
  { 1354,	3,	1,	4,	138,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1354 = FADD_S_MMR6
  { 1355,	3,	1,	4,	635,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1355 = FADD_W
  { 1356,	3,	1,	4,	554,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1356 = FCAF_D
  { 1357,	3,	1,	4,	554,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1357 = FCAF_W
  { 1358,	3,	1,	4,	555,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1358 = FCEQ_D
  { 1359,	3,	1,	4,	555,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1359 = FCEQ_W
  { 1360,	2,	1,	4,	576,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1360 = FCLASS_D
  { 1361,	2,	1,	4,	576,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1361 = FCLASS_W
  { 1362,	3,	1,	4,	556,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1362 = FCLE_D
  { 1363,	3,	1,	4,	556,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1363 = FCLE_W
  { 1364,	3,	1,	4,	557,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1364 = FCLT_D
  { 1365,	3,	1,	4,	557,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1365 = FCLT_W
  { 1366,	3,	0,	4,	615,	0, 0x84ULL, nullptr, ImplicitList17, OperandInfo231, -1 ,nullptr },  // Inst #1366 = FCMP_D32
  { 1367,	3,	0,	4,	78,	0, 0x84ULL, nullptr, ImplicitList17, OperandInfo231, -1 ,nullptr },  // Inst #1367 = FCMP_D32_MM
  { 1368,	3,	0,	4,	615,	0, 0x84ULL, nullptr, ImplicitList17, OperandInfo232, -1 ,nullptr },  // Inst #1368 = FCMP_D64
  { 1369,	3,	0,	4,	616,	0, 0x84ULL, nullptr, ImplicitList17, OperandInfo233, -1 ,nullptr },  // Inst #1369 = FCMP_S32
  { 1370,	3,	0,	4,	79,	0, 0x84ULL, nullptr, ImplicitList17, OperandInfo233, -1 ,nullptr },  // Inst #1370 = FCMP_S32_MM
  { 1371,	3,	1,	4,	558,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1371 = FCNE_D
  { 1372,	3,	1,	4,	558,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1372 = FCNE_W
  { 1373,	3,	1,	4,	559,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1373 = FCOR_D
  { 1374,	3,	1,	4,	559,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1374 = FCOR_W
  { 1375,	3,	1,	4,	560,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1375 = FCUEQ_D
  { 1376,	3,	1,	4,	560,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1376 = FCUEQ_W
  { 1377,	3,	1,	4,	561,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1377 = FCULE_D
  { 1378,	3,	1,	4,	561,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1378 = FCULE_W
  { 1379,	3,	1,	4,	562,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1379 = FCULT_D
  { 1380,	3,	1,	4,	562,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1380 = FCULT_W
  { 1381,	3,	1,	4,	563,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1381 = FCUNE_D
  { 1382,	3,	1,	4,	563,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1382 = FCUNE_W
  { 1383,	3,	1,	4,	564,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1383 = FCUN_D
  { 1384,	3,	1,	4,	564,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1384 = FCUN_W
  { 1385,	3,	1,	4,	631,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1385 = FDIV_D
  { 1386,	3,	1,	4,	619,	0, 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1386 = FDIV_D32
  { 1387,	3,	1,	4,	139,	0, 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1387 = FDIV_D32_MM
  { 1388,	3,	1,	4,	619,	0, 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1388 = FDIV_D64
  { 1389,	3,	1,	4,	139,	0, 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1389 = FDIV_D64_MM
  { 1390,	3,	1,	4,	618,	0, 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1390 = FDIV_S
  { 1391,	3,	1,	4,	140,	0, 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1391 = FDIV_S_MM
  { 1392,	3,	1,	4,	140,	0, 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1392 = FDIV_S_MMR6
  { 1393,	3,	1,	4,	630,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1393 = FDIV_W
  { 1394,	3,	1,	4,	573,	0, 0x6ULL, nullptr, nullptr, OperandInfo234, -1 ,nullptr },  // Inst #1394 = FEXDO_H
  { 1395,	3,	1,	4,	573,	0, 0x6ULL, nullptr, nullptr, OperandInfo235, -1 ,nullptr },  // Inst #1395 = FEXDO_W
  { 1396,	3,	1,	4,	530,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1396 = FEXP2_D
  { 1397,	3,	1,	4,	530,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1397 = FEXP2_W
  { 1398,	2,	1,	4,	574,	0, 0x6ULL, nullptr, nullptr, OperandInfo236, -1 ,nullptr },  // Inst #1398 = FEXUPL_D
  { 1399,	2,	1,	4,	574,	0, 0x6ULL, nullptr, nullptr, OperandInfo237, -1 ,nullptr },  // Inst #1399 = FEXUPL_W
  { 1400,	2,	1,	4,	575,	0, 0x6ULL, nullptr, nullptr, OperandInfo236, -1 ,nullptr },  // Inst #1400 = FEXUPR_D
  { 1401,	2,	1,	4,	575,	0, 0x6ULL, nullptr, nullptr, OperandInfo237, -1 ,nullptr },  // Inst #1401 = FEXUPR_W
  { 1402,	2,	1,	4,	566,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1402 = FFINT_S_D
  { 1403,	2,	1,	4,	566,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1403 = FFINT_S_W
  { 1404,	2,	1,	4,	566,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1404 = FFINT_U_D
  { 1405,	2,	1,	4,	566,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1405 = FFINT_U_W
  { 1406,	2,	1,	4,	567,	0, 0x6ULL, nullptr, nullptr, OperandInfo236, -1 ,nullptr },  // Inst #1406 = FFQL_D
  { 1407,	2,	1,	4,	567,	0, 0x6ULL, nullptr, nullptr, OperandInfo237, -1 ,nullptr },  // Inst #1407 = FFQL_W
  { 1408,	2,	1,	4,	568,	0, 0x6ULL, nullptr, nullptr, OperandInfo236, -1 ,nullptr },  // Inst #1408 = FFQR_D
  { 1409,	2,	1,	4,	568,	0, 0x6ULL, nullptr, nullptr, OperandInfo237, -1 ,nullptr },  // Inst #1409 = FFQR_W
  { 1410,	2,	1,	4,	524,	0, 0x6ULL, nullptr, nullptr, OperandInfo238, -1 ,nullptr },  // Inst #1410 = FILL_B
  { 1411,	2,	1,	4,	524,	0, 0x6ULL, nullptr, nullptr, OperandInfo239, -1 ,nullptr },  // Inst #1411 = FILL_D
  { 1412,	2,	1,	4,	524,	0, 0x6ULL, nullptr, nullptr, OperandInfo240, -1 ,nullptr },  // Inst #1412 = FILL_H
  { 1413,	2,	1,	4,	524,	0, 0x6ULL, nullptr, nullptr, OperandInfo241, -1 ,nullptr },  // Inst #1413 = FILL_W
  { 1414,	2,	1,	4,	581,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1414 = FLOG2_D
  { 1415,	2,	1,	4,	581,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1415 = FLOG2_W
  { 1416,	2,	1,	4,	688,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1416 = FLOOR_L_D64
  { 1417,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1417 = FLOOR_L_D_MMR6
  { 1418,	2,	1,	4,	688,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1418 = FLOOR_L_S
  { 1419,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #1419 = FLOOR_L_S_MMR6
  { 1420,	2,	1,	4,	688,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1420 = FLOOR_W_D32
  { 1421,	2,	1,	4,	688,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #1421 = FLOOR_W_D64
  { 1422,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1422 = FLOOR_W_D_MMR6
  { 1423,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #1423 = FLOOR_W_MM
  { 1424,	2,	1,	4,	688,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1424 = FLOOR_W_S
  { 1425,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1425 = FLOOR_W_S_MM
  { 1426,	2,	1,	4,	141,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1426 = FLOOR_W_S_MMR6
  { 1427,	4,	1,	4,	628,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #1427 = FMADD_D
  { 1428,	4,	1,	4,	628,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1428 = FMADD_W
  { 1429,	3,	1,	4,	577,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1429 = FMAX_A_D
  { 1430,	3,	1,	4,	577,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1430 = FMAX_A_W
  { 1431,	3,	1,	4,	578,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1431 = FMAX_D
  { 1432,	3,	1,	4,	578,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1432 = FMAX_W
  { 1433,	3,	1,	4,	579,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1433 = FMIN_A_D
  { 1434,	3,	1,	4,	579,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1434 = FMIN_A_W
  { 1435,	3,	1,	4,	580,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1435 = FMIN_D
  { 1436,	3,	1,	4,	580,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1436 = FMIN_W
  { 1437,	2,	1,	4,	515,	0|(1ULL<<MCID::MoveReg), 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1437 = FMOV_D32
  { 1438,	2,	1,	4,	142,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1438 = FMOV_D32_MM
  { 1439,	2,	1,	4,	515,	0|(1ULL<<MCID::MoveReg), 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1439 = FMOV_D64
  { 1440,	2,	1,	4,	142,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1440 = FMOV_D64_MM
  { 1441,	2,	1,	4,	516,	0|(1ULL<<MCID::MoveReg), 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1441 = FMOV_S
  { 1442,	2,	1,	4,	143,	0|(1ULL<<MCID::MoveReg), 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1442 = FMOV_S_MM
  { 1443,	2,	1,	4,	143,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1443 = FMOV_S_MMR6
  { 1444,	4,	1,	4,	629,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #1444 = FMSUB_D
  { 1445,	4,	1,	4,	629,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1445 = FMSUB_W
  { 1446,	3,	1,	4,	634,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1446 = FMUL_D
  { 1447,	3,	1,	4,	607,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1447 = FMUL_D32
  { 1448,	3,	1,	4,	144,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1448 = FMUL_D32_MM
  { 1449,	3,	1,	4,	607,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1449 = FMUL_D64
  { 1450,	3,	1,	4,	144,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1450 = FMUL_D64_MM
  { 1451,	3,	1,	4,	608,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1451 = FMUL_S
  { 1452,	3,	1,	4,	145,	0|(1ULL<<MCID::Commutable), 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1452 = FMUL_S_MM
  { 1453,	3,	1,	4,	145,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1453 = FMUL_S_MMR6
  { 1454,	3,	1,	4,	634,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1454 = FMUL_W
  { 1455,	2,	1,	4,	517,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1455 = FNEG_D32
  { 1456,	2,	1,	4,	146,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1456 = FNEG_D32_MM
  { 1457,	2,	1,	4,	517,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1457 = FNEG_D64
  { 1458,	2,	1,	4,	146,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1458 = FNEG_D64_MM
  { 1459,	2,	1,	4,	517,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1459 = FNEG_S
  { 1460,	2,	1,	4,	146,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1460 = FNEG_S_MM
  { 1461,	2,	1,	4,	146,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1461 = FNEG_S_MMR6
  { 1462,	3,	2,	4,	147,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1462 = FORK
  { 1463,	2,	1,	4,	622,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1463 = FRCP_D
  { 1464,	2,	1,	4,	622,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1464 = FRCP_W
  { 1465,	2,	1,	4,	570,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1465 = FRINT_D
  { 1466,	2,	1,	4,	570,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1466 = FRINT_W
  { 1467,	2,	1,	4,	623,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1467 = FRSQRT_D
  { 1468,	2,	1,	4,	623,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1468 = FRSQRT_W
  { 1469,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1469 = FSAF_D
  { 1470,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1470 = FSAF_W
  { 1471,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1471 = FSEQ_D
  { 1472,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1472 = FSEQ_W
  { 1473,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1473 = FSLE_D
  { 1474,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1474 = FSLE_W
  { 1475,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1475 = FSLT_D
  { 1476,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1476 = FSLT_W
  { 1477,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1477 = FSNE_D
  { 1478,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1478 = FSNE_W
  { 1479,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1479 = FSOR_D
  { 1480,	3,	1,	4,	548,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1480 = FSOR_W
  { 1481,	2,	1,	4,	633,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1481 = FSQRT_D
  { 1482,	2,	1,	4,	621,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1482 = FSQRT_D32
  { 1483,	2,	1,	4,	136,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #1483 = FSQRT_D32_MM
  { 1484,	2,	1,	4,	621,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1484 = FSQRT_D64
  { 1485,	2,	1,	4,	136,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #1485 = FSQRT_D64_MM
  { 1486,	2,	1,	4,	620,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1486 = FSQRT_S
  { 1487,	2,	1,	4,	148,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #1487 = FSQRT_S_MM
  { 1488,	2,	1,	4,	632,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1488 = FSQRT_W
  { 1489,	3,	1,	4,	636,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1489 = FSUB_D
  { 1490,	3,	1,	4,	609,	0, 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1490 = FSUB_D32
  { 1491,	3,	1,	4,	149,	0, 0x4ULL, nullptr, nullptr, OperandInfo228, -1 ,nullptr },  // Inst #1491 = FSUB_D32_MM
  { 1492,	3,	1,	4,	609,	0, 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1492 = FSUB_D64
  { 1493,	3,	1,	4,	149,	0, 0x4ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1493 = FSUB_D64_MM
  { 1494,	3,	1,	4,	610,	0, 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1494 = FSUB_S
  { 1495,	3,	1,	4,	150,	0, 0x4ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1495 = FSUB_S_MM
  { 1496,	3,	1,	4,	150,	0, 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1496 = FSUB_S_MMR6
  { 1497,	3,	1,	4,	636,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1497 = FSUB_W
  { 1498,	3,	1,	4,	549,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1498 = FSUEQ_D
  { 1499,	3,	1,	4,	549,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1499 = FSUEQ_W
  { 1500,	3,	1,	4,	550,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1500 = FSULE_D
  { 1501,	3,	1,	4,	550,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1501 = FSULE_W
  { 1502,	3,	1,	4,	551,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1502 = FSULT_D
  { 1503,	3,	1,	4,	551,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1503 = FSULT_W
  { 1504,	3,	1,	4,	552,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1504 = FSUNE_D
  { 1505,	3,	1,	4,	552,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1505 = FSUNE_W
  { 1506,	3,	1,	4,	553,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1506 = FSUN_D
  { 1507,	3,	1,	4,	553,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1507 = FSUN_W
  { 1508,	2,	1,	4,	569,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1508 = FTINT_S_D
  { 1509,	2,	1,	4,	569,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1509 = FTINT_S_W
  { 1510,	2,	1,	4,	569,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1510 = FTINT_U_D
  { 1511,	2,	1,	4,	569,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1511 = FTINT_U_W
  { 1512,	3,	1,	4,	571,	0, 0x6ULL, nullptr, nullptr, OperandInfo234, -1 ,nullptr },  // Inst #1512 = FTQ_H
  { 1513,	3,	1,	4,	571,	0, 0x6ULL, nullptr, nullptr, OperandInfo235, -1 ,nullptr },  // Inst #1513 = FTQ_W
  { 1514,	2,	1,	4,	572,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1514 = FTRUNC_S_D
  { 1515,	2,	1,	4,	572,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1515 = FTRUNC_S_W
  { 1516,	2,	1,	4,	572,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #1516 = FTRUNC_U_D
  { 1517,	2,	1,	4,	572,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #1517 = FTRUNC_U_W
  { 1518,	1,	0,	4,	151,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1518 = GINVI
  { 1519,	1,	0,	4,	151,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1519 = GINVI_MMR6
  { 1520,	2,	0,	4,	152,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1520 = GINVT
  { 1521,	2,	0,	4,	152,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1521 = GINVT_MMR6
  { 1522,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1522 = HADD_S_D
  { 1523,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1523 = HADD_S_H
  { 1524,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1524 = HADD_S_W
  { 1525,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1525 = HADD_U_D
  { 1526,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1526 = HADD_U_H
  { 1527,	3,	1,	4,	592,	0, 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1527 = HADD_U_W
  { 1528,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1528 = HSUB_S_D
  { 1529,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1529 = HSUB_S_H
  { 1530,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1530 = HSUB_S_W
  { 1531,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo216, -1 ,nullptr },  // Inst #1531 = HSUB_U_D
  { 1532,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo217, -1 ,nullptr },  // Inst #1532 = HSUB_U_H
  { 1533,	3,	1,	4,	593,	0, 0x6ULL, nullptr, nullptr, OperandInfo218, -1 ,nullptr },  // Inst #1533 = HSUB_U_W
  { 1534,	1,	0,	4,	153,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #1534 = HYPCALL
  { 1535,	1,	0,	4,	153,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #1535 = HYPCALL_MM
  { 1536,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1536 = ILVEV_B
  { 1537,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1537 = ILVEV_D
  { 1538,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1538 = ILVEV_H
  { 1539,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1539 = ILVEV_W
  { 1540,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1540 = ILVL_B
  { 1541,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1541 = ILVL_D
  { 1542,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1542 = ILVL_H
  { 1543,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1543 = ILVL_W
  { 1544,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1544 = ILVOD_B
  { 1545,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1545 = ILVOD_D
  { 1546,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1546 = ILVOD_H
  { 1547,	3,	1,	4,	583,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1547 = ILVOD_W
  { 1548,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1548 = ILVR_B
  { 1549,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1549 = ILVR_D
  { 1550,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1550 = ILVR_H
  { 1551,	3,	1,	4,	582,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1551 = ILVR_W
  { 1552,	5,	1,	4,	476,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo242, -1 ,nullptr },  // Inst #1552 = INS
  { 1553,	4,	1,	4,	499,	0, 0x6ULL, nullptr, nullptr, OperandInfo243, -1 ,nullptr },  // Inst #1553 = INSERT_B
  { 1554,	4,	1,	4,	499,	0, 0x6ULL, nullptr, nullptr, OperandInfo244, -1 ,nullptr },  // Inst #1554 = INSERT_D
  { 1555,	4,	1,	4,	499,	0, 0x6ULL, nullptr, nullptr, OperandInfo245, -1 ,nullptr },  // Inst #1555 = INSERT_H
  { 1556,	4,	1,	4,	499,	0, 0x6ULL, nullptr, nullptr, OperandInfo246, -1 ,nullptr },  // Inst #1556 = INSERT_W
  { 1557,	3,	1,	4,	699,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList18, nullptr, OperandInfo247, -1 ,nullptr },  // Inst #1557 = INSV
  { 1558,	5,	1,	4,	584,	0, 0x6ULL, nullptr, nullptr, OperandInfo248, -1 ,nullptr },  // Inst #1558 = INSVE_B
  { 1559,	5,	1,	4,	584,	0, 0x6ULL, nullptr, nullptr, OperandInfo249, -1 ,nullptr },  // Inst #1559 = INSVE_D
  { 1560,	5,	1,	4,	584,	0, 0x6ULL, nullptr, nullptr, OperandInfo250, -1 ,nullptr },  // Inst #1560 = INSVE_H
  { 1561,	5,	1,	4,	584,	0, 0x6ULL, nullptr, nullptr, OperandInfo251, -1 ,nullptr },  // Inst #1561 = INSVE_W
  { 1562,	3,	1,	4,	890,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList18, nullptr, OperandInfo247, -1 ,nullptr },  // Inst #1562 = INSV_MM
  { 1563,	5,	1,	4,	61,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo242, -1 ,nullptr },  // Inst #1563 = INS_MM
  { 1564,	5,	1,	4,	61,	0, 0x1ULL, nullptr, nullptr, OperandInfo242, -1 ,nullptr },  // Inst #1564 = INS_MMR6
  { 1565,	1,	0,	4,	375,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x13ULL, nullptr, ImplicitList2, OperandInfo2, -1 ,nullptr },  // Inst #1565 = J
  { 1566,	1,	0,	4,	398,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x13ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1566 = JAL
  { 1567,	2,	1,	4,	399,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, ImplicitList3, OperandInfo31, -1 ,nullptr },  // Inst #1567 = JALR
  { 1568,	1,	0,	2,	6,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x0ULL, nullptr, ImplicitList3, OperandInfo44, -1 ,nullptr },  // Inst #1568 = JALR16_MM
  { 1569,	2,	1,	4,	6,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, ImplicitList3, OperandInfo103, -1 ,nullptr },  // Inst #1569 = JALR64
  { 1570,	1,	0,	2,	6,	0|(1ULL<<MCID::Call), 0x0ULL, nullptr, ImplicitList3, OperandInfo44, -1 ,nullptr },  // Inst #1570 = JALRC16_MMR6
  { 1571,	2,	1,	4,	155,	0|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::UnmodeledSideEffects), 0x3ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #1571 = JALRC_HB_MMR6
  { 1572,	2,	1,	4,	156,	0|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList3, OperandInfo31, -1 ,nullptr },  // Inst #1572 = JALRC_MMR6
  { 1573,	1,	0,	2,	157,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList3, OperandInfo44, -1 ,nullptr },  // Inst #1573 = JALRS16_MM
  { 1574,	2,	1,	4,	157,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList3, OperandInfo31, -1 ,nullptr },  // Inst #1574 = JALRS_MM
  { 1575,	2,	1,	4,	400,	0|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x13ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #1575 = JALR_HB
  { 1576,	2,	1,	4,	155,	0|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x13ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #1576 = JALR_HB64
  { 1577,	2,	1,	4,	6,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, ImplicitList3, OperandInfo31, -1 ,nullptr },  // Inst #1577 = JALR_MM
  { 1578,	1,	0,	4,	158,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x3ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1578 = JALS_MM
  { 1579,	1,	0,	4,	401,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x13ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1579 = JALX
  { 1580,	1,	0,	4,	154,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x13ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1580 = JALX_MM
  { 1581,	1,	0,	4,	154,	0|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Call), 0x13ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1581 = JAL_MM
  { 1582,	2,	0,	4,	159,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo95, -1 ,nullptr },  // Inst #1582 = JIALC
  { 1583,	2,	0,	4,	159,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Call)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList3, OperandInfo93, -1 ,nullptr },  // Inst #1583 = JIALC64
  { 1584,	2,	0,	4,	159,	0|(1ULL<<MCID::Call)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList3, OperandInfo95, -1 ,nullptr },  // Inst #1584 = JIALC_MMR6
  { 1585,	2,	0,	4,	159,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo95, -1 ,nullptr },  // Inst #1585 = JIC
  { 1586,	2,	0,	4,	160,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, ImplicitList2, OperandInfo93, -1 ,nullptr },  // Inst #1586 = JIC64
  { 1587,	2,	0,	4,	160,	0|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList2, OperandInfo95, -1 ,nullptr },  // Inst #1587 = JIC_MMR6
  { 1588,	1,	0,	4,	376,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1588 = JR
  { 1589,	1,	0,	2,	25,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1589 = JR16_MM
  { 1590,	1,	0,	4,	25,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #1590 = JR64
  { 1591,	1,	0,	2,	161,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #1591 = JRADDIUSP
  { 1592,	1,	0,	2,	162,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1592 = JRC16_MM
  { 1593,	1,	0,	2,	25,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1593 = JRC16_MMR6
  { 1594,	1,	0,	2,	161,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #1594 = JRCADDIUSP_MMR6
  { 1595,	1,	0,	4,	377,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x13ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1595 = JR_HB
  { 1596,	1,	0,	4,	163,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x13ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #1596 = JR_HB64
  { 1597,	1,	0,	4,	163,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #1597 = JR_HB64_R6
  { 1598,	1,	0,	4,	163,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1598 = JR_HB_R6
  { 1599,	1,	0,	4,	25,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x11ULL, nullptr, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1599 = JR_MM
  { 1600,	1,	0,	4,	24,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator), 0x13ULL, nullptr, ImplicitList2, OperandInfo2, -1 ,nullptr },  // Inst #1600 = J_MM
  { 1601,	1,	0,	6,	38,	0|(1ULL<<MCID::Call), 0x0ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1601 = Jal16
  { 1602,	1,	0,	6,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList3, OperandInfo2, -1 ,nullptr },  // Inst #1602 = JalB16
  { 1603,	0,	0,	2,	38,	0|(1ULL<<MCID::Return)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::DelaySlot)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1603 = JrRa16
  { 1604,	0,	0,	2,	38,	0|(1ULL<<MCID::Return)|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #1604 = JrcRa16
  { 1605,	1,	0,	2,	38,	0|(1ULL<<MCID::Branch)|(1ULL<<MCID::IndirectBranch)|(1ULL<<MCID::Barrier)|(1ULL<<MCID::Terminator), 0x0ULL, nullptr, nullptr, OperandInfo252, -1 ,nullptr },  // Inst #1605 = JrcRx16
  { 1606,	1,	0,	2,	156,	0|(1ULL<<MCID::Call), 0x0ULL, nullptr, ImplicitList3, OperandInfo252, -1 ,nullptr },  // Inst #1606 = JumpLinkReg16
  { 1607,	3,	1,	4,	412,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1607 = LB
  { 1608,	3,	1,	4,	164,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1608 = LB64
  { 1609,	3,	1,	4,	422,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1609 = LBE
  { 1610,	3,	1,	4,	165,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1610 = LBE_MM
  { 1611,	3,	1,	2,	166,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo253, -1 ,nullptr },  // Inst #1611 = LBU16_MM
  { 1612,	3,	1,	4,	735,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1612 = LBUX
  { 1613,	3,	1,	4,	891,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1613 = LBUX_MM
  { 1614,	3,	1,	4,	166,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1614 = LBU_MMR6
  { 1615,	3,	1,	4,	164,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1615 = LB_MM
  { 1616,	3,	1,	4,	164,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1616 = LB_MMR6
  { 1617,	3,	1,	4,	413,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1617 = LBu
  { 1618,	3,	1,	4,	166,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1618 = LBu64
  { 1619,	3,	1,	4,	423,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1619 = LBuE
  { 1620,	3,	1,	4,	167,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1620 = LBuE_MM
  { 1621,	3,	1,	4,	166,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1621 = LBu_MM
  { 1622,	3,	1,	4,	168,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1622 = LD
  { 1623,	3,	1,	4,	681,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo255, -1 ,nullptr },  // Inst #1623 = LDC1
  { 1624,	3,	1,	4,	169,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo256, -1 ,nullptr },  // Inst #1624 = LDC164
  { 1625,	3,	1,	4,	169,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo256, -1 ,nullptr },  // Inst #1625 = LDC1_D64_MMR6
  { 1626,	3,	1,	4,	169,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo255, -1 ,nullptr },  // Inst #1626 = LDC1_MM
  { 1627,	3,	1,	4,	420,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #1627 = LDC2
  { 1628,	3,	1,	4,	170,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo258, -1 ,nullptr },  // Inst #1628 = LDC2_MMR6
  { 1629,	3,	1,	4,	170,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #1629 = LDC2_R6
  { 1630,	3,	1,	4,	421,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo259, -1 ,nullptr },  // Inst #1630 = LDC3
  { 1631,	2,	1,	4,	527,	0, 0x6ULL, nullptr, nullptr, OperandInfo260, -1 ,nullptr },  // Inst #1631 = LDI_B
  { 1632,	2,	1,	4,	527,	0, 0x6ULL, nullptr, nullptr, OperandInfo261, -1 ,nullptr },  // Inst #1632 = LDI_D
  { 1633,	2,	1,	4,	527,	0, 0x6ULL, nullptr, nullptr, OperandInfo262, -1 ,nullptr },  // Inst #1633 = LDI_H
  { 1634,	2,	1,	4,	527,	0, 0x6ULL, nullptr, nullptr, OperandInfo263, -1 ,nullptr },  // Inst #1634 = LDI_W
  { 1635,	4,	1,	4,	172,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo264, -1 ,nullptr },  // Inst #1635 = LDL
  { 1636,	2,	1,	4,	173,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #1636 = LDPC
  { 1637,	4,	1,	4,	174,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo264, -1 ,nullptr },  // Inst #1637 = LDR
  { 1638,	3,	1,	4,	682,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo265, -1 ,nullptr },  // Inst #1638 = LDXC1
  { 1639,	3,	1,	4,	175,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #1639 = LDXC164
  { 1640,	3,	1,	4,	686,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo267, -1 ,nullptr },  // Inst #1640 = LD_B
  { 1641,	3,	1,	4,	686,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo268, -1 ,nullptr },  // Inst #1641 = LD_D
  { 1642,	3,	1,	4,	686,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo269, -1 ,nullptr },  // Inst #1642 = LD_H
  { 1643,	3,	1,	4,	686,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo270, -1 ,nullptr },  // Inst #1643 = LD_W
  { 1644,	3,	1,	4,	29,	0, 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1644 = LEA_ADDiu
  { 1645,	3,	1,	4,	29,	0, 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1645 = LEA_ADDiu64
  { 1646,	3,	1,	4,	29,	0, 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1646 = LEA_ADDiu_MM
  { 1647,	3,	1,	4,	414,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1647 = LH
  { 1648,	3,	1,	4,	176,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1648 = LH64
  { 1649,	3,	1,	4,	424,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1649 = LHE
  { 1650,	3,	1,	4,	177,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1650 = LHE_MM
  { 1651,	3,	1,	2,	178,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo253, -1 ,nullptr },  // Inst #1651 = LHU16_MM
  { 1652,	3,	1,	4,	736,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1652 = LHX
  { 1653,	3,	1,	4,	892,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1653 = LHX_MM
  { 1654,	3,	1,	4,	176,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1654 = LH_MM
  { 1655,	3,	1,	4,	415,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1655 = LHu
  { 1656,	3,	1,	4,	178,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1656 = LHu64
  { 1657,	3,	1,	4,	425,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1657 = LHuE
  { 1658,	3,	1,	4,	179,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1658 = LHuE_MM
  { 1659,	3,	1,	4,	178,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1659 = LHu_MM
  { 1660,	2,	1,	2,	180,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo147, -1 ,nullptr },  // Inst #1660 = LI16_MM
  { 1661,	2,	1,	2,	180,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects)|(1ULL<<MCID::CheapAsAMove), 0x0ULL, nullptr, nullptr, OperandInfo147, -1 ,nullptr },  // Inst #1661 = LI16_MMR6
  { 1662,	3,	1,	4,	417,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1662 = LL
  { 1663,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1663 = LL64
  { 1664,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1664 = LL64_R6
  { 1665,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1665 = LLD
  { 1666,	3,	1,	4,	182,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1666 = LLD_R6
  { 1667,	3,	1,	4,	427,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1667 = LLE
  { 1668,	3,	1,	4,	183,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1668 = LLE_MM
  { 1669,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1669 = LL_MM
  { 1670,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1670 = LL_MMR6
  { 1671,	3,	1,	4,	181,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1671 = LL_R6
  { 1672,	4,	1,	4,	494,	0, 0x6ULL, nullptr, nullptr, OperandInfo157, -1 ,nullptr },  // Inst #1672 = LSA
  { 1673,	4,	1,	4,	184,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo157, -1 ,nullptr },  // Inst #1673 = LSA_MMR6
  { 1674,	4,	1,	4,	184,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo157, -1 ,nullptr },  // Inst #1674 = LSA_R6
  { 1675,	2,	1,	4,	185,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1675 = LUI_MMR6
  { 1676,	3,	1,	4,	685,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo265, -1 ,nullptr },  // Inst #1676 = LUXC1
  { 1677,	3,	1,	4,	186,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #1677 = LUXC164
  { 1678,	3,	1,	4,	186,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #1678 = LUXC1_MM
  { 1679,	2,	1,	4,	359,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x2ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1679 = LUi
  { 1680,	2,	1,	4,	185,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x2ULL, nullptr, nullptr, OperandInfo93, -1 ,nullptr },  // Inst #1680 = LUi64
  { 1681,	2,	1,	4,	185,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::CheapAsAMove), 0x2ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1681 = LUi_MM
  { 1682,	3,	1,	4,	416,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1682 = LW
  { 1683,	3,	1,	2,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo253, -1 ,nullptr },  // Inst #1683 = LW16_MM
  { 1684,	3,	1,	4,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1684 = LW64
  { 1685,	3,	1,	4,	683,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo271, -1 ,nullptr },  // Inst #1685 = LWC1
  { 1686,	3,	1,	4,	188,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo271, -1 ,nullptr },  // Inst #1686 = LWC1_MM
  { 1687,	3,	1,	4,	418,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #1687 = LWC2
  { 1688,	3,	1,	4,	189,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo258, -1 ,nullptr },  // Inst #1688 = LWC2_MMR6
  { 1689,	3,	1,	4,	189,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #1689 = LWC2_R6
  { 1690,	3,	1,	4,	419,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo259, -1 ,nullptr },  // Inst #1690 = LWC3
  { 1691,	3,	1,	4,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo272, -1 ,nullptr },  // Inst #1691 = LWDSP
  { 1692,	3,	1,	4,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo272, -1 ,nullptr },  // Inst #1692 = LWDSP_MM
  { 1693,	3,	1,	4,	426,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1693 = LWE
  { 1694,	3,	1,	4,	191,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1694 = LWE_MM
  { 1695,	3,	1,	2,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo273, -1 ,nullptr },  // Inst #1695 = LWGP_MM
  { 1696,	4,	1,	4,	429,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1696 = LWL
  { 1697,	4,	1,	4,	192,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo264, -1 ,nullptr },  // Inst #1697 = LWL64
  { 1698,	4,	1,	4,	431,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1698 = LWLE
  { 1699,	4,	1,	4,	193,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1699 = LWLE_MM
  { 1700,	4,	1,	4,	192,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1700 = LWL_MM
  { 1701,	3,	1,	2,	194,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo275, -1 ,nullptr },  // Inst #1701 = LWM16_MM
  { 1702,	3,	1,	2,	194,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo275, -1 ,nullptr },  // Inst #1702 = LWM16_MMR6
  { 1703,	3,	1,	4,	194,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo91, -1 ,nullptr },  // Inst #1703 = LWM32_MM
  { 1704,	2,	1,	4,	428,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1704 = LWPC
  { 1705,	2,	1,	4,	195,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1705 = LWPC_MMR6
  { 1706,	4,	2,	4,	196,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo276, -1 ,nullptr },  // Inst #1706 = LWP_MM
  { 1707,	4,	1,	4,	430,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1707 = LWR
  { 1708,	4,	1,	4,	197,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo264, -1 ,nullptr },  // Inst #1708 = LWR64
  { 1709,	4,	1,	4,	432,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1709 = LWRE
  { 1710,	4,	1,	4,	198,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1710 = LWRE_MM
  { 1711,	4,	1,	4,	197,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo274, -1 ,nullptr },  // Inst #1711 = LWR_MM
  { 1712,	3,	1,	2,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo277, -1 ,nullptr },  // Inst #1712 = LWSP_MM
  { 1713,	2,	1,	4,	199,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #1713 = LWUPC
  { 1714,	3,	1,	4,	200,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1714 = LWU_MM
  { 1715,	3,	1,	4,	737,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1715 = LWX
  { 1716,	3,	1,	4,	684,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo278, -1 ,nullptr },  // Inst #1716 = LWXC1
  { 1717,	3,	1,	4,	201,	0|(1ULL<<MCID::MayLoad), 0x5ULL, nullptr, nullptr, OperandInfo278, -1 ,nullptr },  // Inst #1717 = LWXC1_MM
  { 1718,	3,	1,	4,	202,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1718 = LWXS_MM
  { 1719,	3,	1,	4,	893,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo254, -1 ,nullptr },  // Inst #1719 = LWX_MM
  { 1720,	3,	1,	4,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1720 = LW_MM
  { 1721,	3,	1,	4,	187,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #1721 = LW_MMR6
  { 1722,	3,	1,	4,	200,	0|(1ULL<<MCID::FoldableAsLoad)|(1ULL<<MCID::MayLoad), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #1722 = LWu
  { 1723,	3,	1,	4,	164,	0|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #1723 = LbRxRyOffMemX16
  { 1724,	3,	1,	4,	166,	0|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #1724 = LbuRxRyOffMemX16
  { 1725,	3,	1,	4,	176,	0|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #1725 = LhRxRyOffMemX16
  { 1726,	3,	1,	4,	178,	0|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #1726 = LhuRxRyOffMemX16
  { 1727,	2,	1,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo160, -1 ,nullptr },  // Inst #1727 = LiRxImm16
  { 1728,	2,	1,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo160, -1 ,nullptr },  // Inst #1728 = LiRxImmAlignX16
  { 1729,	2,	1,	4,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo160, -1 ,nullptr },  // Inst #1729 = LiRxImmX16
  { 1730,	3,	1,	2,	187,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo280, -1 ,nullptr },  // Inst #1730 = LwRxPcTcp16
  { 1731,	3,	1,	4,	187,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo280, -1 ,nullptr },  // Inst #1731 = LwRxPcTcpX16
  { 1732,	3,	1,	4,	187,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #1732 = LwRxRyOffMemX16
  { 1733,	3,	1,	4,	187,	0|(1ULL<<MCID::MayLoad), 0x0ULL, nullptr, nullptr, OperandInfo162, -1 ,nullptr },  // Inst #1733 = LwRxSpImmX16
  { 1734,	2,	0,	4,	470,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1734 = MADD
  { 1735,	4,	1,	4,	203,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #1735 = MADDF_D
  { 1736,	4,	1,	4,	203,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #1736 = MADDF_D_MMR6
  { 1737,	4,	1,	4,	204,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo282, -1 ,nullptr },  // Inst #1737 = MADDF_S
  { 1738,	4,	1,	4,	204,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo282, -1 ,nullptr },  // Inst #1738 = MADDF_S_MMR6
  { 1739,	4,	1,	4,	643,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1739 = MADDR_Q_H
  { 1740,	4,	1,	4,	643,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1740 = MADDR_Q_W
  { 1741,	2,	0,	4,	471,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1741 = MADDU
  { 1742,	4,	1,	4,	738,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1742 = MADDU_DSP
  { 1743,	4,	1,	4,	894,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1743 = MADDU_DSP_MM
  { 1744,	2,	0,	4,	14,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1744 = MADDU_MM
  { 1745,	4,	1,	4,	641,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #1745 = MADDV_B
  { 1746,	4,	1,	4,	641,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #1746 = MADDV_D
  { 1747,	4,	1,	4,	641,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1747 = MADDV_H
  { 1748,	4,	1,	4,	641,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1748 = MADDV_W
  { 1749,	4,	1,	4,	649,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #1749 = MADD_D32
  { 1750,	4,	1,	4,	205,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #1750 = MADD_D32_MM
  { 1751,	4,	1,	4,	649,	0, 0x4ULL, nullptr, nullptr, OperandInfo284, -1 ,nullptr },  // Inst #1751 = MADD_D64
  { 1752,	4,	1,	4,	739,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1752 = MADD_DSP
  { 1753,	4,	1,	4,	895,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1753 = MADD_DSP_MM
  { 1754,	2,	0,	4,	13,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1754 = MADD_MM
  { 1755,	4,	1,	4,	644,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1755 = MADD_Q_H
  { 1756,	4,	1,	4,	644,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1756 = MADD_Q_W
  { 1757,	4,	1,	4,	650,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #1757 = MADD_S
  { 1758,	4,	1,	4,	206,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #1758 = MADD_S_MM
  { 1759,	4,	1,	4,	740,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1759 = MAQ_SA_W_PHL
  { 1760,	4,	1,	4,	896,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1760 = MAQ_SA_W_PHL_MM
  { 1761,	4,	1,	4,	741,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1761 = MAQ_SA_W_PHR
  { 1762,	4,	1,	4,	897,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1762 = MAQ_SA_W_PHR_MM
  { 1763,	4,	1,	4,	742,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1763 = MAQ_S_W_PHL
  { 1764,	4,	1,	4,	898,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1764 = MAQ_S_W_PHL_MM
  { 1765,	4,	1,	4,	743,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1765 = MAQ_S_W_PHR
  { 1766,	4,	1,	4,	899,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1766 = MAQ_S_W_PHR_MM
  { 1767,	3,	1,	4,	207,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1767 = MAXA_D
  { 1768,	3,	1,	4,	208,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1768 = MAXA_D_MMR6
  { 1769,	3,	1,	4,	209,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1769 = MAXA_S
  { 1770,	3,	1,	4,	210,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1770 = MAXA_S_MMR6
  { 1771,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #1771 = MAXI_S_B
  { 1772,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #1772 = MAXI_S_D
  { 1773,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #1773 = MAXI_S_H
  { 1774,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #1774 = MAXI_S_W
  { 1775,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #1775 = MAXI_U_B
  { 1776,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #1776 = MAXI_U_D
  { 1777,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #1777 = MAXI_U_H
  { 1778,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #1778 = MAXI_U_W
  { 1779,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1779 = MAX_A_B
  { 1780,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1780 = MAX_A_D
  { 1781,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1781 = MAX_A_H
  { 1782,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1782 = MAX_A_W
  { 1783,	3,	1,	4,	207,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1783 = MAX_D
  { 1784,	3,	1,	4,	207,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1784 = MAX_D_MMR6
  { 1785,	3,	1,	4,	209,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1785 = MAX_S
  { 1786,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1786 = MAX_S_B
  { 1787,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1787 = MAX_S_D
  { 1788,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1788 = MAX_S_H
  { 1789,	3,	1,	4,	209,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1789 = MAX_S_MMR6
  { 1790,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1790 = MAX_S_W
  { 1791,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1791 = MAX_U_B
  { 1792,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1792 = MAX_U_D
  { 1793,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1793 = MAX_U_H
  { 1794,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1794 = MAX_U_W
  { 1795,	3,	1,	4,	408,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1795 = MFC0
  { 1796,	3,	1,	4,	211,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1796 = MFC0_MMR6
  { 1797,	2,	1,	4,	667,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo102, -1 ,nullptr },  // Inst #1797 = MFC1
  { 1798,	2,	1,	4,	667,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo286, -1 ,nullptr },  // Inst #1798 = MFC1_D64
  { 1799,	2,	1,	4,	5,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo102, -1 ,nullptr },  // Inst #1799 = MFC1_MM
  { 1800,	2,	1,	4,	5,	0|(1ULL<<MCID::Bitcast), 0x6ULL, nullptr, nullptr, OperandInfo102, -1 ,nullptr },  // Inst #1800 = MFC1_MMR6
  { 1801,	3,	1,	4,	410,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo287, -1 ,nullptr },  // Inst #1801 = MFC2
  { 1802,	2,	1,	4,	212,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo189, -1 ,nullptr },  // Inst #1802 = MFC2_MMR6
  { 1803,	3,	1,	4,	213,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1803 = MFGC0
  { 1804,	3,	1,	4,	213,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1804 = MFGC0_MM
  { 1805,	3,	1,	4,	214,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1805 = MFHC0_MMR6
  { 1806,	2,	1,	4,	668,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo288, -1 ,nullptr },  // Inst #1806 = MFHC1_D32
  { 1807,	2,	1,	4,	215,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo288, -1 ,nullptr },  // Inst #1807 = MFHC1_D32_MM
  { 1808,	2,	1,	4,	668,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo286, -1 ,nullptr },  // Inst #1808 = MFHC1_D64
  { 1809,	2,	1,	4,	215,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo286, -1 ,nullptr },  // Inst #1809 = MFHC1_D64_MM
  { 1810,	2,	1,	4,	212,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo189, -1 ,nullptr },  // Inst #1810 = MFHC2_MMR6
  { 1811,	3,	1,	4,	216,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1811 = MFHGC0
  { 1812,	3,	1,	4,	216,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo101, -1 ,nullptr },  // Inst #1812 = MFHGC0_MM
  { 1813,	1,	1,	4,	459,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1813 = MFHI
  { 1814,	1,	1,	2,	15,	0|(1ULL<<MCID::MoveReg), 0x0ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1814 = MFHI16_MM
  { 1815,	1,	1,	4,	15,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList20, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #1815 = MFHI64
  { 1816,	2,	1,	4,	744,	0|(1ULL<<MCID::MoveReg), 0x6ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #1816 = MFHI_DSP
  { 1817,	2,	1,	4,	900,	0, 0x6ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #1817 = MFHI_DSP_MM
  { 1818,	1,	1,	4,	15,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1818 = MFHI_MM
  { 1819,	1,	1,	4,	459,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1819 = MFLO
  { 1820,	1,	1,	2,	15,	0|(1ULL<<MCID::MoveReg), 0x0ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1820 = MFLO16_MM
  { 1821,	1,	1,	4,	15,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList20, nullptr, OperandInfo81, -1 ,nullptr },  // Inst #1821 = MFLO64
  { 1822,	2,	1,	4,	745,	0|(1ULL<<MCID::MoveReg), 0x6ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #1822 = MFLO_DSP
  { 1823,	2,	1,	4,	901,	0, 0x6ULL, nullptr, nullptr, OperandInfo100, -1 ,nullptr },  // Inst #1823 = MFLO_DSP_MM
  { 1824,	1,	1,	4,	15,	0|(1ULL<<MCID::MoveReg), 0x1ULL, ImplicitList19, nullptr, OperandInfo44, -1 ,nullptr },  // Inst #1824 = MFLO_MM
  { 1825,	5,	1,	4,	217,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo289, -1 ,nullptr },  // Inst #1825 = MFTR
  { 1826,	3,	1,	4,	218,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1826 = MINA_D
  { 1827,	3,	1,	4,	219,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1827 = MINA_D_MMR6
  { 1828,	3,	1,	4,	220,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1828 = MINA_S
  { 1829,	3,	1,	4,	221,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1829 = MINA_S_MMR6
  { 1830,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #1830 = MINI_S_B
  { 1831,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #1831 = MINI_S_D
  { 1832,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #1832 = MINI_S_H
  { 1833,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #1833 = MINI_S_W
  { 1834,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #1834 = MINI_U_B
  { 1835,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #1835 = MINI_U_D
  { 1836,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #1836 = MINI_U_H
  { 1837,	3,	1,	4,	597,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #1837 = MINI_U_W
  { 1838,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1838 = MIN_A_B
  { 1839,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1839 = MIN_A_D
  { 1840,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1840 = MIN_A_H
  { 1841,	3,	1,	4,	596,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1841 = MIN_A_W
  { 1842,	3,	1,	4,	220,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1842 = MIN_D
  { 1843,	3,	1,	4,	220,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #1843 = MIN_D_MMR6
  { 1844,	3,	1,	4,	218,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1844 = MIN_S
  { 1845,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1845 = MIN_S_B
  { 1846,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1846 = MIN_S_D
  { 1847,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1847 = MIN_S_H
  { 1848,	3,	1,	4,	218,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #1848 = MIN_S_MMR6
  { 1849,	3,	1,	4,	594,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1849 = MIN_S_W
  { 1850,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1850 = MIN_U_B
  { 1851,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1851 = MIN_U_D
  { 1852,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1852 = MIN_U_H
  { 1853,	3,	1,	4,	595,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1853 = MIN_U_W
  { 1854,	3,	1,	4,	222,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1854 = MOD
  { 1855,	3,	1,	4,	746,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1855 = MODSUB
  { 1856,	3,	1,	4,	902,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1856 = MODSUB_MM
  { 1857,	3,	1,	4,	223,	0|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1857 = MODU
  { 1858,	3,	1,	4,	223,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1858 = MODU_MMR6
  { 1859,	3,	1,	4,	222,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UsesCustomInserter), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1859 = MOD_MMR6
  { 1860,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1860 = MOD_S_B
  { 1861,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1861 = MOD_S_D
  { 1862,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1862 = MOD_S_H
  { 1863,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1863 = MOD_S_W
  { 1864,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #1864 = MOD_U_B
  { 1865,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #1865 = MOD_U_D
  { 1866,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1866 = MOD_U_H
  { 1867,	3,	1,	4,	590,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1867 = MOD_U_W
  { 1868,	2,	1,	2,	224,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #1868 = MOVE16_MM
  { 1869,	2,	1,	2,	224,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #1869 = MOVE16_MMR6
  { 1870,	4,	1,	2,	903,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo290, -1 ,nullptr },  // Inst #1870 = MOVEP_MM
  { 1871,	4,	1,	2,	904,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo290, -1 ,nullptr },  // Inst #1871 = MOVEP_MMR6
  { 1872,	2,	1,	4,	526,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo291, -1 ,nullptr },  // Inst #1872 = MOVE_V
  { 1873,	4,	1,	4,	511,	0, 0x4ULL, nullptr, nullptr, OperandInfo292, -1 ,nullptr },  // Inst #1873 = MOVF_D32
  { 1874,	4,	1,	4,	225,	0, 0x4ULL, nullptr, nullptr, OperandInfo292, -1 ,nullptr },  // Inst #1874 = MOVF_D32_MM
  { 1875,	4,	1,	4,	511,	0, 0x4ULL, nullptr, nullptr, OperandInfo293, -1 ,nullptr },  // Inst #1875 = MOVF_D64
  { 1876,	4,	1,	4,	669,	0, 0x4ULL, nullptr, nullptr, OperandInfo294, -1 ,nullptr },  // Inst #1876 = MOVF_I
  { 1877,	4,	1,	4,	226,	0, 0x4ULL, nullptr, nullptr, OperandInfo295, -1 ,nullptr },  // Inst #1877 = MOVF_I64
  { 1878,	4,	1,	4,	226,	0, 0x4ULL, nullptr, nullptr, OperandInfo294, -1 ,nullptr },  // Inst #1878 = MOVF_I_MM
  { 1879,	4,	1,	4,	512,	0, 0x4ULL, nullptr, nullptr, OperandInfo296, -1 ,nullptr },  // Inst #1879 = MOVF_S
  { 1880,	4,	1,	4,	227,	0, 0x4ULL, nullptr, nullptr, OperandInfo296, -1 ,nullptr },  // Inst #1880 = MOVF_S_MM
  { 1881,	4,	1,	4,	228,	0, 0x4ULL, nullptr, nullptr, OperandInfo297, -1 ,nullptr },  // Inst #1881 = MOVN_I64_D64
  { 1882,	4,	1,	4,	229,	0, 0x4ULL, nullptr, nullptr, OperandInfo298, -1 ,nullptr },  // Inst #1882 = MOVN_I64_I
  { 1883,	4,	1,	4,	229,	0, 0x4ULL, nullptr, nullptr, OperandInfo299, -1 ,nullptr },  // Inst #1883 = MOVN_I64_I64
  { 1884,	4,	1,	4,	230,	0, 0x4ULL, nullptr, nullptr, OperandInfo300, -1 ,nullptr },  // Inst #1884 = MOVN_I64_S
  { 1885,	4,	1,	4,	677,	0, 0x4ULL, nullptr, nullptr, OperandInfo301, -1 ,nullptr },  // Inst #1885 = MOVN_I_D32
  { 1886,	4,	1,	4,	228,	0, 0x4ULL, nullptr, nullptr, OperandInfo301, -1 ,nullptr },  // Inst #1886 = MOVN_I_D32_MM
  { 1887,	4,	1,	4,	677,	0, 0x4ULL, nullptr, nullptr, OperandInfo302, -1 ,nullptr },  // Inst #1887 = MOVN_I_D64
  { 1888,	4,	1,	4,	463,	0, 0x4ULL, nullptr, nullptr, OperandInfo303, -1 ,nullptr },  // Inst #1888 = MOVN_I_I
  { 1889,	4,	1,	4,	229,	0, 0x4ULL, nullptr, nullptr, OperandInfo304, -1 ,nullptr },  // Inst #1889 = MOVN_I_I64
  { 1890,	4,	1,	4,	905,	0, 0x4ULL, nullptr, nullptr, OperandInfo303, -1 ,nullptr },  // Inst #1890 = MOVN_I_MM
  { 1891,	4,	1,	4,	678,	0, 0x4ULL, nullptr, nullptr, OperandInfo305, -1 ,nullptr },  // Inst #1891 = MOVN_I_S
  { 1892,	4,	1,	4,	230,	0, 0x4ULL, nullptr, nullptr, OperandInfo305, -1 ,nullptr },  // Inst #1892 = MOVN_I_S_MM
  { 1893,	4,	1,	4,	513,	0, 0x4ULL, nullptr, nullptr, OperandInfo292, -1 ,nullptr },  // Inst #1893 = MOVT_D32
  { 1894,	4,	1,	4,	231,	0, 0x4ULL, nullptr, nullptr, OperandInfo292, -1 ,nullptr },  // Inst #1894 = MOVT_D32_MM
  { 1895,	4,	1,	4,	513,	0, 0x4ULL, nullptr, nullptr, OperandInfo293, -1 ,nullptr },  // Inst #1895 = MOVT_D64
  { 1896,	4,	1,	4,	670,	0, 0x4ULL, nullptr, nullptr, OperandInfo294, -1 ,nullptr },  // Inst #1896 = MOVT_I
  { 1897,	4,	1,	4,	232,	0, 0x4ULL, nullptr, nullptr, OperandInfo295, -1 ,nullptr },  // Inst #1897 = MOVT_I64
  { 1898,	4,	1,	4,	232,	0, 0x4ULL, nullptr, nullptr, OperandInfo294, -1 ,nullptr },  // Inst #1898 = MOVT_I_MM
  { 1899,	4,	1,	4,	514,	0, 0x4ULL, nullptr, nullptr, OperandInfo296, -1 ,nullptr },  // Inst #1899 = MOVT_S
  { 1900,	4,	1,	4,	233,	0, 0x4ULL, nullptr, nullptr, OperandInfo296, -1 ,nullptr },  // Inst #1900 = MOVT_S_MM
  { 1901,	4,	1,	4,	234,	0, 0x4ULL, nullptr, nullptr, OperandInfo297, -1 ,nullptr },  // Inst #1901 = MOVZ_I64_D64
  { 1902,	4,	1,	4,	235,	0, 0x4ULL, nullptr, nullptr, OperandInfo298, -1 ,nullptr },  // Inst #1902 = MOVZ_I64_I
  { 1903,	4,	1,	4,	235,	0, 0x4ULL, nullptr, nullptr, OperandInfo299, -1 ,nullptr },  // Inst #1903 = MOVZ_I64_I64
  { 1904,	4,	1,	4,	236,	0, 0x4ULL, nullptr, nullptr, OperandInfo300, -1 ,nullptr },  // Inst #1904 = MOVZ_I64_S
  { 1905,	4,	1,	4,	679,	0, 0x4ULL, nullptr, nullptr, OperandInfo301, -1 ,nullptr },  // Inst #1905 = MOVZ_I_D32
  { 1906,	4,	1,	4,	234,	0, 0x4ULL, nullptr, nullptr, OperandInfo301, -1 ,nullptr },  // Inst #1906 = MOVZ_I_D32_MM
  { 1907,	4,	1,	4,	679,	0, 0x4ULL, nullptr, nullptr, OperandInfo302, -1 ,nullptr },  // Inst #1907 = MOVZ_I_D64
  { 1908,	4,	1,	4,	464,	0, 0x4ULL, nullptr, nullptr, OperandInfo303, -1 ,nullptr },  // Inst #1908 = MOVZ_I_I
  { 1909,	4,	1,	4,	235,	0, 0x4ULL, nullptr, nullptr, OperandInfo304, -1 ,nullptr },  // Inst #1909 = MOVZ_I_I64
  { 1910,	4,	1,	4,	906,	0, 0x4ULL, nullptr, nullptr, OperandInfo303, -1 ,nullptr },  // Inst #1910 = MOVZ_I_MM
  { 1911,	4,	1,	4,	680,	0, 0x4ULL, nullptr, nullptr, OperandInfo305, -1 ,nullptr },  // Inst #1911 = MOVZ_I_S
  { 1912,	4,	1,	4,	236,	0, 0x4ULL, nullptr, nullptr, OperandInfo305, -1 ,nullptr },  // Inst #1912 = MOVZ_I_S_MM
  { 1913,	2,	0,	4,	472,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1913 = MSUB
  { 1914,	4,	1,	4,	237,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #1914 = MSUBF_D
  { 1915,	4,	1,	4,	237,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #1915 = MSUBF_D_MMR6
  { 1916,	4,	1,	4,	238,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo282, -1 ,nullptr },  // Inst #1916 = MSUBF_S
  { 1917,	4,	1,	4,	238,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo282, -1 ,nullptr },  // Inst #1917 = MSUBF_S_MMR6
  { 1918,	4,	1,	4,	645,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1918 = MSUBR_Q_H
  { 1919,	4,	1,	4,	645,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1919 = MSUBR_Q_W
  { 1920,	2,	0,	4,	473,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1920 = MSUBU
  { 1921,	4,	1,	4,	747,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1921 = MSUBU_DSP
  { 1922,	4,	1,	4,	907,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1922 = MSUBU_DSP_MM
  { 1923,	2,	0,	4,	17,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1923 = MSUBU_MM
  { 1924,	4,	1,	4,	640,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #1924 = MSUBV_B
  { 1925,	4,	1,	4,	640,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #1925 = MSUBV_D
  { 1926,	4,	1,	4,	640,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1926 = MSUBV_H
  { 1927,	4,	1,	4,	640,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1927 = MSUBV_W
  { 1928,	4,	1,	4,	651,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #1928 = MSUB_D32
  { 1929,	4,	1,	4,	239,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #1929 = MSUB_D32_MM
  { 1930,	4,	1,	4,	651,	0, 0x4ULL, nullptr, nullptr, OperandInfo284, -1 ,nullptr },  // Inst #1930 = MSUB_D64
  { 1931,	4,	1,	4,	748,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1931 = MSUB_DSP
  { 1932,	4,	1,	4,	908,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #1932 = MSUB_DSP_MM
  { 1933,	2,	0,	4,	16,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, ImplicitList6, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #1933 = MSUB_MM
  { 1934,	4,	1,	4,	646,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #1934 = MSUB_Q_H
  { 1935,	4,	1,	4,	646,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #1935 = MSUB_Q_W
  { 1936,	4,	1,	4,	652,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #1936 = MSUB_S
  { 1937,	4,	1,	4,	240,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #1937 = MSUB_S_MM
  { 1938,	3,	1,	4,	409,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1938 = MTC0
  { 1939,	3,	1,	4,	241,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1939 = MTC0_MMR6
  { 1940,	2,	1,	4,	658,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #1940 = MTC1
  { 1941,	2,	1,	4,	658,	0|(1ULL<<MCID::MoveReg), 0x4ULL, nullptr, nullptr, OperandInfo116, -1 ,nullptr },  // Inst #1941 = MTC1_D64
  { 1942,	2,	1,	4,	4,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::Bitcast), 0x4ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #1942 = MTC1_MM
  { 1943,	2,	1,	4,	4,	0|(1ULL<<MCID::Bitcast), 0x6ULL, nullptr, nullptr, OperandInfo110, -1 ,nullptr },  // Inst #1943 = MTC1_MMR6
  { 1944,	3,	1,	4,	411,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo306, -1 ,nullptr },  // Inst #1944 = MTC2
  { 1945,	2,	1,	4,	242,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo202, -1 ,nullptr },  // Inst #1945 = MTC2_MMR6
  { 1946,	3,	1,	4,	243,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1946 = MTGC0
  { 1947,	3,	1,	4,	243,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1947 = MTGC0_MM
  { 1948,	3,	1,	4,	244,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1948 = MTHC0_MMR6
  { 1949,	3,	1,	4,	659,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo307, -1 ,nullptr },  // Inst #1949 = MTHC1_D32
  { 1950,	3,	1,	4,	245,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo307, -1 ,nullptr },  // Inst #1950 = MTHC1_D32_MM
  { 1951,	3,	1,	4,	659,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo308, -1 ,nullptr },  // Inst #1951 = MTHC1_D64
  { 1952,	3,	1,	4,	245,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo308, -1 ,nullptr },  // Inst #1952 = MTHC1_D64_MM
  { 1953,	2,	1,	4,	242,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo202, -1 ,nullptr },  // Inst #1953 = MTHC2_MMR6
  { 1954,	3,	1,	4,	246,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x4ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1954 = MTHGC0
  { 1955,	3,	1,	4,	246,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo109, -1 ,nullptr },  // Inst #1955 = MTHGC0_MM
  { 1956,	1,	0,	4,	474,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList21, OperandInfo44, -1 ,nullptr },  // Inst #1956 = MTHI
  { 1957,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList22, OperandInfo81, -1 ,nullptr },  // Inst #1957 = MTHI64
  { 1958,	2,	1,	4,	701,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo309, -1 ,nullptr },  // Inst #1958 = MTHI_DSP
  { 1959,	2,	1,	4,	909,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo309, -1 ,nullptr },  // Inst #1959 = MTHI_DSP_MM
  { 1960,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList21, OperandInfo44, -1 ,nullptr },  // Inst #1960 = MTHI_MM
  { 1961,	3,	1,	4,	700,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList4, OperandInfo310, -1 ,nullptr },  // Inst #1961 = MTHLIP
  { 1962,	3,	1,	4,	910,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList4, OperandInfo310, -1 ,nullptr },  // Inst #1962 = MTHLIP_MM
  { 1963,	1,	0,	4,	474,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList23, OperandInfo44, -1 ,nullptr },  // Inst #1963 = MTLO
  { 1964,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList24, OperandInfo81, -1 ,nullptr },  // Inst #1964 = MTLO64
  { 1965,	2,	1,	4,	702,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo311, -1 ,nullptr },  // Inst #1965 = MTLO_DSP
  { 1966,	2,	1,	4,	911,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo311, -1 ,nullptr },  // Inst #1966 = MTLO_DSP_MM
  { 1967,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList23, OperandInfo44, -1 ,nullptr },  // Inst #1967 = MTLO_MM
  { 1968,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList25, OperandInfo81, -1 ,nullptr },  // Inst #1968 = MTM0
  { 1969,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList26, OperandInfo81, -1 ,nullptr },  // Inst #1969 = MTM1
  { 1970,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList27, OperandInfo81, -1 ,nullptr },  // Inst #1970 = MTM2
  { 1971,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList28, OperandInfo81, -1 ,nullptr },  // Inst #1971 = MTP0
  { 1972,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList29, OperandInfo81, -1 ,nullptr },  // Inst #1972 = MTP1
  { 1973,	1,	0,	4,	18,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, ImplicitList30, OperandInfo81, -1 ,nullptr },  // Inst #1973 = MTP2
  { 1974,	5,	1,	4,	247,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo289, -1 ,nullptr },  // Inst #1974 = MTTR
  { 1975,	3,	1,	4,	248,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1975 = MUH
  { 1976,	3,	1,	4,	249,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1976 = MUHU
  { 1977,	3,	1,	4,	249,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1977 = MUHU_MMR6
  { 1978,	3,	1,	4,	248,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #1978 = MUH_MMR6
  { 1979,	3,	1,	4,	467,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, ImplicitList6, OperandInfo58, -1 ,nullptr },  // Inst #1979 = MUL
  { 1980,	3,	1,	4,	749,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo194, -1 ,nullptr },  // Inst #1980 = MULEQ_S_W_PHL
  { 1981,	3,	1,	4,	912,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo194, -1 ,nullptr },  // Inst #1981 = MULEQ_S_W_PHL_MM
  { 1982,	3,	1,	4,	750,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo194, -1 ,nullptr },  // Inst #1982 = MULEQ_S_W_PHR
  { 1983,	3,	1,	4,	913,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo194, -1 ,nullptr },  // Inst #1983 = MULEQ_S_W_PHR_MM
  { 1984,	3,	1,	4,	751,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1984 = MULEU_S_PH_QBL
  { 1985,	3,	1,	4,	914,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1985 = MULEU_S_PH_QBL_MM
  { 1986,	3,	1,	4,	752,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1986 = MULEU_S_PH_QBR
  { 1987,	3,	1,	4,	915,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1987 = MULEU_S_PH_QBR_MM
  { 1988,	3,	1,	4,	753,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1988 = MULQ_RS_PH
  { 1989,	3,	1,	4,	916,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1989 = MULQ_RS_PH_MM
  { 1990,	3,	1,	4,	828,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo58, -1 ,nullptr },  // Inst #1990 = MULQ_RS_W
  { 1991,	3,	1,	4,	991,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo58, -1 ,nullptr },  // Inst #1991 = MULQ_RS_W_MMR2
  { 1992,	3,	1,	4,	829,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1992 = MULQ_S_PH
  { 1993,	3,	1,	4,	992,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #1993 = MULQ_S_PH_MMR2
  { 1994,	3,	1,	4,	830,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo58, -1 ,nullptr },  // Inst #1994 = MULQ_S_W
  { 1995,	3,	1,	4,	993,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo58, -1 ,nullptr },  // Inst #1995 = MULQ_S_W_MMR2
  { 1996,	3,	1,	4,	647,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #1996 = MULR_Q_H
  { 1997,	3,	1,	4,	647,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #1997 = MULR_Q_W
  { 1998,	4,	1,	4,	754,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1998 = MULSAQ_S_W_PH
  { 1999,	4,	1,	4,	917,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList13, OperandInfo222, -1 ,nullptr },  // Inst #1999 = MULSAQ_S_W_PH_MM
  { 2000,	4,	1,	4,	831,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #2000 = MULSA_W_PH
  { 2001,	4,	1,	4,	994,	0, 0x6ULL, nullptr, nullptr, OperandInfo222, -1 ,nullptr },  // Inst #2001 = MULSA_W_PH_MMR2
  { 2002,	2,	0,	4,	468,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2002 = MULT
  { 2003,	3,	1,	4,	755,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo122, -1 ,nullptr },  // Inst #2003 = MULTU_DSP
  { 2004,	3,	1,	4,	918,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo122, -1 ,nullptr },  // Inst #2004 = MULTU_DSP_MM
  { 2005,	3,	1,	4,	756,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo122, -1 ,nullptr },  // Inst #2005 = MULT_DSP
  { 2006,	3,	1,	4,	919,	0|(1ULL<<MCID::Commutable), 0x6ULL, nullptr, nullptr, OperandInfo122, -1 ,nullptr },  // Inst #2006 = MULT_DSP_MM
  { 2007,	2,	0,	4,	19,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2007 = MULT_MM
  { 2008,	2,	0,	4,	469,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2008 = MULTu
  { 2009,	2,	0,	4,	20,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2009 = MULTu_MM
  { 2010,	3,	1,	4,	251,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2010 = MULU
  { 2011,	3,	1,	4,	251,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2011 = MULU_MMR6
  { 2012,	3,	1,	4,	642,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2012 = MULV_B
  { 2013,	3,	1,	4,	642,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2013 = MULV_D
  { 2014,	3,	1,	4,	642,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2014 = MULV_H
  { 2015,	3,	1,	4,	642,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2015 = MULV_W
  { 2016,	3,	1,	4,	250,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, ImplicitList6, OperandInfo58, -1 ,nullptr },  // Inst #2016 = MUL_MM
  { 2017,	3,	1,	4,	250,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2017 = MUL_MMR6
  { 2018,	3,	1,	4,	826,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #2018 = MUL_PH
  { 2019,	3,	1,	4,	989,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #2019 = MUL_PH_MMR2
  { 2020,	3,	1,	4,	648,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2020 = MUL_Q_H
  { 2021,	3,	1,	4,	648,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2021 = MUL_Q_W
  { 2022,	3,	1,	4,	250,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2022 = MUL_R6
  { 2023,	3,	1,	4,	827,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #2023 = MUL_S_PH
  { 2024,	3,	1,	4,	990,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList31, OperandInfo150, -1 ,nullptr },  // Inst #2024 = MUL_S_PH_MMR2
  { 2025,	1,	1,	2,	38,	0|(1ULL<<MCID::MoveReg), 0x0ULL, ImplicitList21, nullptr, OperandInfo252, -1 ,nullptr },  // Inst #2025 = Mfhi16
  { 2026,	1,	1,	2,	38,	0, 0x0ULL, ImplicitList23, nullptr, OperandInfo252, -1 ,nullptr },  // Inst #2026 = Mflo16
  { 2027,	2,	1,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo312, -1 ,nullptr },  // Inst #2027 = Move32R16
  { 2028,	2,	1,	2,	38,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo313, -1 ,nullptr },  // Inst #2028 = MoveR3216
  { 2029,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo291, -1 ,nullptr },  // Inst #2029 = NLOC_B
  { 2030,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #2030 = NLOC_D
  { 2031,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo314, -1 ,nullptr },  // Inst #2031 = NLOC_H
  { 2032,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #2032 = NLOC_W
  { 2033,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo291, -1 ,nullptr },  // Inst #2033 = NLZC_B
  { 2034,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #2034 = NLZC_D
  { 2035,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo314, -1 ,nullptr },  // Inst #2035 = NLZC_H
  { 2036,	2,	1,	4,	604,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #2036 = NLZC_W
  { 2037,	4,	1,	4,	653,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #2037 = NMADD_D32
  { 2038,	4,	1,	4,	252,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #2038 = NMADD_D32_MM
  { 2039,	4,	1,	4,	653,	0, 0x4ULL, nullptr, nullptr, OperandInfo284, -1 ,nullptr },  // Inst #2039 = NMADD_D64
  { 2040,	4,	1,	4,	654,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #2040 = NMADD_S
  { 2041,	4,	1,	4,	253,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #2041 = NMADD_S_MM
  { 2042,	4,	1,	4,	655,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #2042 = NMSUB_D32
  { 2043,	4,	1,	4,	254,	0, 0x4ULL, nullptr, nullptr, OperandInfo283, -1 ,nullptr },  // Inst #2043 = NMSUB_D32_MM
  { 2044,	4,	1,	4,	655,	0, 0x4ULL, nullptr, nullptr, OperandInfo284, -1 ,nullptr },  // Inst #2044 = NMSUB_D64
  { 2045,	4,	1,	4,	656,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #2045 = NMSUB_S
  { 2046,	4,	1,	4,	255,	0, 0x4ULL, nullptr, nullptr, OperandInfo285, -1 ,nullptr },  // Inst #2046 = NMSUB_S_MM
  { 2047,	3,	1,	4,	360,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2047 = NOR
  { 2048,	3,	1,	4,	256,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2048 = NOR64
  { 2049,	3,	1,	4,	529,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2049 = NORI_B
  { 2050,	3,	1,	4,	256,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2050 = NOR_MM
  { 2051,	3,	1,	4,	256,	0|(1ULL<<MCID::Commutable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2051 = NOR_MMR6
  { 2052,	3,	1,	4,	528,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2052 = NOR_V
  { 2053,	2,	1,	2,	257,	0, 0x0ULL, nullptr, nullptr, OperandInfo315, -1 ,nullptr },  // Inst #2053 = NOT16_MM
  { 2054,	2,	1,	2,	257,	0, 0x0ULL, nullptr, nullptr, OperandInfo315, -1 ,nullptr },  // Inst #2054 = NOT16_MMR6
  { 2055,	2,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo111, -1 ,nullptr },  // Inst #2055 = NegRxRy16
  { 2056,	2,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo111, -1 ,nullptr },  // Inst #2056 = NotRxRy16
  { 2057,	3,	1,	4,	361,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2057 = OR
  { 2058,	3,	1,	2,	258,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #2058 = OR16_MM
  { 2059,	3,	1,	2,	258,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #2059 = OR16_MMR6
  { 2060,	3,	1,	4,	258,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2060 = OR64
  { 2061,	3,	1,	4,	529,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2061 = ORI_B
  { 2062,	3,	1,	4,	259,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2062 = ORI_MMR6
  { 2063,	3,	1,	4,	258,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2063 = OR_MM
  { 2064,	3,	1,	4,	258,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2064 = OR_MMR6
  { 2065,	3,	1,	4,	528,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2065 = OR_V
  { 2066,	3,	1,	4,	481,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2066 = ORi
  { 2067,	3,	1,	4,	258,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #2067 = ORi64
  { 2068,	3,	1,	4,	259,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2068 = ORi_MM
  { 2069,	3,	1,	2,	38,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #2069 = OrRxRxRy16
  { 2070,	3,	1,	4,	757,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2070 = PACKRL_PH
  { 2071,	3,	1,	4,	920,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2071 = PACKRL_PH_MM
  { 2072,	0,	0,	4,	397,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2072 = PAUSE
  { 2073,	0,	0,	4,	260,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2073 = PAUSE_MM
  { 2074,	0,	0,	4,	260,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2074 = PAUSE_MMR6
  { 2075,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2075 = PCKEV_B
  { 2076,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2076 = PCKEV_D
  { 2077,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2077 = PCKEV_H
  { 2078,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2078 = PCKEV_W
  { 2079,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2079 = PCKOD_B
  { 2080,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2080 = PCKOD_D
  { 2081,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2081 = PCKOD_H
  { 2082,	3,	1,	4,	603,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2082 = PCKOD_W
  { 2083,	2,	1,	4,	506,	0, 0x6ULL, nullptr, nullptr, OperandInfo291, -1 ,nullptr },  // Inst #2083 = PCNT_B
  { 2084,	2,	1,	4,	506,	0, 0x6ULL, nullptr, nullptr, OperandInfo62, -1 ,nullptr },  // Inst #2084 = PCNT_D
  { 2085,	2,	1,	4,	506,	0, 0x6ULL, nullptr, nullptr, OperandInfo314, -1 ,nullptr },  // Inst #2085 = PCNT_H
  { 2086,	2,	1,	4,	506,	0, 0x6ULL, nullptr, nullptr, OperandInfo63, -1 ,nullptr },  // Inst #2086 = PCNT_W
  { 2087,	3,	1,	4,	758,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList10, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2087 = PICK_PH
  { 2088,	3,	1,	4,	921,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList10, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2088 = PICK_PH_MM
  { 2089,	3,	1,	4,	759,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList10, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2089 = PICK_QB
  { 2090,	3,	1,	4,	922,	0|(1ULL<<MCID::MayLoad), 0x6ULL, ImplicitList10, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2090 = PICK_QB_MM
  { 2091,	2,	1,	4,	109,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2091 = POP
  { 2092,	2,	1,	4,	761,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2092 = PRECEQU_PH_QBL
  { 2093,	2,	1,	4,	760,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2093 = PRECEQU_PH_QBLA
  { 2094,	2,	1,	4,	923,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2094 = PRECEQU_PH_QBLA_MM
  { 2095,	2,	1,	4,	924,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2095 = PRECEQU_PH_QBL_MM
  { 2096,	2,	1,	4,	763,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2096 = PRECEQU_PH_QBR
  { 2097,	2,	1,	4,	762,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2097 = PRECEQU_PH_QBRA
  { 2098,	2,	1,	4,	925,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2098 = PRECEQU_PH_QBRA_MM
  { 2099,	2,	1,	4,	926,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2099 = PRECEQU_PH_QBR_MM
  { 2100,	2,	1,	4,	764,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2100 = PRECEQ_W_PHL
  { 2101,	2,	1,	4,	927,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2101 = PRECEQ_W_PHL_MM
  { 2102,	2,	1,	4,	765,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2102 = PRECEQ_W_PHR
  { 2103,	2,	1,	4,	928,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2103 = PRECEQ_W_PHR_MM
  { 2104,	2,	1,	4,	767,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2104 = PRECEU_PH_QBL
  { 2105,	2,	1,	4,	766,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2105 = PRECEU_PH_QBLA
  { 2106,	2,	1,	4,	929,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2106 = PRECEU_PH_QBLA_MM
  { 2107,	2,	1,	4,	930,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2107 = PRECEU_PH_QBL_MM
  { 2108,	2,	1,	4,	769,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2108 = PRECEU_PH_QBR
  { 2109,	2,	1,	4,	768,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2109 = PRECEU_PH_QBRA
  { 2110,	2,	1,	4,	931,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2110 = PRECEU_PH_QBRA_MM
  { 2111,	2,	1,	4,	932,	0, 0x6ULL, nullptr, nullptr, OperandInfo146, -1 ,nullptr },  // Inst #2111 = PRECEU_PH_QBR_MM
  { 2112,	3,	1,	4,	770,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo150, -1 ,nullptr },  // Inst #2112 = PRECRQU_S_QB_PH
  { 2113,	3,	1,	4,	933,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo150, -1 ,nullptr },  // Inst #2113 = PRECRQU_S_QB_PH_MM
  { 2114,	3,	1,	4,	771,	0, 0x6ULL, nullptr, nullptr, OperandInfo317, -1 ,nullptr },  // Inst #2114 = PRECRQ_PH_W
  { 2115,	3,	1,	4,	934,	0, 0x6ULL, nullptr, nullptr, OperandInfo317, -1 ,nullptr },  // Inst #2115 = PRECRQ_PH_W_MM
  { 2116,	3,	1,	4,	772,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2116 = PRECRQ_QB_PH
  { 2117,	3,	1,	4,	935,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2117 = PRECRQ_QB_PH_MM
  { 2118,	3,	1,	4,	773,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo317, -1 ,nullptr },  // Inst #2118 = PRECRQ_RS_PH_W
  { 2119,	3,	1,	4,	936,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo317, -1 ,nullptr },  // Inst #2119 = PRECRQ_RS_PH_W_MM
  { 2120,	3,	1,	4,	832,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2120 = PRECR_QB_PH
  { 2121,	3,	1,	4,	995,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2121 = PRECR_QB_PH_MMR2
  { 2122,	4,	1,	4,	833,	0, 0x6ULL, nullptr, nullptr, OperandInfo318, -1 ,nullptr },  // Inst #2122 = PRECR_SRA_PH_W
  { 2123,	4,	1,	4,	996,	0, 0x6ULL, nullptr, nullptr, OperandInfo318, -1 ,nullptr },  // Inst #2123 = PRECR_SRA_PH_W_MMR2
  { 2124,	4,	1,	4,	834,	0, 0x6ULL, nullptr, nullptr, OperandInfo318, -1 ,nullptr },  // Inst #2124 = PRECR_SRA_R_PH_W
  { 2125,	4,	1,	4,	997,	0, 0x6ULL, nullptr, nullptr, OperandInfo318, -1 ,nullptr },  // Inst #2125 = PRECR_SRA_R_PH_W_MMR2
  { 2126,	3,	0,	4,	449,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2126 = PREF
  { 2127,	3,	0,	4,	450,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2127 = PREFE
  { 2128,	3,	0,	4,	262,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2128 = PREFE_MM
  { 2129,	3,	0,	4,	261,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo319, -1 ,nullptr },  // Inst #2129 = PREFX_MM
  { 2130,	3,	0,	4,	261,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2130 = PREF_MM
  { 2131,	3,	0,	4,	261,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2131 = PREF_MMR6
  { 2132,	3,	0,	4,	261,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo182, -1 ,nullptr },  // Inst #2132 = PREF_R6
  { 2133,	4,	1,	4,	835,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #2133 = PREPEND
  { 2134,	4,	1,	4,	998,	0, 0x6ULL, nullptr, nullptr, OperandInfo159, -1 ,nullptr },  // Inst #2134 = PREPEND_MMR2
  { 2135,	2,	1,	4,	774,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2135 = RADDU_W_QB
  { 2136,	2,	1,	4,	937,	0, 0x6ULL, nullptr, nullptr, OperandInfo316, -1 ,nullptr },  // Inst #2136 = RADDU_W_QB_MM
  { 2137,	2,	1,	4,	775,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2137 = RDDSP
  { 2138,	2,	1,	4,	938,	0|(1ULL<<MCID::MayLoad), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2138 = RDDSP_MM
  { 2139,	3,	1,	4,	461,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo320, -1 ,nullptr },  // Inst #2139 = RDHWR
  { 2140,	3,	1,	4,	263,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo321, -1 ,nullptr },  // Inst #2140 = RDHWR64
  { 2141,	3,	1,	4,	263,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo320, -1 ,nullptr },  // Inst #2141 = RDHWR_MM
  { 2142,	3,	1,	4,	263,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo320, -1 ,nullptr },  // Inst #2142 = RDHWR_MMR6
  { 2143,	2,	1,	4,	264,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2143 = RDPGPR_MMR6
  { 2144,	2,	1,	4,	624,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #2144 = RECIP_D32
  { 2145,	2,	1,	4,	265,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #2145 = RECIP_D32_MM
  { 2146,	2,	1,	4,	624,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2146 = RECIP_D64
  { 2147,	2,	1,	4,	265,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2147 = RECIP_D64_MM
  { 2148,	2,	1,	4,	626,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2148 = RECIP_S
  { 2149,	2,	1,	4,	266,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2149 = RECIP_S_MM
  { 2150,	2,	1,	4,	776,	0, 0x6ULL, nullptr, nullptr, OperandInfo322, -1 ,nullptr },  // Inst #2150 = REPLV_PH
  { 2151,	2,	1,	4,	939,	0, 0x6ULL, nullptr, nullptr, OperandInfo322, -1 ,nullptr },  // Inst #2151 = REPLV_PH_MM
  { 2152,	2,	1,	4,	777,	0, 0x6ULL, nullptr, nullptr, OperandInfo322, -1 ,nullptr },  // Inst #2152 = REPLV_QB
  { 2153,	2,	1,	4,	940,	0, 0x6ULL, nullptr, nullptr, OperandInfo322, -1 ,nullptr },  // Inst #2153 = REPLV_QB_MM
  { 2154,	2,	1,	4,	778,	0, 0x6ULL, nullptr, nullptr, OperandInfo323, -1 ,nullptr },  // Inst #2154 = REPL_PH
  { 2155,	2,	1,	4,	941,	0, 0x6ULL, nullptr, nullptr, OperandInfo323, -1 ,nullptr },  // Inst #2155 = REPL_PH_MM
  { 2156,	2,	1,	4,	779,	0, 0x6ULL, nullptr, nullptr, OperandInfo323, -1 ,nullptr },  // Inst #2156 = REPL_QB
  { 2157,	2,	1,	4,	942,	0, 0x6ULL, nullptr, nullptr, OperandInfo323, -1 ,nullptr },  // Inst #2157 = REPL_QB_MM
  { 2158,	2,	1,	4,	267,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2158 = RINT_D
  { 2159,	2,	1,	4,	268,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2159 = RINT_D_MMR6
  { 2160,	2,	1,	4,	268,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2160 = RINT_S
  { 2161,	2,	1,	4,	268,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2161 = RINT_S_MMR6
  { 2162,	3,	1,	4,	482,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2162 = ROTR
  { 2163,	3,	1,	4,	690,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2163 = ROTRV
  { 2164,	3,	1,	4,	270,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2164 = ROTRV_MM
  { 2165,	3,	1,	4,	269,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2165 = ROTR_MM
  { 2166,	2,	1,	4,	689,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2166 = ROUND_L_D64
  { 2167,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2167 = ROUND_L_D_MMR6
  { 2168,	2,	1,	4,	689,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #2168 = ROUND_L_S
  { 2169,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #2169 = ROUND_L_S_MMR6
  { 2170,	2,	1,	4,	689,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #2170 = ROUND_W_D32
  { 2171,	2,	1,	4,	689,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #2171 = ROUND_W_D64
  { 2172,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2172 = ROUND_W_D_MMR6
  { 2173,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #2173 = ROUND_W_MM
  { 2174,	2,	1,	4,	689,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2174 = ROUND_W_S
  { 2175,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2175 = ROUND_W_S_MM
  { 2176,	2,	1,	4,	271,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2176 = ROUND_W_S_MMR6
  { 2177,	2,	1,	4,	625,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #2177 = RSQRT_D32
  { 2178,	2,	1,	4,	265,	0, 0x4ULL, nullptr, nullptr, OperandInfo227, -1 ,nullptr },  // Inst #2178 = RSQRT_D32_MM
  { 2179,	2,	1,	4,	625,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2179 = RSQRT_D64
  { 2180,	2,	1,	4,	265,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2180 = RSQRT_D64_MM
  { 2181,	2,	1,	4,	627,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2181 = RSQRT_S
  { 2182,	2,	1,	4,	266,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2182 = RSQRT_S_MM
  { 2183,	0,	0,	2,	274,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, nullptr, -1 ,nullptr },  // Inst #2183 = Restore16
  { 2184,	0,	0,	2,	274,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, nullptr, -1 ,nullptr },  // Inst #2184 = RestoreX16
  { 2185,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2185 = SAT_S_B
  { 2186,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2186 = SAT_S_D
  { 2187,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2187 = SAT_S_H
  { 2188,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2188 = SAT_S_W
  { 2189,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2189 = SAT_U_B
  { 2190,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2190 = SAT_U_D
  { 2191,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2191 = SAT_U_H
  { 2192,	3,	1,	4,	507,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2192 = SAT_U_W
  { 2193,	3,	0,	4,	433,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2193 = SB
  { 2194,	3,	0,	2,	275,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2194 = SB16_MM
  { 2195,	3,	0,	2,	275,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2195 = SB16_MMR6
  { 2196,	3,	0,	4,	275,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2196 = SB64
  { 2197,	3,	0,	4,	441,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2197 = SBE
  { 2198,	3,	0,	4,	276,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2198 = SBE_MM
  { 2199,	3,	0,	4,	275,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2199 = SB_MM
  { 2200,	3,	0,	4,	275,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2200 = SB_MMR6
  { 2201,	4,	1,	4,	440,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2201 = SC
  { 2202,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2202 = SC64
  { 2203,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2203 = SC64_R6
  { 2204,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo326, -1 ,nullptr },  // Inst #2204 = SCD
  { 2205,	4,	1,	4,	278,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo326, -1 ,nullptr },  // Inst #2205 = SCD_R6
  { 2206,	4,	1,	4,	444,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2206 = SCE
  { 2207,	4,	1,	4,	279,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2207 = SCE_MM
  { 2208,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2208 = SC_MM
  { 2209,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2209 = SC_MMR6
  { 2210,	4,	1,	4,	277,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo325, -1 ,nullptr },  // Inst #2210 = SC_R6
  { 2211,	3,	0,	4,	280,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2211 = SD
  { 2212,	1,	0,	4,	380,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2212 = SDBBP
  { 2213,	1,	0,	2,	281,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2213 = SDBBP16_MM
  { 2214,	1,	0,	2,	281,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2214 = SDBBP16_MMR6
  { 2215,	1,	0,	4,	281,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2215 = SDBBP_MM
  { 2216,	1,	0,	4,	281,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2216 = SDBBP_MMR6
  { 2217,	1,	0,	4,	281,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2217 = SDBBP_R6
  { 2218,	3,	0,	4,	671,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo255, -1 ,nullptr },  // Inst #2218 = SDC1
  { 2219,	3,	0,	4,	282,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo256, -1 ,nullptr },  // Inst #2219 = SDC164
  { 2220,	3,	0,	4,	282,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo256, -1 ,nullptr },  // Inst #2220 = SDC1_D64_MMR6
  { 2221,	3,	0,	4,	282,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo255, -1 ,nullptr },  // Inst #2221 = SDC1_MM
  { 2222,	3,	0,	4,	438,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #2222 = SDC2
  { 2223,	3,	0,	4,	283,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo258, -1 ,nullptr },  // Inst #2223 = SDC2_MMR6
  { 2224,	3,	0,	4,	283,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #2224 = SDC2_R6
  { 2225,	3,	0,	4,	439,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo259, -1 ,nullptr },  // Inst #2225 = SDC3
  { 2226,	2,	0,	4,	465,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2226 = SDIV
  { 2227,	2,	0,	4,	22,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2227 = SDIV_MM
  { 2228,	3,	0,	4,	285,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2228 = SDL
  { 2229,	3,	0,	4,	286,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2229 = SDR
  { 2230,	3,	0,	4,	672,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo265, -1 ,nullptr },  // Inst #2230 = SDXC1
  { 2231,	3,	0,	4,	287,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #2231 = SDXC164
  { 2232,	2,	1,	4,	483,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2232 = SEB
  { 2233,	2,	1,	4,	288,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #2233 = SEB64
  { 2234,	2,	1,	4,	288,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2234 = SEB_MM
  { 2235,	2,	1,	4,	484,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2235 = SEH
  { 2236,	2,	1,	4,	289,	0, 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #2236 = SEH64
  { 2237,	2,	1,	4,	289,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2237 = SEH_MM
  { 2238,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2238 = SELEQZ
  { 2239,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2239 = SELEQZ64
  { 2240,	3,	1,	4,	291,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #2240 = SELEQZ_D
  { 2241,	3,	1,	4,	291,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #2241 = SELEQZ_D_MMR6
  { 2242,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2242 = SELEQZ_MMR6
  { 2243,	3,	1,	4,	292,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #2243 = SELEQZ_S
  { 2244,	3,	1,	4,	292,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #2244 = SELEQZ_S_MMR6
  { 2245,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2245 = SELNEZ
  { 2246,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2246 = SELNEZ64
  { 2247,	3,	1,	4,	291,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #2247 = SELNEZ_D
  { 2248,	3,	1,	4,	291,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo229, -1 ,nullptr },  // Inst #2248 = SELNEZ_D_MMR6
  { 2249,	3,	1,	4,	290,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2249 = SELNEZ_MMR6
  { 2250,	3,	1,	4,	292,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #2250 = SELNEZ_S
  { 2251,	3,	1,	4,	292,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo230, -1 ,nullptr },  // Inst #2251 = SELNEZ_S_MMR6
  { 2252,	4,	1,	4,	293,	0, 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #2252 = SEL_D
  { 2253,	4,	1,	4,	293,	0, 0x6ULL, nullptr, nullptr, OperandInfo281, -1 ,nullptr },  // Inst #2253 = SEL_D_MMR6
  { 2254,	4,	1,	4,	294,	0, 0x6ULL, nullptr, nullptr, OperandInfo327, -1 ,nullptr },  // Inst #2254 = SEL_S
  { 2255,	4,	1,	4,	294,	0, 0x6ULL, nullptr, nullptr, OperandInfo327, -1 ,nullptr },  // Inst #2255 = SEL_S_MMR6
  { 2256,	3,	1,	4,	295,	0, 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2256 = SEQ
  { 2257,	3,	1,	4,	296,	0, 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #2257 = SEQi
  { 2258,	3,	0,	4,	434,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2258 = SH
  { 2259,	3,	0,	2,	297,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2259 = SH16_MM
  { 2260,	3,	0,	2,	297,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2260 = SH16_MMR6
  { 2261,	3,	0,	4,	297,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2261 = SH64
  { 2262,	3,	0,	4,	442,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2262 = SHE
  { 2263,	3,	0,	4,	298,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2263 = SHE_MM
  { 2264,	3,	1,	4,	523,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2264 = SHF_B
  { 2265,	3,	1,	4,	523,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2265 = SHF_H
  { 2266,	3,	1,	4,	523,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2266 = SHF_W
  { 2267,	3,	1,	4,	781,	0, 0x6ULL, nullptr, nullptr, OperandInfo328, -1 ,nullptr },  // Inst #2267 = SHILO
  { 2268,	3,	1,	4,	780,	0, 0x6ULL, nullptr, nullptr, OperandInfo310, -1 ,nullptr },  // Inst #2268 = SHILOV
  { 2269,	3,	1,	4,	943,	0, 0x6ULL, nullptr, nullptr, OperandInfo310, -1 ,nullptr },  // Inst #2269 = SHILOV_MM
  { 2270,	3,	1,	4,	944,	0, 0x6ULL, nullptr, nullptr, OperandInfo328, -1 ,nullptr },  // Inst #2270 = SHILO_MM
  { 2271,	3,	1,	4,	782,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2271 = SHLLV_PH
  { 2272,	3,	1,	4,	945,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2272 = SHLLV_PH_MM
  { 2273,	3,	1,	4,	783,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2273 = SHLLV_QB
  { 2274,	3,	1,	4,	946,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2274 = SHLLV_QB_MM
  { 2275,	3,	1,	4,	784,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2275 = SHLLV_S_PH
  { 2276,	3,	1,	4,	947,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo329, -1 ,nullptr },  // Inst #2276 = SHLLV_S_PH_MM
  { 2277,	3,	1,	4,	785,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo58, -1 ,nullptr },  // Inst #2277 = SHLLV_S_W
  { 2278,	3,	1,	4,	948,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo58, -1 ,nullptr },  // Inst #2278 = SHLLV_S_W_MM
  { 2279,	3,	1,	4,	786,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2279 = SHLL_PH
  { 2280,	3,	1,	4,	949,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2280 = SHLL_PH_MM
  { 2281,	3,	1,	4,	787,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2281 = SHLL_QB
  { 2282,	3,	1,	4,	950,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2282 = SHLL_QB_MM
  { 2283,	3,	1,	4,	788,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2283 = SHLL_S_PH
  { 2284,	3,	1,	4,	951,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo330, -1 ,nullptr },  // Inst #2284 = SHLL_S_PH_MM
  { 2285,	3,	1,	4,	789,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo59, -1 ,nullptr },  // Inst #2285 = SHLL_S_W
  { 2286,	3,	1,	4,	952,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList32, OperandInfo59, -1 ,nullptr },  // Inst #2286 = SHLL_S_W_MM
  { 2287,	3,	1,	4,	790,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2287 = SHRAV_PH
  { 2288,	3,	1,	4,	953,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2288 = SHRAV_PH_MM
  { 2289,	3,	1,	4,	838,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2289 = SHRAV_QB
  { 2290,	3,	1,	4,	1001,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2290 = SHRAV_QB_MMR2
  { 2291,	3,	1,	4,	791,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2291 = SHRAV_R_PH
  { 2292,	3,	1,	4,	954,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2292 = SHRAV_R_PH_MM
  { 2293,	3,	1,	4,	839,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2293 = SHRAV_R_QB
  { 2294,	3,	1,	4,	1002,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2294 = SHRAV_R_QB_MMR2
  { 2295,	3,	1,	4,	792,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2295 = SHRAV_R_W
  { 2296,	3,	1,	4,	955,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2296 = SHRAV_R_W_MM
  { 2297,	3,	1,	4,	793,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2297 = SHRA_PH
  { 2298,	3,	1,	4,	956,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2298 = SHRA_PH_MM
  { 2299,	3,	1,	4,	836,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2299 = SHRA_QB
  { 2300,	3,	1,	4,	999,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2300 = SHRA_QB_MMR2
  { 2301,	3,	1,	4,	794,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2301 = SHRA_R_PH
  { 2302,	3,	1,	4,	957,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2302 = SHRA_R_PH_MM
  { 2303,	3,	1,	4,	837,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2303 = SHRA_R_QB
  { 2304,	3,	1,	4,	1000,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2304 = SHRA_R_QB_MMR2
  { 2305,	3,	1,	4,	795,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2305 = SHRA_R_W
  { 2306,	3,	1,	4,	958,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2306 = SHRA_R_W_MM
  { 2307,	3,	1,	4,	841,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2307 = SHRLV_PH
  { 2308,	3,	1,	4,	1004,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2308 = SHRLV_PH_MMR2
  { 2309,	3,	1,	4,	796,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2309 = SHRLV_QB
  { 2310,	3,	1,	4,	959,	0, 0x6ULL, nullptr, nullptr, OperandInfo329, -1 ,nullptr },  // Inst #2310 = SHRLV_QB_MM
  { 2311,	3,	1,	4,	840,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2311 = SHRL_PH
  { 2312,	3,	1,	4,	1003,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2312 = SHRL_PH_MMR2
  { 2313,	3,	1,	4,	797,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2313 = SHRL_QB
  { 2314,	3,	1,	4,	960,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo330, -1 ,nullptr },  // Inst #2314 = SHRL_QB_MM
  { 2315,	3,	0,	4,	297,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2315 = SH_MM
  { 2316,	3,	0,	4,	297,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2316 = SH_MMR6
  { 2317,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo172, -1 ,nullptr },  // Inst #2317 = SLDI_B
  { 2318,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo173, -1 ,nullptr },  // Inst #2318 = SLDI_D
  { 2319,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo174, -1 ,nullptr },  // Inst #2319 = SLDI_H
  { 2320,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo175, -1 ,nullptr },  // Inst #2320 = SLDI_W
  { 2321,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo331, -1 ,nullptr },  // Inst #2321 = SLD_B
  { 2322,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo332, -1 ,nullptr },  // Inst #2322 = SLD_D
  { 2323,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo333, -1 ,nullptr },  // Inst #2323 = SLD_H
  { 2324,	4,	1,	4,	500,	0, 0x6ULL, nullptr, nullptr, OperandInfo334, -1 ,nullptr },  // Inst #2324 = SLD_W
  { 2325,	3,	1,	4,	486,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2325 = SLL
  { 2326,	3,	1,	2,	299,	0, 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #2326 = SLL16_MM
  { 2327,	3,	1,	2,	299,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #2327 = SLL16_MMR6
  { 2328,	2,	1,	4,	299,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, nullptr, OperandInfo224, -1 ,nullptr },  // Inst #2328 = SLL64_32
  { 2329,	2,	1,	4,	299,	0|(1ULL<<MCID::MoveReg), 0x1ULL, nullptr, nullptr, OperandInfo103, -1 ,nullptr },  // Inst #2329 = SLL64_64
  { 2330,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2330 = SLLI_B
  { 2331,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2331 = SLLI_D
  { 2332,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2332 = SLLI_H
  { 2333,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2333 = SLLI_W
  { 2334,	3,	1,	4,	491,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2334 = SLLV
  { 2335,	3,	1,	4,	300,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2335 = SLLV_MM
  { 2336,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2336 = SLL_B
  { 2337,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2337 = SLL_D
  { 2338,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2338 = SLL_H
  { 2339,	3,	1,	4,	299,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2339 = SLL_MM
  { 2340,	3,	1,	4,	299,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2340 = SLL_MMR6
  { 2341,	3,	1,	4,	602,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2341 = SLL_W
  { 2342,	3,	1,	4,	485,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2342 = SLT
  { 2343,	3,	1,	4,	301,	0, 0x1ULL, nullptr, nullptr, OperandInfo335, -1 ,nullptr },  // Inst #2343 = SLT64
  { 2344,	3,	1,	4,	301,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2344 = SLT_MM
  { 2345,	3,	1,	4,	362,	0, 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2345 = SLTi
  { 2346,	3,	1,	4,	302,	0, 0x2ULL, nullptr, nullptr, OperandInfo336, -1 ,nullptr },  // Inst #2346 = SLTi64
  { 2347,	3,	1,	4,	302,	0, 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2347 = SLTi_MM
  { 2348,	3,	1,	4,	362,	0, 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2348 = SLTiu
  { 2349,	3,	1,	4,	302,	0, 0x2ULL, nullptr, nullptr, OperandInfo336, -1 ,nullptr },  // Inst #2349 = SLTiu64
  { 2350,	3,	1,	4,	302,	0, 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2350 = SLTiu_MM
  { 2351,	3,	1,	4,	485,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2351 = SLTu
  { 2352,	3,	1,	4,	301,	0, 0x1ULL, nullptr, nullptr, OperandInfo335, -1 ,nullptr },  // Inst #2352 = SLTu64
  { 2353,	3,	1,	4,	301,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2353 = SLTu_MM
  { 2354,	3,	1,	4,	295,	0, 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2354 = SNE
  { 2355,	3,	1,	4,	296,	0, 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #2355 = SNEi
  { 2356,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2356 = SPLATI_B
  { 2357,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2357 = SPLATI_D
  { 2358,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2358 = SPLATI_H
  { 2359,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2359 = SPLATI_W
  { 2360,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo337, -1 ,nullptr },  // Inst #2360 = SPLAT_B
  { 2361,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo338, -1 ,nullptr },  // Inst #2361 = SPLAT_D
  { 2362,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo339, -1 ,nullptr },  // Inst #2362 = SPLAT_H
  { 2363,	3,	1,	4,	525,	0, 0x6ULL, nullptr, nullptr, OperandInfo340, -1 ,nullptr },  // Inst #2363 = SPLAT_W
  { 2364,	3,	1,	4,	487,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2364 = SRA
  { 2365,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2365 = SRAI_B
  { 2366,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2366 = SRAI_D
  { 2367,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2367 = SRAI_H
  { 2368,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2368 = SRAI_W
  { 2369,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2369 = SRARI_B
  { 2370,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2370 = SRARI_D
  { 2371,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2371 = SRARI_H
  { 2372,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2372 = SRARI_W
  { 2373,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2373 = SRAR_B
  { 2374,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2374 = SRAR_D
  { 2375,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2375 = SRAR_H
  { 2376,	3,	1,	4,	600,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2376 = SRAR_W
  { 2377,	3,	1,	4,	492,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2377 = SRAV
  { 2378,	3,	1,	4,	304,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2378 = SRAV_MM
  { 2379,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2379 = SRA_B
  { 2380,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2380 = SRA_D
  { 2381,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2381 = SRA_H
  { 2382,	3,	1,	4,	303,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2382 = SRA_MM
  { 2383,	3,	1,	4,	598,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2383 = SRA_W
  { 2384,	3,	1,	4,	488,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2384 = SRL
  { 2385,	3,	1,	2,	305,	0, 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #2385 = SRL16_MM
  { 2386,	3,	1,	2,	305,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo148, -1 ,nullptr },  // Inst #2386 = SRL16_MMR6
  { 2387,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2387 = SRLI_B
  { 2388,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2388 = SRLI_D
  { 2389,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2389 = SRLI_H
  { 2390,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2390 = SRLI_W
  { 2391,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2391 = SRLRI_B
  { 2392,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2392 = SRLRI_D
  { 2393,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2393 = SRLRI_H
  { 2394,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2394 = SRLRI_W
  { 2395,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2395 = SRLR_B
  { 2396,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2396 = SRLR_D
  { 2397,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2397 = SRLR_H
  { 2398,	3,	1,	4,	601,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2398 = SRLR_W
  { 2399,	3,	1,	4,	493,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2399 = SRLV
  { 2400,	3,	1,	4,	306,	0, 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2400 = SRLV_MM
  { 2401,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2401 = SRL_B
  { 2402,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2402 = SRL_D
  { 2403,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2403 = SRL_H
  { 2404,	3,	1,	4,	305,	0, 0x1ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2404 = SRL_MM
  { 2405,	3,	1,	4,	599,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2405 = SRL_W
  { 2406,	0,	0,	4,	381,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2406 = SSNOP
  { 2407,	0,	0,	4,	307,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2407 = SSNOP_MM
  { 2408,	0,	0,	4,	307,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2408 = SSNOP_MMR6
  { 2409,	3,	0,	4,	676,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo267, -1 ,nullptr },  // Inst #2409 = ST_B
  { 2410,	3,	0,	4,	676,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo268, -1 ,nullptr },  // Inst #2410 = ST_D
  { 2411,	3,	0,	4,	676,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo269, -1 ,nullptr },  // Inst #2411 = ST_H
  { 2412,	3,	0,	4,	676,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo270, -1 ,nullptr },  // Inst #2412 = ST_W
  { 2413,	3,	1,	4,	363,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2413 = SUB
  { 2414,	3,	1,	4,	842,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2414 = SUBQH_PH
  { 2415,	3,	1,	4,	1005,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2415 = SUBQH_PH_MMR2
  { 2416,	3,	1,	4,	843,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2416 = SUBQH_R_PH
  { 2417,	3,	1,	4,	1006,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2417 = SUBQH_R_PH_MMR2
  { 2418,	3,	1,	4,	845,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2418 = SUBQH_R_W
  { 2419,	3,	1,	4,	1008,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2419 = SUBQH_R_W_MMR2
  { 2420,	3,	1,	4,	844,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2420 = SUBQH_W
  { 2421,	3,	1,	4,	1007,	0, 0x6ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2421 = SUBQH_W_MMR2
  { 2422,	3,	1,	4,	798,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2422 = SUBQ_PH
  { 2423,	3,	1,	4,	961,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2423 = SUBQ_PH_MM
  { 2424,	3,	1,	4,	799,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2424 = SUBQ_S_PH
  { 2425,	3,	1,	4,	962,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2425 = SUBQ_S_PH_MM
  { 2426,	3,	1,	4,	800,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #2426 = SUBQ_S_W
  { 2427,	3,	1,	4,	963,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo58, -1 ,nullptr },  // Inst #2427 = SUBQ_S_W_MM
  { 2428,	3,	1,	4,	586,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2428 = SUBSUS_U_B
  { 2429,	3,	1,	4,	586,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2429 = SUBSUS_U_D
  { 2430,	3,	1,	4,	586,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2430 = SUBSUS_U_H
  { 2431,	3,	1,	4,	586,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2431 = SUBSUS_U_W
  { 2432,	3,	1,	4,	587,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2432 = SUBSUU_S_B
  { 2433,	3,	1,	4,	587,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2433 = SUBSUU_S_D
  { 2434,	3,	1,	4,	587,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2434 = SUBSUU_S_H
  { 2435,	3,	1,	4,	587,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2435 = SUBSUU_S_W
  { 2436,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2436 = SUBS_S_B
  { 2437,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2437 = SUBS_S_D
  { 2438,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2438 = SUBS_S_H
  { 2439,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2439 = SUBS_S_W
  { 2440,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2440 = SUBS_U_B
  { 2441,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2441 = SUBS_U_D
  { 2442,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2442 = SUBS_U_H
  { 2443,	3,	1,	4,	585,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2443 = SUBS_U_W
  { 2444,	3,	1,	2,	309,	0, 0x0ULL, nullptr, nullptr, OperandInfo152, -1 ,nullptr },  // Inst #2444 = SUBU16_MM
  { 2445,	3,	1,	2,	309,	0, 0x0ULL, nullptr, nullptr, OperandInfo152, -1 ,nullptr },  // Inst #2445 = SUBU16_MMR6
  { 2446,	3,	1,	4,	848,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2446 = SUBUH_QB
  { 2447,	3,	1,	4,	1011,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2447 = SUBUH_QB_MMR2
  { 2448,	3,	1,	4,	849,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2448 = SUBUH_R_QB
  { 2449,	3,	1,	4,	1012,	0, 0x6ULL, nullptr, nullptr, OperandInfo150, -1 ,nullptr },  // Inst #2449 = SUBUH_R_QB_MMR2
  { 2450,	3,	1,	4,	309,	0|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2450 = SUBU_MMR6
  { 2451,	3,	1,	4,	846,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2451 = SUBU_PH
  { 2452,	3,	1,	4,	1009,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2452 = SUBU_PH_MMR2
  { 2453,	3,	1,	4,	801,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2453 = SUBU_QB
  { 2454,	3,	1,	4,	964,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2454 = SUBU_QB_MM
  { 2455,	3,	1,	4,	847,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2455 = SUBU_S_PH
  { 2456,	3,	1,	4,	1010,	0|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2456 = SUBU_S_PH_MMR2
  { 2457,	3,	1,	4,	802,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2457 = SUBU_S_QB
  { 2458,	3,	1,	4,	965,	0, 0x6ULL, nullptr, ImplicitList8, OperandInfo150, -1 ,nullptr },  // Inst #2458 = SUBU_S_QB_MM
  { 2459,	3,	1,	4,	588,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2459 = SUBVI_B
  { 2460,	3,	1,	4,	588,	0, 0x6ULL, nullptr, nullptr, OperandInfo154, -1 ,nullptr },  // Inst #2460 = SUBVI_D
  { 2461,	3,	1,	4,	588,	0, 0x6ULL, nullptr, nullptr, OperandInfo155, -1 ,nullptr },  // Inst #2461 = SUBVI_H
  { 2462,	3,	1,	4,	588,	0, 0x6ULL, nullptr, nullptr, OperandInfo156, -1 ,nullptr },  // Inst #2462 = SUBVI_W
  { 2463,	3,	1,	4,	589,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2463 = SUBV_B
  { 2464,	3,	1,	4,	589,	0, 0x6ULL, nullptr, nullptr, OperandInfo32, -1 ,nullptr },  // Inst #2464 = SUBV_D
  { 2465,	3,	1,	4,	589,	0, 0x6ULL, nullptr, nullptr, OperandInfo33, -1 ,nullptr },  // Inst #2465 = SUBV_H
  { 2466,	3,	1,	4,	589,	0, 0x6ULL, nullptr, nullptr, OperandInfo34, -1 ,nullptr },  // Inst #2466 = SUBV_W
  { 2467,	3,	1,	4,	308,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2467 = SUB_MM
  { 2468,	3,	1,	4,	308,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2468 = SUB_MMR6
  { 2469,	3,	1,	4,	364,	0|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2469 = SUBu
  { 2470,	3,	1,	4,	309,	0|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2470 = SUBu_MM
  { 2471,	3,	0,	4,	673,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo265, -1 ,nullptr },  // Inst #2471 = SUXC1
  { 2472,	3,	0,	4,	310,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #2472 = SUXC164
  { 2473,	3,	0,	4,	310,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x5ULL, nullptr, nullptr, OperandInfo266, -1 ,nullptr },  // Inst #2473 = SUXC1_MM
  { 2474,	3,	0,	4,	435,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2474 = SW
  { 2475,	3,	0,	2,	311,	0|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2475 = SW16_MM
  { 2476,	3,	0,	2,	311,	0|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo324, -1 ,nullptr },  // Inst #2476 = SW16_MMR6
  { 2477,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2477 = SW64
  { 2478,	3,	0,	4,	674,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo271, -1 ,nullptr },  // Inst #2478 = SWC1
  { 2479,	3,	0,	4,	312,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo271, -1 ,nullptr },  // Inst #2479 = SWC1_MM
  { 2480,	3,	0,	4,	436,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #2480 = SWC2
  { 2481,	3,	0,	4,	313,	0|(1ULL<<MCID::MayStore), 0x6ULL, nullptr, nullptr, OperandInfo258, -1 ,nullptr },  // Inst #2481 = SWC2_MMR6
  { 2482,	3,	0,	4,	313,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo257, -1 ,nullptr },  // Inst #2482 = SWC2_R6
  { 2483,	3,	0,	4,	437,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo259, -1 ,nullptr },  // Inst #2483 = SWC3
  { 2484,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo272, -1 ,nullptr },  // Inst #2484 = SWDSP
  { 2485,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo272, -1 ,nullptr },  // Inst #2485 = SWDSP_MM
  { 2486,	3,	0,	4,	443,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2486 = SWE
  { 2487,	3,	0,	4,	315,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2487 = SWE_MM
  { 2488,	3,	0,	4,	445,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2488 = SWL
  { 2489,	3,	0,	4,	316,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2489 = SWL64
  { 2490,	3,	0,	4,	447,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2490 = SWLE
  { 2491,	3,	0,	4,	317,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2491 = SWLE_MM
  { 2492,	3,	0,	4,	316,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2492 = SWL_MM
  { 2493,	3,	0,	2,	318,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo275, -1 ,nullptr },  // Inst #2493 = SWM16_MM
  { 2494,	3,	0,	2,	318,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo275, -1 ,nullptr },  // Inst #2494 = SWM16_MMR6
  { 2495,	3,	0,	4,	318,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo91, -1 ,nullptr },  // Inst #2495 = SWM32_MM
  { 2496,	4,	0,	4,	319,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo276, -1 ,nullptr },  // Inst #2496 = SWP_MM
  { 2497,	3,	0,	4,	446,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2497 = SWR
  { 2498,	3,	0,	4,	320,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo94, -1 ,nullptr },  // Inst #2498 = SWR64
  { 2499,	3,	0,	4,	448,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2499 = SWRE
  { 2500,	3,	0,	4,	321,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2500 = SWRE_MM
  { 2501,	3,	0,	4,	320,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2501 = SWR_MM
  { 2502,	3,	0,	2,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo277, -1 ,nullptr },  // Inst #2502 = SWSP_MM
  { 2503,	3,	0,	2,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo277, -1 ,nullptr },  // Inst #2503 = SWSP_MMR6
  { 2504,	3,	0,	4,	675,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo278, -1 ,nullptr },  // Inst #2504 = SWXC1
  { 2505,	3,	0,	4,	322,	0|(1ULL<<MCID::MayStore), 0x5ULL, nullptr, nullptr, OperandInfo278, -1 ,nullptr },  // Inst #2505 = SWXC1_MM
  { 2506,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2506 = SW_MM
  { 2507,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x2ULL, nullptr, nullptr, OperandInfo82, -1 ,nullptr },  // Inst #2507 = SW_MMR6
  { 2508,	1,	0,	4,	453,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2508 = SYNC
  { 2509,	2,	0,	4,	454,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo341, -1 ,nullptr },  // Inst #2509 = SYNCI
  { 2510,	2,	0,	4,	324,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo341, -1 ,nullptr },  // Inst #2510 = SYNCI_MM
  { 2511,	2,	0,	4,	324,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo341, -1 ,nullptr },  // Inst #2511 = SYNCI_MMR6
  { 2512,	1,	0,	4,	323,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2512 = SYNC_MM
  { 2513,	1,	0,	4,	323,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2513 = SYNC_MMR6
  { 2514,	1,	0,	4,	382,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2514 = SYSCALL
  { 2515,	1,	0,	4,	325,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2515 = SYSCALL_MM
  { 2516,	0,	0,	2,	326,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, nullptr, -1 ,nullptr },  // Inst #2516 = Save16
  { 2517,	0,	0,	2,	326,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::Variadic)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, ImplicitList1, ImplicitList1, nullptr, -1 ,nullptr },  // Inst #2517 = SaveX16
  { 2518,	3,	0,	4,	275,	0|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #2518 = SbRxRyOffMemX16
  { 2519,	2,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo342, -1 ,nullptr },  // Inst #2519 = SebRx16
  { 2520,	2,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo342, -1 ,nullptr },  // Inst #2520 = SehRx16
  { 2521,	3,	0,	4,	297,	0|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #2521 = ShRxRyOffMemX16
  { 2522,	3,	1,	4,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo145, -1 ,nullptr },  // Inst #2522 = SllX16
  { 2523,	3,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #2523 = SllvRxRy16
  { 2524,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo111, -1 ,nullptr },  // Inst #2524 = SltRxRy16
  { 2525,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #2525 = SltiRxImm16
  { 2526,	2,	0,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #2526 = SltiRxImmX16
  { 2527,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #2527 = SltiuRxImm16
  { 2528,	2,	0,	4,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo160, -1 ,nullptr },  // Inst #2528 = SltiuRxImmX16
  { 2529,	2,	0,	2,	38,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, ImplicitList7, OperandInfo111, -1 ,nullptr },  // Inst #2529 = SltuRxRy16
  { 2530,	3,	1,	4,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo145, -1 ,nullptr },  // Inst #2530 = SraX16
  { 2531,	3,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #2531 = SravRxRy16
  { 2532,	3,	1,	4,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo145, -1 ,nullptr },  // Inst #2532 = SrlX16
  { 2533,	3,	1,	2,	38,	0, 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #2533 = SrlvRxRy16
  { 2534,	3,	1,	2,	38,	0|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo112, -1 ,nullptr },  // Inst #2534 = SubuRxRyRz16
  { 2535,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo279, -1 ,nullptr },  // Inst #2535 = SwRxRyOffMemX16
  { 2536,	3,	0,	4,	311,	0|(1ULL<<MCID::MayStore), 0x0ULL, nullptr, nullptr, OperandInfo162, -1 ,nullptr },  // Inst #2536 = SwRxSpImmX16
  { 2537,	3,	0,	4,	383,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2537 = TEQ
  { 2538,	2,	0,	4,	384,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2538 = TEQI
  { 2539,	2,	0,	4,	328,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2539 = TEQI_MM
  { 2540,	3,	0,	4,	327,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2540 = TEQ_MM
  { 2541,	3,	0,	4,	385,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2541 = TGE
  { 2542,	2,	0,	4,	386,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2542 = TGEI
  { 2543,	2,	0,	4,	387,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2543 = TGEIU
  { 2544,	2,	0,	4,	331,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2544 = TGEIU_MM
  { 2545,	2,	0,	4,	330,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2545 = TGEI_MM
  { 2546,	3,	0,	4,	388,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2546 = TGEU
  { 2547,	3,	0,	4,	332,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2547 = TGEU_MM
  { 2548,	3,	0,	4,	329,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2548 = TGE_MM
  { 2549,	0,	0,	4,	333,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2549 = TLBGINV
  { 2550,	0,	0,	4,	334,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2550 = TLBGINVF
  { 2551,	0,	0,	4,	334,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2551 = TLBGINVF_MM
  { 2552,	0,	0,	4,	333,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2552 = TLBGINV_MM
  { 2553,	0,	0,	4,	335,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2553 = TLBGP
  { 2554,	0,	0,	4,	335,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2554 = TLBGP_MM
  { 2555,	0,	0,	4,	336,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2555 = TLBGR
  { 2556,	0,	0,	4,	336,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2556 = TLBGR_MM
  { 2557,	0,	0,	4,	337,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2557 = TLBGWI
  { 2558,	0,	0,	4,	337,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2558 = TLBGWI_MM
  { 2559,	0,	0,	4,	338,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2559 = TLBGWR
  { 2560,	0,	0,	4,	338,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2560 = TLBGWR_MM
  { 2561,	0,	0,	4,	402,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2561 = TLBINV
  { 2562,	0,	0,	4,	403,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2562 = TLBINVF
  { 2563,	0,	0,	4,	340,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2563 = TLBINVF_MMR6
  { 2564,	0,	0,	4,	339,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2564 = TLBINV_MMR6
  { 2565,	0,	0,	4,	404,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2565 = TLBP
  { 2566,	0,	0,	4,	341,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2566 = TLBP_MM
  { 2567,	0,	0,	4,	405,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2567 = TLBR
  { 2568,	0,	0,	4,	342,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2568 = TLBR_MM
  { 2569,	0,	0,	4,	406,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2569 = TLBWI
  { 2570,	0,	0,	4,	343,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2570 = TLBWI_MM
  { 2571,	0,	0,	4,	407,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2571 = TLBWR
  { 2572,	0,	0,	4,	344,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2572 = TLBWR_MM
  { 2573,	3,	0,	4,	389,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2573 = TLT
  { 2574,	2,	0,	4,	390,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2574 = TLTI
  { 2575,	2,	0,	4,	347,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2575 = TLTIU_MM
  { 2576,	2,	0,	4,	346,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2576 = TLTI_MM
  { 2577,	3,	0,	4,	391,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2577 = TLTU
  { 2578,	3,	0,	4,	348,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2578 = TLTU_MM
  { 2579,	3,	0,	4,	345,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2579 = TLT_MM
  { 2580,	3,	0,	4,	392,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2580 = TNE
  { 2581,	2,	0,	4,	393,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2581 = TNEI
  { 2582,	2,	0,	4,	350,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2582 = TNEI_MM
  { 2583,	3,	0,	4,	349,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x12ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2583 = TNE_MM
  { 2584,	2,	1,	4,	611,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2584 = TRUNC_L_D64
  { 2585,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo183, -1 ,nullptr },  // Inst #2585 = TRUNC_L_D_MMR6
  { 2586,	2,	1,	4,	611,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #2586 = TRUNC_L_S
  { 2587,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo184, -1 ,nullptr },  // Inst #2587 = TRUNC_L_S_MMR6
  { 2588,	2,	1,	4,	611,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #2588 = TRUNC_W_D32
  { 2589,	2,	1,	4,	611,	0, 0x4ULL, nullptr, nullptr, OperandInfo186, -1 ,nullptr },  // Inst #2589 = TRUNC_W_D64
  { 2590,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #2590 = TRUNC_W_D_MMR6
  { 2591,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo185, -1 ,nullptr },  // Inst #2591 = TRUNC_W_MM
  { 2592,	2,	1,	4,	611,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2592 = TRUNC_W_S
  { 2593,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2593 = TRUNC_W_S_MM
  { 2594,	2,	1,	4,	351,	0, 0x4ULL, nullptr, nullptr, OperandInfo187, -1 ,nullptr },  // Inst #2594 = TRUNC_W_S_MMR6
  { 2595,	2,	0,	4,	395,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2595 = TTLTIU
  { 2596,	2,	0,	4,	466,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2596 = UDIV
  { 2597,	2,	0,	4,	23,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList6, OperandInfo31, -1 ,nullptr },  // Inst #2597 = UDIV_MM
  { 2598,	3,	1,	4,	108,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList33, OperandInfo57, -1 ,nullptr },  // Inst #2598 = V3MULU
  { 2599,	3,	1,	4,	108,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList25, OperandInfo57, -1 ,nullptr },  // Inst #2599 = VMM0
  { 2600,	3,	1,	4,	108,	0|(1ULL<<MCID::Rematerializable)|(1ULL<<MCID::UnmodeledSideEffects), 0x1ULL, nullptr, ImplicitList34, OperandInfo57, -1 ,nullptr },  // Inst #2600 = VMULU
  { 2601,	4,	1,	4,	496,	0, 0x6ULL, nullptr, nullptr, OperandInfo176, -1 ,nullptr },  // Inst #2601 = VSHF_B
  { 2602,	4,	1,	4,	496,	0, 0x6ULL, nullptr, nullptr, OperandInfo45, -1 ,nullptr },  // Inst #2602 = VSHF_D
  { 2603,	4,	1,	4,	496,	0, 0x6ULL, nullptr, nullptr, OperandInfo47, -1 ,nullptr },  // Inst #2603 = VSHF_H
  { 2604,	4,	1,	4,	496,	0, 0x6ULL, nullptr, nullptr, OperandInfo46, -1 ,nullptr },  // Inst #2604 = VSHF_W
  { 2605,	0,	0,	4,	396,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x16ULL, nullptr, nullptr, nullptr, -1 ,nullptr },  // Inst #2605 = WAIT
  { 2606,	1,	0,	4,	352,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2606 = WAIT_MM
  { 2607,	1,	0,	4,	352,	0|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo2, -1 ,nullptr },  // Inst #2607 = WAIT_MMR6
  { 2608,	2,	0,	4,	803,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2608 = WRDSP
  { 2609,	2,	0,	4,	966,	0|(1ULL<<MCID::MoveReg)|(1ULL<<MCID::MayLoad)|(1ULL<<MCID::MayStore)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo95, -1 ,nullptr },  // Inst #2609 = WRDSP_MM
  { 2610,	2,	1,	4,	353,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2610 = WRPGPR_MMR6
  { 2611,	2,	1,	4,	462,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2611 = WSBH
  { 2612,	2,	1,	4,	354,	0, 0x1ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2612 = WSBH_MM
  { 2613,	2,	1,	4,	354,	0, 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2613 = WSBH_MMR6
  { 2614,	3,	1,	4,	365,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2614 = XOR
  { 2615,	3,	1,	2,	355,	0|(1ULL<<MCID::Commutable), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #2615 = XOR16_MM
  { 2616,	3,	1,	2,	355,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::UnmodeledSideEffects), 0x0ULL, nullptr, nullptr, OperandInfo158, -1 ,nullptr },  // Inst #2616 = XOR16_MMR6
  { 2617,	3,	1,	4,	355,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo57, -1 ,nullptr },  // Inst #2617 = XOR64
  { 2618,	3,	1,	4,	529,	0, 0x6ULL, nullptr, nullptr, OperandInfo153, -1 ,nullptr },  // Inst #2618 = XORI_B
  { 2619,	3,	1,	4,	356,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2619 = XORI_MMR6
  { 2620,	3,	1,	4,	355,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2620 = XOR_MM
  { 2621,	3,	1,	4,	355,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x1ULL, nullptr, nullptr, OperandInfo58, -1 ,nullptr },  // Inst #2621 = XOR_MMR6
  { 2622,	3,	1,	4,	528,	0, 0x6ULL, nullptr, nullptr, OperandInfo151, -1 ,nullptr },  // Inst #2622 = XOR_V
  { 2623,	3,	1,	4,	489,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2623 = XORi
  { 2624,	3,	1,	4,	355,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo56, -1 ,nullptr },  // Inst #2624 = XORi64
  { 2625,	3,	1,	4,	356,	0|(1ULL<<MCID::Rematerializable), 0x2ULL, nullptr, nullptr, OperandInfo59, -1 ,nullptr },  // Inst #2625 = XORi_MM
  { 2626,	3,	1,	2,	38,	0|(1ULL<<MCID::Commutable)|(1ULL<<MCID::Rematerializable), 0x0ULL, nullptr, nullptr, OperandInfo163, -1 ,nullptr },  // Inst #2626 = XorRxRxRy16
  { 2627,	2,	1,	4,	357,	0|(1ULL<<MCID::NotDuplicable)|(1ULL<<MCID::UnmodeledSideEffects), 0x6ULL, nullptr, nullptr, OperandInfo31, -1 ,nullptr },  // Inst #2627 = YIELD
};

extern const char MipsInstrNameData[] = {
  /* 0 */ 'D', 'M', 'F', 'C', '0', 0,
  /* 6 */ 'D', 'M', 'F', 'G', 'C', '0', 0,
  /* 13 */ 'M', 'F', 'H', 'G', 'C', '0', 0,
  /* 20 */ 'M', 'T', 'H', 'G', 'C', '0', 0,
  /* 27 */ 'D', 'M', 'T', 'G', 'C', '0', 0,
  /* 34 */ 'M', 'F', 'T', 'C', '0', 0,
  /* 40 */ 'D', 'M', 'T', 'C', '0', 0,
  /* 46 */ 'M', 'T', 'T', 'C', '0', 0,
  /* 52 */ 'V', 'M', 'M', '0', 0,
  /* 57 */ 'M', 'T', 'M', '0', 0,
  /* 62 */ 'M', 'T', 'P', '0', 0,
  /* 67 */ 'B', 'B', 'I', 'T', '0', 0,
  /* 73 */ 'L', 'D', 'C', '1', 0,
  /* 78 */ 'S', 'D', 'C', '1', 0,
  /* 83 */ 'C', 'F', 'C', '1', 0,
  /* 88 */ 'D', 'M', 'F', 'C', '1', 0,
  /* 94 */ 'M', 'F', 'T', 'H', 'C', '1', 0,
  /* 101 */ 'M', 'T', 'T', 'H', 'C', '1', 0,
  /* 108 */ 'C', 'T', 'C', '1', 0,
  /* 113 */ 'C', 'F', 'T', 'C', '1', 0,
  /* 119 */ 'M', 'F', 'T', 'C', '1', 0,
  /* 125 */ 'D', 'M', 'T', 'C', '1', 0,
  /* 131 */ 'C', 'T', 'T', 'C', '1', 0,
  /* 137 */ 'M', 'T', 'T', 'C', '1', 0,
  /* 143 */ 'L', 'W', 'C', '1', 0,
  /* 148 */ 'S', 'W', 'C', '1', 0,
  /* 153 */ 'L', 'D', 'X', 'C', '1', 0,
  /* 159 */ 'S', 'D', 'X', 'C', '1', 0,
  /* 165 */ 'L', 'U', 'X', 'C', '1', 0,
  /* 171 */ 'S', 'U', 'X', 'C', '1', 0,
  /* 177 */ 'L', 'W', 'X', 'C', '1', 0,
  /* 183 */ 'S', 'W', 'X', 'C', '1', 0,
  /* 189 */ 'M', 'T', 'M', '1', 0,
  /* 194 */ 'M', 'T', 'P', '1', 0,
  /* 199 */ 'B', 'B', 'I', 'T', '1', 0,
  /* 205 */ 'B', 'B', 'I', 'T', '0', '3', '2', 0,
  /* 213 */ 'B', 'B', 'I', 'T', '1', '3', '2', 0,
  /* 221 */ 'D', 'S', 'R', 'A', '3', '2', 0,
  /* 228 */ 'M', 'F', 'H', 'C', '1', '_', 'D', '3', '2', 0,
  /* 238 */ 'M', 'T', 'H', 'C', '1', '_', 'D', '3', '2', 0,
  /* 248 */ 'F', 'S', 'U', 'B', '_', 'D', '3', '2', 0,
  /* 257 */ 'N', 'M', 'S', 'U', 'B', '_', 'D', '3', '2', 0,
  /* 267 */ 'F', 'A', 'D', 'D', '_', 'D', '3', '2', 0,
  /* 276 */ 'N', 'M', 'A', 'D', 'D', '_', 'D', '3', '2', 0,
  /* 286 */ 'C', '_', 'N', 'G', 'E', '_', 'D', '3', '2', 0,
  /* 296 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'D', '3', '2', 0,
  /* 307 */ 'C', '_', 'O', 'L', 'E', '_', 'D', '3', '2', 0,
  /* 317 */ 'C', '_', 'U', 'L', 'E', '_', 'D', '3', '2', 0,
  /* 327 */ 'C', '_', 'L', 'E', '_', 'D', '3', '2', 0,
  /* 336 */ 'C', '_', 'S', 'F', '_', 'D', '3', '2', 0,
  /* 345 */ 'M', 'O', 'V', 'F', '_', 'D', '3', '2', 0,
  /* 354 */ 'C', '_', 'F', '_', 'D', '3', '2', 0,
  /* 362 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'F', '_', 'D', '3', '2', 0,
  /* 383 */ 'F', 'N', 'E', 'G', '_', 'D', '3', '2', 0,
  /* 392 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'D', '3', '2', 0,
  /* 403 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'D', '3', '2', 0,
  /* 414 */ 'C', '_', 'N', 'G', 'L', '_', 'D', '3', '2', 0,
  /* 424 */ 'F', 'M', 'U', 'L', '_', 'D', '3', '2', 0,
  /* 433 */ 'C', '_', 'U', 'N', '_', 'D', '3', '2', 0,
  /* 442 */ 'R', 'E', 'C', 'I', 'P', '_', 'D', '3', '2', 0,
  /* 452 */ 'F', 'C', 'M', 'P', '_', 'D', '3', '2', 0,
  /* 461 */ 'C', '_', 'S', 'E', 'Q', '_', 'D', '3', '2', 0,
  /* 471 */ 'C', '_', 'U', 'E', 'Q', '_', 'D', '3', '2', 0,
  /* 481 */ 'C', '_', 'E', 'Q', '_', 'D', '3', '2', 0,
  /* 490 */ 'F', 'A', 'B', 'S', '_', 'D', '3', '2', 0,
  /* 499 */ 'C', 'V', 'T', '_', 'S', '_', 'D', '3', '2', 0,
  /* 509 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', '_', 'D', '3', '2', 0,
  /* 526 */ 'C', '_', 'N', 'G', 'T', '_', 'D', '3', '2', 0,
  /* 536 */ 'C', '_', 'O', 'L', 'T', '_', 'D', '3', '2', 0,
  /* 546 */ 'C', '_', 'U', 'L', 'T', '_', 'D', '3', '2', 0,
  /* 556 */ 'C', '_', 'L', 'T', '_', 'D', '3', '2', 0,
  /* 565 */ 'F', 'S', 'Q', 'R', 'T', '_', 'D', '3', '2', 0,
  /* 575 */ 'R', 'S', 'Q', 'R', 'T', '_', 'D', '3', '2', 0,
  /* 585 */ 'M', 'O', 'V', 'T', '_', 'D', '3', '2', 0,
  /* 594 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'T', '_', 'D', '3', '2', 0,
  /* 615 */ 'F', 'D', 'I', 'V', '_', 'D', '3', '2', 0,
  /* 624 */ 'F', 'M', 'O', 'V', '_', 'D', '3', '2', 0,
  /* 633 */ 'P', 's', 'e', 'u', 'd', 'o', 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'D', '3', '2', 0,
  /* 651 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'D', '3', '2', 0,
  /* 663 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'D', '3', '2', 0,
  /* 674 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'D', '3', '2', 0,
  /* 686 */ 'C', 'V', 'T', '_', 'W', '_', 'D', '3', '2', 0,
  /* 696 */ 'B', 'P', 'O', 'S', 'G', 'E', '3', '2', 0,
  /* 705 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '3', '2', 0,
  /* 725 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '3', '2', 0,
  /* 745 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '3', '2', 0,
  /* 766 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '3', '2', 0,
  /* 786 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '3', '2', 0,
  /* 802 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '3', '2', 0,
  /* 822 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '3', '2', 0,
  /* 842 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '3', '2', 0,
  /* 861 */ 'D', 'S', 'L', 'L', '3', '2', 0,
  /* 868 */ 'D', 'S', 'R', 'L', '3', '2', 0,
  /* 875 */ 'D', 'R', 'O', 'T', 'R', '3', '2', 0,
  /* 883 */ 'C', 'I', 'N', 'S', '3', '2', 0,
  /* 890 */ 'E', 'X', 'T', 'S', '3', '2', 0,
  /* 897 */ 'F', 'C', 'M', 'P', '_', 'S', '3', '2', 0,
  /* 906 */ 'D', 'S', 'L', 'L', '6', '4', '_', '3', '2', 0,
  /* 916 */ 'C', 'I', 'N', 'S', '6', '4', '_', '3', '2', 0,
  /* 926 */ 'D', 'E', 'X', 'T', '6', '4', '_', '3', '2', 0,
  /* 936 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', 'D', 'o', 'u', 'b', 'l', 'e', 'F', 'G', 'R', '_', '3', '2', 0,
  /* 956 */ 'L', 'o', 'a', 'd', 'A', 'd', 'd', 'r', 'R', 'e', 'g', '3', '2', 0,
  /* 970 */ 'C', 'I', 'N', 'S', '_', 'i', '3', '2', 0,
  /* 979 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', '3', '2', 0,
  /* 989 */ 'L', 'o', 'a', 'd', 'A', 'd', 'd', 'r', 'I', 'm', 'm', '3', '2', 0,
  /* 1003 */ 'M', 'I', 'P', 'S', 'e', 'h', '_', 'r', 'e', 't', 'u', 'r', 'n', '3', '2', 0,
  /* 1019 */ 'L', 'w', 'C', 'o', 'n', 's', 't', 'a', 'n', 't', '3', '2', 0,
  /* 1032 */ 'L', 'D', 'C', '2', 0,
  /* 1037 */ 'S', 'D', 'C', '2', 0,
  /* 1042 */ 'D', 'M', 'F', 'C', '2', 0,
  /* 1048 */ 'D', 'M', 'T', 'C', '2', 0,
  /* 1054 */ 'L', 'W', 'C', '2', 0,
  /* 1059 */ 'S', 'W', 'C', '2', 0,
  /* 1064 */ 'G', '_', 'F', 'L', 'O', 'G', '2', 0,
  /* 1072 */ 'M', 'T', 'M', '2', 0,
  /* 1077 */ 'M', 'T', 'P', '2', 0,
  /* 1082 */ 'G', '_', 'F', 'E', 'X', 'P', '2', 0,
  /* 1090 */ 'S', 'H', 'R', 'A', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1103 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'L', 'E', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1121 */ 'S', 'U', 'B', 'U', 'H', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1135 */ 'A', 'D', 'D', 'U', 'H', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1149 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'E', 'Q', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1167 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1182 */ 'S', 'U', 'B', 'U', 'H', '_', 'R', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1198 */ 'A', 'D', 'D', 'U', 'H', '_', 'R', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1214 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1230 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1245 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'L', 'T', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1263 */ 'S', 'H', 'R', 'A', 'V', '_', 'Q', 'B', '_', 'M', 'M', 'R', '2', 0,
  /* 1277 */ 'P', 'R', 'E', 'P', 'E', 'N', 'D', '_', 'M', 'M', 'R', '2', 0,
  /* 1290 */ 'A', 'P', 'P', 'E', 'N', 'D', '_', 'M', 'M', 'R', '2', 0,
  /* 1302 */ 'P', 'R', 'E', 'C', 'R', '_', 'Q', 'B', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1319 */ 'S', 'U', 'B', 'Q', 'H', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1333 */ 'A', 'D', 'D', 'Q', 'H', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1347 */ 'S', 'H', 'R', 'L', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1360 */ 'M', 'U', 'L', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1372 */ 'S', 'U', 'B', 'Q', 'H', '_', 'R', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1388 */ 'A', 'D', 'D', 'Q', 'H', '_', 'R', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1404 */ 'M', 'U', 'L', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1418 */ 'M', 'U', 'L', 'Q', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1433 */ 'S', 'U', 'B', 'U', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1448 */ 'A', 'D', 'D', 'U', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1463 */ 'S', 'U', 'B', 'U', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1476 */ 'A', 'D', 'D', 'U', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1489 */ 'S', 'H', 'R', 'L', 'V', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1503 */ 'D', 'P', 'A', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1517 */ 'M', 'U', 'L', 'S', 'A', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1533 */ 'D', 'P', 'A', 'Q', 'X', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1552 */ 'D', 'P', 'S', 'Q', 'X', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1571 */ 'D', 'P', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1585 */ 'D', 'P', 'A', 'Q', 'X', '_', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1603 */ 'D', 'P', 'S', 'Q', 'X', '_', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1621 */ 'D', 'P', 'A', 'X', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1636 */ 'D', 'P', 'S', 'X', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 'R', '2', 0,
  /* 1651 */ 'B', 'A', 'L', 'I', 'G', 'N', '_', 'M', 'M', 'R', '2', 0,
  /* 1663 */ 'P', 'R', 'E', 'C', 'R', '_', 'S', 'R', 'A', '_', 'P', 'H', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1683 */ 'P', 'R', 'E', 'C', 'R', '_', 'S', 'R', 'A', '_', 'R', '_', 'P', 'H', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1705 */ 'S', 'U', 'B', 'Q', 'H', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1718 */ 'A', 'D', 'D', 'Q', 'H', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1731 */ 'S', 'U', 'B', 'Q', 'H', '_', 'R', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1746 */ 'A', 'D', 'D', 'Q', 'H', '_', 'R', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1761 */ 'M', 'U', 'L', 'Q', '_', 'R', 'S', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1776 */ 'M', 'U', 'L', 'Q', '_', 'S', '_', 'W', '_', 'M', 'M', 'R', '2', 0,
  /* 1790 */ 'L', 'D', 'C', '3', 0,
  /* 1795 */ 'S', 'D', 'C', '3', 0,
  /* 1800 */ 'L', 'W', 'C', '3', 0,
  /* 1805 */ 'S', 'W', 'C', '3', 0,
  /* 1810 */ 'B', 'P', 'O', 'S', 'G', 'E', '3', '2', 'C', '_', 'M', 'M', 'R', '3', 0,
  /* 1825 */ 'L', 'D', 'C', '1', '6', '4', 0,
  /* 1832 */ 'S', 'D', 'C', '1', '6', '4', 0,
  /* 1839 */ 'L', 'D', 'X', 'C', '1', '6', '4', 0,
  /* 1847 */ 'S', 'D', 'X', 'C', '1', '6', '4', 0,
  /* 1855 */ 'L', 'U', 'X', 'C', '1', '6', '4', 0,
  /* 1863 */ 'S', 'U', 'X', 'C', '1', '6', '4', 0,
  /* 1871 */ 'S', 'E', 'B', '6', '4', 0,
  /* 1877 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', 'H', 'B', '6', '4', 0,
  /* 1893 */ 'J', 'R', '_', 'H', 'B', '6', '4', 0,
  /* 1901 */ 'J', 'A', 'L', 'R', '_', 'H', 'B', '6', '4', 0,
  /* 1911 */ 'L', 'B', '6', '4', 0,
  /* 1916 */ 'S', 'B', '6', '4', 0,
  /* 1921 */ 'L', 'O', 'A', 'D', '_', 'A', 'C', 'C', '6', '4', 0,
  /* 1932 */ 'S', 'T', 'O', 'R', 'E', '_', 'A', 'C', 'C', '6', '4', 0,
  /* 1944 */ 'B', 'G', 'E', 'C', '6', '4', 0,
  /* 1951 */ 'B', 'N', 'E', 'C', '6', '4', 0,
  /* 1958 */ 'J', 'I', 'C', '6', '4', 0,
  /* 1964 */ 'J', 'I', 'A', 'L', 'C', '6', '4', 0,
  /* 1972 */ 'B', 'E', 'Q', 'C', '6', '4', 0,
  /* 1979 */ 'S', 'C', '6', '4', 0,
  /* 1984 */ 'B', 'L', 'T', 'C', '6', '4', 0,
  /* 1991 */ 'B', 'G', 'E', 'U', 'C', '6', '4', 0,
  /* 1999 */ 'B', 'L', 'T', 'U', 'C', '6', '4', 0,
  /* 2007 */ 'B', 'G', 'E', 'Z', 'C', '6', '4', 0,
  /* 2015 */ 'B', 'L', 'E', 'Z', 'C', '6', '4', 0,
  /* 2023 */ 'B', 'N', 'E', 'Z', 'C', '6', '4', 0,
  /* 2031 */ 'B', 'E', 'Q', 'Z', 'C', '6', '4', 0,
  /* 2039 */ 'B', 'G', 'T', 'Z', 'C', '6', '4', 0,
  /* 2047 */ 'B', 'L', 'T', 'Z', 'C', '6', '4', 0,
  /* 2055 */ 'A', 'N', 'D', '6', '4', 0,
  /* 2061 */ 'M', 'F', 'C', '1', '_', 'D', '6', '4', 0,
  /* 2070 */ 'M', 'F', 'H', 'C', '1', '_', 'D', '6', '4', 0,
  /* 2080 */ 'M', 'T', 'H', 'C', '1', '_', 'D', '6', '4', 0,
  /* 2090 */ 'M', 'T', 'C', '1', '_', 'D', '6', '4', 0,
  /* 2099 */ 'M', 'O', 'V', 'N', '_', 'I', '6', '4', '_', 'D', '6', '4', 0,
  /* 2112 */ 'M', 'O', 'V', 'Z', '_', 'I', '6', '4', '_', 'D', '6', '4', 0,
  /* 2125 */ 'F', 'S', 'U', 'B', '_', 'D', '6', '4', 0,
  /* 2134 */ 'N', 'M', 'S', 'U', 'B', '_', 'D', '6', '4', 0,
  /* 2144 */ 'F', 'A', 'D', 'D', '_', 'D', '6', '4', 0,
  /* 2153 */ 'N', 'M', 'A', 'D', 'D', '_', 'D', '6', '4', 0,
  /* 2163 */ 'C', '_', 'N', 'G', 'E', '_', 'D', '6', '4', 0,
  /* 2173 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'D', '6', '4', 0,
  /* 2184 */ 'C', '_', 'O', 'L', 'E', '_', 'D', '6', '4', 0,
  /* 2194 */ 'C', '_', 'U', 'L', 'E', '_', 'D', '6', '4', 0,
  /* 2204 */ 'C', '_', 'L', 'E', '_', 'D', '6', '4', 0,
  /* 2213 */ 'C', '_', 'S', 'F', '_', 'D', '6', '4', 0,
  /* 2222 */ 'M', 'O', 'V', 'F', '_', 'D', '6', '4', 0,
  /* 2231 */ 'C', '_', 'F', '_', 'D', '6', '4', 0,
  /* 2239 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'F', '_', 'D', '6', '4', 0,
  /* 2260 */ 'F', 'N', 'E', 'G', '_', 'D', '6', '4', 0,
  /* 2269 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'D', '6', '4', 0,
  /* 2280 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'D', '6', '4', 0,
  /* 2291 */ 'C', '_', 'N', 'G', 'L', '_', 'D', '6', '4', 0,
  /* 2301 */ 'F', 'M', 'U', 'L', '_', 'D', '6', '4', 0,
  /* 2310 */ 'T', 'R', 'U', 'N', 'C', '_', 'L', '_', 'D', '6', '4', 0,
  /* 2322 */ 'R', 'O', 'U', 'N', 'D', '_', 'L', '_', 'D', '6', '4', 0,
  /* 2334 */ 'C', 'E', 'I', 'L', '_', 'L', '_', 'D', '6', '4', 0,
  /* 2345 */ 'F', 'L', 'O', 'O', 'R', '_', 'L', '_', 'D', '6', '4', 0,
  /* 2357 */ 'C', 'V', 'T', '_', 'L', '_', 'D', '6', '4', 0,
  /* 2367 */ 'C', '_', 'U', 'N', '_', 'D', '6', '4', 0,
  /* 2376 */ 'R', 'E', 'C', 'I', 'P', '_', 'D', '6', '4', 0,
  /* 2386 */ 'F', 'C', 'M', 'P', '_', 'D', '6', '4', 0,
  /* 2395 */ 'C', '_', 'S', 'E', 'Q', '_', 'D', '6', '4', 0,
  /* 2405 */ 'C', '_', 'U', 'E', 'Q', '_', 'D', '6', '4', 0,
  /* 2415 */ 'C', '_', 'E', 'Q', '_', 'D', '6', '4', 0,
  /* 2424 */ 'F', 'A', 'B', 'S', '_', 'D', '6', '4', 0,
  /* 2433 */ 'C', 'V', 'T', '_', 'S', '_', 'D', '6', '4', 0,
  /* 2443 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', '_', 'D', '6', '4', 0,
  /* 2460 */ 'C', '_', 'N', 'G', 'T', '_', 'D', '6', '4', 0,
  /* 2470 */ 'C', '_', 'O', 'L', 'T', '_', 'D', '6', '4', 0,
  /* 2480 */ 'C', '_', 'U', 'L', 'T', '_', 'D', '6', '4', 0,
  /* 2490 */ 'C', '_', 'L', 'T', '_', 'D', '6', '4', 0,
  /* 2499 */ 'F', 'S', 'Q', 'R', 'T', '_', 'D', '6', '4', 0,
  /* 2509 */ 'R', 'S', 'Q', 'R', 'T', '_', 'D', '6', '4', 0,
  /* 2519 */ 'M', 'O', 'V', 'T', '_', 'D', '6', '4', 0,
  /* 2528 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'T', '_', 'D', '6', '4', 0,
  /* 2549 */ 'F', 'D', 'I', 'V', '_', 'D', '6', '4', 0,
  /* 2558 */ 'F', 'M', 'O', 'V', '_', 'D', '6', '4', 0,
  /* 2567 */ 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'D', '6', '4', 0,
  /* 2579 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'D', '6', '4', 0,
  /* 2591 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'D', '6', '4', 0,
  /* 2602 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'D', '6', '4', 0,
  /* 2614 */ 'C', 'V', 'T', '_', 'W', '_', 'D', '6', '4', 0,
  /* 2624 */ 'B', 'N', 'E', '6', '4', 0,
  /* 2630 */ 'B', 'u', 'i', 'l', 'd', 'P', 'a', 'i', 'r', 'F', '6', '4', 0,
  /* 2643 */ 'E', 'x', 't', 'r', 'a', 'c', 't', 'E', 'l', 'e', 'm', 'e', 'n', 't', 'F', '6', '4', 0,
  /* 2661 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', '6', '4', 0,
  /* 2675 */ 'S', 'E', 'H', '6', '4', 0,
  /* 2681 */ 'L', 'H', '6', '4', 0,
  /* 2686 */ 'S', 'H', '6', '4', 0,
  /* 2691 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'F', 'H', 'I', '6', '4', 0,
  /* 2704 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'T', 'L', 'O', 'H', 'I', '6', '4', 0,
  /* 2719 */ 'M', 'T', 'H', 'I', '6', '4', 0,
  /* 2726 */ 'M', 'O', 'V', 'N', '_', 'I', '6', '4', '_', 'I', '6', '4', 0,
  /* 2739 */ 'M', 'O', 'V', 'Z', '_', 'I', '6', '4', '_', 'I', '6', '4', 0,
  /* 2752 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '6', '4', 0,
  /* 2772 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '6', '4', 0,
  /* 2792 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '6', '4', 0,
  /* 2813 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '6', '4', 0,
  /* 2833 */ 'M', 'O', 'V', 'F', '_', 'I', '6', '4', 0,
  /* 2842 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'F', '_', 'I', '6', '4', 0,
  /* 2863 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'I', '6', '4', 0,
  /* 2874 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'I', '6', '4', 0,
  /* 2885 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '6', '4', 0,
  /* 2901 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '6', '4', 0,
  /* 2921 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '6', '4', 0,
  /* 2941 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '6', '4', 0,
  /* 2960 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', '_', 'I', '6', '4', 0,
  /* 2977 */ 'M', 'O', 'V', 'T', '_', 'I', '6', '4', 0,
  /* 2986 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'T', '_', 'I', '6', '4', 0,
  /* 3007 */ 'L', 'L', '6', '4', 0,
  /* 3012 */ 'L', 'W', 'L', '6', '4', 0,
  /* 3018 */ 'S', 'W', 'L', '6', '4', 0,
  /* 3024 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'F', 'L', 'O', '6', '4', 0,
  /* 3037 */ 'M', 'T', 'L', 'O', '6', '4', 0,
  /* 3044 */ 'B', 'E', 'Q', '6', '4', 0,
  /* 3050 */ 'J', 'R', '6', '4', 0,
  /* 3055 */ 'J', 'A', 'L', 'R', '6', '4', 0,
  /* 3062 */ 'N', 'O', 'R', '6', '4', 0,
  /* 3068 */ 'X', 'O', 'R', '6', '4', 0,
  /* 3074 */ 'R', 'D', 'H', 'W', 'R', '6', '4', 0,
  /* 3082 */ 'L', 'W', 'R', '6', '4', 0,
  /* 3088 */ 'S', 'W', 'R', '6', '4', 0,
  /* 3094 */ 'S', 'L', 'T', '6', '4', 0,
  /* 3100 */ 'L', 'W', '6', '4', 0,
  /* 3105 */ 'S', 'W', '6', '4', 0,
  /* 3110 */ 'B', 'G', 'E', 'Z', '6', '4', 0,
  /* 3117 */ 'B', 'L', 'E', 'Z', '6', '4', 0,
  /* 3124 */ 'S', 'E', 'L', 'N', 'E', 'Z', '6', '4', 0,
  /* 3133 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '6', '4', 0,
  /* 3142 */ 'B', 'G', 'T', 'Z', '6', '4', 0,
  /* 3149 */ 'B', 'L', 'T', 'Z', '6', '4', 0,
  /* 3156 */ 'B', 'u', 'i', 'l', 'd', 'P', 'a', 'i', 'r', 'F', '6', '4', '_', '6', '4', 0,
  /* 3172 */ 'E', 'x', 't', 'r', 'a', 'c', 't', 'E', 'l', 'e', 'm', 'e', 'n', 't', 'F', '6', '4', '_', '6', '4', 0,
  /* 3193 */ 'S', 'L', 'L', '6', '4', '_', '6', '4', 0,
  /* 3202 */ 'L', 'o', 'a', 'd', 'A', 'd', 'd', 'r', 'R', 'e', 'g', '6', '4', 0,
  /* 3216 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'H', 'a', 'z', 'a', 'r', 'd', 'B', 'r', 'a', 'n', 'c', 'h', '6', '4', 0,
  /* 3245 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', '6', '4', 0,
  /* 3268 */ 'A', 'N', 'D', 'i', '6', '4', 0,
  /* 3275 */ 'X', 'O', 'R', 'i', '6', '4', 0,
  /* 3282 */ 'S', 'L', 'T', 'i', '6', '4', 0,
  /* 3289 */ 'L', 'U', 'i', '6', '4', 0,
  /* 3295 */ 'N', 'O', 'R', 'I', 'm', 'm', '6', '4', 0,
  /* 3304 */ 'S', 'L', 'T', 'I', 'm', 'm', '6', '4', 0,
  /* 3313 */ 'S', 'L', 'T', 'U', 'I', 'm', 'm', '6', '4', 0,
  /* 3323 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', '6', '4', 0,
  /* 3333 */ 'L', 'o', 'a', 'd', 'A', 'd', 'd', 'r', 'I', 'm', 'm', '6', '4', 0,
  /* 3347 */ 'P', 's', 'e', 'u', 'd', 'o', 'R', 'e', 't', 'u', 'r', 'n', '6', '4', 0,
  /* 3362 */ 'M', 'I', 'P', 'S', 'e', 'h', '_', 'r', 'e', 't', 'u', 'r', 'n', '6', '4', 0,
  /* 3378 */ 'L', 'B', 'u', '6', '4', 0,
  /* 3384 */ 'L', 'H', 'u', '6', '4', 0,
  /* 3390 */ 'S', 'L', 'T', 'u', '6', '4', 0,
  /* 3397 */ 'L', 'E', 'A', '_', 'A', 'D', 'D', 'i', 'u', '6', '4', 0,
  /* 3409 */ 'S', 'L', 'T', 'i', 'u', '6', '4', 0,
  /* 3417 */ 'M', 'o', 'v', 'e', 'R', '3', '2', '1', '6', 0,
  /* 3427 */ 'R', 'e', 't', 'R', 'A', '1', '6', 0,
  /* 3435 */ 'J', 'a', 'l', 'B', '1', '6', 0,
  /* 3442 */ 'L', 'D', '_', 'F', '1', '6', 0,
  /* 3449 */ 'S', 'T', '_', 'F', '1', '6', 0,
  /* 3456 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '1', '6', 0,
  /* 3476 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '1', '6', 0,
  /* 3496 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '1', '6', 0,
  /* 3517 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '1', '6', 0,
  /* 3537 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '1', '6', 0,
  /* 3553 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '1', '6', 0,
  /* 3573 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '1', '6', 0,
  /* 3593 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '1', '6', 0,
  /* 3612 */ 'M', 'o', 'v', 'e', '3', '2', 'R', '1', '6', 0,
  /* 3622 */ 'S', 'r', 'a', 'X', '1', '6', 0,
  /* 3629 */ 'R', 'e', 's', 't', 'o', 'r', 'e', 'X', '1', '6', 0,
  /* 3640 */ 'S', 'a', 'v', 'e', 'X', '1', '6', 0,
  /* 3648 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'C', 'm', 'p', 'i', 'X', '1', '6', 0,
  /* 3663 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'C', 'm', 'p', 'i', 'X', '1', '6', 0,
  /* 3678 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'S', 'l', 't', 'i', 'X', '1', '6', 0,
  /* 3693 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'S', 'l', 't', 'i', 'X', '1', '6', 0,
  /* 3708 */ 'S', 'l', 'l', 'X', '1', '6', 0,
  /* 3715 */ 'S', 'r', 'l', 'X', '1', '6', 0,
  /* 3722 */ 'L', 'b', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3738 */ 'S', 'b', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3754 */ 'L', 'h', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3770 */ 'S', 'h', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3786 */ 'L', 'b', 'u', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3803 */ 'L', 'h', 'u', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3820 */ 'A', 'd', 'd', 'i', 'u', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3839 */ 'L', 'w', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3855 */ 'S', 'w', 'R', 'x', 'R', 'y', 'O', 'f', 'f', 'M', 'e', 'm', 'X', '1', '6', 0,
  /* 3871 */ 'A', 'd', 'd', 'i', 'u', 'R', 'x', 'P', 'c', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3887 */ 'A', 'd', 'd', 'i', 'u', 'S', 'p', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3901 */ 'L', 'w', 'R', 'x', 'S', 'p', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3914 */ 'S', 'w', 'R', 'x', 'S', 'p', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3927 */ 'S', 'l', 't', 'i', 'C', 'C', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3942 */ 'S', 'l', 't', 'i', 'u', 'C', 'C', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3958 */ 'L', 'i', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3969 */ 'C', 'm', 'p', 'i', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3982 */ 'S', 'l', 't', 'i', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 3995 */ 'A', 'd', 'd', 'i', 'u', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 4009 */ 'S', 'l', 't', 'i', 'u', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 4023 */ 'A', 'd', 'd', 'i', 'u', 'R', 'x', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 4039 */ 'B', 'n', 'e', 'z', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 4052 */ 'B', 'e', 'q', 'z', 'R', 'x', 'I', 'm', 'm', 'X', '1', '6', 0,
  /* 4065 */ 'B', 'i', 'm', 'm', 'X', '1', '6', 0,
  /* 4073 */ 'L', 'i', 'R', 'x', 'I', 'm', 'm', 'A', 'l', 'i', 'g', 'n', 'X', '1', '6', 0,
  /* 4089 */ 'L', 'w', 'R', 'x', 'P', 'c', 'T', 'c', 'p', 'X', '1', '6', 0,
  /* 4102 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'C', 'm', 'p', 'X', '1', '6', 0,
  /* 4116 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'C', 'm', 'p', 'X', '1', '6', 0,
  /* 4130 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'S', 'l', 't', 'X', '1', '6', 0,
  /* 4144 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'S', 'l', 't', 'X', '1', '6', 0,
  /* 4158 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'S', 'l', 't', 'i', 'u', 'X', '1', '6', 0,
  /* 4174 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'S', 'l', 't', 'i', 'u', 'X', '1', '6', 0,
  /* 4190 */ 'B', 't', 'n', 'e', 'z', 'T', '8', 'S', 'l', 't', 'u', 'X', '1', '6', 0,
  /* 4205 */ 'B', 't', 'e', 'q', 'z', 'T', '8', 'S', 'l', 't', 'u', 'X', '1', '6', 0,
  /* 4220 */ 'B', 't', 'n', 'e', 'z', 'X', '1', '6', 0,
  /* 4229 */ 'B', 't', 'e', 'q', 'z', 'X', '1', '6', 0,
  /* 4238 */ 'J', 'r', 'c', 'R', 'a', '1', '6', 0,
  /* 4246 */ 'J', 'r', 'R', 'a', '1', '6', 0,
  /* 4253 */ 'R', 'e', 's', 't', 'o', 'r', 'e', '1', '6', 0,
  /* 4263 */ 'G', 'o', 't', 'P', 'r', 'o', 'l', 'o', 'g', 'u', 'e', '1', '6', 0,
  /* 4277 */ 'S', 'a', 'v', 'e', '1', '6', 0,
  /* 4284 */ 'J', 'u', 'm', 'p', 'L', 'i', 'n', 'k', 'R', 'e', 'g', '1', '6', 0,
  /* 4298 */ 'M', 'f', 'h', 'i', '1', '6', 0,
  /* 4305 */ 'B', 'r', 'e', 'a', 'k', '1', '6', 0,
  /* 4313 */ 'J', 'a', 'l', '1', '6', 0,
  /* 4319 */ 'A', 'd', 'd', 'i', 'u', 'S', 'p', 'I', 'm', 'm', '1', '6', 0,
  /* 4332 */ 'L', 'i', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4342 */ 'C', 'm', 'p', 'i', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4354 */ 'S', 'l', 't', 'i', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4366 */ 'S', 'l', 't', 'i', 'u', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4379 */ 'A', 'd', 'd', 'i', 'u', 'R', 'x', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4394 */ 'B', 'n', 'e', 'z', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4406 */ 'B', 'e', 'q', 'z', 'R', 'x', 'I', 'm', 'm', '1', '6', 0,
  /* 4418 */ 'B', 'i', 'm', 'm', '1', '6', 0,
  /* 4425 */ 'M', 'f', 'l', 'o', '1', '6', 0,
  /* 4432 */ 'L', 'w', 'R', 'x', 'P', 'c', 'T', 'c', 'p', '1', '6', 0,
  /* 4444 */ 'S', 'e', 'b', 'R', 'x', '1', '6', 0,
  /* 4452 */ 'J', 'r', 'c', 'R', 'x', '1', '6', 0,
  /* 4460 */ 'S', 'e', 'h', 'R', 'x', '1', '6', 0,
  /* 4468 */ 'S', 'l', 't', 'C', 'C', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4480 */ 'S', 'l', 't', 'u', 'C', 'C', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4493 */ 'N', 'e', 'g', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4503 */ 'C', 'm', 'p', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4513 */ 'S', 'l', 't', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4523 */ 'M', 'u', 'l', 't', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4534 */ 'N', 'o', 't', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4544 */ 'S', 'l', 't', 'u', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4555 */ 'M', 'u', 'l', 't', 'u', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4567 */ 'D', 'i', 'v', 'u', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4578 */ 'S', 'r', 'a', 'v', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4589 */ 'D', 'i', 'v', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4599 */ 'S', 'l', 'l', 'v', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4610 */ 'S', 'r', 'l', 'v', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4621 */ 'A', 'n', 'd', 'R', 'x', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4633 */ 'O', 'r', 'R', 'x', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4644 */ 'X', 'o', 'r', 'R', 'x', 'R', 'x', 'R', 'y', '1', '6', 0,
  /* 4656 */ 'M', 'u', 'l', 't', 'R', 'x', 'R', 'y', 'R', 'z', '1', '6', 0,
  /* 4669 */ 'S', 'u', 'b', 'u', 'R', 'x', 'R', 'y', 'R', 'z', '1', '6', 0,
  /* 4682 */ 'A', 'd', 'd', 'u', 'R', 'x', 'R', 'y', 'R', 'z', '1', '6', 0,
  /* 4695 */ 'S', 'l', 't', 'u', 'R', 'x', 'R', 'y', 'R', 'z', '1', '6', 0,
  /* 4708 */ 'M', 'u', 'l', 't', 'u', 'R', 'x', 'R', 'y', 'R', 'z', '1', '6', 0,
  /* 4722 */ 'B', 't', 'n', 'e', 'z', '1', '6', 0,
  /* 4730 */ 'B', 't', 'e', 'q', 'z', '1', '6', 0,
  /* 4738 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'r', 'e', 'c', 't', 'H', 'a', 'z', 'a', 'r', 'd', 'B', 'r', 'a', 'n', 'c', 'h', '6', '4', 'R', '6', 0,
  /* 4768 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', '6', '4', 'R', '6', 0,
  /* 4793 */ 'M', 'F', 'C', '0', '_', 'M', 'M', 'R', '6', 0,
  /* 4803 */ 'M', 'F', 'H', 'C', '0', '_', 'M', 'M', 'R', '6', 0,
  /* 4814 */ 'M', 'T', 'H', 'C', '0', '_', 'M', 'M', 'R', '6', 0,
  /* 4825 */ 'M', 'T', 'C', '0', '_', 'M', 'M', 'R', '6', 0,
  /* 4835 */ 'M', 'F', 'C', '1', '_', 'M', 'M', 'R', '6', 0,
  /* 4845 */ 'M', 'T', 'C', '1', '_', 'M', 'M', 'R', '6', 0,
  /* 4855 */ 'L', 'D', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4865 */ 'S', 'D', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4875 */ 'M', 'F', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4885 */ 'M', 'F', 'H', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4896 */ 'M', 'T', 'H', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4907 */ 'M', 'T', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4917 */ 'L', 'W', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4927 */ 'S', 'W', 'C', '2', '_', 'M', 'M', 'R', '6', 0,
  /* 4937 */ 'L', 'D', 'C', '1', '_', 'D', '6', '4', '_', 'M', 'M', 'R', '6', 0,
  /* 4951 */ 'S', 'D', 'C', '1', '_', 'D', '6', '4', '_', 'M', 'M', 'R', '6', 0,
  /* 4965 */ 'S', 'B', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 4975 */ 'B', 'C', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 4985 */ 'J', 'R', 'C', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 4996 */ 'J', 'A', 'L', 'R', 'C', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5009 */ 'B', 'N', 'E', 'Z', 'C', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5022 */ 'B', 'E', 'Q', 'Z', 'C', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5035 */ 'A', 'N', 'D', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5046 */ 'M', 'O', 'V', 'E', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5058 */ 'S', 'H', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5068 */ 'A', 'N', 'D', 'I', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5080 */ 'L', 'I', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5090 */ 'B', 'R', 'E', 'A', 'K', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5103 */ 'S', 'L', 'L', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5114 */ 'S', 'R', 'L', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5125 */ 'L', 'W', 'M', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5136 */ 'S', 'W', 'M', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5147 */ 'S', 'D', 'B', 'B', 'P', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5160 */ 'X', 'O', 'R', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5171 */ 'N', 'O', 'T', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5182 */ 'S', 'U', 'B', 'U', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5194 */ 'A', 'D', 'D', 'U', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5206 */ 'S', 'W', '1', '6', '_', 'M', 'M', 'R', '6', 0,
  /* 5216 */ 'L', 'S', 'A', '_', 'M', 'M', 'R', '6', 0,
  /* 5225 */ 'E', 'H', 'B', '_', 'M', 'M', 'R', '6', 0,
  /* 5234 */ 'J', 'A', 'L', 'R', 'C', '_', 'H', 'B', '_', 'M', 'M', 'R', '6', 0,
  /* 5248 */ 'L', 'B', '_', 'M', 'M', 'R', '6', 0,
  /* 5256 */ 'S', 'B', '_', 'M', 'M', 'R', '6', 0,
  /* 5264 */ 'S', 'U', 'B', '_', 'M', 'M', 'R', '6', 0,
  /* 5273 */ 'B', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5281 */ 'B', 'G', 'E', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5291 */ 'B', 'N', 'E', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5301 */ 'J', 'I', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5310 */ 'B', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5320 */ 'J', 'I', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5331 */ 'B', 'G', 'E', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5344 */ 'B', 'L', 'E', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5357 */ 'B', 'N', 'E', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5370 */ 'B', 'E', 'Q', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5383 */ 'B', 'G', 'T', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5396 */ 'B', 'L', 'T', 'Z', 'A', 'L', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5409 */ 'E', 'R', 'E', 'T', 'N', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5421 */ 'S', 'Y', 'N', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5431 */ 'A', 'U', 'I', 'P', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5442 */ 'A', 'L', 'U', 'I', 'P', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5454 */ 'A', 'D', 'D', 'I', 'U', 'P', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5467 */ 'L', 'W', 'P', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5477 */ 'B', 'E', 'Q', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5487 */ 'J', 'A', 'L', 'R', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5498 */ 'S', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5506 */ 'B', 'L', 'T', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5516 */ 'B', 'G', 'E', 'U', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5527 */ 'B', 'L', 'T', 'U', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5538 */ 'B', 'N', 'V', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5548 */ 'B', 'O', 'V', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5558 */ 'B', 'G', 'E', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5569 */ 'B', 'L', 'E', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5580 */ 'B', 'C', '1', 'N', 'E', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5593 */ 'B', 'C', '2', 'N', 'E', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5606 */ 'B', 'N', 'E', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5617 */ 'B', 'C', '1', 'E', 'Q', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5630 */ 'B', 'C', '2', 'E', 'Q', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5643 */ 'B', 'E', 'Q', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5654 */ 'B', 'G', 'T', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5665 */ 'B', 'L', 'T', 'Z', 'C', '_', 'M', 'M', 'R', '6', 0,
  /* 5676 */ 'A', 'D', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5685 */ 'A', 'N', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5694 */ 'M', 'O', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5703 */ 'M', 'I', 'N', 'A', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5715 */ 'M', 'A', 'X', 'A', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5727 */ 'C', 'M', 'P', '_', 'S', 'L', 'E', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5742 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'E', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5758 */ 'C', 'M', 'P', '_', 'U', 'L', 'E', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5773 */ 'C', 'M', 'P', '_', 'L', 'E', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5787 */ 'C', 'M', 'P', '_', 'S', 'A', 'F', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5802 */ 'C', 'M', 'P', '_', 'A', 'F', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5816 */ 'M', 'S', 'U', 'B', 'F', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5829 */ 'M', 'A', 'D', 'D', 'F', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5842 */ 'S', 'E', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5853 */ 'T', 'R', 'U', 'N', 'C', '_', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5868 */ 'R', 'O', 'U', 'N', 'D', '_', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5883 */ 'C', 'E', 'I', 'L', '_', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5897 */ 'F', 'L', 'O', 'O', 'R', '_', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5912 */ 'C', 'V', 'T', '_', 'L', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5925 */ 'M', 'I', 'N', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5936 */ 'C', 'M', 'P', '_', 'S', 'U', 'N', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5951 */ 'C', 'M', 'P', '_', 'U', 'N', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5965 */ 'C', 'M', 'P', '_', 'S', 'E', 'Q', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5980 */ 'C', 'M', 'P', '_', 'S', 'U', 'E', 'Q', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 5996 */ 'C', 'M', 'P', '_', 'U', 'E', 'Q', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6011 */ 'C', 'M', 'P', '_', 'E', 'Q', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6025 */ 'C', 'L', 'A', 'S', 'S', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6038 */ 'C', 'M', 'P', '_', 'S', 'L', 'T', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6053 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'T', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6069 */ 'C', 'M', 'P', '_', 'U', 'L', 'T', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6084 */ 'C', 'M', 'P', '_', 'L', 'T', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6098 */ 'R', 'I', 'N', 'T', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6110 */ 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6125 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6140 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6154 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6169 */ 'M', 'A', 'X', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6180 */ 'S', 'E', 'L', 'N', 'E', 'Z', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6194 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '_', 'D', '_', 'M', 'M', 'R', '6', 0,
  /* 6208 */ 'C', 'A', 'C', 'H', 'E', '_', 'M', 'M', 'R', '6', 0,
  /* 6219 */ 'P', 'A', 'U', 'S', 'E', '_', 'M', 'M', 'R', '6', 0,
  /* 6230 */ 'P', 'R', 'E', 'F', '_', 'M', 'M', 'R', '6', 0,
  /* 6240 */ 'T', 'L', 'B', 'I', 'N', 'V', 'F', '_', 'M', 'M', 'R', '6', 0,
  /* 6253 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', '_', 'M', 'M', 'R', '6', 0,
  /* 6270 */ 'W', 'S', 'B', 'H', '_', 'M', 'M', 'R', '6', 0,
  /* 6280 */ 'S', 'H', '_', 'M', 'M', 'R', '6', 0,
  /* 6288 */ 'M', 'U', 'H', '_', 'M', 'M', 'R', '6', 0,
  /* 6297 */ 'S', 'Y', 'N', 'C', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6308 */ 'A', 'N', 'D', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6318 */ 'E', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6326 */ 'X', 'O', 'R', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6336 */ 'A', 'U', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6345 */ 'L', 'U', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6354 */ 'G', 'I', 'N', 'V', 'I', '_', 'M', 'M', 'R', '6', 0,
  /* 6365 */ 'B', 'R', 'E', 'A', 'K', '_', 'M', 'M', 'R', '6', 0,
  /* 6376 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', '_', 'M', 'M', 'R', '6', 0,
  /* 6390 */ 'S', 'L', 'L', '_', 'M', 'M', 'R', '6', 0,
  /* 6399 */ 'M', 'U', 'L', '_', 'M', 'M', 'R', '6', 0,
  /* 6408 */ 'C', 'V', 'T', '_', 'D', '_', 'L', '_', 'M', 'M', 'R', '6', 0,
  /* 6421 */ 'C', 'V', 'T', '_', 'S', '_', 'L', '_', 'M', 'M', 'R', '6', 0,
  /* 6434 */ 'A', 'L', 'I', 'G', 'N', '_', 'M', 'M', 'R', '6', 0,
  /* 6445 */ 'C', 'L', 'O', '_', 'M', 'M', 'R', '6', 0,
  /* 6454 */ 'B', 'I', 'T', 'S', 'W', 'A', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6467 */ 'S', 'D', 'B', 'B', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6478 */ 'M', 'O', 'V', 'E', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6489 */ 'S', 'S', 'N', 'O', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6500 */ 'J', 'R', 'C', 'A', 'D', 'D', 'I', 'U', 'S', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6516 */ 'S', 'W', 'S', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6526 */ 'D', 'V', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6535 */ 'E', 'V', 'P', '_', 'M', 'M', 'R', '6', 0,
  /* 6544 */ 'N', 'O', 'R', '_', 'M', 'M', 'R', '6', 0,
  /* 6553 */ 'X', 'O', 'R', '_', 'M', 'M', 'R', '6', 0,
  /* 6562 */ 'R', 'D', 'P', 'G', 'P', 'R', '_', 'M', 'M', 'R', '6', 0,
  /* 6574 */ 'W', 'R', 'P', 'G', 'P', 'R', '_', 'M', 'M', 'R', '6', 0,
  /* 6586 */ 'R', 'D', 'H', 'W', 'R', '_', 'M', 'M', 'R', '6', 0,
  /* 6597 */ 'I', 'N', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6606 */ 'M', 'I', 'N', 'A', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6618 */ 'M', 'A', 'X', 'A', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6630 */ 'F', 'S', 'U', 'B', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6642 */ 'F', 'A', 'D', 'D', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6654 */ 'C', 'M', 'P', '_', 'S', 'L', 'E', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6669 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'E', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6685 */ 'C', 'M', 'P', '_', 'U', 'L', 'E', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6700 */ 'C', 'M', 'P', '_', 'L', 'E', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6714 */ 'C', 'M', 'P', '_', 'S', 'A', 'F', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6729 */ 'C', 'M', 'P', '_', 'A', 'F', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6743 */ 'M', 'S', 'U', 'B', 'F', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6756 */ 'M', 'A', 'D', 'D', 'F', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6769 */ 'F', 'N', 'E', 'G', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6781 */ 'S', 'E', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6792 */ 'F', 'M', 'U', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6804 */ 'T', 'R', 'U', 'N', 'C', '_', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6819 */ 'R', 'O', 'U', 'N', 'D', '_', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6834 */ 'C', 'E', 'I', 'L', '_', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6848 */ 'F', 'L', 'O', 'O', 'R', '_', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6863 */ 'C', 'V', 'T', '_', 'L', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6876 */ 'M', 'I', 'N', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6887 */ 'C', 'M', 'P', '_', 'S', 'U', 'N', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6902 */ 'C', 'M', 'P', '_', 'U', 'N', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6916 */ 'C', 'M', 'P', '_', 'S', 'E', 'Q', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6931 */ 'C', 'M', 'P', '_', 'S', 'U', 'E', 'Q', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6947 */ 'C', 'M', 'P', '_', 'U', 'E', 'Q', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6962 */ 'C', 'M', 'P', '_', 'E', 'Q', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6976 */ 'C', 'L', 'A', 'S', 'S', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 6989 */ 'C', 'M', 'P', '_', 'S', 'L', 'T', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7004 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'T', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7020 */ 'C', 'M', 'P', '_', 'U', 'L', 'T', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7035 */ 'C', 'M', 'P', '_', 'L', 'T', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7049 */ 'R', 'I', 'N', 'T', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7061 */ 'F', 'D', 'I', 'V', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7073 */ 'F', 'M', 'O', 'V', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7085 */ 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7100 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7115 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7129 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7144 */ 'C', 'V', 'T', '_', 'W', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7157 */ 'M', 'A', 'X', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7168 */ 'S', 'E', 'L', 'N', 'E', 'Z', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7182 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '_', 'S', '_', 'M', 'M', 'R', '6', 0,
  /* 7196 */ 'D', 'E', 'R', 'E', 'T', '_', 'M', 'M', 'R', '6', 0,
  /* 7207 */ 'W', 'A', 'I', 'T', '_', 'M', 'M', 'R', '6', 0,
  /* 7217 */ 'G', 'I', 'N', 'V', 'T', '_', 'M', 'M', 'R', '6', 0,
  /* 7228 */ 'E', 'X', 'T', '_', 'M', 'M', 'R', '6', 0,
  /* 7237 */ 'L', 'B', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7246 */ 'S', 'U', 'B', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7256 */ 'A', 'D', 'D', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7266 */ 'M', 'O', 'D', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7276 */ 'M', 'U', 'H', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7286 */ 'A', 'D', 'D', 'I', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7297 */ 'M', 'U', 'L', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7307 */ 'D', 'I', 'V', 'U', '_', 'M', 'M', 'R', '6', 0,
  /* 7317 */ 'D', 'I', 'V', '_', 'M', 'M', 'R', '6', 0,
  /* 7326 */ 'T', 'L', 'B', 'I', 'N', 'V', '_', 'M', 'M', 'R', '6', 0,
  /* 7338 */ 'L', 'W', '_', 'M', 'M', 'R', '6', 0,
  /* 7346 */ 'S', 'W', '_', 'M', 'M', 'R', '6', 0,
  /* 7354 */ 'C', 'V', 'T', '_', 'S', '_', 'W', '_', 'M', 'M', 'R', '6', 0,
  /* 7367 */ 'S', 'E', 'L', 'N', 'E', 'Z', '_', 'M', 'M', 'R', '6', 0,
  /* 7379 */ 'C', 'L', 'Z', '_', 'M', 'M', 'R', '6', 0,
  /* 7388 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '_', 'M', 'M', 'R', '6', 0,
  /* 7400 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', '_', 'M', 'M', 'R', '6', 0,
  /* 7426 */ 'L', 'D', 'C', '2', '_', 'R', '6', 0,
  /* 7434 */ 'S', 'D', 'C', '2', '_', 'R', '6', 0,
  /* 7442 */ 'L', 'W', 'C', '2', '_', 'R', '6', 0,
  /* 7450 */ 'S', 'W', 'C', '2', '_', 'R', '6', 0,
  /* 7458 */ 'J', 'R', '_', 'H', 'B', '6', '4', '_', 'R', '6', 0,
  /* 7469 */ 'S', 'C', '6', '4', '_', 'R', '6', 0,
  /* 7477 */ 'L', 'L', '6', '4', '_', 'R', '6', 0,
  /* 7485 */ 'D', 'L', 'S', 'A', '_', 'R', '6', 0,
  /* 7493 */ 'J', 'R', '_', 'H', 'B', '_', 'R', '6', 0,
  /* 7502 */ 'S', 'C', '_', 'R', '6', 0,
  /* 7508 */ 'S', 'C', 'D', '_', 'R', '6', 0,
  /* 7515 */ 'L', 'L', 'D', '_', 'R', '6', 0,
  /* 7522 */ 'C', 'A', 'C', 'H', 'E', '_', 'R', '6', 0,
  /* 7531 */ 'P', 'R', 'E', 'F', '_', 'R', '6', 0,
  /* 7539 */ 'L', 'L', '_', 'R', '6', 0,
  /* 7545 */ 'D', 'M', 'U', 'L', '_', 'R', '6', 0,
  /* 7553 */ 'D', 'C', 'L', 'O', '_', 'R', '6', 0,
  /* 7561 */ 'S', 'D', 'B', 'B', 'P', '_', 'R', '6', 0,
  /* 7570 */ 'D', 'C', 'L', 'Z', '_', 'R', '6', 0,
  /* 7578 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'r', 'e', 'c', 't', 'H', 'a', 'z', 'a', 'r', 'd', 'B', 'r', 'a', 'n', 'c', 'h', 'R', '6', 0,
  /* 7606 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', 'R', '6', 0,
  /* 7629 */ 'L', 'O', 'A', 'D', '_', 'A', 'C', 'C', '1', '2', '8', 0,
  /* 7641 */ 'S', 'T', 'O', 'R', 'E', '_', 'A', 'C', 'C', '1', '2', '8', 0,
  /* 7654 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '8', 0,
  /* 7673 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '8', 0,
  /* 7692 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '8', 0,
  /* 7712 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '8', 0,
  /* 7731 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '8', 0,
  /* 7746 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '8', 0,
  /* 7765 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '8', 0,
  /* 7784 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '8', 0,
  /* 7802 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 'A', 0,
  /* 7817 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 'A', 0,
  /* 7833 */ 'G', '_', 'F', 'M', 'A', 0,
  /* 7839 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 'A', 0,
  /* 7854 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 'A', 0,
  /* 7870 */ 'D', 'S', 'R', 'A', 0,
  /* 7875 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 7902 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 7929 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 7957 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 7984 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8007 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8034 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8061 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '3', '2', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8087 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8114 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8141 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8169 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8196 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8219 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8246 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8273 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '6', '4', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8299 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8326 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8353 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8381 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8408 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8431 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8458 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8485 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '1', '6', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8511 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'S', 'U', 'B', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8537 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'D', 'D', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8563 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'N', 'A', 'N', 'D', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8590 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'A', 'N', 'D', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8616 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'S', 'W', 'A', 'P', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8638 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', '_', 'S', 'W', 'A', 'P', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8664 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'X', 'O', 'R', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8690 */ 'A', 'T', 'O', 'M', 'I', 'C', '_', 'L', 'O', 'A', 'D', '_', 'O', 'R', '_', 'I', '8', '_', 'P', 'O', 'S', 'T', 'R', 'A', 0,
  /* 8715 */ 'R', 'e', 't', 'R', 'A', 0,
  /* 8721 */ 'D', 'L', 'S', 'A', 0,
  /* 8726 */ 'C', 'F', 'C', 'M', 'S', 'A', 0,
  /* 8733 */ 'C', 'T', 'C', 'M', 'S', 'A', 0,
  /* 8740 */ 'C', 'R', 'C', '3', '2', 'B', 0,
  /* 8747 */ 'C', 'R', 'C', '3', '2', 'C', 'B', 0,
  /* 8755 */ 'S', 'E', 'B', 0,
  /* 8759 */ 'E', 'H', 'B', 0,
  /* 8763 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', 'H', 'B', 0,
  /* 8777 */ 'J', 'R', '_', 'H', 'B', 0,
  /* 8783 */ 'J', 'A', 'L', 'R', '_', 'H', 'B', 0,
  /* 8791 */ 'L', 'B', 0,
  /* 8794 */ 'S', 'H', 'R', 'A', '_', 'Q', 'B', 0,
  /* 8802 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'L', 'E', '_', 'Q', 'B', 0,
  /* 8815 */ 'C', 'M', 'P', 'G', 'U', '_', 'L', 'E', '_', 'Q', 'B', 0,
  /* 8827 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', 'U', '_', 'L', 'E', '_', 'Q', 'B', 0,
  /* 8844 */ 'S', 'U', 'B', 'U', 'H', '_', 'Q', 'B', 0,
  /* 8853 */ 'A', 'D', 'D', 'U', 'H', '_', 'Q', 'B', 0,
  /* 8862 */ 'P', 's', 'e', 'u', 'd', 'o', 'P', 'I', 'C', 'K', '_', 'Q', 'B', 0,
  /* 8876 */ 'S', 'H', 'L', 'L', '_', 'Q', 'B', 0,
  /* 8884 */ 'R', 'E', 'P', 'L', '_', 'Q', 'B', 0,
  /* 8892 */ 'S', 'H', 'R', 'L', '_', 'Q', 'B', 0,
  /* 8900 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'E', 'Q', '_', 'Q', 'B', 0,
  /* 8913 */ 'C', 'M', 'P', 'G', 'U', '_', 'E', 'Q', '_', 'Q', 'B', 0,
  /* 8925 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', 'U', '_', 'E', 'Q', '_', 'Q', 'B', 0,
  /* 8942 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'Q', 'B', 0,
  /* 8952 */ 'S', 'U', 'B', 'U', 'H', '_', 'R', '_', 'Q', 'B', 0,
  /* 8963 */ 'A', 'D', 'D', 'U', 'H', '_', 'R', '_', 'Q', 'B', 0,
  /* 8974 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'Q', 'B', 0,
  /* 8985 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'Q', 'B', 0,
  /* 8995 */ 'S', 'U', 'B', 'U', '_', 'S', '_', 'Q', 'B', 0,
  /* 9005 */ 'A', 'D', 'D', 'U', '_', 'S', '_', 'Q', 'B', 0,
  /* 9015 */ 'C', 'M', 'P', 'G', 'D', 'U', '_', 'L', 'T', '_', 'Q', 'B', 0,
  /* 9028 */ 'C', 'M', 'P', 'G', 'U', '_', 'L', 'T', '_', 'Q', 'B', 0,
  /* 9040 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', 'U', '_', 'L', 'T', '_', 'Q', 'B', 0,
  /* 9057 */ 'S', 'U', 'B', 'U', '_', 'Q', 'B', 0,
  /* 9065 */ 'A', 'D', 'D', 'U', '_', 'Q', 'B', 0,
  /* 9073 */ 'S', 'H', 'R', 'A', 'V', '_', 'Q', 'B', 0,
  /* 9082 */ 'S', 'H', 'L', 'L', 'V', '_', 'Q', 'B', 0,
  /* 9091 */ 'R', 'E', 'P', 'L', 'V', '_', 'Q', 'B', 0,
  /* 9100 */ 'S', 'H', 'R', 'L', 'V', '_', 'Q', 'B', 0,
  /* 9109 */ 'R', 'A', 'D', 'D', 'U', '_', 'W', '_', 'Q', 'B', 0,
  /* 9120 */ 'S', 'B', 0,
  /* 9123 */ 'M', 'O', 'D', 'S', 'U', 'B', 0,
  /* 9130 */ 'G', '_', 'F', 'S', 'U', 'B', 0,
  /* 9137 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'S', 'U', 'B', 0,
  /* 9148 */ 'G', '_', 'S', 'U', 'B', 0,
  /* 9154 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'S', 'U', 'B', 0,
  /* 9170 */ 'S', 'R', 'A', '_', 'B', 0,
  /* 9176 */ 'A', 'D', 'D', '_', 'A', '_', 'B', 0,
  /* 9184 */ 'M', 'I', 'N', '_', 'A', '_', 'B', 0,
  /* 9192 */ 'A', 'D', 'D', 'S', '_', 'A', '_', 'B', 0,
  /* 9201 */ 'M', 'A', 'X', '_', 'A', '_', 'B', 0,
  /* 9209 */ 'N', 'L', 'O', 'C', '_', 'B', 0,
  /* 9216 */ 'N', 'L', 'Z', 'C', '_', 'B', 0,
  /* 9223 */ 'S', 'L', 'D', '_', 'B', 0,
  /* 9229 */ 'P', 'C', 'K', 'O', 'D', '_', 'B', 0,
  /* 9237 */ 'I', 'L', 'V', 'O', 'D', '_', 'B', 0,
  /* 9245 */ 'I', 'N', 'S', 'V', 'E', '_', 'B', 0,
  /* 9253 */ 'V', 'S', 'H', 'F', '_', 'B', 0,
  /* 9260 */ 'B', 'N', 'E', 'G', '_', 'B', 0,
  /* 9267 */ 'S', 'R', 'A', 'I', '_', 'B', 0,
  /* 9274 */ 'S', 'L', 'D', 'I', '_', 'B', 0,
  /* 9281 */ 'A', 'N', 'D', 'I', '_', 'B', 0,
  /* 9288 */ 'B', 'N', 'E', 'G', 'I', '_', 'B', 0,
  /* 9296 */ 'B', 'S', 'E', 'L', 'I', '_', 'B', 0,
  /* 9304 */ 'S', 'L', 'L', 'I', '_', 'B', 0,
  /* 9311 */ 'S', 'R', 'L', 'I', '_', 'B', 0,
  /* 9318 */ 'B', 'I', 'N', 'S', 'L', 'I', '_', 'B', 0,
  /* 9327 */ 'C', 'E', 'Q', 'I', '_', 'B', 0,
  /* 9334 */ 'S', 'R', 'A', 'R', 'I', '_', 'B', 0,
  /* 9342 */ 'B', 'C', 'L', 'R', 'I', '_', 'B', 0,
  /* 9350 */ 'S', 'R', 'L', 'R', 'I', '_', 'B', 0,
  /* 9358 */ 'N', 'O', 'R', 'I', '_', 'B', 0,
  /* 9365 */ 'X', 'O', 'R', 'I', '_', 'B', 0,
  /* 9372 */ 'B', 'I', 'N', 'S', 'R', 'I', '_', 'B', 0,
  /* 9381 */ 'S', 'P', 'L', 'A', 'T', 'I', '_', 'B', 0,
  /* 9390 */ 'B', 'S', 'E', 'T', 'I', '_', 'B', 0,
  /* 9398 */ 'S', 'U', 'B', 'V', 'I', '_', 'B', 0,
  /* 9406 */ 'A', 'D', 'D', 'V', 'I', '_', 'B', 0,
  /* 9414 */ 'B', 'M', 'Z', 'I', '_', 'B', 0,
  /* 9421 */ 'B', 'M', 'N', 'Z', 'I', '_', 'B', 0,
  /* 9429 */ 'F', 'I', 'L', 'L', '_', 'B', 0,
  /* 9436 */ 'S', 'L', 'L', '_', 'B', 0,
  /* 9442 */ 'S', 'R', 'L', '_', 'B', 0,
  /* 9448 */ 'B', 'I', 'N', 'S', 'L', '_', 'B', 0,
  /* 9456 */ 'I', 'L', 'V', 'L', '_', 'B', 0,
  /* 9463 */ 'C', 'E', 'Q', '_', 'B', 0,
  /* 9469 */ 'S', 'R', 'A', 'R', '_', 'B', 0,
  /* 9476 */ 'B', 'C', 'L', 'R', '_', 'B', 0,
  /* 9483 */ 'S', 'R', 'L', 'R', '_', 'B', 0,
  /* 9490 */ 'B', 'I', 'N', 'S', 'R', '_', 'B', 0,
  /* 9498 */ 'I', 'L', 'V', 'R', '_', 'B', 0,
  /* 9505 */ 'A', 'S', 'U', 'B', '_', 'S', '_', 'B', 0,
  /* 9514 */ 'M', 'O', 'D', '_', 'S', '_', 'B', 0,
  /* 9522 */ 'C', 'L', 'E', '_', 'S', '_', 'B', 0,
  /* 9530 */ 'A', 'V', 'E', '_', 'S', '_', 'B', 0,
  /* 9538 */ 'C', 'L', 'E', 'I', '_', 'S', '_', 'B', 0,
  /* 9547 */ 'M', 'I', 'N', 'I', '_', 'S', '_', 'B', 0,
  /* 9556 */ 'C', 'L', 'T', 'I', '_', 'S', '_', 'B', 0,
  /* 9565 */ 'M', 'A', 'X', 'I', '_', 'S', '_', 'B', 0,
  /* 9574 */ 'M', 'I', 'N', '_', 'S', '_', 'B', 0,
  /* 9582 */ 'A', 'V', 'E', 'R', '_', 'S', '_', 'B', 0,
  /* 9591 */ 'S', 'U', 'B', 'S', '_', 'S', '_', 'B', 0,
  /* 9600 */ 'A', 'D', 'D', 'S', '_', 'S', '_', 'B', 0,
  /* 9609 */ 'S', 'A', 'T', '_', 'S', '_', 'B', 0,
  /* 9617 */ 'C', 'L', 'T', '_', 'S', '_', 'B', 0,
  /* 9625 */ 'S', 'U', 'B', 'S', 'U', 'U', '_', 'S', '_', 'B', 0,
  /* 9636 */ 'D', 'I', 'V', '_', 'S', '_', 'B', 0,
  /* 9644 */ 'M', 'A', 'X', '_', 'S', '_', 'B', 0,
  /* 9652 */ 'C', 'O', 'P', 'Y', '_', 'S', '_', 'B', 0,
  /* 9661 */ 'S', 'P', 'L', 'A', 'T', '_', 'B', 0,
  /* 9669 */ 'B', 'S', 'E', 'T', '_', 'B', 0,
  /* 9676 */ 'P', 'C', 'N', 'T', '_', 'B', 0,
  /* 9683 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'B', 0,
  /* 9692 */ 'S', 'T', '_', 'B', 0,
  /* 9697 */ 'A', 'S', 'U', 'B', '_', 'U', '_', 'B', 0,
  /* 9706 */ 'M', 'O', 'D', '_', 'U', '_', 'B', 0,
  /* 9714 */ 'C', 'L', 'E', '_', 'U', '_', 'B', 0,
  /* 9722 */ 'A', 'V', 'E', '_', 'U', '_', 'B', 0,
  /* 9730 */ 'C', 'L', 'E', 'I', '_', 'U', '_', 'B', 0,
  /* 9739 */ 'M', 'I', 'N', 'I', '_', 'U', '_', 'B', 0,
  /* 9748 */ 'C', 'L', 'T', 'I', '_', 'U', '_', 'B', 0,
  /* 9757 */ 'M', 'A', 'X', 'I', '_', 'U', '_', 'B', 0,
  /* 9766 */ 'M', 'I', 'N', '_', 'U', '_', 'B', 0,
  /* 9774 */ 'A', 'V', 'E', 'R', '_', 'U', '_', 'B', 0,
  /* 9783 */ 'S', 'U', 'B', 'S', '_', 'U', '_', 'B', 0,
  /* 9792 */ 'A', 'D', 'D', 'S', '_', 'U', '_', 'B', 0,
  /* 9801 */ 'S', 'U', 'B', 'S', 'U', 'S', '_', 'U', '_', 'B', 0,
  /* 9812 */ 'S', 'A', 'T', '_', 'U', '_', 'B', 0,
  /* 9820 */ 'C', 'L', 'T', '_', 'U', '_', 'B', 0,
  /* 9828 */ 'D', 'I', 'V', '_', 'U', '_', 'B', 0,
  /* 9836 */ 'M', 'A', 'X', '_', 'U', '_', 'B', 0,
  /* 9844 */ 'C', 'O', 'P', 'Y', '_', 'U', '_', 'B', 0,
  /* 9853 */ 'M', 'S', 'U', 'B', 'V', '_', 'B', 0,
  /* 9861 */ 'M', 'A', 'D', 'D', 'V', '_', 'B', 0,
  /* 9869 */ 'P', 'C', 'K', 'E', 'V', '_', 'B', 0,
  /* 9877 */ 'I', 'L', 'V', 'E', 'V', '_', 'B', 0,
  /* 9885 */ 'M', 'U', 'L', 'V', '_', 'B', 0,
  /* 9892 */ 'B', 'Z', '_', 'B', 0,
  /* 9897 */ 'B', 'N', 'Z', '_', 'B', 0,
  /* 9903 */ 'B', 'C', 0,
  /* 9906 */ 'B', 'G', 'E', 'C', 0,
  /* 9911 */ 'B', 'N', 'E', 'C', 0,
  /* 9916 */ 'J', 'I', 'C', 0,
  /* 9920 */ 'G', '_', 'I', 'N', 'T', 'R', 'I', 'N', 'S', 'I', 'C', 0,
  /* 9932 */ 'B', 'A', 'L', 'C', 0,
  /* 9937 */ 'J', 'I', 'A', 'L', 'C', 0,
  /* 9943 */ 'B', 'G', 'E', 'Z', 'A', 'L', 'C', 0,
  /* 9951 */ 'B', 'L', 'E', 'Z', 'A', 'L', 'C', 0,
  /* 9959 */ 'B', 'N', 'E', 'Z', 'A', 'L', 'C', 0,
  /* 9967 */ 'B', 'E', 'Q', 'Z', 'A', 'L', 'C', 0,
  /* 9975 */ 'B', 'G', 'T', 'Z', 'A', 'L', 'C', 0,
  /* 9983 */ 'B', 'L', 'T', 'Z', 'A', 'L', 'C', 0,
  /* 9991 */ 'E', 'R', 'E', 'T', 'N', 'C', 0,
  /* 9998 */ 'G', '_', 'F', 'P', 'T', 'R', 'U', 'N', 'C', 0,
  /* 10008 */ 'G', '_', 'T', 'R', 'U', 'N', 'C', 0,
  /* 10016 */ 'S', 'Y', 'N', 'C', 0,
  /* 10021 */ 'L', 'D', 'P', 'C', 0,
  /* 10026 */ 'A', 'U', 'I', 'P', 'C', 0,
  /* 10032 */ 'A', 'L', 'U', 'I', 'P', 'C', 0,
  /* 10039 */ 'A', 'D', 'D', 'I', 'U', 'P', 'C', 0,
  /* 10047 */ 'L', 'W', 'U', 'P', 'C', 0,
  /* 10053 */ 'L', 'W', 'P', 'C', 0,
  /* 10058 */ 'B', 'E', 'Q', 'C', 0,
  /* 10063 */ 'A', 'D', 'D', 'S', 'C', 0,
  /* 10069 */ 'B', 'L', 'T', 'C', 0,
  /* 10074 */ 'B', 'G', 'E', 'U', 'C', 0,
  /* 10080 */ 'B', 'L', 'T', 'U', 'C', 0,
  /* 10086 */ 'B', 'N', 'V', 'C', 0,
  /* 10091 */ 'B', 'O', 'V', 'C', 0,
  /* 10096 */ 'A', 'D', 'D', 'W', 'C', 0,
  /* 10102 */ 'B', 'G', 'E', 'Z', 'C', 0,
  /* 10108 */ 'B', 'L', 'E', 'Z', 'C', 0,
  /* 10114 */ 'B', 'N', 'E', 'Z', 'C', 0,
  /* 10120 */ 'B', 'E', 'Q', 'Z', 'C', 0,
  /* 10126 */ 'B', 'G', 'T', 'Z', 'C', 0,
  /* 10132 */ 'B', 'L', 'T', 'Z', 'C', 0,
  /* 10138 */ 'C', 'R', 'C', '3', '2', 'D', 0,
  /* 10145 */ 'G', '_', 'S', 'E', 'X', 'T', 'L', 'O', 'A', 'D', 0,
  /* 10156 */ 'G', '_', 'Z', 'E', 'X', 'T', 'L', 'O', 'A', 'D', 0,
  /* 10167 */ 'G', '_', 'L', 'O', 'A', 'D', 0,
  /* 10174 */ 'C', 'R', 'C', '3', '2', 'C', 'D', 0,
  /* 10182 */ 'S', 'C', 'D', 0,
  /* 10186 */ 'D', 'A', 'D', 'D', 0,
  /* 10191 */ 'G', '_', 'F', 'A', 'D', 'D', 0,
  /* 10198 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'A', 'D', 'D', 0,
  /* 10209 */ 'G', '_', 'A', 'D', 'D', 0,
  /* 10215 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'A', 'D', 'D', 0,
  /* 10231 */ 'D', 'S', 'H', 'D', 0,
  /* 10236 */ 'Y', 'I', 'E', 'L', 'D', 0,
  /* 10242 */ 'L', 'L', 'D', 0,
  /* 10246 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'N', 'A', 'N', 'D', 0,
  /* 10263 */ 'G', '_', 'A', 'N', 'D', 0,
  /* 10269 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'A', 'N', 'D', 0,
  /* 10285 */ 'P', 'R', 'E', 'P', 'E', 'N', 'D', 0,
  /* 10293 */ 'A', 'P', 'P', 'E', 'N', 'D', 0,
  /* 10300 */ 'L', 'I', 'F', 'E', 'T', 'I', 'M', 'E', '_', 'E', 'N', 'D', 0,
  /* 10313 */ 'G', '_', 'B', 'R', 'C', 'O', 'N', 'D', 0,
  /* 10322 */ 'D', 'M', 'O', 'D', 0,
  /* 10327 */ 'L', 'O', 'A', 'D', '_', 'S', 'T', 'A', 'C', 'K', '_', 'G', 'U', 'A', 'R', 'D', 0,
  /* 10344 */ 'S', 'D', 0,
  /* 10347 */ 'F', 'L', 'O', 'G', '2', '_', 'D', 0,
  /* 10355 */ 'F', 'E', 'X', 'P', '2', '_', 'D', 0,
  /* 10363 */ 'M', 'I', 'N', 'A', '_', 'D', 0,
  /* 10370 */ 'S', 'R', 'A', '_', 'D', 0,
  /* 10376 */ 'M', 'A', 'X', 'A', '_', 'D', 0,
  /* 10383 */ 'A', 'D', 'D', '_', 'A', '_', 'D', 0,
  /* 10391 */ 'F', 'M', 'I', 'N', '_', 'A', '_', 'D', 0,
  /* 10400 */ 'A', 'D', 'D', 'S', '_', 'A', '_', 'D', 0,
  /* 10409 */ 'F', 'M', 'A', 'X', '_', 'A', '_', 'D', 0,
  /* 10418 */ 'F', 'S', 'U', 'B', '_', 'D', 0,
  /* 10425 */ 'F', 'M', 'S', 'U', 'B', '_', 'D', 0,
  /* 10433 */ 'N', 'L', 'O', 'C', '_', 'D', 0,
  /* 10440 */ 'N', 'L', 'Z', 'C', '_', 'D', 0,
  /* 10447 */ 'F', 'A', 'D', 'D', '_', 'D', 0,
  /* 10454 */ 'F', 'M', 'A', 'D', 'D', '_', 'D', 0,
  /* 10462 */ 'S', 'L', 'D', '_', 'D', 0,
  /* 10468 */ 'P', 'C', 'K', 'O', 'D', '_', 'D', 0,
  /* 10476 */ 'I', 'L', 'V', 'O', 'D', '_', 'D', 0,
  /* 10484 */ 'F', 'C', 'L', 'E', '_', 'D', 0,
  /* 10491 */ 'F', 'S', 'L', 'E', '_', 'D', 0,
  /* 10498 */ 'C', 'M', 'P', '_', 'S', 'L', 'E', '_', 'D', 0,
  /* 10508 */ 'F', 'C', 'U', 'L', 'E', '_', 'D', 0,
  /* 10516 */ 'F', 'S', 'U', 'L', 'E', '_', 'D', 0,
  /* 10524 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'E', '_', 'D', 0,
  /* 10535 */ 'C', 'M', 'P', '_', 'U', 'L', 'E', '_', 'D', 0,
  /* 10545 */ 'C', 'M', 'P', '_', 'L', 'E', '_', 'D', 0,
  /* 10554 */ 'F', 'C', 'N', 'E', '_', 'D', 0,
  /* 10561 */ 'F', 'S', 'N', 'E', '_', 'D', 0,
  /* 10568 */ 'F', 'C', 'U', 'N', 'E', '_', 'D', 0,
  /* 10576 */ 'F', 'S', 'U', 'N', 'E', '_', 'D', 0,
  /* 10584 */ 'I', 'N', 'S', 'V', 'E', '_', 'D', 0,
  /* 10592 */ 'F', 'C', 'A', 'F', '_', 'D', 0,
  /* 10599 */ 'F', 'S', 'A', 'F', '_', 'D', 0,
  /* 10606 */ 'C', 'M', 'P', '_', 'S', 'A', 'F', '_', 'D', 0,
  /* 10616 */ 'M', 'S', 'U', 'B', 'F', '_', 'D', 0,
  /* 10624 */ 'M', 'A', 'D', 'D', 'F', '_', 'D', 0,
  /* 10632 */ 'V', 'S', 'H', 'F', '_', 'D', 0,
  /* 10639 */ 'C', 'M', 'P', '_', 'F', '_', 'D', 0,
  /* 10647 */ 'B', 'N', 'E', 'G', '_', 'D', 0,
  /* 10654 */ 'S', 'R', 'A', 'I', '_', 'D', 0,
  /* 10661 */ 'S', 'L', 'D', 'I', '_', 'D', 0,
  /* 10668 */ 'B', 'N', 'E', 'G', 'I', '_', 'D', 0,
  /* 10676 */ 'S', 'L', 'L', 'I', '_', 'D', 0,
  /* 10683 */ 'S', 'R', 'L', 'I', '_', 'D', 0,
  /* 10690 */ 'B', 'I', 'N', 'S', 'L', 'I', '_', 'D', 0,
  /* 10699 */ 'C', 'E', 'Q', 'I', '_', 'D', 0,
  /* 10706 */ 'S', 'R', 'A', 'R', 'I', '_', 'D', 0,
  /* 10714 */ 'B', 'C', 'L', 'R', 'I', '_', 'D', 0,
  /* 10722 */ 'S', 'R', 'L', 'R', 'I', '_', 'D', 0,
  /* 10730 */ 'B', 'I', 'N', 'S', 'R', 'I', '_', 'D', 0,
  /* 10739 */ 'S', 'P', 'L', 'A', 'T', 'I', '_', 'D', 0,
  /* 10748 */ 'B', 'S', 'E', 'T', 'I', '_', 'D', 0,
  /* 10756 */ 'S', 'U', 'B', 'V', 'I', '_', 'D', 0,
  /* 10764 */ 'A', 'D', 'D', 'V', 'I', '_', 'D', 0,
  /* 10772 */ 'S', 'E', 'L', '_', 'D', 0,
  /* 10778 */ 'F', 'I', 'L', 'L', '_', 'D', 0,
  /* 10785 */ 'S', 'L', 'L', '_', 'D', 0,
  /* 10791 */ 'F', 'E', 'X', 'U', 'P', 'L', '_', 'D', 0,
  /* 10800 */ 'F', 'F', 'Q', 'L', '_', 'D', 0,
  /* 10807 */ 'S', 'R', 'L', '_', 'D', 0,
  /* 10813 */ 'B', 'I', 'N', 'S', 'L', '_', 'D', 0,
  /* 10821 */ 'F', 'M', 'U', 'L', '_', 'D', 0,
  /* 10828 */ 'I', 'L', 'V', 'L', '_', 'D', 0,
  /* 10835 */ 'F', 'M', 'I', 'N', '_', 'D', 0,
  /* 10842 */ 'F', 'C', 'U', 'N', '_', 'D', 0,
  /* 10849 */ 'F', 'S', 'U', 'N', '_', 'D', 0,
  /* 10856 */ 'C', 'M', 'P', '_', 'S', 'U', 'N', '_', 'D', 0,
  /* 10866 */ 'C', 'M', 'P', '_', 'U', 'N', '_', 'D', 0,
  /* 10875 */ 'F', 'R', 'C', 'P', '_', 'D', 0,
  /* 10882 */ 'F', 'C', 'E', 'Q', '_', 'D', 0,
  /* 10889 */ 'F', 'S', 'E', 'Q', '_', 'D', 0,
  /* 10896 */ 'C', 'M', 'P', '_', 'S', 'E', 'Q', '_', 'D', 0,
  /* 10906 */ 'F', 'C', 'U', 'E', 'Q', '_', 'D', 0,
  /* 10914 */ 'F', 'S', 'U', 'E', 'Q', '_', 'D', 0,
  /* 10922 */ 'C', 'M', 'P', '_', 'S', 'U', 'E', 'Q', '_', 'D', 0,
  /* 10933 */ 'C', 'M', 'P', '_', 'U', 'E', 'Q', '_', 'D', 0,
  /* 10943 */ 'C', 'M', 'P', '_', 'E', 'Q', '_', 'D', 0,
  /* 10952 */ 'S', 'R', 'A', 'R', '_', 'D', 0,
  /* 10959 */ 'B', 'C', 'L', 'R', '_', 'D', 0,
  /* 10966 */ 'S', 'R', 'L', 'R', '_', 'D', 0,
  /* 10973 */ 'F', 'C', 'O', 'R', '_', 'D', 0,
  /* 10980 */ 'F', 'S', 'O', 'R', '_', 'D', 0,
  /* 10987 */ 'F', 'E', 'X', 'U', 'P', 'R', '_', 'D', 0,
  /* 10996 */ 'F', 'F', 'Q', 'R', '_', 'D', 0,
  /* 11003 */ 'B', 'I', 'N', 'S', 'R', '_', 'D', 0,
  /* 11011 */ 'I', 'L', 'V', 'R', '_', 'D', 0,
  /* 11018 */ 'F', 'A', 'B', 'S', '_', 'D', 0,
  /* 11025 */ 'F', 'C', 'L', 'A', 'S', 'S', '_', 'D', 0,
  /* 11034 */ 'A', 'S', 'U', 'B', '_', 'S', '_', 'D', 0,
  /* 11043 */ 'H', 'S', 'U', 'B', '_', 'S', '_', 'D', 0,
  /* 11052 */ 'D', 'P', 'S', 'U', 'B', '_', 'S', '_', 'D', 0,
  /* 11062 */ 'F', 'T', 'R', 'U', 'N', 'C', '_', 'S', '_', 'D', 0,
  /* 11073 */ 'H', 'A', 'D', 'D', '_', 'S', '_', 'D', 0,
  /* 11082 */ 'D', 'P', 'A', 'D', 'D', '_', 'S', '_', 'D', 0,
  /* 11092 */ 'M', 'O', 'D', '_', 'S', '_', 'D', 0,
  /* 11100 */ 'C', 'L', 'E', '_', 'S', '_', 'D', 0,
  /* 11108 */ 'A', 'V', 'E', '_', 'S', '_', 'D', 0,
  /* 11116 */ 'C', 'L', 'E', 'I', '_', 'S', '_', 'D', 0,
  /* 11125 */ 'M', 'I', 'N', 'I', '_', 'S', '_', 'D', 0,
  /* 11134 */ 'C', 'L', 'T', 'I', '_', 'S', '_', 'D', 0,
  /* 11143 */ 'M', 'A', 'X', 'I', '_', 'S', '_', 'D', 0,
  /* 11152 */ 'M', 'I', 'N', '_', 'S', '_', 'D', 0,
  /* 11160 */ 'D', 'O', 'T', 'P', '_', 'S', '_', 'D', 0,
  /* 11169 */ 'A', 'V', 'E', 'R', '_', 'S', '_', 'D', 0,
  /* 11178 */ 'S', 'U', 'B', 'S', '_', 'S', '_', 'D', 0,
  /* 11187 */ 'A', 'D', 'D', 'S', '_', 'S', '_', 'D', 0,
  /* 11196 */ 'S', 'A', 'T', '_', 'S', '_', 'D', 0,
  /* 11204 */ 'C', 'L', 'T', '_', 'S', '_', 'D', 0,
  /* 11212 */ 'F', 'F', 'I', 'N', 'T', '_', 'S', '_', 'D', 0,
  /* 11222 */ 'F', 'T', 'I', 'N', 'T', '_', 'S', '_', 'D', 0,
  /* 11232 */ 'S', 'U', 'B', 'S', 'U', 'U', '_', 'S', '_', 'D', 0,
  /* 11243 */ 'D', 'I', 'V', '_', 'S', '_', 'D', 0,
  /* 11251 */ 'M', 'A', 'X', '_', 'S', '_', 'D', 0,
  /* 11259 */ 'C', 'O', 'P', 'Y', '_', 'S', '_', 'D', 0,
  /* 11268 */ 'S', 'P', 'L', 'A', 'T', '_', 'D', 0,
  /* 11276 */ 'B', 'S', 'E', 'T', '_', 'D', 0,
  /* 11283 */ 'F', 'C', 'L', 'T', '_', 'D', 0,
  /* 11290 */ 'F', 'S', 'L', 'T', '_', 'D', 0,
  /* 11297 */ 'C', 'M', 'P', '_', 'S', 'L', 'T', '_', 'D', 0,
  /* 11307 */ 'F', 'C', 'U', 'L', 'T', '_', 'D', 0,
  /* 11315 */ 'F', 'S', 'U', 'L', 'T', '_', 'D', 0,
  /* 11323 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'T', '_', 'D', 0,
  /* 11334 */ 'C', 'M', 'P', '_', 'U', 'L', 'T', '_', 'D', 0,
  /* 11344 */ 'C', 'M', 'P', '_', 'L', 'T', '_', 'D', 0,
  /* 11353 */ 'P', 'C', 'N', 'T', '_', 'D', 0,
  /* 11360 */ 'F', 'R', 'I', 'N', 'T', '_', 'D', 0,
  /* 11368 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'D', 0,
  /* 11377 */ 'F', 'S', 'Q', 'R', 'T', '_', 'D', 0,
  /* 11385 */ 'F', 'R', 'S', 'Q', 'R', 'T', '_', 'D', 0,
  /* 11394 */ 'S', 'T', '_', 'D', 0,
  /* 11399 */ 'A', 'S', 'U', 'B', '_', 'U', '_', 'D', 0,
  /* 11408 */ 'H', 'S', 'U', 'B', '_', 'U', '_', 'D', 0,
  /* 11417 */ 'D', 'P', 'S', 'U', 'B', '_', 'U', '_', 'D', 0,
  /* 11427 */ 'F', 'T', 'R', 'U', 'N', 'C', '_', 'U', '_', 'D', 0,
  /* 11438 */ 'H', 'A', 'D', 'D', '_', 'U', '_', 'D', 0,
  /* 11447 */ 'D', 'P', 'A', 'D', 'D', '_', 'U', '_', 'D', 0,
  /* 11457 */ 'M', 'O', 'D', '_', 'U', '_', 'D', 0,
  /* 11465 */ 'C', 'L', 'E', '_', 'U', '_', 'D', 0,
  /* 11473 */ 'A', 'V', 'E', '_', 'U', '_', 'D', 0,
  /* 11481 */ 'C', 'L', 'E', 'I', '_', 'U', '_', 'D', 0,
  /* 11490 */ 'M', 'I', 'N', 'I', '_', 'U', '_', 'D', 0,
  /* 11499 */ 'C', 'L', 'T', 'I', '_', 'U', '_', 'D', 0,
  /* 11508 */ 'M', 'A', 'X', 'I', '_', 'U', '_', 'D', 0,
  /* 11517 */ 'M', 'I', 'N', '_', 'U', '_', 'D', 0,
  /* 11525 */ 'D', 'O', 'T', 'P', '_', 'U', '_', 'D', 0,
  /* 11534 */ 'A', 'V', 'E', 'R', '_', 'U', '_', 'D', 0,
  /* 11543 */ 'S', 'U', 'B', 'S', '_', 'U', '_', 'D', 0,
  /* 11552 */ 'A', 'D', 'D', 'S', '_', 'U', '_', 'D', 0,
  /* 11561 */ 'S', 'U', 'B', 'S', 'U', 'S', '_', 'U', '_', 'D', 0,
  /* 11572 */ 'S', 'A', 'T', '_', 'U', '_', 'D', 0,
  /* 11580 */ 'C', 'L', 'T', '_', 'U', '_', 'D', 0,
  /* 11588 */ 'F', 'F', 'I', 'N', 'T', '_', 'U', '_', 'D', 0,
  /* 11598 */ 'F', 'T', 'I', 'N', 'T', '_', 'U', '_', 'D', 0,
  /* 11608 */ 'D', 'I', 'V', '_', 'U', '_', 'D', 0,
  /* 11616 */ 'M', 'A', 'X', '_', 'U', '_', 'D', 0,
  /* 11624 */ 'M', 'S', 'U', 'B', 'V', '_', 'D', 0,
  /* 11632 */ 'M', 'A', 'D', 'D', 'V', '_', 'D', 0,
  /* 11640 */ 'P', 'C', 'K', 'E', 'V', '_', 'D', 0,
  /* 11648 */ 'I', 'L', 'V', 'E', 'V', '_', 'D', 0,
  /* 11656 */ 'F', 'D', 'I', 'V', '_', 'D', 0,
  /* 11663 */ 'M', 'U', 'L', 'V', '_', 'D', 0,
  /* 11670 */ 'P', 's', 'e', 'u', 'd', 'o', 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'D', 0,
  /* 11686 */ 'F', 'M', 'A', 'X', '_', 'D', 0,
  /* 11693 */ 'B', 'Z', '_', 'D', 0,
  /* 11698 */ 'S', 'E', 'L', 'N', 'E', 'Z', '_', 'D', 0,
  /* 11707 */ 'B', 'N', 'Z', '_', 'D', 0,
  /* 11713 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '_', 'D', 0,
  /* 11722 */ 'L', 'B', 'E', 0,
  /* 11726 */ 'S', 'B', 'E', 0,
  /* 11730 */ 'G', '_', 'U', 'S', 'U', 'B', 'E', 0,
  /* 11738 */ 'R', 'E', 'G', '_', 'S', 'E', 'Q', 'U', 'E', 'N', 'C', 'E', 0,
  /* 11751 */ 'S', 'C', 'E', 0,
  /* 11755 */ 'G', '_', 'U', 'A', 'D', 'D', 'E', 0,
  /* 11763 */ 'C', 'A', 'C', 'H', 'E', 'E', 0,
  /* 11770 */ 'P', 'R', 'E', 'F', 'E', 0,
  /* 11776 */ 'B', 'G', 'E', 0,
  /* 11780 */ 'T', 'G', 'E', 0,
  /* 11784 */ 'C', 'A', 'C', 'H', 'E', 0,
  /* 11790 */ 'L', 'H', 'E', 0,
  /* 11794 */ 'S', 'H', 'E', 0,
  /* 11798 */ 'B', 'L', 'E', 0,
  /* 11802 */ 'B', 'U', 'N', 'D', 'L', 'E', 0,
  /* 11809 */ 'L', 'L', 'E', 0,
  /* 11813 */ 'L', 'W', 'L', 'E', 0,
  /* 11818 */ 'S', 'W', 'L', 'E', 0,
  /* 11823 */ 'B', 'N', 'E', 0,
  /* 11827 */ 'S', 'N', 'E', 0,
  /* 11831 */ 'T', 'N', 'E', 0,
  /* 11835 */ 'L', 'O', 'C', 'A', 'L', '_', 'E', 'S', 'C', 'A', 'P', 'E', 0,
  /* 11848 */ 'D', 'V', 'P', 'E', 0,
  /* 11853 */ 'E', 'V', 'P', 'E', 0,
  /* 11858 */ 'G', '_', 'S', 'T', 'O', 'R', 'E', 0,
  /* 11866 */ 'L', 'W', 'R', 'E', 0,
  /* 11871 */ 'S', 'W', 'R', 'E', 0,
  /* 11876 */ 'P', 'A', 'U', 'S', 'E', 0,
  /* 11882 */ 'D', 'B', 'G', '_', 'V', 'A', 'L', 'U', 'E', 0,
  /* 11892 */ 'G', '_', 'G', 'L', 'O', 'B', 'A', 'L', '_', 'V', 'A', 'L', 'U', 'E', 0,
  /* 11907 */ 'L', 'W', 'E', 0,
  /* 11911 */ 'S', 'W', 'E', 0,
  /* 11915 */ 'L', 'B', 'u', 'E', 0,
  /* 11920 */ 'L', 'H', 'u', 'E', 0,
  /* 11925 */ 'B', 'C', '1', 'F', 0,
  /* 11930 */ 'G', '_', 'I', 'M', 'P', 'L', 'I', 'C', 'I', 'T', '_', 'D', 'E', 'F', 0,
  /* 11945 */ 'P', 'R', 'E', 'F', 0,
  /* 11950 */ 'T', 'L', 'B', 'I', 'N', 'V', 'F', 0,
  /* 11958 */ 'T', 'L', 'B', 'G', 'I', 'N', 'V', 'F', 0,
  /* 11967 */ 'G', '_', 'F', 'N', 'E', 'G', 0,
  /* 11974 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'H', 'B', '6', '4', 'R', '6', 'R', 'E', 'G', 0,
  /* 11992 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', '6', '4', 'R', '6', 'R', 'E', 'G', 0,
  /* 12008 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'H', 'B', 'R', '6', 'R', 'E', 'G', 0,
  /* 12024 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', '6', 'R', 'E', 'G', 0,
  /* 12038 */ 'E', 'X', 'T', 'R', 'A', 'C', 'T', '_', 'S', 'U', 'B', 'R', 'E', 'G', 0,
  /* 12053 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'S', 'U', 'B', 'R', 'E', 'G', 0,
  /* 12067 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', 0,
  /* 12079 */ 'S', 'U', 'B', 'R', 'E', 'G', '_', 'T', 'O', '_', 'R', 'E', 'G', 0,
  /* 12093 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', 'X', 'C', 'H', 'G', 0,
  /* 12110 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'X', 'C', 'H', 'G', 0,
  /* 12127 */ 'G', '_', 'F', 'L', 'O', 'G', 0,
  /* 12134 */ 'G', '_', 'V', 'A', 'A', 'R', 'G', 0,
  /* 12142 */ 'C', 'R', 'C', '3', '2', 'H', 0,
  /* 12149 */ 'D', 'S', 'B', 'H', 0,
  /* 12154 */ 'W', 'S', 'B', 'H', 0,
  /* 12159 */ 'C', 'R', 'C', '3', '2', 'C', 'H', 0,
  /* 12167 */ 'S', 'E', 'H', 0,
  /* 12171 */ 'G', '_', 'S', 'M', 'U', 'L', 'H', 0,
  /* 12179 */ 'G', '_', 'U', 'M', 'U', 'L', 'H', 0,
  /* 12187 */ 'S', 'H', 'R', 'A', '_', 'P', 'H', 0,
  /* 12195 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'Q', 'B', '_', 'P', 'H', 0,
  /* 12208 */ 'P', 'R', 'E', 'C', 'R', '_', 'Q', 'B', '_', 'P', 'H', 0,
  /* 12220 */ 'P', 'R', 'E', 'C', 'R', 'Q', 'U', '_', 'S', '_', 'Q', 'B', '_', 'P', 'H', 0,
  /* 12236 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', '_', 'L', 'E', '_', 'P', 'H', 0,
  /* 12252 */ 'S', 'U', 'B', 'Q', 'H', '_', 'P', 'H', 0,
  /* 12261 */ 'A', 'D', 'D', 'Q', 'H', '_', 'P', 'H', 0,
  /* 12270 */ 'P', 's', 'e', 'u', 'd', 'o', 'P', 'I', 'C', 'K', '_', 'P', 'H', 0,
  /* 12284 */ 'S', 'H', 'L', 'L', '_', 'P', 'H', 0,
  /* 12292 */ 'R', 'E', 'P', 'L', '_', 'P', 'H', 0,
  /* 12300 */ 'S', 'H', 'R', 'L', '_', 'P', 'H', 0,
  /* 12308 */ 'P', 'A', 'C', 'K', 'R', 'L', '_', 'P', 'H', 0,
  /* 12318 */ 'M', 'U', 'L', '_', 'P', 'H', 0,
  /* 12325 */ 'S', 'U', 'B', 'Q', '_', 'P', 'H', 0,
  /* 12333 */ 'A', 'D', 'D', 'Q', '_', 'P', 'H', 0,
  /* 12341 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', '_', 'E', 'Q', '_', 'P', 'H', 0,
  /* 12357 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'P', 'H', 0,
  /* 12367 */ 'S', 'U', 'B', 'Q', 'H', '_', 'R', '_', 'P', 'H', 0,
  /* 12378 */ 'A', 'D', 'D', 'Q', 'H', '_', 'R', '_', 'P', 'H', 0,
  /* 12389 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'P', 'H', 0,
  /* 12400 */ 'M', 'U', 'L', 'Q', '_', 'R', 'S', '_', 'P', 'H', 0,
  /* 12411 */ 'S', 'H', 'L', 'L', '_', 'S', '_', 'P', 'H', 0,
  /* 12421 */ 'M', 'U', 'L', '_', 'S', '_', 'P', 'H', 0,
  /* 12430 */ 'S', 'U', 'B', 'Q', '_', 'S', '_', 'P', 'H', 0,
  /* 12440 */ 'A', 'D', 'D', 'Q', '_', 'S', '_', 'P', 'H', 0,
  /* 12450 */ 'M', 'U', 'L', 'Q', '_', 'S', '_', 'P', 'H', 0,
  /* 12460 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'P', 'H', 0,
  /* 12470 */ 'S', 'U', 'B', 'U', '_', 'S', '_', 'P', 'H', 0,
  /* 12480 */ 'A', 'D', 'D', 'U', '_', 'S', '_', 'P', 'H', 0,
  /* 12490 */ 'S', 'H', 'L', 'L', 'V', '_', 'S', '_', 'P', 'H', 0,
  /* 12501 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'M', 'P', '_', 'L', 'T', '_', 'P', 'H', 0,
  /* 12517 */ 'S', 'U', 'B', 'U', '_', 'P', 'H', 0,
  /* 12525 */ 'A', 'D', 'D', 'U', '_', 'P', 'H', 0,
  /* 12533 */ 'S', 'H', 'R', 'A', 'V', '_', 'P', 'H', 0,
  /* 12542 */ 'S', 'H', 'L', 'L', 'V', '_', 'P', 'H', 0,
  /* 12551 */ 'R', 'E', 'P', 'L', 'V', '_', 'P', 'H', 0,
  /* 12560 */ 'S', 'H', 'R', 'L', 'V', '_', 'P', 'H', 0,
  /* 12569 */ 'D', 'P', 'A', '_', 'W', '_', 'P', 'H', 0,
  /* 12578 */ 'M', 'U', 'L', 'S', 'A', '_', 'W', '_', 'P', 'H', 0,
  /* 12589 */ 'D', 'P', 'A', 'Q', 'X', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 0,
  /* 12603 */ 'D', 'P', 'S', 'Q', 'X', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 0,
  /* 12617 */ 'D', 'P', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12626 */ 'D', 'P', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12638 */ 'M', 'U', 'L', 'S', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12652 */ 'D', 'P', 'S', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12664 */ 'D', 'P', 'A', 'Q', 'X', '_', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12677 */ 'D', 'P', 'S', 'Q', 'X', '_', 'S', '_', 'W', '_', 'P', 'H', 0,
  /* 12690 */ 'D', 'P', 'A', 'X', '_', 'W', '_', 'P', 'H', 0,
  /* 12700 */ 'D', 'P', 'S', 'X', '_', 'W', '_', 'P', 'H', 0,
  /* 12710 */ 'S', 'H', 0,
  /* 12713 */ 'D', 'M', 'U', 'H', 0,
  /* 12718 */ 'S', 'R', 'A', '_', 'H', 0,
  /* 12724 */ 'A', 'D', 'D', '_', 'A', '_', 'H', 0,
  /* 12732 */ 'M', 'I', 'N', '_', 'A', '_', 'H', 0,
  /* 12740 */ 'A', 'D', 'D', 'S', '_', 'A', '_', 'H', 0,
  /* 12749 */ 'M', 'A', 'X', '_', 'A', '_', 'H', 0,
  /* 12757 */ 'N', 'L', 'O', 'C', '_', 'H', 0,
  /* 12764 */ 'N', 'L', 'Z', 'C', '_', 'H', 0,
  /* 12771 */ 'S', 'L', 'D', '_', 'H', 0,
  /* 12777 */ 'P', 'C', 'K', 'O', 'D', '_', 'H', 0,
  /* 12785 */ 'I', 'L', 'V', 'O', 'D', '_', 'H', 0,
  /* 12793 */ 'I', 'N', 'S', 'V', 'E', '_', 'H', 0,
  /* 12801 */ 'V', 'S', 'H', 'F', '_', 'H', 0,
  /* 12808 */ 'B', 'N', 'E', 'G', '_', 'H', 0,
  /* 12815 */ 'S', 'R', 'A', 'I', '_', 'H', 0,
  /* 12822 */ 'S', 'L', 'D', 'I', '_', 'H', 0,
  /* 12829 */ 'B', 'N', 'E', 'G', 'I', '_', 'H', 0,
  /* 12837 */ 'S', 'L', 'L', 'I', '_', 'H', 0,
  /* 12844 */ 'S', 'R', 'L', 'I', '_', 'H', 0,
  /* 12851 */ 'B', 'I', 'N', 'S', 'L', 'I', '_', 'H', 0,
  /* 12860 */ 'C', 'E', 'Q', 'I', '_', 'H', 0,
  /* 12867 */ 'S', 'R', 'A', 'R', 'I', '_', 'H', 0,
  /* 12875 */ 'B', 'C', 'L', 'R', 'I', '_', 'H', 0,
  /* 12883 */ 'S', 'R', 'L', 'R', 'I', '_', 'H', 0,
  /* 12891 */ 'B', 'I', 'N', 'S', 'R', 'I', '_', 'H', 0,
  /* 12900 */ 'S', 'P', 'L', 'A', 'T', 'I', '_', 'H', 0,
  /* 12909 */ 'B', 'S', 'E', 'T', 'I', '_', 'H', 0,
  /* 12917 */ 'S', 'U', 'B', 'V', 'I', '_', 'H', 0,
  /* 12925 */ 'A', 'D', 'D', 'V', 'I', '_', 'H', 0,
  /* 12933 */ 'F', 'I', 'L', 'L', '_', 'H', 0,
  /* 12940 */ 'S', 'L', 'L', '_', 'H', 0,
  /* 12946 */ 'S', 'R', 'L', '_', 'H', 0,
  /* 12952 */ 'B', 'I', 'N', 'S', 'L', '_', 'H', 0,
  /* 12960 */ 'I', 'L', 'V', 'L', '_', 'H', 0,
  /* 12967 */ 'F', 'E', 'X', 'D', 'O', '_', 'H', 0,
  /* 12975 */ 'C', 'E', 'Q', '_', 'H', 0,
  /* 12981 */ 'F', 'T', 'Q', '_', 'H', 0,
  /* 12987 */ 'M', 'S', 'U', 'B', '_', 'Q', '_', 'H', 0,
  /* 12996 */ 'M', 'A', 'D', 'D', '_', 'Q', '_', 'H', 0,
  /* 13005 */ 'M', 'U', 'L', '_', 'Q', '_', 'H', 0,
  /* 13013 */ 'M', 'S', 'U', 'B', 'R', '_', 'Q', '_', 'H', 0,
  /* 13023 */ 'M', 'A', 'D', 'D', 'R', '_', 'Q', '_', 'H', 0,
  /* 13033 */ 'M', 'U', 'L', 'R', '_', 'Q', '_', 'H', 0,
  /* 13042 */ 'S', 'R', 'A', 'R', '_', 'H', 0,
  /* 13049 */ 'B', 'C', 'L', 'R', '_', 'H', 0,
  /* 13056 */ 'S', 'R', 'L', 'R', '_', 'H', 0,
  /* 13063 */ 'B', 'I', 'N', 'S', 'R', '_', 'H', 0,
  /* 13071 */ 'I', 'L', 'V', 'R', '_', 'H', 0,
  /* 13078 */ 'A', 'S', 'U', 'B', '_', 'S', '_', 'H', 0,
  /* 13087 */ 'H', 'S', 'U', 'B', '_', 'S', '_', 'H', 0,
  /* 13096 */ 'D', 'P', 'S', 'U', 'B', '_', 'S', '_', 'H', 0,
  /* 13106 */ 'H', 'A', 'D', 'D', '_', 'S', '_', 'H', 0,
  /* 13115 */ 'D', 'P', 'A', 'D', 'D', '_', 'S', '_', 'H', 0,
  /* 13125 */ 'M', 'O', 'D', '_', 'S', '_', 'H', 0,
  /* 13133 */ 'C', 'L', 'E', '_', 'S', '_', 'H', 0,
  /* 13141 */ 'A', 'V', 'E', '_', 'S', '_', 'H', 0,
  /* 13149 */ 'C', 'L', 'E', 'I', '_', 'S', '_', 'H', 0,
  /* 13158 */ 'M', 'I', 'N', 'I', '_', 'S', '_', 'H', 0,
  /* 13167 */ 'C', 'L', 'T', 'I', '_', 'S', '_', 'H', 0,
  /* 13176 */ 'M', 'A', 'X', 'I', '_', 'S', '_', 'H', 0,
  /* 13185 */ 'M', 'I', 'N', '_', 'S', '_', 'H', 0,
  /* 13193 */ 'D', 'O', 'T', 'P', '_', 'S', '_', 'H', 0,
  /* 13202 */ 'A', 'V', 'E', 'R', '_', 'S', '_', 'H', 0,
  /* 13211 */ 'E', 'X', 'T', 'R', '_', 'S', '_', 'H', 0,
  /* 13220 */ 'S', 'U', 'B', 'S', '_', 'S', '_', 'H', 0,
  /* 13229 */ 'A', 'D', 'D', 'S', '_', 'S', '_', 'H', 0,
  /* 13238 */ 'S', 'A', 'T', '_', 'S', '_', 'H', 0,
  /* 13246 */ 'C', 'L', 'T', '_', 'S', '_', 'H', 0,
  /* 13254 */ 'S', 'U', 'B', 'S', 'U', 'U', '_', 'S', '_', 'H', 0,
  /* 13265 */ 'D', 'I', 'V', '_', 'S', '_', 'H', 0,
  /* 13273 */ 'E', 'X', 'T', 'R', 'V', '_', 'S', '_', 'H', 0,
  /* 13283 */ 'M', 'A', 'X', '_', 'S', '_', 'H', 0,
  /* 13291 */ 'C', 'O', 'P', 'Y', '_', 'S', '_', 'H', 0,
  /* 13300 */ 'S', 'P', 'L', 'A', 'T', '_', 'H', 0,
  /* 13308 */ 'B', 'S', 'E', 'T', '_', 'H', 0,
  /* 13315 */ 'P', 'C', 'N', 'T', '_', 'H', 0,
  /* 13322 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'H', 0,
  /* 13331 */ 'S', 'T', '_', 'H', 0,
  /* 13336 */ 'A', 'S', 'U', 'B', '_', 'U', '_', 'H', 0,
  /* 13345 */ 'H', 'S', 'U', 'B', '_', 'U', '_', 'H', 0,
  /* 13354 */ 'D', 'P', 'S', 'U', 'B', '_', 'U', '_', 'H', 0,
  /* 13364 */ 'H', 'A', 'D', 'D', '_', 'U', '_', 'H', 0,
  /* 13373 */ 'D', 'P', 'A', 'D', 'D', '_', 'U', '_', 'H', 0,
  /* 13383 */ 'M', 'O', 'D', '_', 'U', '_', 'H', 0,
  /* 13391 */ 'C', 'L', 'E', '_', 'U', '_', 'H', 0,
  /* 13399 */ 'A', 'V', 'E', '_', 'U', '_', 'H', 0,
  /* 13407 */ 'C', 'L', 'E', 'I', '_', 'U', '_', 'H', 0,
  /* 13416 */ 'M', 'I', 'N', 'I', '_', 'U', '_', 'H', 0,
  /* 13425 */ 'C', 'L', 'T', 'I', '_', 'U', '_', 'H', 0,
  /* 13434 */ 'M', 'A', 'X', 'I', '_', 'U', '_', 'H', 0,
  /* 13443 */ 'M', 'I', 'N', '_', 'U', '_', 'H', 0,
  /* 13451 */ 'D', 'O', 'T', 'P', '_', 'U', '_', 'H', 0,
  /* 13460 */ 'A', 'V', 'E', 'R', '_', 'U', '_', 'H', 0,
  /* 13469 */ 'S', 'U', 'B', 'S', '_', 'U', '_', 'H', 0,
  /* 13478 */ 'A', 'D', 'D', 'S', '_', 'U', '_', 'H', 0,
  /* 13487 */ 'S', 'U', 'B', 'S', 'U', 'S', '_', 'U', '_', 'H', 0,
  /* 13498 */ 'S', 'A', 'T', '_', 'U', '_', 'H', 0,
  /* 13506 */ 'C', 'L', 'T', '_', 'U', '_', 'H', 0,
  /* 13514 */ 'D', 'I', 'V', '_', 'U', '_', 'H', 0,
  /* 13522 */ 'M', 'A', 'X', '_', 'U', '_', 'H', 0,
  /* 13530 */ 'C', 'O', 'P', 'Y', '_', 'U', '_', 'H', 0,
  /* 13539 */ 'M', 'S', 'U', 'B', 'V', '_', 'H', 0,
  /* 13547 */ 'M', 'A', 'D', 'D', 'V', '_', 'H', 0,
  /* 13555 */ 'P', 'C', 'K', 'E', 'V', '_', 'H', 0,
  /* 13563 */ 'I', 'L', 'V', 'E', 'V', '_', 'H', 0,
  /* 13571 */ 'M', 'U', 'L', 'V', '_', 'H', 0,
  /* 13578 */ 'B', 'Z', '_', 'H', 0,
  /* 13583 */ 'B', 'N', 'Z', '_', 'H', 0,
  /* 13589 */ 'S', 'Y', 'N', 'C', 'I', 0,
  /* 13595 */ 'D', 'I', 0,
  /* 13598 */ 'T', 'G', 'E', 'I', 0,
  /* 13603 */ 'T', 'N', 'E', 'I', 0,
  /* 13608 */ 'D', 'A', 'H', 'I', 0,
  /* 13613 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'F', 'H', 'I', 0,
  /* 13624 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'T', 'L', 'O', 'H', 'I', 0,
  /* 13637 */ 'G', '_', 'P', 'H', 'I', 0,
  /* 13643 */ 'M', 'F', 'T', 'H', 'I', 0,
  /* 13649 */ 'M', 'T', 'H', 'I', 0,
  /* 13654 */ 'M', 'T', 'T', 'H', 'I', 0,
  /* 13660 */ 'T', 'E', 'Q', 'I', 0,
  /* 13665 */ 'G', '_', 'F', 'P', 'T', 'O', 'S', 'I', 0,
  /* 13674 */ 'D', 'A', 'T', 'I', 0,
  /* 13679 */ 'T', 'L', 'T', 'I', 0,
  /* 13684 */ 'D', 'A', 'U', 'I', 0,
  /* 13689 */ 'G', '_', 'F', 'P', 'T', 'O', 'U', 'I', 0,
  /* 13698 */ 'G', 'I', 'N', 'V', 'I', 0,
  /* 13704 */ 'T', 'L', 'B', 'W', 'I', 0,
  /* 13710 */ 'T', 'L', 'B', 'G', 'W', 'I', 0,
  /* 13717 */ 'M', 'O', 'V', 'N', '_', 'I', '6', '4', '_', 'I', 0,
  /* 13728 */ 'M', 'O', 'V', 'Z', '_', 'I', '6', '4', '_', 'I', 0,
  /* 13739 */ 'M', 'O', 'V', 'F', '_', 'I', 0,
  /* 13746 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'F', '_', 'I', 0,
  /* 13765 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'I', 0,
  /* 13774 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'I', 0,
  /* 13783 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', '_', 'I', 0,
  /* 13798 */ 'M', 'O', 'V', 'T', '_', 'I', 0,
  /* 13805 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'T', '_', 'I', 0,
  /* 13824 */ 'J', 0,
  /* 13826 */ 'B', 'R', 'E', 'A', 'K', 0,
  /* 13832 */ 'F', 'O', 'R', 'K', 0,
  /* 13837 */ 'G', '_', 'P', 'T', 'R', '_', 'M', 'A', 'S', 'K', 0,
  /* 13848 */ 'B', 'A', 'L', 0,
  /* 13852 */ 'J', 'A', 'L', 0,
  /* 13856 */ 'B', 'G', 'E', 'Z', 'A', 'L', 0,
  /* 13863 */ 'B', 'L', 'T', 'Z', 'A', 'L', 0,
  /* 13870 */ 'M', 'U', 'L', 'E', 'U', '_', 'S', '_', 'P', 'H', '_', 'Q', 'B', 'L', 0,
  /* 13885 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 0,
  /* 13899 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 0,
  /* 13914 */ 'D', 'P', 'A', 'U', '_', 'H', '_', 'Q', 'B', 'L', 0,
  /* 13925 */ 'D', 'P', 'S', 'U', '_', 'H', '_', 'Q', 'B', 'L', 0,
  /* 13936 */ 'L', 'D', 'L', 0,
  /* 13940 */ 'S', 'D', 'L', 0,
  /* 13944 */ 'G', 'C', '_', 'L', 'A', 'B', 'E', 'L', 0,
  /* 13953 */ 'D', 'B', 'G', '_', 'L', 'A', 'B', 'E', 'L', 0,
  /* 13963 */ 'E', 'H', '_', 'L', 'A', 'B', 'E', 'L', 0,
  /* 13972 */ 'A', 'N', 'N', 'O', 'T', 'A', 'T', 'I', 'O', 'N', '_', 'L', 'A', 'B', 'E', 'L', 0,
  /* 13989 */ 'B', 'G', 'E', 'L', 0,
  /* 13994 */ 'B', 'L', 'E', 'L', 0,
  /* 13999 */ 'B', 'N', 'E', 'L', 0,
  /* 14004 */ 'I', 'C', 'A', 'L', 'L', '_', 'B', 'R', 'A', 'N', 'C', 'H', '_', 'F', 'U', 'N', 'N', 'E', 'L', 0,
  /* 14024 */ 'B', 'C', '1', 'F', 'L', 0,
  /* 14030 */ 'M', 'A', 'Q', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 'L', 0,
  /* 14043 */ 'P', 'R', 'E', 'C', 'E', 'Q', '_', 'W', '_', 'P', 'H', 'L', 0,
  /* 14056 */ 'M', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'L', 0,
  /* 14068 */ 'M', 'U', 'L', 'E', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'L', 0,
  /* 14082 */ 'G', '_', 'S', 'H', 'L', 0,
  /* 14088 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 0,
  /* 14097 */ 'H', 'Y', 'P', 'C', 'A', 'L', 'L', 0,
  /* 14105 */ 'S', 'Y', 'S', 'C', 'A', 'L', 'L', 0,
  /* 14113 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'T', 'A', 'I', 'L', '_', 'C', 'A', 'L', 'L', 0,
  /* 14133 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'T', 'Y', 'P', 'E', 'D', '_', 'E', 'V', 'E', 'N', 'T', '_', 'C', 'A', 'L', 'L', 0,
  /* 14160 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'E', 'V', 'E', 'N', 'T', '_', 'C', 'A', 'L', 'L', 0,
  /* 14181 */ 'F', 'E', 'N', 'T', 'R', 'Y', '_', 'C', 'A', 'L', 'L', 0,
  /* 14193 */ 'B', 'G', 'E', 'Z', 'A', 'L', 'L', 0,
  /* 14201 */ 'B', 'L', 'T', 'Z', 'A', 'L', 'L', 0,
  /* 14209 */ 'K', 'I', 'L', 'L', 0,
  /* 14214 */ 'D', 'S', 'L', 'L', 0,
  /* 14219 */ 'D', 'R', 'O', 'L', 0,
  /* 14224 */ 'B', 'E', 'Q', 'L', 0,
  /* 14229 */ 'D', 'S', 'R', 'L', 0,
  /* 14234 */ 'B', 'C', '1', 'T', 'L', 0,
  /* 14240 */ 'B', 'G', 'T', 'L', 0,
  /* 14245 */ 'B', 'L', 'T', 'L', 0,
  /* 14250 */ 'B', 'G', 'E', 'U', 'L', 0,
  /* 14256 */ 'B', 'L', 'E', 'U', 'L', 0,
  /* 14262 */ 'D', 'M', 'U', 'L', 0,
  /* 14267 */ 'G', '_', 'F', 'M', 'U', 'L', 0,
  /* 14274 */ 'G', '_', 'M', 'U', 'L', 0,
  /* 14280 */ 'B', 'G', 'T', 'U', 'L', 0,
  /* 14286 */ 'B', 'L', 'T', 'U', 'L', 0,
  /* 14292 */ 'L', 'W', 'L', 0,
  /* 14296 */ 'S', 'W', 'L', 0,
  /* 14300 */ 'B', 'G', 'E', 'Z', 'L', 0,
  /* 14306 */ 'B', 'L', 'E', 'Z', 'L', 0,
  /* 14312 */ 'B', 'G', 'T', 'Z', 'L', 0,
  /* 14318 */ 'B', 'L', 'T', 'Z', 'L', 0,
  /* 14324 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'V', 'T', '_', 'D', '6', '4', '_', 'L', 0,
  /* 14340 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'V', 'T', '_', 'S', '_', 'L', 0,
  /* 14354 */ 'G', '_', 'F', 'R', 'E', 'M', 0,
  /* 14361 */ 'G', '_', 'S', 'R', 'E', 'M', 0,
  /* 14368 */ 'G', '_', 'U', 'R', 'E', 'M', 0,
  /* 14375 */ 'M', 'F', 'G', 'C', '0', '_', 'M', 'M', 0,
  /* 14384 */ 'M', 'F', 'H', 'G', 'C', '0', '_', 'M', 'M', 0,
  /* 14394 */ 'M', 'T', 'H', 'G', 'C', '0', '_', 'M', 'M', 0,
  /* 14404 */ 'M', 'T', 'G', 'C', '0', '_', 'M', 'M', 0,
  /* 14413 */ 'L', 'D', 'C', '1', '_', 'M', 'M', 0,
  /* 14421 */ 'S', 'D', 'C', '1', '_', 'M', 'M', 0,
  /* 14429 */ 'C', 'F', 'C', '1', '_', 'M', 'M', 0,
  /* 14437 */ 'M', 'F', 'C', '1', '_', 'M', 'M', 0,
  /* 14445 */ 'C', 'T', 'C', '1', '_', 'M', 'M', 0,
  /* 14453 */ 'M', 'T', 'C', '1', '_', 'M', 'M', 0,
  /* 14461 */ 'L', 'W', 'C', '1', '_', 'M', 'M', 0,
  /* 14469 */ 'S', 'W', 'C', '1', '_', 'M', 'M', 0,
  /* 14477 */ 'L', 'U', 'X', 'C', '1', '_', 'M', 'M', 0,
  /* 14486 */ 'S', 'U', 'X', 'C', '1', '_', 'M', 'M', 0,
  /* 14495 */ 'L', 'W', 'X', 'C', '1', '_', 'M', 'M', 0,
  /* 14504 */ 'S', 'W', 'X', 'C', '1', '_', 'M', 'M', 0,
  /* 14513 */ 'M', 'F', 'H', 'C', '1', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14526 */ 'M', 'T', 'H', 'C', '1', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14539 */ 'F', 'S', 'U', 'B', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14551 */ 'N', 'M', 'S', 'U', 'B', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14564 */ 'F', 'A', 'D', 'D', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14576 */ 'N', 'M', 'A', 'D', 'D', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14589 */ 'C', '_', 'N', 'G', 'E', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14602 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14616 */ 'C', '_', 'O', 'L', 'E', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14629 */ 'C', '_', 'U', 'L', 'E', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14642 */ 'C', '_', 'L', 'E', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14654 */ 'C', '_', 'S', 'F', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14666 */ 'M', 'O', 'V', 'F', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14678 */ 'C', '_', 'F', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14689 */ 'F', 'N', 'E', 'G', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14701 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14715 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14729 */ 'C', '_', 'N', 'G', 'L', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14742 */ 'F', 'M', 'U', 'L', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14754 */ 'C', '_', 'U', 'N', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14766 */ 'R', 'E', 'C', 'I', 'P', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14779 */ 'F', 'C', 'M', 'P', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14791 */ 'C', '_', 'S', 'E', 'Q', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14804 */ 'C', '_', 'U', 'E', 'Q', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14817 */ 'C', '_', 'E', 'Q', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14829 */ 'F', 'A', 'B', 'S', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14841 */ 'C', 'V', 'T', '_', 'S', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14854 */ 'C', '_', 'N', 'G', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14867 */ 'C', '_', 'O', 'L', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14880 */ 'C', '_', 'U', 'L', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14893 */ 'C', '_', 'L', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14905 */ 'F', 'S', 'Q', 'R', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14918 */ 'R', 'S', 'Q', 'R', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14931 */ 'M', 'O', 'V', 'T', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14943 */ 'F', 'D', 'I', 'V', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14955 */ 'F', 'M', 'O', 'V', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14967 */ 'C', 'V', 'T', '_', 'W', '_', 'D', '3', '2', '_', 'M', 'M', 0,
  /* 14980 */ 'B', 'P', 'O', 'S', 'G', 'E', '3', '2', '_', 'M', 'M', 0,
  /* 14992 */ 'L', 'W', 'M', '3', '2', '_', 'M', 'M', 0,
  /* 15001 */ 'S', 'W', 'M', '3', '2', '_', 'M', 'M', 0,
  /* 15010 */ 'F', 'C', 'M', 'P', '_', 'S', '3', '2', '_', 'M', 'M', 0,
  /* 15022 */ 'C', 'F', 'C', '2', '_', 'M', 'M', 0,
  /* 15030 */ 'C', 'T', 'C', '2', '_', 'M', 'M', 0,
  /* 15038 */ 'A', 'D', 'D', 'I', 'U', 'R', '2', '_', 'M', 'M', 0,
  /* 15049 */ 'M', 'F', 'H', 'C', '1', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15062 */ 'M', 'T', 'H', 'C', '1', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15075 */ 'F', 'S', 'U', 'B', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15087 */ 'F', 'A', 'D', 'D', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15099 */ 'C', '_', 'N', 'G', 'E', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15112 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15126 */ 'C', '_', 'O', 'L', 'E', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15139 */ 'C', '_', 'U', 'L', 'E', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15152 */ 'C', '_', 'L', 'E', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15164 */ 'C', '_', 'S', 'F', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15176 */ 'C', '_', 'F', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15187 */ 'F', 'N', 'E', 'G', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15199 */ 'C', '_', 'N', 'G', 'L', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15212 */ 'F', 'M', 'U', 'L', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15224 */ 'C', 'V', 'T', '_', 'L', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15237 */ 'C', '_', 'U', 'N', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15249 */ 'R', 'E', 'C', 'I', 'P', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15262 */ 'C', '_', 'S', 'E', 'Q', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15275 */ 'C', '_', 'U', 'E', 'Q', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15288 */ 'C', '_', 'E', 'Q', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15300 */ 'F', 'A', 'B', 'S', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15312 */ 'C', 'V', 'T', '_', 'S', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15325 */ 'C', '_', 'N', 'G', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15338 */ 'C', '_', 'O', 'L', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15351 */ 'C', '_', 'U', 'L', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15364 */ 'C', '_', 'L', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15376 */ 'F', 'S', 'Q', 'R', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15389 */ 'R', 'S', 'Q', 'R', 'T', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15402 */ 'F', 'D', 'I', 'V', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15414 */ 'F', 'M', 'O', 'V', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15426 */ 'C', 'V', 'T', '_', 'W', '_', 'D', '6', '4', '_', 'M', 'M', 0,
  /* 15439 */ 'A', 'D', 'D', 'I', 'U', 'S', '5', '_', 'M', 'M', 0,
  /* 15450 */ 'S', 'B', '1', '6', '_', 'M', 'M', 0,
  /* 15458 */ 'J', 'R', 'C', '1', '6', '_', 'M', 'M', 0,
  /* 15467 */ 'A', 'N', 'D', '1', '6', '_', 'M', 'M', 0,
  /* 15476 */ 'M', 'O', 'V', 'E', '1', '6', '_', 'M', 'M', 0,
  /* 15486 */ 'S', 'H', '1', '6', '_', 'M', 'M', 0,
  /* 15494 */ 'A', 'N', 'D', 'I', '1', '6', '_', 'M', 'M', 0,
  /* 15504 */ 'M', 'F', 'H', 'I', '1', '6', '_', 'M', 'M', 0,
  /* 15514 */ 'L', 'I', '1', '6', '_', 'M', 'M', 0,
  /* 15522 */ 'B', 'R', 'E', 'A', 'K', '1', '6', '_', 'M', 'M', 0,
  /* 15533 */ 'S', 'L', 'L', '1', '6', '_', 'M', 'M', 0,
  /* 15542 */ 'S', 'R', 'L', '1', '6', '_', 'M', 'M', 0,
  /* 15551 */ 'L', 'W', 'M', '1', '6', '_', 'M', 'M', 0,
  /* 15560 */ 'S', 'W', 'M', '1', '6', '_', 'M', 'M', 0,
  /* 15569 */ 'M', 'F', 'L', 'O', '1', '6', '_', 'M', 'M', 0,
  /* 15579 */ 'S', 'D', 'B', 'B', 'P', '1', '6', '_', 'M', 'M', 0,
  /* 15590 */ 'J', 'R', '1', '6', '_', 'M', 'M', 0,
  /* 15598 */ 'J', 'A', 'L', 'R', '1', '6', '_', 'M', 'M', 0,
  /* 15608 */ 'X', 'O', 'R', '1', '6', '_', 'M', 'M', 0,
  /* 15617 */ 'J', 'A', 'L', 'R', 'S', '1', '6', '_', 'M', 'M', 0,
  /* 15628 */ 'N', 'O', 'T', '1', '6', '_', 'M', 'M', 0,
  /* 15637 */ 'L', 'B', 'U', '1', '6', '_', 'M', 'M', 0,
  /* 15646 */ 'S', 'U', 'B', 'U', '1', '6', '_', 'M', 'M', 0,
  /* 15656 */ 'A', 'D', 'D', 'U', '1', '6', '_', 'M', 'M', 0,
  /* 15666 */ 'L', 'H', 'U', '1', '6', '_', 'M', 'M', 0,
  /* 15675 */ 'L', 'W', '1', '6', '_', 'M', 'M', 0,
  /* 15683 */ 'S', 'W', '1', '6', '_', 'M', 'M', 0,
  /* 15691 */ 'B', 'N', 'E', 'Z', '1', '6', '_', 'M', 'M', 0,
  /* 15701 */ 'B', 'E', 'Q', 'Z', '1', '6', '_', 'M', 'M', 0,
  /* 15711 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 'A', '_', 'M', 'M', 0,
  /* 15729 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', 'A', '_', 'M', 'M', 0,
  /* 15748 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 'A', '_', 'M', 'M', 0,
  /* 15766 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 'A', '_', 'M', 'M', 0,
  /* 15785 */ 'S', 'R', 'A', '_', 'M', 'M', 0,
  /* 15792 */ 'S', 'E', 'B', '_', 'M', 'M', 0,
  /* 15799 */ 'E', 'H', 'B', '_', 'M', 'M', 0,
  /* 15806 */ 'L', 'B', '_', 'M', 'M', 0,
  /* 15812 */ 'C', 'M', 'P', 'G', 'U', '_', 'L', 'E', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15827 */ 'C', 'M', 'P', 'U', '_', 'L', 'E', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15841 */ 'P', 'I', 'C', 'K', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15852 */ 'S', 'H', 'L', 'L', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15863 */ 'R', 'E', 'P', 'L', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15874 */ 'S', 'H', 'R', 'L', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15885 */ 'C', 'M', 'P', 'G', 'U', '_', 'E', 'Q', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15900 */ 'C', 'M', 'P', 'U', '_', 'E', 'Q', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15914 */ 'S', 'U', 'B', 'U', '_', 'S', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15927 */ 'A', 'D', 'D', 'U', '_', 'S', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15940 */ 'C', 'M', 'P', 'G', 'U', '_', 'L', 'T', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15955 */ 'C', 'M', 'P', 'U', '_', 'L', 'T', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15969 */ 'S', 'U', 'B', 'U', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15980 */ 'A', 'D', 'D', 'U', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 15991 */ 'S', 'H', 'L', 'L', 'V', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 16003 */ 'R', 'E', 'P', 'L', 'V', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 16015 */ 'S', 'H', 'R', 'L', 'V', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 16027 */ 'R', 'A', 'D', 'D', 'U', '_', 'W', '_', 'Q', 'B', '_', 'M', 'M', 0,
  /* 16041 */ 'S', 'B', '_', 'M', 'M', 0,
  /* 16047 */ 'M', 'O', 'D', 'S', 'U', 'B', '_', 'M', 'M', 0,
  /* 16057 */ 'M', 'S', 'U', 'B', '_', 'M', 'M', 0,
  /* 16065 */ 'S', 'Y', 'N', 'C', '_', 'M', 'M', 0,
  /* 16073 */ 'A', 'D', 'D', 'I', 'U', 'P', 'C', '_', 'M', 'M', 0,
  /* 16084 */ 'A', 'D', 'D', 'S', 'C', '_', 'M', 'M', 0,
  /* 16093 */ 'A', 'D', 'D', 'W', 'C', '_', 'M', 'M', 0,
  /* 16102 */ 'B', 'N', 'E', 'Z', 'C', '_', 'M', 'M', 0,
  /* 16111 */ 'B', 'E', 'Q', 'Z', 'C', '_', 'M', 'M', 0,
  /* 16120 */ 'M', 'A', 'D', 'D', '_', 'M', 'M', 0,
  /* 16128 */ 'A', 'N', 'D', '_', 'M', 'M', 0,
  /* 16135 */ 'L', 'B', 'E', '_', 'M', 'M', 0,
  /* 16142 */ 'S', 'B', 'E', '_', 'M', 'M', 0,
  /* 16149 */ 'S', 'C', 'E', '_', 'M', 'M', 0,
  /* 16156 */ 'C', 'A', 'C', 'H', 'E', 'E', '_', 'M', 'M', 0,
  /* 16166 */ 'P', 'R', 'E', 'F', 'E', '_', 'M', 'M', 0,
  /* 16175 */ 'T', 'G', 'E', '_', 'M', 'M', 0,
  /* 16182 */ 'C', 'A', 'C', 'H', 'E', '_', 'M', 'M', 0,
  /* 16191 */ 'L', 'H', 'E', '_', 'M', 'M', 0,
  /* 16198 */ 'S', 'H', 'E', '_', 'M', 'M', 0,
  /* 16205 */ 'L', 'L', 'E', '_', 'M', 'M', 0,
  /* 16212 */ 'L', 'W', 'L', 'E', '_', 'M', 'M', 0,
  /* 16220 */ 'S', 'W', 'L', 'E', '_', 'M', 'M', 0,
  /* 16228 */ 'B', 'N', 'E', '_', 'M', 'M', 0,
  /* 16235 */ 'T', 'N', 'E', '_', 'M', 'M', 0,
  /* 16242 */ 'L', 'W', 'R', 'E', '_', 'M', 'M', 0,
  /* 16250 */ 'S', 'W', 'R', 'E', '_', 'M', 'M', 0,
  /* 16258 */ 'P', 'A', 'U', 'S', 'E', '_', 'M', 'M', 0,
  /* 16267 */ 'L', 'W', 'E', '_', 'M', 'M', 0,
  /* 16274 */ 'S', 'W', 'E', '_', 'M', 'M', 0,
  /* 16281 */ 'L', 'B', 'u', 'E', '_', 'M', 'M', 0,
  /* 16289 */ 'L', 'H', 'u', 'E', '_', 'M', 'M', 0,
  /* 16297 */ 'B', 'C', '1', 'F', '_', 'M', 'M', 0,
  /* 16305 */ 'P', 'R', 'E', 'F', '_', 'M', 'M', 0,
  /* 16313 */ 'T', 'L', 'B', 'G', 'I', 'N', 'V', 'F', '_', 'M', 'M', 0,
  /* 16325 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', 'R', 'E', 'G', '_', 'M', 'M', 0,
  /* 16340 */ 'W', 'S', 'B', 'H', '_', 'M', 'M', 0,
  /* 16348 */ 'S', 'E', 'H', '_', 'M', 'M', 0,
  /* 16355 */ 'L', 'H', '_', 'M', 'M', 0,
  /* 16361 */ 'S', 'H', 'R', 'A', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16372 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'Q', 'B', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16388 */ 'P', 'R', 'E', 'C', 'R', 'Q', 'U', '_', 'S', '_', 'Q', 'B', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16407 */ 'C', 'M', 'P', '_', 'L', 'E', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16420 */ 'P', 'I', 'C', 'K', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16431 */ 'S', 'H', 'L', 'L', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16442 */ 'R', 'E', 'P', 'L', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16453 */ 'P', 'A', 'C', 'K', 'R', 'L', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16466 */ 'S', 'U', 'B', 'Q', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16477 */ 'A', 'D', 'D', 'Q', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16488 */ 'C', 'M', 'P', '_', 'E', 'Q', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16501 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16514 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16528 */ 'M', 'U', 'L', 'Q', '_', 'R', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16542 */ 'S', 'H', 'L', 'L', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16555 */ 'S', 'U', 'B', 'Q', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16568 */ 'A', 'D', 'D', 'Q', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16581 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16594 */ 'S', 'H', 'L', 'L', 'V', '_', 'S', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16608 */ 'C', 'M', 'P', '_', 'L', 'T', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16621 */ 'S', 'H', 'R', 'A', 'V', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16633 */ 'S', 'H', 'L', 'L', 'V', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16645 */ 'R', 'E', 'P', 'L', 'V', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16657 */ 'D', 'P', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16672 */ 'M', 'U', 'L', 'S', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16689 */ 'D', 'P', 'S', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', '_', 'M', 'M', 0,
  /* 16704 */ 'S', 'H', '_', 'M', 'M', 0,
  /* 16710 */ 'E', 'X', 'T', 'R', '_', 'S', '_', 'H', '_', 'M', 'M', 0,
  /* 16722 */ 'E', 'X', 'T', 'R', 'V', '_', 'S', '_', 'H', '_', 'M', 'M', 0,
  /* 16735 */ 'S', 'Y', 'N', 'C', 'I', '_', 'M', 'M', 0,
  /* 16744 */ 'D', 'I', '_', 'M', 'M', 0,
  /* 16750 */ 'T', 'G', 'E', 'I', '_', 'M', 'M', 0,
  /* 16758 */ 'T', 'N', 'E', 'I', '_', 'M', 'M', 0,
  /* 16766 */ 'M', 'F', 'H', 'I', '_', 'M', 'M', 0,
  /* 16774 */ 'M', 'T', 'H', 'I', '_', 'M', 'M', 0,
  /* 16782 */ 'T', 'E', 'Q', 'I', '_', 'M', 'M', 0,
  /* 16790 */ 'T', 'L', 'T', 'I', '_', 'M', 'M', 0,
  /* 16798 */ 'T', 'L', 'B', 'W', 'I', '_', 'M', 'M', 0,
  /* 16807 */ 'T', 'L', 'B', 'G', 'W', 'I', '_', 'M', 'M', 0,
  /* 16817 */ 'M', 'O', 'V', 'F', '_', 'I', '_', 'M', 'M', 0,
  /* 16827 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'M', 'M', 0,
  /* 16837 */ 'M', 'O', 'V', 'T', '_', 'I', '_', 'M', 'M', 0,
  /* 16847 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'M', 'M', 0,
  /* 16857 */ 'J', '_', 'M', 'M', 0,
  /* 16862 */ 'B', 'R', 'E', 'A', 'K', '_', 'M', 'M', 0,
  /* 16871 */ 'J', 'A', 'L', '_', 'M', 'M', 0,
  /* 16878 */ 'B', 'G', 'E', 'Z', 'A', 'L', '_', 'M', 'M', 0,
  /* 16888 */ 'B', 'L', 'T', 'Z', 'A', 'L', '_', 'M', 'M', 0,
  /* 16898 */ 'M', 'U', 'L', 'E', 'U', '_', 'S', '_', 'P', 'H', '_', 'Q', 'B', 'L', '_', 'M', 'M', 0,
  /* 16916 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', '_', 'M', 'M', 0,
  /* 16933 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'L', '_', 'M', 'M', 0,
  /* 16951 */ 'D', 'P', 'A', 'U', '_', 'H', '_', 'Q', 'B', 'L', '_', 'M', 'M', 0,
  /* 16965 */ 'D', 'P', 'S', 'U', '_', 'H', '_', 'Q', 'B', 'L', '_', 'M', 'M', 0,
  /* 16979 */ 'M', 'A', 'Q', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 'L', '_', 'M', 'M', 0,
  /* 16995 */ 'P', 'R', 'E', 'C', 'E', 'Q', '_', 'W', '_', 'P', 'H', 'L', '_', 'M', 'M', 0,
  /* 17011 */ 'M', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'L', '_', 'M', 'M', 0,
  /* 17026 */ 'M', 'U', 'L', 'E', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'L', '_', 'M', 'M', 0,
  /* 17043 */ 'T', 'A', 'I', 'L', 'C', 'A', 'L', 'L', '_', 'M', 'M', 0,
  /* 17055 */ 'H', 'Y', 'P', 'C', 'A', 'L', 'L', '_', 'M', 'M', 0,
  /* 17066 */ 'S', 'Y', 'S', 'C', 'A', 'L', 'L', '_', 'M', 'M', 0,
  /* 17077 */ 'S', 'L', 'L', '_', 'M', 'M', 0,
  /* 17084 */ 'S', 'R', 'L', '_', 'M', 'M', 0,
  /* 17091 */ 'M', 'U', 'L', '_', 'M', 'M', 0,
  /* 17098 */ 'L', 'W', 'L', '_', 'M', 'M', 0,
  /* 17105 */ 'S', 'W', 'L', '_', 'M', 'M', 0,
  /* 17112 */ 'L', 'W', 'M', '_', 'M', 'M', 0,
  /* 17119 */ 'S', 'W', 'M', '_', 'M', 'M', 0,
  /* 17126 */ 'C', 'L', 'O', '_', 'M', 'M', 0,
  /* 17133 */ 'M', 'F', 'L', 'O', '_', 'M', 'M', 0,
  /* 17141 */ 'S', 'H', 'I', 'L', 'O', '_', 'M', 'M', 0,
  /* 17150 */ 'M', 'T', 'L', 'O', '_', 'M', 'M', 0,
  /* 17158 */ 'T', 'R', 'A', 'P', '_', 'M', 'M', 0,
  /* 17166 */ 'S', 'D', 'B', 'B', 'P', '_', 'M', 'M', 0,
  /* 17175 */ 'T', 'L', 'B', 'P', '_', 'M', 'M', 0,
  /* 17183 */ 'E', 'X', 'T', 'P', 'D', 'P', '_', 'M', 'M', 0,
  /* 17193 */ 'M', 'O', 'V', 'E', 'P', '_', 'M', 'M', 0,
  /* 17202 */ 'T', 'L', 'B', 'G', 'P', '_', 'M', 'M', 0,
  /* 17211 */ 'L', 'W', 'G', 'P', '_', 'M', 'M', 0,
  /* 17219 */ 'M', 'T', 'H', 'L', 'I', 'P', '_', 'M', 'M', 0,
  /* 17229 */ 'S', 'S', 'N', 'O', 'P', '_', 'M', 'M', 0,
  /* 17238 */ 'A', 'D', 'D', 'I', 'U', 'R', '1', 'S', 'P', '_', 'M', 'M', 0,
  /* 17251 */ 'R', 'D', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17260 */ 'W', 'R', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17269 */ 'L', 'W', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17278 */ 'S', 'W', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17287 */ 'M', 'S', 'U', 'B', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17299 */ 'M', 'A', 'D', 'D', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17311 */ 'M', 'F', 'H', 'I', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17323 */ 'M', 'T', 'H', 'I', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17335 */ 'M', 'F', 'L', 'O', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17347 */ 'M', 'T', 'L', 'O', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17359 */ 'M', 'U', 'L', 'T', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17371 */ 'M', 'S', 'U', 'B', 'U', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17384 */ 'M', 'A', 'D', 'D', 'U', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17397 */ 'M', 'U', 'L', 'T', 'U', '_', 'D', 'S', 'P', '_', 'M', 'M', 0,
  /* 17410 */ 'A', 'D', 'D', 'I', 'U', 'S', 'P', '_', 'M', 'M', 0,
  /* 17421 */ 'L', 'W', 'S', 'P', '_', 'M', 'M', 0,
  /* 17429 */ 'S', 'W', 'S', 'P', '_', 'M', 'M', 0,
  /* 17437 */ 'E', 'X', 'T', 'P', '_', 'M', 'M', 0,
  /* 17445 */ 'L', 'W', 'P', '_', 'M', 'M', 0,
  /* 17452 */ 'S', 'W', 'P', '_', 'M', 'M', 0,
  /* 17459 */ 'B', 'E', 'Q', '_', 'M', 'M', 0,
  /* 17466 */ 'T', 'E', 'Q', '_', 'M', 'M', 0,
  /* 17473 */ 'T', 'L', 'B', 'R', '_', 'M', 'M', 0,
  /* 17481 */ 'M', 'U', 'L', 'E', 'U', '_', 'S', '_', 'P', 'H', '_', 'Q', 'B', 'R', '_', 'M', 'M', 0,
  /* 17499 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', '_', 'M', 'M', 0,
  /* 17516 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', '_', 'M', 'M', 0,
  /* 17534 */ 'D', 'P', 'A', 'U', '_', 'H', '_', 'Q', 'B', 'R', '_', 'M', 'M', 0,
  /* 17548 */ 'D', 'P', 'S', 'U', '_', 'H', '_', 'Q', 'B', 'R', '_', 'M', 'M', 0,
  /* 17562 */ 'B', 'A', 'L', '_', 'B', 'R', '_', 'M', 'M', 0,
  /* 17572 */ 'T', 'L', 'B', 'G', 'R', '_', 'M', 'M', 0,
  /* 17581 */ 'M', 'A', 'Q', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 'R', '_', 'M', 'M', 0,
  /* 17597 */ 'P', 'R', 'E', 'C', 'E', 'Q', '_', 'W', '_', 'P', 'H', 'R', '_', 'M', 'M', 0,
  /* 17613 */ 'M', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'R', '_', 'M', 'M', 0,
  /* 17628 */ 'M', 'U', 'L', 'E', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'R', '_', 'M', 'M', 0,
  /* 17645 */ 'J', 'R', '_', 'M', 'M', 0,
  /* 17651 */ 'J', 'A', 'L', 'R', '_', 'M', 'M', 0,
  /* 17659 */ 'N', 'O', 'R', '_', 'M', 'M', 0,
  /* 17666 */ 'X', 'O', 'R', '_', 'M', 'M', 0,
  /* 17673 */ 'R', 'O', 'T', 'R', '_', 'M', 'M', 0,
  /* 17681 */ 'T', 'L', 'B', 'W', 'R', '_', 'M', 'M', 0,
  /* 17690 */ 'T', 'L', 'B', 'G', 'W', 'R', '_', 'M', 'M', 0,
  /* 17700 */ 'R', 'D', 'H', 'W', 'R', '_', 'M', 'M', 0,
  /* 17709 */ 'L', 'W', 'R', '_', 'M', 'M', 0,
  /* 17716 */ 'S', 'W', 'R', '_', 'M', 'M', 0,
  /* 17723 */ 'J', 'A', 'L', 'S', '_', 'M', 'M', 0,
  /* 17731 */ 'B', 'G', 'E', 'Z', 'A', 'L', 'S', '_', 'M', 'M', 0,
  /* 17742 */ 'B', 'L', 'T', 'Z', 'A', 'L', 'S', '_', 'M', 'M', 0,
  /* 17753 */ 'I', 'N', 'S', '_', 'M', 'M', 0,
  /* 17760 */ 'J', 'A', 'L', 'R', 'S', '_', 'M', 'M', 0,
  /* 17769 */ 'L', 'W', 'X', 'S', '_', 'M', 'M', 0,
  /* 17777 */ 'C', 'V', 'T', '_', 'D', '3', '2', '_', 'S', '_', 'M', 'M', 0,
  /* 17790 */ 'C', 'V', 'T', '_', 'D', '6', '4', '_', 'S', '_', 'M', 'M', 0,
  /* 17803 */ 'F', 'S', 'U', 'B', '_', 'S', '_', 'M', 'M', 0,
  /* 17813 */ 'N', 'M', 'S', 'U', 'B', '_', 'S', '_', 'M', 'M', 0,
  /* 17824 */ 'F', 'A', 'D', 'D', '_', 'S', '_', 'M', 'M', 0,
  /* 17834 */ 'N', 'M', 'A', 'D', 'D', '_', 'S', '_', 'M', 'M', 0,
  /* 17845 */ 'C', '_', 'N', 'G', 'E', '_', 'S', '_', 'M', 'M', 0,
  /* 17856 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'S', '_', 'M', 'M', 0,
  /* 17868 */ 'C', '_', 'O', 'L', 'E', '_', 'S', '_', 'M', 'M', 0,
  /* 17879 */ 'C', '_', 'U', 'L', 'E', '_', 'S', '_', 'M', 'M', 0,
  /* 17890 */ 'C', '_', 'L', 'E', '_', 'S', '_', 'M', 'M', 0,
  /* 17900 */ 'C', '_', 'S', 'F', '_', 'S', '_', 'M', 'M', 0,
  /* 17910 */ 'M', 'O', 'V', 'F', '_', 'S', '_', 'M', 'M', 0,
  /* 17920 */ 'C', '_', 'F', '_', 'S', '_', 'M', 'M', 0,
  /* 17929 */ 'F', 'N', 'E', 'G', '_', 'S', '_', 'M', 'M', 0,
  /* 17939 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'S', '_', 'M', 'M', 0,
  /* 17951 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'S', '_', 'M', 'M', 0,
  /* 17963 */ 'C', '_', 'N', 'G', 'L', '_', 'S', '_', 'M', 'M', 0,
  /* 17974 */ 'F', 'M', 'U', 'L', '_', 'S', '_', 'M', 'M', 0,
  /* 17984 */ 'C', 'V', 'T', '_', 'L', '_', 'S', '_', 'M', 'M', 0,
  /* 17995 */ 'C', '_', 'U', 'N', '_', 'S', '_', 'M', 'M', 0,
  /* 18005 */ 'R', 'E', 'C', 'I', 'P', '_', 'S', '_', 'M', 'M', 0,
  /* 18016 */ 'C', '_', 'S', 'E', 'Q', '_', 'S', '_', 'M', 'M', 0,
  /* 18027 */ 'C', '_', 'U', 'E', 'Q', '_', 'S', '_', 'M', 'M', 0,
  /* 18038 */ 'C', '_', 'E', 'Q', '_', 'S', '_', 'M', 'M', 0,
  /* 18048 */ 'F', 'A', 'B', 'S', '_', 'S', '_', 'M', 'M', 0,
  /* 18058 */ 'C', '_', 'N', 'G', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18069 */ 'C', '_', 'O', 'L', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18080 */ 'C', '_', 'U', 'L', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18091 */ 'C', '_', 'L', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18101 */ 'F', 'S', 'Q', 'R', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18112 */ 'R', 'S', 'Q', 'R', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18123 */ 'M', 'O', 'V', 'T', '_', 'S', '_', 'M', 'M', 0,
  /* 18133 */ 'F', 'D', 'I', 'V', '_', 'S', '_', 'M', 'M', 0,
  /* 18143 */ 'F', 'M', 'O', 'V', '_', 'S', '_', 'M', 'M', 0,
  /* 18153 */ 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'S', '_', 'M', 'M', 0,
  /* 18166 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'S', '_', 'M', 'M', 0,
  /* 18179 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'S', '_', 'M', 'M', 0,
  /* 18191 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'S', '_', 'M', 'M', 0,
  /* 18204 */ 'C', 'V', 'T', '_', 'W', '_', 'S', '_', 'M', 'M', 0,
  /* 18215 */ 'B', 'C', '1', 'T', '_', 'M', 'M', 0,
  /* 18223 */ 'D', 'E', 'R', 'E', 'T', '_', 'M', 'M', 0,
  /* 18232 */ 'W', 'A', 'I', 'T', '_', 'M', 'M', 0,
  /* 18240 */ 'S', 'L', 'T', '_', 'M', 'M', 0,
  /* 18247 */ 'T', 'L', 'T', '_', 'M', 'M', 0,
  /* 18254 */ 'M', 'U', 'L', 'T', '_', 'M', 'M', 0,
  /* 18262 */ 'E', 'X', 'T', '_', 'M', 'M', 0,
  /* 18269 */ 'M', 'S', 'U', 'B', 'U', '_', 'M', 'M', 0,
  /* 18278 */ 'M', 'A', 'D', 'D', 'U', '_', 'M', 'M', 0,
  /* 18287 */ 'T', 'G', 'E', 'U', '_', 'M', 'M', 0,
  /* 18295 */ 'T', 'G', 'E', 'I', 'U', '_', 'M', 'M', 0,
  /* 18304 */ 'T', 'L', 'T', 'I', 'U', '_', 'M', 'M', 0,
  /* 18313 */ 'T', 'L', 'T', 'U', '_', 'M', 'M', 0,
  /* 18321 */ 'L', 'W', 'U', '_', 'M', 'M', 0,
  /* 18328 */ 'S', 'R', 'A', 'V', '_', 'M', 'M', 0,
  /* 18336 */ 'B', 'I', 'T', 'R', 'E', 'V', '_', 'M', 'M', 0,
  /* 18346 */ 'S', 'D', 'I', 'V', '_', 'M', 'M', 0,
  /* 18354 */ 'U', 'D', 'I', 'V', '_', 'M', 'M', 0,
  /* 18362 */ 'S', 'L', 'L', 'V', '_', 'M', 'M', 0,
  /* 18370 */ 'S', 'R', 'L', 'V', '_', 'M', 'M', 0,
  /* 18378 */ 'T', 'L', 'B', 'G', 'I', 'N', 'V', '_', 'M', 'M', 0,
  /* 18389 */ 'S', 'H', 'I', 'L', 'O', 'V', '_', 'M', 'M', 0,
  /* 18399 */ 'E', 'X', 'T', 'P', 'D', 'P', 'V', '_', 'M', 'M', 0,
  /* 18410 */ 'E', 'X', 'T', 'P', 'V', '_', 'M', 'M', 0,
  /* 18419 */ 'R', 'O', 'T', 'R', 'V', '_', 'M', 'M', 0,
  /* 18428 */ 'I', 'N', 'S', 'V', '_', 'M', 'M', 0,
  /* 18436 */ 'L', 'W', '_', 'M', 'M', 0,
  /* 18442 */ 'S', 'W', '_', 'M', 'M', 0,
  /* 18448 */ 'C', 'V', 'T', '_', 'D', '3', '2', '_', 'W', '_', 'M', 'M', 0,
  /* 18461 */ 'C', 'V', 'T', '_', 'D', '6', '4', '_', 'W', '_', 'M', 'M', 0,
  /* 18474 */ 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'M', 'M', 0,
  /* 18485 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'M', 'M', 0,
  /* 18496 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'P', 'H', '_', 'W', '_', 'M', 'M', 0,
  /* 18511 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'R', 'S', '_', 'P', 'H', '_', 'W', '_', 'M', 'M', 0,
  /* 18529 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'M', 'M', 0,
  /* 18539 */ 'D', 'P', 'A', 'Q', '_', 'S', 'A', '_', 'L', '_', 'W', '_', 'M', 'M', 0,
  /* 18554 */ 'D', 'P', 'S', 'Q', '_', 'S', 'A', '_', 'L', '_', 'W', '_', 'M', 'M', 0,
  /* 18569 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18580 */ 'E', 'X', 'T', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18590 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18602 */ 'E', 'X', 'T', 'R', '_', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18614 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18627 */ 'E', 'X', 'T', 'R', 'V', '_', 'R', '_', 'W', '_', 'M', 'M', 0,
  /* 18640 */ 'E', 'X', 'T', 'R', '_', 'R', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18653 */ 'E', 'X', 'T', 'R', 'V', '_', 'R', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18667 */ 'S', 'H', 'L', 'L', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18679 */ 'S', 'U', 'B', 'Q', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18691 */ 'A', 'D', 'D', 'Q', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18703 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18715 */ 'C', 'V', 'T', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18726 */ 'S', 'H', 'L', 'L', 'V', '_', 'S', '_', 'W', '_', 'M', 'M', 0,
  /* 18739 */ 'E', 'X', 'T', 'R', 'V', '_', 'W', '_', 'M', 'M', 0,
  /* 18750 */ 'P', 'R', 'E', 'F', 'X', '_', 'M', 'M', 0,
  /* 18759 */ 'L', 'H', 'X', '_', 'M', 'M', 0,
  /* 18766 */ 'J', 'A', 'L', 'X', '_', 'M', 'M', 0,
  /* 18774 */ 'L', 'B', 'U', 'X', '_', 'M', 'M', 0,
  /* 18782 */ 'L', 'W', 'X', '_', 'M', 'M', 0,
  /* 18789 */ 'B', 'G', 'E', 'Z', '_', 'M', 'M', 0,
  /* 18797 */ 'B', 'L', 'E', 'Z', '_', 'M', 'M', 0,
  /* 18805 */ 'C', 'L', 'Z', '_', 'M', 'M', 0,
  /* 18812 */ 'B', 'G', 'T', 'Z', '_', 'M', 'M', 0,
  /* 18820 */ 'B', 'L', 'T', 'Z', '_', 'M', 'M', 0,
  /* 18828 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', '_', 'M', 'M', 0,
  /* 18852 */ 'A', 'D', 'D', 'i', '_', 'M', 'M', 0,
  /* 18860 */ 'A', 'N', 'D', 'i', '_', 'M', 'M', 0,
  /* 18868 */ 'X', 'O', 'R', 'i', '_', 'M', 'M', 0,
  /* 18876 */ 'S', 'L', 'T', 'i', '_', 'M', 'M', 0,
  /* 18884 */ 'L', 'U', 'i', '_', 'M', 'M', 0,
  /* 18891 */ 'L', 'B', 'u', '_', 'M', 'M', 0,
  /* 18898 */ 'S', 'U', 'B', 'u', '_', 'M', 'M', 0,
  /* 18906 */ 'A', 'D', 'D', 'u', '_', 'M', 'M', 0,
  /* 18914 */ 'L', 'H', 'u', '_', 'M', 'M', 0,
  /* 18921 */ 'S', 'L', 'T', 'u', '_', 'M', 'M', 0,
  /* 18929 */ 'M', 'U', 'L', 'T', 'u', '_', 'M', 'M', 0,
  /* 18938 */ 'L', 'E', 'A', '_', 'A', 'D', 'D', 'i', 'u', '_', 'M', 'M', 0,
  /* 18951 */ 'S', 'L', 'T', 'i', 'u', '_', 'M', 'M', 0,
  /* 18960 */ 'I', 'N', 'L', 'I', 'N', 'E', 'A', 'S', 'M', 0,
  /* 18970 */ 'D', 'I', 'N', 'S', 'M', 0,
  /* 18976 */ 'D', 'E', 'X', 'T', 'M', 0,
  /* 18982 */ 'B', 'A', 'L', 'I', 'G', 'N', 0,
  /* 18989 */ 'D', 'A', 'L', 'I', 'G', 'N', 0,
  /* 18996 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'U', 'M', 'I', 'N', 0,
  /* 19013 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'M', 'I', 'N', 0,
  /* 19029 */ 'D', 'M', 'F', 'C', '2', '_', 'O', 'C', 'T', 'E', 'O', 'N', 0,
  /* 19042 */ 'D', 'M', 'T', 'C', '2', '_', 'O', 'C', 'T', 'E', 'O', 'N', 0,
  /* 19055 */ 'C', 'F', 'I', '_', 'I', 'N', 'S', 'T', 'R', 'U', 'C', 'T', 'I', 'O', 'N', 0,
  /* 19071 */ 'A', 'D', 'J', 'C', 'A', 'L', 'L', 'S', 'T', 'A', 'C', 'K', 'D', 'O', 'W', 'N', 0,
  /* 19088 */ 'G', '_', 'S', 'S', 'U', 'B', 'O', 0,
  /* 19096 */ 'G', '_', 'S', 'A', 'D', 'D', 'O', 0,
  /* 19104 */ 'F', 'E', 'X', 'P', '2', '_', 'D', '_', '1', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19121 */ 'F', 'E', 'X', 'P', '2', '_', 'W', '_', '1', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19138 */ 'B', 'P', 'O', 'S', 'G', 'E', '3', '2', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19154 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'B', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19177 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'D', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19201 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'D', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19224 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'H', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19247 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'W', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19271 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'W', '_', 'V', 'I', 'D', 'X', '6', '4', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19294 */ 'S', 'N', 'Z', '_', 'B', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19307 */ 'S', 'Z', '_', 'B', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19319 */ 'B', 'S', 'E', 'L', '_', 'F', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19334 */ 'F', 'I', 'L', 'L', '_', 'F', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19349 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19366 */ 'C', 'O', 'P', 'Y', '_', 'F', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19381 */ 'M', 'S', 'A', '_', 'F', 'P', '_', 'E', 'X', 'T', 'E', 'N', 'D', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19404 */ 'M', 'S', 'A', '_', 'F', 'P', '_', 'R', 'O', 'U', 'N', 'D', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19426 */ 'B', 'S', 'E', 'L', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19440 */ 'A', 'N', 'D', '_', 'V', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19455 */ 'N', 'O', 'R', '_', 'V', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19470 */ 'X', 'O', 'R', '_', 'V', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19485 */ 'S', 'N', 'Z', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19498 */ 'S', 'Z', '_', 'D', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19510 */ 'B', 'S', 'E', 'L', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19524 */ 'A', 'N', 'D', '_', 'V', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19539 */ 'N', 'O', 'R', '_', 'V', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19554 */ 'X', 'O', 'R', '_', 'V', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19569 */ 'S', 'N', 'Z', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19582 */ 'S', 'Z', '_', 'H', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19594 */ 'S', 'N', 'Z', '_', 'V', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19607 */ 'S', 'Z', '_', 'V', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19619 */ 'B', 'S', 'E', 'L', '_', 'F', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19634 */ 'F', 'I', 'L', 'L', '_', 'F', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19649 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19666 */ 'C', 'O', 'P', 'Y', '_', 'F', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19681 */ 'M', 'S', 'A', '_', 'F', 'P', '_', 'E', 'X', 'T', 'E', 'N', 'D', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19704 */ 'M', 'S', 'A', '_', 'F', 'P', '_', 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19726 */ 'B', 'S', 'E', 'L', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19740 */ 'A', 'N', 'D', '_', 'V', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19755 */ 'N', 'O', 'R', '_', 'V', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19770 */ 'X', 'O', 'R', '_', 'V', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19785 */ 'S', 'N', 'Z', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19798 */ 'S', 'Z', '_', 'W', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19810 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'B', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19831 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'D', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19853 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'D', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19874 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'H', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19895 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'F', 'W', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19917 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'W', '_', 'V', 'I', 'D', 'X', '_', 'P', 'S', 'E', 'U', 'D', 'O', 0,
  /* 19938 */ 'D', 'C', 'L', 'O', 0,
  /* 19943 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'F', 'L', 'O', 0,
  /* 19954 */ 'S', 'H', 'I', 'L', 'O', 0,
  /* 19960 */ 'M', 'F', 'T', 'L', 'O', 0,
  /* 19966 */ 'M', 'T', 'L', 'O', 0,
  /* 19971 */ 'M', 'T', 'T', 'L', 'O', 0,
  /* 19977 */ 'G', '_', 'S', 'M', 'U', 'L', 'O', 0,
  /* 19985 */ 'G', '_', 'U', 'M', 'U', 'L', 'O', 0,
  /* 19993 */ 'S', 'T', 'A', 'C', 'K', 'M', 'A', 'P', 0,
  /* 20002 */ 'T', 'R', 'A', 'P', 0,
  /* 20007 */ 'G', '_', 'B', 'S', 'W', 'A', 'P', 0,
  /* 20015 */ 'D', 'B', 'I', 'T', 'S', 'W', 'A', 'P', 0,
  /* 20024 */ 'S', 'D', 'B', 'B', 'P', 0,
  /* 20030 */ 'T', 'L', 'B', 'P', 0,
  /* 20035 */ 'E', 'X', 'T', 'P', 'D', 'P', 0,
  /* 20042 */ 'G', '_', 'G', 'E', 'P', 0,
  /* 20048 */ 'G', '_', 'S', 'I', 'T', 'O', 'F', 'P', 0,
  /* 20057 */ 'G', '_', 'U', 'I', 'T', 'O', 'F', 'P', 0,
  /* 20066 */ 'T', 'L', 'B', 'G', 'P', 0,
  /* 20072 */ 'M', 'T', 'H', 'L', 'I', 'P', 0,
  /* 20079 */ 'G', '_', 'F', 'C', 'M', 'P', 0,
  /* 20086 */ 'G', '_', 'I', 'C', 'M', 'P', 0,
  /* 20093 */ 'S', 'S', 'N', 'O', 'P', 0,
  /* 20099 */ 'D', 'P', 'O', 'P', 0,
  /* 20104 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'O', 'P', 0,
  /* 20117 */ 'F', 'A', 'U', 'L', 'T', 'I', 'N', 'G', '_', 'O', 'P', 0,
  /* 20129 */ 'L', 'O', 'A', 'D', '_', 'A', 'C', 'C', '6', '4', 'D', 'S', 'P', 0,
  /* 20143 */ 'S', 'T', 'O', 'R', 'E', '_', 'A', 'C', 'C', '6', '4', 'D', 'S', 'P', 0,
  /* 20158 */ 'R', 'D', 'D', 'S', 'P', 0,
  /* 20164 */ 'W', 'R', 'D', 'S', 'P', 0,
  /* 20170 */ 'M', 'F', 'T', 'D', 'S', 'P', 0,
  /* 20177 */ 'M', 'T', 'T', 'D', 'S', 'P', 0,
  /* 20184 */ 'L', 'W', 'D', 'S', 'P', 0,
  /* 20190 */ 'S', 'W', 'D', 'S', 'P', 0,
  /* 20196 */ 'M', 'S', 'U', 'B', '_', 'D', 'S', 'P', 0,
  /* 20205 */ 'M', 'A', 'D', 'D', '_', 'D', 'S', 'P', 0,
  /* 20214 */ 'L', 'O', 'A', 'D', '_', 'C', 'C', 'O', 'N', 'D', '_', 'D', 'S', 'P', 0,
  /* 20229 */ 'S', 'T', 'O', 'R', 'E', '_', 'C', 'C', 'O', 'N', 'D', '_', 'D', 'S', 'P', 0,
  /* 20245 */ 'M', 'F', 'H', 'I', '_', 'D', 'S', 'P', 0,
  /* 20254 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'T', 'L', 'O', 'H', 'I', '_', 'D', 'S', 'P', 0,
  /* 20271 */ 'M', 'T', 'H', 'I', '_', 'D', 'S', 'P', 0,
  /* 20280 */ 'M', 'F', 'L', 'O', '_', 'D', 'S', 'P', 0,
  /* 20289 */ 'M', 'T', 'L', 'O', '_', 'D', 'S', 'P', 0,
  /* 20298 */ 'M', 'U', 'L', 'T', '_', 'D', 'S', 'P', 0,
  /* 20307 */ 'M', 'S', 'U', 'B', 'U', '_', 'D', 'S', 'P', 0,
  /* 20317 */ 'M', 'A', 'D', 'D', 'U', '_', 'D', 'S', 'P', 0,
  /* 20327 */ 'M', 'U', 'L', 'T', 'U', '_', 'D', 'S', 'P', 0,
  /* 20337 */ 'J', 'R', 'A', 'D', 'D', 'I', 'U', 'S', 'P', 0,
  /* 20347 */ 'E', 'X', 'T', 'P', 0,
  /* 20352 */ 'A', 'D', 'J', 'C', 'A', 'L', 'L', 'S', 'T', 'A', 'C', 'K', 'U', 'P', 0,
  /* 20367 */ 'D', 'V', 'P', 0,
  /* 20371 */ 'E', 'V', 'P', 0,
  /* 20375 */ 'G', '_', 'F', 'E', 'X', 'P', 0,
  /* 20382 */ 'B', 'E', 'Q', 0,
  /* 20386 */ 'S', 'E', 'Q', 0,
  /* 20390 */ 'T', 'E', 'Q', 0,
  /* 20394 */ 'T', 'L', 'B', 'R', 0,
  /* 20399 */ 'M', 'U', 'L', 'E', 'U', '_', 'S', '_', 'P', 'H', '_', 'Q', 'B', 'R', 0,
  /* 20414 */ 'P', 'R', 'E', 'C', 'E', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 0,
  /* 20428 */ 'P', 'R', 'E', 'C', 'E', 'Q', 'U', '_', 'P', 'H', '_', 'Q', 'B', 'R', 0,
  /* 20443 */ 'D', 'P', 'A', 'U', '_', 'H', '_', 'Q', 'B', 'R', 0,
  /* 20454 */ 'D', 'P', 'S', 'U', '_', 'H', '_', 'Q', 'B', 'R', 0,
  /* 20465 */ 'G', '_', 'B', 'R', 0,
  /* 20470 */ 'B', 'A', 'L', '_', 'B', 'R', 0,
  /* 20477 */ 'G', '_', 'B', 'L', 'O', 'C', 'K', '_', 'A', 'D', 'D', 'R', 0,
  /* 20490 */ 'L', 'D', 'R', 0,
  /* 20494 */ 'S', 'D', 'R', 0,
  /* 20498 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'F', 'U', 'N', 'C', 'T', 'I', 'O', 'N', '_', 'E', 'N', 'T', 'E', 'R', 0,
  /* 20523 */ 'T', 'L', 'B', 'G', 'R', 0,
  /* 20529 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', 'D', 'o', 'u', 'b', 'l', 'e', 'F', 'G', 'R', 0,
  /* 20546 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', 'S', 'i', 'n', 'g', 'l', 'e', 'F', 'G', 'R', 0,
  /* 20563 */ 'M', 'A', 'Q', '_', 'S', 'A', '_', 'W', '_', 'P', 'H', 'R', 0,
  /* 20576 */ 'P', 'R', 'E', 'C', 'E', 'Q', '_', 'W', '_', 'P', 'H', 'R', 0,
  /* 20589 */ 'M', 'A', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'R', 0,
  /* 20601 */ 'M', 'U', 'L', 'E', 'Q', '_', 'S', '_', 'W', '_', 'P', 'H', 'R', 0,
  /* 20615 */ 'G', '_', 'A', 'S', 'H', 'R', 0,
  /* 20622 */ 'G', '_', 'L', 'S', 'H', 'R', 0,
  /* 20629 */ 'J', 'R', 0,
  /* 20632 */ 'J', 'A', 'L', 'R', 0,
  /* 20637 */ 'N', 'O', 'R', 0,
  /* 20641 */ 'D', 'R', 'O', 'R', 0,
  /* 20646 */ 'G', '_', 'S', 'H', 'U', 'F', 'F', 'L', 'E', '_', 'V', 'E', 'C', 'T', 'O', 'R', 0,
  /* 20663 */ 'G', '_', 'X', 'O', 'R', 0,
  /* 20669 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'X', 'O', 'R', 0,
  /* 20685 */ 'G', '_', 'O', 'R', 0,
  /* 20690 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'O', 'R', 0,
  /* 20705 */ 'M', 'F', 'T', 'G', 'P', 'R', 0,
  /* 20712 */ 'M', 'T', 'T', 'G', 'P', 'R', 0,
  /* 20719 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', 'D', 'o', 'u', 'b', 'l', 'e', 'G', 'P', 'R', 0,
  /* 20736 */ 'L', 'o', 'a', 'd', 'I', 'm', 'm', 'S', 'i', 'n', 'g', 'l', 'e', 'G', 'P', 'R', 0,
  /* 20753 */ 'M', 'F', 'T', 'R', 0,
  /* 20758 */ 'D', 'R', 'O', 'T', 'R', 0,
  /* 20764 */ 'G', '_', 'I', 'N', 'T', 'T', 'O', 'P', 'T', 'R', 0,
  /* 20775 */ 'M', 'T', 'T', 'R', 0,
  /* 20780 */ 'T', 'L', 'B', 'W', 'R', 0,
  /* 20786 */ 'T', 'L', 'B', 'G', 'W', 'R', 0,
  /* 20793 */ 'R', 'D', 'H', 'W', 'R', 0,
  /* 20799 */ 'L', 'W', 'R', 0,
  /* 20803 */ 'S', 'W', 'R', 0,
  /* 20807 */ 'G', '_', 'F', 'A', 'B', 'S', 0,
  /* 20814 */ 'G', '_', 'U', 'N', 'M', 'E', 'R', 'G', 'E', '_', 'V', 'A', 'L', 'U', 'E', 'S', 0,
  /* 20831 */ 'G', '_', 'M', 'E', 'R', 'G', 'E', '_', 'V', 'A', 'L', 'U', 'E', 'S', 0,
  /* 20846 */ 'C', 'I', 'N', 'S', 0,
  /* 20851 */ 'D', 'I', 'N', 'S', 0,
  /* 20856 */ 'C', 'O', 'P', 'Y', '_', 'T', 'O', '_', 'R', 'E', 'G', 'C', 'L', 'A', 'S', 'S', 0,
  /* 20873 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', '_', 'C', 'M', 'P', 'X', 'C', 'H', 'G', '_', 'W', 'I', 'T', 'H', '_', 'S', 'U', 'C', 'C', 'E', 'S', 'S', 0,
  /* 20903 */ 'G', '_', 'I', 'N', 'T', 'R', 'I', 'N', 'S', 'I', 'C', '_', 'W', '_', 'S', 'I', 'D', 'E', '_', 'E', 'F', 'F', 'E', 'C', 'T', 'S', 0,
  /* 20930 */ 'E', 'X', 'T', 'S', 0,
  /* 20935 */ 'C', 'V', 'T', '_', 'D', '3', '2', '_', 'S', 0,
  /* 20945 */ 'C', 'V', 'T', '_', 'D', '6', '4', '_', 'S', 0,
  /* 20955 */ 'M', 'O', 'V', 'N', '_', 'I', '6', '4', '_', 'S', 0,
  /* 20966 */ 'M', 'O', 'V', 'Z', '_', 'I', '6', '4', '_', 'S', 0,
  /* 20977 */ 'M', 'I', 'N', 'A', '_', 'S', 0,
  /* 20984 */ 'M', 'A', 'X', 'A', '_', 'S', 0,
  /* 20991 */ 'F', 'S', 'U', 'B', '_', 'S', 0,
  /* 20998 */ 'N', 'M', 'S', 'U', 'B', '_', 'S', 0,
  /* 21006 */ 'F', 'A', 'D', 'D', '_', 'S', 0,
  /* 21013 */ 'N', 'M', 'A', 'D', 'D', '_', 'S', 0,
  /* 21021 */ 'C', '_', 'N', 'G', 'E', '_', 'S', 0,
  /* 21029 */ 'C', '_', 'N', 'G', 'L', 'E', '_', 'S', 0,
  /* 21038 */ 'C', '_', 'O', 'L', 'E', '_', 'S', 0,
  /* 21046 */ 'C', 'M', 'P', '_', 'S', 'L', 'E', '_', 'S', 0,
  /* 21056 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'E', '_', 'S', 0,
  /* 21067 */ 'C', '_', 'U', 'L', 'E', '_', 'S', 0,
  /* 21075 */ 'C', 'M', 'P', '_', 'U', 'L', 'E', '_', 'S', 0,
  /* 21085 */ 'C', '_', 'L', 'E', '_', 'S', 0,
  /* 21092 */ 'C', 'M', 'P', '_', 'L', 'E', '_', 'S', 0,
  /* 21101 */ 'C', 'M', 'P', '_', 'S', 'A', 'F', '_', 'S', 0,
  /* 21111 */ 'M', 'S', 'U', 'B', 'F', '_', 'S', 0,
  /* 21119 */ 'M', 'A', 'D', 'D', 'F', '_', 'S', 0,
  /* 21127 */ 'C', '_', 'S', 'F', '_', 'S', 0,
  /* 21134 */ 'M', 'O', 'V', 'F', '_', 'S', 0,
  /* 21141 */ 'C', '_', 'F', '_', 'S', 0,
  /* 21147 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'F', '_', 'S', 0,
  /* 21166 */ 'C', 'M', 'P', '_', 'F', '_', 'S', 0,
  /* 21174 */ 'F', 'N', 'E', 'G', '_', 'S', 0,
  /* 21181 */ 'M', 'O', 'V', 'N', '_', 'I', '_', 'S', 0,
  /* 21190 */ 'M', 'O', 'V', 'Z', '_', 'I', '_', 'S', 0,
  /* 21199 */ 'S', 'E', 'L', '_', 'S', 0,
  /* 21205 */ 'C', '_', 'N', 'G', 'L', '_', 'S', 0,
  /* 21213 */ 'F', 'M', 'U', 'L', '_', 'S', 0,
  /* 21220 */ 'T', 'R', 'U', 'N', 'C', '_', 'L', '_', 'S', 0,
  /* 21230 */ 'R', 'O', 'U', 'N', 'D', '_', 'L', '_', 'S', 0,
  /* 21240 */ 'C', 'E', 'I', 'L', '_', 'L', '_', 'S', 0,
  /* 21249 */ 'F', 'L', 'O', 'O', 'R', '_', 'L', '_', 'S', 0,
  /* 21259 */ 'C', 'V', 'T', '_', 'L', '_', 'S', 0,
  /* 21267 */ 'M', 'I', 'N', '_', 'S', 0,
  /* 21273 */ 'C', 'M', 'P', '_', 'S', 'U', 'N', '_', 'S', 0,
  /* 21283 */ 'C', '_', 'U', 'N', '_', 'S', 0,
  /* 21290 */ 'C', 'M', 'P', '_', 'U', 'N', '_', 'S', 0,
  /* 21299 */ 'R', 'E', 'C', 'I', 'P', '_', 'S', 0,
  /* 21307 */ 'C', '_', 'S', 'E', 'Q', '_', 'S', 0,
  /* 21315 */ 'C', 'M', 'P', '_', 'S', 'E', 'Q', '_', 'S', 0,
  /* 21325 */ 'C', 'M', 'P', '_', 'S', 'U', 'E', 'Q', '_', 'S', 0,
  /* 21336 */ 'C', '_', 'U', 'E', 'Q', '_', 'S', 0,
  /* 21344 */ 'C', 'M', 'P', '_', 'U', 'E', 'Q', '_', 'S', 0,
  /* 21354 */ 'C', '_', 'E', 'Q', '_', 'S', 0,
  /* 21361 */ 'C', 'M', 'P', '_', 'E', 'Q', '_', 'S', 0,
  /* 21370 */ 'F', 'A', 'B', 'S', '_', 'S', 0,
  /* 21377 */ 'C', 'L', 'A', 'S', 'S', '_', 'S', 0,
  /* 21385 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', '_', 'S', 0,
  /* 21400 */ 'C', '_', 'N', 'G', 'T', '_', 'S', 0,
  /* 21408 */ 'C', '_', 'O', 'L', 'T', '_', 'S', 0,
  /* 21416 */ 'C', 'M', 'P', '_', 'S', 'L', 'T', '_', 'S', 0,
  /* 21426 */ 'C', 'M', 'P', '_', 'S', 'U', 'L', 'T', '_', 'S', 0,
  /* 21437 */ 'C', '_', 'U', 'L', 'T', '_', 'S', 0,
  /* 21445 */ 'C', 'M', 'P', '_', 'U', 'L', 'T', '_', 'S', 0,
  /* 21455 */ 'C', '_', 'L', 'T', '_', 'S', 0,
  /* 21462 */ 'C', 'M', 'P', '_', 'L', 'T', '_', 'S', 0,
  /* 21471 */ 'R', 'I', 'N', 'T', '_', 'S', 0,
  /* 21478 */ 'F', 'S', 'Q', 'R', 'T', '_', 'S', 0,
  /* 21486 */ 'R', 'S', 'Q', 'R', 'T', '_', 'S', 0,
  /* 21494 */ 'M', 'O', 'V', 'T', '_', 'S', 0,
  /* 21501 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'E', 'L', 'E', 'C', 'T', 'F', 'P', '_', 'T', '_', 'S', 0,
  /* 21520 */ 'F', 'D', 'I', 'V', '_', 'S', 0,
  /* 21527 */ 'F', 'M', 'O', 'V', '_', 'S', 0,
  /* 21534 */ 'P', 's', 'e', 'u', 'd', 'o', 'T', 'R', 'U', 'N', 'C', '_', 'W', '_', 'S', 0,
  /* 21550 */ 'R', 'O', 'U', 'N', 'D', '_', 'W', '_', 'S', 0,
  /* 21560 */ 'C', 'E', 'I', 'L', '_', 'W', '_', 'S', 0,
  /* 21569 */ 'F', 'L', 'O', 'O', 'R', '_', 'W', '_', 'S', 0,
  /* 21579 */ 'C', 'V', 'T', '_', 'W', '_', 'S', 0,
  /* 21587 */ 'M', 'A', 'X', '_', 'S', 0,
  /* 21593 */ 'S', 'E', 'L', 'N', 'E', 'Z', '_', 'S', 0,
  /* 21602 */ 'S', 'E', 'L', 'E', 'Q', 'Z', '_', 'S', 0,
  /* 21611 */ 'B', 'C', '1', 'T', 0,
  /* 21616 */ 'G', '_', 'E', 'X', 'T', 'R', 'A', 'C', 'T', 0,
  /* 21626 */ 'G', '_', 'S', 'E', 'L', 'E', 'C', 'T', 0,
  /* 21635 */ 'G', '_', 'B', 'R', 'I', 'N', 'D', 'I', 'R', 'E', 'C', 'T', 0,
  /* 21648 */ 'D', 'E', 'R', 'E', 'T', 0,
  /* 21654 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'R', 'E', 'T', 0,
  /* 21668 */ 'B', 'G', 'T', 0,
  /* 21672 */ 'W', 'A', 'I', 'T', 0,
  /* 21677 */ 'P', 'A', 'T', 'C', 'H', 'A', 'B', 'L', 'E', '_', 'F', 'U', 'N', 'C', 'T', 'I', 'O', 'N', '_', 'E', 'X', 'I', 'T', 0,
  /* 21701 */ 'B', 'L', 'T', 0,
  /* 21705 */ 'G', '_', 'E', 'X', 'T', 'R', 'A', 'C', 'T', '_', 'V', 'E', 'C', 'T', 'O', 'R', '_', 'E', 'L', 'T', 0,
  /* 21726 */ 'G', '_', 'I', 'N', 'S', 'E', 'R', 'T', '_', 'V', 'E', 'C', 'T', 'O', 'R', '_', 'E', 'L', 'T', 0,
  /* 21746 */ 'S', 'L', 'T', 0,
  /* 21750 */ 'T', 'L', 'T', 0,
  /* 21754 */ 'P', 's', 'e', 'u', 'd', 'o', 'D', 'M', 'U', 'L', 'T', 0,
  /* 21766 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'U', 'L', 'T', 0,
  /* 21777 */ 'D', 'M', 'T', 0,
  /* 21781 */ 'E', 'M', 'T', 0,
  /* 21785 */ 'G', '_', 'F', 'C', 'O', 'N', 'S', 'T', 'A', 'N', 'T', 0,
  /* 21797 */ 'G', '_', 'C', 'O', 'N', 'S', 'T', 'A', 'N', 'T', 0,
  /* 21808 */ 'S', 'T', 'A', 'T', 'E', 'P', 'O', 'I', 'N', 'T', 0,
  /* 21819 */ 'P', 'A', 'T', 'C', 'H', 'P', 'O', 'I', 'N', 'T', 0,
  /* 21830 */ 'G', '_', 'P', 'T', 'R', 'T', 'O', 'I', 'N', 'T', 0,
  /* 21841 */ 'G', '_', 'V', 'A', 'S', 'T', 'A', 'R', 'T', 0,
  /* 21851 */ 'L', 'I', 'F', 'E', 'T', 'I', 'M', 'E', '_', 'S', 'T', 'A', 'R', 'T', 0,
  /* 21866 */ 'G', '_', 'I', 'N', 'S', 'E', 'R', 'T', 0,
  /* 21875 */ 'G', '_', 'B', 'I', 'T', 'C', 'A', 'S', 'T', 0,
  /* 21885 */ 'G', '_', 'A', 'D', 'D', 'R', 'S', 'P', 'A', 'C', 'E', '_', 'C', 'A', 'S', 'T', 0,
  /* 21902 */ 'G', 'I', 'N', 'V', 'T', 0,
  /* 21908 */ 'D', 'E', 'X', 'T', 0,
  /* 21913 */ 'G', '_', 'F', 'P', 'E', 'X', 'T', 0,
  /* 21921 */ 'G', '_', 'S', 'E', 'X', 'T', 0,
  /* 21928 */ 'G', '_', 'A', 'N', 'Y', 'E', 'X', 'T', 0,
  /* 21937 */ 'G', '_', 'Z', 'E', 'X', 'T', 0,
  /* 21944 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'S', 'U', 'B', 'U', 0,
  /* 21956 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'A', 'D', 'D', 'U', 0,
  /* 21968 */ 'D', 'M', 'O', 'D', 'U', 0,
  /* 21974 */ 'B', 'G', 'E', 'U', 0,
  /* 21979 */ 'T', 'G', 'E', 'U', 0,
  /* 21984 */ 'B', 'L', 'E', 'U', 0,
  /* 21989 */ 'D', 'M', 'U', 'H', 'U', 0,
  /* 21995 */ 'T', 'G', 'E', 'I', 'U', 0,
  /* 22001 */ 'T', 'T', 'L', 'T', 'I', 'U', 0,
  /* 22008 */ 'V', '3', 'M', 'U', 'L', 'U', 0,
  /* 22015 */ 'D', 'M', 'U', 'L', 'U', 0,
  /* 22021 */ 'V', 'M', 'U', 'L', 'U', 0,
  /* 22027 */ 'D', 'I', 'N', 'S', 'U', 0,
  /* 22033 */ 'B', 'G', 'T', 'U', 0,
  /* 22038 */ 'B', 'L', 'T', 'U', 0,
  /* 22043 */ 'T', 'L', 'T', 'U', 0,
  /* 22048 */ 'D', 'E', 'X', 'T', 'U', 0,
  /* 22054 */ 'D', 'D', 'I', 'V', 'U', 0,
  /* 22060 */ 'D', 'S', 'R', 'A', 'V', 0,
  /* 22066 */ 'B', 'I', 'T', 'R', 'E', 'V', 0,
  /* 22073 */ 'D', 'D', 'I', 'V', 0,
  /* 22078 */ 'G', '_', 'F', 'D', 'I', 'V', 0,
  /* 22085 */ 'P', 's', 'e', 'u', 'd', 'o', 'D', 'S', 'D', 'I', 'V', 0,
  /* 22097 */ 'G', '_', 'S', 'D', 'I', 'V', 0,
  /* 22104 */ 'P', 's', 'e', 'u', 'd', 'o', 'S', 'D', 'I', 'V', 0,
  /* 22115 */ 'P', 's', 'e', 'u', 'd', 'o', 'D', 'U', 'D', 'I', 'V', 0,
  /* 22127 */ 'G', '_', 'U', 'D', 'I', 'V', 0,
  /* 22134 */ 'P', 's', 'e', 'u', 'd', 'o', 'U', 'D', 'I', 'V', 0,
  /* 22145 */ 'D', 'S', 'L', 'L', 'V', 0,
  /* 22151 */ 'D', 'S', 'R', 'L', 'V', 0,
  /* 22157 */ 'T', 'L', 'B', 'I', 'N', 'V', 0,
  /* 22164 */ 'T', 'L', 'B', 'G', 'I', 'N', 'V', 0,
  /* 22172 */ 'S', 'H', 'I', 'L', 'O', 'V', 0,
  /* 22179 */ 'E', 'X', 'T', 'P', 'D', 'P', 'V', 0,
  /* 22187 */ 'E', 'X', 'T', 'P', 'V', 0,
  /* 22193 */ 'D', 'R', 'O', 'T', 'R', 'V', 0,
  /* 22200 */ 'I', 'N', 'S', 'V', 0,
  /* 22205 */ 'A', 'N', 'D', '_', 'V', 0,
  /* 22211 */ 'M', 'O', 'V', 'E', '_', 'V', 0,
  /* 22218 */ 'B', 'S', 'E', 'L', '_', 'V', 0,
  /* 22225 */ 'N', 'O', 'R', '_', 'V', 0,
  /* 22231 */ 'X', 'O', 'R', '_', 'V', 0,
  /* 22237 */ 'B', 'Z', '_', 'V', 0,
  /* 22242 */ 'B', 'M', 'Z', '_', 'V', 0,
  /* 22248 */ 'B', 'N', 'Z', '_', 'V', 0,
  /* 22254 */ 'B', 'M', 'N', 'Z', '_', 'V', 0,
  /* 22261 */ 'C', 'R', 'C', '3', '2', 'W', 0,
  /* 22268 */ 'C', 'R', 'C', '3', '2', 'C', 'W', 0,
  /* 22276 */ 'L', 'W', 0,
  /* 22279 */ 'G', '_', 'F', 'P', 'O', 'W', 0,
  /* 22286 */ 'S', 'W', 0,
  /* 22289 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'V', 'T', '_', 'D', '3', '2', '_', 'W', 0,
  /* 22305 */ 'F', 'L', 'O', 'G', '2', '_', 'W', 0,
  /* 22313 */ 'F', 'E', 'X', 'P', '2', '_', 'W', 0,
  /* 22321 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'V', 'T', '_', 'D', '6', '4', '_', 'W', 0,
  /* 22337 */ 'S', 'R', 'A', '_', 'W', 0,
  /* 22343 */ 'A', 'D', 'D', '_', 'A', '_', 'W', 0,
  /* 22351 */ 'F', 'M', 'I', 'N', '_', 'A', '_', 'W', 0,
  /* 22360 */ 'A', 'D', 'D', 'S', '_', 'A', '_', 'W', 0,
  /* 22369 */ 'F', 'M', 'A', 'X', '_', 'A', '_', 'W', 0,
  /* 22378 */ 'F', 'S', 'U', 'B', '_', 'W', 0,
  /* 22385 */ 'F', 'M', 'S', 'U', 'B', '_', 'W', 0,
  /* 22393 */ 'N', 'L', 'O', 'C', '_', 'W', 0,
  /* 22400 */ 'N', 'L', 'Z', 'C', '_', 'W', 0,
  /* 22407 */ 'F', 'A', 'D', 'D', '_', 'W', 0,
  /* 22414 */ 'F', 'M', 'A', 'D', 'D', '_', 'W', 0,
  /* 22422 */ 'S', 'L', 'D', '_', 'W', 0,
  /* 22428 */ 'P', 'C', 'K', 'O', 'D', '_', 'W', 0,
  /* 22436 */ 'I', 'L', 'V', 'O', 'D', '_', 'W', 0,
  /* 22444 */ 'F', 'C', 'L', 'E', '_', 'W', 0,
  /* 22451 */ 'F', 'S', 'L', 'E', '_', 'W', 0,
  /* 22458 */ 'F', 'C', 'U', 'L', 'E', '_', 'W', 0,
  /* 22466 */ 'F', 'S', 'U', 'L', 'E', '_', 'W', 0,
  /* 22474 */ 'F', 'C', 'N', 'E', '_', 'W', 0,
  /* 22481 */ 'F', 'S', 'N', 'E', '_', 'W', 0,
  /* 22488 */ 'F', 'C', 'U', 'N', 'E', '_', 'W', 0,
  /* 22496 */ 'F', 'S', 'U', 'N', 'E', '_', 'W', 0,
  /* 22504 */ 'I', 'N', 'S', 'V', 'E', '_', 'W', 0,
  /* 22512 */ 'F', 'C', 'A', 'F', '_', 'W', 0,
  /* 22519 */ 'F', 'S', 'A', 'F', '_', 'W', 0,
  /* 22526 */ 'V', 'S', 'H', 'F', '_', 'W', 0,
  /* 22533 */ 'B', 'N', 'E', 'G', '_', 'W', 0,
  /* 22540 */ 'P', 'R', 'E', 'C', 'R', '_', 'S', 'R', 'A', '_', 'P', 'H', '_', 'W', 0,
  /* 22555 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'P', 'H', '_', 'W', 0,
  /* 22567 */ 'P', 'R', 'E', 'C', 'R', '_', 'S', 'R', 'A', '_', 'R', '_', 'P', 'H', '_', 'W', 0,
  /* 22584 */ 'P', 'R', 'E', 'C', 'R', 'Q', '_', 'R', 'S', '_', 'P', 'H', '_', 'W', 0,
  /* 22599 */ 'S', 'U', 'B', 'Q', 'H', '_', 'W', 0,
  /* 22607 */ 'A', 'D', 'D', 'Q', 'H', '_', 'W', 0,
  /* 22615 */ 'S', 'R', 'A', 'I', '_', 'W', 0,
  /* 22622 */ 'S', 'L', 'D', 'I', '_', 'W', 0,
  /* 22629 */ 'B', 'N', 'E', 'G', 'I', '_', 'W', 0,
  /* 22637 */ 'S', 'L', 'L', 'I', '_', 'W', 0,
  /* 22644 */ 'S', 'R', 'L', 'I', '_', 'W', 0,
  /* 22651 */ 'B', 'I', 'N', 'S', 'L', 'I', '_', 'W', 0,
  /* 22660 */ 'C', 'E', 'Q', 'I', '_', 'W', 0,
  /* 22667 */ 'S', 'R', 'A', 'R', 'I', '_', 'W', 0,
  /* 22675 */ 'B', 'C', 'L', 'R', 'I', '_', 'W', 0,
  /* 22683 */ 'S', 'R', 'L', 'R', 'I', '_', 'W', 0,
  /* 22691 */ 'B', 'I', 'N', 'S', 'R', 'I', '_', 'W', 0,
  /* 22700 */ 'S', 'P', 'L', 'A', 'T', 'I', '_', 'W', 0,
  /* 22709 */ 'B', 'S', 'E', 'T', 'I', '_', 'W', 0,
  /* 22717 */ 'S', 'U', 'B', 'V', 'I', '_', 'W', 0,
  /* 22725 */ 'A', 'D', 'D', 'V', 'I', '_', 'W', 0,
  /* 22733 */ 'F', 'I', 'L', 'L', '_', 'W', 0,
  /* 22740 */ 'S', 'L', 'L', '_', 'W', 0,
  /* 22746 */ 'F', 'E', 'X', 'U', 'P', 'L', '_', 'W', 0,
  /* 22755 */ 'F', 'F', 'Q', 'L', '_', 'W', 0,
  /* 22762 */ 'S', 'R', 'L', '_', 'W', 0,
  /* 22768 */ 'B', 'I', 'N', 'S', 'L', '_', 'W', 0,
  /* 22776 */ 'F', 'M', 'U', 'L', '_', 'W', 0,
  /* 22783 */ 'I', 'L', 'V', 'L', '_', 'W', 0,
  /* 22790 */ 'D', 'P', 'A', 'Q', '_', 'S', 'A', '_', 'L', '_', 'W', 0,
  /* 22802 */ 'D', 'P', 'S', 'Q', '_', 'S', 'A', '_', 'L', '_', 'W', 0,
  /* 22814 */ 'F', 'M', 'I', 'N', '_', 'W', 0,
  /* 22821 */ 'F', 'C', 'U', 'N', '_', 'W', 0,
  /* 22828 */ 'F', 'S', 'U', 'N', '_', 'W', 0,
  /* 22835 */ 'F', 'E', 'X', 'D', 'O', '_', 'W', 0,
  /* 22843 */ 'F', 'R', 'C', 'P', '_', 'W', 0,
  /* 22850 */ 'F', 'C', 'E', 'Q', '_', 'W', 0,
  /* 22857 */ 'F', 'S', 'E', 'Q', '_', 'W', 0,
  /* 22864 */ 'F', 'C', 'U', 'E', 'Q', '_', 'W', 0,
  /* 22872 */ 'F', 'S', 'U', 'E', 'Q', '_', 'W', 0,
  /* 22880 */ 'F', 'T', 'Q', '_', 'W', 0,
  /* 22886 */ 'M', 'S', 'U', 'B', '_', 'Q', '_', 'W', 0,
  /* 22895 */ 'M', 'A', 'D', 'D', '_', 'Q', '_', 'W', 0,
  /* 22904 */ 'M', 'U', 'L', '_', 'Q', '_', 'W', 0,
  /* 22912 */ 'M', 'S', 'U', 'B', 'R', '_', 'Q', '_', 'W', 0,
  /* 22922 */ 'M', 'A', 'D', 'D', 'R', '_', 'Q', '_', 'W', 0,
  /* 22932 */ 'M', 'U', 'L', 'R', '_', 'Q', '_', 'W', 0,
  /* 22941 */ 'S', 'R', 'A', 'R', '_', 'W', 0,
  /* 22948 */ 'B', 'C', 'L', 'R', '_', 'W', 0,
  /* 22955 */ 'S', 'R', 'L', 'R', '_', 'W', 0,
  /* 22962 */ 'F', 'C', 'O', 'R', '_', 'W', 0,
  /* 22969 */ 'F', 'S', 'O', 'R', '_', 'W', 0,
  /* 22976 */ 'F', 'E', 'X', 'U', 'P', 'R', '_', 'W', 0,
  /* 22985 */ 'F', 'F', 'Q', 'R', '_', 'W', 0,
  /* 22992 */ 'B', 'I', 'N', 'S', 'R', '_', 'W', 0,
  /* 23000 */ 'E', 'X', 'T', 'R', '_', 'W', 0,
  /* 23007 */ 'I', 'L', 'V', 'R', '_', 'W', 0,
  /* 23014 */ 'S', 'H', 'R', 'A', '_', 'R', '_', 'W', 0,
  /* 23023 */ 'S', 'U', 'B', 'Q', 'H', '_', 'R', '_', 'W', 0,
  /* 23033 */ 'A', 'D', 'D', 'Q', 'H', '_', 'R', '_', 'W', 0,
  /* 23043 */ 'E', 'X', 'T', 'R', '_', 'R', '_', 'W', 0,
  /* 23052 */ 'S', 'H', 'R', 'A', 'V', '_', 'R', '_', 'W', 0,
  /* 23062 */ 'E', 'X', 'T', 'R', 'V', '_', 'R', '_', 'W', 0,
  /* 23072 */ 'F', 'A', 'B', 'S', '_', 'W', 0,
  /* 23079 */ 'M', 'U', 'L', 'Q', '_', 'R', 'S', '_', 'W', 0,
  /* 23089 */ 'E', 'X', 'T', 'R', '_', 'R', 'S', '_', 'W', 0,
  /* 23099 */ 'E', 'X', 'T', 'R', 'V', '_', 'R', 'S', '_', 'W', 0,
  /* 23110 */ 'F', 'C', 'L', 'A', 'S', 'S', '_', 'W', 0,
  /* 23119 */ 'A', 'S', 'U', 'B', '_', 'S', '_', 'W', 0,
  /* 23128 */ 'H', 'S', 'U', 'B', '_', 'S', '_', 'W', 0,
  /* 23137 */ 'D', 'P', 'S', 'U', 'B', '_', 'S', '_', 'W', 0,
  /* 23147 */ 'F', 'T', 'R', 'U', 'N', 'C', '_', 'S', '_', 'W', 0,
  /* 23158 */ 'H', 'A', 'D', 'D', '_', 'S', '_', 'W', 0,
  /* 23167 */ 'D', 'P', 'A', 'D', 'D', '_', 'S', '_', 'W', 0,
  /* 23177 */ 'M', 'O', 'D', '_', 'S', '_', 'W', 0,
  /* 23185 */ 'C', 'L', 'E', '_', 'S', '_', 'W', 0,
  /* 23193 */ 'A', 'V', 'E', '_', 'S', '_', 'W', 0,
  /* 23201 */ 'C', 'L', 'E', 'I', '_', 'S', '_', 'W', 0,
  /* 23210 */ 'M', 'I', 'N', 'I', '_', 'S', '_', 'W', 0,
  /* 23219 */ 'C', 'L', 'T', 'I', '_', 'S', '_', 'W', 0,
  /* 23228 */ 'M', 'A', 'X', 'I', '_', 'S', '_', 'W', 0,
  /* 23237 */ 'S', 'H', 'L', 'L', '_', 'S', '_', 'W', 0,
  /* 23246 */ 'M', 'I', 'N', '_', 'S', '_', 'W', 0,
  /* 23254 */ 'D', 'O', 'T', 'P', '_', 'S', '_', 'W', 0,
  /* 23263 */ 'S', 'U', 'B', 'Q', '_', 'S', '_', 'W', 0,
  /* 23272 */ 'A', 'D', 'D', 'Q', '_', 'S', '_', 'W', 0,
  /* 23281 */ 'M', 'U', 'L', 'Q', '_', 'S', '_', 'W', 0,
  /* 23290 */ 'A', 'B', 'S', 'Q', '_', 'S', '_', 'W', 0,
  /* 23299 */ 'A', 'V', 'E', 'R', '_', 'S', '_', 'W', 0,
  /* 23308 */ 'S', 'U', 'B', 'S', '_', 'S', '_', 'W', 0,
  /* 23317 */ 'A', 'D', 'D', 'S', '_', 'S', '_', 'W', 0,
  /* 23326 */ 'S', 'A', 'T', '_', 'S', '_', 'W', 0,
  /* 23334 */ 'C', 'L', 'T', '_', 'S', '_', 'W', 0,
  /* 23342 */ 'F', 'F', 'I', 'N', 'T', '_', 'S', '_', 'W', 0,
  /* 23352 */ 'F', 'T', 'I', 'N', 'T', '_', 'S', '_', 'W', 0,
  /* 23362 */ 'P', 's', 'e', 'u', 'd', 'o', 'C', 'V', 'T', '_', 'S', '_', 'W', 0,
  /* 23376 */ 'S', 'U', 'B', 'S', 'U', 'U', '_', 'S', '_', 'W', 0,
  /* 23387 */ 'D', 'I', 'V', '_', 'S', '_', 'W', 0,
  /* 23395 */ 'S', 'H', 'L', 'L', 'V', '_', 'S', '_', 'W', 0,
  /* 23405 */ 'M', 'A', 'X', '_', 'S', '_', 'W', 0,
  /* 23413 */ 'C', 'O', 'P', 'Y', '_', 'S', '_', 'W', 0,
  /* 23422 */ 'S', 'P', 'L', 'A', 'T', '_', 'W', 0,
  /* 23430 */ 'B', 'S', 'E', 'T', '_', 'W', 0,
  /* 23437 */ 'F', 'C', 'L', 'T', '_', 'W', 0,
  /* 23444 */ 'F', 'S', 'L', 'T', '_', 'W', 0,
  /* 23451 */ 'F', 'C', 'U', 'L', 'T', '_', 'W', 0,
  /* 23459 */ 'F', 'S', 'U', 'L', 'T', '_', 'W', 0,
  /* 23467 */ 'P', 'C', 'N', 'T', '_', 'W', 0,
  /* 23474 */ 'F', 'R', 'I', 'N', 'T', '_', 'W', 0,
  /* 23482 */ 'I', 'N', 'S', 'E', 'R', 'T', '_', 'W', 0,
  /* 23491 */ 'F', 'S', 'Q', 'R', 'T', '_', 'W', 0,
  /* 23499 */ 'F', 'R', 'S', 'Q', 'R', 'T', '_', 'W', 0,
  /* 23508 */ 'S', 'T', '_', 'W', 0,
  /* 23513 */ 'A', 'S', 'U', 'B', '_', 'U', '_', 'W', 0,
  /* 23522 */ 'H', 'S', 'U', 'B', '_', 'U', '_', 'W', 0,
  /* 23531 */ 'D', 'P', 'S', 'U', 'B', '_', 'U', '_', 'W', 0,
  /* 23541 */ 'F', 'T', 'R', 'U', 'N', 'C', '_', 'U', '_', 'W', 0,
  /* 23552 */ 'H', 'A', 'D', 'D', '_', 'U', '_', 'W', 0,
  /* 23561 */ 'D', 'P', 'A', 'D', 'D', '_', 'U', '_', 'W', 0,
  /* 23571 */ 'M', 'O', 'D', '_', 'U', '_', 'W', 0,
  /* 23579 */ 'C', 'L', 'E', '_', 'U', '_', 'W', 0,
  /* 23587 */ 'A', 'V', 'E', '_', 'U', '_', 'W', 0,
  /* 23595 */ 'C', 'L', 'E', 'I', '_', 'U', '_', 'W', 0,
  /* 23604 */ 'M', 'I', 'N', 'I', '_', 'U', '_', 'W', 0,
  /* 23613 */ 'C', 'L', 'T', 'I', '_', 'U', '_', 'W', 0,
  /* 23622 */ 'M', 'A', 'X', 'I', '_', 'U', '_', 'W', 0,
  /* 23631 */ 'M', 'I', 'N', '_', 'U', '_', 'W', 0,
  /* 23639 */ 'D', 'O', 'T', 'P', '_', 'U', '_', 'W', 0,
  /* 23648 */ 'A', 'V', 'E', 'R', '_', 'U', '_', 'W', 0,
  /* 23657 */ 'S', 'U', 'B', 'S', '_', 'U', '_', 'W', 0,
  /* 23666 */ 'A', 'D', 'D', 'S', '_', 'U', '_', 'W', 0,
  /* 23675 */ 'S', 'U', 'B', 'S', 'U', 'S', '_', 'U', '_', 'W', 0,
  /* 23686 */ 'S', 'A', 'T', '_', 'U', '_', 'W', 0,
  /* 23694 */ 'C', 'L', 'T', '_', 'U', '_', 'W', 0,
  /* 23702 */ 'F', 'F', 'I', 'N', 'T', '_', 'U', '_', 'W', 0,
  /* 23712 */ 'F', 'T', 'I', 'N', 'T', '_', 'U', '_', 'W', 0,
  /* 23722 */ 'D', 'I', 'V', '_', 'U', '_', 'W', 0,
  /* 23730 */ 'M', 'A', 'X', '_', 'U', '_', 'W', 0,
  /* 23738 */ 'C', 'O', 'P', 'Y', '_', 'U', '_', 'W', 0,
  /* 23747 */ 'M', 'S', 'U', 'B', 'V', '_', 'W', 0,
  /* 23755 */ 'M', 'A', 'D', 'D', 'V', '_', 'W', 0,
  /* 23763 */ 'P', 'C', 'K', 'E', 'V', '_', 'W', 0,
  /* 23771 */ 'I', 'L', 'V', 'E', 'V', '_', 'W', 0,
  /* 23779 */ 'F', 'D', 'I', 'V', '_', 'W', 0,
  /* 23786 */ 'M', 'U', 'L', 'V', '_', 'W', 0,
  /* 23793 */ 'E', 'X', 'T', 'R', 'V', '_', 'W', 0,
  /* 23801 */ 'F', 'M', 'A', 'X', '_', 'W', 0,
  /* 23808 */ 'B', 'Z', '_', 'W', 0,
  /* 23813 */ 'B', 'N', 'Z', '_', 'W', 0,
  /* 23819 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'U', 'M', 'A', 'X', 0,
  /* 23836 */ 'G', '_', 'A', 'T', 'O', 'M', 'I', 'C', 'R', 'M', 'W', '_', 'M', 'A', 'X', 0,
  /* 23852 */ 'M', 'F', 'T', 'A', 'C', 'X', 0,
  /* 23859 */ 'M', 'T', 'T', 'A', 'C', 'X', 0,
  /* 23866 */ 'G', '_', 'F', 'R', 'A', 'M', 'E', '_', 'I', 'N', 'D', 'E', 'X', 0,
  /* 23880 */ 'L', 'H', 'X', 0,
  /* 23884 */ 'J', 'A', 'L', 'X', 0,
  /* 23889 */ 'L', 'B', 'U', 'X', 0,
  /* 23894 */ 'L', 'W', 'X', 0,
  /* 23898 */ 'C', 'O', 'P', 'Y', 0,
  /* 23903 */ 'C', 'O', 'N', 'S', 'T', 'P', 'O', 'O', 'L', '_', 'E', 'N', 'T', 'R', 'Y', 0,
  /* 23919 */ 'B', 'G', 'E', 'Z', 0,
  /* 23924 */ 'B', 'L', 'E', 'Z', 0,
  /* 23929 */ 'B', 'C', '1', 'N', 'E', 'Z', 0,
  /* 23936 */ 'B', 'C', '2', 'N', 'E', 'Z', 0,
  /* 23943 */ 'S', 'E', 'L', 'N', 'E', 'Z', 0,
  /* 23950 */ 'D', 'C', 'L', 'Z', 0,
  /* 23955 */ 'B', 'C', '1', 'E', 'Q', 'Z', 0,
  /* 23962 */ 'B', 'C', '2', 'E', 'Q', 'Z', 0,
  /* 23969 */ 'S', 'E', 'L', 'E', 'Q', 'Z', 0,
  /* 23976 */ 'B', 'G', 'T', 'Z', 0,
  /* 23981 */ 'B', 'L', 'T', 'Z', 0,
  /* 23986 */ 'S', 'e', 'l', 'B', 'n', 'e', 'Z', 0,
  /* 23994 */ 'S', 'e', 'l', 'B', 'e', 'q', 'Z', 0,
  /* 24002 */ 'J', 'a', 'l', 'O', 'n', 'e', 'R', 'e', 'g', 0,
  /* 24012 */ 'J', 'a', 'l', 'T', 'w', 'o', 'R', 'e', 'g', 0,
  /* 24022 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'H', 'a', 'z', 'a', 'r', 'd', 'B', 'r', 'a', 'n', 'c', 'h', 0,
  /* 24049 */ 'P', 's', 'e', 'u', 'd', 'o', 'I', 'n', 'd', 'i', 'r', 'e', 'c', 't', 'B', 'r', 'a', 'n', 'c', 'h', 0,
  /* 24070 */ 'U', 'l', 'h', 0,
  /* 24074 */ 'U', 's', 'h', 0,
  /* 24078 */ 'D', 'A', 'D', 'D', 'i', 0,
  /* 24084 */ 'A', 'N', 'D', 'i', 0,
  /* 24089 */ 'S', 'N', 'E', 'i', 0,
  /* 24094 */ 'S', 'E', 'Q', 'i', 0,
  /* 24099 */ 'X', 'O', 'R', 'i', 0,
  /* 24104 */ 'S', 'L', 'T', 'i', 0,
  /* 24109 */ 'L', 'O', 'N', 'G', '_', 'B', 'R', 'A', 'N', 'C', 'H', '_', 'L', 'U', 'i', 0,
  /* 24125 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'C', 'm', 'p', 'i', 0,
  /* 24139 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'C', 'm', 'p', 'i', 0,
  /* 24153 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'S', 'l', 't', 'i', 0,
  /* 24167 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'S', 'l', 't', 'i', 0,
  /* 24181 */ 'D', 'R', 'O', 'L', 'I', 'm', 'm', 0,
  /* 24189 */ 'N', 'O', 'R', 'I', 'm', 'm', 0,
  /* 24196 */ 'D', 'R', 'O', 'R', 'I', 'm', 'm', 0,
  /* 24204 */ 'B', 'n', 'e', 'I', 'm', 'm', 0,
  /* 24211 */ 'B', 'e', 'q', 'I', 'm', 'm', 0,
  /* 24218 */ 'P', 's', 'e', 'u', 'd', 'o', 'R', 'e', 't', 'u', 'r', 'n', 0,
  /* 24231 */ 'J', 'A', 'L', 'R', 'H', 'B', '6', '4', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24246 */ 'J', 'A', 'L', 'R', '6', '4', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24259 */ 'J', 'A', 'L', 'R', 'H', 'B', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24272 */ 'J', 'A', 'L', 'R', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24283 */ 'B', '_', 'M', 'M', 'R', '6', '_', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24297 */ 'B', '_', 'M', 'M', '_', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24309 */ 'S', 'D', 'I', 'V', '_', 'M', 'M', '_', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24324 */ 'U', 'D', 'I', 'V', '_', 'M', 'M', '_', 'P', 's', 'e', 'u', 'd', 'o', 0,
  /* 24339 */ 'L', 'D', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24347 */ 'S', 'D', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24355 */ 'S', 'E', 'Q', 'I', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24365 */ 'D', 'S', 'R', 'e', 'm', 'I', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24377 */ 'D', 'U', 'R', 'e', 'm', 'I', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24389 */ 'D', 'S', 'D', 'i', 'v', 'I', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24401 */ 'D', 'U', 'D', 'i', 'v', 'I', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24413 */ 'D', 'M', 'U', 'L', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24423 */ 'D', 'M', 'U', 'L', 'O', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24434 */ 'S', 'E', 'Q', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24443 */ 'A', 'B', 'S', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24452 */ 'D', 'M', 'U', 'L', 'O', 'U', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24464 */ 'D', 'S', 'R', 'e', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24475 */ 'D', 'U', 'R', 'e', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24486 */ 'B', 'G', 'E', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24498 */ 'B', 'L', 'E', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24510 */ 'B', 'G', 'E', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24523 */ 'B', 'L', 'E', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24536 */ 'B', 'N', 'E', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24549 */ 'B', 'E', 'Q', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24562 */ 'B', 'G', 'T', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24575 */ 'B', 'L', 'T', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24588 */ 'B', 'G', 'E', 'U', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24602 */ 'B', 'L', 'E', 'U', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24616 */ 'D', 'M', 'U', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24629 */ 'B', 'G', 'T', 'U', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24643 */ 'B', 'L', 'T', 'U', 'L', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24657 */ 'B', 'G', 'T', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24669 */ 'B', 'L', 'T', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24681 */ 'B', 'G', 'E', 'U', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24694 */ 'B', 'L', 'E', 'U', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24707 */ 'B', 'G', 'T', 'U', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24720 */ 'B', 'L', 'T', 'U', 'I', 'm', 'm', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24733 */ 'D', 'S', 'D', 'i', 'v', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24744 */ 'D', 'U', 'D', 'i', 'v', 'M', 'a', 'c', 'r', 'o', 0,
  /* 24755 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'C', 'm', 'p', 0,
  /* 24768 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'C', 'm', 'p', 0,
  /* 24781 */ 'E', 'R', 'e', 't', 0,
  /* 24786 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'S', 'l', 't', 0,
  /* 24799 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'S', 'l', 't', 0,
  /* 24812 */ 'L', 'B', 'u', 0,
  /* 24816 */ 'D', 'S', 'U', 'B', 'u', 0,
  /* 24822 */ 'B', 'A', 'D', 'D', 'u', 0,
  /* 24828 */ 'D', 'A', 'D', 'D', 'u', 0,
  /* 24834 */ 'L', 'H', 'u', 0,
  /* 24838 */ 'S', 'L', 'T', 'u', 0,
  /* 24843 */ 'P', 's', 'e', 'u', 'd', 'o', 'D', 'M', 'U', 'L', 'T', 'u', 0,
  /* 24856 */ 'P', 's', 'e', 'u', 'd', 'o', 'M', 'U', 'L', 'T', 'u', 0,
  /* 24868 */ 'L', 'W', 'u', 0,
  /* 24872 */ 'U', 'l', 'h', 'u', 0,
  /* 24877 */ 'L', 'O', 'N', 'G', '_', 'B', 'R', 'A', 'N', 'C', 'H', '_', 'D', 'A', 'D', 'D', 'i', 'u', 0,
  /* 24896 */ 'L', 'E', 'A', '_', 'A', 'D', 'D', 'i', 'u', 0,
  /* 24906 */ 'L', 'O', 'N', 'G', '_', 'B', 'R', 'A', 'N', 'C', 'H', '_', 'A', 'D', 'D', 'i', 'u', 0,
  /* 24924 */ 'S', 'L', 'T', 'i', 'u', 0,
  /* 24930 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'S', 'l', 't', 'i', 'u', 0,
  /* 24945 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'S', 'l', 't', 'i', 'u', 0,
  /* 24960 */ 'S', 'e', 'l', 'T', 'B', 't', 'n', 'e', 'Z', 'S', 'l', 't', 'u', 0,
  /* 24974 */ 'S', 'e', 'l', 'T', 'B', 't', 'e', 'q', 'Z', 'S', 'l', 't', 'u', 0,
  /* 24988 */ 'U', 'l', 'w', 0,
  /* 24992 */ 'U', 's', 'w', 0,
};

extern const unsigned MipsInstrNameIndices[] = {
    13639U, 18960U, 19055U, 13963U, 13944U, 13972U, 14209U, 12038U, 
    12053U, 11932U, 12079U, 20856U, 11882U, 13953U, 11738U, 23898U, 
    11802U, 21851U, 10300U, 19993U, 14181U, 21819U, 10327U, 21808U, 
    11835U, 20117U, 20104U, 20498U, 21654U, 21677U, 14113U, 14160U, 
    14133U, 14004U, 10209U, 9148U, 14274U, 22097U, 22127U, 14361U, 
    14368U, 10263U, 20685U, 20663U, 11930U, 13637U, 23866U, 11892U, 
    21616U, 20814U, 21866U, 20831U, 21830U, 20764U, 21875U, 10167U, 
    10145U, 10156U, 11858U, 20873U, 12093U, 12110U, 10215U, 9154U, 
    10269U, 10246U, 20690U, 20669U, 23836U, 19013U, 23819U, 18996U, 
    10313U, 21635U, 9920U, 20903U, 21928U, 10008U, 21797U, 21785U, 
    21841U, 12134U, 21921U, 21937U, 14082U, 20622U, 20615U, 20086U, 
    20079U, 21626U, 11755U, 11730U, 19096U, 19088U, 19985U, 19977U, 
    12179U, 12171U, 10191U, 9130U, 14267U, 7833U, 22078U, 14354U, 
    22279U, 20375U, 1082U, 12127U, 1064U, 11967U, 21913U, 9998U, 
    13665U, 13689U, 20048U, 20057U, 20807U, 20042U, 13837U, 20465U, 
    21726U, 21705U, 20646U, 20007U, 21885U, 20477U, 24443U, 19071U, 
    20352U, 19440U, 19524U, 19740U, 3553U, 8431U, 802U, 8007U, 
    2901U, 8219U, 7746U, 8638U, 3476U, 8326U, 725U, 7902U, 
    2772U, 8114U, 7673U, 8537U, 3517U, 8381U, 766U, 7957U, 
    2813U, 8169U, 7712U, 8590U, 3496U, 8353U, 745U, 7929U, 
    2792U, 8141U, 7692U, 8563U, 3593U, 8485U, 842U, 8061U, 
    2941U, 8273U, 7784U, 8690U, 3456U, 8299U, 705U, 7875U, 
    2752U, 8087U, 7654U, 8511U, 3573U, 8458U, 822U, 8034U, 
    2921U, 8246U, 7765U, 8664U, 3537U, 8408U, 786U, 7984U, 
    2885U, 8196U, 7731U, 8616U, 8745U, 20470U, 17562U, 24549U, 
    11776U, 24486U, 13989U, 24510U, 21974U, 24681U, 14250U, 24588U, 
    21668U, 24657U, 14240U, 24562U, 22033U, 24707U, 14280U, 24629U, 
    11798U, 24498U, 13994U, 24523U, 21984U, 24694U, 14256U, 24602U, 
    21701U, 24669U, 14245U, 24575U, 22038U, 24720U, 14286U, 24643U, 
    24536U, 19138U, 19426U, 19319U, 19619U, 19510U, 19726U, 15794U, 
    24283U, 24297U, 24211U, 24204U, 4116U, 3663U, 4144U, 3693U, 
    4174U, 4205U, 4102U, 3648U, 4130U, 3678U, 4158U, 4190U, 
    2630U, 3156U, 113U, 23903U, 19366U, 19666U, 131U, 1021U, 
    24616U, 24413U, 24423U, 24452U, 14219U, 24181U, 20641U, 24196U, 
    24389U, 24733U, 24365U, 24464U, 24401U, 24744U, 24377U, 24475U, 
    24781U, 2643U, 3172U, 11018U, 23072U, 19104U, 19121U, 19334U, 
    19634U, 4263U, 19154U, 19810U, 19201U, 19853U, 19349U, 19177U, 
    19831U, 19649U, 19247U, 19895U, 19224U, 19874U, 19271U, 19917U, 
    24246U, 24231U, 24259U, 24272U, 24002U, 24012U, 24339U, 3442U, 
    7629U, 1921U, 20129U, 20214U, 24906U, 24877U, 24109U, 17112U, 
    989U, 3333U, 956U, 3202U, 979U, 3323U, 20529U, 936U, 
    20719U, 20546U, 20736U, 1019U, 23852U, 34U, 119U, 20170U, 
    20705U, 94U, 13643U, 19960U, 1003U, 3362U, 19381U, 19681U, 
    19404U, 19704U, 23859U, 46U, 137U, 20177U, 20712U, 101U, 
    13654U, 19971U, 24617U, 24424U, 24453U, 4523U, 4656U, 4555U, 
    4708U, 20095U, 24189U, 3295U, 19455U, 19539U, 19755U, 19456U, 
    19540U, 19756U, 8925U, 8827U, 9040U, 12341U, 12236U, 12501U, 
    22289U, 14324U, 22321U, 14340U, 23362U, 21754U, 24843U, 22085U, 
    22115U, 24049U, 3245U, 4768U, 7606U, 18828U, 7400U, 24022U, 
    3216U, 4738U, 7578U, 10198U, 21956U, 13613U, 2691U, 19943U, 
    3024U, 9137U, 21944U, 13624U, 2704U, 20254U, 21766U, 24856U, 
    12270U, 8862U, 24218U, 3347U, 22104U, 362U, 2239U, 13746U, 
    2842U, 21147U, 594U, 2528U, 13805U, 2986U, 21501U, 509U, 
    2443U, 13783U, 2960U, 21385U, 11670U, 633U, 21534U, 22134U, 
    14220U, 24182U, 20642U, 24197U, 8715U, 3427U, 24309U, 24347U, 
    24390U, 24734U, 24355U, 24434U, 3304U, 3313U, 19294U, 19485U, 
    19569U, 19594U, 19785U, 24366U, 24465U, 7641U, 1932U, 20143U, 
    20229U, 3449U, 17119U, 19307U, 19498U, 19582U, 19607U, 19798U, 
    23994U, 23986U, 24768U, 24139U, 24799U, 24167U, 24945U, 24974U, 
    24755U, 24125U, 24786U, 24153U, 24930U, 24960U, 4468U, 3927U, 
    3942U, 4480U, 4695U, 14088U, 11992U, 11974U, 12008U, 12024U, 
    12067U, 2661U, 8763U, 1877U, 16325U, 6253U, 17043U, 6376U, 
    20002U, 17158U, 24324U, 24402U, 24745U, 24378U, 24476U, 24070U, 
    24872U, 24988U, 24074U, 24992U, 19470U, 19554U, 19770U, 12460U, 
    16581U, 8985U, 1230U, 23290U, 18703U, 10187U, 10039U, 16073U, 
    5454U, 17238U, 15038U, 15439U, 17410U, 7286U, 12261U, 1333U, 
    12378U, 1388U, 23033U, 1746U, 22607U, 1718U, 12333U, 16477U, 
    12440U, 16568U, 23272U, 18691U, 10063U, 16084U, 9192U, 10400U, 
    12740U, 22360U, 9600U, 11187U, 13229U, 23317U, 9792U, 11552U, 
    13478U, 23666U, 15656U, 5194U, 8853U, 1135U, 8963U, 1198U, 
    7256U, 12525U, 1476U, 9065U, 15980U, 12480U, 1448U, 9005U, 
    15927U, 9406U, 10764U, 12925U, 22725U, 9862U, 11633U, 13548U, 
    23756U, 10096U, 16093U, 9176U, 10383U, 12724U, 22343U, 16121U, 
    5676U, 24079U, 18852U, 24890U, 18942U, 24823U, 18906U, 18983U, 
    6434U, 10032U, 5442U, 10259U, 15467U, 5035U, 2055U, 15494U, 
    5068U, 9281U, 6308U, 16128U, 5685U, 22205U, 24084U, 3268U, 
    18860U, 10293U, 1290U, 9505U, 11034U, 13078U, 23119U, 9697U, 
    11399U, 13336U, 23513U, 13685U, 10026U, 5431U, 6336U, 9582U, 
    11169U, 13202U, 23299U, 9774U, 11534U, 13460U, 23648U, 9530U, 
    11108U, 13141U, 23193U, 9722U, 11473U, 13399U, 23587U, 3995U, 
    3871U, 4379U, 4023U, 3820U, 4319U, 3887U, 4682U, 4621U, 
    15451U, 24822U, 13848U, 9932U, 5310U, 18982U, 1651U, 67U, 
    205U, 199U, 213U, 9903U, 4975U, 23955U, 5617U, 11925U, 
    14024U, 16297U, 23929U, 5580U, 21611U, 14234U, 18215U, 23962U, 
    5630U, 23936U, 5593U, 9342U, 10714U, 12875U, 22675U, 9476U, 
    10959U, 13049U, 22948U, 5273U, 20382U, 3044U, 10058U, 1972U, 
    5477U, 14224U, 15701U, 9967U, 5370U, 10120U, 5022U, 2031U, 
    16111U, 5643U, 17459U, 9906U, 1944U, 5281U, 10074U, 1991U, 
    5516U, 23919U, 3110U, 13856U, 9943U, 5331U, 14193U, 17731U, 
    16878U, 10102U, 2007U, 5558U, 14300U, 18789U, 23976U, 3142U, 
    9975U, 5383U, 10126U, 2039U, 5654U, 14312U, 18812U, 9318U, 
    10690U, 12851U, 22651U, 9448U, 10813U, 12952U, 22768U, 9372U, 
    10730U, 12891U, 22691U, 9490U, 11003U, 13063U, 22992U, 22066U, 
    18336U, 20016U, 6454U, 23924U, 3117U, 9951U, 5344U, 10108U, 
    2015U, 5569U, 14306U, 18797U, 10069U, 1984U, 5506U, 10080U, 
    1999U, 5527U, 23981U, 3149U, 13863U, 9983U, 5396U, 14201U, 
    17742U, 16888U, 10132U, 2047U, 5665U, 14318U, 18820U, 9421U, 
    22254U, 9414U, 22242U, 11823U, 2624U, 9911U, 1951U, 5291U, 
    9288U, 10668U, 12829U, 22629U, 9260U, 10647U, 12808U, 22533U, 
    13999U, 15691U, 9959U, 5357U, 10114U, 5009U, 2023U, 16102U, 
    5606U, 16228U, 10086U, 5538U, 9897U, 11707U, 13583U, 22248U, 
    23813U, 10091U, 5548U, 696U, 1810U, 14980U, 13826U, 15522U, 
    5090U, 16862U, 6365U, 9296U, 22218U, 9390U, 10748U, 12909U, 
    22709U, 9669U, 11276U, 13308U, 23430U, 9892U, 11693U, 13578U, 
    22237U, 23808U, 4406U, 4052U, 4418U, 4065U, 4394U, 4039U, 
    4305U, 4730U, 4229U, 4722U, 4220U, 11784U, 11763U, 16156U, 
    16182U, 6208U, 7522U, 2334U, 5883U, 21240U, 6834U, 663U, 
    2591U, 6140U, 18529U, 21560U, 18179U, 7115U, 9327U, 10699U, 
    12860U, 22660U, 9463U, 10883U, 12975U, 22851U, 83U, 14429U, 
    15022U, 8726U, 20846U, 883U, 916U, 970U, 11026U, 6025U, 
    21377U, 6976U, 9538U, 11116U, 13149U, 23201U, 9730U, 11481U, 
    13407U, 23595U, 9522U, 11100U, 13133U, 23185U, 9714U, 11465U, 
    13391U, 23579U, 19939U, 17126U, 6445U, 7554U, 9556U, 11134U, 
    13167U, 23219U, 9748U, 11499U, 13425U, 23613U, 9617U, 11204U, 
    13246U, 23334U, 9820U, 11580U, 13506U, 23694U, 23951U, 18805U, 
    7379U, 7571U, 8900U, 1149U, 8802U, 1103U, 9015U, 1245U, 
    8913U, 15885U, 8815U, 15812U, 9028U, 15940U, 8931U, 15900U, 
    8833U, 15827U, 9046U, 15955U, 5802U, 6729U, 10943U, 6011U, 
    12347U, 16488U, 21361U, 6962U, 10639U, 21166U, 10545U, 5773U, 
    12242U, 16407U, 21092U, 6700U, 11344U, 6084U, 12507U, 16608U, 
    21462U, 7035U, 10606U, 5787U, 21101U, 6714U, 10896U, 5965U, 
    21315U, 6916U, 10498U, 5727U, 21046U, 6654U, 11297U, 6038U, 
    21416U, 6989U, 10922U, 5980U, 21325U, 6931U, 10524U, 5742U, 
    21056U, 6669U, 11323U, 6053U, 21426U, 7004U, 10856U, 5936U, 
    21273U, 6887U, 10933U, 5996U, 21344U, 6947U, 10535U, 5758U, 
    21075U, 6685U, 11334U, 6069U, 21445U, 7020U, 10866U, 5951U, 
    21290U, 6902U, 9652U, 11259U, 13291U, 23413U, 9844U, 13530U, 
    23738U, 8740U, 8747U, 10174U, 12159U, 22268U, 10138U, 12142U, 
    22261U, 108U, 14445U, 15030U, 8733U, 20935U, 17777U, 22295U, 
    18448U, 14330U, 20945U, 17790U, 22327U, 18461U, 6408U, 2357U, 
    15224U, 5912U, 21259U, 17984U, 6863U, 499U, 14841U, 2433U, 
    15312U, 14346U, 6421U, 23368U, 18715U, 7354U, 686U, 14967U, 
    2614U, 15426U, 21579U, 18204U, 7144U, 481U, 14817U, 2415U, 
    15288U, 21354U, 18038U, 354U, 14678U, 2231U, 15176U, 21141U, 
    17920U, 327U, 14642U, 2204U, 15152U, 21085U, 17890U, 556U, 
    14893U, 2490U, 15364U, 21455U, 18091U, 286U, 14589U, 2163U, 
    15099U, 21021U, 17845U, 296U, 14602U, 2173U, 15112U, 21029U, 
    17856U, 414U, 14729U, 2291U, 15199U, 21205U, 17963U, 526U, 
    14854U, 2460U, 15325U, 21400U, 18058U, 307U, 14616U, 2184U, 
    15126U, 21038U, 17868U, 536U, 14867U, 2470U, 15338U, 21408U, 
    18069U, 461U, 14791U, 2395U, 15262U, 21307U, 18016U, 336U, 
    14654U, 2213U, 15164U, 21127U, 17900U, 471U, 14804U, 2405U, 
    15275U, 21336U, 18027U, 317U, 14629U, 2194U, 15139U, 21067U, 
    17879U, 546U, 14880U, 2480U, 15351U, 21437U, 18080U, 433U, 
    14754U, 2367U, 15237U, 21283U, 17995U, 4503U, 4342U, 3969U, 
    10186U, 24078U, 24889U, 24828U, 13608U, 18989U, 13674U, 13684U, 
    20015U, 19938U, 7553U, 23950U, 7570U, 22073U, 22054U, 21648U, 
    18223U, 7196U, 21908U, 926U, 18976U, 22048U, 13595U, 20851U, 
    18970U, 22027U, 22074U, 22055U, 7307U, 7317U, 9636U, 11243U, 
    13265U, 23387U, 9828U, 11608U, 13514U, 23722U, 16744U, 6310U, 
    8721U, 7485U, 0U, 88U, 1042U, 19029U, 6U, 10322U, 
    21968U, 21777U, 40U, 125U, 1048U, 19042U, 27U, 12713U, 
    21989U, 14262U, 21760U, 24849U, 22015U, 7545U, 11160U, 13193U, 
    23254U, 11525U, 13451U, 23639U, 11082U, 13115U, 23167U, 11447U, 
    13373U, 23561U, 12589U, 1533U, 12664U, 1585U, 22790U, 18539U, 
    12626U, 16657U, 13914U, 16951U, 20443U, 17534U, 12690U, 1621U, 
    12569U, 1503U, 20099U, 12603U, 1552U, 12677U, 1603U, 22802U, 
    18554U, 12652U, 16689U, 11052U, 13096U, 23137U, 11417U, 13354U, 
    23531U, 13925U, 16965U, 20454U, 17548U, 12700U, 1636U, 12617U, 
    1571U, 20758U, 875U, 22193U, 12149U, 22091U, 10231U, 14214U, 
    861U, 906U, 22145U, 7870U, 221U, 22060U, 14229U, 868U, 
    22151U, 9125U, 24816U, 22121U, 20367U, 11848U, 6526U, 4589U, 
    4567U, 8759U, 15799U, 5225U, 13600U, 16752U, 6318U, 21781U, 
    21649U, 9991U, 5409U, 18224U, 7197U, 20371U, 11853U, 6535U, 
    21909U, 20347U, 20035U, 22179U, 18399U, 17183U, 22187U, 18410U, 
    17437U, 23099U, 18653U, 23062U, 18627U, 13273U, 16722U, 23793U, 
    18739U, 23089U, 18640U, 23043U, 18602U, 13211U, 16710U, 23000U, 
    18580U, 20930U, 890U, 18262U, 7228U, 490U, 14829U, 2424U, 
    15300U, 21370U, 18048U, 10447U, 267U, 14564U, 2144U, 15087U, 
    21006U, 17824U, 6642U, 22407U, 10592U, 22512U, 10882U, 22850U, 
    11025U, 23110U, 10484U, 22444U, 11283U, 23437U, 452U, 14779U, 
    2386U, 897U, 15010U, 10554U, 22474U, 10973U, 22962U, 10906U, 
    22864U, 10508U, 22458U, 11307U, 23451U, 10568U, 22488U, 10842U, 
    22821U, 11656U, 615U, 14943U, 2549U, 15402U, 21520U, 18133U, 
    7061U, 23779U, 12967U, 22835U, 10355U, 22313U, 10791U, 22746U, 
    10987U, 22976U, 11212U, 23342U, 11588U, 23702U, 10800U, 22755U, 
    10996U, 22985U, 9429U, 10778U, 12933U, 22733U, 10347U, 22305U, 
    2345U, 5897U, 21249U, 6848U, 674U, 2602U, 6154U, 18569U, 
    21569U, 18191U, 7129U, 10454U, 22414U, 10409U, 22369U, 11686U, 
    23801U, 10391U, 22351U, 10835U, 22814U, 624U, 14955U, 2558U, 
    15414U, 21527U, 18143U, 7073U, 10425U, 22385U, 10821U, 424U, 
    14742U, 2301U, 15212U, 21213U, 17974U, 6792U, 22776U, 383U, 
    14689U, 2260U, 15187U, 21174U, 17929U, 6769U, 13832U, 10875U, 
    22843U, 11360U, 23474U, 11385U, 23499U, 10599U, 22519U, 10889U, 
    22857U, 10491U, 22451U, 11290U, 23444U, 10561U, 22481U, 10980U, 
    22969U, 11377U, 565U, 14905U, 2499U, 15376U, 21478U, 18101U, 
    23491U, 10418U, 248U, 14539U, 2125U, 15075U, 20991U, 17803U, 
    6630U, 22378U, 10914U, 22872U, 10516U, 22466U, 11315U, 23459U, 
    10576U, 22496U, 10849U, 22828U, 11222U, 23352U, 11598U, 23712U, 
    12981U, 22880U, 11062U, 23147U, 11427U, 23541U, 13698U, 6354U, 
    21902U, 7217U, 11073U, 13106U, 23158U, 11438U, 13364U, 23552U, 
    11043U, 13087U, 23128U, 11408U, 13345U, 23522U, 14097U, 17055U, 
    9877U, 11648U, 13563U, 23771U, 9456U, 10828U, 12960U, 22783U, 
    9237U, 10476U, 12785U, 22436U, 9498U, 11011U, 13071U, 23007U, 
    20847U, 9683U, 11368U, 13322U, 23482U, 22200U, 9245U, 10584U, 
    12793U, 22504U, 18428U, 17753U, 6597U, 13824U, 13852U, 20632U, 
    15598U, 3055U, 4996U, 5234U, 5487U, 15617U, 17760U, 8783U, 
    1901U, 17651U, 17723U, 23884U, 18766U, 16871U, 9937U, 1964U, 
    5320U, 9916U, 1958U, 5301U, 20629U, 15590U, 3050U, 20337U, 
    15458U, 4985U, 6500U, 8777U, 1893U, 7458U, 7493U, 17645U, 
    16857U, 4313U, 3435U, 4246U, 4238U, 4452U, 4284U, 8791U, 
    1911U, 11722U, 16135U, 15637U, 23889U, 18774U, 7237U, 15806U, 
    5248U, 24812U, 3378U, 11915U, 16281U, 18891U, 10239U, 73U, 
    1825U, 4937U, 14413U, 1032U, 4855U, 7426U, 1790U, 9275U, 
    10662U, 12823U, 22623U, 13936U, 10021U, 20490U, 153U, 1839U, 
    9224U, 10463U, 12772U, 22423U, 24896U, 3397U, 18938U, 12176U, 
    2681U, 11790U, 16191U, 15666U, 23880U, 18759U, 16355U, 24834U, 
    3384U, 11920U, 16289U, 18914U, 15514U, 5080U, 14094U, 3007U, 
    7477U, 10242U, 7515U, 11809U, 16205U, 17049U, 6382U, 7539U, 
    8722U, 5216U, 7486U, 6345U, 165U, 1855U, 14477U, 24121U, 
    3289U, 18884U, 22276U, 15675U, 3100U, 143U, 14461U, 1054U, 
    4917U, 7442U, 1800U, 20184U, 17269U, 11907U, 16267U, 17211U, 
    14292U, 3012U, 11813U, 16212U, 17098U, 15551U, 5125U, 14992U, 
    10053U, 5467U, 17445U, 20799U, 3082U, 11866U, 16242U, 17709U, 
    17421U, 10047U, 18321U, 23894U, 177U, 14495U, 17769U, 18782U, 
    18436U, 7338U, 24868U, 3722U, 3786U, 3754U, 3803U, 4332U, 
    4073U, 3958U, 4432U, 4089U, 3839U, 3901U, 10204U, 10624U, 
    5829U, 21119U, 6756U, 13023U, 22922U, 21962U, 20317U, 17384U, 
    18278U, 9861U, 11632U, 13547U, 23755U, 277U, 14577U, 2154U, 
    20205U, 17299U, 16120U, 12996U, 22895U, 21014U, 17835U, 14030U, 
    16979U, 20563U, 17581U, 14056U, 17011U, 20589U, 17613U, 10376U, 
    5715U, 20984U, 6618U, 9565U, 11143U, 13176U, 23228U, 9757U, 
    11508U, 13434U, 23622U, 9201U, 10410U, 12749U, 22370U, 11687U, 
    6169U, 21587U, 9644U, 11251U, 13283U, 7157U, 23405U, 9836U, 
    11616U, 13522U, 23730U, 1U, 4793U, 89U, 2061U, 14437U, 
    4835U, 1043U, 4875U, 7U, 14375U, 4803U, 228U, 14513U, 
    2070U, 15049U, 4885U, 13U, 14384U, 13619U, 15504U, 2697U, 
    20245U, 17311U, 16766U, 19949U, 15569U, 3030U, 20280U, 17335U, 
    17133U, 20753U, 10363U, 5703U, 20977U, 6606U, 9547U, 11125U, 
    13158U, 23210U, 9739U, 11490U, 13416U, 23604U, 9184U, 10392U, 
    12732U, 22352U, 10836U, 5925U, 21267U, 9574U, 11152U, 13185U, 
    6876U, 23246U, 9766U, 11517U, 13443U, 23631U, 10323U, 9123U, 
    16047U, 21969U, 7266U, 5694U, 9514U, 11092U, 13125U, 23177U, 
    9706U, 11457U, 13383U, 23571U, 15476U, 5046U, 17193U, 6478U, 
    22211U, 345U, 14666U, 2222U, 13739U, 2833U, 16817U, 21134U, 
    17910U, 2099U, 13717U, 2726U, 20955U, 392U, 14701U, 2269U, 
    13765U, 2863U, 16827U, 21181U, 17939U, 585U, 14931U, 2519U, 
    13798U, 2977U, 16837U, 21494U, 18123U, 2112U, 13728U, 2739U, 
    20966U, 403U, 14715U, 2280U, 13774U, 2874U, 16847U, 21190U, 
    17951U, 9143U, 10616U, 5816U, 21111U, 6743U, 13013U, 22912U, 
    21950U, 20307U, 17371U, 18269U, 9853U, 11624U, 13539U, 23747U, 
    258U, 14552U, 2135U, 20196U, 17287U, 16057U, 12987U, 22886U, 
    20999U, 17814U, 41U, 4825U, 126U, 2090U, 14453U, 4845U, 
    1049U, 4907U, 28U, 14404U, 4814U, 238U, 14526U, 2080U, 
    15062U, 4896U, 20U, 14394U, 13649U, 2719U, 20271U, 17323U, 
    16774U, 20072U, 17219U, 19966U, 3037U, 20289U, 17347U, 17150U, 
    57U, 189U, 1072U, 62U, 194U, 1077U, 20775U, 12714U, 
    21990U, 7276U, 6288U, 14263U, 14068U, 17026U, 20601U, 17628U, 
    13870U, 16898U, 20399U, 17481U, 12400U, 16528U, 23079U, 1761U, 
    12450U, 1418U, 23281U, 1776U, 13033U, 22932U, 12638U, 16672U, 
    12578U, 1517U, 21761U, 20327U, 17397U, 20298U, 17359U, 18254U, 
    24850U, 18929U, 22010U, 7297U, 9885U, 11663U, 13571U, 23786U, 
    17091U, 6399U, 12318U, 1360U, 13005U, 22904U, 7546U, 12421U, 
    1404U, 4298U, 4425U, 3612U, 3417U, 9209U, 10433U, 12757U, 
    22393U, 9216U, 10440U, 12764U, 22400U, 276U, 14576U, 2153U, 
    21013U, 17834U, 257U, 14551U, 2134U, 20998U, 17813U, 20637U, 
    3062U, 9358U, 17659U, 6544U, 22225U, 15628U, 5171U, 4493U, 
    4534U, 20638U, 15609U, 5161U, 3063U, 9359U, 6327U, 17660U, 
    6545U, 22226U, 24100U, 3276U, 18869U, 4633U, 12308U, 16453U, 
    11876U, 16258U, 6219U, 9869U, 11640U, 13555U, 23763U, 9229U, 
    10468U, 12777U, 22428U, 9676U, 11353U, 13315U, 23467U, 12276U, 
    16420U, 8868U, 15841U, 20100U, 13899U, 7817U, 15729U, 16933U, 
    20428U, 7854U, 15766U, 17516U, 14043U, 16995U, 20576U, 17597U, 
    13885U, 7802U, 15711U, 16916U, 20414U, 7839U, 15748U, 17499U, 
    12220U, 16388U, 22555U, 18496U, 12195U, 16372U, 22584U, 18511U, 
    12208U, 1302U, 22540U, 1663U, 22567U, 1683U, 11945U, 11770U, 
    16166U, 18750U, 16305U, 6230U, 7531U, 10285U, 1277U, 9109U, 
    16027U, 20158U, 17251U, 20793U, 3074U, 17700U, 6586U, 6562U, 
    442U, 14766U, 2376U, 15249U, 21299U, 18005U, 12551U, 16645U, 
    9091U, 16003U, 12292U, 16442U, 8884U, 15863U, 11361U, 6098U, 
    21471U, 7049U, 20759U, 22194U, 18419U, 17673U, 2322U, 5868U, 
    21230U, 6819U, 651U, 2579U, 6125U, 18485U, 21550U, 18166U, 
    7100U, 575U, 14918U, 2509U, 15389U, 21486U, 18112U, 4253U, 
    3629U, 9609U, 11196U, 13238U, 23326U, 9812U, 11572U, 13498U, 
    23686U, 9120U, 15450U, 4965U, 1916U, 11726U, 16142U, 16041U, 
    5256U, 10066U, 1979U, 7469U, 10182U, 7508U, 11751U, 16149U, 
    16087U, 5498U, 7502U, 10344U, 20024U, 15579U, 5147U, 17166U, 
    6467U, 7561U, 78U, 1832U, 4951U, 14421U, 1037U, 4865U, 
    7434U, 1795U, 22092U, 18346U, 13940U, 20494U, 159U, 1847U, 
    8755U, 1871U, 15792U, 12167U, 2675U, 16348U, 23969U, 3133U, 
    11713U, 6194U, 7388U, 21602U, 7182U, 23943U, 3124U, 11698U, 
    6180U, 7367U, 21593U, 7168U, 10772U, 5842U, 21199U, 6781U, 
    20386U, 24094U, 12710U, 15486U, 5058U, 2686U, 11794U, 16198U, 
    9254U, 12802U, 22527U, 19954U, 22172U, 18389U, 17141U, 12542U, 
    16633U, 9082U, 15991U, 12490U, 16594U, 23395U, 18726U, 12284U, 
    16431U, 8876U, 15852U, 12411U, 16542U, 23237U, 18667U, 12533U, 
    16621U, 9073U, 1263U, 12389U, 16514U, 8974U, 1214U, 23052U, 
    18614U, 12187U, 16361U, 8794U, 1090U, 12357U, 16501U, 8942U, 
    1167U, 23014U, 18590U, 12560U, 1489U, 9100U, 16015U, 12300U, 
    1347U, 8892U, 15874U, 16704U, 6280U, 9274U, 10661U, 12822U, 
    22622U, 9223U, 10462U, 12771U, 22422U, 14215U, 15533U, 5103U, 
    907U, 3193U, 9304U, 10676U, 12837U, 22637U, 22146U, 18362U, 
    9436U, 10785U, 12940U, 17077U, 6390U, 22740U, 21746U, 3094U, 
    18240U, 24104U, 3282U, 18876U, 24924U, 3409U, 18951U, 24838U, 
    3390U, 18921U, 11827U, 24089U, 9381U, 10739U, 12900U, 22700U, 
    9661U, 11268U, 13300U, 23422U, 7871U, 9267U, 10654U, 12815U, 
    22615U, 9334U, 10706U, 12867U, 22667U, 9469U, 10952U, 13042U, 
    22941U, 22061U, 18328U, 9170U, 10370U, 12718U, 15785U, 22337U, 
    14230U, 15542U, 5114U, 9311U, 10683U, 12844U, 22644U, 9350U, 
    10722U, 12883U, 22683U, 9483U, 10966U, 13056U, 22955U, 22152U, 
    18370U, 9442U, 10807U, 12946U, 17084U, 22762U, 20093U, 17229U, 
    6489U, 9692U, 11394U, 13331U, 23508U, 9126U, 12252U, 1319U, 
    12367U, 1372U, 23023U, 1731U, 22599U, 1705U, 12325U, 16466U, 
    12430U, 16555U, 23263U, 18679U, 9801U, 11561U, 13487U, 23675U, 
    9625U, 11232U, 13254U, 23376U, 9591U, 11178U, 13220U, 23308U, 
    9783U, 11543U, 13469U, 23657U, 15646U, 5182U, 8844U, 1121U, 
    8952U, 1182U, 7246U, 12517U, 1463U, 9057U, 15969U, 12470U, 
    1433U, 8995U, 15914U, 9398U, 10756U, 12917U, 22717U, 9854U, 
    11625U, 13540U, 23748U, 16050U, 5264U, 24817U, 18898U, 171U, 
    1863U, 14486U, 22286U, 15683U, 5206U, 3105U, 148U, 14469U, 
    1059U, 4927U, 7450U, 1805U, 20190U, 17278U, 11911U, 16274U, 
    14296U, 3018U, 11818U, 16220U, 17105U, 15560U, 5136U, 15001U, 
    17452U, 20803U, 3088U, 11871U, 16250U, 17716U, 17429U, 6516U, 
    183U, 14504U, 18442U, 7346U, 10016U, 13589U, 16735U, 6297U, 
    16065U, 5421U, 14105U, 17066U, 4277U, 3640U, 3738U, 4444U, 
    4460U, 3770U, 3708U, 4599U, 4513U, 4354U, 3982U, 4366U, 
    4009U, 4544U, 3622U, 4578U, 3715U, 4610U, 4669U, 3855U, 
    3914U, 20390U, 13660U, 16782U, 17466U, 11780U, 13598U, 21995U, 
    18295U, 16750U, 21979U, 18287U, 16175U, 22164U, 11958U, 16313U, 
    18378U, 20066U, 17202U, 20523U, 17572U, 13710U, 16807U, 20786U, 
    17690U, 22157U, 11950U, 6240U, 7326U, 20030U, 17175U, 20394U, 
    17473U, 13704U, 16798U, 20780U, 17681U, 21750U, 13679U, 18304U, 
    16790U, 22043U, 18313U, 18247U, 11831U, 13603U, 16758U, 16235U, 
    2310U, 5853U, 21220U, 6804U, 639U, 2567U, 6110U, 18474U, 
    21540U, 18153U, 7085U, 22001U, 22122U, 18354U, 22008U, 52U, 
    22021U, 9253U, 10632U, 12801U, 22526U, 21672U, 18232U, 7207U, 
    20164U, 17260U, 6574U, 12154U, 16340U, 6270U, 20665U, 15608U, 
    5160U, 3068U, 9365U, 6326U, 17666U, 6553U, 22231U, 24099U, 
    3275U, 18868U, 4644U, 10236U, 
};

static inline void InitMipsMCInstrInfo(MCInstrInfo *II) {
  II->InitMCInstrInfo(MipsInsts, MipsInstrNameIndices, MipsInstrNameData, 2628);
}

} // end llvm namespace
#endif // GET_INSTRINFO_MC_DESC

#ifdef GET_INSTRINFO_HEADER
#undef GET_INSTRINFO_HEADER
namespace llvm {
struct MipsGenInstrInfo : public TargetInstrInfo {
  explicit MipsGenInstrInfo(int CFSetupOpcode = -1, int CFDestroyOpcode = -1, int CatchRetOpcode = -1, int ReturnOpcode = -1);
  ~MipsGenInstrInfo() override = default;

};
} // end llvm namespace
#endif // GET_INSTRINFO_HEADER

#ifdef GET_INSTRINFO_CTOR_DTOR
#undef GET_INSTRINFO_CTOR_DTOR
namespace llvm {
extern const MCInstrDesc MipsInsts[];
extern const unsigned MipsInstrNameIndices[];
extern const char MipsInstrNameData[];
MipsGenInstrInfo::MipsGenInstrInfo(int CFSetupOpcode, int CFDestroyOpcode, int CatchRetOpcode, int ReturnOpcode)
  : TargetInstrInfo(CFSetupOpcode, CFDestroyOpcode, CatchRetOpcode, ReturnOpcode) {
  InitMCInstrInfo(MipsInsts, MipsInstrNameIndices, MipsInstrNameData, 2628);
}
} // end llvm namespace
#endif // GET_INSTRINFO_CTOR_DTOR

#ifdef GET_INSTRINFO_OPERAND_ENUM
#undef GET_INSTRINFO_OPERAND_ENUM
namespace llvm {
namespace Mips {
namespace OpName {
enum {
OPERAND_LAST
};
} // end namespace OpName
} // end namespace Mips
} // end namespace llvm
#endif //GET_INSTRINFO_OPERAND_ENUM

#ifdef GET_INSTRINFO_NAMED_OPS
#undef GET_INSTRINFO_NAMED_OPS
namespace llvm {
namespace Mips {
LLVM_READONLY
int16_t getNamedOperandIdx(uint16_t Opcode, uint16_t NamedIdx) {
  return -1;
}
} // end namespace Mips
} // end namespace llvm
#endif //GET_INSTRINFO_NAMED_OPS

#ifdef GET_INSTRINFO_OPERAND_TYPES_ENUM
#undef GET_INSTRINFO_OPERAND_TYPES_ENUM
namespace llvm {
namespace Mips {
namespace OpTypes {
enum OperandType {
  InvertedImOperand = 0,
  InvertedImOperand64 = 1,
  PtrRC = 2,
  brtarget = 3,
  brtarget10_mm = 4,
  brtarget1SImm16 = 5,
  brtarget21 = 6,
  brtarget21_mm = 7,
  brtarget26 = 8,
  brtarget26_mm = 9,
  brtarget7_mm = 10,
  brtarget_lsl2_mm = 11,
  brtarget_mm = 12,
  brtargetr6 = 13,
  calloffset16 = 14,
  calltarget = 15,
  calltarget_mm = 16,
  condcode = 17,
  cpinst_operand = 18,
  f32imm = 19,
  f64imm = 20,
  i16imm = 21,
  i1imm = 22,
  i32imm = 23,
  i64imm = 24,
  i8imm = 25,
  imm64 = 26,
  jmpoffset16 = 27,
  jmptarget = 28,
  jmptarget_mm = 29,
  li16_imm = 30,
  mem = 31,
  mem16 = 32,
  mem16_ea = 33,
  mem16sp = 34,
  mem_ea = 35,
  mem_mm_11 = 36,
  mem_mm_12 = 37,
  mem_mm_16 = 38,
  mem_mm_4 = 39,
  mem_mm_4_lsl1 = 40,
  mem_mm_4_lsl2 = 41,
  mem_mm_4sp = 42,
  mem_mm_9 = 43,
  mem_mm_gp_simm7_lsl2 = 44,
  mem_mm_sp_imm5_lsl2 = 45,
  mem_msa = 46,
  mem_simm10 = 47,
  mem_simm10_lsl1 = 48,
  mem_simm10_lsl2 = 49,
  mem_simm10_lsl3 = 50,
  mem_simm11 = 51,
  mem_simm12 = 52,
  mem_simm16 = 53,
  mem_simm9 = 54,
  mem_simmptr = 55,
  movep_regpair = 56,
  pcrel16 = 57,
  ptype0 = 58,
  ptype1 = 59,
  ptype2 = 60,
  ptype3 = 61,
  ptype4 = 62,
  ptype5 = 63,
  reglist = 64,
  reglist16 = 65,
  simm10 = 66,
  simm10_64 = 67,
  simm10_lsl1 = 68,
  simm10_lsl2 = 69,
  simm10_lsl3 = 70,
  simm11 = 71,
  simm12 = 72,
  simm16 = 73,
  simm16_64 = 74,
  simm16_relaxed = 75,
  simm18_lsl3 = 76,
  simm19_lsl2 = 77,
  simm23_lsl2 = 78,
  simm32 = 79,
  simm32_relaxed = 80,
  simm3_lsa2 = 81,
  simm4 = 82,
  simm5 = 83,
  simm6 = 84,
  simm7_lsl2 = 85,
  simm9 = 86,
  simm9_addiusp = 87,
  size_ins = 88,
  type0 = 89,
  type1 = 90,
  type2 = 91,
  type3 = 92,
  type4 = 93,
  type5 = 94,
  uimm1 = 95,
  uimm10 = 96,
  uimm16 = 97,
  uimm16_64 = 98,
  uimm16_64_relaxed = 99,
  uimm16_altrelaxed = 100,
  uimm16_relaxed = 101,
  uimm1_ptr = 102,
  uimm2 = 103,
  uimm20 = 104,
  uimm26 = 105,
  uimm2_plus1 = 106,
  uimm2_ptr = 107,
  uimm3 = 108,
  uimm32_coerced = 109,
  uimm3_ptr = 110,
  uimm3_shift = 111,
  uimm4 = 112,
  uimm4_andi = 113,
  uimm4_ptr = 114,
  uimm5 = 115,
  uimm5_64 = 116,
  uimm5_64_report_uimm6 = 117,
  uimm5_inssize_plus1 = 118,
  uimm5_lsl2 = 119,
  uimm5_plus1 = 120,
  uimm5_plus1_report_uimm6 = 121,
  uimm5_plus32 = 122,
  uimm5_plus32_normalize = 123,
  uimm5_plus32_normalize_64 = 124,
  uimm5_plus33 = 125,
  uimm5_report_uimm6 = 126,
  uimm6 = 127,
  uimm6_lsl2 = 128,
  uimm7 = 129,
  uimm8 = 130,
  uimm_range_2_64 = 131,
  uimmz = 132,
  vsplat_simm10 = 133,
  vsplat_simm5 = 134,
  vsplat_uimm1 = 135,
  vsplat_uimm2 = 136,
  vsplat_uimm3 = 137,
  vsplat_uimm4 = 138,
  vsplat_uimm5 = 139,
  vsplat_uimm6 = 140,
  vsplat_uimm8 = 141,
  OPERAND_TYPE_LIST_END
};
} // end namespace OpTypes
} // end namespace Mips
} // end namespace llvm
#endif // GET_INSTRINFO_OPERAND_TYPES_ENUM

#ifdef GET_INSTRMAP_INFO
#undef GET_INSTRMAP_INFO
namespace llvm {

namespace Mips {

enum Arch {
	Arch_dsp,
	Arch_mmdsp,
	Arch_mipsr6,
	Arch_micromipsr6,
	Arch_se,
	Arch_micromips
};

// Dsp2MicroMips
LLVM_READONLY
int Dsp2MicroMips(uint16_t Opcode, enum Arch inArch) {
static const uint16_t Dsp2MicroMipsTable[][3] = {
  { Mips::ABSQ_S_PH, Mips::ABSQ_S_PH, Mips::ABSQ_S_PH_MM },
  { Mips::ABSQ_S_QB, Mips::ABSQ_S_QB, Mips::ABSQ_S_QB_MMR2 },
  { Mips::ABSQ_S_W, Mips::ABSQ_S_W, Mips::ABSQ_S_W_MM },
  { Mips::ADDQH_PH, Mips::ADDQH_PH, Mips::ADDQH_PH_MMR2 },
  { Mips::ADDQH_R_PH, Mips::ADDQH_R_PH, Mips::ADDQH_R_PH_MMR2 },
  { Mips::ADDQH_R_W, Mips::ADDQH_R_W, Mips::ADDQH_R_W_MMR2 },
  { Mips::ADDQH_W, Mips::ADDQH_W, Mips::ADDQH_W_MMR2 },
  { Mips::ADDQ_PH, Mips::ADDQ_PH, Mips::ADDQ_PH_MM },
  { Mips::ADDQ_S_PH, Mips::ADDQ_S_PH, Mips::ADDQ_S_PH_MM },
  { Mips::ADDQ_S_W, Mips::ADDQ_S_W, Mips::ADDQ_S_W_MM },
  { Mips::ADDSC, Mips::ADDSC, Mips::ADDSC_MM },
  { Mips::ADDUH_QB, Mips::ADDUH_QB, Mips::ADDUH_QB_MMR2 },
  { Mips::ADDUH_R_QB, Mips::ADDUH_R_QB, Mips::ADDUH_R_QB_MMR2 },
  { Mips::ADDU_PH, Mips::ADDU_PH, Mips::ADDU_PH_MMR2 },
  { Mips::ADDU_QB, Mips::ADDU_QB, Mips::ADDU_QB_MM },
  { Mips::ADDU_S_PH, Mips::ADDU_S_PH, Mips::ADDU_S_PH_MMR2 },
  { Mips::ADDU_S_QB, Mips::ADDU_S_QB, Mips::ADDU_S_QB_MM },
  { Mips::ADDWC, Mips::ADDWC, Mips::ADDWC_MM },
  { Mips::APPEND, Mips::APPEND, Mips::APPEND_MMR2 },
  { Mips::BALIGN, Mips::BALIGN, Mips::BALIGN_MMR2 },
  { Mips::BITREV, Mips::BITREV, Mips::BITREV_MM },
  { Mips::BPOSGE32, Mips::BPOSGE32, Mips::BPOSGE32_MM },
  { Mips::CMPGDU_EQ_QB, Mips::CMPGDU_EQ_QB, Mips::CMPGDU_EQ_QB_MMR2 },
  { Mips::CMPGDU_LE_QB, Mips::CMPGDU_LE_QB, Mips::CMPGDU_LE_QB_MMR2 },
  { Mips::CMPGDU_LT_QB, Mips::CMPGDU_LT_QB, Mips::CMPGDU_LT_QB_MMR2 },
  { Mips::CMPGU_EQ_QB, Mips::CMPGU_EQ_QB, Mips::CMPGU_EQ_QB_MM },
  { Mips::CMPGU_LE_QB, Mips::CMPGU_LE_QB, Mips::CMPGU_LE_QB_MM },
  { Mips::CMPGU_LT_QB, Mips::CMPGU_LT_QB, Mips::CMPGU_LT_QB_MM },
  { Mips::CMPU_EQ_QB, Mips::CMPU_EQ_QB, Mips::CMPU_EQ_QB_MM },
  { Mips::CMPU_LE_QB, Mips::CMPU_LE_QB, Mips::CMPU_LE_QB_MM },
  { Mips::CMPU_LT_QB, Mips::CMPU_LT_QB, Mips::CMPU_LT_QB_MM },
  { Mips::CMP_EQ_PH, Mips::CMP_EQ_PH, Mips::CMP_EQ_PH_MM },
  { Mips::CMP_LE_PH, Mips::CMP_LE_PH, Mips::CMP_LE_PH_MM },
  { Mips::CMP_LT_PH, Mips::CMP_LT_PH, Mips::CMP_LT_PH_MM },
  { Mips::DPAQX_SA_W_PH, Mips::DPAQX_SA_W_PH, Mips::DPAQX_SA_W_PH_MMR2 },
  { Mips::DPAQX_S_W_PH, Mips::DPAQX_S_W_PH, Mips::DPAQX_S_W_PH_MMR2 },
  { Mips::DPAQ_SA_L_W, Mips::DPAQ_SA_L_W, Mips::DPAQ_SA_L_W_MM },
  { Mips::DPAQ_S_W_PH, Mips::DPAQ_S_W_PH, Mips::DPAQ_S_W_PH_MM },
  { Mips::DPAU_H_QBL, Mips::DPAU_H_QBL, Mips::DPAU_H_QBL_MM },
  { Mips::DPAU_H_QBR, Mips::DPAU_H_QBR, Mips::DPAU_H_QBR_MM },
  { Mips::DPAX_W_PH, Mips::DPAX_W_PH, Mips::DPAX_W_PH_MMR2 },
  { Mips::DPA_W_PH, Mips::DPA_W_PH, Mips::DPA_W_PH_MMR2 },
  { Mips::DPSQX_SA_W_PH, Mips::DPSQX_SA_W_PH, Mips::DPSQX_SA_W_PH_MMR2 },
  { Mips::DPSQX_S_W_PH, Mips::DPSQX_S_W_PH, Mips::DPSQX_S_W_PH_MMR2 },
  { Mips::DPSQ_SA_L_W, Mips::DPSQ_SA_L_W, Mips::DPSQ_SA_L_W_MM },
  { Mips::DPSQ_S_W_PH, Mips::DPSQ_S_W_PH, Mips::DPSQ_S_W_PH_MM },
  { Mips::DPSU_H_QBL, Mips::DPSU_H_QBL, Mips::DPSU_H_QBL_MM },
  { Mips::DPSU_H_QBR, Mips::DPSU_H_QBR, Mips::DPSU_H_QBR_MM },
  { Mips::DPSX_W_PH, Mips::DPSX_W_PH, Mips::DPSX_W_PH_MMR2 },
  { Mips::DPS_W_PH, Mips::DPS_W_PH, Mips::DPS_W_PH_MMR2 },
  { Mips::EXTP, Mips::EXTP, Mips::EXTP_MM },
  { Mips::EXTPDP, Mips::EXTPDP, Mips::EXTPDP_MM },
  { Mips::EXTPDPV, Mips::EXTPDPV, Mips::EXTPDPV_MM },
  { Mips::EXTPV, Mips::EXTPV, Mips::EXTPV_MM },
  { Mips::EXTRV_RS_W, Mips::EXTRV_RS_W, Mips::EXTRV_RS_W_MM },
  { Mips::EXTRV_R_W, Mips::EXTRV_R_W, Mips::EXTRV_R_W_MM },
  { Mips::EXTRV_S_H, Mips::EXTRV_S_H, Mips::EXTRV_S_H_MM },
  { Mips::EXTRV_W, Mips::EXTRV_W, Mips::EXTRV_W_MM },
  { Mips::EXTR_RS_W, Mips::EXTR_RS_W, Mips::EXTR_RS_W_MM },
  { Mips::EXTR_R_W, Mips::EXTR_R_W, Mips::EXTR_R_W_MM },
  { Mips::EXTR_S_H, Mips::EXTR_S_H, Mips::EXTR_S_H_MM },
  { Mips::EXTR_W, Mips::EXTR_W, Mips::EXTR_W_MM },
  { Mips::INSV, Mips::INSV, Mips::INSV_MM },
  { Mips::LBUX, Mips::LBUX, Mips::LBUX_MM },
  { Mips::LHX, Mips::LHX, Mips::LHX_MM },
  { Mips::LWDSP, Mips::LWDSP, Mips::LWDSP_MM },
  { Mips::LWX, Mips::LWX, Mips::LWX_MM },
  { Mips::MADDU_DSP, Mips::MADDU_DSP, Mips::MADDU_DSP_MM },
  { Mips::MADD_DSP, Mips::MADD_DSP, Mips::MADD_DSP_MM },
  { Mips::MAQ_SA_W_PHL, Mips::MAQ_SA_W_PHL, Mips::MAQ_SA_W_PHL_MM },
  { Mips::MAQ_SA_W_PHR, Mips::MAQ_SA_W_PHR, Mips::MAQ_SA_W_PHR_MM },
  { Mips::MAQ_S_W_PHL, Mips::MAQ_S_W_PHL, Mips::MAQ_S_W_PHL_MM },
  { Mips::MAQ_S_W_PHR, Mips::MAQ_S_W_PHR, Mips::MAQ_S_W_PHR_MM },
  { Mips::MFHI_DSP, Mips::MFHI_DSP, Mips::MFHI_DSP_MM },
  { Mips::MFLO_DSP, Mips::MFLO_DSP, Mips::MFLO_DSP_MM },
  { Mips::MODSUB, Mips::MODSUB, Mips::MODSUB_MM },
  { Mips::MSUBU_DSP, Mips::MSUBU_DSP, Mips::MSUBU_DSP_MM },
  { Mips::MSUB_DSP, Mips::MSUB_DSP, Mips::MSUB_DSP_MM },
  { Mips::MTHI_DSP, Mips::MTHI_DSP, Mips::MTHI_DSP_MM },
  { Mips::MTHLIP, Mips::MTHLIP, Mips::MTHLIP_MM },
  { Mips::MTLO_DSP, Mips::MTLO_DSP, Mips::MTLO_DSP_MM },
  { Mips::MULEQ_S_W_PHL, Mips::MULEQ_S_W_PHL, Mips::MULEQ_S_W_PHL_MM },
  { Mips::MULEQ_S_W_PHR, Mips::MULEQ_S_W_PHR, Mips::MULEQ_S_W_PHR_MM },
  { Mips::MULEU_S_PH_QBL, Mips::MULEU_S_PH_QBL, Mips::MULEU_S_PH_QBL_MM },
  { Mips::MULEU_S_PH_QBR, Mips::MULEU_S_PH_QBR, Mips::MULEU_S_PH_QBR_MM },
  { Mips::MULQ_RS_PH, Mips::MULQ_RS_PH, Mips::MULQ_RS_PH_MM },
  { Mips::MULQ_RS_W, Mips::MULQ_RS_W, Mips::MULQ_RS_W_MMR2 },
  { Mips::MULQ_S_PH, Mips::MULQ_S_PH, Mips::MULQ_S_PH_MMR2 },
  { Mips::MULQ_S_W, Mips::MULQ_S_W, Mips::MULQ_S_W_MMR2 },
  { Mips::MULSAQ_S_W_PH, Mips::MULSAQ_S_W_PH, Mips::MULSAQ_S_W_PH_MM },
  { Mips::MULSA_W_PH, Mips::MULSA_W_PH, Mips::MULSA_W_PH_MMR2 },
  { Mips::MULTU_DSP, Mips::MULTU_DSP, Mips::MULTU_DSP_MM },
  { Mips::MULT_DSP, Mips::MULT_DSP, Mips::MULT_DSP_MM },
  { Mips::MUL_PH, Mips::MUL_PH, Mips::MUL_PH_MMR2 },
  { Mips::MUL_S_PH, Mips::MUL_S_PH, Mips::MUL_S_PH_MMR2 },
  { Mips::PACKRL_PH, Mips::PACKRL_PH, Mips::PACKRL_PH_MM },
  { Mips::PICK_PH, Mips::PICK_PH, Mips::PICK_PH_MM },
  { Mips::PICK_QB, Mips::PICK_QB, Mips::PICK_QB_MM },
  { Mips::PRECEQU_PH_QBL, Mips::PRECEQU_PH_QBL, Mips::PRECEQU_PH_QBL_MM },
  { Mips::PRECEQU_PH_QBLA, Mips::PRECEQU_PH_QBLA, Mips::PRECEQU_PH_QBLA_MM },
  { Mips::PRECEQU_PH_QBR, Mips::PRECEQU_PH_QBR, Mips::PRECEQU_PH_QBR_MM },
  { Mips::PRECEQU_PH_QBRA, Mips::PRECEQU_PH_QBRA, Mips::PRECEQU_PH_QBRA_MM },
  { Mips::PRECEQ_W_PHL, Mips::PRECEQ_W_PHL, Mips::PRECEQ_W_PHL_MM },
  { Mips::PRECEQ_W_PHR, Mips::PRECEQ_W_PHR, Mips::PRECEQ_W_PHR_MM },
  { Mips::PRECEU_PH_QBL, Mips::PRECEU_PH_QBL, Mips::PRECEU_PH_QBL_MM },
  { Mips::PRECEU_PH_QBLA, Mips::PRECEU_PH_QBLA, Mips::PRECEU_PH_QBLA_MM },
  { Mips::PRECEU_PH_QBR, Mips::PRECEU_PH_QBR, Mips::PRECEU_PH_QBR_MM },
  { Mips::PRECEU_PH_QBRA, Mips::PRECEU_PH_QBRA, Mips::PRECEU_PH_QBRA_MM },
  { Mips::PRECRQU_S_QB_PH, Mips::PRECRQU_S_QB_PH, Mips::PRECRQU_S_QB_PH_MM },
  { Mips::PRECRQ_PH_W, Mips::PRECRQ_PH_W, Mips::PRECRQ_PH_W_MM },
  { Mips::PRECRQ_QB_PH, Mips::PRECRQ_QB_PH, Mips::PRECRQ_QB_PH_MM },
  { Mips::PRECRQ_RS_PH_W, Mips::PRECRQ_RS_PH_W, Mips::PRECRQ_RS_PH_W_MM },
  { Mips::PRECR_QB_PH, Mips::PRECR_QB_PH, Mips::PRECR_QB_PH_MMR2 },
  { Mips::PRECR_SRA_PH_W, Mips::PRECR_SRA_PH_W, Mips::PRECR_SRA_PH_W_MMR2 },
  { Mips::PRECR_SRA_R_PH_W, Mips::PRECR_SRA_R_PH_W, Mips::PRECR_SRA_R_PH_W_MMR2 },
  { Mips::PREPEND, Mips::PREPEND, Mips::PREPEND_MMR2 },
  { Mips::RADDU_W_QB, Mips::RADDU_W_QB, Mips::RADDU_W_QB_MM },
  { Mips::RDDSP, Mips::RDDSP, Mips::RDDSP_MM },
  { Mips::REPLV_PH, Mips::REPLV_PH, Mips::REPLV_PH_MM },
  { Mips::REPLV_QB, Mips::REPLV_QB, Mips::REPLV_QB_MM },
  { Mips::REPL_PH, Mips::REPL_PH, Mips::REPL_PH_MM },
  { Mips::REPL_QB, Mips::REPL_QB, Mips::REPL_QB_MM },
  { Mips::SHILO, Mips::SHILO, Mips::SHILO_MM },
  { Mips::SHILOV, Mips::SHILOV, Mips::SHILOV_MM },
  { Mips::SHLLV_PH, Mips::SHLLV_PH, Mips::SHLLV_PH_MM },
  { Mips::SHLLV_QB, Mips::SHLLV_QB, Mips::SHLLV_QB_MM },
  { Mips::SHLLV_S_PH, Mips::SHLLV_S_PH, Mips::SHLLV_S_PH_MM },
  { Mips::SHLLV_S_W, Mips::SHLLV_S_W, Mips::SHLLV_S_W_MM },
  { Mips::SHLL_PH, Mips::SHLL_PH, Mips::SHLL_PH_MM },
  { Mips::SHLL_QB, Mips::SHLL_QB, Mips::SHLL_QB_MM },
  { Mips::SHLL_S_PH, Mips::SHLL_S_PH, Mips::SHLL_S_PH_MM },
  { Mips::SHLL_S_W, Mips::SHLL_S_W, Mips::SHLL_S_W_MM },
  { Mips::SHRAV_PH, Mips::SHRAV_PH, Mips::SHRAV_PH_MM },
  { Mips::SHRAV_QB, Mips::SHRAV_QB, Mips::SHRAV_QB_MMR2 },
  { Mips::SHRAV_R_PH, Mips::SHRAV_R_PH, Mips::SHRAV_R_PH_MM },
  { Mips::SHRAV_R_QB, Mips::SHRAV_R_QB, Mips::SHRAV_R_QB_MMR2 },
  { Mips::SHRAV_R_W, Mips::SHRAV_R_W, Mips::SHRAV_R_W_MM },
  { Mips::SHRA_PH, Mips::SHRA_PH, Mips::SHRA_PH_MM },
  { Mips::SHRA_QB, Mips::SHRA_QB, Mips::SHRA_QB_MMR2 },
  { Mips::SHRA_R_PH, Mips::SHRA_R_PH, Mips::SHRA_R_PH_MM },
  { Mips::SHRA_R_QB, Mips::SHRA_R_QB, Mips::SHRA_R_QB_MMR2 },
  { Mips::SHRA_R_W, Mips::SHRA_R_W, Mips::SHRA_R_W_MM },
  { Mips::SHRLV_PH, Mips::SHRLV_PH, Mips::SHRLV_PH_MMR2 },
  { Mips::SHRLV_QB, Mips::SHRLV_QB, Mips::SHRLV_QB_MM },
  { Mips::SHRL_PH, Mips::SHRL_PH, Mips::SHRL_PH_MMR2 },
  { Mips::SHRL_QB, Mips::SHRL_QB, Mips::SHRL_QB_MM },
  { Mips::SUBQH_PH, Mips::SUBQH_PH, Mips::SUBQH_PH_MMR2 },
  { Mips::SUBQH_R_PH, Mips::SUBQH_R_PH, Mips::SUBQH_R_PH_MMR2 },
  { Mips::SUBQH_R_W, Mips::SUBQH_R_W, Mips::SUBQH_R_W_MMR2 },
  { Mips::SUBQH_W, Mips::SUBQH_W, Mips::SUBQH_W_MMR2 },
  { Mips::SUBQ_PH, Mips::SUBQ_PH, Mips::SUBQ_PH_MM },
  { Mips::SUBQ_S_PH, Mips::SUBQ_S_PH, Mips::SUBQ_S_PH_MM },
  { Mips::SUBQ_S_W, Mips::SUBQ_S_W, Mips::SUBQ_S_W_MM },
  { Mips::SUBUH_QB, Mips::SUBUH_QB, Mips::SUBUH_QB_MMR2 },
  { Mips::SUBUH_R_QB, Mips::SUBUH_R_QB, Mips::SUBUH_R_QB_MMR2 },
  { Mips::SUBU_PH, Mips::SUBU_PH, Mips::SUBU_PH_MMR2 },
  { Mips::SUBU_QB, Mips::SUBU_QB, Mips::SUBU_QB_MM },
  { Mips::SUBU_S_PH, Mips::SUBU_S_PH, Mips::SUBU_S_PH_MMR2 },
  { Mips::SUBU_S_QB, Mips::SUBU_S_QB, Mips::SUBU_S_QB_MM },
  { Mips::SWDSP, Mips::SWDSP, Mips::SWDSP_MM },
}; // End of Dsp2MicroMipsTable

  unsigned mid;
  unsigned start = 0;
  unsigned end = 160;
  while (start < end) {
    mid = start + (end - start)/2;
    if (Opcode == Dsp2MicroMipsTable[mid][0]) {
      break;
    }
    if (Opcode < Dsp2MicroMipsTable[mid][0])
      end = mid;
    else
      start = mid + 1;
  }
  if (start == end)
    return -1; // Instruction doesn't exist in this table.

  if (inArch == Arch_dsp)
    return Dsp2MicroMipsTable[mid][1];
  if (inArch == Arch_mmdsp)
    return Dsp2MicroMipsTable[mid][2];
  return -1;}

// MipsR62MicroMipsR6
LLVM_READONLY
int MipsR62MicroMipsR6(uint16_t Opcode, enum Arch inArch) {
static const uint16_t MipsR62MicroMipsR6Table[][3] = {
  { Mips::ADDIUPC, Mips::ADDIUPC, Mips::ADDIUPC_MMR6 },
  { Mips::ALIGN, Mips::ALIGN, Mips::ALIGN_MMR6 },
  { Mips::ALUIPC, Mips::ALUIPC, Mips::ALUIPC_MMR6 },
  { Mips::AUI, Mips::AUI, Mips::AUI_MMR6 },
  { Mips::AUIPC, Mips::AUIPC, Mips::AUIPC_MMR6 },
  { Mips::BALC, Mips::BALC, Mips::BALC_MMR6 },
  { Mips::BC, Mips::BC, Mips::BC_MMR6 },
  { Mips::BEQC, Mips::BEQC, Mips::BEQC_MMR6 },
  { Mips::BEQZALC, Mips::BEQZALC, Mips::BEQZALC_MMR6 },
  { Mips::BEQZC, Mips::BEQZC, Mips::BEQZC_MMR6 },
  { Mips::BGEC, Mips::BGEC, Mips::BGEC_MMR6 },
  { Mips::BGEUC, Mips::BGEUC, Mips::BGEUC_MMR6 },
  { Mips::BGEZALC, Mips::BGEZALC, Mips::BGEZALC_MMR6 },
  { Mips::BGEZC, Mips::BGEZC, Mips::BGEZC_MMR6 },
  { Mips::BGTZALC, Mips::BGTZALC, Mips::BGTZALC_MMR6 },
  { Mips::BGTZC, Mips::BGTZC, Mips::BGTZC_MMR6 },
  { Mips::BITSWAP, Mips::BITSWAP, Mips::BITSWAP_MMR6 },
  { Mips::BLEZALC, Mips::BLEZALC, Mips::BLEZALC_MMR6 },
  { Mips::BLEZC, Mips::BLEZC, Mips::BLEZC_MMR6 },
  { Mips::BLTC, Mips::BLTC, Mips::BLTC_MMR6 },
  { Mips::BLTUC, Mips::BLTUC, Mips::BLTUC_MMR6 },
  { Mips::BLTZALC, Mips::BLTZALC, Mips::BLTZALC_MMR6 },
  { Mips::BLTZC, Mips::BLTZC, Mips::BLTZC_MMR6 },
  { Mips::BNEC, Mips::BNEC, Mips::BNEC_MMR6 },
  { Mips::BNEZALC, Mips::BNEZALC, Mips::BNEZALC_MMR6 },
  { Mips::BNEZC, Mips::BNEZC, Mips::BNEZC_MMR6 },
  { Mips::BNVC, Mips::BNVC, Mips::BNVC_MMR6 },
  { Mips::BOVC, Mips::BOVC, Mips::BOVC_MMR6 },
  { Mips::CACHE_R6, Mips::CACHE_R6, Mips::CACHE_MMR6 },
  { Mips::CLO_R6, Mips::CLO_R6, Mips::CLO_MMR6 },
  { Mips::CLZ_R6, Mips::CLZ_R6, Mips::CLZ_MMR6 },
  { Mips::CMP_EQ_D, Mips::CMP_EQ_D, Mips::CMP_EQ_D_MMR6 },
  { Mips::CMP_EQ_S, Mips::CMP_EQ_S, Mips::CMP_EQ_S_MMR6 },
  { Mips::CMP_F_D, Mips::CMP_F_D, Mips::CMP_AF_D_MMR6 },
  { Mips::CMP_F_S, Mips::CMP_F_S, Mips::CMP_AF_S_MMR6 },
  { Mips::CMP_LE_D, Mips::CMP_LE_D, Mips::CMP_LE_D_MMR6 },
  { Mips::CMP_LE_S, Mips::CMP_LE_S, Mips::CMP_LE_S_MMR6 },
  { Mips::CMP_LT_D, Mips::CMP_LT_D, Mips::CMP_LT_D_MMR6 },
  { Mips::CMP_LT_S, Mips::CMP_LT_S, Mips::CMP_LT_S_MMR6 },
  { Mips::CMP_SAF_D, Mips::CMP_SAF_D, Mips::CMP_SAF_D_MMR6 },
  { Mips::CMP_SAF_S, Mips::CMP_SAF_S, Mips::CMP_SAF_S_MMR6 },
  { Mips::CMP_SEQ_D, Mips::CMP_SEQ_D, Mips::CMP_SEQ_D_MMR6 },
  { Mips::CMP_SEQ_S, Mips::CMP_SEQ_S, Mips::CMP_SEQ_S_MMR6 },
  { Mips::CMP_SLE_D, Mips::CMP_SLE_D, Mips::CMP_SLE_D_MMR6 },
  { Mips::CMP_SLE_S, Mips::CMP_SLE_S, Mips::CMP_SLE_S_MMR6 },
  { Mips::CMP_SLT_D, Mips::CMP_SLT_D, Mips::CMP_SLT_D_MMR6 },
  { Mips::CMP_SLT_S, Mips::CMP_SLT_S, Mips::CMP_SLT_S_MMR6 },
  { Mips::CMP_SUEQ_D, Mips::CMP_SUEQ_D, Mips::CMP_SUEQ_D_MMR6 },
  { Mips::CMP_SUEQ_S, Mips::CMP_SUEQ_S, Mips::CMP_SUEQ_S_MMR6 },
  { Mips::CMP_SULE_D, Mips::CMP_SULE_D, Mips::CMP_SULE_D_MMR6 },
  { Mips::CMP_SULE_S, Mips::CMP_SULE_S, Mips::CMP_SULE_S_MMR6 },
  { Mips::CMP_SULT_D, Mips::CMP_SULT_D, Mips::CMP_SULT_D_MMR6 },
  { Mips::CMP_SULT_S, Mips::CMP_SULT_S, Mips::CMP_SULT_S_MMR6 },
  { Mips::CMP_SUN_D, Mips::CMP_SUN_D, Mips::CMP_SUN_D_MMR6 },
  { Mips::CMP_SUN_S, Mips::CMP_SUN_S, Mips::CMP_SUN_S_MMR6 },
  { Mips::CMP_UEQ_D, Mips::CMP_UEQ_D, Mips::CMP_UEQ_D_MMR6 },
  { Mips::CMP_UEQ_S, Mips::CMP_UEQ_S, Mips::CMP_UEQ_S_MMR6 },
  { Mips::CMP_ULE_D, Mips::CMP_ULE_D, Mips::CMP_ULE_D_MMR6 },
  { Mips::CMP_ULE_S, Mips::CMP_ULE_S, Mips::CMP_ULE_S_MMR6 },
  { Mips::CMP_ULT_D, Mips::CMP_ULT_D, Mips::CMP_ULT_D_MMR6 },
  { Mips::CMP_ULT_S, Mips::CMP_ULT_S, Mips::CMP_ULT_S_MMR6 },
  { Mips::CMP_UN_D, Mips::CMP_UN_D, Mips::CMP_UN_D_MMR6 },
  { Mips::CMP_UN_S, Mips::CMP_UN_S, Mips::CMP_UN_S_MMR6 },
  { Mips::CRC32B, Mips::CRC32B, (uint16_t)-1U },
  { Mips::CRC32CB, Mips::CRC32CB, (uint16_t)-1U },
  { Mips::CRC32CD, Mips::CRC32CD, (uint16_t)-1U },
  { Mips::CRC32CH, Mips::CRC32CH, (uint16_t)-1U },
  { Mips::CRC32CW, Mips::CRC32CW, (uint16_t)-1U },
  { Mips::CRC32D, Mips::CRC32D, (uint16_t)-1U },
  { Mips::CRC32H, Mips::CRC32H, (uint16_t)-1U },
  { Mips::CRC32W, Mips::CRC32W, (uint16_t)-1U },
  { Mips::DIV, Mips::DIV, Mips::DIV_MMR6 },
  { Mips::DIVU, Mips::DIVU, Mips::DIVU_MMR6 },
  { Mips::DVP, Mips::DVP, Mips::DVP_MMR6 },
  { Mips::EVP, Mips::EVP, Mips::EVP_MMR6 },
  { Mips::GINVI, Mips::GINVI, Mips::GINVI_MMR6 },
  { Mips::GINVT, Mips::GINVT, Mips::GINVT_MMR6 },
  { Mips::JIALC, Mips::JIALC, Mips::JIALC_MMR6 },
  { Mips::JIC, Mips::JIC, Mips::JIC_MMR6 },
  { Mips::LSA_R6, Mips::LSA_R6, Mips::LSA_MMR6 },
  { Mips::LWPC, Mips::LWPC, Mips::LWPC_MMR6 },
  { Mips::LWUPC, Mips::LWUPC, (uint16_t)-1U },
  { Mips::MOD, Mips::MOD, Mips::MOD_MMR6 },
  { Mips::MODU, Mips::MODU, Mips::MODU_MMR6 },
  { Mips::MUH, Mips::MUH, Mips::MUH_MMR6 },
  { Mips::MUHU, Mips::MUHU, Mips::MUHU_MMR6 },
  { Mips::MULU, Mips::MULU, Mips::MULU_MMR6 },
  { Mips::MUL_R6, Mips::MUL_R6, Mips::MUL_MMR6 },
  { Mips::PREF_R6, Mips::PREF_R6, Mips::PREF_MMR6 },
  { Mips::SELEQZ, Mips::SELEQZ, Mips::SELEQZ_MMR6 },
  { Mips::SELEQZ_D, Mips::SELEQZ_D, Mips::SELEQZ_D_MMR6 },
  { Mips::SELEQZ_S, Mips::SELEQZ_S, Mips::SELEQZ_S_MMR6 },
  { Mips::SELNEZ, Mips::SELNEZ, Mips::SELNEZ_MMR6 },
  { Mips::SELNEZ_D, Mips::SELNEZ_D, Mips::SELNEZ_D_MMR6 },
  { Mips::SELNEZ_S, Mips::SELNEZ_S, Mips::SELNEZ_S_MMR6 },
  { Mips::SEL_D, Mips::SEL_D, Mips::SEL_D_MMR6 },
  { Mips::SEL_S, Mips::SEL_S, Mips::SEL_S_MMR6 },
}; // End of MipsR62MicroMipsR6Table

  unsigned mid;
  unsigned start = 0;
  unsigned end = 97;
  while (start < end) {
    mid = start + (end - start)/2;
    if (Opcode == MipsR62MicroMipsR6Table[mid][0]) {
      break;
    }
    if (Opcode < MipsR62MicroMipsR6Table[mid][0])
      end = mid;
    else
      start = mid + 1;
  }
  if (start == end)
    return -1; // Instruction doesn't exist in this table.

  if (inArch == Arch_mipsr6)
    return MipsR62MicroMipsR6Table[mid][1];
  if (inArch == Arch_micromipsr6)
    return MipsR62MicroMipsR6Table[mid][2];
  return -1;}

// Std2MicroMips
LLVM_READONLY
int Std2MicroMips(uint16_t Opcode, enum Arch inArch) {
static const uint16_t Std2MicroMipsTable[][3] = {
  { Mips::ADD, Mips::ADD, Mips::ADD_MM },
  { Mips::ADDi, Mips::ADDi, Mips::ADDi_MM },
  { Mips::ADDiu, Mips::ADDiu, Mips::ADDiu_MM },
  { Mips::ADDu, Mips::ADDu, Mips::ADDu_MM },
  { Mips::AND, Mips::AND, Mips::AND_MM },
  { Mips::ANDi, Mips::ANDi, Mips::ANDi_MM },
  { Mips::BC1F, Mips::BC1F, Mips::BC1F_MM },
  { Mips::BC1FL, Mips::BC1FL, (uint16_t)-1U },
  { Mips::BC1T, Mips::BC1T, Mips::BC1T_MM },
  { Mips::BC1TL, Mips::BC1TL, (uint16_t)-1U },
  { Mips::BEQ, Mips::BEQ, Mips::BEQ_MM },
  { Mips::BEQL, Mips::BEQL, (uint16_t)-1U },
  { Mips::BGEZ, Mips::BGEZ, Mips::BGEZ_MM },
  { Mips::BGEZAL, Mips::BGEZAL, Mips::BGEZAL_MM },
  { Mips::BGEZALL, Mips::BGEZALL, (uint16_t)-1U },
  { Mips::BGEZL, Mips::BGEZL, (uint16_t)-1U },
  { Mips::BGTZ, Mips::BGTZ, Mips::BGTZ_MM },
  { Mips::BGTZL, Mips::BGTZL, (uint16_t)-1U },
  { Mips::BLEZ, Mips::BLEZ, Mips::BLEZ_MM },
  { Mips::BLEZL, Mips::BLEZL, (uint16_t)-1U },
  { Mips::BLTZ, Mips::BLTZ, Mips::BLTZ_MM },
  { Mips::BLTZAL, Mips::BLTZAL, Mips::BLTZAL_MM },
  { Mips::BLTZALL, Mips::BLTZALL, (uint16_t)-1U },
  { Mips::BLTZL, Mips::BLTZL, (uint16_t)-1U },
  { Mips::BNE, Mips::BNE, Mips::BNE_MM },
  { Mips::BNEL, Mips::BNEL, (uint16_t)-1U },
  { Mips::BREAK, Mips::BREAK, Mips::BREAK_MM },
  { Mips::CACHE, Mips::CACHE, Mips::CACHE_MM },
  { Mips::CACHEE, Mips::CACHEE, Mips::CACHEE_MM },
  { Mips::CEIL_W_D32, Mips::CEIL_W_D32, Mips::CEIL_W_MM },
  { Mips::CEIL_W_S, Mips::CEIL_W_S, Mips::CEIL_W_S_MM },
  { Mips::CFC1, Mips::CFC1, Mips::CFC1_MM },
  { Mips::CLO, Mips::CLO, Mips::CLO_MM },
  { Mips::CLZ, Mips::CLZ, Mips::CLZ_MM },
  { Mips::CTC1, Mips::CTC1, Mips::CTC1_MM },
  { Mips::CVT_D32_S, Mips::CVT_D32_S, Mips::CVT_D32_S_MM },
  { Mips::CVT_D32_W, Mips::CVT_D32_W, Mips::CVT_D32_W_MM },
  { Mips::CVT_L_D64, Mips::CVT_L_D64, Mips::CVT_L_D64_MM },
  { Mips::CVT_L_S, Mips::CVT_L_S, Mips::CVT_L_S_MM },
  { Mips::CVT_S_D32, Mips::CVT_S_D32, Mips::CVT_S_D32_MM },
  { Mips::CVT_S_W, Mips::CVT_S_W, Mips::CVT_S_W_MM },
  { Mips::CVT_W_D32, Mips::CVT_W_D32, Mips::CVT_W_D32_MM },
  { Mips::CVT_W_S, Mips::CVT_W_S, Mips::CVT_W_S_MM },
  { Mips::C_EQ_D32, Mips::C_EQ_D32, Mips::C_EQ_D32_MM },
  { Mips::C_EQ_D64, Mips::C_EQ_D64, Mips::C_EQ_D64_MM },
  { Mips::C_EQ_S, Mips::C_EQ_S, Mips::C_EQ_S_MM },
  { Mips::C_F_D32, Mips::C_F_D32, Mips::C_F_D32_MM },
  { Mips::C_F_D64, Mips::C_F_D64, Mips::C_F_D64_MM },
  { Mips::C_F_S, Mips::C_F_S, Mips::C_F_S_MM },
  { Mips::C_LE_D32, Mips::C_LE_D32, Mips::C_LE_D32_MM },
  { Mips::C_LE_D64, Mips::C_LE_D64, Mips::C_LE_D64_MM },
  { Mips::C_LE_S, Mips::C_LE_S, Mips::C_LE_S_MM },
  { Mips::C_LT_D32, Mips::C_LT_D32, Mips::C_LT_D32_MM },
  { Mips::C_LT_D64, Mips::C_LT_D64, Mips::C_LT_D64_MM },
  { Mips::C_LT_S, Mips::C_LT_S, Mips::C_LT_S_MM },
  { Mips::C_NGE_D32, Mips::C_NGE_D32, Mips::C_NGE_D32_MM },
  { Mips::C_NGE_D64, Mips::C_NGE_D64, Mips::C_NGE_D64_MM },
  { Mips::C_NGE_S, Mips::C_NGE_S, Mips::C_NGE_S_MM },
  { Mips::C_NGLE_D32, Mips::C_NGLE_D32, Mips::C_NGLE_D32_MM },
  { Mips::C_NGLE_D64, Mips::C_NGLE_D64, Mips::C_NGLE_D64_MM },
  { Mips::C_NGLE_S, Mips::C_NGLE_S, Mips::C_NGLE_S_MM },
  { Mips::C_NGL_D32, Mips::C_NGL_D32, Mips::C_NGL_D32_MM },
  { Mips::C_NGL_D64, Mips::C_NGL_D64, Mips::C_NGL_D64_MM },
  { Mips::C_NGL_S, Mips::C_NGL_S, Mips::C_NGL_S_MM },
  { Mips::C_NGT_D32, Mips::C_NGT_D32, Mips::C_NGT_D32_MM },
  { Mips::C_NGT_D64, Mips::C_NGT_D64, Mips::C_NGT_D64_MM },
  { Mips::C_NGT_S, Mips::C_NGT_S, Mips::C_NGT_S_MM },
  { Mips::C_OLE_D32, Mips::C_OLE_D32, Mips::C_OLE_D32_MM },
  { Mips::C_OLE_D64, Mips::C_OLE_D64, Mips::C_OLE_D64_MM },
  { Mips::C_OLE_S, Mips::C_OLE_S, Mips::C_OLE_S_MM },
  { Mips::C_OLT_D32, Mips::C_OLT_D32, Mips::C_OLT_D32_MM },
  { Mips::C_OLT_D64, Mips::C_OLT_D64, Mips::C_OLT_D64_MM },
  { Mips::C_OLT_S, Mips::C_OLT_S, Mips::C_OLT_S_MM },
  { Mips::C_SEQ_D32, Mips::C_SEQ_D32, Mips::C_SEQ_D32_MM },
  { Mips::C_SEQ_D64, Mips::C_SEQ_D64, Mips::C_SEQ_D64_MM },
  { Mips::C_SEQ_S, Mips::C_SEQ_S, Mips::C_SEQ_S_MM },
  { Mips::C_SF_D32, Mips::C_SF_D32, Mips::C_SF_D32_MM },
  { Mips::C_SF_D64, Mips::C_SF_D64, Mips::C_SF_D64_MM },
  { Mips::C_SF_S, Mips::C_SF_S, Mips::C_SF_S_MM },
  { Mips::C_UEQ_D32, Mips::C_UEQ_D32, Mips::C_UEQ_D32_MM },
  { Mips::C_UEQ_D64, Mips::C_UEQ_D64, Mips::C_UEQ_D64_MM },
  { Mips::C_UEQ_S, Mips::C_UEQ_S, Mips::C_UEQ_S_MM },
  { Mips::C_ULE_D32, Mips::C_ULE_D32, Mips::C_ULE_D32_MM },
  { Mips::C_ULE_D64, Mips::C_ULE_D64, Mips::C_ULE_D64_MM },
  { Mips::C_ULE_S, Mips::C_ULE_S, Mips::C_ULE_S_MM },
  { Mips::C_ULT_D32, Mips::C_ULT_D32, Mips::C_ULT_D32_MM },
  { Mips::C_ULT_D64, Mips::C_ULT_D64, Mips::C_ULT_D64_MM },
  { Mips::C_ULT_S, Mips::C_ULT_S, Mips::C_ULT_S_MM },
  { Mips::C_UN_D32, Mips::C_UN_D32, Mips::C_UN_D32_MM },
  { Mips::C_UN_D64, Mips::C_UN_D64, Mips::C_UN_D64_MM },
  { Mips::C_UN_S, Mips::C_UN_S, Mips::C_UN_S_MM },
  { Mips::DERET, Mips::DERET, Mips::DERET_MM },
  { Mips::DI, Mips::DI, Mips::DI_MM },
  { Mips::EHB, Mips::EHB, Mips::EHB_MM },
  { Mips::EI, Mips::EI, Mips::EI_MM },
  { Mips::ERET, Mips::ERET, Mips::ERET_MM },
  { Mips::ERETNC, Mips::ERETNC, (uint16_t)-1U },
  { Mips::EXT, Mips::EXT, Mips::EXT_MM },
  { Mips::FABS_D32, Mips::FABS_D32, Mips::FABS_D32_MM },
  { Mips::FABS_S, Mips::FABS_S, Mips::FABS_S_MM },
  { Mips::FADD_D32, Mips::FADD_D32, Mips::FADD_D32_MM },
  { Mips::FADD_S, Mips::FADD_S, Mips::FADD_S_MM },
  { Mips::FCMP_D32, Mips::FCMP_D32, Mips::FCMP_D32_MM },
  { Mips::FCMP_S32, Mips::FCMP_S32, Mips::FCMP_S32_MM },
  { Mips::FDIV_D32, Mips::FDIV_D32, Mips::FDIV_D32_MM },
  { Mips::FDIV_S, Mips::FDIV_S, Mips::FDIV_S_MM },
  { Mips::FLOOR_W_D32, Mips::FLOOR_W_D32, Mips::FLOOR_W_MM },
  { Mips::FLOOR_W_S, Mips::FLOOR_W_S, Mips::FLOOR_W_S_MM },
  { Mips::FMOV_D32, Mips::FMOV_D32, Mips::FMOV_D32_MM },
  { Mips::FMOV_S, Mips::FMOV_S, Mips::FMOV_S_MM },
  { Mips::FMUL_D32, Mips::FMUL_D32, Mips::FMUL_D32_MM },
  { Mips::FMUL_S, Mips::FMUL_S, Mips::FMUL_S_MM },
  { Mips::FNEG_D32, Mips::FNEG_D32, Mips::FNEG_D32_MM },
  { Mips::FNEG_S, Mips::FNEG_S, Mips::FNEG_S_MM },
  { Mips::FSQRT_D32, Mips::FSQRT_D32, Mips::FSQRT_D32_MM },
  { Mips::FSQRT_S, Mips::FSQRT_S, Mips::FSQRT_S_MM },
  { Mips::FSUB_D32, Mips::FSUB_D32, Mips::FSUB_D32_MM },
  { Mips::FSUB_S, Mips::FSUB_S, Mips::FSUB_S_MM },
  { Mips::HYPCALL, Mips::HYPCALL, Mips::HYPCALL_MM },
  { Mips::INS, Mips::INS, Mips::INS_MM },
  { Mips::J, Mips::J, Mips::J_MM },
  { Mips::JAL, Mips::JAL, Mips::JAL_MM },
  { Mips::JALX, Mips::JALX, Mips::JALX_MM },
  { Mips::JR, Mips::JR, Mips::JR_MM },
  { Mips::LB, Mips::LB, Mips::LB_MM },
  { Mips::LBE, Mips::LBE, Mips::LBE_MM },
  { Mips::LBu, Mips::LBu, Mips::LBu_MM },
  { Mips::LBuE, Mips::LBuE, Mips::LBuE_MM },
  { Mips::LDC1, Mips::LDC1, Mips::LDC1_MM },
  { Mips::LEA_ADDiu, Mips::LEA_ADDiu, Mips::LEA_ADDiu_MM },
  { Mips::LH, Mips::LH, Mips::LH_MM },
  { Mips::LHE, Mips::LHE, Mips::LHE_MM },
  { Mips::LHu, Mips::LHu, Mips::LHu_MM },
  { Mips::LHuE, Mips::LHuE, Mips::LHuE_MM },
  { Mips::LLE, Mips::LLE, Mips::LLE_MM },
  { Mips::LUXC1, Mips::LUXC1, Mips::LUXC1_MM },
  { Mips::LUi, Mips::LUi, Mips::LUi_MM },
  { Mips::LW, Mips::LW, Mips::LW_MM },
  { Mips::LWC1, Mips::LWC1, Mips::LWC1_MM },
  { Mips::LWE, Mips::LWE, Mips::LWE_MM },
  { Mips::LWL, Mips::LWL, Mips::LWL_MM },
  { Mips::LWLE, Mips::LWLE, Mips::LWLE_MM },
  { Mips::LWR, Mips::LWR, Mips::LWR_MM },
  { Mips::LWRE, Mips::LWRE, Mips::LWRE_MM },
  { Mips::LWXC1, Mips::LWXC1, Mips::LWXC1_MM },
  { Mips::LWu, Mips::LWu, Mips::LWU_MM },
  { Mips::MADD, Mips::MADD, Mips::MADD_MM },
  { Mips::MADDU, Mips::MADDU, Mips::MADDU_MM },
  { Mips::MADD_D32, Mips::MADD_D32, Mips::MADD_D32_MM },
  { Mips::MADD_S, Mips::MADD_S, Mips::MADD_S_MM },
  { Mips::MFC1, Mips::MFC1, Mips::MFC1_MM },
  { Mips::MFGC0, Mips::MFGC0, Mips::MFGC0_MM },
  { Mips::MFHC1_D32, Mips::MFHC1_D32, Mips::MFHC1_D32_MM },
  { Mips::MFHGC0, Mips::MFHGC0, Mips::MFHGC0_MM },
  { Mips::MFHI, Mips::MFHI, Mips::MFHI_MM },
  { Mips::MFLO, Mips::MFLO, Mips::MFLO_MM },
  { Mips::MOVF_D32, Mips::MOVF_D32, Mips::MOVF_D32_MM },
  { Mips::MOVF_I, Mips::MOVF_I, Mips::MOVF_I_MM },
  { Mips::MOVF_S, Mips::MOVF_S, Mips::MOVF_S_MM },
  { Mips::MOVN_I_D32, Mips::MOVN_I_D32, Mips::MOVN_I_D32_MM },
  { Mips::MOVN_I_I, Mips::MOVN_I_I, Mips::MOVN_I_MM },
  { Mips::MOVN_I_S, Mips::MOVN_I_S, Mips::MOVN_I_S_MM },
  { Mips::MOVT_D32, Mips::MOVT_D32, Mips::MOVT_D32_MM },
  { Mips::MOVT_I, Mips::MOVT_I, Mips::MOVT_I_MM },
  { Mips::MOVT_S, Mips::MOVT_S, Mips::MOVT_S_MM },
  { Mips::MOVZ_I_D32, Mips::MOVZ_I_D32, Mips::MOVZ_I_D32_MM },
  { Mips::MOVZ_I_I, Mips::MOVZ_I_I, Mips::MOVZ_I_MM },
  { Mips::MOVZ_I_S, Mips::MOVZ_I_S, Mips::MOVZ_I_S_MM },
  { Mips::MSUB, Mips::MSUB, Mips::MSUB_MM },
  { Mips::MSUBU, Mips::MSUBU, Mips::MSUBU_MM },
  { Mips::MSUB_D32, Mips::MSUB_D32, Mips::MSUB_D32_MM },
  { Mips::MSUB_S, Mips::MSUB_S, Mips::MSUB_S_MM },
  { Mips::MTC1, Mips::MTC1, Mips::MTC1_MM },
  { Mips::MTGC0, Mips::MTGC0, Mips::MTGC0_MM },
  { Mips::MTHC1_D32, Mips::MTHC1_D32, Mips::MTHC1_D32_MM },
  { Mips::MTHGC0, Mips::MTHGC0, Mips::MTHGC0_MM },
  { Mips::MTHI, Mips::MTHI, Mips::MTHI_MM },
  { Mips::MTLO, Mips::MTLO, Mips::MTLO_MM },
  { Mips::MUL, Mips::MUL, Mips::MUL_MM },
  { Mips::MULT, Mips::MULT, Mips::MULT_MM },
  { Mips::MULTu, Mips::MULTu, Mips::MULTu_MM },
  { Mips::NMADD_D32, Mips::NMADD_D32, Mips::NMADD_D32_MM },
  { Mips::NMADD_S, Mips::NMADD_S, Mips::NMADD_S_MM },
  { Mips::NMSUB_D32, Mips::NMSUB_D32, Mips::NMSUB_D32_MM },
  { Mips::NMSUB_S, Mips::NMSUB_S, Mips::NMSUB_S_MM },
  { Mips::NOR, Mips::NOR, Mips::NOR_MM },
  { Mips::OR, Mips::OR, Mips::OR_MM },
  { Mips::ORi, Mips::ORi, Mips::ORi_MM },
  { Mips::PAUSE, Mips::PAUSE, Mips::PAUSE_MM },
  { Mips::PREF, Mips::PREF, Mips::PREF_MM },
  { Mips::PREFE, Mips::PREFE, Mips::PREFE_MM },
  { Mips::RDHWR, Mips::RDHWR, Mips::RDHWR_MM },
  { Mips::RECIP_D32, Mips::RECIP_D32, Mips::RECIP_D32_MM },
  { Mips::RECIP_D64, Mips::RECIP_D64, Mips::RECIP_D64_MM },
  { Mips::RECIP_S, Mips::RECIP_S, Mips::RECIP_S_MM },
  { Mips::ROTR, Mips::ROTR, Mips::ROTR_MM },
  { Mips::ROTRV, Mips::ROTRV, Mips::ROTRV_MM },
  { Mips::ROUND_W_D32, Mips::ROUND_W_D32, Mips::ROUND_W_MM },
  { Mips::ROUND_W_S, Mips::ROUND_W_S, Mips::ROUND_W_S_MM },
  { Mips::RSQRT_D32, Mips::RSQRT_D32, Mips::RSQRT_D32_MM },
  { Mips::RSQRT_D64, Mips::RSQRT_D64, Mips::RSQRT_D64_MM },
  { Mips::RSQRT_S, Mips::RSQRT_S, Mips::RSQRT_S_MM },
  { Mips::SB, Mips::SB, Mips::SB_MM },
  { Mips::SBE, Mips::SBE, Mips::SBE_MM },
  { Mips::SCE, Mips::SCE, Mips::SCE_MM },
  { Mips::SDBBP, Mips::SDBBP, Mips::SDBBP_MM },
  { Mips::SDC1, Mips::SDC1, Mips::SDC1_MM },
  { Mips::SDIV, Mips::SDIV, Mips::SDIV_MM },
  { Mips::SEB, Mips::SEB, Mips::SEB_MM },
  { Mips::SEH, Mips::SEH, Mips::SEH_MM },
  { Mips::SH, Mips::SH, Mips::SH_MM },
  { Mips::SHE, Mips::SHE, Mips::SHE_MM },
  { Mips::SLL, Mips::SLL, Mips::SLL_MM },
  { Mips::SLLV, Mips::SLLV, Mips::SLLV_MM },
  { Mips::SLT, Mips::SLT, Mips::SLT_MM },
  { Mips::SLTi, Mips::SLTi, Mips::SLTi_MM },
  { Mips::SLTiu, Mips::SLTiu, Mips::SLTiu_MM },
  { Mips::SLTu, Mips::SLTu, Mips::SLTu_MM },
  { Mips::SRA, Mips::SRA, Mips::SRA_MM },
  { Mips::SRAV, Mips::SRAV, Mips::SRAV_MM },
  { Mips::SRL, Mips::SRL, Mips::SRL_MM },
  { Mips::SRLV, Mips::SRLV, Mips::SRLV_MM },
  { Mips::SSNOP, Mips::SSNOP, Mips::SSNOP_MM },
  { Mips::SUB, Mips::SUB, Mips::SUB_MM },
  { Mips::SUBu, Mips::SUBu, Mips::SUBu_MM },
  { Mips::SUXC1, Mips::SUXC1, Mips::SUXC1_MM },
  { Mips::SW, Mips::SW, Mips::SW_MM },
  { Mips::SWC1, Mips::SWC1, Mips::SWC1_MM },
  { Mips::SWE, Mips::SWE, Mips::SWE_MM },
  { Mips::SWL, Mips::SWL, Mips::SWL_MM },
  { Mips::SWLE, Mips::SWLE, Mips::SWLE_MM },
  { Mips::SWR, Mips::SWR, Mips::SWR_MM },
  { Mips::SWRE, Mips::SWRE, Mips::SWRE_MM },
  { Mips::SWXC1, Mips::SWXC1, Mips::SWXC1_MM },
  { Mips::SYNC, Mips::SYNC, Mips::SYNC_MM },
  { Mips::SYNCI, Mips::SYNCI, Mips::SYNCI_MM },
  { Mips::SYSCALL, Mips::SYSCALL, Mips::SYSCALL_MM },
  { Mips::TEQ, Mips::TEQ, Mips::TEQ_MM },
  { Mips::TEQI, Mips::TEQI, Mips::TEQI_MM },
  { Mips::TGE, Mips::TGE, Mips::TGE_MM },
  { Mips::TGEI, Mips::TGEI, Mips::TGEI_MM },
  { Mips::TGEIU, Mips::TGEIU, Mips::TGEIU_MM },
  { Mips::TGEU, Mips::TGEU, Mips::TGEU_MM },
  { Mips::TLBGINV, Mips::TLBGINV, Mips::TLBGINV_MM },
  { Mips::TLBGINVF, Mips::TLBGINVF, Mips::TLBGINVF_MM },
  { Mips::TLBGP, Mips::TLBGP, Mips::TLBGP_MM },
  { Mips::TLBGR, Mips::TLBGR, Mips::TLBGR_MM },
  { Mips::TLBGWI, Mips::TLBGWI, Mips::TLBGWI_MM },
  { Mips::TLBGWR, Mips::TLBGWR, Mips::TLBGWR_MM },
  { Mips::TLBP, Mips::TLBP, Mips::TLBP_MM },
  { Mips::TLBR, Mips::TLBR, Mips::TLBR_MM },
  { Mips::TLBWI, Mips::TLBWI, Mips::TLBWI_MM },
  { Mips::TLBWR, Mips::TLBWR, Mips::TLBWR_MM },
  { Mips::TLT, Mips::TLT, Mips::TLT_MM },
  { Mips::TLTI, Mips::TLTI, Mips::TLTI_MM },
  { Mips::TLTU, Mips::TLTU, Mips::TLTU_MM },
  { Mips::TNE, Mips::TNE, Mips::TNE_MM },
  { Mips::TNEI, Mips::TNEI, Mips::TNEI_MM },
  { Mips::TRUNC_W_D32, Mips::TRUNC_W_D32, Mips::TRUNC_W_MM },
  { Mips::TRUNC_W_S, Mips::TRUNC_W_S, Mips::TRUNC_W_S_MM },
  { Mips::TTLTIU, Mips::TTLTIU, Mips::TLTIU_MM },
  { Mips::UDIV, Mips::UDIV, Mips::UDIV_MM },
  { Mips::WAIT, Mips::WAIT, Mips::WAIT_MM },
  { Mips::WSBH, Mips::WSBH, Mips::WSBH_MM },
  { Mips::XOR, Mips::XOR, Mips::XOR_MM },
  { Mips::XORi, Mips::XORi, Mips::XORi_MM },
}; // End of Std2MicroMipsTable

  unsigned mid;
  unsigned start = 0;
  unsigned end = 266;
  while (start < end) {
    mid = start + (end - start)/2;
    if (Opcode == Std2MicroMipsTable[mid][0]) {
      break;
    }
    if (Opcode < Std2MicroMipsTable[mid][0])
      end = mid;
    else
      start = mid + 1;
  }
  if (start == end)
    return -1; // Instruction doesn't exist in this table.

  if (inArch == Arch_se)
    return Std2MicroMipsTable[mid][1];
  if (inArch == Arch_micromips)
    return Std2MicroMipsTable[mid][2];
  return -1;}

// Std2MicroMipsR6
LLVM_READONLY
int Std2MicroMipsR6(uint16_t Opcode, enum Arch inArch) {
static const uint16_t Std2MicroMipsR6Table[][3] = {
  { Mips::ADD, Mips::ADD, Mips::ADD_MMR6 },
  { Mips::ADDiu, Mips::ADDiu, Mips::ADDIU_MMR6 },
  { Mips::ADDu, Mips::ADDu, Mips::ADDU_MMR6 },
  { Mips::AND, Mips::AND, Mips::AND_MMR6 },
  { Mips::ANDi, Mips::ANDi, Mips::ANDI_MMR6 },
  { Mips::BREAK, Mips::BREAK, Mips::BREAK_MMR6 },
  { Mips::CEIL_W_D64, Mips::CEIL_W_D64, Mips::CEIL_W_D_MMR6 },
  { Mips::CEIL_W_S, Mips::CEIL_W_S, Mips::CEIL_W_S_MMR6 },
  { Mips::CVT_W_D64, Mips::CVT_W_D64, (uint16_t)-1U },
  { Mips::DI, Mips::DI, Mips::DI_MMR6 },
  { Mips::EI, Mips::EI, Mips::EI_MMR6 },
  { Mips::EXT, Mips::EXT, Mips::EXT_MMR6 },
  { Mips::FLOOR_W_D64, Mips::FLOOR_W_D64, Mips::FLOOR_W_D_MMR6 },
  { Mips::FLOOR_W_S, Mips::FLOOR_W_S, Mips::FLOOR_W_S_MMR6 },
  { Mips::FSQRT_S, Mips::FSQRT_S, (uint16_t)-1U },
  { Mips::INS, Mips::INS, Mips::INS_MMR6 },
  { Mips::LDC1, Mips::LDC1, (uint16_t)-1U },
  { Mips::LDC164, Mips::LDC164, Mips::LDC1_D64_MMR6 },
  { Mips::LDC2, Mips::LDC2, Mips::LDC2_MMR6 },
  { Mips::LW, Mips::LW, Mips::LW_MMR6 },
  { Mips::LWC2, Mips::LWC2, Mips::LWC2_MMR6 },
  { Mips::MFC1, Mips::MFC1, Mips::MFC1_MMR6 },
  { Mips::MTC1, Mips::MTC1, Mips::MTC1_MMR6 },
  { Mips::MTHC1_D32, Mips::MTHC1_D32, (uint16_t)-1U },
  { Mips::NOR, Mips::NOR, Mips::NOR_MMR6 },
  { Mips::OR, Mips::OR, Mips::OR_MMR6 },
  { Mips::ORi, Mips::ORi, Mips::ORI_MMR6 },
  { Mips::PAUSE, Mips::PAUSE, Mips::PAUSE_MMR6 },
  { Mips::ROUND_W_D64, Mips::ROUND_W_D64, Mips::ROUND_W_D_MMR6 },
  { Mips::ROUND_W_S, Mips::ROUND_W_S, Mips::ROUND_W_S_MMR6 },
  { Mips::SB, Mips::SB, Mips::SB_MMR6 },
  { Mips::SDC164, Mips::SDC164, Mips::SDC1_D64_MMR6 },
  { Mips::SDC2, Mips::SDC2, Mips::SDC2_MMR6 },
  { Mips::SEB, Mips::SEB, (uint16_t)-1U },
  { Mips::SEH, Mips::SEH, (uint16_t)-1U },
  { Mips::SSNOP, Mips::SSNOP, Mips::SSNOP_MMR6 },
  { Mips::SUB, Mips::SUB, Mips::SUB_MMR6 },
  { Mips::SUBu, Mips::SUBu, Mips::SUBU_MMR6 },
  { Mips::SWC2, Mips::SWC2, Mips::SWC2_MMR6 },
  { Mips::SYNC, Mips::SYNC, Mips::SYNC_MMR6 },
  { Mips::SYNCI, Mips::SYNCI, Mips::SYNCI_MMR6 },
  { Mips::TRUNC_W_D64, Mips::TRUNC_W_D64, Mips::TRUNC_W_D_MMR6 },
  { Mips::TRUNC_W_S, Mips::TRUNC_W_S, Mips::TRUNC_W_S_MMR6 },
  { Mips::WAIT, Mips::WAIT, Mips::WAIT_MMR6 },
  { Mips::XOR, Mips::XOR, Mips::XOR_MMR6 },
  { Mips::XORi, Mips::XORi, Mips::XORI_MMR6 },
}; // End of Std2MicroMipsR6Table

  unsigned mid;
  unsigned start = 0;
  unsigned end = 46;
  while (start < end) {
    mid = start + (end - start)/2;
    if (Opcode == Std2MicroMipsR6Table[mid][0]) {
      break;
    }
    if (Opcode < Std2MicroMipsR6Table[mid][0])
      end = mid;
    else
      start = mid + 1;
  }
  if (start == end)
    return -1; // Instruction doesn't exist in this table.

  if (inArch == Arch_se)
    return Std2MicroMipsR6Table[mid][1];
  if (inArch == Arch_micromipsr6)
    return Std2MicroMipsR6Table[mid][2];
  return -1;}

} // End Mips namespace
} // End llvm namespace
#endif // GET_INSTRMAP_INFO

