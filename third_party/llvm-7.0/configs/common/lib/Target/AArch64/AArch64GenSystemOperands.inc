#ifdef GET_AT_DECL
enum ATValues {
  S1E1R = 960,
  S1E2R = 9152,
  S1E3R = 13248,
  S1E1W = 961,
  S1E2W = 9153,
  S1E3W = 13249,
  S1E0R = 962,
  S1E0W = 963,
  S12E1R = 9156,
  S12E1W = 9157,
  S12E0R = 9158,
  S12E0W = 9159,
  S1E1RP = 968,
  S1E1WP = 969,
};
#endif

#ifdef GET_DB_DECL
enum DBValues {
  oshld = 1,
  oshst = 2,
  osh = 3,
  nshld = 5,
  nshst = 6,
  nsh = 7,
  ishld = 9,
  ishst = 10,
  ish = 11,
  ld = 13,
  st = 14,
  sy = 15,
};
#endif

#ifdef GET_DC_DECL
enum DCValues {
  ZVA = 7073,
  IVAC = 945,
  ISW = 946,
  CVAC = 7121,
  CSW = 978,
  CVAU = 7129,
  CIVAC = 7153,
  CISW = 1010,
  CVAP = 7137,
};
#endif

#ifdef GET_EXACTFPIMM_DECL
enum ExactFPImmValues {
  zero = 0,
  half = 1,
  one = 2,
  two = 3,
};
#endif

#ifdef GET_IC_DECL
enum ICValues {
  IALLUIS = 904,
  IALLU = 936,
  IVAU = 7081,
};
#endif

#ifdef GET_ISB_DECL
enum ISBValues {
  sy = 15,
};
#endif

#ifdef GET_PRFM_DECL
enum PRFMValues {
  pldl1keep = 0,
  pldl1strm = 1,
  pldl2keep = 2,
  pldl2strm = 3,
  pldl3keep = 4,
  pldl3strm = 5,
  plil1keep = 8,
  plil1strm = 9,
  plil2keep = 10,
  plil2strm = 11,
  plil3keep = 12,
  plil3strm = 13,
  pstl1keep = 16,
  pstl1strm = 17,
  pstl2keep = 18,
  pstl2strm = 19,
  pstl3keep = 20,
  pstl3strm = 21,
};
#endif

#ifdef GET_PSB_DECL
enum PSBValues {
  csync = 17,
};
#endif

#ifdef GET_PSTATE_DECL
enum PStateValues {
  SPSel = 5,
  DAIFSet = 30,
  DAIFClr = 31,
  PAN = 4,
  UAO = 3,
  DIT = 26,
};
#endif

#ifdef GET_SVEPREDPAT_DECL
enum SVEPREDPATValues {
  pow2 = 0,
  vl1 = 1,
  vl2 = 2,
  vl3 = 3,
  vl4 = 4,
  vl5 = 5,
  vl6 = 6,
  vl7 = 7,
  vl8 = 8,
  vl16 = 9,
  vl32 = 10,
  vl64 = 11,
  vl128 = 12,
  vl256 = 13,
  mul4 = 29,
  mul3 = 30,
  all = 31,
};
#endif

#ifdef GET_SVEPRFM_DECL
enum SVEPRFMValues {
  pldl1keep = 0,
  pldl1strm = 1,
  pldl2keep = 2,
  pldl2strm = 3,
  pldl3keep = 4,
  pldl3strm = 5,
  pstl1keep = 8,
  pstl1strm = 9,
  pstl2keep = 10,
  pstl2strm = 11,
  pstl3keep = 12,
  pstl3strm = 13,
};
#endif

#ifdef GET_SYSREG_DECL
enum SysRegValues {
  MDCCSR_EL0 = 38920,
  DBGDTRRX_EL0 = 38952,
  MDRAR_EL1 = 32896,
  OSLSR_EL1 = 32908,
  DBGAUTHSTATUS_EL1 = 33782,
  PMCEID0_EL0 = 56550,
  PMCEID1_EL0 = 56551,
  MIDR_EL1 = 49152,
  CCSIDR_EL1 = 51200,
  CCSIDR2_EL1 = 51202,
  CLIDR_EL1 = 51201,
  CTR_EL0 = 55297,
  MPIDR_EL1 = 49157,
  REVIDR_EL1 = 49158,
  AIDR_EL1 = 51207,
  DCZID_EL0 = 55303,
  ID_PFR0_EL1 = 49160,
  ID_PFR1_EL1 = 49161,
  ID_DFR0_EL1 = 49162,
  ID_AFR0_EL1 = 49163,
  ID_MMFR0_EL1 = 49164,
  ID_MMFR1_EL1 = 49165,
  ID_MMFR2_EL1 = 49166,
  ID_MMFR3_EL1 = 49167,
  ID_ISAR0_EL1 = 49168,
  ID_ISAR1_EL1 = 49169,
  ID_ISAR2_EL1 = 49170,
  ID_ISAR3_EL1 = 49171,
  ID_ISAR4_EL1 = 49172,
  ID_ISAR5_EL1 = 49173,
  ID_ISAR6_EL1 = 49175,
  ID_AA64PFR0_EL1 = 49184,
  ID_AA64PFR1_EL1 = 49185,
  ID_AA64DFR0_EL1 = 49192,
  ID_AA64DFR1_EL1 = 49193,
  ID_AA64AFR0_EL1 = 49196,
  ID_AA64AFR1_EL1 = 49197,
  ID_AA64ISAR0_EL1 = 49200,
  ID_AA64ISAR1_EL1 = 49201,
  ID_AA64MMFR0_EL1 = 49208,
  ID_AA64MMFR1_EL1 = 49209,
  ID_AA64MMFR2_EL1 = 49210,
  MVFR0_EL1 = 49176,
  MVFR1_EL1 = 49177,
  MVFR2_EL1 = 49178,
  RVBAR_EL1 = 50689,
  RVBAR_EL2 = 58881,
  RVBAR_EL3 = 62977,
  ISR_EL1 = 50696,
  CNTPCT_EL0 = 57089,
  CNTVCT_EL0 = 57090,
  ID_MMFR4_EL1 = 49174,
  TRCSTATR = 34840,
  TRCIDR8 = 34822,
  TRCIDR9 = 34830,
  TRCIDR10 = 34838,
  TRCIDR11 = 34846,
  TRCIDR12 = 34854,
  TRCIDR13 = 34862,
  TRCIDR0 = 34887,
  TRCIDR1 = 34895,
  TRCIDR2 = 34903,
  TRCIDR3 = 34911,
  TRCIDR4 = 34919,
  TRCIDR5 = 34927,
  TRCIDR6 = 34935,
  TRCIDR7 = 34943,
  TRCOSLSR = 34956,
  TRCPDSR = 34988,
  TRCDEVAFF0 = 35798,
  TRCDEVAFF1 = 35806,
  TRCLSR = 35822,
  TRCAUTHSTATUS = 35830,
  TRCDEVARCH = 35838,
  TRCDEVID = 35735,
  TRCDEVTYPE = 35743,
  TRCPIDR4 = 35751,
  TRCPIDR5 = 35759,
  TRCPIDR6 = 35767,
  TRCPIDR7 = 35775,
  TRCPIDR0 = 35783,
  TRCPIDR1 = 35791,
  TRCPIDR2 = 35799,
  TRCPIDR3 = 35807,
  TRCCIDR0 = 35815,
  TRCCIDR1 = 35823,
  TRCCIDR2 = 35831,
  TRCCIDR3 = 35839,
  ICC_IAR1_EL1 = 50784,
  ICC_IAR0_EL1 = 50752,
  ICC_HPPIR1_EL1 = 50786,
  ICC_HPPIR0_EL1 = 50754,
  ICC_RPR_EL1 = 50779,
  ICH_VTR_EL2 = 58969,
  ICH_EISR_EL2 = 58971,
  ICH_ELRSR_EL2 = 58973,
  ID_AA64ZFR0_EL1 = 49188,
  LORID_EL1 = 50471,
  ERRIDR_EL1 = 49816,
  ERXFR_EL1 = 49824,
  DBGDTRTX_EL0 = 38952,
  OSLAR_EL1 = 32900,
  PMSWINC_EL0 = 56548,
  TRCOSLAR = 34948,
  TRCLAR = 35814,
  ICC_EOIR1_EL1 = 50785,
  ICC_EOIR0_EL1 = 50753,
  ICC_DIR_EL1 = 50777,
  ICC_SGI1R_EL1 = 50781,
  ICC_ASGI1R_EL1 = 50782,
  ICC_SGI0R_EL1 = 50783,
  OSDTRRX_EL1 = 32770,
  OSDTRTX_EL1 = 32794,
  TEECR32_EL1 = 36864,
  MDCCINT_EL1 = 32784,
  MDSCR_EL1 = 32786,
  DBGDTR_EL0 = 38944,
  OSECCR_EL1 = 32818,
  DBGVCR32_EL2 = 41016,
  DBGBVR0_EL1 = 32772,
  DBGBVR1_EL1 = 32780,
  DBGBVR2_EL1 = 32788,
  DBGBVR3_EL1 = 32796,
  DBGBVR4_EL1 = 32804,
  DBGBVR5_EL1 = 32812,
  DBGBVR6_EL1 = 32820,
  DBGBVR7_EL1 = 32828,
  DBGBVR8_EL1 = 32836,
  DBGBVR9_EL1 = 32844,
  DBGBVR10_EL1 = 32852,
  DBGBVR11_EL1 = 32860,
  DBGBVR12_EL1 = 32868,
  DBGBVR13_EL1 = 32876,
  DBGBVR14_EL1 = 32884,
  DBGBVR15_EL1 = 32892,
  DBGBCR0_EL1 = 32773,
  DBGBCR1_EL1 = 32781,
  DBGBCR2_EL1 = 32789,
  DBGBCR3_EL1 = 32797,
  DBGBCR4_EL1 = 32805,
  DBGBCR5_EL1 = 32813,
  DBGBCR6_EL1 = 32821,
  DBGBCR7_EL1 = 32829,
  DBGBCR8_EL1 = 32837,
  DBGBCR9_EL1 = 32845,
  DBGBCR10_EL1 = 32853,
  DBGBCR11_EL1 = 32861,
  DBGBCR12_EL1 = 32869,
  DBGBCR13_EL1 = 32877,
  DBGBCR14_EL1 = 32885,
  DBGBCR15_EL1 = 32893,
  DBGWVR0_EL1 = 32774,
  DBGWVR1_EL1 = 32782,
  DBGWVR2_EL1 = 32790,
  DBGWVR3_EL1 = 32798,
  DBGWVR4_EL1 = 32806,
  DBGWVR5_EL1 = 32814,
  DBGWVR6_EL1 = 32822,
  DBGWVR7_EL1 = 32830,
  DBGWVR8_EL1 = 32838,
  DBGWVR9_EL1 = 32846,
  DBGWVR10_EL1 = 32854,
  DBGWVR11_EL1 = 32862,
  DBGWVR12_EL1 = 32870,
  DBGWVR13_EL1 = 32878,
  DBGWVR14_EL1 = 32886,
  DBGWVR15_EL1 = 32894,
  DBGWCR0_EL1 = 32775,
  DBGWCR1_EL1 = 32783,
  DBGWCR2_EL1 = 32791,
  DBGWCR3_EL1 = 32799,
  DBGWCR4_EL1 = 32807,
  DBGWCR5_EL1 = 32815,
  DBGWCR6_EL1 = 32823,
  DBGWCR7_EL1 = 32831,
  DBGWCR8_EL1 = 32839,
  DBGWCR9_EL1 = 32847,
  DBGWCR10_EL1 = 32855,
  DBGWCR11_EL1 = 32863,
  DBGWCR12_EL1 = 32871,
  DBGWCR13_EL1 = 32879,
  DBGWCR14_EL1 = 32887,
  DBGWCR15_EL1 = 32895,
  TEEHBR32_EL1 = 36992,
  OSDLR_EL1 = 32924,
  DBGPRCR_EL1 = 32932,
  DBGCLAIMSET_EL1 = 33734,
  DBGCLAIMCLR_EL1 = 33742,
  CSSELR_EL1 = 53248,
  VPIDR_EL2 = 57344,
  VMPIDR_EL2 = 57349,
  CPACR_EL1 = 49282,
  SCTLR_EL1 = 49280,
  SCTLR_EL2 = 57472,
  SCTLR_EL3 = 61568,
  ACTLR_EL1 = 49281,
  ACTLR_EL2 = 57473,
  ACTLR_EL3 = 61569,
  HCR_EL2 = 57480,
  SCR_EL3 = 61576,
  MDCR_EL2 = 57481,
  SDER32_EL3 = 61577,
  CPTR_EL2 = 57482,
  CPTR_EL3 = 61578,
  HSTR_EL2 = 57483,
  HACR_EL2 = 57487,
  MDCR_EL3 = 61593,
  TTBR0_EL1 = 49408,
  TTBR0_EL2 = 57600,
  TTBR0_EL3 = 61696,
  TTBR1_EL1 = 49409,
  TCR_EL1 = 49410,
  TCR_EL2 = 57602,
  TCR_EL3 = 61698,
  VTTBR_EL2 = 57608,
  VTCR_EL2 = 57610,
  DACR32_EL2 = 57728,
  SPSR_EL1 = 49664,
  SPSR_EL2 = 57856,
  SPSR_EL3 = 61952,
  ELR_EL1 = 49665,
  ELR_EL2 = 57857,
  ELR_EL3 = 61953,
  SP_EL0 = 49672,
  SP_EL1 = 57864,
  SP_EL2 = 61960,
  SPSel = 49680,
  NZCV = 55824,
  DAIF = 55825,
  CurrentEL = 49682,
  SPSR_irq = 57880,
  SPSR_abt = 57881,
  SPSR_und = 57882,
  SPSR_fiq = 57883,
  FPCR = 55840,
  FPSR = 55841,
  DSPSR_EL0 = 55848,
  DLR_EL0 = 55849,
  IFSR32_EL2 = 57985,
  AFSR0_EL1 = 49800,
  AFSR0_EL2 = 57992,
  AFSR0_EL3 = 62088,
  AFSR1_EL1 = 49801,
  AFSR1_EL2 = 57993,
  AFSR1_EL3 = 62089,
  ESR_EL1 = 49808,
  ESR_EL2 = 58000,
  ESR_EL3 = 62096,
  FPEXC32_EL2 = 58008,
  FAR_EL1 = 49920,
  FAR_EL2 = 58112,
  FAR_EL3 = 62208,
  HPFAR_EL2 = 58116,
  PAR_EL1 = 50080,
  PMCR_EL0 = 56544,
  PMCNTENSET_EL0 = 56545,
  PMCNTENCLR_EL0 = 56546,
  PMOVSCLR_EL0 = 56547,
  PMSELR_EL0 = 56549,
  PMCCNTR_EL0 = 56552,
  PMXEVTYPER_EL0 = 56553,
  PMXEVCNTR_EL0 = 56554,
  PMUSERENR_EL0 = 56560,
  PMINTENSET_EL1 = 50417,
  PMINTENCLR_EL1 = 50418,
  PMOVSSET_EL0 = 56563,
  MAIR_EL1 = 50448,
  MAIR_EL2 = 58640,
  MAIR_EL3 = 62736,
  AMAIR_EL1 = 50456,
  AMAIR_EL2 = 58648,
  AMAIR_EL3 = 62744,
  VBAR_EL1 = 50688,
  VBAR_EL2 = 58880,
  VBAR_EL3 = 62976,
  RMR_EL1 = 50690,
  RMR_EL2 = 58882,
  RMR_EL3 = 62978,
  CONTEXTIDR_EL1 = 50817,
  TPIDR_EL0 = 56962,
  TPIDR_EL2 = 59010,
  TPIDR_EL3 = 63106,
  TPIDRRO_EL0 = 56963,
  TPIDR_EL1 = 50820,
  CNTFRQ_EL0 = 57088,
  CNTVOFF_EL2 = 59139,
  CNTKCTL_EL1 = 50952,
  CNTHCTL_EL2 = 59144,
  CNTP_TVAL_EL0 = 57104,
  CNTHP_TVAL_EL2 = 59152,
  CNTPS_TVAL_EL1 = 65296,
  CNTP_CTL_EL0 = 57105,
  CNTHP_CTL_EL2 = 59153,
  CNTPS_CTL_EL1 = 65297,
  CNTP_CVAL_EL0 = 57106,
  CNTHP_CVAL_EL2 = 59154,
  CNTPS_CVAL_EL1 = 65298,
  CNTV_TVAL_EL0 = 57112,
  CNTV_CTL_EL0 = 57113,
  CNTV_CVAL_EL0 = 57114,
  PMEVCNTR0_EL0 = 57152,
  PMEVCNTR1_EL0 = 57153,
  PMEVCNTR2_EL0 = 57154,
  PMEVCNTR3_EL0 = 57155,
  PMEVCNTR4_EL0 = 57156,
  PMEVCNTR5_EL0 = 57157,
  PMEVCNTR6_EL0 = 57158,
  PMEVCNTR7_EL0 = 57159,
  PMEVCNTR8_EL0 = 57160,
  PMEVCNTR9_EL0 = 57161,
  PMEVCNTR10_EL0 = 57162,
  PMEVCNTR11_EL0 = 57163,
  PMEVCNTR12_EL0 = 57164,
  PMEVCNTR13_EL0 = 57165,
  PMEVCNTR14_EL0 = 57166,
  PMEVCNTR15_EL0 = 57167,
  PMEVCNTR16_EL0 = 57168,
  PMEVCNTR17_EL0 = 57169,
  PMEVCNTR18_EL0 = 57170,
  PMEVCNTR19_EL0 = 57171,
  PMEVCNTR20_EL0 = 57172,
  PMEVCNTR21_EL0 = 57173,
  PMEVCNTR22_EL0 = 57174,
  PMEVCNTR23_EL0 = 57175,
  PMEVCNTR24_EL0 = 57176,
  PMEVCNTR25_EL0 = 57177,
  PMEVCNTR26_EL0 = 57178,
  PMEVCNTR27_EL0 = 57179,
  PMEVCNTR28_EL0 = 57180,
  PMEVCNTR29_EL0 = 57181,
  PMEVCNTR30_EL0 = 57182,
  PMCCFILTR_EL0 = 57215,
  PMEVTYPER0_EL0 = 57184,
  PMEVTYPER1_EL0 = 57185,
  PMEVTYPER2_EL0 = 57186,
  PMEVTYPER3_EL0 = 57187,
  PMEVTYPER4_EL0 = 57188,
  PMEVTYPER5_EL0 = 57189,
  PMEVTYPER6_EL0 = 57190,
  PMEVTYPER7_EL0 = 57191,
  PMEVTYPER8_EL0 = 57192,
  PMEVTYPER9_EL0 = 57193,
  PMEVTYPER10_EL0 = 57194,
  PMEVTYPER11_EL0 = 57195,
  PMEVTYPER12_EL0 = 57196,
  PMEVTYPER13_EL0 = 57197,
  PMEVTYPER14_EL0 = 57198,
  PMEVTYPER15_EL0 = 57199,
  PMEVTYPER16_EL0 = 57200,
  PMEVTYPER17_EL0 = 57201,
  PMEVTYPER18_EL0 = 57202,
  PMEVTYPER19_EL0 = 57203,
  PMEVTYPER20_EL0 = 57204,
  PMEVTYPER21_EL0 = 57205,
  PMEVTYPER22_EL0 = 57206,
  PMEVTYPER23_EL0 = 57207,
  PMEVTYPER24_EL0 = 57208,
  PMEVTYPER25_EL0 = 57209,
  PMEVTYPER26_EL0 = 57210,
  PMEVTYPER27_EL0 = 57211,
  PMEVTYPER28_EL0 = 57212,
  PMEVTYPER29_EL0 = 57213,
  PMEVTYPER30_EL0 = 57214,
  TRCPRGCTLR = 34824,
  TRCPROCSELR = 34832,
  TRCCONFIGR = 34848,
  TRCAUXCTLR = 34864,
  TRCEVENTCTL0R = 34880,
  TRCEVENTCTL1R = 34888,
  TRCSTALLCTLR = 34904,
  TRCTSCTLR = 34912,
  TRCSYNCPR = 34920,
  TRCCCCTLR = 34928,
  TRCBBCTLR = 34936,
  TRCTRACEIDR = 34817,
  TRCQCTLR = 34825,
  TRCVICTLR = 34818,
  TRCVIIECTLR = 34826,
  TRCVISSCTLR = 34834,
  TRCVIPCSSCTLR = 34842,
  TRCVDCTLR = 34882,
  TRCVDSACCTLR = 34890,
  TRCVDARCCTLR = 34898,
  TRCSEQEVR0 = 34820,
  TRCSEQEVR1 = 34828,
  TRCSEQEVR2 = 34836,
  TRCSEQRSTEVR = 34868,
  TRCSEQSTR = 34876,
  TRCEXTINSELR = 34884,
  TRCCNTRLDVR0 = 34821,
  TRCCNTRLDVR1 = 34829,
  TRCCNTRLDVR2 = 34837,
  TRCCNTRLDVR3 = 34845,
  TRCCNTCTLR0 = 34853,
  TRCCNTCTLR1 = 34861,
  TRCCNTCTLR2 = 34869,
  TRCCNTCTLR3 = 34877,
  TRCCNTVR0 = 34885,
  TRCCNTVR1 = 34893,
  TRCCNTVR2 = 34901,
  TRCCNTVR3 = 34909,
  TRCIMSPEC0 = 34823,
  TRCIMSPEC1 = 34831,
  TRCIMSPEC2 = 34839,
  TRCIMSPEC3 = 34847,
  TRCIMSPEC4 = 34855,
  TRCIMSPEC5 = 34863,
  TRCIMSPEC6 = 34871,
  TRCIMSPEC7 = 34879,
  TRCRSCTLR2 = 34960,
  TRCRSCTLR3 = 34968,
  TRCRSCTLR4 = 34976,
  TRCRSCTLR5 = 34984,
  TRCRSCTLR6 = 34992,
  TRCRSCTLR7 = 35000,
  TRCRSCTLR8 = 35008,
  TRCRSCTLR9 = 35016,
  TRCRSCTLR10 = 35024,
  TRCRSCTLR11 = 35032,
  TRCRSCTLR12 = 35040,
  TRCRSCTLR13 = 35048,
  TRCRSCTLR14 = 35056,
  TRCRSCTLR15 = 35064,
  TRCRSCTLR16 = 34945,
  TRCRSCTLR17 = 34953,
  TRCRSCTLR18 = 34961,
  TRCRSCTLR19 = 34969,
  TRCRSCTLR20 = 34977,
  TRCRSCTLR21 = 34985,
  TRCRSCTLR22 = 34993,
  TRCRSCTLR23 = 35001,
  TRCRSCTLR24 = 35009,
  TRCRSCTLR25 = 35017,
  TRCRSCTLR26 = 35025,
  TRCRSCTLR27 = 35033,
  TRCRSCTLR28 = 35041,
  TRCRSCTLR29 = 35049,
  TRCRSCTLR30 = 35057,
  TRCRSCTLR31 = 35065,
  TRCSSCCR0 = 34946,
  TRCSSCCR1 = 34954,
  TRCSSCCR2 = 34962,
  TRCSSCCR3 = 34970,
  TRCSSCCR4 = 34978,
  TRCSSCCR5 = 34986,
  TRCSSCCR6 = 34994,
  TRCSSCCR7 = 35002,
  TRCSSCSR0 = 35010,
  TRCSSCSR1 = 35018,
  TRCSSCSR2 = 35026,
  TRCSSCSR3 = 35034,
  TRCSSCSR4 = 35042,
  TRCSSCSR5 = 35050,
  TRCSSCSR6 = 35058,
  TRCSSCSR7 = 35066,
  TRCSSPCICR0 = 34947,
  TRCSSPCICR1 = 34955,
  TRCSSPCICR2 = 34963,
  TRCSSPCICR3 = 34971,
  TRCSSPCICR4 = 34979,
  TRCSSPCICR5 = 34987,
  TRCSSPCICR6 = 34995,
  TRCSSPCICR7 = 35003,
  TRCPDCR = 34980,
  TRCACVR0 = 35072,
  TRCACVR1 = 35088,
  TRCACVR2 = 35104,
  TRCACVR3 = 35120,
  TRCACVR4 = 35136,
  TRCACVR5 = 35152,
  TRCACVR6 = 35168,
  TRCACVR7 = 35184,
  TRCACVR8 = 35073,
  TRCACVR9 = 35089,
  TRCACVR10 = 35105,
  TRCACVR11 = 35121,
  TRCACVR12 = 35137,
  TRCACVR13 = 35153,
  TRCACVR14 = 35169,
  TRCACVR15 = 35185,
  TRCACATR0 = 35074,
  TRCACATR1 = 35090,
  TRCACATR2 = 35106,
  TRCACATR3 = 35122,
  TRCACATR4 = 35138,
  TRCACATR5 = 35154,
  TRCACATR6 = 35170,
  TRCACATR7 = 35186,
  TRCACATR8 = 35075,
  TRCACATR9 = 35091,
  TRCACATR10 = 35107,
  TRCACATR11 = 35123,
  TRCACATR12 = 35139,
  TRCACATR13 = 35155,
  TRCACATR14 = 35171,
  TRCACATR15 = 35187,
  TRCDVCVR0 = 35076,
  TRCDVCVR1 = 35108,
  TRCDVCVR2 = 35140,
  TRCDVCVR3 = 35172,
  TRCDVCVR4 = 35077,
  TRCDVCVR5 = 35109,
  TRCDVCVR6 = 35141,
  TRCDVCVR7 = 35173,
  TRCDVCMR0 = 35078,
  TRCDVCMR1 = 35110,
  TRCDVCMR2 = 35142,
  TRCDVCMR3 = 35174,
  TRCDVCMR4 = 35079,
  TRCDVCMR5 = 35111,
  TRCDVCMR6 = 35143,
  TRCDVCMR7 = 35175,
  TRCCIDCVR0 = 35200,
  TRCCIDCVR1 = 35216,
  TRCCIDCVR2 = 35232,
  TRCCIDCVR3 = 35248,
  TRCCIDCVR4 = 35264,
  TRCCIDCVR5 = 35280,
  TRCCIDCVR6 = 35296,
  TRCCIDCVR7 = 35312,
  TRCVMIDCVR0 = 35201,
  TRCVMIDCVR1 = 35217,
  TRCVMIDCVR2 = 35233,
  TRCVMIDCVR3 = 35249,
  TRCVMIDCVR4 = 35265,
  TRCVMIDCVR5 = 35281,
  TRCVMIDCVR6 = 35297,
  TRCVMIDCVR7 = 35313,
  TRCCIDCCTLR0 = 35202,
  TRCCIDCCTLR1 = 35210,
  TRCVMIDCCTLR0 = 35218,
  TRCVMIDCCTLR1 = 35226,
  TRCITCTRL = 35716,
  TRCCLAIMSET = 35782,
  TRCCLAIMCLR = 35790,
  ICC_BPR1_EL1 = 50787,
  ICC_BPR0_EL1 = 50755,
  ICC_PMR_EL1 = 49712,
  ICC_CTLR_EL1 = 50788,
  ICC_CTLR_EL3 = 63076,
  ICC_SRE_EL1 = 50789,
  ICC_SRE_EL2 = 58957,
  ICC_SRE_EL3 = 63077,
  ICC_IGRPEN0_EL1 = 50790,
  ICC_IGRPEN1_EL1 = 50791,
  ICC_IGRPEN1_EL3 = 63079,
  ICC_SEIEN_EL1 = 50792,
  ICC_AP0R0_EL1 = 50756,
  ICC_AP0R1_EL1 = 50757,
  ICC_AP0R2_EL1 = 50758,
  ICC_AP0R3_EL1 = 50759,
  ICC_AP1R0_EL1 = 50760,
  ICC_AP1R1_EL1 = 50761,
  ICC_AP1R2_EL1 = 50762,
  ICC_AP1R3_EL1 = 50763,
  ICH_AP0R0_EL2 = 58944,
  ICH_AP0R1_EL2 = 58945,
  ICH_AP0R2_EL2 = 58946,
  ICH_AP0R3_EL2 = 58947,
  ICH_AP1R0_EL2 = 58952,
  ICH_AP1R1_EL2 = 58953,
  ICH_AP1R2_EL2 = 58954,
  ICH_AP1R3_EL2 = 58955,
  ICH_HCR_EL2 = 58968,
  ICH_MISR_EL2 = 58970,
  ICH_VMCR_EL2 = 58975,
  ICH_VSEIR_EL2 = 58956,
  ICH_LR0_EL2 = 58976,
  ICH_LR1_EL2 = 58977,
  ICH_LR2_EL2 = 58978,
  ICH_LR3_EL2 = 58979,
  ICH_LR4_EL2 = 58980,
  ICH_LR5_EL2 = 58981,
  ICH_LR6_EL2 = 58982,
  ICH_LR7_EL2 = 58983,
  ICH_LR8_EL2 = 58984,
  ICH_LR9_EL2 = 58985,
  ICH_LR10_EL2 = 58986,
  ICH_LR11_EL2 = 58987,
  ICH_LR12_EL2 = 58988,
  ICH_LR13_EL2 = 58989,
  ICH_LR14_EL2 = 58990,
  ICH_LR15_EL2 = 58991,
  PAN = 49683,
  LORSA_EL1 = 50464,
  LOREA_EL1 = 50465,
  LORN_EL1 = 50466,
  LORC_EL1 = 50467,
  TTBR1_EL2 = 57601,
  CONTEXTIDR_EL2 = 59009,
  CNTHV_TVAL_EL2 = 59160,
  CNTHV_CVAL_EL2 = 59162,
  CNTHV_CTL_EL2 = 59161,
  SCTLR_EL12 = 59520,
  CPACR_EL12 = 59522,
  TTBR0_EL12 = 59648,
  TTBR1_EL12 = 59649,
  TCR_EL12 = 59650,
  AFSR0_EL12 = 60040,
  AFSR1_EL12 = 60041,
  ESR_EL12 = 60048,
  FAR_EL12 = 60160,
  MAIR_EL12 = 60688,
  AMAIR_EL12 = 60696,
  VBAR_EL12 = 60928,
  CONTEXTIDR_EL12 = 61057,
  CNTKCTL_EL12 = 61192,
  CNTP_TVAL_EL02 = 61200,
  CNTP_CTL_EL02 = 61201,
  CNTP_CVAL_EL02 = 61202,
  CNTV_TVAL_EL02 = 61208,
  CNTV_CTL_EL02 = 61209,
  CNTV_CVAL_EL02 = 61210,
  SPSR_EL12 = 59904,
  ELR_EL12 = 59905,
  UAO = 49684,
  PMBLIMITR_EL1 = 50384,
  PMBPTR_EL1 = 50385,
  PMBSR_EL1 = 50387,
  PMBIDR_EL1 = 50391,
  PMSCR_EL2 = 58568,
  PMSCR_EL12 = 60616,
  PMSCR_EL1 = 50376,
  PMSICR_EL1 = 50378,
  PMSIRR_EL1 = 50379,
  PMSFCR_EL1 = 50380,
  PMSEVFR_EL1 = 50381,
  PMSLATFR_EL1 = 50382,
  PMSIDR_EL1 = 50383,
  ERRSELR_EL1 = 49817,
  ERXCTLR_EL1 = 49825,
  ERXSTATUS_EL1 = 49826,
  ERXADDR_EL1 = 49827,
  ERXMISC0_EL1 = 49832,
  ERXMISC1_EL1 = 49833,
  DISR_EL1 = 50697,
  VDISR_EL2 = 58889,
  VSESR_EL2 = 58003,
  APIAKeyLo_EL1 = 49416,
  APIAKeyHi_EL1 = 49417,
  APIBKeyLo_EL1 = 49418,
  APIBKeyHi_EL1 = 49419,
  APDAKeyLo_EL1 = 49424,
  APDAKeyHi_EL1 = 49425,
  APDBKeyLo_EL1 = 49426,
  APDBKeyHi_EL1 = 49427,
  APGAKeyLo_EL1 = 49432,
  APGAKeyHi_EL1 = 49433,
  VSTCR_EL2 = 57650,
  VSTTBR_EL2 = 57648,
  CNTHVS_TVAL_EL2 = 59168,
  CNTHVS_CVAL_EL2 = 59170,
  CNTHVS_CTL_EL2 = 59169,
  CNTHPS_TVAL_EL2 = 59176,
  CNTHPS_CVAL_EL2 = 59178,
  CNTHPS_CTL_EL2 = 59177,
  SDER32_EL2 = 57497,
  ERXPFGCTL_EL1 = 49829,
  ERXPFGCDN_EL1 = 49830,
  ERXTS_EL1 = 49839,
  ERXMISC2_EL1 = 49834,
  ERXMISC3_EL1 = 49835,
  ERXPFGF_EL1 = 49828,
  MPAM0_EL1 = 50473,
  MPAM1_EL1 = 50472,
  MPAM2_EL2 = 58664,
  MPAM3_EL3 = 62760,
  MPAM1_EL12 = 60712,
  MPAMHCR_EL2 = 58656,
  MPAMVPMV_EL2 = 58657,
  MPAMVPM0_EL2 = 58672,
  MPAMVPM1_EL2 = 58673,
  MPAMVPM2_EL2 = 58674,
  MPAMVPM3_EL2 = 58675,
  MPAMVPM4_EL2 = 58676,
  MPAMVPM5_EL2 = 58677,
  MPAMVPM6_EL2 = 58678,
  MPAMVPM7_EL2 = 58679,
  MPAMIDR_EL1 = 50468,
  AMCR_EL0 = 56976,
  AMCFGR_EL0 = 56977,
  AMCGCR_EL0 = 56978,
  AMUSERENR_EL0 = 56979,
  AMCNTENCLR0_EL0 = 56980,
  AMCNTENSET0_EL0 = 56981,
  AMEVCNTR00_EL0 = 56992,
  AMEVCNTR01_EL0 = 56993,
  AMEVCNTR02_EL0 = 56994,
  AMEVCNTR03_EL0 = 56995,
  AMEVTYPER00_EL0 = 57008,
  AMEVTYPER01_EL0 = 57009,
  AMEVTYPER02_EL0 = 57010,
  AMEVTYPER03_EL0 = 57011,
  AMCNTENCLR1_EL0 = 56984,
  AMCNTENSET1_EL0 = 56985,
  AMEVCNTR10_EL0 = 57056,
  AMEVCNTR11_EL0 = 57057,
  AMEVCNTR12_EL0 = 57058,
  AMEVCNTR13_EL0 = 57059,
  AMEVCNTR14_EL0 = 57060,
  AMEVCNTR15_EL0 = 57061,
  AMEVCNTR16_EL0 = 57062,
  AMEVCNTR17_EL0 = 57063,
  AMEVCNTR18_EL0 = 57064,
  AMEVCNTR19_EL0 = 57065,
  AMEVCNTR110_EL0 = 57066,
  AMEVCNTR111_EL0 = 57067,
  AMEVCNTR112_EL0 = 57068,
  AMEVCNTR113_EL0 = 57069,
  AMEVCNTR114_EL0 = 57070,
  AMEVCNTR115_EL0 = 57071,
  AMEVTYPER10_EL0 = 57072,
  AMEVTYPER11_EL0 = 57073,
  AMEVTYPER12_EL0 = 57074,
  AMEVTYPER13_EL0 = 57075,
  AMEVTYPER14_EL0 = 57076,
  AMEVTYPER15_EL0 = 57077,
  AMEVTYPER16_EL0 = 57078,
  AMEVTYPER17_EL0 = 57079,
  AMEVTYPER18_EL0 = 57080,
  AMEVTYPER19_EL0 = 57081,
  AMEVTYPER110_EL0 = 57082,
  AMEVTYPER111_EL0 = 57083,
  AMEVTYPER112_EL0 = 57084,
  AMEVTYPER113_EL0 = 57085,
  AMEVTYPER114_EL0 = 57086,
  AMEVTYPER115_EL0 = 57087,
  TRFCR_EL1 = 49297,
  TRFCR_EL2 = 57489,
  TRFCR_EL12 = 59537,
  DIT = 55829,
  VNCR_EL2 = 57616,
  ZCR_EL1 = 49296,
  ZCR_EL2 = 57488,
  ZCR_EL3 = 61584,
  ZCR_EL12 = 59536,
  CPM_IOACC_CTL_EL3 = 65424,
};
#endif

#ifdef GET_TLBI_DECL
enum TLBIValues {
  IPAS2E1IS = 9217,
  IPAS2LE1IS = 9221,
  VMALLE1IS = 1048,
  ALLE2IS = 9240,
  ALLE3IS = 13336,
  VAE1IS = 1049,
  VAE2IS = 9241,
  VAE3IS = 13337,
  ASIDE1IS = 1050,
  VAAE1IS = 1051,
  ALLE1IS = 9244,
  VALE1IS = 1053,
  VALE2IS = 9245,
  VALE3IS = 13341,
  VMALLS12E1IS = 9246,
  VAALE1IS = 1055,
  IPAS2E1 = 9249,
  IPAS2LE1 = 9253,
  VMALLE1 = 1080,
  ALLE2 = 9272,
  ALLE3 = 13368,
  VAE1 = 1081,
  VAE2 = 9273,
  VAE3 = 13369,
  ASIDE1 = 1082,
  VAAE1 = 1083,
  ALLE1 = 9276,
  VALE1 = 1085,
  VALE2 = 9277,
  VALE3 = 13373,
  VMALLS12E1 = 9278,
  VAALE1 = 1087,
  VMALLE1OS = 1032,
  VAE1OS = 1033,
  ASIDE1OS = 1034,
  VAAE1OS = 1035,
  VALE1OS = 1037,
  VAALE1OS = 1039,
  IPAS2E1OS = 9248,
  IPAS2LE1OS = 9252,
  VAE2OS = 9225,
  VALE2OS = 9229,
  VMALLS12E1OS = 9230,
  VAE3OS = 13321,
  VALE3OS = 13325,
  ALLE2OS = 9224,
  ALLE1OS = 9228,
  ALLE3OS = 13320,
  RVAE1 = 1073,
  RVAAE1 = 1075,
  RVALE1 = 1077,
  RVAALE1 = 1079,
  RVAE1IS = 1041,
  RVAAE1IS = 1043,
  RVALE1IS = 1045,
  RVAALE1IS = 1047,
  RVAE1OS = 1065,
  RVAAE1OS = 1067,
  RVALE1OS = 1069,
  RVAALE1OS = 1071,
  RIPAS2E1IS = 9218,
  RIPAS2LE1IS = 9222,
  RIPAS2E1 = 9250,
  RIPAS2LE1 = 9254,
  RIPAS2E1OS = 9251,
  RIPAS2LE1OS = 9255,
  RVAE2 = 9265,
  RVALE2 = 9269,
  RVAE2IS = 9233,
  RVALE2IS = 9237,
  RVAE2OS = 9257,
  RVALE2OS = 9261,
  RVAE3 = 13361,
  RVALE3 = 13365,
  RVAE3IS = 13329,
  RVALE3IS = 13333,
  RVAE3OS = 13353,
  RVALE3OS = 13357,
};
#endif

#ifdef GET_TSB_DECL
enum TSBValues {
  csync = 0,
};
#endif

#ifdef GET_AT_DECL
const AT *lookupATByName(StringRef Name);
const AT *lookupATByEncoding(uint16_t Encoding);
#endif

#ifdef GET_AT_IMPL
const AT ATsList[] = {
  { "S1E1R", 0x3C0,  {}  }, // 0
  { "S1E2R", 0x23C0,  {}  }, // 1
  { "S1E3R", 0x33C0,  {}  }, // 2
  { "S1E1W", 0x3C1,  {}  }, // 3
  { "S1E2W", 0x23C1,  {}  }, // 4
  { "S1E3W", 0x33C1,  {}  }, // 5
  { "S1E0R", 0x3C2,  {}  }, // 6
  { "S1E0W", 0x3C3,  {}  }, // 7
  { "S12E1R", 0x23C4,  {}  }, // 8
  { "S12E1W", 0x23C5,  {}  }, // 9
  { "S12E0R", 0x23C6,  {}  }, // 10
  { "S12E0W", 0x23C7,  {}  }, // 11
  { "S1E1RP", 0x3C8,  {AArch64::HasV8_2aOps}  }, // 12
  { "S1E1WP", 0x3C9,  {AArch64::HasV8_2aOps}  }, // 13
 };

const AT *lookupATByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "S12E0R", 10 },
    { "S12E0W", 11 },
    { "S12E1R", 8 },
    { "S12E1W", 9 },
    { "S1E0R", 6 },
    { "S1E0W", 7 },
    { "S1E1R", 0 },
    { "S1E1RP", 12 },
    { "S1E1W", 3 },
    { "S1E1WP", 13 },
    { "S1E2R", 1 },
    { "S1E2W", 4 },
    { "S1E3R", 2 },
    { "S1E3W", 5 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &ATsList[Idx->_index];
}

const AT *lookupATByEncoding(uint16_t Encoding) {
  struct IndexType {
    uint16_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x3C0, 0 },
    { 0x3C1, 3 },
    { 0x3C2, 6 },
    { 0x3C3, 7 },
    { 0x3C8, 12 },
    { 0x3C9, 13 },
    { 0x23C0, 1 },
    { 0x23C1, 4 },
    { 0x23C4, 8 },
    { 0x23C5, 9 },
    { 0x23C6, 10 },
    { 0x23C7, 11 },
    { 0x33C0, 2 },
    { 0x33C1, 5 },
  };

  struct KeyType {
    uint16_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &ATsList[Idx->_index];
}
#endif

#ifdef GET_DB_DECL
const DB *lookupDBByName(StringRef Name);
const DB *lookupDBByEncoding(uint8_t Encoding);
#endif

#ifdef GET_DB_IMPL
const DB DBsList[] = {
  { "oshld", 0x1 }, // 0
  { "oshst", 0x2 }, // 1
  { "osh", 0x3 }, // 2
  { "nshld", 0x5 }, // 3
  { "nshst", 0x6 }, // 4
  { "nsh", 0x7 }, // 5
  { "ishld", 0x9 }, // 6
  { "ishst", 0xA }, // 7
  { "ish", 0xB }, // 8
  { "ld", 0xD }, // 9
  { "st", 0xE }, // 10
  { "sy", 0xF }, // 11
 };

const DB *lookupDBByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "ISH", 8 },
    { "ISHLD", 6 },
    { "ISHST", 7 },
    { "LD", 9 },
    { "NSH", 5 },
    { "NSHLD", 3 },
    { "NSHST", 4 },
    { "OSH", 2 },
    { "OSHLD", 0 },
    { "OSHST", 1 },
    { "ST", 10 },
    { "SY", 11 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &DBsList[Idx->_index];
}

const DB *lookupDBByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x1, 0 },
    { 0x2, 1 },
    { 0x3, 2 },
    { 0x5, 3 },
    { 0x6, 4 },
    { 0x7, 5 },
    { 0x9, 6 },
    { 0xA, 7 },
    { 0xB, 8 },
    { 0xD, 9 },
    { 0xE, 10 },
    { 0xF, 11 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &DBsList[Idx->_index];
}
#endif

#ifdef GET_DC_DECL
const DC *lookupDCByName(StringRef Name);
const DC *lookupDCByEncoding(uint16_t Encoding);
#endif

#ifdef GET_DC_IMPL
const DC DCsList[] = {
  { "ZVA", 0x1BA1,  {}  }, // 0
  { "IVAC", 0x3B1,  {}  }, // 1
  { "ISW", 0x3B2,  {}  }, // 2
  { "CVAC", 0x1BD1,  {}  }, // 3
  { "CSW", 0x3D2,  {}  }, // 4
  { "CVAU", 0x1BD9,  {}  }, // 5
  { "CIVAC", 0x1BF1,  {}  }, // 6
  { "CISW", 0x3F2,  {}  }, // 7
  { "CVAP", 0x1BE1,  {AArch64::HasV8_2aOps}  }, // 8
 };

const DC *lookupDCByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "CISW", 7 },
    { "CIVAC", 6 },
    { "CSW", 4 },
    { "CVAC", 3 },
    { "CVAP", 8 },
    { "CVAU", 5 },
    { "ISW", 2 },
    { "IVAC", 1 },
    { "ZVA", 0 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &DCsList[Idx->_index];
}

const DC *lookupDCByEncoding(uint16_t Encoding) {
  struct IndexType {
    uint16_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x3B1, 1 },
    { 0x3B2, 2 },
    { 0x3D2, 4 },
    { 0x3F2, 7 },
    { 0x1BA1, 0 },
    { 0x1BD1, 3 },
    { 0x1BD9, 5 },
    { 0x1BE1, 8 },
    { 0x1BF1, 6 },
  };

  struct KeyType {
    uint16_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &DCsList[Idx->_index];
}
#endif

#ifdef GET_EXACTFPIMM_DECL
const ExactFPImm *lookupExactFPImmByEnum(uint8_t Enum);
const ExactFPImm *lookupExactFPImmByRepr(StringRef Repr);
#endif

#ifdef GET_EXACTFPIMM_IMPL
const ExactFPImm ExactFPImmsList[] = {
  { "zero", 0x0, "0.0" }, // 0
  { "half", 0x1, "0.5" }, // 1
  { "one", 0x2, "1.0" }, // 2
  { "two", 0x3, "2.0" }, // 3
 };

const ExactFPImm *lookupExactFPImmByEnum(uint8_t Enum) {
  struct IndexType {
    uint8_t Enum;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x0, 0 },
    { 0x1, 1 },
    { 0x2, 2 },
    { 0x3, 3 },
  };

  auto Table = makeArrayRef(Index);
  size_t Idx = Enum;
  return Idx >= Table.size() ? nullptr : &ExactFPImmsList[Table[Idx]._index];
}

const ExactFPImm *lookupExactFPImmByRepr(StringRef Repr) {
  struct IndexType {
    const char * Repr;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "0.0", 0 },
    { "0.5", 1 },
    { "1.0", 2 },
    { "2.0", 3 },
  };

  struct KeyType {
    std::string Repr;
  };
  KeyType Key = { Repr.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpRepr = StringRef(LHS.Repr).compare(RHS.Repr);
      if (CmpRepr < 0) return true;
      if (CmpRepr > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Repr != Idx->Repr)
    return nullptr;
  return &ExactFPImmsList[Idx->_index];
}
#endif

#ifdef GET_IC_DECL
const IC *lookupICByName(StringRef Name);
const IC *lookupICByEncoding(uint16_t Encoding);
#endif

#ifdef GET_IC_IMPL
const IC ICsList[] = {
  { "IALLUIS", 0x388, false }, // 0
  { "IALLU", 0x3A8, false }, // 1
  { "IVAU", 0x1BA9, true }, // 2
 };

const IC *lookupICByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "IALLU", 1 },
    { "IALLUIS", 0 },
    { "IVAU", 2 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &ICsList[Idx->_index];
}

const IC *lookupICByEncoding(uint16_t Encoding) {
  struct IndexType {
    uint16_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x388, 0 },
    { 0x3A8, 1 },
    { 0x1BA9, 2 },
  };

  struct KeyType {
    uint16_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &ICsList[Idx->_index];
}
#endif

#ifdef GET_ISB_DECL
const ISB *lookupISBByName(StringRef Name);
const ISB *lookupISBByEncoding(uint8_t Encoding);
#endif

#ifdef GET_ISB_IMPL
const ISB ISBsList[] = {
  { "sy", 0xF }, // 0
 };

const ISB *lookupISBByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "SY", 0 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &ISBsList[Idx->_index];
}

const ISB *lookupISBByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0xF, 0 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &ISBsList[Idx->_index];
}
#endif

#ifdef GET_PRFM_DECL
const PRFM *lookupPRFMByName(StringRef Name);
const PRFM *lookupPRFMByEncoding(uint8_t Encoding);
#endif

#ifdef GET_PRFM_IMPL
const PRFM PRFMsList[] = {
  { "pldl1keep", 0x0 }, // 0
  { "pldl1strm", 0x1 }, // 1
  { "pldl2keep", 0x2 }, // 2
  { "pldl2strm", 0x3 }, // 3
  { "pldl3keep", 0x4 }, // 4
  { "pldl3strm", 0x5 }, // 5
  { "plil1keep", 0x8 }, // 6
  { "plil1strm", 0x9 }, // 7
  { "plil2keep", 0xA }, // 8
  { "plil2strm", 0xB }, // 9
  { "plil3keep", 0xC }, // 10
  { "plil3strm", 0xD }, // 11
  { "pstl1keep", 0x10 }, // 12
  { "pstl1strm", 0x11 }, // 13
  { "pstl2keep", 0x12 }, // 14
  { "pstl2strm", 0x13 }, // 15
  { "pstl3keep", 0x14 }, // 16
  { "pstl3strm", 0x15 }, // 17
 };

const PRFM *lookupPRFMByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "PLDL1KEEP", 0 },
    { "PLDL1STRM", 1 },
    { "PLDL2KEEP", 2 },
    { "PLDL2STRM", 3 },
    { "PLDL3KEEP", 4 },
    { "PLDL3STRM", 5 },
    { "PLIL1KEEP", 6 },
    { "PLIL1STRM", 7 },
    { "PLIL2KEEP", 8 },
    { "PLIL2STRM", 9 },
    { "PLIL3KEEP", 10 },
    { "PLIL3STRM", 11 },
    { "PSTL1KEEP", 12 },
    { "PSTL1STRM", 13 },
    { "PSTL2KEEP", 14 },
    { "PSTL2STRM", 15 },
    { "PSTL3KEEP", 16 },
    { "PSTL3STRM", 17 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &PRFMsList[Idx->_index];
}

const PRFM *lookupPRFMByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x0, 0 },
    { 0x1, 1 },
    { 0x2, 2 },
    { 0x3, 3 },
    { 0x4, 4 },
    { 0x5, 5 },
    { 0x8, 6 },
    { 0x9, 7 },
    { 0xA, 8 },
    { 0xB, 9 },
    { 0xC, 10 },
    { 0xD, 11 },
    { 0x10, 12 },
    { 0x11, 13 },
    { 0x12, 14 },
    { 0x13, 15 },
    { 0x14, 16 },
    { 0x15, 17 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &PRFMsList[Idx->_index];
}
#endif

#ifdef GET_PSB_DECL
const PSB *lookupPSBByName(StringRef Name);
const PSB *lookupPSBByEncoding(uint8_t Encoding);
#endif

#ifdef GET_PSB_IMPL
const PSB PSBsList[] = {
  { "csync", 0x11 }, // 0
 };

const PSB *lookupPSBByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "CSYNC", 0 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &PSBsList[Idx->_index];
}

const PSB *lookupPSBByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x11, 0 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &PSBsList[Idx->_index];
}
#endif

#ifdef GET_PSTATE_DECL
const PState *lookupPStateByName(StringRef Name);
const PState *lookupPStateByEncoding(uint8_t Encoding);
#endif

#ifdef GET_PSTATE_IMPL
const PState PStatesList[] = {
  { "SPSel", 0x5,  {}  }, // 0
  { "DAIFSet", 0x1E,  {}  }, // 1
  { "DAIFClr", 0x1F,  {}  }, // 2
  { "PAN", 0x4,  {AArch64::HasV8_1aOps}  }, // 3
  { "UAO", 0x3,  {AArch64::HasV8_2aOps}  }, // 4
  { "DIT", 0x1A,  {AArch64::HasV8_4aOps}  }, // 5
 };

const PState *lookupPStateByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "DAIFCLR", 2 },
    { "DAIFSET", 1 },
    { "DIT", 5 },
    { "PAN", 3 },
    { "SPSEL", 0 },
    { "UAO", 4 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &PStatesList[Idx->_index];
}

const PState *lookupPStateByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x3, 4 },
    { 0x4, 3 },
    { 0x5, 0 },
    { 0x1A, 5 },
    { 0x1E, 1 },
    { 0x1F, 2 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &PStatesList[Idx->_index];
}
#endif

#ifdef GET_SVEPREDPAT_DECL
const SVEPREDPAT *lookupSVEPREDPATByName(StringRef Name);
const SVEPREDPAT *lookupSVEPREDPATByEncoding(uint8_t Encoding);
#endif

#ifdef GET_SVEPREDPAT_IMPL
const SVEPREDPAT SVEPREDPATsList[] = {
  { "pow2", 0x0 }, // 0
  { "vl1", 0x1 }, // 1
  { "vl2", 0x2 }, // 2
  { "vl3", 0x3 }, // 3
  { "vl4", 0x4 }, // 4
  { "vl5", 0x5 }, // 5
  { "vl6", 0x6 }, // 6
  { "vl7", 0x7 }, // 7
  { "vl8", 0x8 }, // 8
  { "vl16", 0x9 }, // 9
  { "vl32", 0xA }, // 10
  { "vl64", 0xB }, // 11
  { "vl128", 0xC }, // 12
  { "vl256", 0xD }, // 13
  { "mul4", 0x1D }, // 14
  { "mul3", 0x1E }, // 15
  { "all", 0x1F }, // 16
 };

const SVEPREDPAT *lookupSVEPREDPATByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "ALL", 16 },
    { "MUL3", 15 },
    { "MUL4", 14 },
    { "POW2", 0 },
    { "VL1", 1 },
    { "VL128", 12 },
    { "VL16", 9 },
    { "VL2", 2 },
    { "VL256", 13 },
    { "VL3", 3 },
    { "VL32", 10 },
    { "VL4", 4 },
    { "VL5", 5 },
    { "VL6", 6 },
    { "VL64", 11 },
    { "VL7", 7 },
    { "VL8", 8 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &SVEPREDPATsList[Idx->_index];
}

const SVEPREDPAT *lookupSVEPREDPATByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x0, 0 },
    { 0x1, 1 },
    { 0x2, 2 },
    { 0x3, 3 },
    { 0x4, 4 },
    { 0x5, 5 },
    { 0x6, 6 },
    { 0x7, 7 },
    { 0x8, 8 },
    { 0x9, 9 },
    { 0xA, 10 },
    { 0xB, 11 },
    { 0xC, 12 },
    { 0xD, 13 },
    { 0x1D, 14 },
    { 0x1E, 15 },
    { 0x1F, 16 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &SVEPREDPATsList[Idx->_index];
}
#endif

#ifdef GET_SVEPRFM_DECL
const SVEPRFM *lookupSVEPRFMByName(StringRef Name);
const SVEPRFM *lookupSVEPRFMByEncoding(uint8_t Encoding);
#endif

#ifdef GET_SVEPRFM_IMPL
const SVEPRFM SVEPRFMsList[] = {
  { "pldl1keep", 0x0,  {AArch64::FeatureSVE}  }, // 0
  { "pldl1strm", 0x1,  {AArch64::FeatureSVE}  }, // 1
  { "pldl2keep", 0x2,  {AArch64::FeatureSVE}  }, // 2
  { "pldl2strm", 0x3,  {AArch64::FeatureSVE}  }, // 3
  { "pldl3keep", 0x4,  {AArch64::FeatureSVE}  }, // 4
  { "pldl3strm", 0x5,  {AArch64::FeatureSVE}  }, // 5
  { "pstl1keep", 0x8,  {AArch64::FeatureSVE}  }, // 6
  { "pstl1strm", 0x9,  {AArch64::FeatureSVE}  }, // 7
  { "pstl2keep", 0xA,  {AArch64::FeatureSVE}  }, // 8
  { "pstl2strm", 0xB,  {AArch64::FeatureSVE}  }, // 9
  { "pstl3keep", 0xC,  {AArch64::FeatureSVE}  }, // 10
  { "pstl3strm", 0xD,  {AArch64::FeatureSVE}  }, // 11
 };

const SVEPRFM *lookupSVEPRFMByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "PLDL1KEEP", 0 },
    { "PLDL1STRM", 1 },
    { "PLDL2KEEP", 2 },
    { "PLDL2STRM", 3 },
    { "PLDL3KEEP", 4 },
    { "PLDL3STRM", 5 },
    { "PSTL1KEEP", 6 },
    { "PSTL1STRM", 7 },
    { "PSTL2KEEP", 8 },
    { "PSTL2STRM", 9 },
    { "PSTL3KEEP", 10 },
    { "PSTL3STRM", 11 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &SVEPRFMsList[Idx->_index];
}

const SVEPRFM *lookupSVEPRFMByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x0, 0 },
    { 0x1, 1 },
    { 0x2, 2 },
    { 0x3, 3 },
    { 0x4, 4 },
    { 0x5, 5 },
    { 0x8, 6 },
    { 0x9, 7 },
    { 0xA, 8 },
    { 0xB, 9 },
    { 0xC, 10 },
    { 0xD, 11 },
  };

  struct KeyType {
    uint8_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &SVEPRFMsList[Idx->_index];
}
#endif

#ifdef GET_SYSREG_DECL
const SysReg *lookupSysRegByName(StringRef Name);
const SysReg *lookupSysRegByEncoding(uint16_t Encoding);
#endif

#ifdef GET_SYSREG_IMPL
const SysReg SysRegsList[] = {
  { "MDCCSR_EL0", 0x9808, true, false,  {}  }, // 0
  { "DBGDTRRX_EL0", 0x9828, true, false,  {}  }, // 1
  { "MDRAR_EL1", 0x8080, true, false,  {}  }, // 2
  { "OSLSR_EL1", 0x808C, true, false,  {}  }, // 3
  { "DBGAUTHSTATUS_EL1", 0x83F6, true, false,  {}  }, // 4
  { "PMCEID0_EL0", 0xDCE6, true, false,  {}  }, // 5
  { "PMCEID1_EL0", 0xDCE7, true, false,  {}  }, // 6
  { "MIDR_EL1", 0xC000, true, false,  {}  }, // 7
  { "CCSIDR_EL1", 0xC800, true, false,  {}  }, // 8
  { "CCSIDR2_EL1", 0xC802, true, false,  {AArch64::HasV8_3aOps}  }, // 9
  { "CLIDR_EL1", 0xC801, true, false,  {}  }, // 10
  { "CTR_EL0", 0xD801, true, false,  {}  }, // 11
  { "MPIDR_EL1", 0xC005, true, false,  {}  }, // 12
  { "REVIDR_EL1", 0xC006, true, false,  {}  }, // 13
  { "AIDR_EL1", 0xC807, true, false,  {}  }, // 14
  { "DCZID_EL0", 0xD807, true, false,  {}  }, // 15
  { "ID_PFR0_EL1", 0xC008, true, false,  {}  }, // 16
  { "ID_PFR1_EL1", 0xC009, true, false,  {}  }, // 17
  { "ID_DFR0_EL1", 0xC00A, true, false,  {}  }, // 18
  { "ID_AFR0_EL1", 0xC00B, true, false,  {}  }, // 19
  { "ID_MMFR0_EL1", 0xC00C, true, false,  {}  }, // 20
  { "ID_MMFR1_EL1", 0xC00D, true, false,  {}  }, // 21
  { "ID_MMFR2_EL1", 0xC00E, true, false,  {}  }, // 22
  { "ID_MMFR3_EL1", 0xC00F, true, false,  {}  }, // 23
  { "ID_ISAR0_EL1", 0xC010, true, false,  {}  }, // 24
  { "ID_ISAR1_EL1", 0xC011, true, false,  {}  }, // 25
  { "ID_ISAR2_EL1", 0xC012, true, false,  {}  }, // 26
  { "ID_ISAR3_EL1", 0xC013, true, false,  {}  }, // 27
  { "ID_ISAR4_EL1", 0xC014, true, false,  {}  }, // 28
  { "ID_ISAR5_EL1", 0xC015, true, false,  {}  }, // 29
  { "ID_ISAR6_EL1", 0xC017, true, false,  {AArch64::HasV8_2aOps}  }, // 30
  { "ID_AA64PFR0_EL1", 0xC020, true, false,  {}  }, // 31
  { "ID_AA64PFR1_EL1", 0xC021, true, false,  {}  }, // 32
  { "ID_AA64DFR0_EL1", 0xC028, true, false,  {}  }, // 33
  { "ID_AA64DFR1_EL1", 0xC029, true, false,  {}  }, // 34
  { "ID_AA64AFR0_EL1", 0xC02C, true, false,  {}  }, // 35
  { "ID_AA64AFR1_EL1", 0xC02D, true, false,  {}  }, // 36
  { "ID_AA64ISAR0_EL1", 0xC030, true, false,  {}  }, // 37
  { "ID_AA64ISAR1_EL1", 0xC031, true, false,  {}  }, // 38
  { "ID_AA64MMFR0_EL1", 0xC038, true, false,  {}  }, // 39
  { "ID_AA64MMFR1_EL1", 0xC039, true, false,  {}  }, // 40
  { "ID_AA64MMFR2_EL1", 0xC03A, true, false,  {AArch64::HasV8_2aOps}  }, // 41
  { "MVFR0_EL1", 0xC018, true, false,  {}  }, // 42
  { "MVFR1_EL1", 0xC019, true, false,  {}  }, // 43
  { "MVFR2_EL1", 0xC01A, true, false,  {}  }, // 44
  { "RVBAR_EL1", 0xC601, true, false,  {}  }, // 45
  { "RVBAR_EL2", 0xE601, true, false,  {}  }, // 46
  { "RVBAR_EL3", 0xF601, true, false,  {}  }, // 47
  { "ISR_EL1", 0xC608, true, false,  {}  }, // 48
  { "CNTPCT_EL0", 0xDF01, true, false,  {}  }, // 49
  { "CNTVCT_EL0", 0xDF02, true, false,  {}  }, // 50
  { "ID_MMFR4_EL1", 0xC016, true, false,  {}  }, // 51
  { "TRCSTATR", 0x8818, true, false,  {}  }, // 52
  { "TRCIDR8", 0x8806, true, false,  {}  }, // 53
  { "TRCIDR9", 0x880E, true, false,  {}  }, // 54
  { "TRCIDR10", 0x8816, true, false,  {}  }, // 55
  { "TRCIDR11", 0x881E, true, false,  {}  }, // 56
  { "TRCIDR12", 0x8826, true, false,  {}  }, // 57
  { "TRCIDR13", 0x882E, true, false,  {}  }, // 58
  { "TRCIDR0", 0x8847, true, false,  {}  }, // 59
  { "TRCIDR1", 0x884F, true, false,  {}  }, // 60
  { "TRCIDR2", 0x8857, true, false,  {}  }, // 61
  { "TRCIDR3", 0x885F, true, false,  {}  }, // 62
  { "TRCIDR4", 0x8867, true, false,  {}  }, // 63
  { "TRCIDR5", 0x886F, true, false,  {}  }, // 64
  { "TRCIDR6", 0x8877, true, false,  {}  }, // 65
  { "TRCIDR7", 0x887F, true, false,  {}  }, // 66
  { "TRCOSLSR", 0x888C, true, false,  {}  }, // 67
  { "TRCPDSR", 0x88AC, true, false,  {}  }, // 68
  { "TRCDEVAFF0", 0x8BD6, true, false,  {}  }, // 69
  { "TRCDEVAFF1", 0x8BDE, true, false,  {}  }, // 70
  { "TRCLSR", 0x8BEE, true, false,  {}  }, // 71
  { "TRCAUTHSTATUS", 0x8BF6, true, false,  {}  }, // 72
  { "TRCDEVARCH", 0x8BFE, true, false,  {}  }, // 73
  { "TRCDEVID", 0x8B97, true, false,  {}  }, // 74
  { "TRCDEVTYPE", 0x8B9F, true, false,  {}  }, // 75
  { "TRCPIDR4", 0x8BA7, true, false,  {}  }, // 76
  { "TRCPIDR5", 0x8BAF, true, false,  {}  }, // 77
  { "TRCPIDR6", 0x8BB7, true, false,  {}  }, // 78
  { "TRCPIDR7", 0x8BBF, true, false,  {}  }, // 79
  { "TRCPIDR0", 0x8BC7, true, false,  {}  }, // 80
  { "TRCPIDR1", 0x8BCF, true, false,  {}  }, // 81
  { "TRCPIDR2", 0x8BD7, true, false,  {}  }, // 82
  { "TRCPIDR3", 0x8BDF, true, false,  {}  }, // 83
  { "TRCCIDR0", 0x8BE7, true, false,  {}  }, // 84
  { "TRCCIDR1", 0x8BEF, true, false,  {}  }, // 85
  { "TRCCIDR2", 0x8BF7, true, false,  {}  }, // 86
  { "TRCCIDR3", 0x8BFF, true, false,  {}  }, // 87
  { "ICC_IAR1_EL1", 0xC660, true, false,  {}  }, // 88
  { "ICC_IAR0_EL1", 0xC640, true, false,  {}  }, // 89
  { "ICC_HPPIR1_EL1", 0xC662, true, false,  {}  }, // 90
  { "ICC_HPPIR0_EL1", 0xC642, true, false,  {}  }, // 91
  { "ICC_RPR_EL1", 0xC65B, true, false,  {}  }, // 92
  { "ICH_VTR_EL2", 0xE659, true, false,  {}  }, // 93
  { "ICH_EISR_EL2", 0xE65B, true, false,  {}  }, // 94
  { "ICH_ELRSR_EL2", 0xE65D, true, false,  {}  }, // 95
  { "ID_AA64ZFR0_EL1", 0xC024, true, false,  {AArch64::FeatureSVE}  }, // 96
  { "LORID_EL1", 0xC527, true, false,  {AArch64::HasV8_1aOps}  }, // 97
  { "ERRIDR_EL1", 0xC298, true, false,  {AArch64::FeatureRAS}  }, // 98
  { "ERXFR_EL1", 0xC2A0, true, false,  {AArch64::FeatureRAS}  }, // 99
  { "DBGDTRTX_EL0", 0x9828, false, true,  {}  }, // 100
  { "OSLAR_EL1", 0x8084, false, true,  {}  }, // 101
  { "PMSWINC_EL0", 0xDCE4, false, true,  {}  }, // 102
  { "TRCOSLAR", 0x8884, false, true,  {}  }, // 103
  { "TRCLAR", 0x8BE6, false, true,  {}  }, // 104
  { "ICC_EOIR1_EL1", 0xC661, false, true,  {}  }, // 105
  { "ICC_EOIR0_EL1", 0xC641, false, true,  {}  }, // 106
  { "ICC_DIR_EL1", 0xC659, false, true,  {}  }, // 107
  { "ICC_SGI1R_EL1", 0xC65D, false, true,  {}  }, // 108
  { "ICC_ASGI1R_EL1", 0xC65E, false, true,  {}  }, // 109
  { "ICC_SGI0R_EL1", 0xC65F, false, true,  {}  }, // 110
  { "OSDTRRX_EL1", 0x8002, true, true,  {}  }, // 111
  { "OSDTRTX_EL1", 0x801A, true, true,  {}  }, // 112
  { "TEECR32_EL1", 0x9000, true, true,  {}  }, // 113
  { "MDCCINT_EL1", 0x8010, true, true,  {}  }, // 114
  { "MDSCR_EL1", 0x8012, true, true,  {}  }, // 115
  { "DBGDTR_EL0", 0x9820, true, true,  {}  }, // 116
  { "OSECCR_EL1", 0x8032, true, true,  {}  }, // 117
  { "DBGVCR32_EL2", 0xA038, true, true,  {}  }, // 118
  { "DBGBVR0_EL1", 0x8004, true, true,  {}  }, // 119
  { "DBGBVR1_EL1", 0x800C, true, true,  {}  }, // 120
  { "DBGBVR2_EL1", 0x8014, true, true,  {}  }, // 121
  { "DBGBVR3_EL1", 0x801C, true, true,  {}  }, // 122
  { "DBGBVR4_EL1", 0x8024, true, true,  {}  }, // 123
  { "DBGBVR5_EL1", 0x802C, true, true,  {}  }, // 124
  { "DBGBVR6_EL1", 0x8034, true, true,  {}  }, // 125
  { "DBGBVR7_EL1", 0x803C, true, true,  {}  }, // 126
  { "DBGBVR8_EL1", 0x8044, true, true,  {}  }, // 127
  { "DBGBVR9_EL1", 0x804C, true, true,  {}  }, // 128
  { "DBGBVR10_EL1", 0x8054, true, true,  {}  }, // 129
  { "DBGBVR11_EL1", 0x805C, true, true,  {}  }, // 130
  { "DBGBVR12_EL1", 0x8064, true, true,  {}  }, // 131
  { "DBGBVR13_EL1", 0x806C, true, true,  {}  }, // 132
  { "DBGBVR14_EL1", 0x8074, true, true,  {}  }, // 133
  { "DBGBVR15_EL1", 0x807C, true, true,  {}  }, // 134
  { "DBGBCR0_EL1", 0x8005, true, true,  {}  }, // 135
  { "DBGBCR1_EL1", 0x800D, true, true,  {}  }, // 136
  { "DBGBCR2_EL1", 0x8015, true, true,  {}  }, // 137
  { "DBGBCR3_EL1", 0x801D, true, true,  {}  }, // 138
  { "DBGBCR4_EL1", 0x8025, true, true,  {}  }, // 139
  { "DBGBCR5_EL1", 0x802D, true, true,  {}  }, // 140
  { "DBGBCR6_EL1", 0x8035, true, true,  {}  }, // 141
  { "DBGBCR7_EL1", 0x803D, true, true,  {}  }, // 142
  { "DBGBCR8_EL1", 0x8045, true, true,  {}  }, // 143
  { "DBGBCR9_EL1", 0x804D, true, true,  {}  }, // 144
  { "DBGBCR10_EL1", 0x8055, true, true,  {}  }, // 145
  { "DBGBCR11_EL1", 0x805D, true, true,  {}  }, // 146
  { "DBGBCR12_EL1", 0x8065, true, true,  {}  }, // 147
  { "DBGBCR13_EL1", 0x806D, true, true,  {}  }, // 148
  { "DBGBCR14_EL1", 0x8075, true, true,  {}  }, // 149
  { "DBGBCR15_EL1", 0x807D, true, true,  {}  }, // 150
  { "DBGWVR0_EL1", 0x8006, true, true,  {}  }, // 151
  { "DBGWVR1_EL1", 0x800E, true, true,  {}  }, // 152
  { "DBGWVR2_EL1", 0x8016, true, true,  {}  }, // 153
  { "DBGWVR3_EL1", 0x801E, true, true,  {}  }, // 154
  { "DBGWVR4_EL1", 0x8026, true, true,  {}  }, // 155
  { "DBGWVR5_EL1", 0x802E, true, true,  {}  }, // 156
  { "DBGWVR6_EL1", 0x8036, true, true,  {}  }, // 157
  { "DBGWVR7_EL1", 0x803E, true, true,  {}  }, // 158
  { "DBGWVR8_EL1", 0x8046, true, true,  {}  }, // 159
  { "DBGWVR9_EL1", 0x804E, true, true,  {}  }, // 160
  { "DBGWVR10_EL1", 0x8056, true, true,  {}  }, // 161
  { "DBGWVR11_EL1", 0x805E, true, true,  {}  }, // 162
  { "DBGWVR12_EL1", 0x8066, true, true,  {}  }, // 163
  { "DBGWVR13_EL1", 0x806E, true, true,  {}  }, // 164
  { "DBGWVR14_EL1", 0x8076, true, true,  {}  }, // 165
  { "DBGWVR15_EL1", 0x807E, true, true,  {}  }, // 166
  { "DBGWCR0_EL1", 0x8007, true, true,  {}  }, // 167
  { "DBGWCR1_EL1", 0x800F, true, true,  {}  }, // 168
  { "DBGWCR2_EL1", 0x8017, true, true,  {}  }, // 169
  { "DBGWCR3_EL1", 0x801F, true, true,  {}  }, // 170
  { "DBGWCR4_EL1", 0x8027, true, true,  {}  }, // 171
  { "DBGWCR5_EL1", 0x802F, true, true,  {}  }, // 172
  { "DBGWCR6_EL1", 0x8037, true, true,  {}  }, // 173
  { "DBGWCR7_EL1", 0x803F, true, true,  {}  }, // 174
  { "DBGWCR8_EL1", 0x8047, true, true,  {}  }, // 175
  { "DBGWCR9_EL1", 0x804F, true, true,  {}  }, // 176
  { "DBGWCR10_EL1", 0x8057, true, true,  {}  }, // 177
  { "DBGWCR11_EL1", 0x805F, true, true,  {}  }, // 178
  { "DBGWCR12_EL1", 0x8067, true, true,  {}  }, // 179
  { "DBGWCR13_EL1", 0x806F, true, true,  {}  }, // 180
  { "DBGWCR14_EL1", 0x8077, true, true,  {}  }, // 181
  { "DBGWCR15_EL1", 0x807F, true, true,  {}  }, // 182
  { "TEEHBR32_EL1", 0x9080, true, true,  {}  }, // 183
  { "OSDLR_EL1", 0x809C, true, true,  {}  }, // 184
  { "DBGPRCR_EL1", 0x80A4, true, true,  {}  }, // 185
  { "DBGCLAIMSET_EL1", 0x83C6, true, true,  {}  }, // 186
  { "DBGCLAIMCLR_EL1", 0x83CE, true, true,  {}  }, // 187
  { "CSSELR_EL1", 0xD000, true, true,  {}  }, // 188
  { "VPIDR_EL2", 0xE000, true, true,  {}  }, // 189
  { "VMPIDR_EL2", 0xE005, true, true,  {}  }, // 190
  { "CPACR_EL1", 0xC082, true, true,  {}  }, // 191
  { "SCTLR_EL1", 0xC080, true, true,  {}  }, // 192
  { "SCTLR_EL2", 0xE080, true, true,  {}  }, // 193
  { "SCTLR_EL3", 0xF080, true, true,  {}  }, // 194
  { "ACTLR_EL1", 0xC081, true, true,  {}  }, // 195
  { "ACTLR_EL2", 0xE081, true, true,  {}  }, // 196
  { "ACTLR_EL3", 0xF081, true, true,  {}  }, // 197
  { "HCR_EL2", 0xE088, true, true,  {}  }, // 198
  { "SCR_EL3", 0xF088, true, true,  {}  }, // 199
  { "MDCR_EL2", 0xE089, true, true,  {}  }, // 200
  { "SDER32_EL3", 0xF089, true, true,  {}  }, // 201
  { "CPTR_EL2", 0xE08A, true, true,  {}  }, // 202
  { "CPTR_EL3", 0xF08A, true, true,  {}  }, // 203
  { "HSTR_EL2", 0xE08B, true, true,  {}  }, // 204
  { "HACR_EL2", 0xE08F, true, true,  {}  }, // 205
  { "MDCR_EL3", 0xF099, true, true,  {}  }, // 206
  { "TTBR0_EL1", 0xC100, true, true,  {}  }, // 207
  { "TTBR0_EL2", 0xE100, true, true,  {}  }, // 208
  { "TTBR0_EL3", 0xF100, true, true,  {}  }, // 209
  { "TTBR1_EL1", 0xC101, true, true,  {}  }, // 210
  { "TCR_EL1", 0xC102, true, true,  {}  }, // 211
  { "TCR_EL2", 0xE102, true, true,  {}  }, // 212
  { "TCR_EL3", 0xF102, true, true,  {}  }, // 213
  { "VTTBR_EL2", 0xE108, true, true,  {}  }, // 214
  { "VTCR_EL2", 0xE10A, true, true,  {}  }, // 215
  { "DACR32_EL2", 0xE180, true, true,  {}  }, // 216
  { "SPSR_EL1", 0xC200, true, true,  {}  }, // 217
  { "SPSR_EL2", 0xE200, true, true,  {}  }, // 218
  { "SPSR_EL3", 0xF200, true, true,  {}  }, // 219
  { "ELR_EL1", 0xC201, true, true,  {}  }, // 220
  { "ELR_EL2", 0xE201, true, true,  {}  }, // 221
  { "ELR_EL3", 0xF201, true, true,  {}  }, // 222
  { "SP_EL0", 0xC208, true, true,  {}  }, // 223
  { "SP_EL1", 0xE208, true, true,  {}  }, // 224
  { "SP_EL2", 0xF208, true, true,  {}  }, // 225
  { "SPSel", 0xC210, true, true,  {}  }, // 226
  { "NZCV", 0xDA10, true, true,  {}  }, // 227
  { "DAIF", 0xDA11, true, true,  {}  }, // 228
  { "CurrentEL", 0xC212, true, true,  {}  }, // 229
  { "SPSR_irq", 0xE218, true, true,  {}  }, // 230
  { "SPSR_abt", 0xE219, true, true,  {}  }, // 231
  { "SPSR_und", 0xE21A, true, true,  {}  }, // 232
  { "SPSR_fiq", 0xE21B, true, true,  {}  }, // 233
  { "FPCR", 0xDA20, true, true,  {}  }, // 234
  { "FPSR", 0xDA21, true, true,  {}  }, // 235
  { "DSPSR_EL0", 0xDA28, true, true,  {}  }, // 236
  { "DLR_EL0", 0xDA29, true, true,  {}  }, // 237
  { "IFSR32_EL2", 0xE281, true, true,  {}  }, // 238
  { "AFSR0_EL1", 0xC288, true, true,  {}  }, // 239
  { "AFSR0_EL2", 0xE288, true, true,  {}  }, // 240
  { "AFSR0_EL3", 0xF288, true, true,  {}  }, // 241
  { "AFSR1_EL1", 0xC289, true, true,  {}  }, // 242
  { "AFSR1_EL2", 0xE289, true, true,  {}  }, // 243
  { "AFSR1_EL3", 0xF289, true, true,  {}  }, // 244
  { "ESR_EL1", 0xC290, true, true,  {}  }, // 245
  { "ESR_EL2", 0xE290, true, true,  {}  }, // 246
  { "ESR_EL3", 0xF290, true, true,  {}  }, // 247
  { "FPEXC32_EL2", 0xE298, true, true,  {}  }, // 248
  { "FAR_EL1", 0xC300, true, true,  {}  }, // 249
  { "FAR_EL2", 0xE300, true, true,  {}  }, // 250
  { "FAR_EL3", 0xF300, true, true,  {}  }, // 251
  { "HPFAR_EL2", 0xE304, true, true,  {}  }, // 252
  { "PAR_EL1", 0xC3A0, true, true,  {}  }, // 253
  { "PMCR_EL0", 0xDCE0, true, true,  {}  }, // 254
  { "PMCNTENSET_EL0", 0xDCE1, true, true,  {}  }, // 255
  { "PMCNTENCLR_EL0", 0xDCE2, true, true,  {}  }, // 256
  { "PMOVSCLR_EL0", 0xDCE3, true, true,  {}  }, // 257
  { "PMSELR_EL0", 0xDCE5, true, true,  {}  }, // 258
  { "PMCCNTR_EL0", 0xDCE8, true, true,  {}  }, // 259
  { "PMXEVTYPER_EL0", 0xDCE9, true, true,  {}  }, // 260
  { "PMXEVCNTR_EL0", 0xDCEA, true, true,  {}  }, // 261
  { "PMUSERENR_EL0", 0xDCF0, true, true,  {}  }, // 262
  { "PMINTENSET_EL1", 0xC4F1, true, true,  {}  }, // 263
  { "PMINTENCLR_EL1", 0xC4F2, true, true,  {}  }, // 264
  { "PMOVSSET_EL0", 0xDCF3, true, true,  {}  }, // 265
  { "MAIR_EL1", 0xC510, true, true,  {}  }, // 266
  { "MAIR_EL2", 0xE510, true, true,  {}  }, // 267
  { "MAIR_EL3", 0xF510, true, true,  {}  }, // 268
  { "AMAIR_EL1", 0xC518, true, true,  {}  }, // 269
  { "AMAIR_EL2", 0xE518, true, true,  {}  }, // 270
  { "AMAIR_EL3", 0xF518, true, true,  {}  }, // 271
  { "VBAR_EL1", 0xC600, true, true,  {}  }, // 272
  { "VBAR_EL2", 0xE600, true, true,  {}  }, // 273
  { "VBAR_EL3", 0xF600, true, true,  {}  }, // 274
  { "RMR_EL1", 0xC602, true, true,  {}  }, // 275
  { "RMR_EL2", 0xE602, true, true,  {}  }, // 276
  { "RMR_EL3", 0xF602, true, true,  {}  }, // 277
  { "CONTEXTIDR_EL1", 0xC681, true, true,  {}  }, // 278
  { "TPIDR_EL0", 0xDE82, true, true,  {}  }, // 279
  { "TPIDR_EL2", 0xE682, true, true,  {}  }, // 280
  { "TPIDR_EL3", 0xF682, true, true,  {}  }, // 281
  { "TPIDRRO_EL0", 0xDE83, true, true,  {}  }, // 282
  { "TPIDR_EL1", 0xC684, true, true,  {}  }, // 283
  { "CNTFRQ_EL0", 0xDF00, true, true,  {}  }, // 284
  { "CNTVOFF_EL2", 0xE703, true, true,  {}  }, // 285
  { "CNTKCTL_EL1", 0xC708, true, true,  {}  }, // 286
  { "CNTHCTL_EL2", 0xE708, true, true,  {}  }, // 287
  { "CNTP_TVAL_EL0", 0xDF10, true, true,  {}  }, // 288
  { "CNTHP_TVAL_EL2", 0xE710, true, true,  {}  }, // 289
  { "CNTPS_TVAL_EL1", 0xFF10, true, true,  {}  }, // 290
  { "CNTP_CTL_EL0", 0xDF11, true, true,  {}  }, // 291
  { "CNTHP_CTL_EL2", 0xE711, true, true,  {}  }, // 292
  { "CNTPS_CTL_EL1", 0xFF11, true, true,  {}  }, // 293
  { "CNTP_CVAL_EL0", 0xDF12, true, true,  {}  }, // 294
  { "CNTHP_CVAL_EL2", 0xE712, true, true,  {}  }, // 295
  { "CNTPS_CVAL_EL1", 0xFF12, true, true,  {}  }, // 296
  { "CNTV_TVAL_EL0", 0xDF18, true, true,  {}  }, // 297
  { "CNTV_CTL_EL0", 0xDF19, true, true,  {}  }, // 298
  { "CNTV_CVAL_EL0", 0xDF1A, true, true,  {}  }, // 299
  { "PMEVCNTR0_EL0", 0xDF40, true, true,  {}  }, // 300
  { "PMEVCNTR1_EL0", 0xDF41, true, true,  {}  }, // 301
  { "PMEVCNTR2_EL0", 0xDF42, true, true,  {}  }, // 302
  { "PMEVCNTR3_EL0", 0xDF43, true, true,  {}  }, // 303
  { "PMEVCNTR4_EL0", 0xDF44, true, true,  {}  }, // 304
  { "PMEVCNTR5_EL0", 0xDF45, true, true,  {}  }, // 305
  { "PMEVCNTR6_EL0", 0xDF46, true, true,  {}  }, // 306
  { "PMEVCNTR7_EL0", 0xDF47, true, true,  {}  }, // 307
  { "PMEVCNTR8_EL0", 0xDF48, true, true,  {}  }, // 308
  { "PMEVCNTR9_EL0", 0xDF49, true, true,  {}  }, // 309
  { "PMEVCNTR10_EL0", 0xDF4A, true, true,  {}  }, // 310
  { "PMEVCNTR11_EL0", 0xDF4B, true, true,  {}  }, // 311
  { "PMEVCNTR12_EL0", 0xDF4C, true, true,  {}  }, // 312
  { "PMEVCNTR13_EL0", 0xDF4D, true, true,  {}  }, // 313
  { "PMEVCNTR14_EL0", 0xDF4E, true, true,  {}  }, // 314
  { "PMEVCNTR15_EL0", 0xDF4F, true, true,  {}  }, // 315
  { "PMEVCNTR16_EL0", 0xDF50, true, true,  {}  }, // 316
  { "PMEVCNTR17_EL0", 0xDF51, true, true,  {}  }, // 317
  { "PMEVCNTR18_EL0", 0xDF52, true, true,  {}  }, // 318
  { "PMEVCNTR19_EL0", 0xDF53, true, true,  {}  }, // 319
  { "PMEVCNTR20_EL0", 0xDF54, true, true,  {}  }, // 320
  { "PMEVCNTR21_EL0", 0xDF55, true, true,  {}  }, // 321
  { "PMEVCNTR22_EL0", 0xDF56, true, true,  {}  }, // 322
  { "PMEVCNTR23_EL0", 0xDF57, true, true,  {}  }, // 323
  { "PMEVCNTR24_EL0", 0xDF58, true, true,  {}  }, // 324
  { "PMEVCNTR25_EL0", 0xDF59, true, true,  {}  }, // 325
  { "PMEVCNTR26_EL0", 0xDF5A, true, true,  {}  }, // 326
  { "PMEVCNTR27_EL0", 0xDF5B, true, true,  {}  }, // 327
  { "PMEVCNTR28_EL0", 0xDF5C, true, true,  {}  }, // 328
  { "PMEVCNTR29_EL0", 0xDF5D, true, true,  {}  }, // 329
  { "PMEVCNTR30_EL0", 0xDF5E, true, true,  {}  }, // 330
  { "PMCCFILTR_EL0", 0xDF7F, true, true,  {}  }, // 331
  { "PMEVTYPER0_EL0", 0xDF60, true, true,  {}  }, // 332
  { "PMEVTYPER1_EL0", 0xDF61, true, true,  {}  }, // 333
  { "PMEVTYPER2_EL0", 0xDF62, true, true,  {}  }, // 334
  { "PMEVTYPER3_EL0", 0xDF63, true, true,  {}  }, // 335
  { "PMEVTYPER4_EL0", 0xDF64, true, true,  {}  }, // 336
  { "PMEVTYPER5_EL0", 0xDF65, true, true,  {}  }, // 337
  { "PMEVTYPER6_EL0", 0xDF66, true, true,  {}  }, // 338
  { "PMEVTYPER7_EL0", 0xDF67, true, true,  {}  }, // 339
  { "PMEVTYPER8_EL0", 0xDF68, true, true,  {}  }, // 340
  { "PMEVTYPER9_EL0", 0xDF69, true, true,  {}  }, // 341
  { "PMEVTYPER10_EL0", 0xDF6A, true, true,  {}  }, // 342
  { "PMEVTYPER11_EL0", 0xDF6B, true, true,  {}  }, // 343
  { "PMEVTYPER12_EL0", 0xDF6C, true, true,  {}  }, // 344
  { "PMEVTYPER13_EL0", 0xDF6D, true, true,  {}  }, // 345
  { "PMEVTYPER14_EL0", 0xDF6E, true, true,  {}  }, // 346
  { "PMEVTYPER15_EL0", 0xDF6F, true, true,  {}  }, // 347
  { "PMEVTYPER16_EL0", 0xDF70, true, true,  {}  }, // 348
  { "PMEVTYPER17_EL0", 0xDF71, true, true,  {}  }, // 349
  { "PMEVTYPER18_EL0", 0xDF72, true, true,  {}  }, // 350
  { "PMEVTYPER19_EL0", 0xDF73, true, true,  {}  }, // 351
  { "PMEVTYPER20_EL0", 0xDF74, true, true,  {}  }, // 352
  { "PMEVTYPER21_EL0", 0xDF75, true, true,  {}  }, // 353
  { "PMEVTYPER22_EL0", 0xDF76, true, true,  {}  }, // 354
  { "PMEVTYPER23_EL0", 0xDF77, true, true,  {}  }, // 355
  { "PMEVTYPER24_EL0", 0xDF78, true, true,  {}  }, // 356
  { "PMEVTYPER25_EL0", 0xDF79, true, true,  {}  }, // 357
  { "PMEVTYPER26_EL0", 0xDF7A, true, true,  {}  }, // 358
  { "PMEVTYPER27_EL0", 0xDF7B, true, true,  {}  }, // 359
  { "PMEVTYPER28_EL0", 0xDF7C, true, true,  {}  }, // 360
  { "PMEVTYPER29_EL0", 0xDF7D, true, true,  {}  }, // 361
  { "PMEVTYPER30_EL0", 0xDF7E, true, true,  {}  }, // 362
  { "TRCPRGCTLR", 0x8808, true, true,  {}  }, // 363
  { "TRCPROCSELR", 0x8810, true, true,  {}  }, // 364
  { "TRCCONFIGR", 0x8820, true, true,  {}  }, // 365
  { "TRCAUXCTLR", 0x8830, true, true,  {}  }, // 366
  { "TRCEVENTCTL0R", 0x8840, true, true,  {}  }, // 367
  { "TRCEVENTCTL1R", 0x8848, true, true,  {}  }, // 368
  { "TRCSTALLCTLR", 0x8858, true, true,  {}  }, // 369
  { "TRCTSCTLR", 0x8860, true, true,  {}  }, // 370
  { "TRCSYNCPR", 0x8868, true, true,  {}  }, // 371
  { "TRCCCCTLR", 0x8870, true, true,  {}  }, // 372
  { "TRCBBCTLR", 0x8878, true, true,  {}  }, // 373
  { "TRCTRACEIDR", 0x8801, true, true,  {}  }, // 374
  { "TRCQCTLR", 0x8809, true, true,  {}  }, // 375
  { "TRCVICTLR", 0x8802, true, true,  {}  }, // 376
  { "TRCVIIECTLR", 0x880A, true, true,  {}  }, // 377
  { "TRCVISSCTLR", 0x8812, true, true,  {}  }, // 378
  { "TRCVIPCSSCTLR", 0x881A, true, true,  {}  }, // 379
  { "TRCVDCTLR", 0x8842, true, true,  {}  }, // 380
  { "TRCVDSACCTLR", 0x884A, true, true,  {}  }, // 381
  { "TRCVDARCCTLR", 0x8852, true, true,  {}  }, // 382
  { "TRCSEQEVR0", 0x8804, true, true,  {}  }, // 383
  { "TRCSEQEVR1", 0x880C, true, true,  {}  }, // 384
  { "TRCSEQEVR2", 0x8814, true, true,  {}  }, // 385
  { "TRCSEQRSTEVR", 0x8834, true, true,  {}  }, // 386
  { "TRCSEQSTR", 0x883C, true, true,  {}  }, // 387
  { "TRCEXTINSELR", 0x8844, true, true,  {}  }, // 388
  { "TRCCNTRLDVR0", 0x8805, true, true,  {}  }, // 389
  { "TRCCNTRLDVR1", 0x880D, true, true,  {}  }, // 390
  { "TRCCNTRLDVR2", 0x8815, true, true,  {}  }, // 391
  { "TRCCNTRLDVR3", 0x881D, true, true,  {}  }, // 392
  { "TRCCNTCTLR0", 0x8825, true, true,  {}  }, // 393
  { "TRCCNTCTLR1", 0x882D, true, true,  {}  }, // 394
  { "TRCCNTCTLR2", 0x8835, true, true,  {}  }, // 395
  { "TRCCNTCTLR3", 0x883D, true, true,  {}  }, // 396
  { "TRCCNTVR0", 0x8845, true, true,  {}  }, // 397
  { "TRCCNTVR1", 0x884D, true, true,  {}  }, // 398
  { "TRCCNTVR2", 0x8855, true, true,  {}  }, // 399
  { "TRCCNTVR3", 0x885D, true, true,  {}  }, // 400
  { "TRCIMSPEC0", 0x8807, true, true,  {}  }, // 401
  { "TRCIMSPEC1", 0x880F, true, true,  {}  }, // 402
  { "TRCIMSPEC2", 0x8817, true, true,  {}  }, // 403
  { "TRCIMSPEC3", 0x881F, true, true,  {}  }, // 404
  { "TRCIMSPEC4", 0x8827, true, true,  {}  }, // 405
  { "TRCIMSPEC5", 0x882F, true, true,  {}  }, // 406
  { "TRCIMSPEC6", 0x8837, true, true,  {}  }, // 407
  { "TRCIMSPEC7", 0x883F, true, true,  {}  }, // 408
  { "TRCRSCTLR2", 0x8890, true, true,  {}  }, // 409
  { "TRCRSCTLR3", 0x8898, true, true,  {}  }, // 410
  { "TRCRSCTLR4", 0x88A0, true, true,  {}  }, // 411
  { "TRCRSCTLR5", 0x88A8, true, true,  {}  }, // 412
  { "TRCRSCTLR6", 0x88B0, true, true,  {}  }, // 413
  { "TRCRSCTLR7", 0x88B8, true, true,  {}  }, // 414
  { "TRCRSCTLR8", 0x88C0, true, true,  {}  }, // 415
  { "TRCRSCTLR9", 0x88C8, true, true,  {}  }, // 416
  { "TRCRSCTLR10", 0x88D0, true, true,  {}  }, // 417
  { "TRCRSCTLR11", 0x88D8, true, true,  {}  }, // 418
  { "TRCRSCTLR12", 0x88E0, true, true,  {}  }, // 419
  { "TRCRSCTLR13", 0x88E8, true, true,  {}  }, // 420
  { "TRCRSCTLR14", 0x88F0, true, true,  {}  }, // 421
  { "TRCRSCTLR15", 0x88F8, true, true,  {}  }, // 422
  { "TRCRSCTLR16", 0x8881, true, true,  {}  }, // 423
  { "TRCRSCTLR17", 0x8889, true, true,  {}  }, // 424
  { "TRCRSCTLR18", 0x8891, true, true,  {}  }, // 425
  { "TRCRSCTLR19", 0x8899, true, true,  {}  }, // 426
  { "TRCRSCTLR20", 0x88A1, true, true,  {}  }, // 427
  { "TRCRSCTLR21", 0x88A9, true, true,  {}  }, // 428
  { "TRCRSCTLR22", 0x88B1, true, true,  {}  }, // 429
  { "TRCRSCTLR23", 0x88B9, true, true,  {}  }, // 430
  { "TRCRSCTLR24", 0x88C1, true, true,  {}  }, // 431
  { "TRCRSCTLR25", 0x88C9, true, true,  {}  }, // 432
  { "TRCRSCTLR26", 0x88D1, true, true,  {}  }, // 433
  { "TRCRSCTLR27", 0x88D9, true, true,  {}  }, // 434
  { "TRCRSCTLR28", 0x88E1, true, true,  {}  }, // 435
  { "TRCRSCTLR29", 0x88E9, true, true,  {}  }, // 436
  { "TRCRSCTLR30", 0x88F1, true, true,  {}  }, // 437
  { "TRCRSCTLR31", 0x88F9, true, true,  {}  }, // 438
  { "TRCSSCCR0", 0x8882, true, true,  {}  }, // 439
  { "TRCSSCCR1", 0x888A, true, true,  {}  }, // 440
  { "TRCSSCCR2", 0x8892, true, true,  {}  }, // 441
  { "TRCSSCCR3", 0x889A, true, true,  {}  }, // 442
  { "TRCSSCCR4", 0x88A2, true, true,  {}  }, // 443
  { "TRCSSCCR5", 0x88AA, true, true,  {}  }, // 444
  { "TRCSSCCR6", 0x88B2, true, true,  {}  }, // 445
  { "TRCSSCCR7", 0x88BA, true, true,  {}  }, // 446
  { "TRCSSCSR0", 0x88C2, true, true,  {}  }, // 447
  { "TRCSSCSR1", 0x88CA, true, true,  {}  }, // 448
  { "TRCSSCSR2", 0x88D2, true, true,  {}  }, // 449
  { "TRCSSCSR3", 0x88DA, true, true,  {}  }, // 450
  { "TRCSSCSR4", 0x88E2, true, true,  {}  }, // 451
  { "TRCSSCSR5", 0x88EA, true, true,  {}  }, // 452
  { "TRCSSCSR6", 0x88F2, true, true,  {}  }, // 453
  { "TRCSSCSR7", 0x88FA, true, true,  {}  }, // 454
  { "TRCSSPCICR0", 0x8883, true, true,  {}  }, // 455
  { "TRCSSPCICR1", 0x888B, true, true,  {}  }, // 456
  { "TRCSSPCICR2", 0x8893, true, true,  {}  }, // 457
  { "TRCSSPCICR3", 0x889B, true, true,  {}  }, // 458
  { "TRCSSPCICR4", 0x88A3, true, true,  {}  }, // 459
  { "TRCSSPCICR5", 0x88AB, true, true,  {}  }, // 460
  { "TRCSSPCICR6", 0x88B3, true, true,  {}  }, // 461
  { "TRCSSPCICR7", 0x88BB, true, true,  {}  }, // 462
  { "TRCPDCR", 0x88A4, true, true,  {}  }, // 463
  { "TRCACVR0", 0x8900, true, true,  {}  }, // 464
  { "TRCACVR1", 0x8910, true, true,  {}  }, // 465
  { "TRCACVR2", 0x8920, true, true,  {}  }, // 466
  { "TRCACVR3", 0x8930, true, true,  {}  }, // 467
  { "TRCACVR4", 0x8940, true, true,  {}  }, // 468
  { "TRCACVR5", 0x8950, true, true,  {}  }, // 469
  { "TRCACVR6", 0x8960, true, true,  {}  }, // 470
  { "TRCACVR7", 0x8970, true, true,  {}  }, // 471
  { "TRCACVR8", 0x8901, true, true,  {}  }, // 472
  { "TRCACVR9", 0x8911, true, true,  {}  }, // 473
  { "TRCACVR10", 0x8921, true, true,  {}  }, // 474
  { "TRCACVR11", 0x8931, true, true,  {}  }, // 475
  { "TRCACVR12", 0x8941, true, true,  {}  }, // 476
  { "TRCACVR13", 0x8951, true, true,  {}  }, // 477
  { "TRCACVR14", 0x8961, true, true,  {}  }, // 478
  { "TRCACVR15", 0x8971, true, true,  {}  }, // 479
  { "TRCACATR0", 0x8902, true, true,  {}  }, // 480
  { "TRCACATR1", 0x8912, true, true,  {}  }, // 481
  { "TRCACATR2", 0x8922, true, true,  {}  }, // 482
  { "TRCACATR3", 0x8932, true, true,  {}  }, // 483
  { "TRCACATR4", 0x8942, true, true,  {}  }, // 484
  { "TRCACATR5", 0x8952, true, true,  {}  }, // 485
  { "TRCACATR6", 0x8962, true, true,  {}  }, // 486
  { "TRCACATR7", 0x8972, true, true,  {}  }, // 487
  { "TRCACATR8", 0x8903, true, true,  {}  }, // 488
  { "TRCACATR9", 0x8913, true, true,  {}  }, // 489
  { "TRCACATR10", 0x8923, true, true,  {}  }, // 490
  { "TRCACATR11", 0x8933, true, true,  {}  }, // 491
  { "TRCACATR12", 0x8943, true, true,  {}  }, // 492
  { "TRCACATR13", 0x8953, true, true,  {}  }, // 493
  { "TRCACATR14", 0x8963, true, true,  {}  }, // 494
  { "TRCACATR15", 0x8973, true, true,  {}  }, // 495
  { "TRCDVCVR0", 0x8904, true, true,  {}  }, // 496
  { "TRCDVCVR1", 0x8924, true, true,  {}  }, // 497
  { "TRCDVCVR2", 0x8944, true, true,  {}  }, // 498
  { "TRCDVCVR3", 0x8964, true, true,  {}  }, // 499
  { "TRCDVCVR4", 0x8905, true, true,  {}  }, // 500
  { "TRCDVCVR5", 0x8925, true, true,  {}  }, // 501
  { "TRCDVCVR6", 0x8945, true, true,  {}  }, // 502
  { "TRCDVCVR7", 0x8965, true, true,  {}  }, // 503
  { "TRCDVCMR0", 0x8906, true, true,  {}  }, // 504
  { "TRCDVCMR1", 0x8926, true, true,  {}  }, // 505
  { "TRCDVCMR2", 0x8946, true, true,  {}  }, // 506
  { "TRCDVCMR3", 0x8966, true, true,  {}  }, // 507
  { "TRCDVCMR4", 0x8907, true, true,  {}  }, // 508
  { "TRCDVCMR5", 0x8927, true, true,  {}  }, // 509
  { "TRCDVCMR6", 0x8947, true, true,  {}  }, // 510
  { "TRCDVCMR7", 0x8967, true, true,  {}  }, // 511
  { "TRCCIDCVR0", 0x8980, true, true,  {}  }, // 512
  { "TRCCIDCVR1", 0x8990, true, true,  {}  }, // 513
  { "TRCCIDCVR2", 0x89A0, true, true,  {}  }, // 514
  { "TRCCIDCVR3", 0x89B0, true, true,  {}  }, // 515
  { "TRCCIDCVR4", 0x89C0, true, true,  {}  }, // 516
  { "TRCCIDCVR5", 0x89D0, true, true,  {}  }, // 517
  { "TRCCIDCVR6", 0x89E0, true, true,  {}  }, // 518
  { "TRCCIDCVR7", 0x89F0, true, true,  {}  }, // 519
  { "TRCVMIDCVR0", 0x8981, true, true,  {}  }, // 520
  { "TRCVMIDCVR1", 0x8991, true, true,  {}  }, // 521
  { "TRCVMIDCVR2", 0x89A1, true, true,  {}  }, // 522
  { "TRCVMIDCVR3", 0x89B1, true, true,  {}  }, // 523
  { "TRCVMIDCVR4", 0x89C1, true, true,  {}  }, // 524
  { "TRCVMIDCVR5", 0x89D1, true, true,  {}  }, // 525
  { "TRCVMIDCVR6", 0x89E1, true, true,  {}  }, // 526
  { "TRCVMIDCVR7", 0x89F1, true, true,  {}  }, // 527
  { "TRCCIDCCTLR0", 0x8982, true, true,  {}  }, // 528
  { "TRCCIDCCTLR1", 0x898A, true, true,  {}  }, // 529
  { "TRCVMIDCCTLR0", 0x8992, true, true,  {}  }, // 530
  { "TRCVMIDCCTLR1", 0x899A, true, true,  {}  }, // 531
  { "TRCITCTRL", 0x8B84, true, true,  {}  }, // 532
  { "TRCCLAIMSET", 0x8BC6, true, true,  {}  }, // 533
  { "TRCCLAIMCLR", 0x8BCE, true, true,  {}  }, // 534
  { "ICC_BPR1_EL1", 0xC663, true, true,  {}  }, // 535
  { "ICC_BPR0_EL1", 0xC643, true, true,  {}  }, // 536
  { "ICC_PMR_EL1", 0xC230, true, true,  {}  }, // 537
  { "ICC_CTLR_EL1", 0xC664, true, true,  {}  }, // 538
  { "ICC_CTLR_EL3", 0xF664, true, true,  {}  }, // 539
  { "ICC_SRE_EL1", 0xC665, true, true,  {}  }, // 540
  { "ICC_SRE_EL2", 0xE64D, true, true,  {}  }, // 541
  { "ICC_SRE_EL3", 0xF665, true, true,  {}  }, // 542
  { "ICC_IGRPEN0_EL1", 0xC666, true, true,  {}  }, // 543
  { "ICC_IGRPEN1_EL1", 0xC667, true, true,  {}  }, // 544
  { "ICC_IGRPEN1_EL3", 0xF667, true, true,  {}  }, // 545
  { "ICC_SEIEN_EL1", 0xC668, true, true,  {}  }, // 546
  { "ICC_AP0R0_EL1", 0xC644, true, true,  {}  }, // 547
  { "ICC_AP0R1_EL1", 0xC645, true, true,  {}  }, // 548
  { "ICC_AP0R2_EL1", 0xC646, true, true,  {}  }, // 549
  { "ICC_AP0R3_EL1", 0xC647, true, true,  {}  }, // 550
  { "ICC_AP1R0_EL1", 0xC648, true, true,  {}  }, // 551
  { "ICC_AP1R1_EL1", 0xC649, true, true,  {}  }, // 552
  { "ICC_AP1R2_EL1", 0xC64A, true, true,  {}  }, // 553
  { "ICC_AP1R3_EL1", 0xC64B, true, true,  {}  }, // 554
  { "ICH_AP0R0_EL2", 0xE640, true, true,  {}  }, // 555
  { "ICH_AP0R1_EL2", 0xE641, true, true,  {}  }, // 556
  { "ICH_AP0R2_EL2", 0xE642, true, true,  {}  }, // 557
  { "ICH_AP0R3_EL2", 0xE643, true, true,  {}  }, // 558
  { "ICH_AP1R0_EL2", 0xE648, true, true,  {}  }, // 559
  { "ICH_AP1R1_EL2", 0xE649, true, true,  {}  }, // 560
  { "ICH_AP1R2_EL2", 0xE64A, true, true,  {}  }, // 561
  { "ICH_AP1R3_EL2", 0xE64B, true, true,  {}  }, // 562
  { "ICH_HCR_EL2", 0xE658, true, true,  {}  }, // 563
  { "ICH_MISR_EL2", 0xE65A, true, true,  {}  }, // 564
  { "ICH_VMCR_EL2", 0xE65F, true, true,  {}  }, // 565
  { "ICH_VSEIR_EL2", 0xE64C, true, true,  {}  }, // 566
  { "ICH_LR0_EL2", 0xE660, true, true,  {}  }, // 567
  { "ICH_LR1_EL2", 0xE661, true, true,  {}  }, // 568
  { "ICH_LR2_EL2", 0xE662, true, true,  {}  }, // 569
  { "ICH_LR3_EL2", 0xE663, true, true,  {}  }, // 570
  { "ICH_LR4_EL2", 0xE664, true, true,  {}  }, // 571
  { "ICH_LR5_EL2", 0xE665, true, true,  {}  }, // 572
  { "ICH_LR6_EL2", 0xE666, true, true,  {}  }, // 573
  { "ICH_LR7_EL2", 0xE667, true, true,  {}  }, // 574
  { "ICH_LR8_EL2", 0xE668, true, true,  {}  }, // 575
  { "ICH_LR9_EL2", 0xE669, true, true,  {}  }, // 576
  { "ICH_LR10_EL2", 0xE66A, true, true,  {}  }, // 577
  { "ICH_LR11_EL2", 0xE66B, true, true,  {}  }, // 578
  { "ICH_LR12_EL2", 0xE66C, true, true,  {}  }, // 579
  { "ICH_LR13_EL2", 0xE66D, true, true,  {}  }, // 580
  { "ICH_LR14_EL2", 0xE66E, true, true,  {}  }, // 581
  { "ICH_LR15_EL2", 0xE66F, true, true,  {}  }, // 582
  { "PAN", 0xC213, true, true,  {AArch64::HasV8_1aOps}  }, // 583
  { "LORSA_EL1", 0xC520, true, true,  {AArch64::HasV8_1aOps}  }, // 584
  { "LOREA_EL1", 0xC521, true, true,  {AArch64::HasV8_1aOps}  }, // 585
  { "LORN_EL1", 0xC522, true, true,  {AArch64::HasV8_1aOps}  }, // 586
  { "LORC_EL1", 0xC523, true, true,  {AArch64::HasV8_1aOps}  }, // 587
  { "TTBR1_EL2", 0xE101, true, true,  {AArch64::HasV8_1aOps}  }, // 588
  { "CONTEXTIDR_EL2", 0xE681, true, true,  {AArch64::HasV8_1aOps}  }, // 589
  { "CNTHV_TVAL_EL2", 0xE718, true, true,  {AArch64::HasV8_1aOps}  }, // 590
  { "CNTHV_CVAL_EL2", 0xE71A, true, true,  {AArch64::HasV8_1aOps}  }, // 591
  { "CNTHV_CTL_EL2", 0xE719, true, true,  {AArch64::HasV8_1aOps}  }, // 592
  { "SCTLR_EL12", 0xE880, true, true,  {AArch64::HasV8_1aOps}  }, // 593
  { "CPACR_EL12", 0xE882, true, true,  {AArch64::HasV8_1aOps}  }, // 594
  { "TTBR0_EL12", 0xE900, true, true,  {AArch64::HasV8_1aOps}  }, // 595
  { "TTBR1_EL12", 0xE901, true, true,  {AArch64::HasV8_1aOps}  }, // 596
  { "TCR_EL12", 0xE902, true, true,  {AArch64::HasV8_1aOps}  }, // 597
  { "AFSR0_EL12", 0xEA88, true, true,  {AArch64::HasV8_1aOps}  }, // 598
  { "AFSR1_EL12", 0xEA89, true, true,  {AArch64::HasV8_1aOps}  }, // 599
  { "ESR_EL12", 0xEA90, true, true,  {AArch64::HasV8_1aOps}  }, // 600
  { "FAR_EL12", 0xEB00, true, true,  {AArch64::HasV8_1aOps}  }, // 601
  { "MAIR_EL12", 0xED10, true, true,  {AArch64::HasV8_1aOps}  }, // 602
  { "AMAIR_EL12", 0xED18, true, true,  {AArch64::HasV8_1aOps}  }, // 603
  { "VBAR_EL12", 0xEE00, true, true,  {AArch64::HasV8_1aOps}  }, // 604
  { "CONTEXTIDR_EL12", 0xEE81, true, true,  {AArch64::HasV8_1aOps}  }, // 605
  { "CNTKCTL_EL12", 0xEF08, true, true,  {AArch64::HasV8_1aOps}  }, // 606
  { "CNTP_TVAL_EL02", 0xEF10, true, true,  {AArch64::HasV8_1aOps}  }, // 607
  { "CNTP_CTL_EL02", 0xEF11, true, true,  {AArch64::HasV8_1aOps}  }, // 608
  { "CNTP_CVAL_EL02", 0xEF12, true, true,  {AArch64::HasV8_1aOps}  }, // 609
  { "CNTV_TVAL_EL02", 0xEF18, true, true,  {AArch64::HasV8_1aOps}  }, // 610
  { "CNTV_CTL_EL02", 0xEF19, true, true,  {AArch64::HasV8_1aOps}  }, // 611
  { "CNTV_CVAL_EL02", 0xEF1A, true, true,  {AArch64::HasV8_1aOps}  }, // 612
  { "SPSR_EL12", 0xEA00, true, true,  {AArch64::HasV8_1aOps}  }, // 613
  { "ELR_EL12", 0xEA01, true, true,  {AArch64::HasV8_1aOps}  }, // 614
  { "UAO", 0xC214, true, true,  {AArch64::HasV8_2aOps}  }, // 615
  { "PMBLIMITR_EL1", 0xC4D0, true, true,  {AArch64::FeatureSPE}  }, // 616
  { "PMBPTR_EL1", 0xC4D1, true, true,  {AArch64::FeatureSPE}  }, // 617
  { "PMBSR_EL1", 0xC4D3, true, true,  {AArch64::FeatureSPE}  }, // 618
  { "PMBIDR_EL1", 0xC4D7, true, true,  {AArch64::FeatureSPE}  }, // 619
  { "PMSCR_EL2", 0xE4C8, true, true,  {AArch64::FeatureSPE}  }, // 620
  { "PMSCR_EL12", 0xECC8, true, true,  {AArch64::FeatureSPE}  }, // 621
  { "PMSCR_EL1", 0xC4C8, true, true,  {AArch64::FeatureSPE}  }, // 622
  { "PMSICR_EL1", 0xC4CA, true, true,  {AArch64::FeatureSPE}  }, // 623
  { "PMSIRR_EL1", 0xC4CB, true, true,  {AArch64::FeatureSPE}  }, // 624
  { "PMSFCR_EL1", 0xC4CC, true, true,  {AArch64::FeatureSPE}  }, // 625
  { "PMSEVFR_EL1", 0xC4CD, true, true,  {AArch64::FeatureSPE}  }, // 626
  { "PMSLATFR_EL1", 0xC4CE, true, true,  {AArch64::FeatureSPE}  }, // 627
  { "PMSIDR_EL1", 0xC4CF, true, true,  {AArch64::FeatureSPE}  }, // 628
  { "ERRSELR_EL1", 0xC299, true, true,  {AArch64::FeatureRAS}  }, // 629
  { "ERXCTLR_EL1", 0xC2A1, true, true,  {AArch64::FeatureRAS}  }, // 630
  { "ERXSTATUS_EL1", 0xC2A2, true, true,  {AArch64::FeatureRAS}  }, // 631
  { "ERXADDR_EL1", 0xC2A3, true, true,  {AArch64::FeatureRAS}  }, // 632
  { "ERXMISC0_EL1", 0xC2A8, true, true,  {AArch64::FeatureRAS}  }, // 633
  { "ERXMISC1_EL1", 0xC2A9, true, true,  {AArch64::FeatureRAS}  }, // 634
  { "DISR_EL1", 0xC609, true, true,  {AArch64::FeatureRAS}  }, // 635
  { "VDISR_EL2", 0xE609, true, true,  {AArch64::FeatureRAS}  }, // 636
  { "VSESR_EL2", 0xE293, true, true,  {AArch64::FeatureRAS}  }, // 637
  { "APIAKeyLo_EL1", 0xC108, true, true,  {AArch64::HasV8_3aOps}  }, // 638
  { "APIAKeyHi_EL1", 0xC109, true, true,  {AArch64::HasV8_3aOps}  }, // 639
  { "APIBKeyLo_EL1", 0xC10A, true, true,  {AArch64::HasV8_3aOps}  }, // 640
  { "APIBKeyHi_EL1", 0xC10B, true, true,  {AArch64::HasV8_3aOps}  }, // 641
  { "APDAKeyLo_EL1", 0xC110, true, true,  {AArch64::HasV8_3aOps}  }, // 642
  { "APDAKeyHi_EL1", 0xC111, true, true,  {AArch64::HasV8_3aOps}  }, // 643
  { "APDBKeyLo_EL1", 0xC112, true, true,  {AArch64::HasV8_3aOps}  }, // 644
  { "APDBKeyHi_EL1", 0xC113, true, true,  {AArch64::HasV8_3aOps}  }, // 645
  { "APGAKeyLo_EL1", 0xC118, true, true,  {AArch64::HasV8_3aOps}  }, // 646
  { "APGAKeyHi_EL1", 0xC119, true, true,  {AArch64::HasV8_3aOps}  }, // 647
  { "VSTCR_EL2", 0xE132, true, true,  {AArch64::HasV8_4aOps}  }, // 648
  { "VSTTBR_EL2", 0xE130, true, true,  {AArch64::HasV8_4aOps}  }, // 649
  { "CNTHVS_TVAL_EL2", 0xE720, true, true,  {AArch64::HasV8_4aOps}  }, // 650
  { "CNTHVS_CVAL_EL2", 0xE722, true, true,  {AArch64::HasV8_4aOps}  }, // 651
  { "CNTHVS_CTL_EL2", 0xE721, true, true,  {AArch64::HasV8_4aOps}  }, // 652
  { "CNTHPS_TVAL_EL2", 0xE728, true, true,  {AArch64::HasV8_4aOps}  }, // 653
  { "CNTHPS_CVAL_EL2", 0xE72A, true, true,  {AArch64::HasV8_4aOps}  }, // 654
  { "CNTHPS_CTL_EL2", 0xE729, true, true,  {AArch64::HasV8_4aOps}  }, // 655
  { "SDER32_EL2", 0xE099, true, true,  {AArch64::HasV8_4aOps}  }, // 656
  { "ERXPFGCTL_EL1", 0xC2A5, true, true,  {AArch64::HasV8_4aOps}  }, // 657
  { "ERXPFGCDN_EL1", 0xC2A6, true, true,  {AArch64::HasV8_4aOps}  }, // 658
  { "ERXTS_EL1", 0xC2AF, true, true,  {AArch64::HasV8_4aOps}  }, // 659
  { "ERXMISC2_EL1", 0xC2AA, true, true,  {AArch64::HasV8_4aOps}  }, // 660
  { "ERXMISC3_EL1", 0xC2AB, true, true,  {AArch64::HasV8_4aOps}  }, // 661
  { "ERXPFGF_EL1", 0xC2A4, true, false,  {AArch64::HasV8_4aOps}  }, // 662
  { "MPAM0_EL1", 0xC529, true, true,  {AArch64::HasV8_4aOps}  }, // 663
  { "MPAM1_EL1", 0xC528, true, true,  {AArch64::HasV8_4aOps}  }, // 664
  { "MPAM2_EL2", 0xE528, true, true,  {AArch64::HasV8_4aOps}  }, // 665
  { "MPAM3_EL3", 0xF528, true, true,  {AArch64::HasV8_4aOps}  }, // 666
  { "MPAM1_EL12", 0xED28, true, true,  {AArch64::HasV8_4aOps}  }, // 667
  { "MPAMHCR_EL2", 0xE520, true, true,  {AArch64::HasV8_4aOps}  }, // 668
  { "MPAMVPMV_EL2", 0xE521, true, true,  {AArch64::HasV8_4aOps}  }, // 669
  { "MPAMVPM0_EL2", 0xE530, true, true,  {AArch64::HasV8_4aOps}  }, // 670
  { "MPAMVPM1_EL2", 0xE531, true, true,  {AArch64::HasV8_4aOps}  }, // 671
  { "MPAMVPM2_EL2", 0xE532, true, true,  {AArch64::HasV8_4aOps}  }, // 672
  { "MPAMVPM3_EL2", 0xE533, true, true,  {AArch64::HasV8_4aOps}  }, // 673
  { "MPAMVPM4_EL2", 0xE534, true, true,  {AArch64::HasV8_4aOps}  }, // 674
  { "MPAMVPM5_EL2", 0xE535, true, true,  {AArch64::HasV8_4aOps}  }, // 675
  { "MPAMVPM6_EL2", 0xE536, true, true,  {AArch64::HasV8_4aOps}  }, // 676
  { "MPAMVPM7_EL2", 0xE537, true, true,  {AArch64::HasV8_4aOps}  }, // 677
  { "MPAMIDR_EL1", 0xC524, true, false,  {AArch64::HasV8_4aOps}  }, // 678
  { "AMCR_EL0", 0xDE90, true, true,  {AArch64::HasV8_4aOps}  }, // 679
  { "AMCFGR_EL0", 0xDE91, true, false,  {AArch64::HasV8_4aOps}  }, // 680
  { "AMCGCR_EL0", 0xDE92, true, false,  {AArch64::HasV8_4aOps}  }, // 681
  { "AMUSERENR_EL0", 0xDE93, true, true,  {AArch64::HasV8_4aOps}  }, // 682
  { "AMCNTENCLR0_EL0", 0xDE94, true, true,  {AArch64::HasV8_4aOps}  }, // 683
  { "AMCNTENSET0_EL0", 0xDE95, true, true,  {AArch64::HasV8_4aOps}  }, // 684
  { "AMEVCNTR00_EL0", 0xDEA0, true, true,  {AArch64::HasV8_4aOps}  }, // 685
  { "AMEVCNTR01_EL0", 0xDEA1, true, true,  {AArch64::HasV8_4aOps}  }, // 686
  { "AMEVCNTR02_EL0", 0xDEA2, true, true,  {AArch64::HasV8_4aOps}  }, // 687
  { "AMEVCNTR03_EL0", 0xDEA3, true, true,  {AArch64::HasV8_4aOps}  }, // 688
  { "AMEVTYPER00_EL0", 0xDEB0, true, false,  {AArch64::HasV8_4aOps}  }, // 689
  { "AMEVTYPER01_EL0", 0xDEB1, true, false,  {AArch64::HasV8_4aOps}  }, // 690
  { "AMEVTYPER02_EL0", 0xDEB2, true, false,  {AArch64::HasV8_4aOps}  }, // 691
  { "AMEVTYPER03_EL0", 0xDEB3, true, false,  {AArch64::HasV8_4aOps}  }, // 692
  { "AMCNTENCLR1_EL0", 0xDE98, true, true,  {AArch64::HasV8_4aOps}  }, // 693
  { "AMCNTENSET1_EL0", 0xDE99, true, true,  {AArch64::HasV8_4aOps}  }, // 694
  { "AMEVCNTR10_EL0", 0xDEE0, true, true,  {AArch64::HasV8_4aOps}  }, // 695
  { "AMEVCNTR11_EL0", 0xDEE1, true, true,  {AArch64::HasV8_4aOps}  }, // 696
  { "AMEVCNTR12_EL0", 0xDEE2, true, true,  {AArch64::HasV8_4aOps}  }, // 697
  { "AMEVCNTR13_EL0", 0xDEE3, true, true,  {AArch64::HasV8_4aOps}  }, // 698
  { "AMEVCNTR14_EL0", 0xDEE4, true, true,  {AArch64::HasV8_4aOps}  }, // 699
  { "AMEVCNTR15_EL0", 0xDEE5, true, true,  {AArch64::HasV8_4aOps}  }, // 700
  { "AMEVCNTR16_EL0", 0xDEE6, true, true,  {AArch64::HasV8_4aOps}  }, // 701
  { "AMEVCNTR17_EL0", 0xDEE7, true, true,  {AArch64::HasV8_4aOps}  }, // 702
  { "AMEVCNTR18_EL0", 0xDEE8, true, true,  {AArch64::HasV8_4aOps}  }, // 703
  { "AMEVCNTR19_EL0", 0xDEE9, true, true,  {AArch64::HasV8_4aOps}  }, // 704
  { "AMEVCNTR110_EL0", 0xDEEA, true, true,  {AArch64::HasV8_4aOps}  }, // 705
  { "AMEVCNTR111_EL0", 0xDEEB, true, true,  {AArch64::HasV8_4aOps}  }, // 706
  { "AMEVCNTR112_EL0", 0xDEEC, true, true,  {AArch64::HasV8_4aOps}  }, // 707
  { "AMEVCNTR113_EL0", 0xDEED, true, true,  {AArch64::HasV8_4aOps}  }, // 708
  { "AMEVCNTR114_EL0", 0xDEEE, true, true,  {AArch64::HasV8_4aOps}  }, // 709
  { "AMEVCNTR115_EL0", 0xDEEF, true, true,  {AArch64::HasV8_4aOps}  }, // 710
  { "AMEVTYPER10_EL0", 0xDEF0, true, true,  {AArch64::HasV8_4aOps}  }, // 711
  { "AMEVTYPER11_EL0", 0xDEF1, true, true,  {AArch64::HasV8_4aOps}  }, // 712
  { "AMEVTYPER12_EL0", 0xDEF2, true, true,  {AArch64::HasV8_4aOps}  }, // 713
  { "AMEVTYPER13_EL0", 0xDEF3, true, true,  {AArch64::HasV8_4aOps}  }, // 714
  { "AMEVTYPER14_EL0", 0xDEF4, true, true,  {AArch64::HasV8_4aOps}  }, // 715
  { "AMEVTYPER15_EL0", 0xDEF5, true, true,  {AArch64::HasV8_4aOps}  }, // 716
  { "AMEVTYPER16_EL0", 0xDEF6, true, true,  {AArch64::HasV8_4aOps}  }, // 717
  { "AMEVTYPER17_EL0", 0xDEF7, true, true,  {AArch64::HasV8_4aOps}  }, // 718
  { "AMEVTYPER18_EL0", 0xDEF8, true, true,  {AArch64::HasV8_4aOps}  }, // 719
  { "AMEVTYPER19_EL0", 0xDEF9, true, true,  {AArch64::HasV8_4aOps}  }, // 720
  { "AMEVTYPER110_EL0", 0xDEFA, true, true,  {AArch64::HasV8_4aOps}  }, // 721
  { "AMEVTYPER111_EL0", 0xDEFB, true, true,  {AArch64::HasV8_4aOps}  }, // 722
  { "AMEVTYPER112_EL0", 0xDEFC, true, true,  {AArch64::HasV8_4aOps}  }, // 723
  { "AMEVTYPER113_EL0", 0xDEFD, true, true,  {AArch64::HasV8_4aOps}  }, // 724
  { "AMEVTYPER114_EL0", 0xDEFE, true, true,  {AArch64::HasV8_4aOps}  }, // 725
  { "AMEVTYPER115_EL0", 0xDEFF, true, true,  {AArch64::HasV8_4aOps}  }, // 726
  { "TRFCR_EL1", 0xC091, true, true,  {AArch64::HasV8_4aOps}  }, // 727
  { "TRFCR_EL2", 0xE091, true, true,  {AArch64::HasV8_4aOps}  }, // 728
  { "TRFCR_EL12", 0xE891, true, true,  {AArch64::HasV8_4aOps}  }, // 729
  { "DIT", 0xDA15, true, true,  {AArch64::HasV8_4aOps}  }, // 730
  { "VNCR_EL2", 0xE110, true, true,  {AArch64::HasV8_4aOps}  }, // 731
  { "ZCR_EL1", 0xC090, true, true,  {AArch64::FeatureSVE}  }, // 732
  { "ZCR_EL2", 0xE090, true, true,  {AArch64::FeatureSVE}  }, // 733
  { "ZCR_EL3", 0xF090, true, true,  {AArch64::FeatureSVE}  }, // 734
  { "ZCR_EL12", 0xE890, true, true,  {AArch64::FeatureSVE}  }, // 735
  { "CPM_IOACC_CTL_EL3", 0xFF90, true, true,  {AArch64::ProcCyclone}  }, // 736
 };

const SysReg *lookupSysRegByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "ACTLR_EL1", 195 },
    { "ACTLR_EL2", 196 },
    { "ACTLR_EL3", 197 },
    { "AFSR0_EL1", 239 },
    { "AFSR0_EL12", 598 },
    { "AFSR0_EL2", 240 },
    { "AFSR0_EL3", 241 },
    { "AFSR1_EL1", 242 },
    { "AFSR1_EL12", 599 },
    { "AFSR1_EL2", 243 },
    { "AFSR1_EL3", 244 },
    { "AIDR_EL1", 14 },
    { "AMAIR_EL1", 269 },
    { "AMAIR_EL12", 603 },
    { "AMAIR_EL2", 270 },
    { "AMAIR_EL3", 271 },
    { "AMCFGR_EL0", 680 },
    { "AMCGCR_EL0", 681 },
    { "AMCNTENCLR0_EL0", 683 },
    { "AMCNTENCLR1_EL0", 693 },
    { "AMCNTENSET0_EL0", 684 },
    { "AMCNTENSET1_EL0", 694 },
    { "AMCR_EL0", 679 },
    { "AMEVCNTR00_EL0", 685 },
    { "AMEVCNTR01_EL0", 686 },
    { "AMEVCNTR02_EL0", 687 },
    { "AMEVCNTR03_EL0", 688 },
    { "AMEVCNTR10_EL0", 695 },
    { "AMEVCNTR110_EL0", 705 },
    { "AMEVCNTR111_EL0", 706 },
    { "AMEVCNTR112_EL0", 707 },
    { "AMEVCNTR113_EL0", 708 },
    { "AMEVCNTR114_EL0", 709 },
    { "AMEVCNTR115_EL0", 710 },
    { "AMEVCNTR11_EL0", 696 },
    { "AMEVCNTR12_EL0", 697 },
    { "AMEVCNTR13_EL0", 698 },
    { "AMEVCNTR14_EL0", 699 },
    { "AMEVCNTR15_EL0", 700 },
    { "AMEVCNTR16_EL0", 701 },
    { "AMEVCNTR17_EL0", 702 },
    { "AMEVCNTR18_EL0", 703 },
    { "AMEVCNTR19_EL0", 704 },
    { "AMEVTYPER00_EL0", 689 },
    { "AMEVTYPER01_EL0", 690 },
    { "AMEVTYPER02_EL0", 691 },
    { "AMEVTYPER03_EL0", 692 },
    { "AMEVTYPER10_EL0", 711 },
    { "AMEVTYPER110_EL0", 721 },
    { "AMEVTYPER111_EL0", 722 },
    { "AMEVTYPER112_EL0", 723 },
    { "AMEVTYPER113_EL0", 724 },
    { "AMEVTYPER114_EL0", 725 },
    { "AMEVTYPER115_EL0", 726 },
    { "AMEVTYPER11_EL0", 712 },
    { "AMEVTYPER12_EL0", 713 },
    { "AMEVTYPER13_EL0", 714 },
    { "AMEVTYPER14_EL0", 715 },
    { "AMEVTYPER15_EL0", 716 },
    { "AMEVTYPER16_EL0", 717 },
    { "AMEVTYPER17_EL0", 718 },
    { "AMEVTYPER18_EL0", 719 },
    { "AMEVTYPER19_EL0", 720 },
    { "AMUSERENR_EL0", 682 },
    { "APDAKEYHI_EL1", 643 },
    { "APDAKEYLO_EL1", 642 },
    { "APDBKEYHI_EL1", 645 },
    { "APDBKEYLO_EL1", 644 },
    { "APGAKEYHI_EL1", 647 },
    { "APGAKEYLO_EL1", 646 },
    { "APIAKEYHI_EL1", 639 },
    { "APIAKEYLO_EL1", 638 },
    { "APIBKEYHI_EL1", 641 },
    { "APIBKEYLO_EL1", 640 },
    { "CCSIDR2_EL1", 9 },
    { "CCSIDR_EL1", 8 },
    { "CLIDR_EL1", 10 },
    { "CNTFRQ_EL0", 284 },
    { "CNTHCTL_EL2", 287 },
    { "CNTHPS_CTL_EL2", 655 },
    { "CNTHPS_CVAL_EL2", 654 },
    { "CNTHPS_TVAL_EL2", 653 },
    { "CNTHP_CTL_EL2", 292 },
    { "CNTHP_CVAL_EL2", 295 },
    { "CNTHP_TVAL_EL2", 289 },
    { "CNTHVS_CTL_EL2", 652 },
    { "CNTHVS_CVAL_EL2", 651 },
    { "CNTHVS_TVAL_EL2", 650 },
    { "CNTHV_CTL_EL2", 592 },
    { "CNTHV_CVAL_EL2", 591 },
    { "CNTHV_TVAL_EL2", 590 },
    { "CNTKCTL_EL1", 286 },
    { "CNTKCTL_EL12", 606 },
    { "CNTPCT_EL0", 49 },
    { "CNTPS_CTL_EL1", 293 },
    { "CNTPS_CVAL_EL1", 296 },
    { "CNTPS_TVAL_EL1", 290 },
    { "CNTP_CTL_EL0", 291 },
    { "CNTP_CTL_EL02", 608 },
    { "CNTP_CVAL_EL0", 294 },
    { "CNTP_CVAL_EL02", 609 },
    { "CNTP_TVAL_EL0", 288 },
    { "CNTP_TVAL_EL02", 607 },
    { "CNTVCT_EL0", 50 },
    { "CNTVOFF_EL2", 285 },
    { "CNTV_CTL_EL0", 298 },
    { "CNTV_CTL_EL02", 611 },
    { "CNTV_CVAL_EL0", 299 },
    { "CNTV_CVAL_EL02", 612 },
    { "CNTV_TVAL_EL0", 297 },
    { "CNTV_TVAL_EL02", 610 },
    { "CONTEXTIDR_EL1", 278 },
    { "CONTEXTIDR_EL12", 605 },
    { "CONTEXTIDR_EL2", 589 },
    { "CPACR_EL1", 191 },
    { "CPACR_EL12", 594 },
    { "CPM_IOACC_CTL_EL3", 736 },
    { "CPTR_EL2", 202 },
    { "CPTR_EL3", 203 },
    { "CSSELR_EL1", 188 },
    { "CTR_EL0", 11 },
    { "CURRENTEL", 229 },
    { "DACR32_EL2", 216 },
    { "DAIF", 228 },
    { "DBGAUTHSTATUS_EL1", 4 },
    { "DBGBCR0_EL1", 135 },
    { "DBGBCR10_EL1", 145 },
    { "DBGBCR11_EL1", 146 },
    { "DBGBCR12_EL1", 147 },
    { "DBGBCR13_EL1", 148 },
    { "DBGBCR14_EL1", 149 },
    { "DBGBCR15_EL1", 150 },
    { "DBGBCR1_EL1", 136 },
    { "DBGBCR2_EL1", 137 },
    { "DBGBCR3_EL1", 138 },
    { "DBGBCR4_EL1", 139 },
    { "DBGBCR5_EL1", 140 },
    { "DBGBCR6_EL1", 141 },
    { "DBGBCR7_EL1", 142 },
    { "DBGBCR8_EL1", 143 },
    { "DBGBCR9_EL1", 144 },
    { "DBGBVR0_EL1", 119 },
    { "DBGBVR10_EL1", 129 },
    { "DBGBVR11_EL1", 130 },
    { "DBGBVR12_EL1", 131 },
    { "DBGBVR13_EL1", 132 },
    { "DBGBVR14_EL1", 133 },
    { "DBGBVR15_EL1", 134 },
    { "DBGBVR1_EL1", 120 },
    { "DBGBVR2_EL1", 121 },
    { "DBGBVR3_EL1", 122 },
    { "DBGBVR4_EL1", 123 },
    { "DBGBVR5_EL1", 124 },
    { "DBGBVR6_EL1", 125 },
    { "DBGBVR7_EL1", 126 },
    { "DBGBVR8_EL1", 127 },
    { "DBGBVR9_EL1", 128 },
    { "DBGCLAIMCLR_EL1", 187 },
    { "DBGCLAIMSET_EL1", 186 },
    { "DBGDTRRX_EL0", 1 },
    { "DBGDTRTX_EL0", 100 },
    { "DBGDTR_EL0", 116 },
    { "DBGPRCR_EL1", 185 },
    { "DBGVCR32_EL2", 118 },
    { "DBGWCR0_EL1", 167 },
    { "DBGWCR10_EL1", 177 },
    { "DBGWCR11_EL1", 178 },
    { "DBGWCR12_EL1", 179 },
    { "DBGWCR13_EL1", 180 },
    { "DBGWCR14_EL1", 181 },
    { "DBGWCR15_EL1", 182 },
    { "DBGWCR1_EL1", 168 },
    { "DBGWCR2_EL1", 169 },
    { "DBGWCR3_EL1", 170 },
    { "DBGWCR4_EL1", 171 },
    { "DBGWCR5_EL1", 172 },
    { "DBGWCR6_EL1", 173 },
    { "DBGWCR7_EL1", 174 },
    { "DBGWCR8_EL1", 175 },
    { "DBGWCR9_EL1", 176 },
    { "DBGWVR0_EL1", 151 },
    { "DBGWVR10_EL1", 161 },
    { "DBGWVR11_EL1", 162 },
    { "DBGWVR12_EL1", 163 },
    { "DBGWVR13_EL1", 164 },
    { "DBGWVR14_EL1", 165 },
    { "DBGWVR15_EL1", 166 },
    { "DBGWVR1_EL1", 152 },
    { "DBGWVR2_EL1", 153 },
    { "DBGWVR3_EL1", 154 },
    { "DBGWVR4_EL1", 155 },
    { "DBGWVR5_EL1", 156 },
    { "DBGWVR6_EL1", 157 },
    { "DBGWVR7_EL1", 158 },
    { "DBGWVR8_EL1", 159 },
    { "DBGWVR9_EL1", 160 },
    { "DCZID_EL0", 15 },
    { "DISR_EL1", 635 },
    { "DIT", 730 },
    { "DLR_EL0", 237 },
    { "DSPSR_EL0", 236 },
    { "ELR_EL1", 220 },
    { "ELR_EL12", 614 },
    { "ELR_EL2", 221 },
    { "ELR_EL3", 222 },
    { "ERRIDR_EL1", 98 },
    { "ERRSELR_EL1", 629 },
    { "ERXADDR_EL1", 632 },
    { "ERXCTLR_EL1", 630 },
    { "ERXFR_EL1", 99 },
    { "ERXMISC0_EL1", 633 },
    { "ERXMISC1_EL1", 634 },
    { "ERXMISC2_EL1", 660 },
    { "ERXMISC3_EL1", 661 },
    { "ERXPFGCDN_EL1", 658 },
    { "ERXPFGCTL_EL1", 657 },
    { "ERXPFGF_EL1", 662 },
    { "ERXSTATUS_EL1", 631 },
    { "ERXTS_EL1", 659 },
    { "ESR_EL1", 245 },
    { "ESR_EL12", 600 },
    { "ESR_EL2", 246 },
    { "ESR_EL3", 247 },
    { "FAR_EL1", 249 },
    { "FAR_EL12", 601 },
    { "FAR_EL2", 250 },
    { "FAR_EL3", 251 },
    { "FPCR", 234 },
    { "FPEXC32_EL2", 248 },
    { "FPSR", 235 },
    { "HACR_EL2", 205 },
    { "HCR_EL2", 198 },
    { "HPFAR_EL2", 252 },
    { "HSTR_EL2", 204 },
    { "ICC_AP0R0_EL1", 547 },
    { "ICC_AP0R1_EL1", 548 },
    { "ICC_AP0R2_EL1", 549 },
    { "ICC_AP0R3_EL1", 550 },
    { "ICC_AP1R0_EL1", 551 },
    { "ICC_AP1R1_EL1", 552 },
    { "ICC_AP1R2_EL1", 553 },
    { "ICC_AP1R3_EL1", 554 },
    { "ICC_ASGI1R_EL1", 109 },
    { "ICC_BPR0_EL1", 536 },
    { "ICC_BPR1_EL1", 535 },
    { "ICC_CTLR_EL1", 538 },
    { "ICC_CTLR_EL3", 539 },
    { "ICC_DIR_EL1", 107 },
    { "ICC_EOIR0_EL1", 106 },
    { "ICC_EOIR1_EL1", 105 },
    { "ICC_HPPIR0_EL1", 91 },
    { "ICC_HPPIR1_EL1", 90 },
    { "ICC_IAR0_EL1", 89 },
    { "ICC_IAR1_EL1", 88 },
    { "ICC_IGRPEN0_EL1", 543 },
    { "ICC_IGRPEN1_EL1", 544 },
    { "ICC_IGRPEN1_EL3", 545 },
    { "ICC_PMR_EL1", 537 },
    { "ICC_RPR_EL1", 92 },
    { "ICC_SEIEN_EL1", 546 },
    { "ICC_SGI0R_EL1", 110 },
    { "ICC_SGI1R_EL1", 108 },
    { "ICC_SRE_EL1", 540 },
    { "ICC_SRE_EL2", 541 },
    { "ICC_SRE_EL3", 542 },
    { "ICH_AP0R0_EL2", 555 },
    { "ICH_AP0R1_EL2", 556 },
    { "ICH_AP0R2_EL2", 557 },
    { "ICH_AP0R3_EL2", 558 },
    { "ICH_AP1R0_EL2", 559 },
    { "ICH_AP1R1_EL2", 560 },
    { "ICH_AP1R2_EL2", 561 },
    { "ICH_AP1R3_EL2", 562 },
    { "ICH_EISR_EL2", 94 },
    { "ICH_ELRSR_EL2", 95 },
    { "ICH_HCR_EL2", 563 },
    { "ICH_LR0_EL2", 567 },
    { "ICH_LR10_EL2", 577 },
    { "ICH_LR11_EL2", 578 },
    { "ICH_LR12_EL2", 579 },
    { "ICH_LR13_EL2", 580 },
    { "ICH_LR14_EL2", 581 },
    { "ICH_LR15_EL2", 582 },
    { "ICH_LR1_EL2", 568 },
    { "ICH_LR2_EL2", 569 },
    { "ICH_LR3_EL2", 570 },
    { "ICH_LR4_EL2", 571 },
    { "ICH_LR5_EL2", 572 },
    { "ICH_LR6_EL2", 573 },
    { "ICH_LR7_EL2", 574 },
    { "ICH_LR8_EL2", 575 },
    { "ICH_LR9_EL2", 576 },
    { "ICH_MISR_EL2", 564 },
    { "ICH_VMCR_EL2", 565 },
    { "ICH_VSEIR_EL2", 566 },
    { "ICH_VTR_EL2", 93 },
    { "ID_AA64AFR0_EL1", 35 },
    { "ID_AA64AFR1_EL1", 36 },
    { "ID_AA64DFR0_EL1", 33 },
    { "ID_AA64DFR1_EL1", 34 },
    { "ID_AA64ISAR0_EL1", 37 },
    { "ID_AA64ISAR1_EL1", 38 },
    { "ID_AA64MMFR0_EL1", 39 },
    { "ID_AA64MMFR1_EL1", 40 },
    { "ID_AA64MMFR2_EL1", 41 },
    { "ID_AA64PFR0_EL1", 31 },
    { "ID_AA64PFR1_EL1", 32 },
    { "ID_AA64ZFR0_EL1", 96 },
    { "ID_AFR0_EL1", 19 },
    { "ID_DFR0_EL1", 18 },
    { "ID_ISAR0_EL1", 24 },
    { "ID_ISAR1_EL1", 25 },
    { "ID_ISAR2_EL1", 26 },
    { "ID_ISAR3_EL1", 27 },
    { "ID_ISAR4_EL1", 28 },
    { "ID_ISAR5_EL1", 29 },
    { "ID_ISAR6_EL1", 30 },
    { "ID_MMFR0_EL1", 20 },
    { "ID_MMFR1_EL1", 21 },
    { "ID_MMFR2_EL1", 22 },
    { "ID_MMFR3_EL1", 23 },
    { "ID_MMFR4_EL1", 51 },
    { "ID_PFR0_EL1", 16 },
    { "ID_PFR1_EL1", 17 },
    { "IFSR32_EL2", 238 },
    { "ISR_EL1", 48 },
    { "LORC_EL1", 587 },
    { "LOREA_EL1", 585 },
    { "LORID_EL1", 97 },
    { "LORN_EL1", 586 },
    { "LORSA_EL1", 584 },
    { "MAIR_EL1", 266 },
    { "MAIR_EL12", 602 },
    { "MAIR_EL2", 267 },
    { "MAIR_EL3", 268 },
    { "MDCCINT_EL1", 114 },
    { "MDCCSR_EL0", 0 },
    { "MDCR_EL2", 200 },
    { "MDCR_EL3", 206 },
    { "MDRAR_EL1", 2 },
    { "MDSCR_EL1", 115 },
    { "MIDR_EL1", 7 },
    { "MPAM0_EL1", 663 },
    { "MPAM1_EL1", 664 },
    { "MPAM1_EL12", 667 },
    { "MPAM2_EL2", 665 },
    { "MPAM3_EL3", 666 },
    { "MPAMHCR_EL2", 668 },
    { "MPAMIDR_EL1", 678 },
    { "MPAMVPM0_EL2", 670 },
    { "MPAMVPM1_EL2", 671 },
    { "MPAMVPM2_EL2", 672 },
    { "MPAMVPM3_EL2", 673 },
    { "MPAMVPM4_EL2", 674 },
    { "MPAMVPM5_EL2", 675 },
    { "MPAMVPM6_EL2", 676 },
    { "MPAMVPM7_EL2", 677 },
    { "MPAMVPMV_EL2", 669 },
    { "MPIDR_EL1", 12 },
    { "MVFR0_EL1", 42 },
    { "MVFR1_EL1", 43 },
    { "MVFR2_EL1", 44 },
    { "NZCV", 227 },
    { "OSDLR_EL1", 184 },
    { "OSDTRRX_EL1", 111 },
    { "OSDTRTX_EL1", 112 },
    { "OSECCR_EL1", 117 },
    { "OSLAR_EL1", 101 },
    { "OSLSR_EL1", 3 },
    { "PAN", 583 },
    { "PAR_EL1", 253 },
    { "PMBIDR_EL1", 619 },
    { "PMBLIMITR_EL1", 616 },
    { "PMBPTR_EL1", 617 },
    { "PMBSR_EL1", 618 },
    { "PMCCFILTR_EL0", 331 },
    { "PMCCNTR_EL0", 259 },
    { "PMCEID0_EL0", 5 },
    { "PMCEID1_EL0", 6 },
    { "PMCNTENCLR_EL0", 256 },
    { "PMCNTENSET_EL0", 255 },
    { "PMCR_EL0", 254 },
    { "PMEVCNTR0_EL0", 300 },
    { "PMEVCNTR10_EL0", 310 },
    { "PMEVCNTR11_EL0", 311 },
    { "PMEVCNTR12_EL0", 312 },
    { "PMEVCNTR13_EL0", 313 },
    { "PMEVCNTR14_EL0", 314 },
    { "PMEVCNTR15_EL0", 315 },
    { "PMEVCNTR16_EL0", 316 },
    { "PMEVCNTR17_EL0", 317 },
    { "PMEVCNTR18_EL0", 318 },
    { "PMEVCNTR19_EL0", 319 },
    { "PMEVCNTR1_EL0", 301 },
    { "PMEVCNTR20_EL0", 320 },
    { "PMEVCNTR21_EL0", 321 },
    { "PMEVCNTR22_EL0", 322 },
    { "PMEVCNTR23_EL0", 323 },
    { "PMEVCNTR24_EL0", 324 },
    { "PMEVCNTR25_EL0", 325 },
    { "PMEVCNTR26_EL0", 326 },
    { "PMEVCNTR27_EL0", 327 },
    { "PMEVCNTR28_EL0", 328 },
    { "PMEVCNTR29_EL0", 329 },
    { "PMEVCNTR2_EL0", 302 },
    { "PMEVCNTR30_EL0", 330 },
    { "PMEVCNTR3_EL0", 303 },
    { "PMEVCNTR4_EL0", 304 },
    { "PMEVCNTR5_EL0", 305 },
    { "PMEVCNTR6_EL0", 306 },
    { "PMEVCNTR7_EL0", 307 },
    { "PMEVCNTR8_EL0", 308 },
    { "PMEVCNTR9_EL0", 309 },
    { "PMEVTYPER0_EL0", 332 },
    { "PMEVTYPER10_EL0", 342 },
    { "PMEVTYPER11_EL0", 343 },
    { "PMEVTYPER12_EL0", 344 },
    { "PMEVTYPER13_EL0", 345 },
    { "PMEVTYPER14_EL0", 346 },
    { "PMEVTYPER15_EL0", 347 },
    { "PMEVTYPER16_EL0", 348 },
    { "PMEVTYPER17_EL0", 349 },
    { "PMEVTYPER18_EL0", 350 },
    { "PMEVTYPER19_EL0", 351 },
    { "PMEVTYPER1_EL0", 333 },
    { "PMEVTYPER20_EL0", 352 },
    { "PMEVTYPER21_EL0", 353 },
    { "PMEVTYPER22_EL0", 354 },
    { "PMEVTYPER23_EL0", 355 },
    { "PMEVTYPER24_EL0", 356 },
    { "PMEVTYPER25_EL0", 357 },
    { "PMEVTYPER26_EL0", 358 },
    { "PMEVTYPER27_EL0", 359 },
    { "PMEVTYPER28_EL0", 360 },
    { "PMEVTYPER29_EL0", 361 },
    { "PMEVTYPER2_EL0", 334 },
    { "PMEVTYPER30_EL0", 362 },
    { "PMEVTYPER3_EL0", 335 },
    { "PMEVTYPER4_EL0", 336 },
    { "PMEVTYPER5_EL0", 337 },
    { "PMEVTYPER6_EL0", 338 },
    { "PMEVTYPER7_EL0", 339 },
    { "PMEVTYPER8_EL0", 340 },
    { "PMEVTYPER9_EL0", 341 },
    { "PMINTENCLR_EL1", 264 },
    { "PMINTENSET_EL1", 263 },
    { "PMOVSCLR_EL0", 257 },
    { "PMOVSSET_EL0", 265 },
    { "PMSCR_EL1", 622 },
    { "PMSCR_EL12", 621 },
    { "PMSCR_EL2", 620 },
    { "PMSELR_EL0", 258 },
    { "PMSEVFR_EL1", 626 },
    { "PMSFCR_EL1", 625 },
    { "PMSICR_EL1", 623 },
    { "PMSIDR_EL1", 628 },
    { "PMSIRR_EL1", 624 },
    { "PMSLATFR_EL1", 627 },
    { "PMSWINC_EL0", 102 },
    { "PMUSERENR_EL0", 262 },
    { "PMXEVCNTR_EL0", 261 },
    { "PMXEVTYPER_EL0", 260 },
    { "REVIDR_EL1", 13 },
    { "RMR_EL1", 275 },
    { "RMR_EL2", 276 },
    { "RMR_EL3", 277 },
    { "RVBAR_EL1", 45 },
    { "RVBAR_EL2", 46 },
    { "RVBAR_EL3", 47 },
    { "SCR_EL3", 199 },
    { "SCTLR_EL1", 192 },
    { "SCTLR_EL12", 593 },
    { "SCTLR_EL2", 193 },
    { "SCTLR_EL3", 194 },
    { "SDER32_EL2", 656 },
    { "SDER32_EL3", 201 },
    { "SPSEL", 226 },
    { "SPSR_ABT", 231 },
    { "SPSR_EL1", 217 },
    { "SPSR_EL12", 613 },
    { "SPSR_EL2", 218 },
    { "SPSR_EL3", 219 },
    { "SPSR_FIQ", 233 },
    { "SPSR_IRQ", 230 },
    { "SPSR_UND", 232 },
    { "SP_EL0", 223 },
    { "SP_EL1", 224 },
    { "SP_EL2", 225 },
    { "TCR_EL1", 211 },
    { "TCR_EL12", 597 },
    { "TCR_EL2", 212 },
    { "TCR_EL3", 213 },
    { "TEECR32_EL1", 113 },
    { "TEEHBR32_EL1", 183 },
    { "TPIDRRO_EL0", 282 },
    { "TPIDR_EL0", 279 },
    { "TPIDR_EL1", 283 },
    { "TPIDR_EL2", 280 },
    { "TPIDR_EL3", 281 },
    { "TRCACATR0", 480 },
    { "TRCACATR1", 481 },
    { "TRCACATR10", 490 },
    { "TRCACATR11", 491 },
    { "TRCACATR12", 492 },
    { "TRCACATR13", 493 },
    { "TRCACATR14", 494 },
    { "TRCACATR15", 495 },
    { "TRCACATR2", 482 },
    { "TRCACATR3", 483 },
    { "TRCACATR4", 484 },
    { "TRCACATR5", 485 },
    { "TRCACATR6", 486 },
    { "TRCACATR7", 487 },
    { "TRCACATR8", 488 },
    { "TRCACATR9", 489 },
    { "TRCACVR0", 464 },
    { "TRCACVR1", 465 },
    { "TRCACVR10", 474 },
    { "TRCACVR11", 475 },
    { "TRCACVR12", 476 },
    { "TRCACVR13", 477 },
    { "TRCACVR14", 478 },
    { "TRCACVR15", 479 },
    { "TRCACVR2", 466 },
    { "TRCACVR3", 467 },
    { "TRCACVR4", 468 },
    { "TRCACVR5", 469 },
    { "TRCACVR6", 470 },
    { "TRCACVR7", 471 },
    { "TRCACVR8", 472 },
    { "TRCACVR9", 473 },
    { "TRCAUTHSTATUS", 72 },
    { "TRCAUXCTLR", 366 },
    { "TRCBBCTLR", 373 },
    { "TRCCCCTLR", 372 },
    { "TRCCIDCCTLR0", 528 },
    { "TRCCIDCCTLR1", 529 },
    { "TRCCIDCVR0", 512 },
    { "TRCCIDCVR1", 513 },
    { "TRCCIDCVR2", 514 },
    { "TRCCIDCVR3", 515 },
    { "TRCCIDCVR4", 516 },
    { "TRCCIDCVR5", 517 },
    { "TRCCIDCVR6", 518 },
    { "TRCCIDCVR7", 519 },
    { "TRCCIDR0", 84 },
    { "TRCCIDR1", 85 },
    { "TRCCIDR2", 86 },
    { "TRCCIDR3", 87 },
    { "TRCCLAIMCLR", 534 },
    { "TRCCLAIMSET", 533 },
    { "TRCCNTCTLR0", 393 },
    { "TRCCNTCTLR1", 394 },
    { "TRCCNTCTLR2", 395 },
    { "TRCCNTCTLR3", 396 },
    { "TRCCNTRLDVR0", 389 },
    { "TRCCNTRLDVR1", 390 },
    { "TRCCNTRLDVR2", 391 },
    { "TRCCNTRLDVR3", 392 },
    { "TRCCNTVR0", 397 },
    { "TRCCNTVR1", 398 },
    { "TRCCNTVR2", 399 },
    { "TRCCNTVR3", 400 },
    { "TRCCONFIGR", 365 },
    { "TRCDEVAFF0", 69 },
    { "TRCDEVAFF1", 70 },
    { "TRCDEVARCH", 73 },
    { "TRCDEVID", 74 },
    { "TRCDEVTYPE", 75 },
    { "TRCDVCMR0", 504 },
    { "TRCDVCMR1", 505 },
    { "TRCDVCMR2", 506 },
    { "TRCDVCMR3", 507 },
    { "TRCDVCMR4", 508 },
    { "TRCDVCMR5", 509 },
    { "TRCDVCMR6", 510 },
    { "TRCDVCMR7", 511 },
    { "TRCDVCVR0", 496 },
    { "TRCDVCVR1", 497 },
    { "TRCDVCVR2", 498 },
    { "TRCDVCVR3", 499 },
    { "TRCDVCVR4", 500 },
    { "TRCDVCVR5", 501 },
    { "TRCDVCVR6", 502 },
    { "TRCDVCVR7", 503 },
    { "TRCEVENTCTL0R", 367 },
    { "TRCEVENTCTL1R", 368 },
    { "TRCEXTINSELR", 388 },
    { "TRCIDR0", 59 },
    { "TRCIDR1", 60 },
    { "TRCIDR10", 55 },
    { "TRCIDR11", 56 },
    { "TRCIDR12", 57 },
    { "TRCIDR13", 58 },
    { "TRCIDR2", 61 },
    { "TRCIDR3", 62 },
    { "TRCIDR4", 63 },
    { "TRCIDR5", 64 },
    { "TRCIDR6", 65 },
    { "TRCIDR7", 66 },
    { "TRCIDR8", 53 },
    { "TRCIDR9", 54 },
    { "TRCIMSPEC0", 401 },
    { "TRCIMSPEC1", 402 },
    { "TRCIMSPEC2", 403 },
    { "TRCIMSPEC3", 404 },
    { "TRCIMSPEC4", 405 },
    { "TRCIMSPEC5", 406 },
    { "TRCIMSPEC6", 407 },
    { "TRCIMSPEC7", 408 },
    { "TRCITCTRL", 532 },
    { "TRCLAR", 104 },
    { "TRCLSR", 71 },
    { "TRCOSLAR", 103 },
    { "TRCOSLSR", 67 },
    { "TRCPDCR", 463 },
    { "TRCPDSR", 68 },
    { "TRCPIDR0", 80 },
    { "TRCPIDR1", 81 },
    { "TRCPIDR2", 82 },
    { "TRCPIDR3", 83 },
    { "TRCPIDR4", 76 },
    { "TRCPIDR5", 77 },
    { "TRCPIDR6", 78 },
    { "TRCPIDR7", 79 },
    { "TRCPRGCTLR", 363 },
    { "TRCPROCSELR", 364 },
    { "TRCQCTLR", 375 },
    { "TRCRSCTLR10", 417 },
    { "TRCRSCTLR11", 418 },
    { "TRCRSCTLR12", 419 },
    { "TRCRSCTLR13", 420 },
    { "TRCRSCTLR14", 421 },
    { "TRCRSCTLR15", 422 },
    { "TRCRSCTLR16", 423 },
    { "TRCRSCTLR17", 424 },
    { "TRCRSCTLR18", 425 },
    { "TRCRSCTLR19", 426 },
    { "TRCRSCTLR2", 409 },
    { "TRCRSCTLR20", 427 },
    { "TRCRSCTLR21", 428 },
    { "TRCRSCTLR22", 429 },
    { "TRCRSCTLR23", 430 },
    { "TRCRSCTLR24", 431 },
    { "TRCRSCTLR25", 432 },
    { "TRCRSCTLR26", 433 },
    { "TRCRSCTLR27", 434 },
    { "TRCRSCTLR28", 435 },
    { "TRCRSCTLR29", 436 },
    { "TRCRSCTLR3", 410 },
    { "TRCRSCTLR30", 437 },
    { "TRCRSCTLR31", 438 },
    { "TRCRSCTLR4", 411 },
    { "TRCRSCTLR5", 412 },
    { "TRCRSCTLR6", 413 },
    { "TRCRSCTLR7", 414 },
    { "TRCRSCTLR8", 415 },
    { "TRCRSCTLR9", 416 },
    { "TRCSEQEVR0", 383 },
    { "TRCSEQEVR1", 384 },
    { "TRCSEQEVR2", 385 },
    { "TRCSEQRSTEVR", 386 },
    { "TRCSEQSTR", 387 },
    { "TRCSSCCR0", 439 },
    { "TRCSSCCR1", 440 },
    { "TRCSSCCR2", 441 },
    { "TRCSSCCR3", 442 },
    { "TRCSSCCR4", 443 },
    { "TRCSSCCR5", 444 },
    { "TRCSSCCR6", 445 },
    { "TRCSSCCR7", 446 },
    { "TRCSSCSR0", 447 },
    { "TRCSSCSR1", 448 },
    { "TRCSSCSR2", 449 },
    { "TRCSSCSR3", 450 },
    { "TRCSSCSR4", 451 },
    { "TRCSSCSR5", 452 },
    { "TRCSSCSR6", 453 },
    { "TRCSSCSR7", 454 },
    { "TRCSSPCICR0", 455 },
    { "TRCSSPCICR1", 456 },
    { "TRCSSPCICR2", 457 },
    { "TRCSSPCICR3", 458 },
    { "TRCSSPCICR4", 459 },
    { "TRCSSPCICR5", 460 },
    { "TRCSSPCICR6", 461 },
    { "TRCSSPCICR7", 462 },
    { "TRCSTALLCTLR", 369 },
    { "TRCSTATR", 52 },
    { "TRCSYNCPR", 371 },
    { "TRCTRACEIDR", 374 },
    { "TRCTSCTLR", 370 },
    { "TRCVDARCCTLR", 382 },
    { "TRCVDCTLR", 380 },
    { "TRCVDSACCTLR", 381 },
    { "TRCVICTLR", 376 },
    { "TRCVIIECTLR", 377 },
    { "TRCVIPCSSCTLR", 379 },
    { "TRCVISSCTLR", 378 },
    { "TRCVMIDCCTLR0", 530 },
    { "TRCVMIDCCTLR1", 531 },
    { "TRCVMIDCVR0", 520 },
    { "TRCVMIDCVR1", 521 },
    { "TRCVMIDCVR2", 522 },
    { "TRCVMIDCVR3", 523 },
    { "TRCVMIDCVR4", 524 },
    { "TRCVMIDCVR5", 525 },
    { "TRCVMIDCVR6", 526 },
    { "TRCVMIDCVR7", 527 },
    { "TRFCR_EL1", 727 },
    { "TRFCR_EL12", 729 },
    { "TRFCR_EL2", 728 },
    { "TTBR0_EL1", 207 },
    { "TTBR0_EL12", 595 },
    { "TTBR0_EL2", 208 },
    { "TTBR0_EL3", 209 },
    { "TTBR1_EL1", 210 },
    { "TTBR1_EL12", 596 },
    { "TTBR1_EL2", 588 },
    { "UAO", 615 },
    { "VBAR_EL1", 272 },
    { "VBAR_EL12", 604 },
    { "VBAR_EL2", 273 },
    { "VBAR_EL3", 274 },
    { "VDISR_EL2", 636 },
    { "VMPIDR_EL2", 190 },
    { "VNCR_EL2", 731 },
    { "VPIDR_EL2", 189 },
    { "VSESR_EL2", 637 },
    { "VSTCR_EL2", 648 },
    { "VSTTBR_EL2", 649 },
    { "VTCR_EL2", 215 },
    { "VTTBR_EL2", 214 },
    { "ZCR_EL1", 732 },
    { "ZCR_EL12", 735 },
    { "ZCR_EL2", 733 },
    { "ZCR_EL3", 734 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &SysRegsList[Idx->_index];
}

const SysReg *lookupSysRegByEncoding(uint16_t Encoding) {
  struct IndexType {
    uint16_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x8002, 111 },
    { 0x8004, 119 },
    { 0x8005, 135 },
    { 0x8006, 151 },
    { 0x8007, 167 },
    { 0x800C, 120 },
    { 0x800D, 136 },
    { 0x800E, 152 },
    { 0x800F, 168 },
    { 0x8010, 114 },
    { 0x8012, 115 },
    { 0x8014, 121 },
    { 0x8015, 137 },
    { 0x8016, 153 },
    { 0x8017, 169 },
    { 0x801A, 112 },
    { 0x801C, 122 },
    { 0x801D, 138 },
    { 0x801E, 154 },
    { 0x801F, 170 },
    { 0x8024, 123 },
    { 0x8025, 139 },
    { 0x8026, 155 },
    { 0x8027, 171 },
    { 0x802C, 124 },
    { 0x802D, 140 },
    { 0x802E, 156 },
    { 0x802F, 172 },
    { 0x8032, 117 },
    { 0x8034, 125 },
    { 0x8035, 141 },
    { 0x8036, 157 },
    { 0x8037, 173 },
    { 0x803C, 126 },
    { 0x803D, 142 },
    { 0x803E, 158 },
    { 0x803F, 174 },
    { 0x8044, 127 },
    { 0x8045, 143 },
    { 0x8046, 159 },
    { 0x8047, 175 },
    { 0x804C, 128 },
    { 0x804D, 144 },
    { 0x804E, 160 },
    { 0x804F, 176 },
    { 0x8054, 129 },
    { 0x8055, 145 },
    { 0x8056, 161 },
    { 0x8057, 177 },
    { 0x805C, 130 },
    { 0x805D, 146 },
    { 0x805E, 162 },
    { 0x805F, 178 },
    { 0x8064, 131 },
    { 0x8065, 147 },
    { 0x8066, 163 },
    { 0x8067, 179 },
    { 0x806C, 132 },
    { 0x806D, 148 },
    { 0x806E, 164 },
    { 0x806F, 180 },
    { 0x8074, 133 },
    { 0x8075, 149 },
    { 0x8076, 165 },
    { 0x8077, 181 },
    { 0x807C, 134 },
    { 0x807D, 150 },
    { 0x807E, 166 },
    { 0x807F, 182 },
    { 0x8080, 2 },
    { 0x8084, 101 },
    { 0x808C, 3 },
    { 0x809C, 184 },
    { 0x80A4, 185 },
    { 0x83C6, 186 },
    { 0x83CE, 187 },
    { 0x83F6, 4 },
    { 0x8801, 374 },
    { 0x8802, 376 },
    { 0x8804, 383 },
    { 0x8805, 389 },
    { 0x8806, 53 },
    { 0x8807, 401 },
    { 0x8808, 363 },
    { 0x8809, 375 },
    { 0x880A, 377 },
    { 0x880C, 384 },
    { 0x880D, 390 },
    { 0x880E, 54 },
    { 0x880F, 402 },
    { 0x8810, 364 },
    { 0x8812, 378 },
    { 0x8814, 385 },
    { 0x8815, 391 },
    { 0x8816, 55 },
    { 0x8817, 403 },
    { 0x8818, 52 },
    { 0x881A, 379 },
    { 0x881D, 392 },
    { 0x881E, 56 },
    { 0x881F, 404 },
    { 0x8820, 365 },
    { 0x8825, 393 },
    { 0x8826, 57 },
    { 0x8827, 405 },
    { 0x882D, 394 },
    { 0x882E, 58 },
    { 0x882F, 406 },
    { 0x8830, 366 },
    { 0x8834, 386 },
    { 0x8835, 395 },
    { 0x8837, 407 },
    { 0x883C, 387 },
    { 0x883D, 396 },
    { 0x883F, 408 },
    { 0x8840, 367 },
    { 0x8842, 380 },
    { 0x8844, 388 },
    { 0x8845, 397 },
    { 0x8847, 59 },
    { 0x8848, 368 },
    { 0x884A, 381 },
    { 0x884D, 398 },
    { 0x884F, 60 },
    { 0x8852, 382 },
    { 0x8855, 399 },
    { 0x8857, 61 },
    { 0x8858, 369 },
    { 0x885D, 400 },
    { 0x885F, 62 },
    { 0x8860, 370 },
    { 0x8867, 63 },
    { 0x8868, 371 },
    { 0x886F, 64 },
    { 0x8870, 372 },
    { 0x8877, 65 },
    { 0x8878, 373 },
    { 0x887F, 66 },
    { 0x8881, 423 },
    { 0x8882, 439 },
    { 0x8883, 455 },
    { 0x8884, 103 },
    { 0x8889, 424 },
    { 0x888A, 440 },
    { 0x888B, 456 },
    { 0x888C, 67 },
    { 0x8890, 409 },
    { 0x8891, 425 },
    { 0x8892, 441 },
    { 0x8893, 457 },
    { 0x8898, 410 },
    { 0x8899, 426 },
    { 0x889A, 442 },
    { 0x889B, 458 },
    { 0x88A0, 411 },
    { 0x88A1, 427 },
    { 0x88A2, 443 },
    { 0x88A3, 459 },
    { 0x88A4, 463 },
    { 0x88A8, 412 },
    { 0x88A9, 428 },
    { 0x88AA, 444 },
    { 0x88AB, 460 },
    { 0x88AC, 68 },
    { 0x88B0, 413 },
    { 0x88B1, 429 },
    { 0x88B2, 445 },
    { 0x88B3, 461 },
    { 0x88B8, 414 },
    { 0x88B9, 430 },
    { 0x88BA, 446 },
    { 0x88BB, 462 },
    { 0x88C0, 415 },
    { 0x88C1, 431 },
    { 0x88C2, 447 },
    { 0x88C8, 416 },
    { 0x88C9, 432 },
    { 0x88CA, 448 },
    { 0x88D0, 417 },
    { 0x88D1, 433 },
    { 0x88D2, 449 },
    { 0x88D8, 418 },
    { 0x88D9, 434 },
    { 0x88DA, 450 },
    { 0x88E0, 419 },
    { 0x88E1, 435 },
    { 0x88E2, 451 },
    { 0x88E8, 420 },
    { 0x88E9, 436 },
    { 0x88EA, 452 },
    { 0x88F0, 421 },
    { 0x88F1, 437 },
    { 0x88F2, 453 },
    { 0x88F8, 422 },
    { 0x88F9, 438 },
    { 0x88FA, 454 },
    { 0x8900, 464 },
    { 0x8901, 472 },
    { 0x8902, 480 },
    { 0x8903, 488 },
    { 0x8904, 496 },
    { 0x8905, 500 },
    { 0x8906, 504 },
    { 0x8907, 508 },
    { 0x8910, 465 },
    { 0x8911, 473 },
    { 0x8912, 481 },
    { 0x8913, 489 },
    { 0x8920, 466 },
    { 0x8921, 474 },
    { 0x8922, 482 },
    { 0x8923, 490 },
    { 0x8924, 497 },
    { 0x8925, 501 },
    { 0x8926, 505 },
    { 0x8927, 509 },
    { 0x8930, 467 },
    { 0x8931, 475 },
    { 0x8932, 483 },
    { 0x8933, 491 },
    { 0x8940, 468 },
    { 0x8941, 476 },
    { 0x8942, 484 },
    { 0x8943, 492 },
    { 0x8944, 498 },
    { 0x8945, 502 },
    { 0x8946, 506 },
    { 0x8947, 510 },
    { 0x8950, 469 },
    { 0x8951, 477 },
    { 0x8952, 485 },
    { 0x8953, 493 },
    { 0x8960, 470 },
    { 0x8961, 478 },
    { 0x8962, 486 },
    { 0x8963, 494 },
    { 0x8964, 499 },
    { 0x8965, 503 },
    { 0x8966, 507 },
    { 0x8967, 511 },
    { 0x8970, 471 },
    { 0x8971, 479 },
    { 0x8972, 487 },
    { 0x8973, 495 },
    { 0x8980, 512 },
    { 0x8981, 520 },
    { 0x8982, 528 },
    { 0x898A, 529 },
    { 0x8990, 513 },
    { 0x8991, 521 },
    { 0x8992, 530 },
    { 0x899A, 531 },
    { 0x89A0, 514 },
    { 0x89A1, 522 },
    { 0x89B0, 515 },
    { 0x89B1, 523 },
    { 0x89C0, 516 },
    { 0x89C1, 524 },
    { 0x89D0, 517 },
    { 0x89D1, 525 },
    { 0x89E0, 518 },
    { 0x89E1, 526 },
    { 0x89F0, 519 },
    { 0x89F1, 527 },
    { 0x8B84, 532 },
    { 0x8B97, 74 },
    { 0x8B9F, 75 },
    { 0x8BA7, 76 },
    { 0x8BAF, 77 },
    { 0x8BB7, 78 },
    { 0x8BBF, 79 },
    { 0x8BC6, 533 },
    { 0x8BC7, 80 },
    { 0x8BCE, 534 },
    { 0x8BCF, 81 },
    { 0x8BD6, 69 },
    { 0x8BD7, 82 },
    { 0x8BDE, 70 },
    { 0x8BDF, 83 },
    { 0x8BE6, 104 },
    { 0x8BE7, 84 },
    { 0x8BEE, 71 },
    { 0x8BEF, 85 },
    { 0x8BF6, 72 },
    { 0x8BF7, 86 },
    { 0x8BFE, 73 },
    { 0x8BFF, 87 },
    { 0x9000, 113 },
    { 0x9080, 183 },
    { 0x9808, 0 },
    { 0x9820, 116 },
    { 0x9828, 1 },
    { 0x9828, 100 },
    { 0xA038, 118 },
    { 0xC000, 7 },
    { 0xC005, 12 },
    { 0xC006, 13 },
    { 0xC008, 16 },
    { 0xC009, 17 },
    { 0xC00A, 18 },
    { 0xC00B, 19 },
    { 0xC00C, 20 },
    { 0xC00D, 21 },
    { 0xC00E, 22 },
    { 0xC00F, 23 },
    { 0xC010, 24 },
    { 0xC011, 25 },
    { 0xC012, 26 },
    { 0xC013, 27 },
    { 0xC014, 28 },
    { 0xC015, 29 },
    { 0xC016, 51 },
    { 0xC017, 30 },
    { 0xC018, 42 },
    { 0xC019, 43 },
    { 0xC01A, 44 },
    { 0xC020, 31 },
    { 0xC021, 32 },
    { 0xC024, 96 },
    { 0xC028, 33 },
    { 0xC029, 34 },
    { 0xC02C, 35 },
    { 0xC02D, 36 },
    { 0xC030, 37 },
    { 0xC031, 38 },
    { 0xC038, 39 },
    { 0xC039, 40 },
    { 0xC03A, 41 },
    { 0xC080, 192 },
    { 0xC081, 195 },
    { 0xC082, 191 },
    { 0xC090, 732 },
    { 0xC091, 727 },
    { 0xC100, 207 },
    { 0xC101, 210 },
    { 0xC102, 211 },
    { 0xC108, 638 },
    { 0xC109, 639 },
    { 0xC10A, 640 },
    { 0xC10B, 641 },
    { 0xC110, 642 },
    { 0xC111, 643 },
    { 0xC112, 644 },
    { 0xC113, 645 },
    { 0xC118, 646 },
    { 0xC119, 647 },
    { 0xC200, 217 },
    { 0xC201, 220 },
    { 0xC208, 223 },
    { 0xC210, 226 },
    { 0xC212, 229 },
    { 0xC213, 583 },
    { 0xC214, 615 },
    { 0xC230, 537 },
    { 0xC288, 239 },
    { 0xC289, 242 },
    { 0xC290, 245 },
    { 0xC298, 98 },
    { 0xC299, 629 },
    { 0xC2A0, 99 },
    { 0xC2A1, 630 },
    { 0xC2A2, 631 },
    { 0xC2A3, 632 },
    { 0xC2A4, 662 },
    { 0xC2A5, 657 },
    { 0xC2A6, 658 },
    { 0xC2A8, 633 },
    { 0xC2A9, 634 },
    { 0xC2AA, 660 },
    { 0xC2AB, 661 },
    { 0xC2AF, 659 },
    { 0xC300, 249 },
    { 0xC3A0, 253 },
    { 0xC4C8, 622 },
    { 0xC4CA, 623 },
    { 0xC4CB, 624 },
    { 0xC4CC, 625 },
    { 0xC4CD, 626 },
    { 0xC4CE, 627 },
    { 0xC4CF, 628 },
    { 0xC4D0, 616 },
    { 0xC4D1, 617 },
    { 0xC4D3, 618 },
    { 0xC4D7, 619 },
    { 0xC4F1, 263 },
    { 0xC4F2, 264 },
    { 0xC510, 266 },
    { 0xC518, 269 },
    { 0xC520, 584 },
    { 0xC521, 585 },
    { 0xC522, 586 },
    { 0xC523, 587 },
    { 0xC524, 678 },
    { 0xC527, 97 },
    { 0xC528, 664 },
    { 0xC529, 663 },
    { 0xC600, 272 },
    { 0xC601, 45 },
    { 0xC602, 275 },
    { 0xC608, 48 },
    { 0xC609, 635 },
    { 0xC640, 89 },
    { 0xC641, 106 },
    { 0xC642, 91 },
    { 0xC643, 536 },
    { 0xC644, 547 },
    { 0xC645, 548 },
    { 0xC646, 549 },
    { 0xC647, 550 },
    { 0xC648, 551 },
    { 0xC649, 552 },
    { 0xC64A, 553 },
    { 0xC64B, 554 },
    { 0xC659, 107 },
    { 0xC65B, 92 },
    { 0xC65D, 108 },
    { 0xC65E, 109 },
    { 0xC65F, 110 },
    { 0xC660, 88 },
    { 0xC661, 105 },
    { 0xC662, 90 },
    { 0xC663, 535 },
    { 0xC664, 538 },
    { 0xC665, 540 },
    { 0xC666, 543 },
    { 0xC667, 544 },
    { 0xC668, 546 },
    { 0xC681, 278 },
    { 0xC684, 283 },
    { 0xC708, 286 },
    { 0xC800, 8 },
    { 0xC801, 10 },
    { 0xC802, 9 },
    { 0xC807, 14 },
    { 0xD000, 188 },
    { 0xD801, 11 },
    { 0xD807, 15 },
    { 0xDA10, 227 },
    { 0xDA11, 228 },
    { 0xDA15, 730 },
    { 0xDA20, 234 },
    { 0xDA21, 235 },
    { 0xDA28, 236 },
    { 0xDA29, 237 },
    { 0xDCE0, 254 },
    { 0xDCE1, 255 },
    { 0xDCE2, 256 },
    { 0xDCE3, 257 },
    { 0xDCE4, 102 },
    { 0xDCE5, 258 },
    { 0xDCE6, 5 },
    { 0xDCE7, 6 },
    { 0xDCE8, 259 },
    { 0xDCE9, 260 },
    { 0xDCEA, 261 },
    { 0xDCF0, 262 },
    { 0xDCF3, 265 },
    { 0xDE82, 279 },
    { 0xDE83, 282 },
    { 0xDE90, 679 },
    { 0xDE91, 680 },
    { 0xDE92, 681 },
    { 0xDE93, 682 },
    { 0xDE94, 683 },
    { 0xDE95, 684 },
    { 0xDE98, 693 },
    { 0xDE99, 694 },
    { 0xDEA0, 685 },
    { 0xDEA1, 686 },
    { 0xDEA2, 687 },
    { 0xDEA3, 688 },
    { 0xDEB0, 689 },
    { 0xDEB1, 690 },
    { 0xDEB2, 691 },
    { 0xDEB3, 692 },
    { 0xDEE0, 695 },
    { 0xDEE1, 696 },
    { 0xDEE2, 697 },
    { 0xDEE3, 698 },
    { 0xDEE4, 699 },
    { 0xDEE5, 700 },
    { 0xDEE6, 701 },
    { 0xDEE7, 702 },
    { 0xDEE8, 703 },
    { 0xDEE9, 704 },
    { 0xDEEA, 705 },
    { 0xDEEB, 706 },
    { 0xDEEC, 707 },
    { 0xDEED, 708 },
    { 0xDEEE, 709 },
    { 0xDEEF, 710 },
    { 0xDEF0, 711 },
    { 0xDEF1, 712 },
    { 0xDEF2, 713 },
    { 0xDEF3, 714 },
    { 0xDEF4, 715 },
    { 0xDEF5, 716 },
    { 0xDEF6, 717 },
    { 0xDEF7, 718 },
    { 0xDEF8, 719 },
    { 0xDEF9, 720 },
    { 0xDEFA, 721 },
    { 0xDEFB, 722 },
    { 0xDEFC, 723 },
    { 0xDEFD, 724 },
    { 0xDEFE, 725 },
    { 0xDEFF, 726 },
    { 0xDF00, 284 },
    { 0xDF01, 49 },
    { 0xDF02, 50 },
    { 0xDF10, 288 },
    { 0xDF11, 291 },
    { 0xDF12, 294 },
    { 0xDF18, 297 },
    { 0xDF19, 298 },
    { 0xDF1A, 299 },
    { 0xDF40, 300 },
    { 0xDF41, 301 },
    { 0xDF42, 302 },
    { 0xDF43, 303 },
    { 0xDF44, 304 },
    { 0xDF45, 305 },
    { 0xDF46, 306 },
    { 0xDF47, 307 },
    { 0xDF48, 308 },
    { 0xDF49, 309 },
    { 0xDF4A, 310 },
    { 0xDF4B, 311 },
    { 0xDF4C, 312 },
    { 0xDF4D, 313 },
    { 0xDF4E, 314 },
    { 0xDF4F, 315 },
    { 0xDF50, 316 },
    { 0xDF51, 317 },
    { 0xDF52, 318 },
    { 0xDF53, 319 },
    { 0xDF54, 320 },
    { 0xDF55, 321 },
    { 0xDF56, 322 },
    { 0xDF57, 323 },
    { 0xDF58, 324 },
    { 0xDF59, 325 },
    { 0xDF5A, 326 },
    { 0xDF5B, 327 },
    { 0xDF5C, 328 },
    { 0xDF5D, 329 },
    { 0xDF5E, 330 },
    { 0xDF60, 332 },
    { 0xDF61, 333 },
    { 0xDF62, 334 },
    { 0xDF63, 335 },
    { 0xDF64, 336 },
    { 0xDF65, 337 },
    { 0xDF66, 338 },
    { 0xDF67, 339 },
    { 0xDF68, 340 },
    { 0xDF69, 341 },
    { 0xDF6A, 342 },
    { 0xDF6B, 343 },
    { 0xDF6C, 344 },
    { 0xDF6D, 345 },
    { 0xDF6E, 346 },
    { 0xDF6F, 347 },
    { 0xDF70, 348 },
    { 0xDF71, 349 },
    { 0xDF72, 350 },
    { 0xDF73, 351 },
    { 0xDF74, 352 },
    { 0xDF75, 353 },
    { 0xDF76, 354 },
    { 0xDF77, 355 },
    { 0xDF78, 356 },
    { 0xDF79, 357 },
    { 0xDF7A, 358 },
    { 0xDF7B, 359 },
    { 0xDF7C, 360 },
    { 0xDF7D, 361 },
    { 0xDF7E, 362 },
    { 0xDF7F, 331 },
    { 0xE000, 189 },
    { 0xE005, 190 },
    { 0xE080, 193 },
    { 0xE081, 196 },
    { 0xE088, 198 },
    { 0xE089, 200 },
    { 0xE08A, 202 },
    { 0xE08B, 204 },
    { 0xE08F, 205 },
    { 0xE090, 733 },
    { 0xE091, 728 },
    { 0xE099, 656 },
    { 0xE100, 208 },
    { 0xE101, 588 },
    { 0xE102, 212 },
    { 0xE108, 214 },
    { 0xE10A, 215 },
    { 0xE110, 731 },
    { 0xE130, 649 },
    { 0xE132, 648 },
    { 0xE180, 216 },
    { 0xE200, 218 },
    { 0xE201, 221 },
    { 0xE208, 224 },
    { 0xE218, 230 },
    { 0xE219, 231 },
    { 0xE21A, 232 },
    { 0xE21B, 233 },
    { 0xE281, 238 },
    { 0xE288, 240 },
    { 0xE289, 243 },
    { 0xE290, 246 },
    { 0xE293, 637 },
    { 0xE298, 248 },
    { 0xE300, 250 },
    { 0xE304, 252 },
    { 0xE4C8, 620 },
    { 0xE510, 267 },
    { 0xE518, 270 },
    { 0xE520, 668 },
    { 0xE521, 669 },
    { 0xE528, 665 },
    { 0xE530, 670 },
    { 0xE531, 671 },
    { 0xE532, 672 },
    { 0xE533, 673 },
    { 0xE534, 674 },
    { 0xE535, 675 },
    { 0xE536, 676 },
    { 0xE537, 677 },
    { 0xE600, 273 },
    { 0xE601, 46 },
    { 0xE602, 276 },
    { 0xE609, 636 },
    { 0xE640, 555 },
    { 0xE641, 556 },
    { 0xE642, 557 },
    { 0xE643, 558 },
    { 0xE648, 559 },
    { 0xE649, 560 },
    { 0xE64A, 561 },
    { 0xE64B, 562 },
    { 0xE64C, 566 },
    { 0xE64D, 541 },
    { 0xE658, 563 },
    { 0xE659, 93 },
    { 0xE65A, 564 },
    { 0xE65B, 94 },
    { 0xE65D, 95 },
    { 0xE65F, 565 },
    { 0xE660, 567 },
    { 0xE661, 568 },
    { 0xE662, 569 },
    { 0xE663, 570 },
    { 0xE664, 571 },
    { 0xE665, 572 },
    { 0xE666, 573 },
    { 0xE667, 574 },
    { 0xE668, 575 },
    { 0xE669, 576 },
    { 0xE66A, 577 },
    { 0xE66B, 578 },
    { 0xE66C, 579 },
    { 0xE66D, 580 },
    { 0xE66E, 581 },
    { 0xE66F, 582 },
    { 0xE681, 589 },
    { 0xE682, 280 },
    { 0xE703, 285 },
    { 0xE708, 287 },
    { 0xE710, 289 },
    { 0xE711, 292 },
    { 0xE712, 295 },
    { 0xE718, 590 },
    { 0xE719, 592 },
    { 0xE71A, 591 },
    { 0xE720, 650 },
    { 0xE721, 652 },
    { 0xE722, 651 },
    { 0xE728, 653 },
    { 0xE729, 655 },
    { 0xE72A, 654 },
    { 0xE880, 593 },
    { 0xE882, 594 },
    { 0xE890, 735 },
    { 0xE891, 729 },
    { 0xE900, 595 },
    { 0xE901, 596 },
    { 0xE902, 597 },
    { 0xEA00, 613 },
    { 0xEA01, 614 },
    { 0xEA88, 598 },
    { 0xEA89, 599 },
    { 0xEA90, 600 },
    { 0xEB00, 601 },
    { 0xECC8, 621 },
    { 0xED10, 602 },
    { 0xED18, 603 },
    { 0xED28, 667 },
    { 0xEE00, 604 },
    { 0xEE81, 605 },
    { 0xEF08, 606 },
    { 0xEF10, 607 },
    { 0xEF11, 608 },
    { 0xEF12, 609 },
    { 0xEF18, 610 },
    { 0xEF19, 611 },
    { 0xEF1A, 612 },
    { 0xF080, 194 },
    { 0xF081, 197 },
    { 0xF088, 199 },
    { 0xF089, 201 },
    { 0xF08A, 203 },
    { 0xF090, 734 },
    { 0xF099, 206 },
    { 0xF100, 209 },
    { 0xF102, 213 },
    { 0xF200, 219 },
    { 0xF201, 222 },
    { 0xF208, 225 },
    { 0xF288, 241 },
    { 0xF289, 244 },
    { 0xF290, 247 },
    { 0xF300, 251 },
    { 0xF510, 268 },
    { 0xF518, 271 },
    { 0xF528, 666 },
    { 0xF600, 274 },
    { 0xF601, 47 },
    { 0xF602, 277 },
    { 0xF664, 539 },
    { 0xF665, 542 },
    { 0xF667, 545 },
    { 0xF682, 281 },
    { 0xFF10, 290 },
    { 0xFF11, 293 },
    { 0xFF12, 296 },
    { 0xFF90, 736 },
  };

  struct KeyType {
    uint16_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &SysRegsList[Idx->_index];
}
#endif

#ifdef GET_TLBI_DECL
const TLBI *lookupTLBIByName(StringRef Name);
const TLBI *lookupTLBIByEncoding(uint16_t Encoding);
#endif

#ifdef GET_TLBI_IMPL
const TLBI TLBIsList[] = {
  { "IPAS2E1IS", 0x2401, true,  {}  }, // 0
  { "IPAS2LE1IS", 0x2405, true,  {}  }, // 1
  { "VMALLE1IS", 0x418, false,  {}  }, // 2
  { "ALLE2IS", 0x2418, false,  {}  }, // 3
  { "ALLE3IS", 0x3418, false,  {}  }, // 4
  { "VAE1IS", 0x419, true,  {}  }, // 5
  { "VAE2IS", 0x2419, true,  {}  }, // 6
  { "VAE3IS", 0x3419, true,  {}  }, // 7
  { "ASIDE1IS", 0x41A, true,  {}  }, // 8
  { "VAAE1IS", 0x41B, true,  {}  }, // 9
  { "ALLE1IS", 0x241C, false,  {}  }, // 10
  { "VALE1IS", 0x41D, true,  {}  }, // 11
  { "VALE2IS", 0x241D, true,  {}  }, // 12
  { "VALE3IS", 0x341D, true,  {}  }, // 13
  { "VMALLS12E1IS", 0x241E, false,  {}  }, // 14
  { "VAALE1IS", 0x41F, true,  {}  }, // 15
  { "IPAS2E1", 0x2421, true,  {}  }, // 16
  { "IPAS2LE1", 0x2425, true,  {}  }, // 17
  { "VMALLE1", 0x438, false,  {}  }, // 18
  { "ALLE2", 0x2438, false,  {}  }, // 19
  { "ALLE3", 0x3438, false,  {}  }, // 20
  { "VAE1", 0x439, true,  {}  }, // 21
  { "VAE2", 0x2439, true,  {}  }, // 22
  { "VAE3", 0x3439, true,  {}  }, // 23
  { "ASIDE1", 0x43A, true,  {}  }, // 24
  { "VAAE1", 0x43B, true,  {}  }, // 25
  { "ALLE1", 0x243C, false,  {}  }, // 26
  { "VALE1", 0x43D, true,  {}  }, // 27
  { "VALE2", 0x243D, true,  {}  }, // 28
  { "VALE3", 0x343D, true,  {}  }, // 29
  { "VMALLS12E1", 0x243E, false,  {}  }, // 30
  { "VAALE1", 0x43F, true,  {}  }, // 31
  { "VMALLE1OS", 0x408, false,  {AArch64::HasV8_4aOps}  }, // 32
  { "VAE1OS", 0x409, true,  {AArch64::HasV8_4aOps}  }, // 33
  { "ASIDE1OS", 0x40A, true,  {AArch64::HasV8_4aOps}  }, // 34
  { "VAAE1OS", 0x40B, true,  {AArch64::HasV8_4aOps}  }, // 35
  { "VALE1OS", 0x40D, true,  {AArch64::HasV8_4aOps}  }, // 36
  { "VAALE1OS", 0x40F, true,  {AArch64::HasV8_4aOps}  }, // 37
  { "IPAS2E1OS", 0x2420, true,  {AArch64::HasV8_4aOps}  }, // 38
  { "IPAS2LE1OS", 0x2424, true,  {AArch64::HasV8_4aOps}  }, // 39
  { "VAE2OS", 0x2409, true,  {AArch64::HasV8_4aOps}  }, // 40
  { "VALE2OS", 0x240D, true,  {AArch64::HasV8_4aOps}  }, // 41
  { "VMALLS12E1OS", 0x240E, false,  {AArch64::HasV8_4aOps}  }, // 42
  { "VAE3OS", 0x3409, true,  {AArch64::HasV8_4aOps}  }, // 43
  { "VALE3OS", 0x340D, true,  {AArch64::HasV8_4aOps}  }, // 44
  { "ALLE2OS", 0x2408, false,  {AArch64::HasV8_4aOps}  }, // 45
  { "ALLE1OS", 0x240C, false,  {AArch64::HasV8_4aOps}  }, // 46
  { "ALLE3OS", 0x3408, false,  {AArch64::HasV8_4aOps}  }, // 47
  { "RVAE1", 0x431, true,  {AArch64::HasV8_4aOps}  }, // 48
  { "RVAAE1", 0x433, true,  {AArch64::HasV8_4aOps}  }, // 49
  { "RVALE1", 0x435, true,  {AArch64::HasV8_4aOps}  }, // 50
  { "RVAALE1", 0x437, true,  {AArch64::HasV8_4aOps}  }, // 51
  { "RVAE1IS", 0x411, true,  {AArch64::HasV8_4aOps}  }, // 52
  { "RVAAE1IS", 0x413, true,  {AArch64::HasV8_4aOps}  }, // 53
  { "RVALE1IS", 0x415, true,  {AArch64::HasV8_4aOps}  }, // 54
  { "RVAALE1IS", 0x417, true,  {AArch64::HasV8_4aOps}  }, // 55
  { "RVAE1OS", 0x429, true,  {AArch64::HasV8_4aOps}  }, // 56
  { "RVAAE1OS", 0x42B, true,  {AArch64::HasV8_4aOps}  }, // 57
  { "RVALE1OS", 0x42D, true,  {AArch64::HasV8_4aOps}  }, // 58
  { "RVAALE1OS", 0x42F, true,  {AArch64::HasV8_4aOps}  }, // 59
  { "RIPAS2E1IS", 0x2402, true,  {AArch64::HasV8_4aOps}  }, // 60
  { "RIPAS2LE1IS", 0x2406, true,  {AArch64::HasV8_4aOps}  }, // 61
  { "RIPAS2E1", 0x2422, true,  {AArch64::HasV8_4aOps}  }, // 62
  { "RIPAS2LE1", 0x2426, true,  {AArch64::HasV8_4aOps}  }, // 63
  { "RIPAS2E1OS", 0x2423, true,  {AArch64::HasV8_4aOps}  }, // 64
  { "RIPAS2LE1OS", 0x2427, true,  {AArch64::HasV8_4aOps}  }, // 65
  { "RVAE2", 0x2431, true,  {AArch64::HasV8_4aOps}  }, // 66
  { "RVALE2", 0x2435, true,  {AArch64::HasV8_4aOps}  }, // 67
  { "RVAE2IS", 0x2411, true,  {AArch64::HasV8_4aOps}  }, // 68
  { "RVALE2IS", 0x2415, true,  {AArch64::HasV8_4aOps}  }, // 69
  { "RVAE2OS", 0x2429, true,  {AArch64::HasV8_4aOps}  }, // 70
  { "RVALE2OS", 0x242D, true,  {AArch64::HasV8_4aOps}  }, // 71
  { "RVAE3", 0x3431, true,  {AArch64::HasV8_4aOps}  }, // 72
  { "RVALE3", 0x3435, true,  {AArch64::HasV8_4aOps}  }, // 73
  { "RVAE3IS", 0x3411, true,  {AArch64::HasV8_4aOps}  }, // 74
  { "RVALE3IS", 0x3415, true,  {AArch64::HasV8_4aOps}  }, // 75
  { "RVAE3OS", 0x3429, true,  {AArch64::HasV8_4aOps}  }, // 76
  { "RVALE3OS", 0x342D, true,  {AArch64::HasV8_4aOps}  }, // 77
 };

const TLBI *lookupTLBIByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "ALLE1", 26 },
    { "ALLE1IS", 10 },
    { "ALLE1OS", 46 },
    { "ALLE2", 19 },
    { "ALLE2IS", 3 },
    { "ALLE2OS", 45 },
    { "ALLE3", 20 },
    { "ALLE3IS", 4 },
    { "ALLE3OS", 47 },
    { "ASIDE1", 24 },
    { "ASIDE1IS", 8 },
    { "ASIDE1OS", 34 },
    { "IPAS2E1", 16 },
    { "IPAS2E1IS", 0 },
    { "IPAS2E1OS", 38 },
    { "IPAS2LE1", 17 },
    { "IPAS2LE1IS", 1 },
    { "IPAS2LE1OS", 39 },
    { "RIPAS2E1", 62 },
    { "RIPAS2E1IS", 60 },
    { "RIPAS2E1OS", 64 },
    { "RIPAS2LE1", 63 },
    { "RIPAS2LE1IS", 61 },
    { "RIPAS2LE1OS", 65 },
    { "RVAAE1", 49 },
    { "RVAAE1IS", 53 },
    { "RVAAE1OS", 57 },
    { "RVAALE1", 51 },
    { "RVAALE1IS", 55 },
    { "RVAALE1OS", 59 },
    { "RVAE1", 48 },
    { "RVAE1IS", 52 },
    { "RVAE1OS", 56 },
    { "RVAE2", 66 },
    { "RVAE2IS", 68 },
    { "RVAE2OS", 70 },
    { "RVAE3", 72 },
    { "RVAE3IS", 74 },
    { "RVAE3OS", 76 },
    { "RVALE1", 50 },
    { "RVALE1IS", 54 },
    { "RVALE1OS", 58 },
    { "RVALE2", 67 },
    { "RVALE2IS", 69 },
    { "RVALE2OS", 71 },
    { "RVALE3", 73 },
    { "RVALE3IS", 75 },
    { "RVALE3OS", 77 },
    { "VAAE1", 25 },
    { "VAAE1IS", 9 },
    { "VAAE1OS", 35 },
    { "VAALE1", 31 },
    { "VAALE1IS", 15 },
    { "VAALE1OS", 37 },
    { "VAE1", 21 },
    { "VAE1IS", 5 },
    { "VAE1OS", 33 },
    { "VAE2", 22 },
    { "VAE2IS", 6 },
    { "VAE2OS", 40 },
    { "VAE3", 23 },
    { "VAE3IS", 7 },
    { "VAE3OS", 43 },
    { "VALE1", 27 },
    { "VALE1IS", 11 },
    { "VALE1OS", 36 },
    { "VALE2", 28 },
    { "VALE2IS", 12 },
    { "VALE2OS", 41 },
    { "VALE3", 29 },
    { "VALE3IS", 13 },
    { "VALE3OS", 44 },
    { "VMALLE1", 18 },
    { "VMALLE1IS", 2 },
    { "VMALLE1OS", 32 },
    { "VMALLS12E1", 30 },
    { "VMALLS12E1IS", 14 },
    { "VMALLS12E1OS", 42 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &TLBIsList[Idx->_index];
}

const TLBI *lookupTLBIByEncoding(uint16_t Encoding) {
  struct IndexType {
    uint16_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x408, 32 },
    { 0x409, 33 },
    { 0x40A, 34 },
    { 0x40B, 35 },
    { 0x40D, 36 },
    { 0x40F, 37 },
    { 0x411, 52 },
    { 0x413, 53 },
    { 0x415, 54 },
    { 0x417, 55 },
    { 0x418, 2 },
    { 0x419, 5 },
    { 0x41A, 8 },
    { 0x41B, 9 },
    { 0x41D, 11 },
    { 0x41F, 15 },
    { 0x429, 56 },
    { 0x42B, 57 },
    { 0x42D, 58 },
    { 0x42F, 59 },
    { 0x431, 48 },
    { 0x433, 49 },
    { 0x435, 50 },
    { 0x437, 51 },
    { 0x438, 18 },
    { 0x439, 21 },
    { 0x43A, 24 },
    { 0x43B, 25 },
    { 0x43D, 27 },
    { 0x43F, 31 },
    { 0x2401, 0 },
    { 0x2402, 60 },
    { 0x2405, 1 },
    { 0x2406, 61 },
    { 0x2408, 45 },
    { 0x2409, 40 },
    { 0x240C, 46 },
    { 0x240D, 41 },
    { 0x240E, 42 },
    { 0x2411, 68 },
    { 0x2415, 69 },
    { 0x2418, 3 },
    { 0x2419, 6 },
    { 0x241C, 10 },
    { 0x241D, 12 },
    { 0x241E, 14 },
    { 0x2420, 38 },
    { 0x2421, 16 },
    { 0x2422, 62 },
    { 0x2423, 64 },
    { 0x2424, 39 },
    { 0x2425, 17 },
    { 0x2426, 63 },
    { 0x2427, 65 },
    { 0x2429, 70 },
    { 0x242D, 71 },
    { 0x2431, 66 },
    { 0x2435, 67 },
    { 0x2438, 19 },
    { 0x2439, 22 },
    { 0x243C, 26 },
    { 0x243D, 28 },
    { 0x243E, 30 },
    { 0x3408, 47 },
    { 0x3409, 43 },
    { 0x340D, 44 },
    { 0x3411, 74 },
    { 0x3415, 75 },
    { 0x3418, 4 },
    { 0x3419, 7 },
    { 0x341D, 13 },
    { 0x3429, 76 },
    { 0x342D, 77 },
    { 0x3431, 72 },
    { 0x3435, 73 },
    { 0x3438, 20 },
    { 0x3439, 23 },
    { 0x343D, 29 },
  };

  struct KeyType {
    uint16_t Encoding;
  };
  KeyType Key = { Encoding };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      if (LHS.Encoding < RHS.Encoding)
        return true;
      if (LHS.Encoding > RHS.Encoding)
        return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Encoding != Idx->Encoding)
    return nullptr;
  return &TLBIsList[Idx->_index];
}
#endif

#ifdef GET_TSB_DECL
const TSB *lookupTSBByName(StringRef Name);
const TSB *lookupTSBByEncoding(uint8_t Encoding);
#endif

#ifdef GET_TSB_IMPL
const TSB TSBsList[] = {
  { "csync", 0x0,  {AArch64::HasV8_4aOps}  }, // 0
 };

const TSB *lookupTSBByName(StringRef Name) {
  struct IndexType {
    const char * Name;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { "CSYNC", 0 },
  };

  struct KeyType {
    std::string Name;
  };
  KeyType Key = { Name.upper() };
  auto Table = makeArrayRef(Index);
  auto Idx = std::lower_bound(Table.begin(), Table.end(), Key,
    [](const IndexType &LHS, const KeyType &RHS) {
      int CmpName = StringRef(LHS.Name).compare(RHS.Name);
      if (CmpName < 0) return true;
      if (CmpName > 0) return false;
      return false;
    });

  if (Idx == Table.end() ||
      Key.Name != Idx->Name)
    return nullptr;
  return &TSBsList[Idx->_index];
}

const TSB *lookupTSBByEncoding(uint8_t Encoding) {
  struct IndexType {
    uint8_t Encoding;
    unsigned _index;
  };
  static const struct IndexType Index[] = {
    { 0x0, 0 },
  };

  auto Table = makeArrayRef(Index);
  size_t Idx = Encoding;
  return Idx >= Table.size() ? nullptr : &TSBsList[Table[Idx]._index];
}
#endif

#undef GET_AT_DECL
#undef GET_AT_IMPL
#undef GET_DB_DECL
#undef GET_DB_IMPL
#undef GET_DC_DECL
#undef GET_DC_IMPL
#undef GET_EXACTFPIMM_DECL
#undef GET_EXACTFPIMM_IMPL
#undef GET_IC_DECL
#undef GET_IC_IMPL
#undef GET_ISB_DECL
#undef GET_ISB_IMPL
#undef GET_PRFM_DECL
#undef GET_PRFM_IMPL
#undef GET_PSB_DECL
#undef GET_PSB_IMPL
#undef GET_PSTATE_DECL
#undef GET_PSTATE_IMPL
#undef GET_SVEPREDPAT_DECL
#undef GET_SVEPREDPAT_IMPL
#undef GET_SVEPRFM_DECL
#undef GET_SVEPRFM_IMPL
#undef GET_SYSREG_DECL
#undef GET_SYSREG_IMPL
#undef GET_TLBI_DECL
#undef GET_TLBI_IMPL
#undef GET_TSB_DECL
#undef GET_TSB_IMPL
