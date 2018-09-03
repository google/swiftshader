/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Target Register Enum Values                                                *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_REGINFO_ENUM
#undef GET_REGINFO_ENUM

namespace llvm {

class MCRegisterClass;
extern const MCRegisterClass AArch64MCRegisterClasses[];

namespace AArch64 {
enum {
  NoRegister,
  FFR = 1,
  FP = 2,
  LR = 3,
  NZCV = 4,
  SP = 5,
  WSP = 6,
  WZR = 7,
  XZR = 8,
  B0 = 9,
  B1 = 10,
  B2 = 11,
  B3 = 12,
  B4 = 13,
  B5 = 14,
  B6 = 15,
  B7 = 16,
  B8 = 17,
  B9 = 18,
  B10 = 19,
  B11 = 20,
  B12 = 21,
  B13 = 22,
  B14 = 23,
  B15 = 24,
  B16 = 25,
  B17 = 26,
  B18 = 27,
  B19 = 28,
  B20 = 29,
  B21 = 30,
  B22 = 31,
  B23 = 32,
  B24 = 33,
  B25 = 34,
  B26 = 35,
  B27 = 36,
  B28 = 37,
  B29 = 38,
  B30 = 39,
  B31 = 40,
  D0 = 41,
  D1 = 42,
  D2 = 43,
  D3 = 44,
  D4 = 45,
  D5 = 46,
  D6 = 47,
  D7 = 48,
  D8 = 49,
  D9 = 50,
  D10 = 51,
  D11 = 52,
  D12 = 53,
  D13 = 54,
  D14 = 55,
  D15 = 56,
  D16 = 57,
  D17 = 58,
  D18 = 59,
  D19 = 60,
  D20 = 61,
  D21 = 62,
  D22 = 63,
  D23 = 64,
  D24 = 65,
  D25 = 66,
  D26 = 67,
  D27 = 68,
  D28 = 69,
  D29 = 70,
  D30 = 71,
  D31 = 72,
  H0 = 73,
  H1 = 74,
  H2 = 75,
  H3 = 76,
  H4 = 77,
  H5 = 78,
  H6 = 79,
  H7 = 80,
  H8 = 81,
  H9 = 82,
  H10 = 83,
  H11 = 84,
  H12 = 85,
  H13 = 86,
  H14 = 87,
  H15 = 88,
  H16 = 89,
  H17 = 90,
  H18 = 91,
  H19 = 92,
  H20 = 93,
  H21 = 94,
  H22 = 95,
  H23 = 96,
  H24 = 97,
  H25 = 98,
  H26 = 99,
  H27 = 100,
  H28 = 101,
  H29 = 102,
  H30 = 103,
  H31 = 104,
  P0 = 105,
  P1 = 106,
  P2 = 107,
  P3 = 108,
  P4 = 109,
  P5 = 110,
  P6 = 111,
  P7 = 112,
  P8 = 113,
  P9 = 114,
  P10 = 115,
  P11 = 116,
  P12 = 117,
  P13 = 118,
  P14 = 119,
  P15 = 120,
  Q0 = 121,
  Q1 = 122,
  Q2 = 123,
  Q3 = 124,
  Q4 = 125,
  Q5 = 126,
  Q6 = 127,
  Q7 = 128,
  Q8 = 129,
  Q9 = 130,
  Q10 = 131,
  Q11 = 132,
  Q12 = 133,
  Q13 = 134,
  Q14 = 135,
  Q15 = 136,
  Q16 = 137,
  Q17 = 138,
  Q18 = 139,
  Q19 = 140,
  Q20 = 141,
  Q21 = 142,
  Q22 = 143,
  Q23 = 144,
  Q24 = 145,
  Q25 = 146,
  Q26 = 147,
  Q27 = 148,
  Q28 = 149,
  Q29 = 150,
  Q30 = 151,
  Q31 = 152,
  S0 = 153,
  S1 = 154,
  S2 = 155,
  S3 = 156,
  S4 = 157,
  S5 = 158,
  S6 = 159,
  S7 = 160,
  S8 = 161,
  S9 = 162,
  S10 = 163,
  S11 = 164,
  S12 = 165,
  S13 = 166,
  S14 = 167,
  S15 = 168,
  S16 = 169,
  S17 = 170,
  S18 = 171,
  S19 = 172,
  S20 = 173,
  S21 = 174,
  S22 = 175,
  S23 = 176,
  S24 = 177,
  S25 = 178,
  S26 = 179,
  S27 = 180,
  S28 = 181,
  S29 = 182,
  S30 = 183,
  S31 = 184,
  W0 = 185,
  W1 = 186,
  W2 = 187,
  W3 = 188,
  W4 = 189,
  W5 = 190,
  W6 = 191,
  W7 = 192,
  W8 = 193,
  W9 = 194,
  W10 = 195,
  W11 = 196,
  W12 = 197,
  W13 = 198,
  W14 = 199,
  W15 = 200,
  W16 = 201,
  W17 = 202,
  W18 = 203,
  W19 = 204,
  W20 = 205,
  W21 = 206,
  W22 = 207,
  W23 = 208,
  W24 = 209,
  W25 = 210,
  W26 = 211,
  W27 = 212,
  W28 = 213,
  W29 = 214,
  W30 = 215,
  X0 = 216,
  X1 = 217,
  X2 = 218,
  X3 = 219,
  X4 = 220,
  X5 = 221,
  X6 = 222,
  X7 = 223,
  X8 = 224,
  X9 = 225,
  X10 = 226,
  X11 = 227,
  X12 = 228,
  X13 = 229,
  X14 = 230,
  X15 = 231,
  X16 = 232,
  X17 = 233,
  X18 = 234,
  X19 = 235,
  X20 = 236,
  X21 = 237,
  X22 = 238,
  X23 = 239,
  X24 = 240,
  X25 = 241,
  X26 = 242,
  X27 = 243,
  X28 = 244,
  Z0 = 245,
  Z1 = 246,
  Z2 = 247,
  Z3 = 248,
  Z4 = 249,
  Z5 = 250,
  Z6 = 251,
  Z7 = 252,
  Z8 = 253,
  Z9 = 254,
  Z10 = 255,
  Z11 = 256,
  Z12 = 257,
  Z13 = 258,
  Z14 = 259,
  Z15 = 260,
  Z16 = 261,
  Z17 = 262,
  Z18 = 263,
  Z19 = 264,
  Z20 = 265,
  Z21 = 266,
  Z22 = 267,
  Z23 = 268,
  Z24 = 269,
  Z25 = 270,
  Z26 = 271,
  Z27 = 272,
  Z28 = 273,
  Z29 = 274,
  Z30 = 275,
  Z31 = 276,
  Z0_HI = 277,
  Z1_HI = 278,
  Z2_HI = 279,
  Z3_HI = 280,
  Z4_HI = 281,
  Z5_HI = 282,
  Z6_HI = 283,
  Z7_HI = 284,
  Z8_HI = 285,
  Z9_HI = 286,
  Z10_HI = 287,
  Z11_HI = 288,
  Z12_HI = 289,
  Z13_HI = 290,
  Z14_HI = 291,
  Z15_HI = 292,
  Z16_HI = 293,
  Z17_HI = 294,
  Z18_HI = 295,
  Z19_HI = 296,
  Z20_HI = 297,
  Z21_HI = 298,
  Z22_HI = 299,
  Z23_HI = 300,
  Z24_HI = 301,
  Z25_HI = 302,
  Z26_HI = 303,
  Z27_HI = 304,
  Z28_HI = 305,
  Z29_HI = 306,
  Z30_HI = 307,
  Z31_HI = 308,
  D0_D1 = 309,
  D1_D2 = 310,
  D2_D3 = 311,
  D3_D4 = 312,
  D4_D5 = 313,
  D5_D6 = 314,
  D6_D7 = 315,
  D7_D8 = 316,
  D8_D9 = 317,
  D9_D10 = 318,
  D10_D11 = 319,
  D11_D12 = 320,
  D12_D13 = 321,
  D13_D14 = 322,
  D14_D15 = 323,
  D15_D16 = 324,
  D16_D17 = 325,
  D17_D18 = 326,
  D18_D19 = 327,
  D19_D20 = 328,
  D20_D21 = 329,
  D21_D22 = 330,
  D22_D23 = 331,
  D23_D24 = 332,
  D24_D25 = 333,
  D25_D26 = 334,
  D26_D27 = 335,
  D27_D28 = 336,
  D28_D29 = 337,
  D29_D30 = 338,
  D30_D31 = 339,
  D31_D0 = 340,
  D0_D1_D2_D3 = 341,
  D1_D2_D3_D4 = 342,
  D2_D3_D4_D5 = 343,
  D3_D4_D5_D6 = 344,
  D4_D5_D6_D7 = 345,
  D5_D6_D7_D8 = 346,
  D6_D7_D8_D9 = 347,
  D7_D8_D9_D10 = 348,
  D8_D9_D10_D11 = 349,
  D9_D10_D11_D12 = 350,
  D10_D11_D12_D13 = 351,
  D11_D12_D13_D14 = 352,
  D12_D13_D14_D15 = 353,
  D13_D14_D15_D16 = 354,
  D14_D15_D16_D17 = 355,
  D15_D16_D17_D18 = 356,
  D16_D17_D18_D19 = 357,
  D17_D18_D19_D20 = 358,
  D18_D19_D20_D21 = 359,
  D19_D20_D21_D22 = 360,
  D20_D21_D22_D23 = 361,
  D21_D22_D23_D24 = 362,
  D22_D23_D24_D25 = 363,
  D23_D24_D25_D26 = 364,
  D24_D25_D26_D27 = 365,
  D25_D26_D27_D28 = 366,
  D26_D27_D28_D29 = 367,
  D27_D28_D29_D30 = 368,
  D28_D29_D30_D31 = 369,
  D29_D30_D31_D0 = 370,
  D30_D31_D0_D1 = 371,
  D31_D0_D1_D2 = 372,
  D0_D1_D2 = 373,
  D1_D2_D3 = 374,
  D2_D3_D4 = 375,
  D3_D4_D5 = 376,
  D4_D5_D6 = 377,
  D5_D6_D7 = 378,
  D6_D7_D8 = 379,
  D7_D8_D9 = 380,
  D8_D9_D10 = 381,
  D9_D10_D11 = 382,
  D10_D11_D12 = 383,
  D11_D12_D13 = 384,
  D12_D13_D14 = 385,
  D13_D14_D15 = 386,
  D14_D15_D16 = 387,
  D15_D16_D17 = 388,
  D16_D17_D18 = 389,
  D17_D18_D19 = 390,
  D18_D19_D20 = 391,
  D19_D20_D21 = 392,
  D20_D21_D22 = 393,
  D21_D22_D23 = 394,
  D22_D23_D24 = 395,
  D23_D24_D25 = 396,
  D24_D25_D26 = 397,
  D25_D26_D27 = 398,
  D26_D27_D28 = 399,
  D27_D28_D29 = 400,
  D28_D29_D30 = 401,
  D29_D30_D31 = 402,
  D30_D31_D0 = 403,
  D31_D0_D1 = 404,
  Q0_Q1 = 405,
  Q1_Q2 = 406,
  Q2_Q3 = 407,
  Q3_Q4 = 408,
  Q4_Q5 = 409,
  Q5_Q6 = 410,
  Q6_Q7 = 411,
  Q7_Q8 = 412,
  Q8_Q9 = 413,
  Q9_Q10 = 414,
  Q10_Q11 = 415,
  Q11_Q12 = 416,
  Q12_Q13 = 417,
  Q13_Q14 = 418,
  Q14_Q15 = 419,
  Q15_Q16 = 420,
  Q16_Q17 = 421,
  Q17_Q18 = 422,
  Q18_Q19 = 423,
  Q19_Q20 = 424,
  Q20_Q21 = 425,
  Q21_Q22 = 426,
  Q22_Q23 = 427,
  Q23_Q24 = 428,
  Q24_Q25 = 429,
  Q25_Q26 = 430,
  Q26_Q27 = 431,
  Q27_Q28 = 432,
  Q28_Q29 = 433,
  Q29_Q30 = 434,
  Q30_Q31 = 435,
  Q31_Q0 = 436,
  Q0_Q1_Q2_Q3 = 437,
  Q1_Q2_Q3_Q4 = 438,
  Q2_Q3_Q4_Q5 = 439,
  Q3_Q4_Q5_Q6 = 440,
  Q4_Q5_Q6_Q7 = 441,
  Q5_Q6_Q7_Q8 = 442,
  Q6_Q7_Q8_Q9 = 443,
  Q7_Q8_Q9_Q10 = 444,
  Q8_Q9_Q10_Q11 = 445,
  Q9_Q10_Q11_Q12 = 446,
  Q10_Q11_Q12_Q13 = 447,
  Q11_Q12_Q13_Q14 = 448,
  Q12_Q13_Q14_Q15 = 449,
  Q13_Q14_Q15_Q16 = 450,
  Q14_Q15_Q16_Q17 = 451,
  Q15_Q16_Q17_Q18 = 452,
  Q16_Q17_Q18_Q19 = 453,
  Q17_Q18_Q19_Q20 = 454,
  Q18_Q19_Q20_Q21 = 455,
  Q19_Q20_Q21_Q22 = 456,
  Q20_Q21_Q22_Q23 = 457,
  Q21_Q22_Q23_Q24 = 458,
  Q22_Q23_Q24_Q25 = 459,
  Q23_Q24_Q25_Q26 = 460,
  Q24_Q25_Q26_Q27 = 461,
  Q25_Q26_Q27_Q28 = 462,
  Q26_Q27_Q28_Q29 = 463,
  Q27_Q28_Q29_Q30 = 464,
  Q28_Q29_Q30_Q31 = 465,
  Q29_Q30_Q31_Q0 = 466,
  Q30_Q31_Q0_Q1 = 467,
  Q31_Q0_Q1_Q2 = 468,
  Q0_Q1_Q2 = 469,
  Q1_Q2_Q3 = 470,
  Q2_Q3_Q4 = 471,
  Q3_Q4_Q5 = 472,
  Q4_Q5_Q6 = 473,
  Q5_Q6_Q7 = 474,
  Q6_Q7_Q8 = 475,
  Q7_Q8_Q9 = 476,
  Q8_Q9_Q10 = 477,
  Q9_Q10_Q11 = 478,
  Q10_Q11_Q12 = 479,
  Q11_Q12_Q13 = 480,
  Q12_Q13_Q14 = 481,
  Q13_Q14_Q15 = 482,
  Q14_Q15_Q16 = 483,
  Q15_Q16_Q17 = 484,
  Q16_Q17_Q18 = 485,
  Q17_Q18_Q19 = 486,
  Q18_Q19_Q20 = 487,
  Q19_Q20_Q21 = 488,
  Q20_Q21_Q22 = 489,
  Q21_Q22_Q23 = 490,
  Q22_Q23_Q24 = 491,
  Q23_Q24_Q25 = 492,
  Q24_Q25_Q26 = 493,
  Q25_Q26_Q27 = 494,
  Q26_Q27_Q28 = 495,
  Q27_Q28_Q29 = 496,
  Q28_Q29_Q30 = 497,
  Q29_Q30_Q31 = 498,
  Q30_Q31_Q0 = 499,
  Q31_Q0_Q1 = 500,
  WZR_W0 = 501,
  W30_WZR = 502,
  W0_W1 = 503,
  W1_W2 = 504,
  W2_W3 = 505,
  W3_W4 = 506,
  W4_W5 = 507,
  W5_W6 = 508,
  W6_W7 = 509,
  W7_W8 = 510,
  W8_W9 = 511,
  W9_W10 = 512,
  W10_W11 = 513,
  W11_W12 = 514,
  W12_W13 = 515,
  W13_W14 = 516,
  W14_W15 = 517,
  W15_W16 = 518,
  W16_W17 = 519,
  W17_W18 = 520,
  W18_W19 = 521,
  W19_W20 = 522,
  W20_W21 = 523,
  W21_W22 = 524,
  W22_W23 = 525,
  W23_W24 = 526,
  W24_W25 = 527,
  W25_W26 = 528,
  W26_W27 = 529,
  W27_W28 = 530,
  W28_W29 = 531,
  W29_W30 = 532,
  FP_LR = 533,
  LR_XZR = 534,
  XZR_X0 = 535,
  X28_FP = 536,
  X0_X1 = 537,
  X1_X2 = 538,
  X2_X3 = 539,
  X3_X4 = 540,
  X4_X5 = 541,
  X5_X6 = 542,
  X6_X7 = 543,
  X7_X8 = 544,
  X8_X9 = 545,
  X9_X10 = 546,
  X10_X11 = 547,
  X11_X12 = 548,
  X12_X13 = 549,
  X13_X14 = 550,
  X14_X15 = 551,
  X15_X16 = 552,
  X16_X17 = 553,
  X17_X18 = 554,
  X18_X19 = 555,
  X19_X20 = 556,
  X20_X21 = 557,
  X21_X22 = 558,
  X22_X23 = 559,
  X23_X24 = 560,
  X24_X25 = 561,
  X25_X26 = 562,
  X26_X27 = 563,
  X27_X28 = 564,
  Z0_Z1 = 565,
  Z1_Z2 = 566,
  Z2_Z3 = 567,
  Z3_Z4 = 568,
  Z4_Z5 = 569,
  Z5_Z6 = 570,
  Z6_Z7 = 571,
  Z7_Z8 = 572,
  Z8_Z9 = 573,
  Z9_Z10 = 574,
  Z10_Z11 = 575,
  Z11_Z12 = 576,
  Z12_Z13 = 577,
  Z13_Z14 = 578,
  Z14_Z15 = 579,
  Z15_Z16 = 580,
  Z16_Z17 = 581,
  Z17_Z18 = 582,
  Z18_Z19 = 583,
  Z19_Z20 = 584,
  Z20_Z21 = 585,
  Z21_Z22 = 586,
  Z22_Z23 = 587,
  Z23_Z24 = 588,
  Z24_Z25 = 589,
  Z25_Z26 = 590,
  Z26_Z27 = 591,
  Z27_Z28 = 592,
  Z28_Z29 = 593,
  Z29_Z30 = 594,
  Z30_Z31 = 595,
  Z31_Z0 = 596,
  Z0_Z1_Z2_Z3 = 597,
  Z1_Z2_Z3_Z4 = 598,
  Z2_Z3_Z4_Z5 = 599,
  Z3_Z4_Z5_Z6 = 600,
  Z4_Z5_Z6_Z7 = 601,
  Z5_Z6_Z7_Z8 = 602,
  Z6_Z7_Z8_Z9 = 603,
  Z7_Z8_Z9_Z10 = 604,
  Z8_Z9_Z10_Z11 = 605,
  Z9_Z10_Z11_Z12 = 606,
  Z10_Z11_Z12_Z13 = 607,
  Z11_Z12_Z13_Z14 = 608,
  Z12_Z13_Z14_Z15 = 609,
  Z13_Z14_Z15_Z16 = 610,
  Z14_Z15_Z16_Z17 = 611,
  Z15_Z16_Z17_Z18 = 612,
  Z16_Z17_Z18_Z19 = 613,
  Z17_Z18_Z19_Z20 = 614,
  Z18_Z19_Z20_Z21 = 615,
  Z19_Z20_Z21_Z22 = 616,
  Z20_Z21_Z22_Z23 = 617,
  Z21_Z22_Z23_Z24 = 618,
  Z22_Z23_Z24_Z25 = 619,
  Z23_Z24_Z25_Z26 = 620,
  Z24_Z25_Z26_Z27 = 621,
  Z25_Z26_Z27_Z28 = 622,
  Z26_Z27_Z28_Z29 = 623,
  Z27_Z28_Z29_Z30 = 624,
  Z28_Z29_Z30_Z31 = 625,
  Z29_Z30_Z31_Z0 = 626,
  Z30_Z31_Z0_Z1 = 627,
  Z31_Z0_Z1_Z2 = 628,
  Z0_Z1_Z2 = 629,
  Z1_Z2_Z3 = 630,
  Z2_Z3_Z4 = 631,
  Z3_Z4_Z5 = 632,
  Z4_Z5_Z6 = 633,
  Z5_Z6_Z7 = 634,
  Z6_Z7_Z8 = 635,
  Z7_Z8_Z9 = 636,
  Z8_Z9_Z10 = 637,
  Z9_Z10_Z11 = 638,
  Z10_Z11_Z12 = 639,
  Z11_Z12_Z13 = 640,
  Z12_Z13_Z14 = 641,
  Z13_Z14_Z15 = 642,
  Z14_Z15_Z16 = 643,
  Z15_Z16_Z17 = 644,
  Z16_Z17_Z18 = 645,
  Z17_Z18_Z19 = 646,
  Z18_Z19_Z20 = 647,
  Z19_Z20_Z21 = 648,
  Z20_Z21_Z22 = 649,
  Z21_Z22_Z23 = 650,
  Z22_Z23_Z24 = 651,
  Z23_Z24_Z25 = 652,
  Z24_Z25_Z26 = 653,
  Z25_Z26_Z27 = 654,
  Z26_Z27_Z28 = 655,
  Z27_Z28_Z29 = 656,
  Z28_Z29_Z30 = 657,
  Z29_Z30_Z31 = 658,
  Z30_Z31_Z0 = 659,
  Z31_Z0_Z1 = 660,
  NUM_TARGET_REGS 	// 661
};
} // end namespace AArch64

// Register classes

namespace AArch64 {
enum {
  FPR8RegClassID = 0,
  FPR16RegClassID = 1,
  PPRRegClassID = 2,
  PPR_3bRegClassID = 3,
  GPR32allRegClassID = 4,
  FPR32RegClassID = 5,
  GPR32RegClassID = 6,
  GPR32spRegClassID = 7,
  GPR32commonRegClassID = 8,
  CCRRegClassID = 9,
  GPR32sponlyRegClassID = 10,
  WSeqPairsClassRegClassID = 11,
  WSeqPairsClass_with_sube32_in_GPR32commonRegClassID = 12,
  WSeqPairsClass_with_subo32_in_GPR32commonRegClassID = 13,
  WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClassID = 14,
  GPR64allRegClassID = 15,
  FPR64RegClassID = 16,
  GPR64RegClassID = 17,
  GPR64spRegClassID = 18,
  GPR64commonRegClassID = 19,
  tcGPR64RegClassID = 20,
  GPR64sponlyRegClassID = 21,
  DDRegClassID = 22,
  XSeqPairsClassRegClassID = 23,
  XSeqPairsClass_with_sub_32_in_GPR32commonRegClassID = 24,
  XSeqPairsClass_with_subo64_in_GPR64commonRegClassID = 25,
  XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClassID = 26,
  XSeqPairsClass_with_sube64_in_tcGPR64RegClassID = 27,
  XSeqPairsClass_with_subo64_in_tcGPR64RegClassID = 28,
  XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClassID = 29,
  FPR128RegClassID = 30,
  ZPRRegClassID = 31,
  FPR128_loRegClassID = 32,
  ZPR_4bRegClassID = 33,
  ZPR_3bRegClassID = 34,
  DDDRegClassID = 35,
  DDDDRegClassID = 36,
  QQRegClassID = 37,
  ZPR2RegClassID = 38,
  QQ_with_qsub0_in_FPR128_loRegClassID = 39,
  QQ_with_qsub1_in_FPR128_loRegClassID = 40,
  ZPR2_with_zsub1_in_ZPR_4bRegClassID = 41,
  ZPR2_with_zsub_in_FPR128_loRegClassID = 42,
  QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClassID = 43,
  ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClassID = 44,
  ZPR2_with_zsub0_in_ZPR_3bRegClassID = 45,
  ZPR2_with_zsub1_in_ZPR_3bRegClassID = 46,
  ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClassID = 47,
  QQQRegClassID = 48,
  ZPR3RegClassID = 49,
  QQQ_with_qsub0_in_FPR128_loRegClassID = 50,
  QQQ_with_qsub1_in_FPR128_loRegClassID = 51,
  QQQ_with_qsub2_in_FPR128_loRegClassID = 52,
  ZPR3_with_zsub1_in_ZPR_4bRegClassID = 53,
  ZPR3_with_zsub2_in_ZPR_4bRegClassID = 54,
  ZPR3_with_zsub_in_FPR128_loRegClassID = 55,
  QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClassID = 56,
  QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID = 57,
  ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID = 58,
  ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClassID = 59,
  QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID = 60,
  ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID = 61,
  ZPR3_with_zsub0_in_ZPR_3bRegClassID = 62,
  ZPR3_with_zsub1_in_ZPR_3bRegClassID = 63,
  ZPR3_with_zsub2_in_ZPR_3bRegClassID = 64,
  ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID = 65,
  ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClassID = 66,
  ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID = 67,
  QQQQRegClassID = 68,
  ZPR4RegClassID = 69,
  QQQQ_with_qsub0_in_FPR128_loRegClassID = 70,
  QQQQ_with_qsub1_in_FPR128_loRegClassID = 71,
  QQQQ_with_qsub2_in_FPR128_loRegClassID = 72,
  QQQQ_with_qsub3_in_FPR128_loRegClassID = 73,
  ZPR4_with_zsub1_in_ZPR_4bRegClassID = 74,
  ZPR4_with_zsub2_in_ZPR_4bRegClassID = 75,
  ZPR4_with_zsub3_in_ZPR_4bRegClassID = 76,
  ZPR4_with_zsub_in_FPR128_loRegClassID = 77,
  QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClassID = 78,
  QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID = 79,
  QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID = 80,
  ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID = 81,
  ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID = 82,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClassID = 83,
  QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID = 84,
  QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID = 85,
  ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID = 86,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID = 87,
  QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID = 88,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID = 89,
  ZPR4_with_zsub0_in_ZPR_3bRegClassID = 90,
  ZPR4_with_zsub1_in_ZPR_3bRegClassID = 91,
  ZPR4_with_zsub2_in_ZPR_3bRegClassID = 92,
  ZPR4_with_zsub3_in_ZPR_3bRegClassID = 93,
  ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID = 94,
  ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID = 95,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClassID = 96,
  ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID = 97,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID = 98,
  ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID = 99,

  };
} // end namespace AArch64


// Register alternate name indices

namespace AArch64 {
enum {
  NoRegAltName,	// 0
  vlist1,	// 1
  vreg,	// 2
  NUM_TARGET_REG_ALT_NAMES = 3
};
} // end namespace AArch64


// Subregister indices

namespace AArch64 {
enum {
  NoSubRegister,
  bsub,	// 1
  dsub,	// 2
  dsub0,	// 3
  dsub1,	// 4
  dsub2,	// 5
  dsub3,	// 6
  hsub,	// 7
  qhisub,	// 8
  qsub,	// 9
  qsub0,	// 10
  qsub1,	// 11
  qsub2,	// 12
  qsub3,	// 13
  ssub,	// 14
  sub_32,	// 15
  sube32,	// 16
  sube64,	// 17
  subo32,	// 18
  subo64,	// 19
  zsub,	// 20
  zsub0,	// 21
  zsub1,	// 22
  zsub2,	// 23
  zsub3,	// 24
  zsub_hi,	// 25
  dsub1_then_bsub,	// 26
  dsub1_then_hsub,	// 27
  dsub1_then_ssub,	// 28
  dsub3_then_bsub,	// 29
  dsub3_then_hsub,	// 30
  dsub3_then_ssub,	// 31
  dsub2_then_bsub,	// 32
  dsub2_then_hsub,	// 33
  dsub2_then_ssub,	// 34
  qsub1_then_bsub,	// 35
  qsub1_then_dsub,	// 36
  qsub1_then_hsub,	// 37
  qsub1_then_ssub,	// 38
  qsub3_then_bsub,	// 39
  qsub3_then_dsub,	// 40
  qsub3_then_hsub,	// 41
  qsub3_then_ssub,	// 42
  qsub2_then_bsub,	// 43
  qsub2_then_dsub,	// 44
  qsub2_then_hsub,	// 45
  qsub2_then_ssub,	// 46
  subo64_then_sub_32,	// 47
  zsub1_then_bsub,	// 48
  zsub1_then_dsub,	// 49
  zsub1_then_hsub,	// 50
  zsub1_then_ssub,	// 51
  zsub1_then_zsub,	// 52
  zsub1_then_zsub_hi,	// 53
  zsub3_then_bsub,	// 54
  zsub3_then_dsub,	// 55
  zsub3_then_hsub,	// 56
  zsub3_then_ssub,	// 57
  zsub3_then_zsub,	// 58
  zsub3_then_zsub_hi,	// 59
  zsub2_then_bsub,	// 60
  zsub2_then_dsub,	// 61
  zsub2_then_hsub,	// 62
  zsub2_then_ssub,	// 63
  zsub2_then_zsub,	// 64
  zsub2_then_zsub_hi,	// 65
  dsub0_dsub1,	// 66
  dsub0_dsub1_dsub2,	// 67
  dsub1_dsub2,	// 68
  dsub1_dsub2_dsub3,	// 69
  dsub2_dsub3,	// 70
  dsub_qsub1_then_dsub,	// 71
  dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub,	// 72
  dsub_qsub1_then_dsub_qsub2_then_dsub,	// 73
  qsub0_qsub1,	// 74
  qsub0_qsub1_qsub2,	// 75
  qsub1_qsub2,	// 76
  qsub1_qsub2_qsub3,	// 77
  qsub2_qsub3,	// 78
  qsub1_then_dsub_qsub2_then_dsub,	// 79
  qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub,	// 80
  qsub2_then_dsub_qsub3_then_dsub,	// 81
  sub_32_subo64_then_sub_32,	// 82
  dsub_zsub1_then_dsub,	// 83
  zsub_zsub1_then_zsub,	// 84
  dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub,	// 85
  dsub_zsub1_then_dsub_zsub2_then_dsub,	// 86
  zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub,	// 87
  zsub_zsub1_then_zsub_zsub2_then_zsub,	// 88
  zsub0_zsub1,	// 89
  zsub0_zsub1_zsub2,	// 90
  zsub1_zsub2,	// 91
  zsub1_zsub2_zsub3,	// 92
  zsub2_zsub3,	// 93
  zsub1_then_dsub_zsub2_then_dsub,	// 94
  zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub,	// 95
  zsub1_then_zsub_zsub2_then_zsub,	// 96
  zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub,	// 97
  zsub2_then_dsub_zsub3_then_dsub,	// 98
  zsub2_then_zsub_zsub3_then_zsub,	// 99
  NUM_TARGET_SUBREGS
};
} // end namespace AArch64

} // end namespace llvm

#endif // GET_REGINFO_ENUM

/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* MC Register Information                                                    *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_REGINFO_MC_DESC
#undef GET_REGINFO_MC_DESC

namespace llvm {

extern const MCPhysReg AArch64RegDiffLists[] = {
  /* 0 */ 64945, 1, 1, 1, 74, 1, 1, 1, 0,
  /* 9 */ 65105, 1, 1, 1, 0,
  /* 14 */ 65201, 1, 1, 1, 0,
  /* 19 */ 6, 29, 1, 1, 0,
  /* 24 */ 6, 29, 1, 1, 46, 29, 1, 1, 0,
  /* 33 */ 65324, 499, 30, 1, 1, 0,
  /* 39 */ 64913, 1, 1, 75, 1, 1, 0,
  /* 46 */ 65073, 1, 1, 0,
  /* 50 */ 65169, 1, 1, 0,
  /* 54 */ 6, 1, 29, 1, 0,
  /* 59 */ 6, 1, 29, 1, 46, 1, 29, 1, 0,
  /* 68 */ 6, 30, 1, 0,
  /* 72 */ 6, 30, 1, 46, 30, 1, 0,
  /* 79 */ 1, 493, 1, 32, 1, 0,
  /* 85 */ 31, 286, 1, 33, 1, 0,
  /* 91 */ 64977, 1, 76, 1, 0,
  /* 96 */ 65204, 112, 65456, 65472, 33, 112, 65456, 65472, 33, 112, 65456, 65472, 298, 1, 0,
  /* 111 */ 320, 1, 0,
  /* 114 */ 65204, 112, 65456, 65472, 33, 112, 65456, 65472, 1, 112, 65456, 65472, 330, 1, 0,
  /* 129 */ 526, 1, 0,
  /* 132 */ 530, 1, 0,
  /* 135 */ 65053, 1, 0,
  /* 138 */ 65087, 1, 0,
  /* 141 */ 65137, 1, 0,
  /* 144 */ 65218, 1, 0,
  /* 147 */ 65233, 1, 0,
  /* 150 */ 64, 80, 65424, 80, 124, 63, 1, 62, 65503, 34, 65503, 34, 65503, 1, 63, 1, 62, 65503, 34, 65503, 34, 65503, 1, 127, 1, 62, 65503, 34, 65503, 34, 65503, 1, 0,
  /* 183 */ 124, 159, 1, 62, 65503, 34, 65503, 34, 65503, 1, 127, 1, 62, 65503, 34, 65503, 34, 65503, 1, 0,
  /* 203 */ 65504, 319, 1, 62, 65503, 34, 65503, 34, 65503, 1, 0,
  /* 214 */ 64, 80, 65424, 80, 124, 64, 31, 33, 65504, 62, 65503, 34, 65503, 1, 33, 31, 33, 65504, 62, 65503, 34, 65503, 1, 97, 31, 33, 65504, 62, 65503, 34, 65503, 1, 0,
  /* 247 */ 124, 160, 31, 33, 65504, 62, 65503, 34, 65503, 1, 97, 31, 33, 65504, 62, 65503, 34, 65503, 1, 0,
  /* 267 */ 65504, 320, 31, 33, 65504, 62, 65503, 34, 65503, 1, 0,
  /* 278 */ 63, 65503, 34, 65503, 1, 64, 63, 65503, 34, 65503, 1, 128, 63, 65503, 34, 65503, 1, 0,
  /* 296 */ 64, 80, 65424, 80, 124, 63, 1, 63, 1, 65503, 1, 62, 65503, 1, 33, 1, 63, 1, 65503, 1, 62, 65503, 1, 97, 1, 63, 1, 65503, 1, 62, 65503, 1, 0,
  /* 329 */ 124, 159, 1, 63, 1, 65503, 1, 62, 65503, 1, 97, 1, 63, 1, 65503, 1, 62, 65503, 1, 0,
  /* 349 */ 65504, 319, 1, 63, 1, 65503, 1, 62, 65503, 1, 0,
  /* 360 */ 64, 65504, 63, 65503, 1, 33, 64, 65504, 63, 65503, 1, 97, 64, 65504, 63, 65503, 1, 0,
  /* 378 */ 65503, 1, 128, 65503, 1, 192, 65503, 1, 0,
  /* 387 */ 31, 285, 2, 32, 2, 0,
  /* 393 */ 319, 2, 0,
  /* 396 */ 65324, 529, 1, 1, 3, 0,
  /* 402 */ 2, 3, 0,
  /* 405 */ 531, 3, 0,
  /* 408 */ 65004, 3, 0,
  /* 411 */ 4, 0,
  /* 413 */ 5, 0,
  /* 415 */ 31, 286, 1, 5, 28, 0,
  /* 421 */ 292, 28, 0,
  /* 424 */ 6, 1, 1, 29, 0,
  /* 429 */ 6, 1, 1, 29, 46, 1, 1, 29, 0,
  /* 438 */ 64, 80, 65424, 80, 124, 63, 1, 62, 1, 65503, 34, 65503, 1, 29, 34, 1, 62, 1, 65503, 34, 65503, 1, 29, 98, 1, 62, 1, 65503, 34, 65503, 1, 29, 0,
  /* 471 */ 124, 159, 1, 62, 1, 65503, 34, 65503, 1, 29, 98, 1, 62, 1, 65503, 34, 65503, 1, 29, 0,
  /* 491 */ 65504, 319, 1, 62, 1, 65503, 34, 65503, 1, 29, 0,
  /* 502 */ 6, 1, 30, 0,
  /* 506 */ 6, 1, 30, 46, 1, 30, 0,
  /* 513 */ 63, 1, 65503, 1, 30, 34, 63, 1, 65503, 1, 30, 98, 63, 1, 65503, 1, 30, 0,
  /* 531 */ 6, 31, 0,
  /* 534 */ 6, 31, 46, 31, 0,
  /* 539 */ 65504, 31, 97, 65504, 31, 161, 65504, 31, 0,
  /* 548 */ 32, 0,
  /* 550 */ 34, 0,
  /* 552 */ 5, 49, 0,
  /* 555 */ 63936, 49, 0,
  /* 558 */ 65297, 77, 0,
  /* 561 */ 1, 81, 0,
  /* 564 */ 65216, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 31, 96, 0,
  /* 581 */ 65216, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 63, 96, 0,
  /* 598 */ 65152, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 30, 96, 65504, 96, 96, 1, 65280, 96, 0,
  /* 628 */ 65152, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 62, 96, 65504, 96, 96, 1, 65280, 96, 0,
  /* 658 */ 65152, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 62, 96, 65504, 96, 96, 65505, 65280, 96, 0,
  /* 688 */ 65184, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 61, 96, 65472, 32, 64, 32, 96, 64, 65473, 64, 65441, 65311, 64, 32, 64, 65345, 96, 0,
  /* 734 */ 65184, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 61, 96, 65472, 32, 64, 32, 96, 64, 65441, 64, 65473, 65279, 64, 32, 64, 65377, 96, 0,
  /* 780 */ 65184, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 29, 96, 65472, 32, 64, 32, 96, 64, 65473, 64, 65473, 65279, 64, 32, 64, 65377, 96, 0,
  /* 826 */ 65184, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65505, 65412, 65456, 112, 65456, 65472, 268, 65473, 65412, 65456, 112, 65456, 65472, 268, 61, 96, 65472, 32, 64, 32, 96, 64, 65473, 64, 65473, 65279, 64, 32, 64, 65377, 96, 0,
  /* 872 */ 96, 160, 0,
  /* 875 */ 65042, 178, 0,
  /* 878 */ 212, 0,
  /* 880 */ 65412, 65456, 112, 65456, 65472, 268, 0,
  /* 887 */ 65252, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 299, 0,
  /* 899 */ 65009, 65535, 209, 65505, 316, 0,
  /* 905 */ 65005, 212, 65325, 212, 317, 0,
  /* 911 */ 65244, 65505, 65325, 212, 317, 0,
  /* 917 */ 65215, 65505, 32, 65505, 317, 0,
  /* 923 */ 65252, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 331, 0,
  /* 935 */ 65005, 212, 65329, 65535, 495, 0,
  /* 941 */ 65323, 0,
  /* 943 */ 65249, 65328, 0,
  /* 946 */ 65342, 0,
  /* 948 */ 65374, 0,
  /* 950 */ 65389, 0,
  /* 952 */ 65405, 0,
  /* 954 */ 65421, 0,
  /* 956 */ 65188, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 298, 64, 32, 1, 65440, 0,
  /* 977 */ 65188, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 330, 64, 32, 1, 65440, 0,
  /* 998 */ 65188, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 330, 64, 32, 65505, 65440, 0,
  /* 1019 */ 65220, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 329, 32, 32, 32, 64, 65473, 64, 65441, 65471, 64, 65441, 0,
  /* 1051 */ 65236, 112, 65456, 65472, 33, 112, 65456, 65472, 1, 112, 65456, 65472, 33, 112, 65456, 65472, 329, 64, 65473, 64, 65441, 0,
  /* 1073 */ 65469, 0,
  /* 1075 */ 65268, 112, 65456, 65472, 1, 112, 65456, 65472, 0,
  /* 1084 */ 65268, 112, 65456, 65472, 33, 112, 65456, 65472, 0,
  /* 1093 */ 65456, 112, 65456, 65472, 0,
  /* 1098 */ 65220, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 329, 32, 32, 32, 64, 65441, 64, 65473, 65439, 64, 65473, 0,
  /* 1130 */ 65220, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 297, 32, 32, 32, 64, 65473, 64, 65473, 65439, 64, 65473, 0,
  /* 1162 */ 65220, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 113, 65456, 112, 65456, 65472, 81, 65456, 112, 65456, 65472, 329, 32, 32, 32, 64, 65473, 64, 65473, 65439, 64, 65473, 0,
  /* 1194 */ 65236, 112, 65456, 65472, 1, 112, 65456, 65472, 33, 112, 65456, 65472, 33, 112, 65456, 65472, 329, 64, 65441, 64, 65473, 0,
  /* 1216 */ 65236, 112, 65456, 65472, 33, 112, 65456, 65472, 33, 112, 65456, 65472, 33, 112, 65456, 65472, 297, 64, 65473, 64, 65473, 0,
  /* 1238 */ 65236, 112, 65456, 65472, 33, 112, 65456, 65472, 33, 112, 65456, 65472, 1, 112, 65456, 65472, 329, 64, 65473, 64, 65473, 0,
  /* 1260 */ 65501, 0,
  /* 1262 */ 65204, 112, 65456, 65472, 1, 112, 65456, 65472, 33, 112, 65456, 65472, 330, 65505, 0,
  /* 1277 */ 65533, 0,
  /* 1279 */ 65535, 0,
};

extern const LaneBitmask AArch64LaneMaskLists[] = {
  /* 0 */ LaneBitmask(0x00000000), LaneBitmask::getAll(),
  /* 2 */ LaneBitmask(0x00000080), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 5 */ LaneBitmask(0x00000080), LaneBitmask(0x00000200), LaneBitmask(0x00000100), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 10 */ LaneBitmask(0x00000080), LaneBitmask(0x00000200), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 14 */ LaneBitmask(0x00000400), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 17 */ LaneBitmask(0x00000400), LaneBitmask(0x00001000), LaneBitmask(0x00000800), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 22 */ LaneBitmask(0x00000400), LaneBitmask(0x00001000), LaneBitmask(0x00000001), LaneBitmask::getAll(),
  /* 26 */ LaneBitmask(0x00002000), LaneBitmask(0x00000008), LaneBitmask::getAll(),
  /* 29 */ LaneBitmask(0x00000020), LaneBitmask(0x00000010), LaneBitmask::getAll(),
  /* 32 */ LaneBitmask(0x00000010), LaneBitmask(0x00000020), LaneBitmask::getAll(),
  /* 35 */ LaneBitmask(0x00000001), LaneBitmask(0x00000040), LaneBitmask::getAll(),
  /* 38 */ LaneBitmask(0x00004000), LaneBitmask(0x00000001), LaneBitmask(0x00008000), LaneBitmask(0x00000040), LaneBitmask::getAll(),
  /* 43 */ LaneBitmask(0x00004000), LaneBitmask(0x00040000), LaneBitmask(0x00010000), LaneBitmask(0x00000001), LaneBitmask(0x00008000), LaneBitmask(0x00080000), LaneBitmask(0x00020000), LaneBitmask(0x00000040), LaneBitmask::getAll(),
  /* 52 */ LaneBitmask(0x00004000), LaneBitmask(0x00040000), LaneBitmask(0x00000001), LaneBitmask(0x00008000), LaneBitmask(0x00080000), LaneBitmask(0x00000040), LaneBitmask::getAll(),
  /* 59 */ LaneBitmask(0x00000200), LaneBitmask(0x00000100), LaneBitmask(0x00000001), LaneBitmask(0x00000080), LaneBitmask::getAll(),
  /* 64 */ LaneBitmask(0x00000200), LaneBitmask(0x00000001), LaneBitmask(0x00000080), LaneBitmask::getAll(),
  /* 68 */ LaneBitmask(0x00000001), LaneBitmask(0x00000080), LaneBitmask(0x00000200), LaneBitmask(0x00000100), LaneBitmask::getAll(),
  /* 73 */ LaneBitmask(0x00000100), LaneBitmask(0x00000001), LaneBitmask(0x00000080), LaneBitmask(0x00000200), LaneBitmask::getAll(),
  /* 78 */ LaneBitmask(0x00001000), LaneBitmask(0x00000800), LaneBitmask(0x00000001), LaneBitmask(0x00000400), LaneBitmask::getAll(),
  /* 83 */ LaneBitmask(0x00001000), LaneBitmask(0x00000001), LaneBitmask(0x00000400), LaneBitmask::getAll(),
  /* 87 */ LaneBitmask(0x00000001), LaneBitmask(0x00000400), LaneBitmask(0x00001000), LaneBitmask(0x00000800), LaneBitmask::getAll(),
  /* 92 */ LaneBitmask(0x00000800), LaneBitmask(0x00000001), LaneBitmask(0x00000400), LaneBitmask(0x00001000), LaneBitmask::getAll(),
  /* 97 */ LaneBitmask(0x00000008), LaneBitmask(0x00002000), LaneBitmask::getAll(),
  /* 100 */ LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask::getAll(),
  /* 105 */ LaneBitmask(0x00040000), LaneBitmask(0x00010000), LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00080000), LaneBitmask(0x00020000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask::getAll(),
  /* 114 */ LaneBitmask(0x00040000), LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00080000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask::getAll(),
  /* 121 */ LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00040000), LaneBitmask(0x00010000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask(0x00080000), LaneBitmask(0x00020000), LaneBitmask::getAll(),
  /* 130 */ LaneBitmask(0x00010000), LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00040000), LaneBitmask(0x00020000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask(0x00080000), LaneBitmask::getAll(),
  /* 139 */ LaneBitmask(0x00000001), LaneBitmask(0x00004000), LaneBitmask(0x00040000), LaneBitmask(0x00000040), LaneBitmask(0x00008000), LaneBitmask(0x00080000), LaneBitmask::getAll(),
};

extern const uint16_t AArch64SubRegIdxLists[] = {
  /* 0 */ 2, 14, 7, 1, 0,
  /* 5 */ 15, 0,
  /* 7 */ 16, 18, 0,
  /* 10 */ 20, 2, 14, 7, 1, 25, 0,
  /* 17 */ 3, 14, 7, 1, 4, 28, 27, 26, 0,
  /* 26 */ 3, 14, 7, 1, 4, 28, 27, 26, 5, 34, 33, 32, 66, 68, 0,
  /* 41 */ 3, 14, 7, 1, 4, 28, 27, 26, 5, 34, 33, 32, 6, 31, 30, 29, 66, 67, 68, 69, 70, 0,
  /* 63 */ 10, 2, 14, 7, 1, 11, 36, 38, 37, 35, 71, 0,
  /* 75 */ 10, 2, 14, 7, 1, 11, 36, 38, 37, 35, 12, 44, 46, 45, 43, 71, 73, 74, 76, 79, 0,
  /* 96 */ 10, 2, 14, 7, 1, 11, 36, 38, 37, 35, 12, 44, 46, 45, 43, 13, 40, 42, 41, 39, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 0,
  /* 128 */ 17, 15, 19, 47, 82, 0,
  /* 134 */ 21, 20, 2, 14, 7, 1, 25, 22, 52, 49, 51, 50, 48, 53, 83, 84, 0,
  /* 151 */ 21, 20, 2, 14, 7, 1, 25, 22, 52, 49, 51, 50, 48, 53, 23, 64, 61, 63, 62, 60, 65, 83, 84, 86, 88, 89, 91, 94, 96, 0,
  /* 181 */ 21, 20, 2, 14, 7, 1, 25, 22, 52, 49, 51, 50, 48, 53, 23, 64, 61, 63, 62, 60, 65, 24, 58, 55, 57, 56, 54, 59, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 0,
};

extern const MCRegisterInfo::SubRegCoveredBits AArch64SubRegIdxRanges[] = {
  { 65535, 65535 },
  { 0, 8 },	// bsub
  { 0, 32 },	// dsub
  { 0, 64 },	// dsub0
  { 0, 64 },	// dsub1
  { 0, 64 },	// dsub2
  { 0, 64 },	// dsub3
  { 0, 16 },	// hsub
  { 0, 64 },	// qhisub
  { 0, 64 },	// qsub
  { 0, 128 },	// qsub0
  { 0, 128 },	// qsub1
  { 0, 128 },	// qsub2
  { 0, 128 },	// qsub3
  { 0, 32 },	// ssub
  { 0, 32 },	// sub_32
  { 0, 32 },	// sube32
  { 0, 64 },	// sube64
  { 0, 32 },	// subo32
  { 0, 64 },	// subo64
  { 0, 128 },	// zsub
  { 65535, 128 },	// zsub0
  { 65535, 128 },	// zsub1
  { 65535, 128 },	// zsub2
  { 65535, 128 },	// zsub3
  { 0, 128 },	// zsub_hi
  { 0, 8 },	// dsub1_then_bsub
  { 0, 16 },	// dsub1_then_hsub
  { 0, 32 },	// dsub1_then_ssub
  { 0, 8 },	// dsub3_then_bsub
  { 0, 16 },	// dsub3_then_hsub
  { 0, 32 },	// dsub3_then_ssub
  { 0, 8 },	// dsub2_then_bsub
  { 0, 16 },	// dsub2_then_hsub
  { 0, 32 },	// dsub2_then_ssub
  { 0, 8 },	// qsub1_then_bsub
  { 0, 32 },	// qsub1_then_dsub
  { 0, 16 },	// qsub1_then_hsub
  { 0, 32 },	// qsub1_then_ssub
  { 0, 8 },	// qsub3_then_bsub
  { 0, 32 },	// qsub3_then_dsub
  { 0, 16 },	// qsub3_then_hsub
  { 0, 32 },	// qsub3_then_ssub
  { 0, 8 },	// qsub2_then_bsub
  { 0, 32 },	// qsub2_then_dsub
  { 0, 16 },	// qsub2_then_hsub
  { 0, 32 },	// qsub2_then_ssub
  { 0, 32 },	// subo64_then_sub_32
  { 65535, 65535 },	// zsub1_then_bsub
  { 65535, 65535 },	// zsub1_then_dsub
  { 65535, 65535 },	// zsub1_then_hsub
  { 65535, 65535 },	// zsub1_then_ssub
  { 65535, 65535 },	// zsub1_then_zsub
  { 65535, 65535 },	// zsub1_then_zsub_hi
  { 65535, 65535 },	// zsub3_then_bsub
  { 65535, 65535 },	// zsub3_then_dsub
  { 65535, 65535 },	// zsub3_then_hsub
  { 65535, 65535 },	// zsub3_then_ssub
  { 65535, 65535 },	// zsub3_then_zsub
  { 65535, 65535 },	// zsub3_then_zsub_hi
  { 65535, 65535 },	// zsub2_then_bsub
  { 65535, 65535 },	// zsub2_then_dsub
  { 65535, 65535 },	// zsub2_then_hsub
  { 65535, 65535 },	// zsub2_then_ssub
  { 65535, 65535 },	// zsub2_then_zsub
  { 65535, 65535 },	// zsub2_then_zsub_hi
  { 65535, 128 },	// dsub0_dsub1
  { 65535, 192 },	// dsub0_dsub1_dsub2
  { 65535, 128 },	// dsub1_dsub2
  { 65535, 192 },	// dsub1_dsub2_dsub3
  { 65535, 128 },	// dsub2_dsub3
  { 65535, 64 },	// dsub_qsub1_then_dsub
  { 65535, 128 },	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  { 65535, 96 },	// dsub_qsub1_then_dsub_qsub2_then_dsub
  { 65535, 256 },	// qsub0_qsub1
  { 65535, 384 },	// qsub0_qsub1_qsub2
  { 65535, 256 },	// qsub1_qsub2
  { 65535, 384 },	// qsub1_qsub2_qsub3
  { 65535, 256 },	// qsub2_qsub3
  { 65535, 64 },	// qsub1_then_dsub_qsub2_then_dsub
  { 65535, 96 },	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  { 65535, 64 },	// qsub2_then_dsub_qsub3_then_dsub
  { 65535, 64 },	// sub_32_subo64_then_sub_32
  { 65535, 31 },	// dsub_zsub1_then_dsub
  { 65535, 127 },	// zsub_zsub1_then_zsub
  { 65535, 29 },	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
  { 65535, 30 },	// dsub_zsub1_then_dsub_zsub2_then_dsub
  { 65535, 125 },	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
  { 65535, 126 },	// zsub_zsub1_then_zsub_zsub2_then_zsub
  { 65535, 256 },	// zsub0_zsub1
  { 65535, 384 },	// zsub0_zsub1_zsub2
  { 65535, 256 },	// zsub1_zsub2
  { 65535, 384 },	// zsub1_zsub2_zsub3
  { 65535, 256 },	// zsub2_zsub3
  { 65535, 65534 },	// zsub1_then_dsub_zsub2_then_dsub
  { 65535, 65533 },	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
  { 65535, 65534 },	// zsub1_then_zsub_zsub2_then_zsub
  { 65535, 65533 },	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
  { 65535, 65534 },	// zsub2_then_dsub_zsub3_then_dsub
  { 65535, 65534 },	// zsub2_then_zsub_zsub3_then_zsub
};

extern const char AArch64RegStrings[] = {
  /* 0 */ 'B', '1', '0', 0,
  /* 4 */ 'D', '7', '_', 'D', '8', '_', 'D', '9', '_', 'D', '1', '0', 0,
  /* 17 */ 'H', '1', '0', 0,
  /* 21 */ 'P', '1', '0', 0,
  /* 25 */ 'Q', '7', '_', 'Q', '8', '_', 'Q', '9', '_', 'Q', '1', '0', 0,
  /* 38 */ 'S', '1', '0', 0,
  /* 42 */ 'W', '9', '_', 'W', '1', '0', 0,
  /* 49 */ 'X', '9', '_', 'X', '1', '0', 0,
  /* 56 */ 'Z', '7', '_', 'Z', '8', '_', 'Z', '9', '_', 'Z', '1', '0', 0,
  /* 69 */ 'B', '2', '0', 0,
  /* 73 */ 'D', '1', '7', '_', 'D', '1', '8', '_', 'D', '1', '9', '_', 'D', '2', '0', 0,
  /* 89 */ 'H', '2', '0', 0,
  /* 93 */ 'Q', '1', '7', '_', 'Q', '1', '8', '_', 'Q', '1', '9', '_', 'Q', '2', '0', 0,
  /* 109 */ 'S', '2', '0', 0,
  /* 113 */ 'W', '1', '9', '_', 'W', '2', '0', 0,
  /* 121 */ 'X', '1', '9', '_', 'X', '2', '0', 0,
  /* 129 */ 'Z', '1', '7', '_', 'Z', '1', '8', '_', 'Z', '1', '9', '_', 'Z', '2', '0', 0,
  /* 145 */ 'B', '3', '0', 0,
  /* 149 */ 'D', '2', '7', '_', 'D', '2', '8', '_', 'D', '2', '9', '_', 'D', '3', '0', 0,
  /* 165 */ 'H', '3', '0', 0,
  /* 169 */ 'Q', '2', '7', '_', 'Q', '2', '8', '_', 'Q', '2', '9', '_', 'Q', '3', '0', 0,
  /* 185 */ 'S', '3', '0', 0,
  /* 189 */ 'W', '2', '9', '_', 'W', '3', '0', 0,
  /* 197 */ 'Z', '2', '7', '_', 'Z', '2', '8', '_', 'Z', '2', '9', '_', 'Z', '3', '0', 0,
  /* 213 */ 'B', '0', 0,
  /* 216 */ 'D', '2', '9', '_', 'D', '3', '0', '_', 'D', '3', '1', '_', 'D', '0', 0,
  /* 231 */ 'H', '0', 0,
  /* 234 */ 'P', '0', 0,
  /* 237 */ 'Q', '2', '9', '_', 'Q', '3', '0', '_', 'Q', '3', '1', '_', 'Q', '0', 0,
  /* 252 */ 'S', '0', 0,
  /* 255 */ 'W', 'Z', 'R', '_', 'W', '0', 0,
  /* 262 */ 'X', 'Z', 'R', '_', 'X', '0', 0,
  /* 269 */ 'Z', '2', '9', '_', 'Z', '3', '0', '_', 'Z', '3', '1', '_', 'Z', '0', 0,
  /* 284 */ 'B', '1', '1', 0,
  /* 288 */ 'D', '8', '_', 'D', '9', '_', 'D', '1', '0', '_', 'D', '1', '1', 0,
  /* 302 */ 'H', '1', '1', 0,
  /* 306 */ 'P', '1', '1', 0,
  /* 310 */ 'Q', '8', '_', 'Q', '9', '_', 'Q', '1', '0', '_', 'Q', '1', '1', 0,
  /* 324 */ 'S', '1', '1', 0,
  /* 328 */ 'W', '1', '0', '_', 'W', '1', '1', 0,
  /* 336 */ 'X', '1', '0', '_', 'X', '1', '1', 0,
  /* 344 */ 'Z', '8', '_', 'Z', '9', '_', 'Z', '1', '0', '_', 'Z', '1', '1', 0,
  /* 358 */ 'B', '2', '1', 0,
  /* 362 */ 'D', '1', '8', '_', 'D', '1', '9', '_', 'D', '2', '0', '_', 'D', '2', '1', 0,
  /* 378 */ 'H', '2', '1', 0,
  /* 382 */ 'Q', '1', '8', '_', 'Q', '1', '9', '_', 'Q', '2', '0', '_', 'Q', '2', '1', 0,
  /* 398 */ 'S', '2', '1', 0,
  /* 402 */ 'W', '2', '0', '_', 'W', '2', '1', 0,
  /* 410 */ 'X', '2', '0', '_', 'X', '2', '1', 0,
  /* 418 */ 'Z', '1', '8', '_', 'Z', '1', '9', '_', 'Z', '2', '0', '_', 'Z', '2', '1', 0,
  /* 434 */ 'B', '3', '1', 0,
  /* 438 */ 'D', '2', '8', '_', 'D', '2', '9', '_', 'D', '3', '0', '_', 'D', '3', '1', 0,
  /* 454 */ 'H', '3', '1', 0,
  /* 458 */ 'Q', '2', '8', '_', 'Q', '2', '9', '_', 'Q', '3', '0', '_', 'Q', '3', '1', 0,
  /* 474 */ 'S', '3', '1', 0,
  /* 478 */ 'Z', '2', '8', '_', 'Z', '2', '9', '_', 'Z', '3', '0', '_', 'Z', '3', '1', 0,
  /* 494 */ 'B', '1', 0,
  /* 497 */ 'D', '3', '0', '_', 'D', '3', '1', '_', 'D', '0', '_', 'D', '1', 0,
  /* 511 */ 'H', '1', 0,
  /* 514 */ 'P', '1', 0,
  /* 517 */ 'Q', '3', '0', '_', 'Q', '3', '1', '_', 'Q', '0', '_', 'Q', '1', 0,
  /* 531 */ 'S', '1', 0,
  /* 534 */ 'W', '0', '_', 'W', '1', 0,
  /* 540 */ 'X', '0', '_', 'X', '1', 0,
  /* 546 */ 'Z', '3', '0', '_', 'Z', '3', '1', '_', 'Z', '0', '_', 'Z', '1', 0,
  /* 560 */ 'B', '1', '2', 0,
  /* 564 */ 'D', '9', '_', 'D', '1', '0', '_', 'D', '1', '1', '_', 'D', '1', '2', 0,
  /* 579 */ 'H', '1', '2', 0,
  /* 583 */ 'P', '1', '2', 0,
  /* 587 */ 'Q', '9', '_', 'Q', '1', '0', '_', 'Q', '1', '1', '_', 'Q', '1', '2', 0,
  /* 602 */ 'S', '1', '2', 0,
  /* 606 */ 'W', '1', '1', '_', 'W', '1', '2', 0,
  /* 614 */ 'X', '1', '1', '_', 'X', '1', '2', 0,
  /* 622 */ 'Z', '9', '_', 'Z', '1', '0', '_', 'Z', '1', '1', '_', 'Z', '1', '2', 0,
  /* 637 */ 'B', '2', '2', 0,
  /* 641 */ 'D', '1', '9', '_', 'D', '2', '0', '_', 'D', '2', '1', '_', 'D', '2', '2', 0,
  /* 657 */ 'H', '2', '2', 0,
  /* 661 */ 'Q', '1', '9', '_', 'Q', '2', '0', '_', 'Q', '2', '1', '_', 'Q', '2', '2', 0,
  /* 677 */ 'S', '2', '2', 0,
  /* 681 */ 'W', '2', '1', '_', 'W', '2', '2', 0,
  /* 689 */ 'X', '2', '1', '_', 'X', '2', '2', 0,
  /* 697 */ 'Z', '1', '9', '_', 'Z', '2', '0', '_', 'Z', '2', '1', '_', 'Z', '2', '2', 0,
  /* 713 */ 'B', '2', 0,
  /* 716 */ 'D', '3', '1', '_', 'D', '0', '_', 'D', '1', '_', 'D', '2', 0,
  /* 729 */ 'H', '2', 0,
  /* 732 */ 'P', '2', 0,
  /* 735 */ 'Q', '3', '1', '_', 'Q', '0', '_', 'Q', '1', '_', 'Q', '2', 0,
  /* 748 */ 'S', '2', 0,
  /* 751 */ 'W', '1', '_', 'W', '2', 0,
  /* 757 */ 'X', '1', '_', 'X', '2', 0,
  /* 763 */ 'Z', '3', '1', '_', 'Z', '0', '_', 'Z', '1', '_', 'Z', '2', 0,
  /* 776 */ 'B', '1', '3', 0,
  /* 780 */ 'D', '1', '0', '_', 'D', '1', '1', '_', 'D', '1', '2', '_', 'D', '1', '3', 0,
  /* 796 */ 'H', '1', '3', 0,
  /* 800 */ 'P', '1', '3', 0,
  /* 804 */ 'Q', '1', '0', '_', 'Q', '1', '1', '_', 'Q', '1', '2', '_', 'Q', '1', '3', 0,
  /* 820 */ 'S', '1', '3', 0,
  /* 824 */ 'W', '1', '2', '_', 'W', '1', '3', 0,
  /* 832 */ 'X', '1', '2', '_', 'X', '1', '3', 0,
  /* 840 */ 'Z', '1', '0', '_', 'Z', '1', '1', '_', 'Z', '1', '2', '_', 'Z', '1', '3', 0,
  /* 856 */ 'B', '2', '3', 0,
  /* 860 */ 'D', '2', '0', '_', 'D', '2', '1', '_', 'D', '2', '2', '_', 'D', '2', '3', 0,
  /* 876 */ 'H', '2', '3', 0,
  /* 880 */ 'Q', '2', '0', '_', 'Q', '2', '1', '_', 'Q', '2', '2', '_', 'Q', '2', '3', 0,
  /* 896 */ 'S', '2', '3', 0,
  /* 900 */ 'W', '2', '2', '_', 'W', '2', '3', 0,
  /* 908 */ 'X', '2', '2', '_', 'X', '2', '3', 0,
  /* 916 */ 'Z', '2', '0', '_', 'Z', '2', '1', '_', 'Z', '2', '2', '_', 'Z', '2', '3', 0,
  /* 932 */ 'B', '3', 0,
  /* 935 */ 'D', '0', '_', 'D', '1', '_', 'D', '2', '_', 'D', '3', 0,
  /* 947 */ 'H', '3', 0,
  /* 950 */ 'P', '3', 0,
  /* 953 */ 'Q', '0', '_', 'Q', '1', '_', 'Q', '2', '_', 'Q', '3', 0,
  /* 965 */ 'S', '3', 0,
  /* 968 */ 'W', '2', '_', 'W', '3', 0,
  /* 974 */ 'X', '2', '_', 'X', '3', 0,
  /* 980 */ 'Z', '0', '_', 'Z', '1', '_', 'Z', '2', '_', 'Z', '3', 0,
  /* 992 */ 'B', '1', '4', 0,
  /* 996 */ 'D', '1', '1', '_', 'D', '1', '2', '_', 'D', '1', '3', '_', 'D', '1', '4', 0,
  /* 1012 */ 'H', '1', '4', 0,
  /* 1016 */ 'P', '1', '4', 0,
  /* 1020 */ 'Q', '1', '1', '_', 'Q', '1', '2', '_', 'Q', '1', '3', '_', 'Q', '1', '4', 0,
  /* 1036 */ 'S', '1', '4', 0,
  /* 1040 */ 'W', '1', '3', '_', 'W', '1', '4', 0,
  /* 1048 */ 'X', '1', '3', '_', 'X', '1', '4', 0,
  /* 1056 */ 'Z', '1', '1', '_', 'Z', '1', '2', '_', 'Z', '1', '3', '_', 'Z', '1', '4', 0,
  /* 1072 */ 'B', '2', '4', 0,
  /* 1076 */ 'D', '2', '1', '_', 'D', '2', '2', '_', 'D', '2', '3', '_', 'D', '2', '4', 0,
  /* 1092 */ 'H', '2', '4', 0,
  /* 1096 */ 'Q', '2', '1', '_', 'Q', '2', '2', '_', 'Q', '2', '3', '_', 'Q', '2', '4', 0,
  /* 1112 */ 'S', '2', '4', 0,
  /* 1116 */ 'W', '2', '3', '_', 'W', '2', '4', 0,
  /* 1124 */ 'X', '2', '3', '_', 'X', '2', '4', 0,
  /* 1132 */ 'Z', '2', '1', '_', 'Z', '2', '2', '_', 'Z', '2', '3', '_', 'Z', '2', '4', 0,
  /* 1148 */ 'B', '4', 0,
  /* 1151 */ 'D', '1', '_', 'D', '2', '_', 'D', '3', '_', 'D', '4', 0,
  /* 1163 */ 'H', '4', 0,
  /* 1166 */ 'P', '4', 0,
  /* 1169 */ 'Q', '1', '_', 'Q', '2', '_', 'Q', '3', '_', 'Q', '4', 0,
  /* 1181 */ 'S', '4', 0,
  /* 1184 */ 'W', '3', '_', 'W', '4', 0,
  /* 1190 */ 'X', '3', '_', 'X', '4', 0,
  /* 1196 */ 'Z', '1', '_', 'Z', '2', '_', 'Z', '3', '_', 'Z', '4', 0,
  /* 1208 */ 'B', '1', '5', 0,
  /* 1212 */ 'D', '1', '2', '_', 'D', '1', '3', '_', 'D', '1', '4', '_', 'D', '1', '5', 0,
  /* 1228 */ 'H', '1', '5', 0,
  /* 1232 */ 'P', '1', '5', 0,
  /* 1236 */ 'Q', '1', '2', '_', 'Q', '1', '3', '_', 'Q', '1', '4', '_', 'Q', '1', '5', 0,
  /* 1252 */ 'S', '1', '5', 0,
  /* 1256 */ 'W', '1', '4', '_', 'W', '1', '5', 0,
  /* 1264 */ 'X', '1', '4', '_', 'X', '1', '5', 0,
  /* 1272 */ 'Z', '1', '2', '_', 'Z', '1', '3', '_', 'Z', '1', '4', '_', 'Z', '1', '5', 0,
  /* 1288 */ 'B', '2', '5', 0,
  /* 1292 */ 'D', '2', '2', '_', 'D', '2', '3', '_', 'D', '2', '4', '_', 'D', '2', '5', 0,
  /* 1308 */ 'H', '2', '5', 0,
  /* 1312 */ 'Q', '2', '2', '_', 'Q', '2', '3', '_', 'Q', '2', '4', '_', 'Q', '2', '5', 0,
  /* 1328 */ 'S', '2', '5', 0,
  /* 1332 */ 'W', '2', '4', '_', 'W', '2', '5', 0,
  /* 1340 */ 'X', '2', '4', '_', 'X', '2', '5', 0,
  /* 1348 */ 'Z', '2', '2', '_', 'Z', '2', '3', '_', 'Z', '2', '4', '_', 'Z', '2', '5', 0,
  /* 1364 */ 'B', '5', 0,
  /* 1367 */ 'D', '2', '_', 'D', '3', '_', 'D', '4', '_', 'D', '5', 0,
  /* 1379 */ 'H', '5', 0,
  /* 1382 */ 'P', '5', 0,
  /* 1385 */ 'Q', '2', '_', 'Q', '3', '_', 'Q', '4', '_', 'Q', '5', 0,
  /* 1397 */ 'S', '5', 0,
  /* 1400 */ 'W', '4', '_', 'W', '5', 0,
  /* 1406 */ 'X', '4', '_', 'X', '5', 0,
  /* 1412 */ 'Z', '2', '_', 'Z', '3', '_', 'Z', '4', '_', 'Z', '5', 0,
  /* 1424 */ 'B', '1', '6', 0,
  /* 1428 */ 'D', '1', '3', '_', 'D', '1', '4', '_', 'D', '1', '5', '_', 'D', '1', '6', 0,
  /* 1444 */ 'H', '1', '6', 0,
  /* 1448 */ 'Q', '1', '3', '_', 'Q', '1', '4', '_', 'Q', '1', '5', '_', 'Q', '1', '6', 0,
  /* 1464 */ 'S', '1', '6', 0,
  /* 1468 */ 'W', '1', '5', '_', 'W', '1', '6', 0,
  /* 1476 */ 'X', '1', '5', '_', 'X', '1', '6', 0,
  /* 1484 */ 'Z', '1', '3', '_', 'Z', '1', '4', '_', 'Z', '1', '5', '_', 'Z', '1', '6', 0,
  /* 1500 */ 'B', '2', '6', 0,
  /* 1504 */ 'D', '2', '3', '_', 'D', '2', '4', '_', 'D', '2', '5', '_', 'D', '2', '6', 0,
  /* 1520 */ 'H', '2', '6', 0,
  /* 1524 */ 'Q', '2', '3', '_', 'Q', '2', '4', '_', 'Q', '2', '5', '_', 'Q', '2', '6', 0,
  /* 1540 */ 'S', '2', '6', 0,
  /* 1544 */ 'W', '2', '5', '_', 'W', '2', '6', 0,
  /* 1552 */ 'X', '2', '5', '_', 'X', '2', '6', 0,
  /* 1560 */ 'Z', '2', '3', '_', 'Z', '2', '4', '_', 'Z', '2', '5', '_', 'Z', '2', '6', 0,
  /* 1576 */ 'B', '6', 0,
  /* 1579 */ 'D', '3', '_', 'D', '4', '_', 'D', '5', '_', 'D', '6', 0,
  /* 1591 */ 'H', '6', 0,
  /* 1594 */ 'P', '6', 0,
  /* 1597 */ 'Q', '3', '_', 'Q', '4', '_', 'Q', '5', '_', 'Q', '6', 0,
  /* 1609 */ 'S', '6', 0,
  /* 1612 */ 'W', '5', '_', 'W', '6', 0,
  /* 1618 */ 'X', '5', '_', 'X', '6', 0,
  /* 1624 */ 'Z', '3', '_', 'Z', '4', '_', 'Z', '5', '_', 'Z', '6', 0,
  /* 1636 */ 'B', '1', '7', 0,
  /* 1640 */ 'D', '1', '4', '_', 'D', '1', '5', '_', 'D', '1', '6', '_', 'D', '1', '7', 0,
  /* 1656 */ 'H', '1', '7', 0,
  /* 1660 */ 'Q', '1', '4', '_', 'Q', '1', '5', '_', 'Q', '1', '6', '_', 'Q', '1', '7', 0,
  /* 1676 */ 'S', '1', '7', 0,
  /* 1680 */ 'W', '1', '6', '_', 'W', '1', '7', 0,
  /* 1688 */ 'X', '1', '6', '_', 'X', '1', '7', 0,
  /* 1696 */ 'Z', '1', '4', '_', 'Z', '1', '5', '_', 'Z', '1', '6', '_', 'Z', '1', '7', 0,
  /* 1712 */ 'B', '2', '7', 0,
  /* 1716 */ 'D', '2', '4', '_', 'D', '2', '5', '_', 'D', '2', '6', '_', 'D', '2', '7', 0,
  /* 1732 */ 'H', '2', '7', 0,
  /* 1736 */ 'Q', '2', '4', '_', 'Q', '2', '5', '_', 'Q', '2', '6', '_', 'Q', '2', '7', 0,
  /* 1752 */ 'S', '2', '7', 0,
  /* 1756 */ 'W', '2', '6', '_', 'W', '2', '7', 0,
  /* 1764 */ 'X', '2', '6', '_', 'X', '2', '7', 0,
  /* 1772 */ 'Z', '2', '4', '_', 'Z', '2', '5', '_', 'Z', '2', '6', '_', 'Z', '2', '7', 0,
  /* 1788 */ 'B', '7', 0,
  /* 1791 */ 'D', '4', '_', 'D', '5', '_', 'D', '6', '_', 'D', '7', 0,
  /* 1803 */ 'H', '7', 0,
  /* 1806 */ 'P', '7', 0,
  /* 1809 */ 'Q', '4', '_', 'Q', '5', '_', 'Q', '6', '_', 'Q', '7', 0,
  /* 1821 */ 'S', '7', 0,
  /* 1824 */ 'W', '6', '_', 'W', '7', 0,
  /* 1830 */ 'X', '6', '_', 'X', '7', 0,
  /* 1836 */ 'Z', '4', '_', 'Z', '5', '_', 'Z', '6', '_', 'Z', '7', 0,
  /* 1848 */ 'B', '1', '8', 0,
  /* 1852 */ 'D', '1', '5', '_', 'D', '1', '6', '_', 'D', '1', '7', '_', 'D', '1', '8', 0,
  /* 1868 */ 'H', '1', '8', 0,
  /* 1872 */ 'Q', '1', '5', '_', 'Q', '1', '6', '_', 'Q', '1', '7', '_', 'Q', '1', '8', 0,
  /* 1888 */ 'S', '1', '8', 0,
  /* 1892 */ 'W', '1', '7', '_', 'W', '1', '8', 0,
  /* 1900 */ 'X', '1', '7', '_', 'X', '1', '8', 0,
  /* 1908 */ 'Z', '1', '5', '_', 'Z', '1', '6', '_', 'Z', '1', '7', '_', 'Z', '1', '8', 0,
  /* 1924 */ 'B', '2', '8', 0,
  /* 1928 */ 'D', '2', '5', '_', 'D', '2', '6', '_', 'D', '2', '7', '_', 'D', '2', '8', 0,
  /* 1944 */ 'H', '2', '8', 0,
  /* 1948 */ 'Q', '2', '5', '_', 'Q', '2', '6', '_', 'Q', '2', '7', '_', 'Q', '2', '8', 0,
  /* 1964 */ 'S', '2', '8', 0,
  /* 1968 */ 'W', '2', '7', '_', 'W', '2', '8', 0,
  /* 1976 */ 'X', '2', '7', '_', 'X', '2', '8', 0,
  /* 1984 */ 'Z', '2', '5', '_', 'Z', '2', '6', '_', 'Z', '2', '7', '_', 'Z', '2', '8', 0,
  /* 2000 */ 'B', '8', 0,
  /* 2003 */ 'D', '5', '_', 'D', '6', '_', 'D', '7', '_', 'D', '8', 0,
  /* 2015 */ 'H', '8', 0,
  /* 2018 */ 'P', '8', 0,
  /* 2021 */ 'Q', '5', '_', 'Q', '6', '_', 'Q', '7', '_', 'Q', '8', 0,
  /* 2033 */ 'S', '8', 0,
  /* 2036 */ 'W', '7', '_', 'W', '8', 0,
  /* 2042 */ 'X', '7', '_', 'X', '8', 0,
  /* 2048 */ 'Z', '5', '_', 'Z', '6', '_', 'Z', '7', '_', 'Z', '8', 0,
  /* 2060 */ 'B', '1', '9', 0,
  /* 2064 */ 'D', '1', '6', '_', 'D', '1', '7', '_', 'D', '1', '8', '_', 'D', '1', '9', 0,
  /* 2080 */ 'H', '1', '9', 0,
  /* 2084 */ 'Q', '1', '6', '_', 'Q', '1', '7', '_', 'Q', '1', '8', '_', 'Q', '1', '9', 0,
  /* 2100 */ 'S', '1', '9', 0,
  /* 2104 */ 'W', '1', '8', '_', 'W', '1', '9', 0,
  /* 2112 */ 'X', '1', '8', '_', 'X', '1', '9', 0,
  /* 2120 */ 'Z', '1', '6', '_', 'Z', '1', '7', '_', 'Z', '1', '8', '_', 'Z', '1', '9', 0,
  /* 2136 */ 'B', '2', '9', 0,
  /* 2140 */ 'D', '2', '6', '_', 'D', '2', '7', '_', 'D', '2', '8', '_', 'D', '2', '9', 0,
  /* 2156 */ 'H', '2', '9', 0,
  /* 2160 */ 'Q', '2', '6', '_', 'Q', '2', '7', '_', 'Q', '2', '8', '_', 'Q', '2', '9', 0,
  /* 2176 */ 'S', '2', '9', 0,
  /* 2180 */ 'W', '2', '8', '_', 'W', '2', '9', 0,
  /* 2188 */ 'Z', '2', '6', '_', 'Z', '2', '7', '_', 'Z', '2', '8', '_', 'Z', '2', '9', 0,
  /* 2204 */ 'B', '9', 0,
  /* 2207 */ 'D', '6', '_', 'D', '7', '_', 'D', '8', '_', 'D', '9', 0,
  /* 2219 */ 'H', '9', 0,
  /* 2222 */ 'P', '9', 0,
  /* 2225 */ 'Q', '6', '_', 'Q', '7', '_', 'Q', '8', '_', 'Q', '9', 0,
  /* 2237 */ 'S', '9', 0,
  /* 2240 */ 'W', '8', '_', 'W', '9', 0,
  /* 2246 */ 'X', '8', '_', 'X', '9', 0,
  /* 2252 */ 'Z', '6', '_', 'Z', '7', '_', 'Z', '8', '_', 'Z', '9', 0,
  /* 2264 */ 'Z', '1', '0', '_', 'H', 'I', 0,
  /* 2271 */ 'Z', '2', '0', '_', 'H', 'I', 0,
  /* 2278 */ 'Z', '3', '0', '_', 'H', 'I', 0,
  /* 2285 */ 'Z', '0', '_', 'H', 'I', 0,
  /* 2291 */ 'Z', '1', '1', '_', 'H', 'I', 0,
  /* 2298 */ 'Z', '2', '1', '_', 'H', 'I', 0,
  /* 2305 */ 'Z', '3', '1', '_', 'H', 'I', 0,
  /* 2312 */ 'Z', '1', '_', 'H', 'I', 0,
  /* 2318 */ 'Z', '1', '2', '_', 'H', 'I', 0,
  /* 2325 */ 'Z', '2', '2', '_', 'H', 'I', 0,
  /* 2332 */ 'Z', '2', '_', 'H', 'I', 0,
  /* 2338 */ 'Z', '1', '3', '_', 'H', 'I', 0,
  /* 2345 */ 'Z', '2', '3', '_', 'H', 'I', 0,
  /* 2352 */ 'Z', '3', '_', 'H', 'I', 0,
  /* 2358 */ 'Z', '1', '4', '_', 'H', 'I', 0,
  /* 2365 */ 'Z', '2', '4', '_', 'H', 'I', 0,
  /* 2372 */ 'Z', '4', '_', 'H', 'I', 0,
  /* 2378 */ 'Z', '1', '5', '_', 'H', 'I', 0,
  /* 2385 */ 'Z', '2', '5', '_', 'H', 'I', 0,
  /* 2392 */ 'Z', '5', '_', 'H', 'I', 0,
  /* 2398 */ 'Z', '1', '6', '_', 'H', 'I', 0,
  /* 2405 */ 'Z', '2', '6', '_', 'H', 'I', 0,
  /* 2412 */ 'Z', '6', '_', 'H', 'I', 0,
  /* 2418 */ 'Z', '1', '7', '_', 'H', 'I', 0,
  /* 2425 */ 'Z', '2', '7', '_', 'H', 'I', 0,
  /* 2432 */ 'Z', '7', '_', 'H', 'I', 0,
  /* 2438 */ 'Z', '1', '8', '_', 'H', 'I', 0,
  /* 2445 */ 'Z', '2', '8', '_', 'H', 'I', 0,
  /* 2452 */ 'Z', '8', '_', 'H', 'I', 0,
  /* 2458 */ 'Z', '1', '9', '_', 'H', 'I', 0,
  /* 2465 */ 'Z', '2', '9', '_', 'H', 'I', 0,
  /* 2472 */ 'Z', '9', '_', 'H', 'I', 0,
  /* 2478 */ 'X', '2', '8', '_', 'F', 'P', 0,
  /* 2485 */ 'W', 'S', 'P', 0,
  /* 2489 */ 'F', 'F', 'R', 0,
  /* 2493 */ 'F', 'P', '_', 'L', 'R', 0,
  /* 2499 */ 'W', '3', '0', '_', 'W', 'Z', 'R', 0,
  /* 2507 */ 'L', 'R', '_', 'X', 'Z', 'R', 0,
  /* 2514 */ 'N', 'Z', 'C', 'V', 0,
};

extern const MCRegisterDesc AArch64RegDesc[] = { // Descriptors
  { 3, 0, 0, 0, 0, 0 },
  { 2489, 8, 8, 4, 20465, 0 },
  { 2482, 878, 405, 5, 20465, 27 },
  { 2496, 878, 132, 5, 20465, 27 },
  { 2514, 8, 8, 4, 20465, 0 },
  { 2486, 7, 8, 5, 6576, 27 },
  { 2485, 8, 1279, 4, 6576, 0 },
  { 2503, 8, 79, 4, 6608, 0 },
  { 2510, 1279, 129, 5, 6608, 27 },
  { 213, 8, 214, 4, 20433, 0 },
  { 494, 8, 296, 4, 20433, 0 },
  { 713, 8, 438, 4, 20433, 0 },
  { 932, 8, 150, 4, 20433, 0 },
  { 1148, 8, 150, 4, 20433, 0 },
  { 1364, 8, 150, 4, 20433, 0 },
  { 1576, 8, 150, 4, 20433, 0 },
  { 1788, 8, 150, 4, 20433, 0 },
  { 2000, 8, 150, 4, 20433, 0 },
  { 2204, 8, 150, 4, 20433, 0 },
  { 0, 8, 150, 4, 20433, 0 },
  { 284, 8, 150, 4, 20433, 0 },
  { 560, 8, 150, 4, 20433, 0 },
  { 776, 8, 150, 4, 20433, 0 },
  { 992, 8, 150, 4, 20433, 0 },
  { 1208, 8, 150, 4, 20433, 0 },
  { 1424, 8, 150, 4, 20433, 0 },
  { 1636, 8, 150, 4, 20433, 0 },
  { 1848, 8, 150, 4, 20433, 0 },
  { 2060, 8, 150, 4, 20433, 0 },
  { 69, 8, 150, 4, 20433, 0 },
  { 358, 8, 150, 4, 20433, 0 },
  { 637, 8, 150, 4, 20433, 0 },
  { 856, 8, 150, 4, 20433, 0 },
  { 1072, 8, 150, 4, 20433, 0 },
  { 1288, 8, 150, 4, 20433, 0 },
  { 1500, 8, 150, 4, 20433, 0 },
  { 1712, 8, 150, 4, 20433, 0 },
  { 1924, 8, 150, 4, 20433, 0 },
  { 2136, 8, 150, 4, 20433, 0 },
  { 145, 8, 150, 4, 20433, 0 },
  { 434, 8, 150, 4, 20433, 0 },
  { 228, 1080, 217, 1, 20161, 3 },
  { 508, 1080, 299, 1, 20161, 3 },
  { 726, 1080, 441, 1, 20161, 3 },
  { 944, 1080, 153, 1, 20161, 3 },
  { 1160, 1080, 153, 1, 20161, 3 },
  { 1376, 1080, 153, 1, 20161, 3 },
  { 1588, 1080, 153, 1, 20161, 3 },
  { 1800, 1080, 153, 1, 20161, 3 },
  { 2012, 1080, 153, 1, 20161, 3 },
  { 2216, 1080, 153, 1, 20161, 3 },
  { 13, 1080, 153, 1, 20161, 3 },
  { 298, 1080, 153, 1, 20161, 3 },
  { 575, 1080, 153, 1, 20161, 3 },
  { 792, 1080, 153, 1, 20161, 3 },
  { 1008, 1080, 153, 1, 20161, 3 },
  { 1224, 1080, 153, 1, 20161, 3 },
  { 1440, 1080, 153, 1, 20161, 3 },
  { 1652, 1080, 153, 1, 20161, 3 },
  { 1864, 1080, 153, 1, 20161, 3 },
  { 2076, 1080, 153, 1, 20161, 3 },
  { 85, 1080, 153, 1, 20161, 3 },
  { 374, 1080, 153, 1, 20161, 3 },
  { 653, 1080, 153, 1, 20161, 3 },
  { 872, 1080, 153, 1, 20161, 3 },
  { 1088, 1080, 153, 1, 20161, 3 },
  { 1304, 1080, 153, 1, 20161, 3 },
  { 1516, 1080, 153, 1, 20161, 3 },
  { 1728, 1080, 153, 1, 20161, 3 },
  { 1940, 1080, 153, 1, 20161, 3 },
  { 2152, 1080, 153, 1, 20161, 3 },
  { 161, 1080, 153, 1, 20161, 3 },
  { 450, 1080, 153, 1, 20161, 3 },
  { 231, 1082, 215, 3, 17169, 3 },
  { 511, 1082, 297, 3, 17169, 3 },
  { 729, 1082, 439, 3, 17169, 3 },
  { 947, 1082, 151, 3, 17169, 3 },
  { 1163, 1082, 151, 3, 17169, 3 },
  { 1379, 1082, 151, 3, 17169, 3 },
  { 1591, 1082, 151, 3, 17169, 3 },
  { 1803, 1082, 151, 3, 17169, 3 },
  { 2015, 1082, 151, 3, 17169, 3 },
  { 2219, 1082, 151, 3, 17169, 3 },
  { 17, 1082, 151, 3, 17169, 3 },
  { 302, 1082, 151, 3, 17169, 3 },
  { 579, 1082, 151, 3, 17169, 3 },
  { 796, 1082, 151, 3, 17169, 3 },
  { 1012, 1082, 151, 3, 17169, 3 },
  { 1228, 1082, 151, 3, 17169, 3 },
  { 1444, 1082, 151, 3, 17169, 3 },
  { 1656, 1082, 151, 3, 17169, 3 },
  { 1868, 1082, 151, 3, 17169, 3 },
  { 2080, 1082, 151, 3, 17169, 3 },
  { 89, 1082, 151, 3, 17169, 3 },
  { 378, 1082, 151, 3, 17169, 3 },
  { 657, 1082, 151, 3, 17169, 3 },
  { 876, 1082, 151, 3, 17169, 3 },
  { 1092, 1082, 151, 3, 17169, 3 },
  { 1308, 1082, 151, 3, 17169, 3 },
  { 1520, 1082, 151, 3, 17169, 3 },
  { 1732, 1082, 151, 3, 17169, 3 },
  { 1944, 1082, 151, 3, 17169, 3 },
  { 2156, 1082, 151, 3, 17169, 3 },
  { 165, 1082, 151, 3, 17169, 3 },
  { 454, 1082, 151, 3, 17169, 3 },
  { 234, 8, 8, 4, 17169, 0 },
  { 514, 8, 8, 4, 17169, 0 },
  { 732, 8, 8, 4, 17169, 0 },
  { 950, 8, 8, 4, 17169, 0 },
  { 1166, 8, 8, 4, 17169, 0 },
  { 1382, 8, 8, 4, 17169, 0 },
  { 1594, 8, 8, 4, 17169, 0 },
  { 1806, 8, 8, 4, 17169, 0 },
  { 2018, 8, 8, 4, 17169, 0 },
  { 2222, 8, 8, 4, 17169, 0 },
  { 21, 8, 8, 4, 17169, 0 },
  { 306, 8, 8, 4, 17169, 0 },
  { 583, 8, 8, 4, 17169, 0 },
  { 800, 8, 8, 4, 17169, 0 },
  { 1016, 8, 8, 4, 17169, 0 },
  { 1232, 8, 8, 4, 17169, 0 },
  { 249, 1093, 247, 0, 15265, 3 },
  { 528, 1093, 329, 0, 15265, 3 },
  { 745, 1093, 471, 0, 15265, 3 },
  { 962, 1093, 183, 0, 15265, 3 },
  { 1178, 1093, 183, 0, 15265, 3 },
  { 1394, 1093, 183, 0, 15265, 3 },
  { 1606, 1093, 183, 0, 15265, 3 },
  { 1818, 1093, 183, 0, 15265, 3 },
  { 2030, 1093, 183, 0, 15265, 3 },
  { 2234, 1093, 183, 0, 15265, 3 },
  { 34, 1093, 183, 0, 15265, 3 },
  { 320, 1093, 183, 0, 15265, 3 },
  { 598, 1093, 183, 0, 15265, 3 },
  { 816, 1093, 183, 0, 15265, 3 },
  { 1032, 1093, 183, 0, 15265, 3 },
  { 1248, 1093, 183, 0, 15265, 3 },
  { 1460, 1093, 183, 0, 15265, 3 },
  { 1672, 1093, 183, 0, 15265, 3 },
  { 1884, 1093, 183, 0, 15265, 3 },
  { 2096, 1093, 183, 0, 15265, 3 },
  { 105, 1093, 183, 0, 15265, 3 },
  { 394, 1093, 183, 0, 15265, 3 },
  { 673, 1093, 183, 0, 15265, 3 },
  { 892, 1093, 183, 0, 15265, 3 },
  { 1108, 1093, 183, 0, 15265, 3 },
  { 1324, 1093, 183, 0, 15265, 3 },
  { 1536, 1093, 183, 0, 15265, 3 },
  { 1748, 1093, 183, 0, 15265, 3 },
  { 1960, 1093, 183, 0, 15265, 3 },
  { 2172, 1093, 183, 0, 15265, 3 },
  { 181, 1093, 183, 0, 15265, 3 },
  { 470, 1093, 183, 0, 15265, 3 },
  { 252, 1081, 216, 2, 15201, 3 },
  { 531, 1081, 298, 2, 15201, 3 },
  { 748, 1081, 440, 2, 15201, 3 },
  { 965, 1081, 152, 2, 15201, 3 },
  { 1181, 1081, 152, 2, 15201, 3 },
  { 1397, 1081, 152, 2, 15201, 3 },
  { 1609, 1081, 152, 2, 15201, 3 },
  { 1821, 1081, 152, 2, 15201, 3 },
  { 2033, 1081, 152, 2, 15201, 3 },
  { 2237, 1081, 152, 2, 15201, 3 },
  { 38, 1081, 152, 2, 15201, 3 },
  { 324, 1081, 152, 2, 15201, 3 },
  { 602, 1081, 152, 2, 15201, 3 },
  { 820, 1081, 152, 2, 15201, 3 },
  { 1036, 1081, 152, 2, 15201, 3 },
  { 1252, 1081, 152, 2, 15201, 3 },
  { 1464, 1081, 152, 2, 15201, 3 },
  { 1676, 1081, 152, 2, 15201, 3 },
  { 1888, 1081, 152, 2, 15201, 3 },
  { 2100, 1081, 152, 2, 15201, 3 },
  { 109, 1081, 152, 2, 15201, 3 },
  { 398, 1081, 152, 2, 15201, 3 },
  { 677, 1081, 152, 2, 15201, 3 },
  { 896, 1081, 152, 2, 15201, 3 },
  { 1112, 1081, 152, 2, 15201, 3 },
  { 1328, 1081, 152, 2, 15201, 3 },
  { 1540, 1081, 152, 2, 15201, 3 },
  { 1752, 1081, 152, 2, 15201, 3 },
  { 1964, 1081, 152, 2, 15201, 3 },
  { 2176, 1081, 152, 2, 15201, 3 },
  { 185, 1081, 152, 2, 15201, 3 },
  { 474, 1081, 152, 2, 15201, 3 },
  { 259, 8, 387, 4, 15233, 0 },
  { 537, 8, 85, 4, 15233, 0 },
  { 754, 8, 85, 4, 15233, 0 },
  { 971, 8, 85, 4, 15233, 0 },
  { 1187, 8, 85, 4, 15233, 0 },
  { 1403, 8, 85, 4, 15233, 0 },
  { 1615, 8, 85, 4, 15233, 0 },
  { 1827, 8, 85, 4, 15233, 0 },
  { 2039, 8, 85, 4, 15233, 0 },
  { 2243, 8, 85, 4, 15233, 0 },
  { 45, 8, 85, 4, 15233, 0 },
  { 332, 8, 85, 4, 15233, 0 },
  { 610, 8, 85, 4, 15233, 0 },
  { 828, 8, 85, 4, 15233, 0 },
  { 1044, 8, 85, 4, 15233, 0 },
  { 1260, 8, 85, 4, 15233, 0 },
  { 1472, 8, 85, 4, 15233, 0 },
  { 1684, 8, 85, 4, 15233, 0 },
  { 1896, 8, 85, 4, 15233, 0 },
  { 2108, 8, 85, 4, 15233, 0 },
  { 117, 8, 85, 4, 15233, 0 },
  { 406, 8, 85, 4, 15233, 0 },
  { 685, 8, 85, 4, 15233, 0 },
  { 904, 8, 85, 4, 15233, 0 },
  { 1120, 8, 85, 4, 15233, 0 },
  { 1336, 8, 85, 4, 15233, 0 },
  { 1548, 8, 85, 4, 15233, 0 },
  { 1760, 8, 85, 4, 15233, 0 },
  { 1972, 8, 415, 4, 15233, 0 },
  { 2184, 8, 396, 4, 15057, 0 },
  { 193, 8, 33, 4, 15057, 0 },
  { 266, 1275, 393, 5, 15169, 27 },
  { 543, 1275, 111, 5, 15169, 27 },
  { 760, 1275, 111, 5, 15169, 27 },
  { 977, 1275, 111, 5, 15169, 27 },
  { 1193, 1275, 111, 5, 15169, 27 },
  { 1409, 1275, 111, 5, 15169, 27 },
  { 1621, 1275, 111, 5, 15169, 27 },
  { 1833, 1275, 111, 5, 15169, 27 },
  { 2045, 1275, 111, 5, 15169, 27 },
  { 2249, 1275, 111, 5, 15169, 27 },
  { 52, 1275, 111, 5, 15169, 27 },
  { 340, 1275, 111, 5, 15169, 27 },
  { 618, 1275, 111, 5, 15169, 27 },
  { 836, 1275, 111, 5, 15169, 27 },
  { 1052, 1275, 111, 5, 15169, 27 },
  { 1268, 1275, 111, 5, 15169, 27 },
  { 1480, 1275, 111, 5, 15169, 27 },
  { 1692, 1275, 111, 5, 15169, 27 },
  { 1904, 1275, 111, 5, 15169, 27 },
  { 2116, 1275, 111, 5, 15169, 27 },
  { 125, 1275, 111, 5, 15169, 27 },
  { 414, 1275, 111, 5, 15169, 27 },
  { 693, 1275, 111, 5, 15169, 27 },
  { 912, 1275, 111, 5, 15169, 27 },
  { 1128, 1275, 111, 5, 15169, 27 },
  { 1344, 1275, 111, 5, 15169, 27 },
  { 1556, 1275, 111, 5, 15169, 27 },
  { 1768, 1275, 111, 5, 15169, 27 },
  { 1980, 1275, 421, 5, 15169, 27 },
  { 281, 880, 268, 10, 8929, 35 },
  { 557, 880, 350, 10, 8929, 35 },
  { 773, 880, 492, 10, 8929, 35 },
  { 989, 880, 204, 10, 8929, 35 },
  { 1205, 880, 204, 10, 8929, 35 },
  { 1421, 880, 204, 10, 8929, 35 },
  { 1633, 880, 204, 10, 8929, 35 },
  { 1845, 880, 204, 10, 8929, 35 },
  { 2057, 880, 204, 10, 8929, 35 },
  { 2261, 880, 204, 10, 8929, 35 },
  { 65, 880, 204, 10, 8929, 35 },
  { 354, 880, 204, 10, 8929, 35 },
  { 633, 880, 204, 10, 8929, 35 },
  { 852, 880, 204, 10, 8929, 35 },
  { 1068, 880, 204, 10, 8929, 35 },
  { 1284, 880, 204, 10, 8929, 35 },
  { 1496, 880, 204, 10, 8929, 35 },
  { 1708, 880, 204, 10, 8929, 35 },
  { 1920, 880, 204, 10, 8929, 35 },
  { 2132, 880, 204, 10, 8929, 35 },
  { 141, 880, 204, 10, 8929, 35 },
  { 430, 880, 204, 10, 8929, 35 },
  { 709, 880, 204, 10, 8929, 35 },
  { 928, 880, 204, 10, 8929, 35 },
  { 1144, 880, 204, 10, 8929, 35 },
  { 1360, 880, 204, 10, 8929, 35 },
  { 1572, 880, 204, 10, 8929, 35 },
  { 1784, 880, 204, 10, 8929, 35 },
  { 1996, 880, 204, 10, 8929, 35 },
  { 2200, 880, 204, 10, 8929, 35 },
  { 209, 880, 204, 10, 8929, 35 },
  { 490, 880, 204, 10, 8929, 35 },
  { 2285, 8, 267, 4, 15137, 0 },
  { 2312, 8, 349, 4, 15137, 0 },
  { 2332, 8, 491, 4, 15137, 0 },
  { 2352, 8, 203, 4, 15137, 0 },
  { 2372, 8, 203, 4, 15137, 0 },
  { 2392, 8, 203, 4, 15137, 0 },
  { 2412, 8, 203, 4, 15137, 0 },
  { 2432, 8, 203, 4, 15137, 0 },
  { 2452, 8, 203, 4, 15137, 0 },
  { 2472, 8, 203, 4, 15137, 0 },
  { 2264, 8, 203, 4, 15137, 0 },
  { 2291, 8, 203, 4, 15137, 0 },
  { 2318, 8, 203, 4, 15137, 0 },
  { 2338, 8, 203, 4, 15137, 0 },
  { 2358, 8, 203, 4, 15137, 0 },
  { 2378, 8, 203, 4, 15137, 0 },
  { 2398, 8, 203, 4, 15137, 0 },
  { 2418, 8, 203, 4, 15137, 0 },
  { 2438, 8, 203, 4, 15137, 0 },
  { 2458, 8, 203, 4, 15137, 0 },
  { 2271, 8, 203, 4, 15137, 0 },
  { 2298, 8, 203, 4, 15137, 0 },
  { 2325, 8, 203, 4, 15137, 0 },
  { 2345, 8, 203, 4, 15137, 0 },
  { 2365, 8, 203, 4, 15137, 0 },
  { 2385, 8, 203, 4, 15137, 0 },
  { 2405, 8, 203, 4, 15137, 0 },
  { 2425, 8, 203, 4, 15137, 0 },
  { 2445, 8, 203, 4, 15137, 0 },
  { 2465, 8, 203, 4, 15137, 0 },
  { 2278, 8, 203, 4, 15137, 0 },
  { 2305, 8, 203, 4, 15137, 0 },
  { 505, 1084, 360, 17, 2353, 61 },
  { 723, 1084, 513, 17, 2353, 61 },
  { 941, 1084, 278, 17, 2353, 61 },
  { 1157, 1084, 278, 17, 2353, 61 },
  { 1373, 1084, 278, 17, 2353, 61 },
  { 1585, 1084, 278, 17, 2353, 61 },
  { 1797, 1084, 278, 17, 2353, 61 },
  { 2009, 1084, 278, 17, 2353, 61 },
  { 2213, 1084, 278, 17, 2353, 61 },
  { 10, 1084, 278, 17, 2353, 61 },
  { 294, 1084, 278, 17, 2353, 61 },
  { 571, 1084, 278, 17, 2353, 61 },
  { 788, 1084, 278, 17, 2353, 61 },
  { 1004, 1084, 278, 17, 2353, 61 },
  { 1220, 1084, 278, 17, 2353, 61 },
  { 1436, 1084, 278, 17, 2353, 61 },
  { 1648, 1084, 278, 17, 2353, 61 },
  { 1860, 1084, 278, 17, 2353, 61 },
  { 2072, 1084, 278, 17, 2353, 61 },
  { 81, 1084, 278, 17, 2353, 61 },
  { 370, 1084, 278, 17, 2353, 61 },
  { 649, 1084, 278, 17, 2353, 61 },
  { 868, 1084, 278, 17, 2353, 61 },
  { 1084, 1084, 278, 17, 2353, 61 },
  { 1300, 1084, 278, 17, 2353, 61 },
  { 1512, 1084, 278, 17, 2353, 61 },
  { 1724, 1084, 278, 17, 2353, 61 },
  { 1936, 1084, 278, 17, 2353, 61 },
  { 2148, 1084, 278, 17, 2353, 61 },
  { 157, 1084, 278, 17, 2353, 61 },
  { 446, 1084, 278, 17, 2353, 61 },
  { 224, 1075, 278, 17, 8496, 2 },
  { 935, 1216, 872, 41, 225, 68 },
  { 1151, 1216, 872, 41, 225, 68 },
  { 1367, 1216, 872, 41, 225, 68 },
  { 1579, 1216, 872, 41, 225, 68 },
  { 1791, 1216, 872, 41, 225, 68 },
  { 2003, 1216, 872, 41, 225, 68 },
  { 2207, 1216, 872, 41, 225, 68 },
  { 4, 1216, 872, 41, 225, 68 },
  { 288, 1216, 872, 41, 225, 68 },
  { 564, 1216, 872, 41, 225, 68 },
  { 780, 1216, 872, 41, 225, 68 },
  { 996, 1216, 872, 41, 225, 68 },
  { 1212, 1216, 872, 41, 225, 68 },
  { 1428, 1216, 872, 41, 225, 68 },
  { 1640, 1216, 872, 41, 225, 68 },
  { 1852, 1216, 872, 41, 225, 68 },
  { 2064, 1216, 872, 41, 225, 68 },
  { 73, 1216, 872, 41, 225, 68 },
  { 362, 1216, 872, 41, 225, 68 },
  { 641, 1216, 872, 41, 225, 68 },
  { 860, 1216, 872, 41, 225, 68 },
  { 1076, 1216, 872, 41, 225, 68 },
  { 1292, 1216, 872, 41, 225, 68 },
  { 1504, 1216, 872, 41, 225, 68 },
  { 1716, 1216, 872, 41, 225, 68 },
  { 1928, 1216, 872, 41, 225, 68 },
  { 2140, 1216, 872, 41, 225, 68 },
  { 149, 1216, 872, 41, 225, 68 },
  { 438, 1216, 872, 41, 225, 68 },
  { 216, 1238, 872, 41, 304, 73 },
  { 497, 1051, 872, 41, 864, 59 },
  { 716, 1194, 872, 41, 6784, 5 },
  { 720, 96, 539, 26, 801, 74 },
  { 938, 96, 378, 26, 801, 74 },
  { 1154, 96, 378, 26, 801, 74 },
  { 1370, 96, 378, 26, 801, 74 },
  { 1582, 96, 378, 26, 801, 74 },
  { 1794, 96, 378, 26, 801, 74 },
  { 2006, 96, 378, 26, 801, 74 },
  { 2210, 96, 378, 26, 801, 74 },
  { 7, 96, 378, 26, 801, 74 },
  { 291, 96, 378, 26, 801, 74 },
  { 567, 96, 378, 26, 801, 74 },
  { 784, 96, 378, 26, 801, 74 },
  { 1000, 96, 378, 26, 801, 74 },
  { 1216, 96, 378, 26, 801, 74 },
  { 1432, 96, 378, 26, 801, 74 },
  { 1644, 96, 378, 26, 801, 74 },
  { 1856, 96, 378, 26, 801, 74 },
  { 2068, 96, 378, 26, 801, 74 },
  { 77, 96, 378, 26, 801, 74 },
  { 366, 96, 378, 26, 801, 74 },
  { 645, 96, 378, 26, 801, 74 },
  { 864, 96, 378, 26, 801, 74 },
  { 1080, 96, 378, 26, 801, 74 },
  { 1296, 96, 378, 26, 801, 74 },
  { 1508, 96, 378, 26, 801, 74 },
  { 1720, 96, 378, 26, 801, 74 },
  { 1932, 96, 378, 26, 801, 74 },
  { 2144, 96, 378, 26, 801, 74 },
  { 153, 96, 378, 26, 801, 74 },
  { 442, 96, 378, 26, 801, 74 },
  { 220, 114, 378, 26, 1088, 64 },
  { 501, 1262, 378, 26, 8032, 10 },
  { 525, 887, 366, 63, 2257, 80 },
  { 742, 887, 519, 63, 2257, 80 },
  { 959, 887, 284, 63, 2257, 80 },
  { 1175, 887, 284, 63, 2257, 80 },
  { 1391, 887, 284, 63, 2257, 80 },
  { 1603, 887, 284, 63, 2257, 80 },
  { 1815, 887, 284, 63, 2257, 80 },
  { 2027, 887, 284, 63, 2257, 80 },
  { 2231, 887, 284, 63, 2257, 80 },
  { 31, 887, 284, 63, 2257, 80 },
  { 316, 887, 284, 63, 2257, 80 },
  { 594, 887, 284, 63, 2257, 80 },
  { 812, 887, 284, 63, 2257, 80 },
  { 1028, 887, 284, 63, 2257, 80 },
  { 1244, 887, 284, 63, 2257, 80 },
  { 1456, 887, 284, 63, 2257, 80 },
  { 1668, 887, 284, 63, 2257, 80 },
  { 1880, 887, 284, 63, 2257, 80 },
  { 2092, 887, 284, 63, 2257, 80 },
  { 101, 887, 284, 63, 2257, 80 },
  { 390, 887, 284, 63, 2257, 80 },
  { 669, 887, 284, 63, 2257, 80 },
  { 888, 887, 284, 63, 2257, 80 },
  { 1104, 887, 284, 63, 2257, 80 },
  { 1320, 887, 284, 63, 2257, 80 },
  { 1532, 887, 284, 63, 2257, 80 },
  { 1744, 887, 284, 63, 2257, 80 },
  { 1956, 887, 284, 63, 2257, 80 },
  { 2168, 887, 284, 63, 2257, 80 },
  { 177, 887, 284, 63, 2257, 80 },
  { 466, 887, 284, 63, 2257, 80 },
  { 245, 923, 284, 63, 8496, 14 },
  { 953, 1130, 873, 96, 145, 87 },
  { 1169, 1130, 873, 96, 145, 87 },
  { 1385, 1130, 873, 96, 145, 87 },
  { 1597, 1130, 873, 96, 145, 87 },
  { 1809, 1130, 873, 96, 145, 87 },
  { 2021, 1130, 873, 96, 145, 87 },
  { 2225, 1130, 873, 96, 145, 87 },
  { 25, 1130, 873, 96, 145, 87 },
  { 310, 1130, 873, 96, 145, 87 },
  { 587, 1130, 873, 96, 145, 87 },
  { 804, 1130, 873, 96, 145, 87 },
  { 1020, 1130, 873, 96, 145, 87 },
  { 1236, 1130, 873, 96, 145, 87 },
  { 1448, 1130, 873, 96, 145, 87 },
  { 1660, 1130, 873, 96, 145, 87 },
  { 1872, 1130, 873, 96, 145, 87 },
  { 2084, 1130, 873, 96, 145, 87 },
  { 93, 1130, 873, 96, 145, 87 },
  { 382, 1130, 873, 96, 145, 87 },
  { 661, 1130, 873, 96, 145, 87 },
  { 880, 1130, 873, 96, 145, 87 },
  { 1096, 1130, 873, 96, 145, 87 },
  { 1312, 1130, 873, 96, 145, 87 },
  { 1524, 1130, 873, 96, 145, 87 },
  { 1736, 1130, 873, 96, 145, 87 },
  { 1948, 1130, 873, 96, 145, 87 },
  { 2160, 1130, 873, 96, 145, 87 },
  { 169, 1130, 873, 96, 145, 87 },
  { 458, 1130, 873, 96, 145, 87 },
  { 237, 1162, 873, 96, 304, 92 },
  { 517, 1019, 873, 96, 864, 78 },
  { 735, 1098, 873, 96, 6784, 17 },
  { 739, 956, 542, 75, 737, 93 },
  { 956, 956, 381, 75, 737, 93 },
  { 1172, 956, 381, 75, 737, 93 },
  { 1388, 956, 381, 75, 737, 93 },
  { 1600, 956, 381, 75, 737, 93 },
  { 1812, 956, 381, 75, 737, 93 },
  { 2024, 956, 381, 75, 737, 93 },
  { 2228, 956, 381, 75, 737, 93 },
  { 28, 956, 381, 75, 737, 93 },
  { 313, 956, 381, 75, 737, 93 },
  { 590, 956, 381, 75, 737, 93 },
  { 808, 956, 381, 75, 737, 93 },
  { 1024, 956, 381, 75, 737, 93 },
  { 1240, 956, 381, 75, 737, 93 },
  { 1452, 956, 381, 75, 737, 93 },
  { 1664, 956, 381, 75, 737, 93 },
  { 1876, 956, 381, 75, 737, 93 },
  { 2088, 956, 381, 75, 737, 93 },
  { 97, 956, 381, 75, 737, 93 },
  { 386, 956, 381, 75, 737, 93 },
  { 665, 956, 381, 75, 737, 93 },
  { 884, 956, 381, 75, 737, 93 },
  { 1100, 956, 381, 75, 737, 93 },
  { 1316, 956, 381, 75, 737, 93 },
  { 1528, 956, 381, 75, 737, 93 },
  { 1740, 956, 381, 75, 737, 93 },
  { 1952, 956, 381, 75, 737, 93 },
  { 2164, 956, 381, 75, 737, 93 },
  { 173, 956, 381, 75, 737, 93 },
  { 462, 956, 381, 75, 737, 93 },
  { 241, 977, 381, 75, 1088, 83 },
  { 521, 998, 381, 75, 8032, 22 },
  { 255, 875, 550, 7, 8832, 32 },
  { 2499, 943, 548, 7, 6432, 32 },
  { 534, 144, 550, 7, 2209, 32 },
  { 751, 144, 550, 7, 2209, 32 },
  { 968, 144, 550, 7, 2209, 32 },
  { 1184, 144, 550, 7, 2209, 32 },
  { 1400, 144, 550, 7, 2209, 32 },
  { 1612, 144, 550, 7, 2209, 32 },
  { 1824, 144, 550, 7, 2209, 32 },
  { 2036, 144, 550, 7, 2209, 32 },
  { 2240, 144, 550, 7, 2209, 32 },
  { 42, 144, 550, 7, 2209, 32 },
  { 328, 144, 550, 7, 2209, 32 },
  { 606, 144, 550, 7, 2209, 32 },
  { 824, 144, 550, 7, 2209, 32 },
  { 1040, 144, 550, 7, 2209, 32 },
  { 1256, 144, 550, 7, 2209, 32 },
  { 1468, 144, 550, 7, 2209, 32 },
  { 1680, 144, 550, 7, 2209, 32 },
  { 1892, 144, 550, 7, 2209, 32 },
  { 2104, 144, 550, 7, 2209, 32 },
  { 113, 144, 550, 7, 2209, 32 },
  { 402, 144, 550, 7, 2209, 32 },
  { 681, 144, 550, 7, 2209, 32 },
  { 900, 144, 550, 7, 2209, 32 },
  { 1116, 144, 550, 7, 2209, 32 },
  { 1332, 144, 550, 7, 2209, 32 },
  { 1544, 144, 550, 7, 2209, 32 },
  { 1756, 144, 550, 7, 2209, 32 },
  { 1968, 144, 550, 7, 2209, 32 },
  { 2180, 144, 413, 7, 8976, 29 },
  { 189, 144, 7, 7, 96, 32 },
  { 2493, 905, 8, 128, 96, 97 },
  { 2507, 935, 8, 128, 6529, 97 },
  { 262, 899, 8, 128, 8883, 97 },
  { 2478, 911, 8, 128, 8976, 26 },
  { 540, 917, 8, 128, 2161, 97 },
  { 757, 917, 8, 128, 2161, 97 },
  { 974, 917, 8, 128, 2161, 97 },
  { 1190, 917, 8, 128, 2161, 97 },
  { 1406, 917, 8, 128, 2161, 97 },
  { 1618, 917, 8, 128, 2161, 97 },
  { 1830, 917, 8, 128, 2161, 97 },
  { 2042, 917, 8, 128, 2161, 97 },
  { 2246, 917, 8, 128, 2161, 97 },
  { 49, 917, 8, 128, 2161, 97 },
  { 336, 917, 8, 128, 2161, 97 },
  { 614, 917, 8, 128, 2161, 97 },
  { 832, 917, 8, 128, 2161, 97 },
  { 1048, 917, 8, 128, 2161, 97 },
  { 1264, 917, 8, 128, 2161, 97 },
  { 1476, 917, 8, 128, 2161, 97 },
  { 1688, 917, 8, 128, 2161, 97 },
  { 1900, 917, 8, 128, 2161, 97 },
  { 2112, 917, 8, 128, 2161, 97 },
  { 121, 917, 8, 128, 2161, 97 },
  { 410, 917, 8, 128, 2161, 97 },
  { 689, 917, 8, 128, 2161, 97 },
  { 908, 917, 8, 128, 2161, 97 },
  { 1124, 917, 8, 128, 2161, 97 },
  { 1340, 917, 8, 128, 2161, 97 },
  { 1552, 917, 8, 128, 2161, 97 },
  { 1764, 917, 8, 128, 2161, 97 },
  { 1976, 917, 8, 128, 2161, 97 },
  { 554, 564, 372, 134, 1457, 100 },
  { 770, 564, 525, 134, 1457, 100 },
  { 986, 564, 290, 134, 1457, 100 },
  { 1202, 564, 290, 134, 1457, 100 },
  { 1418, 564, 290, 134, 1457, 100 },
  { 1630, 564, 290, 134, 1457, 100 },
  { 1842, 564, 290, 134, 1457, 100 },
  { 2054, 564, 290, 134, 1457, 100 },
  { 2258, 564, 290, 134, 1457, 100 },
  { 62, 564, 290, 134, 1457, 100 },
  { 350, 564, 290, 134, 1457, 100 },
  { 629, 564, 290, 134, 1457, 100 },
  { 848, 564, 290, 134, 1457, 100 },
  { 1064, 564, 290, 134, 1457, 100 },
  { 1280, 564, 290, 134, 1457, 100 },
  { 1492, 564, 290, 134, 1457, 100 },
  { 1704, 564, 290, 134, 1457, 100 },
  { 1916, 564, 290, 134, 1457, 100 },
  { 2128, 564, 290, 134, 1457, 100 },
  { 137, 564, 290, 134, 1457, 100 },
  { 426, 564, 290, 134, 1457, 100 },
  { 705, 564, 290, 134, 1457, 100 },
  { 924, 564, 290, 134, 1457, 100 },
  { 1140, 564, 290, 134, 1457, 100 },
  { 1356, 564, 290, 134, 1457, 100 },
  { 1568, 564, 290, 134, 1457, 100 },
  { 1780, 564, 290, 134, 1457, 100 },
  { 1992, 564, 290, 134, 1457, 100 },
  { 2196, 564, 290, 134, 1457, 100 },
  { 205, 564, 290, 134, 1457, 100 },
  { 486, 564, 290, 134, 1457, 100 },
  { 277, 581, 290, 134, 8544, 38 },
  { 980, 780, 8, 181, 1, 121 },
  { 1196, 780, 8, 181, 1, 121 },
  { 1412, 780, 8, 181, 1, 121 },
  { 1624, 780, 8, 181, 1, 121 },
  { 1836, 780, 8, 181, 1, 121 },
  { 2048, 780, 8, 181, 1, 121 },
  { 2252, 780, 8, 181, 1, 121 },
  { 56, 780, 8, 181, 1, 121 },
  { 344, 780, 8, 181, 1, 121 },
  { 622, 780, 8, 181, 1, 121 },
  { 840, 780, 8, 181, 1, 121 },
  { 1056, 780, 8, 181, 1, 121 },
  { 1272, 780, 8, 181, 1, 121 },
  { 1484, 780, 8, 181, 1, 121 },
  { 1696, 780, 8, 181, 1, 121 },
  { 1908, 780, 8, 181, 1, 121 },
  { 2120, 780, 8, 181, 1, 121 },
  { 129, 780, 8, 181, 1, 121 },
  { 418, 780, 8, 181, 1, 121 },
  { 697, 780, 8, 181, 1, 121 },
  { 916, 780, 8, 181, 1, 121 },
  { 1132, 780, 8, 181, 1, 121 },
  { 1348, 780, 8, 181, 1, 121 },
  { 1560, 780, 8, 181, 1, 121 },
  { 1772, 780, 8, 181, 1, 121 },
  { 1984, 780, 8, 181, 1, 121 },
  { 2188, 780, 8, 181, 1, 121 },
  { 197, 780, 8, 181, 1, 121 },
  { 478, 780, 8, 181, 1, 121 },
  { 269, 826, 8, 181, 384, 130 },
  { 546, 688, 8, 181, 944, 105 },
  { 763, 734, 8, 181, 6864, 43 },
  { 767, 598, 545, 151, 625, 139 },
  { 983, 598, 180, 151, 625, 139 },
  { 1199, 598, 180, 151, 625, 139 },
  { 1415, 598, 180, 151, 625, 139 },
  { 1627, 598, 180, 151, 625, 139 },
  { 1839, 598, 180, 151, 625, 139 },
  { 2051, 598, 180, 151, 625, 139 },
  { 2255, 598, 180, 151, 625, 139 },
  { 59, 598, 180, 151, 625, 139 },
  { 347, 598, 180, 151, 625, 139 },
  { 625, 598, 180, 151, 625, 139 },
  { 844, 598, 180, 151, 625, 139 },
  { 1060, 598, 180, 151, 625, 139 },
  { 1276, 598, 180, 151, 625, 139 },
  { 1488, 598, 180, 151, 625, 139 },
  { 1700, 598, 180, 151, 625, 139 },
  { 1912, 598, 180, 151, 625, 139 },
  { 2124, 598, 180, 151, 625, 139 },
  { 133, 598, 180, 151, 625, 139 },
  { 422, 598, 180, 151, 625, 139 },
  { 701, 598, 180, 151, 625, 139 },
  { 920, 598, 180, 151, 625, 139 },
  { 1136, 598, 180, 151, 625, 139 },
  { 1352, 598, 180, 151, 625, 139 },
  { 1564, 598, 180, 151, 625, 139 },
  { 1776, 598, 180, 151, 625, 139 },
  { 1988, 598, 180, 151, 625, 139 },
  { 2192, 598, 180, 151, 625, 139 },
  { 201, 598, 180, 151, 625, 139 },
  { 482, 598, 180, 151, 625, 139 },
  { 273, 628, 180, 151, 1152, 114 },
  { 550, 658, 180, 151, 8096, 52 },
};

extern const MCPhysReg AArch64RegUnitRoots[][2] = {
  { AArch64::FFR },
  { AArch64::W29 },
  { AArch64::W30 },
  { AArch64::NZCV },
  { AArch64::WSP },
  { AArch64::WZR },
  { AArch64::B0 },
  { AArch64::B1 },
  { AArch64::B2 },
  { AArch64::B3 },
  { AArch64::B4 },
  { AArch64::B5 },
  { AArch64::B6 },
  { AArch64::B7 },
  { AArch64::B8 },
  { AArch64::B9 },
  { AArch64::B10 },
  { AArch64::B11 },
  { AArch64::B12 },
  { AArch64::B13 },
  { AArch64::B14 },
  { AArch64::B15 },
  { AArch64::B16 },
  { AArch64::B17 },
  { AArch64::B18 },
  { AArch64::B19 },
  { AArch64::B20 },
  { AArch64::B21 },
  { AArch64::B22 },
  { AArch64::B23 },
  { AArch64::B24 },
  { AArch64::B25 },
  { AArch64::B26 },
  { AArch64::B27 },
  { AArch64::B28 },
  { AArch64::B29 },
  { AArch64::B30 },
  { AArch64::B31 },
  { AArch64::P0 },
  { AArch64::P1 },
  { AArch64::P2 },
  { AArch64::P3 },
  { AArch64::P4 },
  { AArch64::P5 },
  { AArch64::P6 },
  { AArch64::P7 },
  { AArch64::P8 },
  { AArch64::P9 },
  { AArch64::P10 },
  { AArch64::P11 },
  { AArch64::P12 },
  { AArch64::P13 },
  { AArch64::P14 },
  { AArch64::P15 },
  { AArch64::W0 },
  { AArch64::W1 },
  { AArch64::W2 },
  { AArch64::W3 },
  { AArch64::W4 },
  { AArch64::W5 },
  { AArch64::W6 },
  { AArch64::W7 },
  { AArch64::W8 },
  { AArch64::W9 },
  { AArch64::W10 },
  { AArch64::W11 },
  { AArch64::W12 },
  { AArch64::W13 },
  { AArch64::W14 },
  { AArch64::W15 },
  { AArch64::W16 },
  { AArch64::W17 },
  { AArch64::W18 },
  { AArch64::W19 },
  { AArch64::W20 },
  { AArch64::W21 },
  { AArch64::W22 },
  { AArch64::W23 },
  { AArch64::W24 },
  { AArch64::W25 },
  { AArch64::W26 },
  { AArch64::W27 },
  { AArch64::W28 },
  { AArch64::Z0_HI },
  { AArch64::Z1_HI },
  { AArch64::Z2_HI },
  { AArch64::Z3_HI },
  { AArch64::Z4_HI },
  { AArch64::Z5_HI },
  { AArch64::Z6_HI },
  { AArch64::Z7_HI },
  { AArch64::Z8_HI },
  { AArch64::Z9_HI },
  { AArch64::Z10_HI },
  { AArch64::Z11_HI },
  { AArch64::Z12_HI },
  { AArch64::Z13_HI },
  { AArch64::Z14_HI },
  { AArch64::Z15_HI },
  { AArch64::Z16_HI },
  { AArch64::Z17_HI },
  { AArch64::Z18_HI },
  { AArch64::Z19_HI },
  { AArch64::Z20_HI },
  { AArch64::Z21_HI },
  { AArch64::Z22_HI },
  { AArch64::Z23_HI },
  { AArch64::Z24_HI },
  { AArch64::Z25_HI },
  { AArch64::Z26_HI },
  { AArch64::Z27_HI },
  { AArch64::Z28_HI },
  { AArch64::Z29_HI },
  { AArch64::Z30_HI },
  { AArch64::Z31_HI },
};

namespace {     // Register classes...
  // FPR8 Register Class...
  const MCPhysReg FPR8[] = {
    AArch64::B0, AArch64::B1, AArch64::B2, AArch64::B3, AArch64::B4, AArch64::B5, AArch64::B6, AArch64::B7, AArch64::B8, AArch64::B9, AArch64::B10, AArch64::B11, AArch64::B12, AArch64::B13, AArch64::B14, AArch64::B15, AArch64::B16, AArch64::B17, AArch64::B18, AArch64::B19, AArch64::B20, AArch64::B21, AArch64::B22, AArch64::B23, AArch64::B24, AArch64::B25, AArch64::B26, AArch64::B27, AArch64::B28, AArch64::B29, AArch64::B30, AArch64::B31, 
  };

  // FPR8 Bit set.
  const uint8_t FPR8Bits[] = {
    0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 
  };

  // FPR16 Register Class...
  const MCPhysReg FPR16[] = {
    AArch64::H0, AArch64::H1, AArch64::H2, AArch64::H3, AArch64::H4, AArch64::H5, AArch64::H6, AArch64::H7, AArch64::H8, AArch64::H9, AArch64::H10, AArch64::H11, AArch64::H12, AArch64::H13, AArch64::H14, AArch64::H15, AArch64::H16, AArch64::H17, AArch64::H18, AArch64::H19, AArch64::H20, AArch64::H21, AArch64::H22, AArch64::H23, AArch64::H24, AArch64::H25, AArch64::H26, AArch64::H27, AArch64::H28, AArch64::H29, AArch64::H30, AArch64::H31, 
  };

  // FPR16 Bit set.
  const uint8_t FPR16Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 
  };

  // PPR Register Class...
  const MCPhysReg PPR[] = {
    AArch64::P0, AArch64::P1, AArch64::P2, AArch64::P3, AArch64::P4, AArch64::P5, AArch64::P6, AArch64::P7, AArch64::P8, AArch64::P9, AArch64::P10, AArch64::P11, AArch64::P12, AArch64::P13, AArch64::P14, AArch64::P15, 
  };

  // PPR Bit set.
  const uint8_t PPRBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 
  };

  // PPR_3b Register Class...
  const MCPhysReg PPR_3b[] = {
    AArch64::P0, AArch64::P1, AArch64::P2, AArch64::P3, AArch64::P4, AArch64::P5, AArch64::P6, AArch64::P7, 
  };

  // PPR_3b Bit set.
  const uint8_t PPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 
  };

  // GPR32all Register Class...
  const MCPhysReg GPR32all[] = {
    AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WZR, AArch64::WSP, 
  };

  // GPR32all Bit set.
  const uint8_t GPR32allBits[] = {
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 
  };

  // FPR32 Register Class...
  const MCPhysReg FPR32[] = {
    AArch64::S0, AArch64::S1, AArch64::S2, AArch64::S3, AArch64::S4, AArch64::S5, AArch64::S6, AArch64::S7, AArch64::S8, AArch64::S9, AArch64::S10, AArch64::S11, AArch64::S12, AArch64::S13, AArch64::S14, AArch64::S15, AArch64::S16, AArch64::S17, AArch64::S18, AArch64::S19, AArch64::S20, AArch64::S21, AArch64::S22, AArch64::S23, AArch64::S24, AArch64::S25, AArch64::S26, AArch64::S27, AArch64::S28, AArch64::S29, AArch64::S30, AArch64::S31, 
  };

  // FPR32 Bit set.
  const uint8_t FPR32Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 
  };

  // GPR32 Register Class...
  const MCPhysReg GPR32[] = {
    AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WZR, 
  };

  // GPR32 Bit set.
  const uint8_t GPR32Bits[] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 
  };

  // GPR32sp Register Class...
  const MCPhysReg GPR32sp[] = {
    AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WSP, 
  };

  // GPR32sp Bit set.
  const uint8_t GPR32spBits[] = {
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 
  };

  // GPR32common Register Class...
  const MCPhysReg GPR32common[] = {
    AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, 
  };

  // GPR32common Bit set.
  const uint8_t GPR32commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 
  };

  // CCR Register Class...
  const MCPhysReg CCR[] = {
    AArch64::NZCV, 
  };

  // CCR Bit set.
  const uint8_t CCRBits[] = {
    0x10, 
  };

  // GPR32sponly Register Class...
  const MCPhysReg GPR32sponly[] = {
    AArch64::WSP, 
  };

  // GPR32sponly Bit set.
  const uint8_t GPR32sponlyBits[] = {
    0x40, 
  };

  // WSeqPairsClass Register Class...
  const MCPhysReg WSeqPairsClass[] = {
    AArch64::W0_W1, AArch64::W1_W2, AArch64::W2_W3, AArch64::W3_W4, AArch64::W4_W5, AArch64::W5_W6, AArch64::W6_W7, AArch64::W7_W8, AArch64::W8_W9, AArch64::W9_W10, AArch64::W10_W11, AArch64::W11_W12, AArch64::W12_W13, AArch64::W13_W14, AArch64::W14_W15, AArch64::W15_W16, AArch64::W16_W17, AArch64::W17_W18, AArch64::W18_W19, AArch64::W19_W20, AArch64::W20_W21, AArch64::W21_W22, AArch64::W22_W23, AArch64::W23_W24, AArch64::W24_W25, AArch64::W25_W26, AArch64::W26_W27, AArch64::W27_W28, AArch64::W28_W29, AArch64::W29_W30, AArch64::W30_WZR, AArch64::WZR_W0, 
  };

  // WSeqPairsClass Bit set.
  const uint8_t WSeqPairsClassBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // WSeqPairsClass_with_sube32_in_GPR32common Register Class...
  const MCPhysReg WSeqPairsClass_with_sube32_in_GPR32common[] = {
    AArch64::W0_W1, AArch64::W1_W2, AArch64::W2_W3, AArch64::W3_W4, AArch64::W4_W5, AArch64::W5_W6, AArch64::W6_W7, AArch64::W7_W8, AArch64::W8_W9, AArch64::W9_W10, AArch64::W10_W11, AArch64::W11_W12, AArch64::W12_W13, AArch64::W13_W14, AArch64::W14_W15, AArch64::W15_W16, AArch64::W16_W17, AArch64::W17_W18, AArch64::W18_W19, AArch64::W19_W20, AArch64::W20_W21, AArch64::W21_W22, AArch64::W22_W23, AArch64::W23_W24, AArch64::W24_W25, AArch64::W25_W26, AArch64::W26_W27, AArch64::W27_W28, AArch64::W28_W29, AArch64::W29_W30, AArch64::W30_WZR, 
  };

  // WSeqPairsClass_with_sube32_in_GPR32common Bit set.
  const uint8_t WSeqPairsClass_with_sube32_in_GPR32commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // WSeqPairsClass_with_subo32_in_GPR32common Register Class...
  const MCPhysReg WSeqPairsClass_with_subo32_in_GPR32common[] = {
    AArch64::W0_W1, AArch64::W1_W2, AArch64::W2_W3, AArch64::W3_W4, AArch64::W4_W5, AArch64::W5_W6, AArch64::W6_W7, AArch64::W7_W8, AArch64::W8_W9, AArch64::W9_W10, AArch64::W10_W11, AArch64::W11_W12, AArch64::W12_W13, AArch64::W13_W14, AArch64::W14_W15, AArch64::W15_W16, AArch64::W16_W17, AArch64::W17_W18, AArch64::W18_W19, AArch64::W19_W20, AArch64::W20_W21, AArch64::W21_W22, AArch64::W22_W23, AArch64::W23_W24, AArch64::W24_W25, AArch64::W25_W26, AArch64::W26_W27, AArch64::W27_W28, AArch64::W28_W29, AArch64::W29_W30, AArch64::WZR_W0, 
  };

  // WSeqPairsClass_with_subo32_in_GPR32common Bit set.
  const uint8_t WSeqPairsClass_with_subo32_in_GPR32commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common Register Class...
  const MCPhysReg WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common[] = {
    AArch64::W0_W1, AArch64::W1_W2, AArch64::W2_W3, AArch64::W3_W4, AArch64::W4_W5, AArch64::W5_W6, AArch64::W6_W7, AArch64::W7_W8, AArch64::W8_W9, AArch64::W9_W10, AArch64::W10_W11, AArch64::W11_W12, AArch64::W12_W13, AArch64::W13_W14, AArch64::W14_W15, AArch64::W15_W16, AArch64::W16_W17, AArch64::W17_W18, AArch64::W18_W19, AArch64::W19_W20, AArch64::W20_W21, AArch64::W21_W22, AArch64::W22_W23, AArch64::W23_W24, AArch64::W24_W25, AArch64::W25_W26, AArch64::W26_W27, AArch64::W27_W28, AArch64::W28_W29, AArch64::W29_W30, 
  };

  // WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common Bit set.
  const uint8_t WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x1f, 
  };

  // GPR64all Register Class...
  const MCPhysReg GPR64all[] = {
    AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::XZR, AArch64::SP, 
  };

  // GPR64all Bit set.
  const uint8_t GPR64allBits[] = {
    0x2c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 
  };

  // FPR64 Register Class...
  const MCPhysReg FPR64[] = {
    AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, 
  };

  // FPR64 Bit set.
  const uint8_t FPR64Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 
  };

  // GPR64 Register Class...
  const MCPhysReg GPR64[] = {
    AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::XZR, 
  };

  // GPR64 Bit set.
  const uint8_t GPR64Bits[] = {
    0x0c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 
  };

  // GPR64sp Register Class...
  const MCPhysReg GPR64sp[] = {
    AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::SP, 
  };

  // GPR64sp Bit set.
  const uint8_t GPR64spBits[] = {
    0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 
  };

  // GPR64common Register Class...
  const MCPhysReg GPR64common[] = {
    AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, 
  };

  // GPR64common Bit set.
  const uint8_t GPR64commonBits[] = {
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 
  };

  // tcGPR64 Register Class...
  const MCPhysReg tcGPR64[] = {
    AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, 
  };

  // tcGPR64 Bit set.
  const uint8_t tcGPR64Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x07, 
  };

  // GPR64sponly Register Class...
  const MCPhysReg GPR64sponly[] = {
    AArch64::SP, 
  };

  // GPR64sponly Bit set.
  const uint8_t GPR64sponlyBits[] = {
    0x20, 
  };

  // DD Register Class...
  const MCPhysReg DD[] = {
    AArch64::D0_D1, AArch64::D1_D2, AArch64::D2_D3, AArch64::D3_D4, AArch64::D4_D5, AArch64::D5_D6, AArch64::D6_D7, AArch64::D7_D8, AArch64::D8_D9, AArch64::D9_D10, AArch64::D10_D11, AArch64::D11_D12, AArch64::D12_D13, AArch64::D13_D14, AArch64::D14_D15, AArch64::D15_D16, AArch64::D16_D17, AArch64::D17_D18, AArch64::D18_D19, AArch64::D19_D20, AArch64::D20_D21, AArch64::D21_D22, AArch64::D22_D23, AArch64::D23_D24, AArch64::D24_D25, AArch64::D25_D26, AArch64::D26_D27, AArch64::D27_D28, AArch64::D28_D29, AArch64::D29_D30, AArch64::D30_D31, AArch64::D31_D0, 
  };

  // DD Bit set.
  const uint8_t DDBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // XSeqPairsClass Register Class...
  const MCPhysReg XSeqPairsClass[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::X18_X19, AArch64::X19_X20, AArch64::X20_X21, AArch64::X21_X22, AArch64::X22_X23, AArch64::X23_X24, AArch64::X24_X25, AArch64::X25_X26, AArch64::X26_X27, AArch64::X27_X28, AArch64::X28_FP, AArch64::FP_LR, AArch64::LR_XZR, AArch64::XZR_X0, 
  };

  // XSeqPairsClass Bit set.
  const uint8_t XSeqPairsClassBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common Register Class...
  const MCPhysReg XSeqPairsClass_with_sub_32_in_GPR32common[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::X18_X19, AArch64::X19_X20, AArch64::X20_X21, AArch64::X21_X22, AArch64::X22_X23, AArch64::X23_X24, AArch64::X24_X25, AArch64::X25_X26, AArch64::X26_X27, AArch64::X27_X28, AArch64::X28_FP, AArch64::FP_LR, AArch64::LR_XZR, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common Bit set.
  const uint8_t XSeqPairsClass_with_sub_32_in_GPR32commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xff, 0xff, 0xff, 0x1f, 
  };

  // XSeqPairsClass_with_subo64_in_GPR64common Register Class...
  const MCPhysReg XSeqPairsClass_with_subo64_in_GPR64common[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::X18_X19, AArch64::X19_X20, AArch64::X20_X21, AArch64::X21_X22, AArch64::X22_X23, AArch64::X23_X24, AArch64::X24_X25, AArch64::X25_X26, AArch64::X26_X27, AArch64::X27_X28, AArch64::X28_FP, AArch64::FP_LR, AArch64::XZR_X0, 
  };

  // XSeqPairsClass_with_subo64_in_GPR64common Bit set.
  const uint8_t XSeqPairsClass_with_subo64_in_GPR64commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common Register Class...
  const MCPhysReg XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::X18_X19, AArch64::X19_X20, AArch64::X20_X21, AArch64::X21_X22, AArch64::X22_X23, AArch64::X23_X24, AArch64::X24_X25, AArch64::X25_X26, AArch64::X26_X27, AArch64::X27_X28, AArch64::X28_FP, AArch64::FP_LR, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common Bit set.
  const uint8_t XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xff, 0xff, 0xff, 0x1f, 
  };

  // XSeqPairsClass_with_sube64_in_tcGPR64 Register Class...
  const MCPhysReg XSeqPairsClass_with_sube64_in_tcGPR64[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::X18_X19, 
  };

  // XSeqPairsClass_with_sube64_in_tcGPR64 Bit set.
  const uint8_t XSeqPairsClass_with_sube64_in_tcGPR64Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x0f, 
  };

  // XSeqPairsClass_with_subo64_in_tcGPR64 Register Class...
  const MCPhysReg XSeqPairsClass_with_subo64_in_tcGPR64[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, AArch64::XZR_X0, 
  };

  // XSeqPairsClass_with_subo64_in_tcGPR64 Bit set.
  const uint8_t XSeqPairsClass_with_subo64_in_tcGPR64Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xfe, 0xff, 0x07, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64 Register Class...
  const MCPhysReg XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64[] = {
    AArch64::X0_X1, AArch64::X1_X2, AArch64::X2_X3, AArch64::X3_X4, AArch64::X4_X5, AArch64::X5_X6, AArch64::X6_X7, AArch64::X7_X8, AArch64::X8_X9, AArch64::X9_X10, AArch64::X10_X11, AArch64::X11_X12, AArch64::X12_X13, AArch64::X13_X14, AArch64::X14_X15, AArch64::X15_X16, AArch64::X16_X17, AArch64::X17_X18, 
  };

  // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64 Bit set.
  const uint8_t XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x07, 
  };

  // FPR128 Register Class...
  const MCPhysReg FPR128[] = {
    AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 
  };

  // FPR128 Bit set.
  const uint8_t FPR128Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x01, 
  };

  // ZPR Register Class...
  const MCPhysReg ZPR[] = {
    AArch64::Z0, AArch64::Z1, AArch64::Z2, AArch64::Z3, AArch64::Z4, AArch64::Z5, AArch64::Z6, AArch64::Z7, AArch64::Z8, AArch64::Z9, AArch64::Z10, AArch64::Z11, AArch64::Z12, AArch64::Z13, AArch64::Z14, AArch64::Z15, AArch64::Z16, AArch64::Z17, AArch64::Z18, AArch64::Z19, AArch64::Z20, AArch64::Z21, AArch64::Z22, AArch64::Z23, AArch64::Z24, AArch64::Z25, AArch64::Z26, AArch64::Z27, AArch64::Z28, AArch64::Z29, AArch64::Z30, AArch64::Z31, 
  };

  // ZPR Bit set.
  const uint8_t ZPRBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // FPR128_lo Register Class...
  const MCPhysReg FPR128_lo[] = {
    AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, 
  };

  // FPR128_lo Bit set.
  const uint8_t FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 
  };

  // ZPR_4b Register Class...
  const MCPhysReg ZPR_4b[] = {
    AArch64::Z0, AArch64::Z1, AArch64::Z2, AArch64::Z3, AArch64::Z4, AArch64::Z5, AArch64::Z6, AArch64::Z7, AArch64::Z8, AArch64::Z9, AArch64::Z10, AArch64::Z11, AArch64::Z12, AArch64::Z13, AArch64::Z14, AArch64::Z15, 
  };

  // ZPR_4b Bit set.
  const uint8_t ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // ZPR_3b Register Class...
  const MCPhysReg ZPR_3b[] = {
    AArch64::Z0, AArch64::Z1, AArch64::Z2, AArch64::Z3, AArch64::Z4, AArch64::Z5, AArch64::Z6, AArch64::Z7, 
  };

  // ZPR_3b Bit set.
  const uint8_t ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1f, 
  };

  // DDD Register Class...
  const MCPhysReg DDD[] = {
    AArch64::D0_D1_D2, AArch64::D1_D2_D3, AArch64::D2_D3_D4, AArch64::D3_D4_D5, AArch64::D4_D5_D6, AArch64::D5_D6_D7, AArch64::D6_D7_D8, AArch64::D7_D8_D9, AArch64::D8_D9_D10, AArch64::D9_D10_D11, AArch64::D10_D11_D12, AArch64::D11_D12_D13, AArch64::D12_D13_D14, AArch64::D13_D14_D15, AArch64::D14_D15_D16, AArch64::D15_D16_D17, AArch64::D16_D17_D18, AArch64::D17_D18_D19, AArch64::D18_D19_D20, AArch64::D19_D20_D21, AArch64::D20_D21_D22, AArch64::D21_D22_D23, AArch64::D22_D23_D24, AArch64::D23_D24_D25, AArch64::D24_D25_D26, AArch64::D25_D26_D27, AArch64::D26_D27_D28, AArch64::D27_D28_D29, AArch64::D28_D29_D30, AArch64::D29_D30_D31, AArch64::D30_D31_D0, AArch64::D31_D0_D1, 
  };

  // DDD Bit set.
  const uint8_t DDDBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // DDDD Register Class...
  const MCPhysReg DDDD[] = {
    AArch64::D0_D1_D2_D3, AArch64::D1_D2_D3_D4, AArch64::D2_D3_D4_D5, AArch64::D3_D4_D5_D6, AArch64::D4_D5_D6_D7, AArch64::D5_D6_D7_D8, AArch64::D6_D7_D8_D9, AArch64::D7_D8_D9_D10, AArch64::D8_D9_D10_D11, AArch64::D9_D10_D11_D12, AArch64::D10_D11_D12_D13, AArch64::D11_D12_D13_D14, AArch64::D12_D13_D14_D15, AArch64::D13_D14_D15_D16, AArch64::D14_D15_D16_D17, AArch64::D15_D16_D17_D18, AArch64::D16_D17_D18_D19, AArch64::D17_D18_D19_D20, AArch64::D18_D19_D20_D21, AArch64::D19_D20_D21_D22, AArch64::D20_D21_D22_D23, AArch64::D21_D22_D23_D24, AArch64::D22_D23_D24_D25, AArch64::D23_D24_D25_D26, AArch64::D24_D25_D26_D27, AArch64::D25_D26_D27_D28, AArch64::D26_D27_D28_D29, AArch64::D27_D28_D29_D30, AArch64::D28_D29_D30_D31, AArch64::D29_D30_D31_D0, AArch64::D30_D31_D0_D1, AArch64::D31_D0_D1_D2, 
  };

  // DDDD Bit set.
  const uint8_t DDDDBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // QQ Register Class...
  const MCPhysReg QQ[] = {
    AArch64::Q0_Q1, AArch64::Q1_Q2, AArch64::Q2_Q3, AArch64::Q3_Q4, AArch64::Q4_Q5, AArch64::Q5_Q6, AArch64::Q6_Q7, AArch64::Q7_Q8, AArch64::Q8_Q9, AArch64::Q9_Q10, AArch64::Q10_Q11, AArch64::Q11_Q12, AArch64::Q12_Q13, AArch64::Q13_Q14, AArch64::Q14_Q15, AArch64::Q15_Q16, AArch64::Q16_Q17, AArch64::Q17_Q18, AArch64::Q18_Q19, AArch64::Q19_Q20, AArch64::Q20_Q21, AArch64::Q21_Q22, AArch64::Q22_Q23, AArch64::Q23_Q24, AArch64::Q24_Q25, AArch64::Q25_Q26, AArch64::Q26_Q27, AArch64::Q27_Q28, AArch64::Q28_Q29, AArch64::Q29_Q30, AArch64::Q30_Q31, AArch64::Q31_Q0, 
  };

  // QQ Bit set.
  const uint8_t QQBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // ZPR2 Register Class...
  const MCPhysReg ZPR2[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z7_Z8, AArch64::Z8_Z9, AArch64::Z9_Z10, AArch64::Z10_Z11, AArch64::Z11_Z12, AArch64::Z12_Z13, AArch64::Z13_Z14, AArch64::Z14_Z15, AArch64::Z15_Z16, AArch64::Z16_Z17, AArch64::Z17_Z18, AArch64::Z18_Z19, AArch64::Z19_Z20, AArch64::Z20_Z21, AArch64::Z21_Z22, AArch64::Z22_Z23, AArch64::Z23_Z24, AArch64::Z24_Z25, AArch64::Z25_Z26, AArch64::Z26_Z27, AArch64::Z27_Z28, AArch64::Z28_Z29, AArch64::Z29_Z30, AArch64::Z30_Z31, AArch64::Z31_Z0, 
  };

  // ZPR2 Bit set.
  const uint8_t ZPR2Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // QQ_with_qsub0_in_FPR128_lo Register Class...
  const MCPhysReg QQ_with_qsub0_in_FPR128_lo[] = {
    AArch64::Q0_Q1, AArch64::Q1_Q2, AArch64::Q2_Q3, AArch64::Q3_Q4, AArch64::Q4_Q5, AArch64::Q5_Q6, AArch64::Q6_Q7, AArch64::Q7_Q8, AArch64::Q8_Q9, AArch64::Q9_Q10, AArch64::Q10_Q11, AArch64::Q11_Q12, AArch64::Q12_Q13, AArch64::Q13_Q14, AArch64::Q14_Q15, AArch64::Q15_Q16, 
  };

  // QQ_with_qsub0_in_FPR128_lo Bit set.
  const uint8_t QQ_with_qsub0_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1, AArch64::Q1_Q2, AArch64::Q2_Q3, AArch64::Q3_Q4, AArch64::Q4_Q5, AArch64::Q5_Q6, AArch64::Q6_Q7, AArch64::Q7_Q8, AArch64::Q8_Q9, AArch64::Q9_Q10, AArch64::Q10_Q11, AArch64::Q11_Q12, AArch64::Q12_Q13, AArch64::Q13_Q14, AArch64::Q14_Q15, AArch64::Q31_Q0, 
  };

  // QQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // ZPR2_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR2_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z7_Z8, AArch64::Z8_Z9, AArch64::Z9_Z10, AArch64::Z10_Z11, AArch64::Z11_Z12, AArch64::Z12_Z13, AArch64::Z13_Z14, AArch64::Z14_Z15, AArch64::Z31_Z0, 
  };

  // ZPR2_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR2_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // ZPR2_with_zsub_in_FPR128_lo Register Class...
  const MCPhysReg ZPR2_with_zsub_in_FPR128_lo[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z7_Z8, AArch64::Z8_Z9, AArch64::Z9_Z10, AArch64::Z10_Z11, AArch64::Z11_Z12, AArch64::Z12_Z13, AArch64::Z13_Z14, AArch64::Z14_Z15, AArch64::Z15_Z16, 
  };

  // ZPR2_with_zsub_in_FPR128_lo Bit set.
  const uint8_t ZPR2_with_zsub_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1, AArch64::Q1_Q2, AArch64::Q2_Q3, AArch64::Q3_Q4, AArch64::Q4_Q5, AArch64::Q5_Q6, AArch64::Q6_Q7, AArch64::Q7_Q8, AArch64::Q8_Q9, AArch64::Q9_Q10, AArch64::Q10_Q11, AArch64::Q11_Q12, AArch64::Q12_Q13, AArch64::Q13_Q14, AArch64::Q14_Q15, 
  };

  // QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z7_Z8, AArch64::Z8_Z9, AArch64::Z9_Z10, AArch64::Z10_Z11, AArch64::Z11_Z12, AArch64::Z12_Z13, AArch64::Z13_Z14, AArch64::Z14_Z15, 
  };

  // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // ZPR2_with_zsub0_in_ZPR_3b Register Class...
  const MCPhysReg ZPR2_with_zsub0_in_ZPR_3b[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z7_Z8, 
  };

  // ZPR2_with_zsub0_in_ZPR_3b Bit set.
  const uint8_t ZPR2_with_zsub0_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1f, 
  };

  // ZPR2_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR2_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, AArch64::Z31_Z0, 
  };

  // ZPR2_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR2_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x10, 
  };

  // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1, AArch64::Z1_Z2, AArch64::Z2_Z3, AArch64::Z3_Z4, AArch64::Z4_Z5, AArch64::Z5_Z6, AArch64::Z6_Z7, 
  };

  // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 
  };

  // QQQ Register Class...
  const MCPhysReg QQQ[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q14_Q15_Q16, AArch64::Q15_Q16_Q17, AArch64::Q16_Q17_Q18, AArch64::Q17_Q18_Q19, AArch64::Q18_Q19_Q20, AArch64::Q19_Q20_Q21, AArch64::Q20_Q21_Q22, AArch64::Q21_Q22_Q23, AArch64::Q22_Q23_Q24, AArch64::Q23_Q24_Q25, AArch64::Q24_Q25_Q26, AArch64::Q25_Q26_Q27, AArch64::Q26_Q27_Q28, AArch64::Q27_Q28_Q29, AArch64::Q28_Q29_Q30, AArch64::Q29_Q30_Q31, AArch64::Q30_Q31_Q0, AArch64::Q31_Q0_Q1, 
  };

  // QQQ Bit set.
  const uint8_t QQQBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // ZPR3 Register Class...
  const MCPhysReg ZPR3[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z14_Z15_Z16, AArch64::Z15_Z16_Z17, AArch64::Z16_Z17_Z18, AArch64::Z17_Z18_Z19, AArch64::Z18_Z19_Z20, AArch64::Z19_Z20_Z21, AArch64::Z20_Z21_Z22, AArch64::Z21_Z22_Z23, AArch64::Z22_Z23_Z24, AArch64::Z23_Z24_Z25, AArch64::Z24_Z25_Z26, AArch64::Z25_Z26_Z27, AArch64::Z26_Z27_Z28, AArch64::Z27_Z28_Z29, AArch64::Z28_Z29_Z30, AArch64::Z29_Z30_Z31, AArch64::Z30_Z31_Z0, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3 Bit set.
  const uint8_t ZPR3Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // QQQ_with_qsub0_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub0_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q14_Q15_Q16, AArch64::Q15_Q16_Q17, 
  };

  // QQQ_with_qsub0_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub0_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q14_Q15_Q16, AArch64::Q31_Q0_Q1, 
  };

  // QQQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // QQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q30_Q31_Q0, AArch64::Q31_Q0_Q1, 
  };

  // QQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x18, 
  };

  // ZPR3_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR3_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z14_Z15_Z16, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR3_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // ZPR3_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR3_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z30_Z31_Z0, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR3_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x18, 
  };

  // ZPR3_with_zsub_in_FPR128_lo Register Class...
  const MCPhysReg ZPR3_with_zsub_in_FPR128_lo[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z14_Z15_Z16, AArch64::Z15_Z16_Z17, 
  };

  // ZPR3_with_zsub_in_FPR128_lo Bit set.
  const uint8_t ZPR3_with_zsub_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q14_Q15_Q16, 
  };

  // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, AArch64::Q31_Q0_Q1, 
  };

  // QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x10, 
  };

  // ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x10, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, AArch64::Z14_Z15_Z16, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2, AArch64::Q1_Q2_Q3, AArch64::Q2_Q3_Q4, AArch64::Q3_Q4_Q5, AArch64::Q4_Q5_Q6, AArch64::Q5_Q6_Q7, AArch64::Q6_Q7_Q8, AArch64::Q7_Q8_Q9, AArch64::Q8_Q9_Q10, AArch64::Q9_Q10_Q11, AArch64::Q10_Q11_Q12, AArch64::Q11_Q12_Q13, AArch64::Q12_Q13_Q14, AArch64::Q13_Q14_Q15, 
  };

  // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, AArch64::Z8_Z9_Z10, AArch64::Z9_Z10_Z11, AArch64::Z10_Z11_Z12, AArch64::Z11_Z12_Z13, AArch64::Z12_Z13_Z14, AArch64::Z13_Z14_Z15, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 
  };

  // ZPR3_with_zsub0_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub0_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z7_Z8_Z9, 
  };

  // ZPR3_with_zsub0_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub0_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1f, 
  };

  // ZPR3_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x10, 
  };

  // ZPR3_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z30_Z31_Z0, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x18, 
  };

  // ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z31_Z0_Z1, 
  };

  // ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x10, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, AArch64::Z6_Z7_Z8, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2, AArch64::Z1_Z2_Z3, AArch64::Z2_Z3_Z4, AArch64::Z3_Z4_Z5, AArch64::Z4_Z5_Z6, AArch64::Z5_Z6_Z7, 
  };

  // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 
  };

  // QQQQ Register Class...
  const MCPhysReg QQQQ[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q14_Q15_Q16_Q17, AArch64::Q15_Q16_Q17_Q18, AArch64::Q16_Q17_Q18_Q19, AArch64::Q17_Q18_Q19_Q20, AArch64::Q18_Q19_Q20_Q21, AArch64::Q19_Q20_Q21_Q22, AArch64::Q20_Q21_Q22_Q23, AArch64::Q21_Q22_Q23_Q24, AArch64::Q22_Q23_Q24_Q25, AArch64::Q23_Q24_Q25_Q26, AArch64::Q24_Q25_Q26_Q27, AArch64::Q25_Q26_Q27_Q28, AArch64::Q26_Q27_Q28_Q29, AArch64::Q27_Q28_Q29_Q30, AArch64::Q28_Q29_Q30_Q31, AArch64::Q29_Q30_Q31_Q0, AArch64::Q30_Q31_Q0_Q1, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ Bit set.
  const uint8_t QQQQBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // ZPR4 Register Class...
  const MCPhysReg ZPR4[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z14_Z15_Z16_Z17, AArch64::Z15_Z16_Z17_Z18, AArch64::Z16_Z17_Z18_Z19, AArch64::Z17_Z18_Z19_Z20, AArch64::Z18_Z19_Z20_Z21, AArch64::Z19_Z20_Z21_Z22, AArch64::Z20_Z21_Z22_Z23, AArch64::Z21_Z22_Z23_Z24, AArch64::Z22_Z23_Z24_Z25, AArch64::Z23_Z24_Z25_Z26, AArch64::Z24_Z25_Z26_Z27, AArch64::Z25_Z26_Z27_Z28, AArch64::Z26_Z27_Z28_Z29, AArch64::Z27_Z28_Z29_Z30, AArch64::Z28_Z29_Z30_Z31, AArch64::Z29_Z30_Z31_Z0, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4 Bit set.
  const uint8_t ZPR4Bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x1f, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub0_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q14_Q15_Q16_Q17, AArch64::Q15_Q16_Q17_Q18, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub0_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q14_Q15_Q16_Q17, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // QQQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q30_Q31_Q0_Q1, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x18, 
  };

  // QQQQ_with_qsub3_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub3_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q29_Q30_Q31_Q0, AArch64::Q30_Q31_Q0_Q1, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub3_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub3_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x1c, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z14_Z15_Z16_Z17, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0x10, 
  };

  // ZPR4_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x18, 
  };

  // ZPR4_with_zsub3_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub3_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z29_Z30_Z31_Z0, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub3_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub3_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x1c, 
  };

  // ZPR4_with_zsub_in_FPR128_lo Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z14_Z15_Z16_Z17, AArch64::Z15_Z16_Z17_Z18, 
  };

  // ZPR4_with_zsub_in_FPR128_lo Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q14_Q15_Q16_Q17, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x10, 
  };

  // QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q30_Q31_Q0_Q1, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x18, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x10, 
  };

  // ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x18, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, AArch64::Z14_Z15_Z16_Z17, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q13_Q14_Q15_Q16, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, AArch64::Q31_Q0_Q1_Q2, 
  };

  // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x10, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x10, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, AArch64::Z13_Z14_Z15_Z16, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Register Class...
  const MCPhysReg QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo[] = {
    AArch64::Q0_Q1_Q2_Q3, AArch64::Q1_Q2_Q3_Q4, AArch64::Q2_Q3_Q4_Q5, AArch64::Q3_Q4_Q5_Q6, AArch64::Q4_Q5_Q6_Q7, AArch64::Q5_Q6_Q7_Q8, AArch64::Q6_Q7_Q8_Q9, AArch64::Q7_Q8_Q9_Q10, AArch64::Q8_Q9_Q10_Q11, AArch64::Q9_Q10_Q11_Q12, AArch64::Q10_Q11_Q12_Q13, AArch64::Q11_Q12_Q13_Q14, AArch64::Q12_Q13_Q14_Q15, 
  };

  // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo Bit set.
  const uint8_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, AArch64::Z8_Z9_Z10_Z11, AArch64::Z9_Z10_Z11_Z12, AArch64::Z10_Z11_Z12_Z13, AArch64::Z11_Z12_Z13_Z14, AArch64::Z12_Z13_Z14_Z15, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x03, 
  };

  // ZPR4_with_zsub0_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub0_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z7_Z8_Z9_Z10, 
  };

  // ZPR4_with_zsub0_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub0_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1f, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x10, 
  };

  // ZPR4_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x18, 
  };

  // ZPR4_with_zsub3_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub3_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z29_Z30_Z31_Z0, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub3_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub3_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x1c, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x10, 
  };

  // ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z30_Z31_Z0_Z1, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x18, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, AArch64::Z6_Z7_Z8_Z9, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z31_Z0_Z1_Z2, 
  };

  // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x10, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, AArch64::Z5_Z6_Z7_Z8, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b Register Class...
  const MCPhysReg ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b[] = {
    AArch64::Z0_Z1_Z2_Z3, AArch64::Z1_Z2_Z3_Z4, AArch64::Z2_Z3_Z4_Z5, AArch64::Z3_Z4_Z5_Z6, AArch64::Z4_Z5_Z6_Z7, 
  };

  // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b Bit set.
  const uint8_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 
  };

} // end anonymous namespace

extern const char AArch64RegClassStrings[] = {
  /* 0 */ 'F', 'P', 'R', '3', '2', 0,
  /* 6 */ 'G', 'P', 'R', '3', '2', 0,
  /* 12 */ 'Z', 'P', 'R', '2', 0,
  /* 17 */ 'Z', 'P', 'R', '3', 0,
  /* 22 */ 'F', 'P', 'R', '6', '4', 0,
  /* 28 */ 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'e', '6', '4', '_', 'i', 'n', '_', 't', 'c', 'G', 'P', 'R', '6', '4', 0,
  /* 66 */ 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', '_', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', '_', 'a', 'n', 'd', '_', 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'o', '6', '4', '_', 'i', 'n', '_', 't', 'c', 'G', 'P', 'R', '6', '4', 0,
  /* 150 */ 'Z', 'P', 'R', '4', 0,
  /* 155 */ 'F', 'P', 'R', '1', '6', 0,
  /* 161 */ 'F', 'P', 'R', '1', '2', '8', 0,
  /* 168 */ 'F', 'P', 'R', '8', 0,
  /* 173 */ 'D', 'D', 'D', 'D', 0,
  /* 178 */ 'Q', 'Q', 'Q', 'Q', 0,
  /* 183 */ 'C', 'C', 'R', 0,
  /* 187 */ 'P', 'P', 'R', 0,
  /* 191 */ 'Z', 'P', 'R', 0,
  /* 195 */ 'P', 'P', 'R', '_', '3', 'b', 0,
  /* 202 */ 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 228 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 254 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 280 */ 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 338 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 396 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 454 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 510 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 568 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 624 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 682 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 738 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 794 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '3', 'b', 0,
  /* 852 */ 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 910 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 968 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1026 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1082 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1140 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1196 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1254 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1310 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1366 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'Z', 'P', 'R', '_', '4', 'b', 0,
  /* 1424 */ 'G', 'P', 'R', '3', '2', 'a', 'l', 'l', 0,
  /* 1433 */ 'G', 'P', 'R', '6', '4', 'a', 'l', 'l', 0,
  /* 1442 */ 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', '_', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', 0,
  /* 1484 */ 'W', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'e', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', 0,
  /* 1526 */ 'W', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'e', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', '_', 'a', 'n', 'd', '_', 'W', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'o', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', 0,
  /* 1614 */ 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', '_', '3', '2', '_', 'i', 'n', '_', 'G', 'P', 'R', '3', '2', 'c', 'o', 'm', 'm', 'o', 'n', '_', 'a', 'n', 'd', '_', 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', '_', 'w', 'i', 't', 'h', '_', 's', 'u', 'b', 'o', '6', '4', '_', 'i', 'n', '_', 'G', 'P', 'R', '6', '4', 'c', 'o', 'm', 'm', 'o', 'n', 0,
  /* 1702 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 1731 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 1793 */ 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 1853 */ 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 1911 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 1973 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2035 */ 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2095 */ 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2155 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '0', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2217 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '1', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2279 */ 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '2', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', '_', 'a', 'n', 'd', '_', 'Q', 'Q', 'Q', 'Q', '_', 'w', 'i', 't', 'h', '_', 'q', 's', 'u', 'b', '3', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2341 */ 'Z', 'P', 'R', '2', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2369 */ 'Z', 'P', 'R', '3', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2397 */ 'Z', 'P', 'R', '4', '_', 'w', 'i', 't', 'h', '_', 'z', 's', 'u', 'b', '_', 'i', 'n', '_', 'F', 'P', 'R', '1', '2', '8', '_', 'l', 'o', 0,
  /* 2425 */ 'G', 'P', 'R', '3', '2', 's', 'p', 0,
  /* 2433 */ 'G', 'P', 'R', '6', '4', 's', 'p', 0,
  /* 2441 */ 'W', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', 0,
  /* 2456 */ 'X', 'S', 'e', 'q', 'P', 'a', 'i', 'r', 's', 'C', 'l', 'a', 's', 's', 0,
  /* 2471 */ 'G', 'P', 'R', '3', '2', 's', 'p', 'o', 'n', 'l', 'y', 0,
  /* 2483 */ 'G', 'P', 'R', '6', '4', 's', 'p', 'o', 'n', 'l', 'y', 0,
};

extern const MCRegisterClass AArch64MCRegisterClasses[] = {
  { FPR8, FPR8Bits, 168, 32, sizeof(FPR8Bits), AArch64::FPR8RegClassID, 1, 1, true },
  { FPR16, FPR16Bits, 155, 32, sizeof(FPR16Bits), AArch64::FPR16RegClassID, 2, 1, true },
  { PPR, PPRBits, 187, 16, sizeof(PPRBits), AArch64::PPRRegClassID, 2, 1, true },
  { PPR_3b, PPR_3bBits, 195, 8, sizeof(PPR_3bBits), AArch64::PPR_3bRegClassID, 2, 1, true },
  { GPR32all, GPR32allBits, 1424, 33, sizeof(GPR32allBits), AArch64::GPR32allRegClassID, 4, 1, true },
  { FPR32, FPR32Bits, 0, 32, sizeof(FPR32Bits), AArch64::FPR32RegClassID, 4, 1, true },
  { GPR32, GPR32Bits, 6, 32, sizeof(GPR32Bits), AArch64::GPR32RegClassID, 4, 1, true },
  { GPR32sp, GPR32spBits, 2425, 32, sizeof(GPR32spBits), AArch64::GPR32spRegClassID, 4, 1, true },
  { GPR32common, GPR32commonBits, 1472, 31, sizeof(GPR32commonBits), AArch64::GPR32commonRegClassID, 4, 1, true },
  { CCR, CCRBits, 183, 1, sizeof(CCRBits), AArch64::CCRRegClassID, 4, -1, false },
  { GPR32sponly, GPR32sponlyBits, 2471, 1, sizeof(GPR32sponlyBits), AArch64::GPR32sponlyRegClassID, 4, 1, true },
  { WSeqPairsClass, WSeqPairsClassBits, 2441, 32, sizeof(WSeqPairsClassBits), AArch64::WSeqPairsClassRegClassID, 8, 1, true },
  { WSeqPairsClass_with_sube32_in_GPR32common, WSeqPairsClass_with_sube32_in_GPR32commonBits, 1484, 31, sizeof(WSeqPairsClass_with_sube32_in_GPR32commonBits), AArch64::WSeqPairsClass_with_sube32_in_GPR32commonRegClassID, 8, 1, true },
  { WSeqPairsClass_with_subo32_in_GPR32common, WSeqPairsClass_with_subo32_in_GPR32commonBits, 1572, 31, sizeof(WSeqPairsClass_with_subo32_in_GPR32commonBits), AArch64::WSeqPairsClass_with_subo32_in_GPR32commonRegClassID, 8, 1, true },
  { WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common, WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonBits, 1526, 30, sizeof(WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonBits), AArch64::WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClassID, 8, 1, true },
  { GPR64all, GPR64allBits, 1433, 33, sizeof(GPR64allBits), AArch64::GPR64allRegClassID, 8, 1, true },
  { FPR64, FPR64Bits, 22, 32, sizeof(FPR64Bits), AArch64::FPR64RegClassID, 8, 1, true },
  { GPR64, GPR64Bits, 60, 32, sizeof(GPR64Bits), AArch64::GPR64RegClassID, 8, 1, true },
  { GPR64sp, GPR64spBits, 2433, 32, sizeof(GPR64spBits), AArch64::GPR64spRegClassID, 8, 1, true },
  { GPR64common, GPR64commonBits, 1690, 31, sizeof(GPR64commonBits), AArch64::GPR64commonRegClassID, 8, 1, true },
  { tcGPR64, tcGPR64Bits, 58, 19, sizeof(tcGPR64Bits), AArch64::tcGPR64RegClassID, 8, 1, true },
  { GPR64sponly, GPR64sponlyBits, 2483, 1, sizeof(GPR64sponlyBits), AArch64::GPR64sponlyRegClassID, 8, 1, true },
  { DD, DDBits, 175, 32, sizeof(DDBits), AArch64::DDRegClassID, 16, 1, true },
  { XSeqPairsClass, XSeqPairsClassBits, 2456, 32, sizeof(XSeqPairsClassBits), AArch64::XSeqPairsClassRegClassID, 16, 1, true },
  { XSeqPairsClass_with_sub_32_in_GPR32common, XSeqPairsClass_with_sub_32_in_GPR32commonBits, 1442, 31, sizeof(XSeqPairsClass_with_sub_32_in_GPR32commonBits), AArch64::XSeqPairsClass_with_sub_32_in_GPR32commonRegClassID, 16, 1, true },
  { XSeqPairsClass_with_subo64_in_GPR64common, XSeqPairsClass_with_subo64_in_GPR64commonBits, 1660, 31, sizeof(XSeqPairsClass_with_subo64_in_GPR64commonBits), AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClassID, 16, 1, true },
  { XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common, XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonBits, 1614, 30, sizeof(XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonBits), AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClassID, 16, 1, true },
  { XSeqPairsClass_with_sube64_in_tcGPR64, XSeqPairsClass_with_sube64_in_tcGPR64Bits, 28, 19, sizeof(XSeqPairsClass_with_sube64_in_tcGPR64Bits), AArch64::XSeqPairsClass_with_sube64_in_tcGPR64RegClassID, 16, 1, true },
  { XSeqPairsClass_with_subo64_in_tcGPR64, XSeqPairsClass_with_subo64_in_tcGPR64Bits, 112, 19, sizeof(XSeqPairsClass_with_subo64_in_tcGPR64Bits), AArch64::XSeqPairsClass_with_subo64_in_tcGPR64RegClassID, 16, 1, true },
  { XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64, XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64Bits, 66, 18, sizeof(XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64Bits), AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClassID, 16, 1, true },
  { FPR128, FPR128Bits, 161, 32, sizeof(FPR128Bits), AArch64::FPR128RegClassID, 16, 1, true },
  { ZPR, ZPRBits, 191, 32, sizeof(ZPRBits), AArch64::ZPRRegClassID, 16, 1, true },
  { FPR128_lo, FPR128_loBits, 1721, 16, sizeof(FPR128_loBits), AArch64::FPR128_loRegClassID, 16, 1, true },
  { ZPR_4b, ZPR_4bBits, 903, 16, sizeof(ZPR_4bBits), AArch64::ZPR_4bRegClassID, 16, 1, true },
  { ZPR_3b, ZPR_3bBits, 221, 8, sizeof(ZPR_3bBits), AArch64::ZPR_3bRegClassID, 16, 1, true },
  { DDD, DDDBits, 174, 32, sizeof(DDDBits), AArch64::DDDRegClassID, 24, 1, true },
  { DDDD, DDDDBits, 173, 32, sizeof(DDDDBits), AArch64::DDDDRegClassID, 32, 1, true },
  { QQ, QQBits, 180, 32, sizeof(QQBits), AArch64::QQRegClassID, 32, 1, true },
  { ZPR2, ZPR2Bits, 12, 32, sizeof(ZPR2Bits), AArch64::ZPR2RegClassID, 32, 1, true },
  { QQ_with_qsub0_in_FPR128_lo, QQ_with_qsub0_in_FPR128_loBits, 1704, 16, sizeof(QQ_with_qsub0_in_FPR128_loBits), AArch64::QQ_with_qsub0_in_FPR128_loRegClassID, 32, 1, true },
  { QQ_with_qsub1_in_FPR128_lo, QQ_with_qsub1_in_FPR128_loBits, 1766, 16, sizeof(QQ_with_qsub1_in_FPR128_loBits), AArch64::QQ_with_qsub1_in_FPR128_loRegClassID, 32, 1, true },
  { ZPR2_with_zsub1_in_ZPR_4b, ZPR2_with_zsub1_in_ZPR_4bBits, 884, 16, sizeof(ZPR2_with_zsub1_in_ZPR_4bBits), AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClassID, 32, 1, true },
  { ZPR2_with_zsub_in_FPR128_lo, ZPR2_with_zsub_in_FPR128_loBits, 2341, 16, sizeof(ZPR2_with_zsub_in_FPR128_loBits), AArch64::ZPR2_with_zsub_in_FPR128_loRegClassID, 32, 1, true },
  { QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo, QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loBits, 1853, 15, sizeof(QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loBits), AArch64::QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClassID, 32, 1, true },
  { ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b, ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bBits, 852, 15, sizeof(ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bBits), AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClassID, 32, 1, true },
  { ZPR2_with_zsub0_in_ZPR_3b, ZPR2_with_zsub0_in_ZPR_3bBits, 202, 8, sizeof(ZPR2_with_zsub0_in_ZPR_3bBits), AArch64::ZPR2_with_zsub0_in_ZPR_3bRegClassID, 32, 1, true },
  { ZPR2_with_zsub1_in_ZPR_3b, ZPR2_with_zsub1_in_ZPR_3bBits, 312, 8, sizeof(ZPR2_with_zsub1_in_ZPR_3bBits), AArch64::ZPR2_with_zsub1_in_ZPR_3bRegClassID, 32, 1, true },
  { ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b, ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bBits, 280, 7, sizeof(ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bBits), AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClassID, 32, 1, true },
  { QQQ, QQQBits, 179, 32, sizeof(QQQBits), AArch64::QQQRegClassID, 48, 1, true },
  { ZPR3, ZPR3Bits, 17, 32, sizeof(ZPR3Bits), AArch64::ZPR3RegClassID, 48, 1, true },
  { QQQ_with_qsub0_in_FPR128_lo, QQQ_with_qsub0_in_FPR128_loBits, 1703, 16, sizeof(QQQ_with_qsub0_in_FPR128_loBits), AArch64::QQQ_with_qsub0_in_FPR128_loRegClassID, 48, 1, true },
  { QQQ_with_qsub1_in_FPR128_lo, QQQ_with_qsub1_in_FPR128_loBits, 1765, 16, sizeof(QQQ_with_qsub1_in_FPR128_loBits), AArch64::QQQ_with_qsub1_in_FPR128_loRegClassID, 48, 1, true },
  { QQQ_with_qsub2_in_FPR128_lo, QQQ_with_qsub2_in_FPR128_loBits, 1945, 16, sizeof(QQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQ_with_qsub2_in_FPR128_loRegClassID, 48, 1, true },
  { ZPR3_with_zsub1_in_ZPR_4b, ZPR3_with_zsub1_in_ZPR_4bBits, 942, 16, sizeof(ZPR3_with_zsub1_in_ZPR_4bBits), AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClassID, 48, 1, true },
  { ZPR3_with_zsub2_in_ZPR_4b, ZPR3_with_zsub2_in_ZPR_4bBits, 1056, 16, sizeof(ZPR3_with_zsub2_in_ZPR_4bBits), AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClassID, 48, 1, true },
  { ZPR3_with_zsub_in_FPR128_lo, ZPR3_with_zsub_in_FPR128_loBits, 2369, 16, sizeof(ZPR3_with_zsub_in_FPR128_loBits), AArch64::ZPR3_with_zsub_in_FPR128_loRegClassID, 48, 1, true },
  { QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo, QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loBits, 1793, 15, sizeof(QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loBits), AArch64::QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClassID, 48, 1, true },
  { QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo, QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits, 2095, 15, sizeof(QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID, 48, 1, true },
  { ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b, ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bBits, 1026, 15, sizeof(ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bBits), AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID, 48, 1, true },
  { ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b, ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bBits, 910, 15, sizeof(ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bBits), AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClassID, 48, 1, true },
  { QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo, QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits, 2035, 14, sizeof(QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID, 48, 1, true },
  { ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b, ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bBits, 1082, 14, sizeof(ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bBits), AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID, 48, 1, true },
  { ZPR3_with_zsub0_in_ZPR_3b, ZPR3_with_zsub0_in_ZPR_3bBits, 228, 8, sizeof(ZPR3_with_zsub0_in_ZPR_3bBits), AArch64::ZPR3_with_zsub0_in_ZPR_3bRegClassID, 48, 1, true },
  { ZPR3_with_zsub1_in_ZPR_3b, ZPR3_with_zsub1_in_ZPR_3bBits, 370, 8, sizeof(ZPR3_with_zsub1_in_ZPR_3bBits), AArch64::ZPR3_with_zsub1_in_ZPR_3bRegClassID, 48, 1, true },
  { ZPR3_with_zsub2_in_ZPR_3b, ZPR3_with_zsub2_in_ZPR_3bBits, 484, 8, sizeof(ZPR3_with_zsub2_in_ZPR_3bBits), AArch64::ZPR3_with_zsub2_in_ZPR_3bRegClassID, 48, 1, true },
  { ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b, ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bBits, 454, 7, sizeof(ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bBits), AArch64::ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID, 48, 1, true },
  { ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b, ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bBits, 338, 7, sizeof(ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bBits), AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClassID, 48, 1, true },
  { ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b, ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bBits, 510, 6, sizeof(ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bBits), AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID, 48, 1, true },
  { QQQQ, QQQQBits, 178, 32, sizeof(QQQQBits), AArch64::QQQQRegClassID, 64, 1, true },
  { ZPR4, ZPR4Bits, 150, 32, sizeof(ZPR4Bits), AArch64::ZPR4RegClassID, 64, 1, true },
  { QQQQ_with_qsub0_in_FPR128_lo, QQQQ_with_qsub0_in_FPR128_loBits, 1702, 16, sizeof(QQQQ_with_qsub0_in_FPR128_loBits), AArch64::QQQQ_with_qsub0_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub1_in_FPR128_lo, QQQQ_with_qsub1_in_FPR128_loBits, 1764, 16, sizeof(QQQQ_with_qsub1_in_FPR128_loBits), AArch64::QQQQ_with_qsub1_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub2_in_FPR128_lo, QQQQ_with_qsub2_in_FPR128_loBits, 1944, 16, sizeof(QQQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQQ_with_qsub2_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub3_in_FPR128_lo, QQQQ_with_qsub3_in_FPR128_loBits, 2188, 16, sizeof(QQQQ_with_qsub3_in_FPR128_loBits), AArch64::QQQQ_with_qsub3_in_FPR128_loRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_4b, ZPR4_with_zsub1_in_ZPR_4bBits, 1000, 16, sizeof(ZPR4_with_zsub1_in_ZPR_4bBits), AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub2_in_ZPR_4b, ZPR4_with_zsub2_in_ZPR_4bBits, 1170, 16, sizeof(ZPR4_with_zsub2_in_ZPR_4bBits), AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub3_in_ZPR_4b, ZPR4_with_zsub3_in_ZPR_4bBits, 1284, 16, sizeof(ZPR4_with_zsub3_in_ZPR_4bBits), AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo, ZPR4_with_zsub_in_FPR128_loBits, 2397, 16, sizeof(ZPR4_with_zsub_in_FPR128_loBits), AArch64::ZPR4_with_zsub_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo, QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loBits, 1731, 15, sizeof(QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loBits), AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo, QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits, 1973, 15, sizeof(QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo, QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits, 2279, 15, sizeof(QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits), AArch64::QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b, ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bBits, 1140, 15, sizeof(ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bBits), AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b, ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits, 1310, 15, sizeof(ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits), AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bBits, 968, 15, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClassID, 64, 1, true },
  { QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo, QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits, 1911, 14, sizeof(QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loBits), AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID, 64, 1, true },
  { QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo, QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits, 2217, 14, sizeof(QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits), AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b, ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits, 1254, 14, sizeof(ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bBits), AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bBits, 1196, 14, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID, 64, 1, true },
  { QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo, QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits, 2155, 13, sizeof(QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loBits), AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bBits, 1366, 13, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID, 64, 1, true },
  { ZPR4_with_zsub0_in_ZPR_3b, ZPR4_with_zsub0_in_ZPR_3bBits, 254, 8, sizeof(ZPR4_with_zsub0_in_ZPR_3bBits), AArch64::ZPR4_with_zsub0_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_3b, ZPR4_with_zsub1_in_ZPR_3bBits, 428, 8, sizeof(ZPR4_with_zsub1_in_ZPR_3bBits), AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub2_in_ZPR_3b, ZPR4_with_zsub2_in_ZPR_3bBits, 598, 8, sizeof(ZPR4_with_zsub2_in_ZPR_3bBits), AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub3_in_ZPR_3b, ZPR4_with_zsub3_in_ZPR_3bBits, 712, 8, sizeof(ZPR4_with_zsub3_in_ZPR_3bBits), AArch64::ZPR4_with_zsub3_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b, ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bBits, 568, 7, sizeof(ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bBits), AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b, ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits, 738, 7, sizeof(ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits), AArch64::ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bBits, 396, 7, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b, ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits, 682, 6, sizeof(ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bBits), AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bBits, 624, 6, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID, 64, 1, true },
  { ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b, ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bBits, 794, 5, sizeof(ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bBits), AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID, 64, 1, true },
};

// AArch64 Dwarf<->LLVM register mappings.
extern const MCRegisterInfo::DwarfLLVMRegPair AArch64DwarfFlavour0Dwarf2L[] = {
  { 0U, AArch64::W0 },
  { 1U, AArch64::W1 },
  { 2U, AArch64::W2 },
  { 3U, AArch64::W3 },
  { 4U, AArch64::W4 },
  { 5U, AArch64::W5 },
  { 6U, AArch64::W6 },
  { 7U, AArch64::W7 },
  { 8U, AArch64::W8 },
  { 9U, AArch64::W9 },
  { 10U, AArch64::W10 },
  { 11U, AArch64::W11 },
  { 12U, AArch64::W12 },
  { 13U, AArch64::W13 },
  { 14U, AArch64::W14 },
  { 15U, AArch64::W15 },
  { 16U, AArch64::W16 },
  { 17U, AArch64::W17 },
  { 18U, AArch64::W18 },
  { 19U, AArch64::W19 },
  { 20U, AArch64::W20 },
  { 21U, AArch64::W21 },
  { 22U, AArch64::W22 },
  { 23U, AArch64::W23 },
  { 24U, AArch64::W24 },
  { 25U, AArch64::W25 },
  { 26U, AArch64::W26 },
  { 27U, AArch64::W27 },
  { 28U, AArch64::W28 },
  { 29U, AArch64::W29 },
  { 30U, AArch64::W30 },
  { 31U, AArch64::WSP },
  { 47U, AArch64::FFR },
  { 48U, AArch64::P0 },
  { 49U, AArch64::P1 },
  { 50U, AArch64::P2 },
  { 51U, AArch64::P3 },
  { 52U, AArch64::P4 },
  { 53U, AArch64::P5 },
  { 54U, AArch64::P6 },
  { 55U, AArch64::P7 },
  { 56U, AArch64::P8 },
  { 57U, AArch64::P9 },
  { 58U, AArch64::P10 },
  { 59U, AArch64::P11 },
  { 60U, AArch64::P12 },
  { 61U, AArch64::P13 },
  { 62U, AArch64::P14 },
  { 63U, AArch64::P15 },
  { 64U, AArch64::B0 },
  { 65U, AArch64::B1 },
  { 66U, AArch64::B2 },
  { 67U, AArch64::B3 },
  { 68U, AArch64::B4 },
  { 69U, AArch64::B5 },
  { 70U, AArch64::B6 },
  { 71U, AArch64::B7 },
  { 72U, AArch64::B8 },
  { 73U, AArch64::B9 },
  { 74U, AArch64::B10 },
  { 75U, AArch64::B11 },
  { 76U, AArch64::B12 },
  { 77U, AArch64::B13 },
  { 78U, AArch64::B14 },
  { 79U, AArch64::B15 },
  { 80U, AArch64::B16 },
  { 81U, AArch64::B17 },
  { 82U, AArch64::B18 },
  { 83U, AArch64::B19 },
  { 84U, AArch64::B20 },
  { 85U, AArch64::B21 },
  { 86U, AArch64::B22 },
  { 87U, AArch64::B23 },
  { 88U, AArch64::B24 },
  { 89U, AArch64::B25 },
  { 90U, AArch64::B26 },
  { 91U, AArch64::B27 },
  { 92U, AArch64::B28 },
  { 93U, AArch64::B29 },
  { 94U, AArch64::B30 },
  { 95U, AArch64::B31 },
  { 96U, AArch64::Z0 },
  { 97U, AArch64::Z1 },
  { 98U, AArch64::Z2 },
  { 99U, AArch64::Z3 },
  { 100U, AArch64::Z4 },
  { 101U, AArch64::Z5 },
  { 102U, AArch64::Z6 },
  { 103U, AArch64::Z7 },
  { 104U, AArch64::Z8 },
  { 105U, AArch64::Z9 },
  { 106U, AArch64::Z10 },
  { 107U, AArch64::Z11 },
  { 108U, AArch64::Z12 },
  { 109U, AArch64::Z13 },
  { 110U, AArch64::Z14 },
  { 111U, AArch64::Z15 },
  { 112U, AArch64::Z16 },
  { 113U, AArch64::Z17 },
  { 114U, AArch64::Z18 },
  { 115U, AArch64::Z19 },
  { 116U, AArch64::Z20 },
  { 117U, AArch64::Z21 },
  { 118U, AArch64::Z22 },
  { 119U, AArch64::Z23 },
  { 120U, AArch64::Z24 },
  { 121U, AArch64::Z25 },
  { 122U, AArch64::Z26 },
  { 123U, AArch64::Z27 },
  { 124U, AArch64::Z28 },
  { 125U, AArch64::Z29 },
  { 126U, AArch64::Z30 },
  { 127U, AArch64::Z31 },
};
extern const unsigned AArch64DwarfFlavour0Dwarf2LSize = array_lengthof(AArch64DwarfFlavour0Dwarf2L);

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64EHFlavour0Dwarf2L[] = {
  { 0U, AArch64::W0 },
  { 1U, AArch64::W1 },
  { 2U, AArch64::W2 },
  { 3U, AArch64::W3 },
  { 4U, AArch64::W4 },
  { 5U, AArch64::W5 },
  { 6U, AArch64::W6 },
  { 7U, AArch64::W7 },
  { 8U, AArch64::W8 },
  { 9U, AArch64::W9 },
  { 10U, AArch64::W10 },
  { 11U, AArch64::W11 },
  { 12U, AArch64::W12 },
  { 13U, AArch64::W13 },
  { 14U, AArch64::W14 },
  { 15U, AArch64::W15 },
  { 16U, AArch64::W16 },
  { 17U, AArch64::W17 },
  { 18U, AArch64::W18 },
  { 19U, AArch64::W19 },
  { 20U, AArch64::W20 },
  { 21U, AArch64::W21 },
  { 22U, AArch64::W22 },
  { 23U, AArch64::W23 },
  { 24U, AArch64::W24 },
  { 25U, AArch64::W25 },
  { 26U, AArch64::W26 },
  { 27U, AArch64::W27 },
  { 28U, AArch64::W28 },
  { 29U, AArch64::W29 },
  { 30U, AArch64::W30 },
  { 31U, AArch64::WSP },
  { 47U, AArch64::FFR },
  { 48U, AArch64::P0 },
  { 49U, AArch64::P1 },
  { 50U, AArch64::P2 },
  { 51U, AArch64::P3 },
  { 52U, AArch64::P4 },
  { 53U, AArch64::P5 },
  { 54U, AArch64::P6 },
  { 55U, AArch64::P7 },
  { 56U, AArch64::P8 },
  { 57U, AArch64::P9 },
  { 58U, AArch64::P10 },
  { 59U, AArch64::P11 },
  { 60U, AArch64::P12 },
  { 61U, AArch64::P13 },
  { 62U, AArch64::P14 },
  { 63U, AArch64::P15 },
  { 64U, AArch64::B0 },
  { 65U, AArch64::B1 },
  { 66U, AArch64::B2 },
  { 67U, AArch64::B3 },
  { 68U, AArch64::B4 },
  { 69U, AArch64::B5 },
  { 70U, AArch64::B6 },
  { 71U, AArch64::B7 },
  { 72U, AArch64::B8 },
  { 73U, AArch64::B9 },
  { 74U, AArch64::B10 },
  { 75U, AArch64::B11 },
  { 76U, AArch64::B12 },
  { 77U, AArch64::B13 },
  { 78U, AArch64::B14 },
  { 79U, AArch64::B15 },
  { 80U, AArch64::B16 },
  { 81U, AArch64::B17 },
  { 82U, AArch64::B18 },
  { 83U, AArch64::B19 },
  { 84U, AArch64::B20 },
  { 85U, AArch64::B21 },
  { 86U, AArch64::B22 },
  { 87U, AArch64::B23 },
  { 88U, AArch64::B24 },
  { 89U, AArch64::B25 },
  { 90U, AArch64::B26 },
  { 91U, AArch64::B27 },
  { 92U, AArch64::B28 },
  { 93U, AArch64::B29 },
  { 94U, AArch64::B30 },
  { 95U, AArch64::B31 },
  { 96U, AArch64::Z0 },
  { 97U, AArch64::Z1 },
  { 98U, AArch64::Z2 },
  { 99U, AArch64::Z3 },
  { 100U, AArch64::Z4 },
  { 101U, AArch64::Z5 },
  { 102U, AArch64::Z6 },
  { 103U, AArch64::Z7 },
  { 104U, AArch64::Z8 },
  { 105U, AArch64::Z9 },
  { 106U, AArch64::Z10 },
  { 107U, AArch64::Z11 },
  { 108U, AArch64::Z12 },
  { 109U, AArch64::Z13 },
  { 110U, AArch64::Z14 },
  { 111U, AArch64::Z15 },
  { 112U, AArch64::Z16 },
  { 113U, AArch64::Z17 },
  { 114U, AArch64::Z18 },
  { 115U, AArch64::Z19 },
  { 116U, AArch64::Z20 },
  { 117U, AArch64::Z21 },
  { 118U, AArch64::Z22 },
  { 119U, AArch64::Z23 },
  { 120U, AArch64::Z24 },
  { 121U, AArch64::Z25 },
  { 122U, AArch64::Z26 },
  { 123U, AArch64::Z27 },
  { 124U, AArch64::Z28 },
  { 125U, AArch64::Z29 },
  { 126U, AArch64::Z30 },
  { 127U, AArch64::Z31 },
};
extern const unsigned AArch64EHFlavour0Dwarf2LSize = array_lengthof(AArch64EHFlavour0Dwarf2L);

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64DwarfFlavour0L2Dwarf[] = {
  { AArch64::FFR, 47U },
  { AArch64::FP, 29U },
  { AArch64::LR, 30U },
  { AArch64::SP, 31U },
  { AArch64::WSP, 31U },
  { AArch64::WZR, 31U },
  { AArch64::XZR, 31U },
  { AArch64::B0, 64U },
  { AArch64::B1, 65U },
  { AArch64::B2, 66U },
  { AArch64::B3, 67U },
  { AArch64::B4, 68U },
  { AArch64::B5, 69U },
  { AArch64::B6, 70U },
  { AArch64::B7, 71U },
  { AArch64::B8, 72U },
  { AArch64::B9, 73U },
  { AArch64::B10, 74U },
  { AArch64::B11, 75U },
  { AArch64::B12, 76U },
  { AArch64::B13, 77U },
  { AArch64::B14, 78U },
  { AArch64::B15, 79U },
  { AArch64::B16, 80U },
  { AArch64::B17, 81U },
  { AArch64::B18, 82U },
  { AArch64::B19, 83U },
  { AArch64::B20, 84U },
  { AArch64::B21, 85U },
  { AArch64::B22, 86U },
  { AArch64::B23, 87U },
  { AArch64::B24, 88U },
  { AArch64::B25, 89U },
  { AArch64::B26, 90U },
  { AArch64::B27, 91U },
  { AArch64::B28, 92U },
  { AArch64::B29, 93U },
  { AArch64::B30, 94U },
  { AArch64::B31, 95U },
  { AArch64::D0, 64U },
  { AArch64::D1, 65U },
  { AArch64::D2, 66U },
  { AArch64::D3, 67U },
  { AArch64::D4, 68U },
  { AArch64::D5, 69U },
  { AArch64::D6, 70U },
  { AArch64::D7, 71U },
  { AArch64::D8, 72U },
  { AArch64::D9, 73U },
  { AArch64::D10, 74U },
  { AArch64::D11, 75U },
  { AArch64::D12, 76U },
  { AArch64::D13, 77U },
  { AArch64::D14, 78U },
  { AArch64::D15, 79U },
  { AArch64::D16, 80U },
  { AArch64::D17, 81U },
  { AArch64::D18, 82U },
  { AArch64::D19, 83U },
  { AArch64::D20, 84U },
  { AArch64::D21, 85U },
  { AArch64::D22, 86U },
  { AArch64::D23, 87U },
  { AArch64::D24, 88U },
  { AArch64::D25, 89U },
  { AArch64::D26, 90U },
  { AArch64::D27, 91U },
  { AArch64::D28, 92U },
  { AArch64::D29, 93U },
  { AArch64::D30, 94U },
  { AArch64::D31, 95U },
  { AArch64::H0, 64U },
  { AArch64::H1, 65U },
  { AArch64::H2, 66U },
  { AArch64::H3, 67U },
  { AArch64::H4, 68U },
  { AArch64::H5, 69U },
  { AArch64::H6, 70U },
  { AArch64::H7, 71U },
  { AArch64::H8, 72U },
  { AArch64::H9, 73U },
  { AArch64::H10, 74U },
  { AArch64::H11, 75U },
  { AArch64::H12, 76U },
  { AArch64::H13, 77U },
  { AArch64::H14, 78U },
  { AArch64::H15, 79U },
  { AArch64::H16, 80U },
  { AArch64::H17, 81U },
  { AArch64::H18, 82U },
  { AArch64::H19, 83U },
  { AArch64::H20, 84U },
  { AArch64::H21, 85U },
  { AArch64::H22, 86U },
  { AArch64::H23, 87U },
  { AArch64::H24, 88U },
  { AArch64::H25, 89U },
  { AArch64::H26, 90U },
  { AArch64::H27, 91U },
  { AArch64::H28, 92U },
  { AArch64::H29, 93U },
  { AArch64::H30, 94U },
  { AArch64::H31, 95U },
  { AArch64::P0, 48U },
  { AArch64::P1, 49U },
  { AArch64::P2, 50U },
  { AArch64::P3, 51U },
  { AArch64::P4, 52U },
  { AArch64::P5, 53U },
  { AArch64::P6, 54U },
  { AArch64::P7, 55U },
  { AArch64::P8, 56U },
  { AArch64::P9, 57U },
  { AArch64::P10, 58U },
  { AArch64::P11, 59U },
  { AArch64::P12, 60U },
  { AArch64::P13, 61U },
  { AArch64::P14, 62U },
  { AArch64::P15, 63U },
  { AArch64::Q0, 64U },
  { AArch64::Q1, 65U },
  { AArch64::Q2, 66U },
  { AArch64::Q3, 67U },
  { AArch64::Q4, 68U },
  { AArch64::Q5, 69U },
  { AArch64::Q6, 70U },
  { AArch64::Q7, 71U },
  { AArch64::Q8, 72U },
  { AArch64::Q9, 73U },
  { AArch64::Q10, 74U },
  { AArch64::Q11, 75U },
  { AArch64::Q12, 76U },
  { AArch64::Q13, 77U },
  { AArch64::Q14, 78U },
  { AArch64::Q15, 79U },
  { AArch64::Q16, 80U },
  { AArch64::Q17, 81U },
  { AArch64::Q18, 82U },
  { AArch64::Q19, 83U },
  { AArch64::Q20, 84U },
  { AArch64::Q21, 85U },
  { AArch64::Q22, 86U },
  { AArch64::Q23, 87U },
  { AArch64::Q24, 88U },
  { AArch64::Q25, 89U },
  { AArch64::Q26, 90U },
  { AArch64::Q27, 91U },
  { AArch64::Q28, 92U },
  { AArch64::Q29, 93U },
  { AArch64::Q30, 94U },
  { AArch64::Q31, 95U },
  { AArch64::S0, 64U },
  { AArch64::S1, 65U },
  { AArch64::S2, 66U },
  { AArch64::S3, 67U },
  { AArch64::S4, 68U },
  { AArch64::S5, 69U },
  { AArch64::S6, 70U },
  { AArch64::S7, 71U },
  { AArch64::S8, 72U },
  { AArch64::S9, 73U },
  { AArch64::S10, 74U },
  { AArch64::S11, 75U },
  { AArch64::S12, 76U },
  { AArch64::S13, 77U },
  { AArch64::S14, 78U },
  { AArch64::S15, 79U },
  { AArch64::S16, 80U },
  { AArch64::S17, 81U },
  { AArch64::S18, 82U },
  { AArch64::S19, 83U },
  { AArch64::S20, 84U },
  { AArch64::S21, 85U },
  { AArch64::S22, 86U },
  { AArch64::S23, 87U },
  { AArch64::S24, 88U },
  { AArch64::S25, 89U },
  { AArch64::S26, 90U },
  { AArch64::S27, 91U },
  { AArch64::S28, 92U },
  { AArch64::S29, 93U },
  { AArch64::S30, 94U },
  { AArch64::S31, 95U },
  { AArch64::W0, 0U },
  { AArch64::W1, 1U },
  { AArch64::W2, 2U },
  { AArch64::W3, 3U },
  { AArch64::W4, 4U },
  { AArch64::W5, 5U },
  { AArch64::W6, 6U },
  { AArch64::W7, 7U },
  { AArch64::W8, 8U },
  { AArch64::W9, 9U },
  { AArch64::W10, 10U },
  { AArch64::W11, 11U },
  { AArch64::W12, 12U },
  { AArch64::W13, 13U },
  { AArch64::W14, 14U },
  { AArch64::W15, 15U },
  { AArch64::W16, 16U },
  { AArch64::W17, 17U },
  { AArch64::W18, 18U },
  { AArch64::W19, 19U },
  { AArch64::W20, 20U },
  { AArch64::W21, 21U },
  { AArch64::W22, 22U },
  { AArch64::W23, 23U },
  { AArch64::W24, 24U },
  { AArch64::W25, 25U },
  { AArch64::W26, 26U },
  { AArch64::W27, 27U },
  { AArch64::W28, 28U },
  { AArch64::W29, 29U },
  { AArch64::W30, 30U },
  { AArch64::X0, 0U },
  { AArch64::X1, 1U },
  { AArch64::X2, 2U },
  { AArch64::X3, 3U },
  { AArch64::X4, 4U },
  { AArch64::X5, 5U },
  { AArch64::X6, 6U },
  { AArch64::X7, 7U },
  { AArch64::X8, 8U },
  { AArch64::X9, 9U },
  { AArch64::X10, 10U },
  { AArch64::X11, 11U },
  { AArch64::X12, 12U },
  { AArch64::X13, 13U },
  { AArch64::X14, 14U },
  { AArch64::X15, 15U },
  { AArch64::X16, 16U },
  { AArch64::X17, 17U },
  { AArch64::X18, 18U },
  { AArch64::X19, 19U },
  { AArch64::X20, 20U },
  { AArch64::X21, 21U },
  { AArch64::X22, 22U },
  { AArch64::X23, 23U },
  { AArch64::X24, 24U },
  { AArch64::X25, 25U },
  { AArch64::X26, 26U },
  { AArch64::X27, 27U },
  { AArch64::X28, 28U },
  { AArch64::Z0, 96U },
  { AArch64::Z1, 97U },
  { AArch64::Z2, 98U },
  { AArch64::Z3, 99U },
  { AArch64::Z4, 100U },
  { AArch64::Z5, 101U },
  { AArch64::Z6, 102U },
  { AArch64::Z7, 103U },
  { AArch64::Z8, 104U },
  { AArch64::Z9, 105U },
  { AArch64::Z10, 106U },
  { AArch64::Z11, 107U },
  { AArch64::Z12, 108U },
  { AArch64::Z13, 109U },
  { AArch64::Z14, 110U },
  { AArch64::Z15, 111U },
  { AArch64::Z16, 112U },
  { AArch64::Z17, 113U },
  { AArch64::Z18, 114U },
  { AArch64::Z19, 115U },
  { AArch64::Z20, 116U },
  { AArch64::Z21, 117U },
  { AArch64::Z22, 118U },
  { AArch64::Z23, 119U },
  { AArch64::Z24, 120U },
  { AArch64::Z25, 121U },
  { AArch64::Z26, 122U },
  { AArch64::Z27, 123U },
  { AArch64::Z28, 124U },
  { AArch64::Z29, 125U },
  { AArch64::Z30, 126U },
  { AArch64::Z31, 127U },
};
extern const unsigned AArch64DwarfFlavour0L2DwarfSize = array_lengthof(AArch64DwarfFlavour0L2Dwarf);

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64EHFlavour0L2Dwarf[] = {
  { AArch64::FFR, 47U },
  { AArch64::FP, 29U },
  { AArch64::LR, 30U },
  { AArch64::SP, 31U },
  { AArch64::WSP, 31U },
  { AArch64::WZR, 31U },
  { AArch64::XZR, 31U },
  { AArch64::B0, 64U },
  { AArch64::B1, 65U },
  { AArch64::B2, 66U },
  { AArch64::B3, 67U },
  { AArch64::B4, 68U },
  { AArch64::B5, 69U },
  { AArch64::B6, 70U },
  { AArch64::B7, 71U },
  { AArch64::B8, 72U },
  { AArch64::B9, 73U },
  { AArch64::B10, 74U },
  { AArch64::B11, 75U },
  { AArch64::B12, 76U },
  { AArch64::B13, 77U },
  { AArch64::B14, 78U },
  { AArch64::B15, 79U },
  { AArch64::B16, 80U },
  { AArch64::B17, 81U },
  { AArch64::B18, 82U },
  { AArch64::B19, 83U },
  { AArch64::B20, 84U },
  { AArch64::B21, 85U },
  { AArch64::B22, 86U },
  { AArch64::B23, 87U },
  { AArch64::B24, 88U },
  { AArch64::B25, 89U },
  { AArch64::B26, 90U },
  { AArch64::B27, 91U },
  { AArch64::B28, 92U },
  { AArch64::B29, 93U },
  { AArch64::B30, 94U },
  { AArch64::B31, 95U },
  { AArch64::D0, 64U },
  { AArch64::D1, 65U },
  { AArch64::D2, 66U },
  { AArch64::D3, 67U },
  { AArch64::D4, 68U },
  { AArch64::D5, 69U },
  { AArch64::D6, 70U },
  { AArch64::D7, 71U },
  { AArch64::D8, 72U },
  { AArch64::D9, 73U },
  { AArch64::D10, 74U },
  { AArch64::D11, 75U },
  { AArch64::D12, 76U },
  { AArch64::D13, 77U },
  { AArch64::D14, 78U },
  { AArch64::D15, 79U },
  { AArch64::D16, 80U },
  { AArch64::D17, 81U },
  { AArch64::D18, 82U },
  { AArch64::D19, 83U },
  { AArch64::D20, 84U },
  { AArch64::D21, 85U },
  { AArch64::D22, 86U },
  { AArch64::D23, 87U },
  { AArch64::D24, 88U },
  { AArch64::D25, 89U },
  { AArch64::D26, 90U },
  { AArch64::D27, 91U },
  { AArch64::D28, 92U },
  { AArch64::D29, 93U },
  { AArch64::D30, 94U },
  { AArch64::D31, 95U },
  { AArch64::H0, 64U },
  { AArch64::H1, 65U },
  { AArch64::H2, 66U },
  { AArch64::H3, 67U },
  { AArch64::H4, 68U },
  { AArch64::H5, 69U },
  { AArch64::H6, 70U },
  { AArch64::H7, 71U },
  { AArch64::H8, 72U },
  { AArch64::H9, 73U },
  { AArch64::H10, 74U },
  { AArch64::H11, 75U },
  { AArch64::H12, 76U },
  { AArch64::H13, 77U },
  { AArch64::H14, 78U },
  { AArch64::H15, 79U },
  { AArch64::H16, 80U },
  { AArch64::H17, 81U },
  { AArch64::H18, 82U },
  { AArch64::H19, 83U },
  { AArch64::H20, 84U },
  { AArch64::H21, 85U },
  { AArch64::H22, 86U },
  { AArch64::H23, 87U },
  { AArch64::H24, 88U },
  { AArch64::H25, 89U },
  { AArch64::H26, 90U },
  { AArch64::H27, 91U },
  { AArch64::H28, 92U },
  { AArch64::H29, 93U },
  { AArch64::H30, 94U },
  { AArch64::H31, 95U },
  { AArch64::P0, 48U },
  { AArch64::P1, 49U },
  { AArch64::P2, 50U },
  { AArch64::P3, 51U },
  { AArch64::P4, 52U },
  { AArch64::P5, 53U },
  { AArch64::P6, 54U },
  { AArch64::P7, 55U },
  { AArch64::P8, 56U },
  { AArch64::P9, 57U },
  { AArch64::P10, 58U },
  { AArch64::P11, 59U },
  { AArch64::P12, 60U },
  { AArch64::P13, 61U },
  { AArch64::P14, 62U },
  { AArch64::P15, 63U },
  { AArch64::Q0, 64U },
  { AArch64::Q1, 65U },
  { AArch64::Q2, 66U },
  { AArch64::Q3, 67U },
  { AArch64::Q4, 68U },
  { AArch64::Q5, 69U },
  { AArch64::Q6, 70U },
  { AArch64::Q7, 71U },
  { AArch64::Q8, 72U },
  { AArch64::Q9, 73U },
  { AArch64::Q10, 74U },
  { AArch64::Q11, 75U },
  { AArch64::Q12, 76U },
  { AArch64::Q13, 77U },
  { AArch64::Q14, 78U },
  { AArch64::Q15, 79U },
  { AArch64::Q16, 80U },
  { AArch64::Q17, 81U },
  { AArch64::Q18, 82U },
  { AArch64::Q19, 83U },
  { AArch64::Q20, 84U },
  { AArch64::Q21, 85U },
  { AArch64::Q22, 86U },
  { AArch64::Q23, 87U },
  { AArch64::Q24, 88U },
  { AArch64::Q25, 89U },
  { AArch64::Q26, 90U },
  { AArch64::Q27, 91U },
  { AArch64::Q28, 92U },
  { AArch64::Q29, 93U },
  { AArch64::Q30, 94U },
  { AArch64::Q31, 95U },
  { AArch64::S0, 64U },
  { AArch64::S1, 65U },
  { AArch64::S2, 66U },
  { AArch64::S3, 67U },
  { AArch64::S4, 68U },
  { AArch64::S5, 69U },
  { AArch64::S6, 70U },
  { AArch64::S7, 71U },
  { AArch64::S8, 72U },
  { AArch64::S9, 73U },
  { AArch64::S10, 74U },
  { AArch64::S11, 75U },
  { AArch64::S12, 76U },
  { AArch64::S13, 77U },
  { AArch64::S14, 78U },
  { AArch64::S15, 79U },
  { AArch64::S16, 80U },
  { AArch64::S17, 81U },
  { AArch64::S18, 82U },
  { AArch64::S19, 83U },
  { AArch64::S20, 84U },
  { AArch64::S21, 85U },
  { AArch64::S22, 86U },
  { AArch64::S23, 87U },
  { AArch64::S24, 88U },
  { AArch64::S25, 89U },
  { AArch64::S26, 90U },
  { AArch64::S27, 91U },
  { AArch64::S28, 92U },
  { AArch64::S29, 93U },
  { AArch64::S30, 94U },
  { AArch64::S31, 95U },
  { AArch64::W0, 0U },
  { AArch64::W1, 1U },
  { AArch64::W2, 2U },
  { AArch64::W3, 3U },
  { AArch64::W4, 4U },
  { AArch64::W5, 5U },
  { AArch64::W6, 6U },
  { AArch64::W7, 7U },
  { AArch64::W8, 8U },
  { AArch64::W9, 9U },
  { AArch64::W10, 10U },
  { AArch64::W11, 11U },
  { AArch64::W12, 12U },
  { AArch64::W13, 13U },
  { AArch64::W14, 14U },
  { AArch64::W15, 15U },
  { AArch64::W16, 16U },
  { AArch64::W17, 17U },
  { AArch64::W18, 18U },
  { AArch64::W19, 19U },
  { AArch64::W20, 20U },
  { AArch64::W21, 21U },
  { AArch64::W22, 22U },
  { AArch64::W23, 23U },
  { AArch64::W24, 24U },
  { AArch64::W25, 25U },
  { AArch64::W26, 26U },
  { AArch64::W27, 27U },
  { AArch64::W28, 28U },
  { AArch64::W29, 29U },
  { AArch64::W30, 30U },
  { AArch64::X0, 0U },
  { AArch64::X1, 1U },
  { AArch64::X2, 2U },
  { AArch64::X3, 3U },
  { AArch64::X4, 4U },
  { AArch64::X5, 5U },
  { AArch64::X6, 6U },
  { AArch64::X7, 7U },
  { AArch64::X8, 8U },
  { AArch64::X9, 9U },
  { AArch64::X10, 10U },
  { AArch64::X11, 11U },
  { AArch64::X12, 12U },
  { AArch64::X13, 13U },
  { AArch64::X14, 14U },
  { AArch64::X15, 15U },
  { AArch64::X16, 16U },
  { AArch64::X17, 17U },
  { AArch64::X18, 18U },
  { AArch64::X19, 19U },
  { AArch64::X20, 20U },
  { AArch64::X21, 21U },
  { AArch64::X22, 22U },
  { AArch64::X23, 23U },
  { AArch64::X24, 24U },
  { AArch64::X25, 25U },
  { AArch64::X26, 26U },
  { AArch64::X27, 27U },
  { AArch64::X28, 28U },
  { AArch64::Z0, 96U },
  { AArch64::Z1, 97U },
  { AArch64::Z2, 98U },
  { AArch64::Z3, 99U },
  { AArch64::Z4, 100U },
  { AArch64::Z5, 101U },
  { AArch64::Z6, 102U },
  { AArch64::Z7, 103U },
  { AArch64::Z8, 104U },
  { AArch64::Z9, 105U },
  { AArch64::Z10, 106U },
  { AArch64::Z11, 107U },
  { AArch64::Z12, 108U },
  { AArch64::Z13, 109U },
  { AArch64::Z14, 110U },
  { AArch64::Z15, 111U },
  { AArch64::Z16, 112U },
  { AArch64::Z17, 113U },
  { AArch64::Z18, 114U },
  { AArch64::Z19, 115U },
  { AArch64::Z20, 116U },
  { AArch64::Z21, 117U },
  { AArch64::Z22, 118U },
  { AArch64::Z23, 119U },
  { AArch64::Z24, 120U },
  { AArch64::Z25, 121U },
  { AArch64::Z26, 122U },
  { AArch64::Z27, 123U },
  { AArch64::Z28, 124U },
  { AArch64::Z29, 125U },
  { AArch64::Z30, 126U },
  { AArch64::Z31, 127U },
};
extern const unsigned AArch64EHFlavour0L2DwarfSize = array_lengthof(AArch64EHFlavour0L2Dwarf);

extern const uint16_t AArch64RegEncodingTable[] = {
  0,
  0,
  29,
  30,
  0,
  31,
  31,
  31,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  31,
  30,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  29,
  30,
  31,
  28,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
};
static inline void InitAArch64MCRegisterInfo(MCRegisterInfo *RI, unsigned RA, unsigned DwarfFlavour = 0, unsigned EHFlavour = 0, unsigned PC = 0) {
  RI->InitMCRegisterInfo(AArch64RegDesc, 661, RA, PC, AArch64MCRegisterClasses, 100, AArch64RegUnitRoots, 115, AArch64RegDiffLists, AArch64LaneMaskLists, AArch64RegStrings, AArch64RegClassStrings, AArch64SubRegIdxLists, 100,
AArch64SubRegIdxRanges, AArch64RegEncodingTable);

  switch (DwarfFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    RI->mapDwarfRegsToLLVMRegs(AArch64DwarfFlavour0Dwarf2L, AArch64DwarfFlavour0Dwarf2LSize, false);
    break;
  }
  switch (EHFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    RI->mapDwarfRegsToLLVMRegs(AArch64EHFlavour0Dwarf2L, AArch64EHFlavour0Dwarf2LSize, true);
    break;
  }
  switch (DwarfFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    RI->mapLLVMRegsToDwarfRegs(AArch64DwarfFlavour0L2Dwarf, AArch64DwarfFlavour0L2DwarfSize, false);
    break;
  }
  switch (EHFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    RI->mapLLVMRegsToDwarfRegs(AArch64EHFlavour0L2Dwarf, AArch64EHFlavour0L2DwarfSize, true);
    break;
  }
}

} // end namespace llvm

#endif // GET_REGINFO_MC_DESC

/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Register Information Header Fragment                                       *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_REGINFO_HEADER
#undef GET_REGINFO_HEADER

#include "llvm/CodeGen/TargetRegisterInfo.h"

namespace llvm {

class AArch64FrameLowering;

struct AArch64GenRegisterInfo : public TargetRegisterInfo {
  explicit AArch64GenRegisterInfo(unsigned RA, unsigned D = 0, unsigned E = 0,
      unsigned PC = 0, unsigned HwMode = 0);
  unsigned composeSubRegIndicesImpl(unsigned, unsigned) const override;
  LaneBitmask composeSubRegIndexLaneMaskImpl(unsigned, LaneBitmask) const override;
  LaneBitmask reverseComposeSubRegIndexLaneMaskImpl(unsigned, LaneBitmask) const override;
  const TargetRegisterClass *getSubClassWithSubReg(const TargetRegisterClass*, unsigned) const override;
  const RegClassWeight &getRegClassWeight(const TargetRegisterClass *RC) const override;
  unsigned getRegUnitWeight(unsigned RegUnit) const override;
  unsigned getNumRegPressureSets() const override;
  const char *getRegPressureSetName(unsigned Idx) const override;
  unsigned getRegPressureSetLimit(const MachineFunction &MF, unsigned Idx) const override;
  const int *getRegClassPressureSets(const TargetRegisterClass *RC) const override;
  const int *getRegUnitPressureSets(unsigned RegUnit) const override;
  ArrayRef<const char *> getRegMaskNames() const override;
  ArrayRef<const uint32_t *> getRegMasks() const override;
  /// Devirtualized TargetFrameLowering.
  static const AArch64FrameLowering *getFrameLowering(
      const MachineFunction &MF);
};

namespace AArch64 { // Register classes
  extern const TargetRegisterClass FPR8RegClass;
  extern const TargetRegisterClass FPR16RegClass;
  extern const TargetRegisterClass PPRRegClass;
  extern const TargetRegisterClass PPR_3bRegClass;
  extern const TargetRegisterClass GPR32allRegClass;
  extern const TargetRegisterClass FPR32RegClass;
  extern const TargetRegisterClass GPR32RegClass;
  extern const TargetRegisterClass GPR32spRegClass;
  extern const TargetRegisterClass GPR32commonRegClass;
  extern const TargetRegisterClass CCRRegClass;
  extern const TargetRegisterClass GPR32sponlyRegClass;
  extern const TargetRegisterClass WSeqPairsClassRegClass;
  extern const TargetRegisterClass WSeqPairsClass_with_sube32_in_GPR32commonRegClass;
  extern const TargetRegisterClass WSeqPairsClass_with_subo32_in_GPR32commonRegClass;
  extern const TargetRegisterClass WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClass;
  extern const TargetRegisterClass GPR64allRegClass;
  extern const TargetRegisterClass FPR64RegClass;
  extern const TargetRegisterClass GPR64RegClass;
  extern const TargetRegisterClass GPR64spRegClass;
  extern const TargetRegisterClass GPR64commonRegClass;
  extern const TargetRegisterClass tcGPR64RegClass;
  extern const TargetRegisterClass GPR64sponlyRegClass;
  extern const TargetRegisterClass DDRegClass;
  extern const TargetRegisterClass XSeqPairsClassRegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32commonRegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_subo64_in_GPR64commonRegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_sube64_in_tcGPR64RegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_subo64_in_tcGPR64RegClass;
  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClass;
  extern const TargetRegisterClass FPR128RegClass;
  extern const TargetRegisterClass ZPRRegClass;
  extern const TargetRegisterClass FPR128_loRegClass;
  extern const TargetRegisterClass ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR_3bRegClass;
  extern const TargetRegisterClass DDDRegClass;
  extern const TargetRegisterClass DDDDRegClass;
  extern const TargetRegisterClass QQRegClass;
  extern const TargetRegisterClass ZPR2RegClass;
  extern const TargetRegisterClass QQ_with_qsub0_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub0_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass QQQRegClass;
  extern const TargetRegisterClass ZPR3RegClass;
  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub0_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass QQQQRegClass;
  extern const TargetRegisterClass ZPR4RegClass;
  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub3_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub3_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass;
  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub0_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub3_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClass;
  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClass;
} // end namespace AArch64

} // end namespace llvm

#endif // GET_REGINFO_HEADER

/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Target Register and Register Classes Information                           *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#ifdef GET_REGINFO_TARGET_DESC
#undef GET_REGINFO_TARGET_DESC

namespace llvm {

extern const MCRegisterClass AArch64MCRegisterClasses[];

static const MVT::SimpleValueType VTLists[] = {
  /* 0 */ MVT::f32, MVT::i32, MVT::Other,
  /* 3 */ MVT::i64, MVT::Other,
  /* 5 */ MVT::f16, MVT::Other,
  /* 7 */ MVT::nxv16i1, MVT::nxv8i1, MVT::nxv4i1, MVT::nxv2i1, MVT::Other,
  /* 12 */ MVT::f64, MVT::i64, MVT::v2f32, MVT::v1f64, MVT::v8i8, MVT::v4i16, MVT::v2i32, MVT::v1i64, MVT::v4f16, MVT::Other,
  /* 22 */ MVT::v16i8, MVT::v8i16, MVT::v4i32, MVT::v2i64, MVT::v4f32, MVT::v2f64, MVT::f128, MVT::v8f16, MVT::Other,
  /* 31 */ MVT::v16i8, MVT::v8i16, MVT::v4i32, MVT::v2i64, MVT::v4f32, MVT::v2f64, MVT::v8f16, MVT::Other,
  /* 39 */ MVT::nxv16i8, MVT::nxv8i16, MVT::nxv4i32, MVT::nxv2i64, MVT::nxv2f16, MVT::nxv4f16, MVT::nxv8f16, MVT::nxv1f32, MVT::nxv2f32, MVT::nxv4f32, MVT::nxv1f64, MVT::nxv2f64, MVT::Other,
  /* 52 */ MVT::Untyped, MVT::Other,
};

static const char *const SubRegIndexNameTable[] = { "bsub", "dsub", "dsub0", "dsub1", "dsub2", "dsub3", "hsub", "qhisub", "qsub", "qsub0", "qsub1", "qsub2", "qsub3", "ssub", "sub_32", "sube32", "sube64", "subo32", "subo64", "zsub", "zsub0", "zsub1", "zsub2", "zsub3", "zsub_hi", "dsub1_then_bsub", "dsub1_then_hsub", "dsub1_then_ssub", "dsub3_then_bsub", "dsub3_then_hsub", "dsub3_then_ssub", "dsub2_then_bsub", "dsub2_then_hsub", "dsub2_then_ssub", "qsub1_then_bsub", "qsub1_then_dsub", "qsub1_then_hsub", "qsub1_then_ssub", "qsub3_then_bsub", "qsub3_then_dsub", "qsub3_then_hsub", "qsub3_then_ssub", "qsub2_then_bsub", "qsub2_then_dsub", "qsub2_then_hsub", "qsub2_then_ssub", "subo64_then_sub_32", "zsub1_then_bsub", "zsub1_then_dsub", "zsub1_then_hsub", "zsub1_then_ssub", "zsub1_then_zsub", "zsub1_then_zsub_hi", "zsub3_then_bsub", "zsub3_then_dsub", "zsub3_then_hsub", "zsub3_then_ssub", "zsub3_then_zsub", "zsub3_then_zsub_hi", "zsub2_then_bsub", "zsub2_then_dsub", "zsub2_then_hsub", "zsub2_then_ssub", "zsub2_then_zsub", "zsub2_then_zsub_hi", "dsub0_dsub1", "dsub0_dsub1_dsub2", "dsub1_dsub2", "dsub1_dsub2_dsub3", "dsub2_dsub3", "dsub_qsub1_then_dsub", "dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub", "dsub_qsub1_then_dsub_qsub2_then_dsub", "qsub0_qsub1", "qsub0_qsub1_qsub2", "qsub1_qsub2", "qsub1_qsub2_qsub3", "qsub2_qsub3", "qsub1_then_dsub_qsub2_then_dsub", "qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub", "qsub2_then_dsub_qsub3_then_dsub", "sub_32_subo64_then_sub_32", "dsub_zsub1_then_dsub", "zsub_zsub1_then_zsub", "dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub", "dsub_zsub1_then_dsub_zsub2_then_dsub", "zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub", "zsub_zsub1_then_zsub_zsub2_then_zsub", "zsub0_zsub1", "zsub0_zsub1_zsub2", "zsub1_zsub2", "zsub1_zsub2_zsub3", "zsub2_zsub3", "zsub1_then_dsub_zsub2_then_dsub", "zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub", "zsub1_then_zsub_zsub2_then_zsub", "zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub", "zsub2_then_dsub_zsub3_then_dsub", "zsub2_then_zsub_zsub3_then_zsub", "" };


static const LaneBitmask SubRegIndexLaneMaskTable[] = {
  LaneBitmask::getAll(),
  LaneBitmask(0x00000001), // bsub
  LaneBitmask(0x00000001), // dsub
  LaneBitmask(0x00000001), // dsub0
  LaneBitmask(0x00000080), // dsub1
  LaneBitmask(0x00000200), // dsub2
  LaneBitmask(0x00000100), // dsub3
  LaneBitmask(0x00000001), // hsub
  LaneBitmask(0x00000002), // qhisub
  LaneBitmask(0x00000004), // qsub
  LaneBitmask(0x00000001), // qsub0
  LaneBitmask(0x00000400), // qsub1
  LaneBitmask(0x00001000), // qsub2
  LaneBitmask(0x00000800), // qsub3
  LaneBitmask(0x00000001), // ssub
  LaneBitmask(0x00000008), // sub_32
  LaneBitmask(0x00000010), // sube32
  LaneBitmask(0x00000008), // sube64
  LaneBitmask(0x00000020), // subo32
  LaneBitmask(0x00002000), // subo64
  LaneBitmask(0x00000001), // zsub
  LaneBitmask(0x00000041), // zsub0
  LaneBitmask(0x0000C000), // zsub1
  LaneBitmask(0x000C0000), // zsub2
  LaneBitmask(0x00030000), // zsub3
  LaneBitmask(0x00000040), // zsub_hi
  LaneBitmask(0x00000080), // dsub1_then_bsub
  LaneBitmask(0x00000080), // dsub1_then_hsub
  LaneBitmask(0x00000080), // dsub1_then_ssub
  LaneBitmask(0x00000100), // dsub3_then_bsub
  LaneBitmask(0x00000100), // dsub3_then_hsub
  LaneBitmask(0x00000100), // dsub3_then_ssub
  LaneBitmask(0x00000200), // dsub2_then_bsub
  LaneBitmask(0x00000200), // dsub2_then_hsub
  LaneBitmask(0x00000200), // dsub2_then_ssub
  LaneBitmask(0x00000400), // qsub1_then_bsub
  LaneBitmask(0x00000400), // qsub1_then_dsub
  LaneBitmask(0x00000400), // qsub1_then_hsub
  LaneBitmask(0x00000400), // qsub1_then_ssub
  LaneBitmask(0x00000800), // qsub3_then_bsub
  LaneBitmask(0x00000800), // qsub3_then_dsub
  LaneBitmask(0x00000800), // qsub3_then_hsub
  LaneBitmask(0x00000800), // qsub3_then_ssub
  LaneBitmask(0x00001000), // qsub2_then_bsub
  LaneBitmask(0x00001000), // qsub2_then_dsub
  LaneBitmask(0x00001000), // qsub2_then_hsub
  LaneBitmask(0x00001000), // qsub2_then_ssub
  LaneBitmask(0x00002000), // subo64_then_sub_32
  LaneBitmask(0x00004000), // zsub1_then_bsub
  LaneBitmask(0x00004000), // zsub1_then_dsub
  LaneBitmask(0x00004000), // zsub1_then_hsub
  LaneBitmask(0x00004000), // zsub1_then_ssub
  LaneBitmask(0x00004000), // zsub1_then_zsub
  LaneBitmask(0x00008000), // zsub1_then_zsub_hi
  LaneBitmask(0x00010000), // zsub3_then_bsub
  LaneBitmask(0x00010000), // zsub3_then_dsub
  LaneBitmask(0x00010000), // zsub3_then_hsub
  LaneBitmask(0x00010000), // zsub3_then_ssub
  LaneBitmask(0x00010000), // zsub3_then_zsub
  LaneBitmask(0x00020000), // zsub3_then_zsub_hi
  LaneBitmask(0x00040000), // zsub2_then_bsub
  LaneBitmask(0x00040000), // zsub2_then_dsub
  LaneBitmask(0x00040000), // zsub2_then_hsub
  LaneBitmask(0x00040000), // zsub2_then_ssub
  LaneBitmask(0x00040000), // zsub2_then_zsub
  LaneBitmask(0x00080000), // zsub2_then_zsub_hi
  LaneBitmask(0x00000081), // dsub0_dsub1
  LaneBitmask(0x00000281), // dsub0_dsub1_dsub2
  LaneBitmask(0x00000280), // dsub1_dsub2
  LaneBitmask(0x00000380), // dsub1_dsub2_dsub3
  LaneBitmask(0x00000300), // dsub2_dsub3
  LaneBitmask(0x00000401), // dsub_qsub1_then_dsub
  LaneBitmask(0x00001C01), // dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  LaneBitmask(0x00001401), // dsub_qsub1_then_dsub_qsub2_then_dsub
  LaneBitmask(0x00000401), // qsub0_qsub1
  LaneBitmask(0x00001401), // qsub0_qsub1_qsub2
  LaneBitmask(0x00001400), // qsub1_qsub2
  LaneBitmask(0x00001C00), // qsub1_qsub2_qsub3
  LaneBitmask(0x00001800), // qsub2_qsub3
  LaneBitmask(0x00001400), // qsub1_then_dsub_qsub2_then_dsub
  LaneBitmask(0x00001C00), // qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  LaneBitmask(0x00001800), // qsub2_then_dsub_qsub3_then_dsub
  LaneBitmask(0x00002008), // sub_32_subo64_then_sub_32
  LaneBitmask(0x00004001), // dsub_zsub1_then_dsub
  LaneBitmask(0x00004001), // zsub_zsub1_then_zsub
  LaneBitmask(0x00054001), // dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
  LaneBitmask(0x00044001), // dsub_zsub1_then_dsub_zsub2_then_dsub
  LaneBitmask(0x00054001), // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
  LaneBitmask(0x00044001), // zsub_zsub1_then_zsub_zsub2_then_zsub
  LaneBitmask(0x0000C041), // zsub0_zsub1
  LaneBitmask(0x000CC041), // zsub0_zsub1_zsub2
  LaneBitmask(0x000CC000), // zsub1_zsub2
  LaneBitmask(0x000FC000), // zsub1_zsub2_zsub3
  LaneBitmask(0x000F0000), // zsub2_zsub3
  LaneBitmask(0x00044000), // zsub1_then_dsub_zsub2_then_dsub
  LaneBitmask(0x00054000), // zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
  LaneBitmask(0x00044000), // zsub1_then_zsub_zsub2_then_zsub
  LaneBitmask(0x00054000), // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
  LaneBitmask(0x00050000), // zsub2_then_dsub_zsub3_then_dsub
  LaneBitmask(0x00050000), // zsub2_then_zsub_zsub3_then_zsub
 };



static const TargetRegisterInfo::RegClassInfo RegClassInfos[] = {
  // Mode = 0 (Default)
  { 8, 8, 8, VTLists+52 },    // FPR8
  { 16, 16, 16, VTLists+5 },    // FPR16
  { 16, 16, 16, VTLists+7 },    // PPR
  { 16, 16, 16, VTLists+7 },    // PPR_3b
  { 32, 32, 32, VTLists+1 },    // GPR32all
  { 32, 32, 32, VTLists+0 },    // FPR32
  { 32, 32, 32, VTLists+1 },    // GPR32
  { 32, 32, 32, VTLists+1 },    // GPR32sp
  { 32, 32, 32, VTLists+1 },    // GPR32common
  { 32, 32, 32, VTLists+1 },    // CCR
  { 32, 32, 32, VTLists+1 },    // GPR32sponly
  { 64, 64, 32, VTLists+52 },    // WSeqPairsClass
  { 64, 64, 32, VTLists+52 },    // WSeqPairsClass_with_sube32_in_GPR32common
  { 64, 64, 32, VTLists+52 },    // WSeqPairsClass_with_subo32_in_GPR32common
  { 64, 64, 32, VTLists+52 },    // WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common
  { 64, 64, 64, VTLists+3 },    // GPR64all
  { 64, 64, 64, VTLists+12 },    // FPR64
  { 64, 64, 64, VTLists+3 },    // GPR64
  { 64, 64, 64, VTLists+3 },    // GPR64sp
  { 64, 64, 64, VTLists+3 },    // GPR64common
  { 64, 64, 64, VTLists+3 },    // tcGPR64
  { 64, 64, 64, VTLists+3 },    // GPR64sponly
  { 128, 128, 64, VTLists+52 },    // DD
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_sub_32_in_GPR32common
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_subo64_in_GPR64common
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_sube64_in_tcGPR64
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_subo64_in_tcGPR64
  { 128, 128, 64, VTLists+52 },    // XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
  { 128, 128, 128, VTLists+22 },    // FPR128
  { 128, 128, 128, VTLists+39 },    // ZPR
  { 128, 128, 128, VTLists+31 },    // FPR128_lo
  { 128, 128, 128, VTLists+39 },    // ZPR_4b
  { 128, 128, 128, VTLists+39 },    // ZPR_3b
  { 192, 192, 64, VTLists+52 },    // DDD
  { 256, 256, 64, VTLists+52 },    // DDDD
  { 256, 256, 128, VTLists+52 },    // QQ
  { 256, 256, 128, VTLists+52 },    // ZPR2
  { 256, 256, 128, VTLists+52 },    // QQ_with_qsub0_in_FPR128_lo
  { 256, 256, 128, VTLists+52 },    // QQ_with_qsub1_in_FPR128_lo
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub1_in_ZPR_4b
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub_in_FPR128_lo
  { 256, 256, 128, VTLists+52 },    // QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub0_in_ZPR_3b
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub1_in_ZPR_3b
  { 256, 256, 128, VTLists+52 },    // ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // QQQ
  { 384, 384, 128, VTLists+52 },    // ZPR3
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub0_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub1_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub2_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub1_in_ZPR_4b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub2_in_ZPR_4b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
  { 384, 384, 128, VTLists+52 },    // QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub0_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub1_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub2_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
  { 384, 384, 128, VTLists+52 },    // ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // QQQQ
  { 512, 512, 128, VTLists+52 },    // ZPR4
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub0_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub1_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub2_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub3_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub2_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub3_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub0_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub2_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub3_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
  { 512, 512, 128, VTLists+52 },    // ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
};

static const TargetRegisterClass *const NullRegClasses[] = { nullptr };

static const uint32_t FPR8SubClassMask[] = {
  0x00000001, 0x00000000, 0x00000000, 0x00000000, 
  0xc0410022, 0xffffffff, 0xffffffff, 0x0000000f, // bsub
  0x00400000, 0x00000018, 0x00000000, 0x00000000, // dsub1_then_bsub
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub3_then_bsub
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub2_then_bsub
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub1_then_bsub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub3_then_bsub
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub2_then_bsub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1_then_bsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3_then_bsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2_then_bsub
};

static const uint32_t FPR16SubClassMask[] = {
  0x00000002, 0x00000000, 0x00000000, 0x00000000, 
  0xc0410020, 0xffffffff, 0xffffffff, 0x0000000f, // hsub
  0x00400000, 0x00000018, 0x00000000, 0x00000000, // dsub1_then_hsub
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub3_then_hsub
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub2_then_hsub
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub1_then_hsub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub3_then_hsub
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub2_then_hsub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1_then_hsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3_then_hsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2_then_hsub
};

static const uint32_t PPRSubClassMask[] = {
  0x0000000c, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t PPR_3bSubClassMask[] = {
  0x00000008, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t GPR32allSubClassMask[] = {
  0x000005d0, 0x00000000, 0x00000000, 0x00000000, 
  0x3fbe8000, 0x00000000, 0x00000000, 0x00000000, // sub_32
  0x00007800, 0x00000000, 0x00000000, 0x00000000, // sube32
  0x00007800, 0x00000000, 0x00000000, 0x00000000, // subo32
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // subo64_then_sub_32
};

static const uint32_t FPR32SubClassMask[] = {
  0x00000020, 0x00000000, 0x00000000, 0x00000000, 
  0xc0410000, 0xffffffff, 0xffffffff, 0x0000000f, // ssub
  0x00400000, 0x00000018, 0x00000000, 0x00000000, // dsub1_then_ssub
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub3_then_ssub
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub2_then_ssub
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub1_then_ssub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub3_then_ssub
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub2_then_ssub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1_then_ssub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3_then_ssub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2_then_ssub
};

static const uint32_t GPR32SubClassMask[] = {
  0x00000140, 0x00000000, 0x00000000, 0x00000000, 
  0x3f9a0000, 0x00000000, 0x00000000, 0x00000000, // sub_32
  0x00007800, 0x00000000, 0x00000000, 0x00000000, // sube32
  0x00007800, 0x00000000, 0x00000000, 0x00000000, // subo32
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // subo64_then_sub_32
};

static const uint32_t GPR32spSubClassMask[] = {
  0x00000580, 0x00000000, 0x00000000, 0x00000000, 
  0x2d3c0000, 0x00000000, 0x00000000, 0x00000000, // sub_32
  0x00005000, 0x00000000, 0x00000000, 0x00000000, // sube32
  0x00006000, 0x00000000, 0x00000000, 0x00000000, // subo32
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, // subo64_then_sub_32
};

static const uint32_t GPR32commonSubClassMask[] = {
  0x00000100, 0x00000000, 0x00000000, 0x00000000, 
  0x2d180000, 0x00000000, 0x00000000, 0x00000000, // sub_32
  0x00005000, 0x00000000, 0x00000000, 0x00000000, // sube32
  0x00006000, 0x00000000, 0x00000000, 0x00000000, // subo32
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, // subo64_then_sub_32
};

static const uint32_t CCRSubClassMask[] = {
  0x00000200, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t GPR32sponlySubClassMask[] = {
  0x00000400, 0x00000000, 0x00000000, 0x00000000, 
  0x00200000, 0x00000000, 0x00000000, 0x00000000, // sub_32
};

static const uint32_t WSeqPairsClassSubClassMask[] = {
  0x00007800, 0x00000000, 0x00000000, 0x00000000, 
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // sub_32_subo64_then_sub_32
};

static const uint32_t WSeqPairsClass_with_sube32_in_GPR32commonSubClassMask[] = {
  0x00005000, 0x00000000, 0x00000000, 0x00000000, 
  0x2d000000, 0x00000000, 0x00000000, 0x00000000, // sub_32_subo64_then_sub_32
};

static const uint32_t WSeqPairsClass_with_subo32_in_GPR32commonSubClassMask[] = {
  0x00006000, 0x00000000, 0x00000000, 0x00000000, 
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, // sub_32_subo64_then_sub_32
};

static const uint32_t WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonSubClassMask[] = {
  0x00004000, 0x00000000, 0x00000000, 0x00000000, 
  0x2c000000, 0x00000000, 0x00000000, 0x00000000, // sub_32_subo64_then_sub_32
};

static const uint32_t GPR64allSubClassMask[] = {
  0x003e8000, 0x00000000, 0x00000000, 0x00000000, 
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // sube64
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // subo64
};

static const uint32_t FPR64SubClassMask[] = {
  0x00010000, 0x00000000, 0x00000000, 0x00000000, 
  0xc0000000, 0xffffffe7, 0xffffffff, 0x0000000f, // dsub
  0x00400000, 0x00000018, 0x00000000, 0x00000000, // dsub0
  0x00400000, 0x00000018, 0x00000000, 0x00000000, // dsub1
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub2
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub3
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub1_then_dsub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub3_then_dsub
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub2_then_dsub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1_then_dsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3_then_dsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2_then_dsub
};

static const uint32_t GPR64SubClassMask[] = {
  0x001a0000, 0x00000000, 0x00000000, 0x00000000, 
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // sube64
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, // subo64
};

static const uint32_t GPR64spSubClassMask[] = {
  0x003c0000, 0x00000000, 0x00000000, 0x00000000, 
  0x2d000000, 0x00000000, 0x00000000, 0x00000000, // sube64
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, // subo64
};

static const uint32_t GPR64commonSubClassMask[] = {
  0x00180000, 0x00000000, 0x00000000, 0x00000000, 
  0x2d000000, 0x00000000, 0x00000000, 0x00000000, // sube64
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, // subo64
};

static const uint32_t tcGPR64SubClassMask[] = {
  0x00100000, 0x00000000, 0x00000000, 0x00000000, 
  0x28000000, 0x00000000, 0x00000000, 0x00000000, // sube64
  0x30000000, 0x00000000, 0x00000000, 0x00000000, // subo64
};

static const uint32_t GPR64sponlySubClassMask[] = {
  0x00200000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t DDSubClassMask[] = {
  0x00400000, 0x00000000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub0_dsub1
  0x00000000, 0x00000018, 0x00000000, 0x00000000, // dsub1_dsub2
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub2_dsub3
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // dsub_qsub1_then_dsub
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub1_then_dsub_qsub2_then_dsub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub2_then_dsub_qsub3_then_dsub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // dsub_zsub1_then_dsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub1_then_dsub_zsub2_then_dsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub2_then_dsub_zsub3_then_dsub
};

static const uint32_t XSeqPairsClassSubClassMask[] = {
  0x3f800000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_sub_32_in_GPR32commonSubClassMask[] = {
  0x2d000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_subo64_in_GPR64commonSubClassMask[] = {
  0x3e000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonSubClassMask[] = {
  0x2c000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_sube64_in_tcGPR64SubClassMask[] = {
  0x28000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_subo64_in_tcGPR64SubClassMask[] = {
  0x30000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64SubClassMask[] = {
  0x20000000, 0x00000000, 0x00000000, 0x00000000, 
};

static const uint32_t FPR128SubClassMask[] = {
  0x40000000, 0x00000001, 0x00000000, 0x00000000, 
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub0
  0x00000000, 0x131d09a0, 0x0131c3d0, 0x00000000, // qsub1
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub2
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub3
  0x80000000, 0xece2f646, 0xfece3c2f, 0x0000000f, // zsub
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1_then_zsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3_then_zsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2_then_zsub
};

static const uint32_t ZPRSubClassMask[] = {
  0x80000000, 0x00000006, 0x00000000, 0x00000000, 
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub0
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub1
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub2
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub3
};

static const uint32_t FPR128_loSubClassMask[] = {
  0x00000000, 0x00000001, 0x00000000, 0x00000000, 
  0x00000000, 0x11040880, 0x01104040, 0x00000000, // qsub0
  0x00000000, 0x13080900, 0x0130c080, 0x00000000, // qsub1
  0x00000000, 0x12100000, 0x01318100, 0x00000000, // qsub2
  0x00000000, 0x00000000, 0x01210200, 0x00000000, // qsub3
  0x00000000, 0x6880b406, 0x0688200c, 0x0000000d, // zsub
  0x00000000, 0xec20f200, 0x4eca040e, 0x0000000f, // zsub1_then_zsub
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub3_then_zsub
  0x00000000, 0xe4400000, 0xdec6080f, 0x0000000f, // zsub2_then_zsub
};

static const uint32_t ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000006, 0x00000000, 0x00000000, 
  0x00000000, 0x6880b400, 0x0688200c, 0x0000000d, // zsub0
  0x00000000, 0xec20f200, 0x4eca040e, 0x0000000f, // zsub1
  0x00000000, 0xe4400000, 0xdec6080f, 0x0000000f, // zsub2
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub3
};

static const uint32_t ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000004, 0x00000000, 0x00000000, 
  0x00000000, 0x4000a000, 0x0400000c, 0x0000000d, // zsub0
  0x00000000, 0x8000c000, 0x4800000e, 0x0000000f, // zsub1
  0x00000000, 0x00000000, 0xd000000b, 0x0000000e, // zsub2
  0x00000000, 0x00000000, 0xa0000000, 0x0000000a, // zsub3
};

static const uint32_t DDDSubClassMask[] = {
  0x00000000, 0x00000008, 0x00000000, 0x00000000, 
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub0_dsub1_dsub2
  0x00000000, 0x00000010, 0x00000000, 0x00000000, // dsub1_dsub2_dsub3
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // dsub_qsub1_then_dsub_qsub2_then_dsub
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // dsub_zsub1_then_dsub_zsub2_then_dsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
};

static const uint32_t DDDDSubClassMask[] = {
  0x00000000, 0x00000010, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
};

static const uint32_t QQSubClassMask[] = {
  0x00000000, 0x000009a0, 0x00000000, 0x00000000, 
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub0_qsub1
  0x00000000, 0x131d0000, 0x0131c3d0, 0x00000000, // qsub1_qsub2
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub2_qsub3
  0x00000000, 0xece2f640, 0xfece3c2f, 0x0000000f, // zsub_zsub1_then_zsub
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR2SubClassMask[] = {
  0x00000000, 0x0000f640, 0x00000000, 0x00000000, 
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub0_zsub1
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub1_zsub2
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub2_zsub3
};

static const uint32_t QQ_with_qsub0_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000880, 0x00000000, 0x00000000, 
  0x00000000, 0x11040000, 0x01104040, 0x00000000, // qsub0_qsub1
  0x00000000, 0x13080000, 0x0130c080, 0x00000000, // qsub1_qsub2
  0x00000000, 0x00000000, 0x01318100, 0x00000000, // qsub2_qsub3
  0x00000000, 0x6880b400, 0x0688200c, 0x0000000d, // zsub_zsub1_then_zsub
  0x00000000, 0xec200000, 0x4eca040e, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000900, 0x00000000, 0x00000000, 
  0x00000000, 0x13080000, 0x0130c080, 0x00000000, // qsub0_qsub1
  0x00000000, 0x12100000, 0x01318100, 0x00000000, // qsub1_qsub2
  0x00000000, 0x00000000, 0x01210200, 0x00000000, // qsub2_qsub3
  0x00000000, 0xec20f200, 0x4eca040e, 0x0000000f, // zsub_zsub1_then_zsub
  0x00000000, 0xe4400000, 0xdec6080f, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR2_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x0000f200, 0x00000000, 0x00000000, 
  0x00000000, 0xec200000, 0x4eca040e, 0x0000000f, // zsub0_zsub1
  0x00000000, 0xe4400000, 0xdec6080f, 0x0000000f, // zsub1_zsub2
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub2_zsub3
};

static const uint32_t ZPR2_with_zsub_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x0000b400, 0x00000000, 0x00000000, 
  0x00000000, 0x68800000, 0x0688200c, 0x0000000d, // zsub0_zsub1
  0x00000000, 0xec200000, 0x4eca040e, 0x0000000f, // zsub1_zsub2
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub2_zsub3
};

static const uint32_t QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000800, 0x00000000, 0x00000000, 
  0x00000000, 0x11000000, 0x01104000, 0x00000000, // qsub0_qsub1
  0x00000000, 0x12000000, 0x01308000, 0x00000000, // qsub1_qsub2
  0x00000000, 0x00000000, 0x01210000, 0x00000000, // qsub2_qsub3
  0x00000000, 0x6800b000, 0x0688000c, 0x0000000d, // zsub_zsub1_then_zsub
  0x00000000, 0xe4000000, 0x4ec2000e, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, // zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x0000b000, 0x00000000, 0x00000000, 
  0x00000000, 0x68000000, 0x0688000c, 0x0000000d, // zsub0_zsub1
  0x00000000, 0xe4000000, 0x4ec2000e, 0x0000000f, // zsub1_zsub2
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, // zsub2_zsub3
};

static const uint32_t ZPR2_with_zsub0_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x0000a000, 0x00000000, 0x00000000, 
  0x00000000, 0x40000000, 0x0400000c, 0x0000000d, // zsub0_zsub1
  0x00000000, 0x80000000, 0x4800000e, 0x0000000f, // zsub1_zsub2
  0x00000000, 0x00000000, 0xd0000000, 0x0000000e, // zsub2_zsub3
};

static const uint32_t ZPR2_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x0000c000, 0x00000000, 0x00000000, 
  0x00000000, 0x80000000, 0x4800000e, 0x0000000f, // zsub0_zsub1
  0x00000000, 0x00000000, 0xd000000b, 0x0000000e, // zsub1_zsub2
  0x00000000, 0x00000000, 0xa0000000, 0x0000000a, // zsub2_zsub3
};

static const uint32_t ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00008000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x0000000c, 0x0000000d, // zsub0_zsub1
  0x00000000, 0x00000000, 0x4000000a, 0x0000000e, // zsub1_zsub2
  0x00000000, 0x00000000, 0x80000000, 0x0000000a, // zsub2_zsub3
};

static const uint32_t QQQSubClassMask[] = {
  0x00000000, 0x131d0000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0xece20000, 0xfece3c2f, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR3SubClassMask[] = {
  0x00000000, 0xece20000, 0x0000000f, 0x00000000, 
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t QQQ_with_qsub0_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x11040000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x01104040, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x0130c080, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0x68800000, 0x0688200c, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0x4eca0400, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x13080000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x0130c080, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x01318100, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0xec200000, 0x4eca040e, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x12100000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x01318100, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x01210200, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0xe4400000, 0xdec6080f, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR3_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0xec200000, 0x0000000e, 0x00000000, 
  0x00000000, 0x00000000, 0x4eca0400, 0x0000000f, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0xe4400000, 0x0000000f, 0x00000000, 
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x68800000, 0x0000000c, 0x00000000, 
  0x00000000, 0x00000000, 0x06882000, 0x0000000d, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x4eca0400, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x11000000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x01104000, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x01308000, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0x68000000, 0x0688000c, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0x4ec20000, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x12000000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x01308000, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x01210000, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0xe4000000, 0x4ec2000e, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0xe4000000, 0x0000000e, 0x00000000, 
  0x00000000, 0x00000000, 0x4ec20000, 0x0000000f, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x68000000, 0x0000000c, 0x00000000, 
  0x00000000, 0x00000000, 0x06880000, 0x0000000d, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x4ec20000, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x10000000, 0x00000000, 0x00000000, 
  0x00000000, 0x00000000, 0x01100000, 0x00000000, // qsub0_qsub1_qsub2
  0x00000000, 0x00000000, 0x01200000, 0x00000000, // qsub1_qsub2_qsub3
  0x00000000, 0x60000000, 0x0680000c, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub
  0x00000000, 0x00000000, 0x4e400000, 0x0000000f, // zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x60000000, 0x0000000c, 0x00000000, 
  0x00000000, 0x00000000, 0x06800000, 0x0000000d, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x4e400000, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub0_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x40000000, 0x0000000c, 0x00000000, 
  0x00000000, 0x00000000, 0x04000000, 0x0000000d, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x48000000, 0x0000000f, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x80000000, 0x0000000e, 0x00000000, 
  0x00000000, 0x00000000, 0x48000000, 0x0000000f, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xd0000000, 0x0000000e, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x0000000b, 0x00000000, 
  0x00000000, 0x00000000, 0xd0000000, 0x0000000e, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0xa0000000, 0x0000000a, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x0000000a, 0x00000000, 
  0x00000000, 0x00000000, 0x40000000, 0x0000000e, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x80000000, 0x0000000a, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x0000000c, 0x00000000, 
  0x00000000, 0x00000000, 0x00000000, 0x0000000d, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x40000000, 0x0000000e, // zsub1_zsub2_zsub3
};

static const uint32_t ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x00000008, 0x00000000, 
  0x00000000, 0x00000000, 0x00000000, 0x0000000c, // zsub0_zsub1_zsub2
  0x00000000, 0x00000000, 0x00000000, 0x0000000a, // zsub1_zsub2_zsub3
};

static const uint32_t QQQQSubClassMask[] = {
  0x00000000, 0x00000000, 0x0131c3d0, 0x00000000, 
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR4SubClassMask[] = {
  0x00000000, 0x00000000, 0xfece3c20, 0x0000000f, 
};

static const uint32_t QQQQ_with_qsub0_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01104040, 0x00000000, 
  0x00000000, 0x00000000, 0x06882000, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x0130c080, 0x00000000, 
  0x00000000, 0x00000000, 0x4eca0400, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01318100, 0x00000000, 
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub3_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01210200, 0x00000000, 
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x4eca0400, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0xdec60800, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub3_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0xfe441000, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x06882000, 0x0000000d, 
};

static const uint32_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01104000, 0x00000000, 
  0x00000000, 0x00000000, 0x06880000, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01308000, 0x00000000, 
  0x00000000, 0x00000000, 0x4ec20000, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01210000, 0x00000000, 
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x4ec20000, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0xde440000, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x06880000, 0x0000000d, 
};

static const uint32_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01100000, 0x00000000, 
  0x00000000, 0x00000000, 0x06800000, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01200000, 0x00000000, 
  0x00000000, 0x00000000, 0x4e400000, 0x0000000f, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x4e400000, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x06800000, 0x0000000d, 
};

static const uint32_t QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask[] = {
  0x00000000, 0x00000000, 0x01000000, 0x00000000, 
  0x00000000, 0x00000000, 0x06000000, 0x0000000d, // zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask[] = {
  0x00000000, 0x00000000, 0x06000000, 0x0000000d, 
};

static const uint32_t ZPR4_with_zsub0_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x04000000, 0x0000000d, 
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x48000000, 0x0000000f, 
};

static const uint32_t ZPR4_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0xd0000000, 0x0000000e, 
};

static const uint32_t ZPR4_with_zsub3_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0xa0000000, 0x0000000a, 
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x40000000, 0x0000000e, 
};

static const uint32_t ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x80000000, 0x0000000a, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x00000000, 0x0000000d, 
};

static const uint32_t ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x00000000, 0x0000000a, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x00000000, 0x0000000c, 
};

static const uint32_t ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask[] = {
  0x00000000, 0x00000000, 0x00000000, 0x00000008, 
};

static const uint16_t SuperRegIdxSeqs[] = {
  /* 0 */ 15, 0,
  /* 2 */ 17, 19, 0,
  /* 5 */ 21, 22, 23, 24, 0,
  /* 10 */ 15, 16, 18, 47, 0,
  /* 15 */ 1, 26, 29, 32, 35, 39, 43, 48, 54, 60, 0,
  /* 26 */ 2, 3, 4, 5, 6, 36, 40, 44, 49, 55, 61, 0,
  /* 38 */ 7, 27, 30, 33, 37, 41, 45, 50, 56, 62, 0,
  /* 49 */ 14, 28, 31, 34, 38, 42, 46, 51, 57, 63, 0,
  /* 60 */ 10, 11, 12, 13, 20, 52, 58, 64, 0,
  /* 69 */ 82, 0,
  /* 71 */ 72, 85, 0,
  /* 74 */ 87, 0,
  /* 76 */ 90, 92, 0,
  /* 79 */ 89, 91, 93, 0,
  /* 83 */ 67, 69, 73, 80, 86, 95, 0,
  /* 90 */ 75, 77, 88, 97, 0,
  /* 95 */ 66, 68, 70, 71, 79, 81, 83, 94, 98, 0,
  /* 105 */ 74, 76, 78, 84, 96, 99, 0,
};

static const TargetRegisterClass *const PPR_3bSuperclasses[] = {
  &AArch64::PPRRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR32Superclasses[] = {
  &AArch64::GPR32allRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR32spSuperclasses[] = {
  &AArch64::GPR32allRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR32commonSuperclasses[] = {
  &AArch64::GPR32allRegClass,
  &AArch64::GPR32RegClass,
  &AArch64::GPR32spRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR32sponlySuperclasses[] = {
  &AArch64::GPR32allRegClass,
  &AArch64::GPR32spRegClass,
  nullptr
};

static const TargetRegisterClass *const WSeqPairsClass_with_sube32_in_GPR32commonSuperclasses[] = {
  &AArch64::WSeqPairsClassRegClass,
  nullptr
};

static const TargetRegisterClass *const WSeqPairsClass_with_subo32_in_GPR32commonSuperclasses[] = {
  &AArch64::WSeqPairsClassRegClass,
  nullptr
};

static const TargetRegisterClass *const WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonSuperclasses[] = {
  &AArch64::WSeqPairsClassRegClass,
  &AArch64::WSeqPairsClass_with_sube32_in_GPR32commonRegClass,
  &AArch64::WSeqPairsClass_with_subo32_in_GPR32commonRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR64Superclasses[] = {
  &AArch64::GPR64allRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR64spSuperclasses[] = {
  &AArch64::GPR64allRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR64commonSuperclasses[] = {
  &AArch64::GPR64allRegClass,
  &AArch64::GPR64RegClass,
  &AArch64::GPR64spRegClass,
  nullptr
};

static const TargetRegisterClass *const tcGPR64Superclasses[] = {
  &AArch64::GPR64allRegClass,
  &AArch64::GPR64RegClass,
  &AArch64::GPR64spRegClass,
  &AArch64::GPR64commonRegClass,
  nullptr
};

static const TargetRegisterClass *const GPR64sponlySuperclasses[] = {
  &AArch64::GPR64allRegClass,
  &AArch64::GPR64spRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_sub_32_in_GPR32commonSuperclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_subo64_in_GPR64commonSuperclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonSuperclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  &AArch64::XSeqPairsClass_with_sub_32_in_GPR32commonRegClass,
  &AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_sube64_in_tcGPR64Superclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  &AArch64::XSeqPairsClass_with_sub_32_in_GPR32commonRegClass,
  &AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  &AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_subo64_in_tcGPR64Superclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  &AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  nullptr
};

static const TargetRegisterClass *const XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64Superclasses[] = {
  &AArch64::XSeqPairsClassRegClass,
  &AArch64::XSeqPairsClass_with_sub_32_in_GPR32commonRegClass,
  &AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  &AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
  &AArch64::XSeqPairsClass_with_sube64_in_tcGPR64RegClass,
  &AArch64::XSeqPairsClass_with_subo64_in_tcGPR64RegClass,
  nullptr
};

static const TargetRegisterClass *const FPR128_loSuperclasses[] = {
  &AArch64::FPR128RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR_4bSuperclasses[] = {
  &AArch64::ZPRRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR_3bSuperclasses[] = {
  &AArch64::ZPRRegClass,
  &AArch64::ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const QQ_with_qsub0_in_FPR128_loSuperclasses[] = {
  &AArch64::QQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub_in_FPR128_loSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  nullptr
};

static const TargetRegisterClass *const QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQRegClass,
  &AArch64::QQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQ_with_qsub1_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  &AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR2_with_zsub_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub0_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  &AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR2_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  &AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR2RegClass,
  &AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR2_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR2_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR2_with_zsub1_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub0_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub_in_FPR128_loSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  &AArch64::QQQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub1_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  &AArch64::QQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub2_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQRegClass,
  &AArch64::QQQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub0_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR3RegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR3_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub0_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub3_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub3_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_loSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub3_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub3_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses[] = {
  &AArch64::QQQQRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub3_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
  &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub0_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub3_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClass,
  nullptr
};

static const TargetRegisterClass *const ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses[] = {
  &AArch64::ZPR4RegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
  &AArch64::ZPR4_with_zsub0_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub3_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
  &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
  nullptr
};


static inline unsigned GPR32AltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR32GetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WZR, AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR32RegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR32AltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

static inline unsigned GPR32spAltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR32spGetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WSP, AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR32spRegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR32spAltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

static inline unsigned GPR32commonAltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR32commonGetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR32commonRegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR32commonAltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

static inline unsigned GPR64AltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR64GetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::XZR, AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR64RegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR64AltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

static inline unsigned GPR64spAltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR64spGetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::SP, AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR64spRegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR64spAltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

static inline unsigned GPR64commonAltOrderSelect(const MachineFunction &MF) { return 1; }

static ArrayRef<MCPhysReg> GPR64commonGetRawAllocationOrder(const MachineFunction &MF) {
  static const MCPhysReg AltOrder1[] = { AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7 };
  const MCRegisterClass &MCR = AArch64MCRegisterClasses[AArch64::GPR64commonRegClassID];
  const ArrayRef<MCPhysReg> Order[] = {
    makeArrayRef(MCR.begin(), MCR.getNumRegs()),
    makeArrayRef(AltOrder1)
  };
  const unsigned Select = GPR64commonAltOrderSelect(MF);
  assert(Select < 2);
  return Order[Select];
}

namespace AArch64 {   // Register class instances
  extern const TargetRegisterClass FPR8RegClass = {
    &AArch64MCRegisterClasses[FPR8RegClassID],
    FPR8SubClassMask,
    SuperRegIdxSeqs + 15,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass FPR16RegClass = {
    &AArch64MCRegisterClasses[FPR16RegClassID],
    FPR16SubClassMask,
    SuperRegIdxSeqs + 38,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass PPRRegClass = {
    &AArch64MCRegisterClasses[PPRRegClassID],
    PPRSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass PPR_3bRegClass = {
    &AArch64MCRegisterClasses[PPR_3bRegClassID],
    PPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    PPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass GPR32allRegClass = {
    &AArch64MCRegisterClasses[GPR32allRegClassID],
    GPR32allSubClassMask,
    SuperRegIdxSeqs + 10,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass FPR32RegClass = {
    &AArch64MCRegisterClasses[FPR32RegClassID],
    FPR32SubClassMask,
    SuperRegIdxSeqs + 49,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass GPR32RegClass = {
    &AArch64MCRegisterClasses[GPR32RegClassID],
    GPR32SubClassMask,
    SuperRegIdxSeqs + 10,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR32Superclasses,
    GPR32GetRawAllocationOrder
  };

  extern const TargetRegisterClass GPR32spRegClass = {
    &AArch64MCRegisterClasses[GPR32spRegClassID],
    GPR32spSubClassMask,
    SuperRegIdxSeqs + 10,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR32spSuperclasses,
    GPR32spGetRawAllocationOrder
  };

  extern const TargetRegisterClass GPR32commonRegClass = {
    &AArch64MCRegisterClasses[GPR32commonRegClassID],
    GPR32commonSubClassMask,
    SuperRegIdxSeqs + 10,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR32commonSuperclasses,
    GPR32commonGetRawAllocationOrder
  };

  extern const TargetRegisterClass CCRRegClass = {
    &AArch64MCRegisterClasses[CCRRegClassID],
    CCRSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass GPR32sponlyRegClass = {
    &AArch64MCRegisterClasses[GPR32sponlyRegClassID],
    GPR32sponlySubClassMask,
    SuperRegIdxSeqs + 0,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR32sponlySuperclasses,
    nullptr
  };

  extern const TargetRegisterClass WSeqPairsClassRegClass = {
    &AArch64MCRegisterClasses[WSeqPairsClassRegClassID],
    WSeqPairsClassSubClassMask,
    SuperRegIdxSeqs + 69,
    LaneBitmask(0x00000030),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass WSeqPairsClass_with_sube32_in_GPR32commonRegClass = {
    &AArch64MCRegisterClasses[WSeqPairsClass_with_sube32_in_GPR32commonRegClassID],
    WSeqPairsClass_with_sube32_in_GPR32commonSubClassMask,
    SuperRegIdxSeqs + 69,
    LaneBitmask(0x00000030),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    WSeqPairsClass_with_sube32_in_GPR32commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass WSeqPairsClass_with_subo32_in_GPR32commonRegClass = {
    &AArch64MCRegisterClasses[WSeqPairsClass_with_subo32_in_GPR32commonRegClassID],
    WSeqPairsClass_with_subo32_in_GPR32commonSubClassMask,
    SuperRegIdxSeqs + 69,
    LaneBitmask(0x00000030),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    WSeqPairsClass_with_subo32_in_GPR32commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClass = {
    &AArch64MCRegisterClasses[WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClassID],
    WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonSubClassMask,
    SuperRegIdxSeqs + 69,
    LaneBitmask(0x00000030),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass GPR64allRegClass = {
    &AArch64MCRegisterClasses[GPR64allRegClassID],
    GPR64allSubClassMask,
    SuperRegIdxSeqs + 2,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass FPR64RegClass = {
    &AArch64MCRegisterClasses[FPR64RegClassID],
    FPR64SubClassMask,
    SuperRegIdxSeqs + 26,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass GPR64RegClass = {
    &AArch64MCRegisterClasses[GPR64RegClassID],
    GPR64SubClassMask,
    SuperRegIdxSeqs + 2,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR64Superclasses,
    GPR64GetRawAllocationOrder
  };

  extern const TargetRegisterClass GPR64spRegClass = {
    &AArch64MCRegisterClasses[GPR64spRegClassID],
    GPR64spSubClassMask,
    SuperRegIdxSeqs + 2,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR64spSuperclasses,
    GPR64spGetRawAllocationOrder
  };

  extern const TargetRegisterClass GPR64commonRegClass = {
    &AArch64MCRegisterClasses[GPR64commonRegClassID],
    GPR64commonSubClassMask,
    SuperRegIdxSeqs + 2,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR64commonSuperclasses,
    GPR64commonGetRawAllocationOrder
  };

  extern const TargetRegisterClass tcGPR64RegClass = {
    &AArch64MCRegisterClasses[tcGPR64RegClassID],
    tcGPR64SubClassMask,
    SuperRegIdxSeqs + 2,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    tcGPR64Superclasses,
    nullptr
  };

  extern const TargetRegisterClass GPR64sponlyRegClass = {
    &AArch64MCRegisterClasses[GPR64sponlyRegClassID],
    GPR64sponlySubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00000008),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    GPR64sponlySuperclasses,
    nullptr
  };

  extern const TargetRegisterClass DDRegClass = {
    &AArch64MCRegisterClasses[DDRegClassID],
    DDSubClassMask,
    SuperRegIdxSeqs + 95,
    LaneBitmask(0x00000081),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClassRegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClassRegClassID],
    XSeqPairsClassSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32commonRegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_sub_32_in_GPR32commonRegClassID],
    XSeqPairsClass_with_sub_32_in_GPR32commonSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_sub_32_in_GPR32commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_subo64_in_GPR64commonRegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_subo64_in_GPR64commonRegClassID],
    XSeqPairsClass_with_subo64_in_GPR64commonSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_subo64_in_GPR64commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClassID],
    XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_sube64_in_tcGPR64RegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_sube64_in_tcGPR64RegClassID],
    XSeqPairsClass_with_sube64_in_tcGPR64SubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_sube64_in_tcGPR64Superclasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_subo64_in_tcGPR64RegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_subo64_in_tcGPR64RegClassID],
    XSeqPairsClass_with_subo64_in_tcGPR64SubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_subo64_in_tcGPR64Superclasses,
    nullptr
  };

  extern const TargetRegisterClass XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClass = {
    &AArch64MCRegisterClasses[XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClassID],
    XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64SubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x00002008),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64Superclasses,
    nullptr
  };

  extern const TargetRegisterClass FPR128RegClass = {
    &AArch64MCRegisterClasses[FPR128RegClassID],
    FPR128SubClassMask,
    SuperRegIdxSeqs + 60,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass ZPRRegClass = {
    &AArch64MCRegisterClasses[ZPRRegClassID],
    ZPRSubClassMask,
    SuperRegIdxSeqs + 5,
    LaneBitmask(0x00000041),
    0,
    true, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass FPR128_loRegClass = {
    &AArch64MCRegisterClasses[FPR128_loRegClassID],
    FPR128_loSubClassMask,
    SuperRegIdxSeqs + 60,
    LaneBitmask(0x00000001),
    0,
    false, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR_4bRegClassID],
    ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 5,
    LaneBitmask(0x00000041),
    0,
    true, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR_3bRegClassID],
    ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 5,
    LaneBitmask(0x00000041),
    0,
    true, /* HasDisjunctSubRegs */
    false, /* CoveredBySubRegs */
    ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass DDDRegClass = {
    &AArch64MCRegisterClasses[DDDRegClassID],
    DDDSubClassMask,
    SuperRegIdxSeqs + 83,
    LaneBitmask(0x00000281),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass DDDDRegClass = {
    &AArch64MCRegisterClasses[DDDDRegClassID],
    DDDDSubClassMask,
    SuperRegIdxSeqs + 71,
    LaneBitmask(0x00000381),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass QQRegClass = {
    &AArch64MCRegisterClasses[QQRegClassID],
    QQSubClassMask,
    SuperRegIdxSeqs + 105,
    LaneBitmask(0x00000401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2RegClass = {
    &AArch64MCRegisterClasses[ZPR2RegClassID],
    ZPR2SubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass QQ_with_qsub0_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQ_with_qsub0_in_FPR128_loRegClassID],
    QQ_with_qsub0_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 105,
    LaneBitmask(0x00000401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQ_with_qsub0_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQ_with_qsub1_in_FPR128_loRegClassID],
    QQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 105,
    LaneBitmask(0x00000401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub1_in_ZPR_4bRegClassID],
    ZPR2_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub_in_FPR128_loRegClassID],
    ZPR2_with_zsub_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClassID],
    QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 105,
    LaneBitmask(0x00000401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClassID],
    ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub0_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub0_in_ZPR_3bRegClassID],
    ZPR2_with_zsub0_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub0_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub1_in_ZPR_3bRegClassID],
    ZPR2_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClassID],
    ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 79,
    LaneBitmask(0x0000C041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQRegClass = {
    &AArch64MCRegisterClasses[QQQRegClassID],
    QQQSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3RegClass = {
    &AArch64MCRegisterClasses[ZPR3RegClassID],
    ZPR3SubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub0_in_FPR128_loRegClassID],
    QQQ_with_qsub0_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub0_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub1_in_FPR128_loRegClassID],
    QQQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub1_in_ZPR_4bRegClassID],
    ZPR3_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub2_in_ZPR_4bRegClassID],
    ZPR3_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub_in_FPR128_loRegClassID],
    ZPR3_with_zsub_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClassID],
    QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID],
    ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClassID],
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 90,
    LaneBitmask(0x00001401),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClassID],
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub0_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub0_in_ZPR_3bRegClassID],
    ZPR3_with_zsub0_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub0_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub1_in_ZPR_3bRegClassID],
    ZPR3_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub2_in_ZPR_3bRegClassID],
    ZPR3_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID],
    ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClassID],
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClassID],
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 76,
    LaneBitmask(0x000CC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQRegClass = {
    &AArch64MCRegisterClasses[QQQQRegClassID],
    QQQQSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4RegClass = {
    &AArch64MCRegisterClasses[ZPR4RegClassID],
    ZPR4SubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    NullRegClasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub0_in_FPR128_loRegClassID],
    QQQQ_with_qsub0_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub0_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub1_in_FPR128_loRegClassID],
    QQQQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub3_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub3_in_FPR128_loRegClassID],
    QQQQ_with_qsub3_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub3_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_4bRegClassID],
    ZPR4_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub2_in_ZPR_4bRegClassID],
    ZPR4_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub3_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub3_in_ZPR_4bRegClassID],
    ZPR4_with_zsub3_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub3_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_loRegClassID],
    ZPR4_with_zsub_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClassID],
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID],
    QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID],
    ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID],
    ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClassID],
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID],
    QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID],
    ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass = {
    &AArch64MCRegisterClasses[QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClassID],
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSubClassMask,
    SuperRegIdxSeqs + 74,
    LaneBitmask(0x00001C01),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub0_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub0_in_ZPR_3bRegClassID],
    ZPR4_with_zsub0_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub0_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_3bRegClassID],
    ZPR4_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub2_in_ZPR_3bRegClassID],
    ZPR4_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub3_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub3_in_ZPR_3bRegClassID],
    ZPR4_with_zsub3_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub3_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID],
    ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID],
    ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID],
    ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bSuperclasses,
    nullptr
  };

  extern const TargetRegisterClass ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClass = {
    &AArch64MCRegisterClasses[ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClassID],
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bSubClassMask,
    SuperRegIdxSeqs + 1,
    LaneBitmask(0x000FC041),
    0,
    true, /* HasDisjunctSubRegs */
    true, /* CoveredBySubRegs */
    ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bSuperclasses,
    nullptr
  };

} // end namespace AArch64

namespace {
  const TargetRegisterClass* const RegisterClasses[] = {
    &AArch64::FPR8RegClass,
    &AArch64::FPR16RegClass,
    &AArch64::PPRRegClass,
    &AArch64::PPR_3bRegClass,
    &AArch64::GPR32allRegClass,
    &AArch64::FPR32RegClass,
    &AArch64::GPR32RegClass,
    &AArch64::GPR32spRegClass,
    &AArch64::GPR32commonRegClass,
    &AArch64::CCRRegClass,
    &AArch64::GPR32sponlyRegClass,
    &AArch64::WSeqPairsClassRegClass,
    &AArch64::WSeqPairsClass_with_sube32_in_GPR32commonRegClass,
    &AArch64::WSeqPairsClass_with_subo32_in_GPR32commonRegClass,
    &AArch64::WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32commonRegClass,
    &AArch64::GPR64allRegClass,
    &AArch64::FPR64RegClass,
    &AArch64::GPR64RegClass,
    &AArch64::GPR64spRegClass,
    &AArch64::GPR64commonRegClass,
    &AArch64::tcGPR64RegClass,
    &AArch64::GPR64sponlyRegClass,
    &AArch64::DDRegClass,
    &AArch64::XSeqPairsClassRegClass,
    &AArch64::XSeqPairsClass_with_sub_32_in_GPR32commonRegClass,
    &AArch64::XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
    &AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64commonRegClass,
    &AArch64::XSeqPairsClass_with_sube64_in_tcGPR64RegClass,
    &AArch64::XSeqPairsClass_with_subo64_in_tcGPR64RegClass,
    &AArch64::XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64RegClass,
    &AArch64::FPR128RegClass,
    &AArch64::ZPRRegClass,
    &AArch64::FPR128_loRegClass,
    &AArch64::ZPR_4bRegClass,
    &AArch64::ZPR_3bRegClass,
    &AArch64::DDDRegClass,
    &AArch64::DDDDRegClass,
    &AArch64::QQRegClass,
    &AArch64::ZPR2RegClass,
    &AArch64::QQ_with_qsub0_in_FPR128_loRegClass,
    &AArch64::QQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::ZPR2_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::ZPR2_with_zsub_in_FPR128_loRegClass,
    &AArch64::QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::ZPR2_with_zsub0_in_ZPR_3bRegClass,
    &AArch64::ZPR2_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::QQQRegClass,
    &AArch64::ZPR3RegClass,
    &AArch64::QQQ_with_qsub0_in_FPR128_loRegClass,
    &AArch64::QQQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::QQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::ZPR3_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::ZPR3_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::ZPR3_with_zsub_in_FPR128_loRegClass,
    &AArch64::QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::ZPR3_with_zsub0_in_ZPR_3bRegClass,
    &AArch64::ZPR3_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::ZPR3_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::QQQQRegClass,
    &AArch64::ZPR4RegClass,
    &AArch64::QQQQ_with_qsub0_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub3_in_FPR128_loRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub3_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4bRegClass,
    &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_loRegClass,
    &AArch64::QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4bRegClass,
    &AArch64::QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_loRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4bRegClass,
    &AArch64::ZPR4_with_zsub0_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub3_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3bRegClass,
    &AArch64::ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3bRegClass,
  };
} // end anonymous namespace

static const TargetRegisterInfoDesc AArch64RegInfoDesc[] = { // Extra Descriptors
  { 0, false },
  { 0, false },
  { 0, true },
  { 0, true },
  { 0, false },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, false },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
  { 0, true },
};
unsigned AArch64GenRegisterInfo::composeSubRegIndicesImpl(unsigned IdxA, unsigned IdxB) const {
  static const uint8_t RowMap[99] = {
    0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 4, 5, 6, 0, 0, 0, 0, 0, 1, 0, 0, 7, 8, 9, 0, 0, 1, 1, 0, 3, 3, 0, 2, 2, 0, 4, 4, 4, 0, 6, 6, 6, 0, 5, 5, 5, 0, 0, 7, 7, 7, 7, 0, 0, 9, 9, 9, 9, 0, 0, 8, 8, 8, 8, 0, 0, 0, 1, 1, 2, 10, 10, 10, 0, 0, 4, 4, 5, 4, 4, 5, 0, 11, 10, 11, 11, 10, 10, 0, 0, 7, 7, 8, 7, 7, 7, 7, 8, 8, 
  };
  static const uint8_t Rows[12][99] = {
    { 1, 2, 3, 4, 5, 0, 7, 0, 0, 10, 11, 12, 0, 14, 15, 15, 0, 47, 0, 20, 21, 22, 23, 0, 25, 26, 27, 28, 0, 0, 0, 32, 33, 34, 35, 36, 37, 38, 0, 0, 0, 0, 43, 44, 45, 46, 0, 48, 49, 50, 51, 52, 53, 0, 0, 0, 0, 0, 0, 60, 61, 62, 63, 64, 65, 66, 0, 68, 0, 0, 71, 0, 73, 74, 0, 76, 0, 0, 79, 0, 0, 0, 83, 84, 0, 86, 0, 88, 89, 0, 91, 0, 0, 94, 0, 96, 0, 0, 0, },
    { 26, 0, 4, 5, 6, 0, 27, 0, 0, 0, 0, 0, 0, 28, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33, 34, 0, 0, 0, 29, 30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 32, 0, 5, 6, 0, 0, 33, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 29, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 35, 36, 36, 44, 40, 0, 37, 0, 0, 11, 12, 13, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 45, 46, 0, 0, 0, 39, 41, 42, 43, 44, 45, 46, 0, 0, 0, 0, 39, 40, 41, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 79, 0, 81, 0, 0, 79, 0, 80, 76, 0, 78, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 43, 44, 44, 40, 0, 0, 45, 0, 0, 12, 13, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 41, 42, 0, 0, 0, 0, 0, 0, 39, 40, 41, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 39, 40, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 48, 49, 49, 61, 55, 0, 50, 0, 0, 52, 64, 58, 0, 51, 0, 0, 0, 0, 0, 52, 22, 23, 24, 0, 53, 60, 62, 63, 0, 0, 0, 54, 56, 57, 60, 61, 62, 63, 0, 0, 0, 0, 54, 55, 56, 57, 0, 60, 61, 62, 63, 64, 65, 0, 0, 0, 0, 0, 0, 54, 55, 56, 57, 58, 59, 94, 0, 98, 0, 0, 94, 0, 95, 96, 0, 99, 0, 0, 98, 0, 0, 0, 94, 96, 0, 95, 0, 97, 91, 0, 93, 0, 0, 98, 0, 99, 0, 0, 0, },
    { 60, 61, 61, 55, 0, 0, 62, 0, 0, 64, 58, 0, 0, 63, 0, 0, 0, 0, 0, 64, 23, 24, 0, 0, 65, 54, 56, 57, 0, 0, 0, 0, 0, 0, 54, 55, 56, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 55, 56, 57, 58, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 54, 55, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 57, 0, 0, 0, 0, 0, 58, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 1, 2, 2, 36, 44, 40, 7, 0, 0, 20, 52, 64, 58, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 37, 38, 39, 41, 42, 43, 45, 46, 48, 49, 50, 51, 54, 55, 56, 57, 60, 61, 62, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 73, 79, 80, 81, 83, 85, 86, 84, 88, 96, 97, 99, 94, 95, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 1, 0, 2, 49, 61, 55, 7, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 50, 51, 54, 56, 57, 60, 62, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 86, 94, 95, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
  };

  --IdxA; assert(IdxA < 99);
  --IdxB; assert(IdxB < 99);
  return Rows[RowMap[IdxA]][IdxB];
}

  struct MaskRolOp {
    LaneBitmask Mask;
    uint8_t  RotateLeft;
  };
  static const MaskRolOp LaneMaskComposeSequences[] = {
    { LaneBitmask(0xFFFFFFFF),  0 }, { LaneBitmask::getNone(), 0 },   // Sequence 0
    { LaneBitmask(0xFFFFFFFF),  7 }, { LaneBitmask::getNone(), 0 },   // Sequence 2
    { LaneBitmask(0xFFFFFFFF),  9 }, { LaneBitmask::getNone(), 0 },   // Sequence 4
    { LaneBitmask(0xFFFFFFFF),  8 }, { LaneBitmask::getNone(), 0 },   // Sequence 6
    { LaneBitmask(0xFFFFFFFF),  1 }, { LaneBitmask::getNone(), 0 },   // Sequence 8
    { LaneBitmask(0xFFFFFFFF),  2 }, { LaneBitmask::getNone(), 0 },   // Sequence 10
    { LaneBitmask(0xFFFFFFFF), 10 }, { LaneBitmask::getNone(), 0 },   // Sequence 12
    { LaneBitmask(0xFFFFFFFF), 12 }, { LaneBitmask::getNone(), 0 },   // Sequence 14
    { LaneBitmask(0xFFFFFFFF), 11 }, { LaneBitmask::getNone(), 0 },   // Sequence 16
    { LaneBitmask(0xFFFFFFFF),  3 }, { LaneBitmask::getNone(), 0 },   // Sequence 18
    { LaneBitmask(0xFFFFFFFF),  4 }, { LaneBitmask::getNone(), 0 },   // Sequence 20
    { LaneBitmask(0xFFFFFFFF),  5 }, { LaneBitmask::getNone(), 0 },   // Sequence 22
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000040),  9 }, { LaneBitmask::getNone(), 0 },   // Sequence 24
    { LaneBitmask(0x00000001), 18 }, { LaneBitmask(0x00000040), 13 }, { LaneBitmask::getNone(), 0 },   // Sequence 27
    { LaneBitmask(0x00000001), 16 }, { LaneBitmask(0x00000040), 11 }, { LaneBitmask::getNone(), 0 },   // Sequence 30
    { LaneBitmask(0xFFFFFFFF),  6 }, { LaneBitmask::getNone(), 0 },   // Sequence 33
    { LaneBitmask(0xFFFFFFFF), 13 }, { LaneBitmask::getNone(), 0 },   // Sequence 35
    { LaneBitmask(0xFFFFFFFF), 14 }, { LaneBitmask::getNone(), 0 },   // Sequence 37
    { LaneBitmask(0xFFFFFFFF), 15 }, { LaneBitmask::getNone(), 0 },   // Sequence 39
    { LaneBitmask(0xFFFFFFFF), 16 }, { LaneBitmask::getNone(), 0 },   // Sequence 41
    { LaneBitmask(0xFFFFFFFF), 17 }, { LaneBitmask::getNone(), 0 },   // Sequence 43
    { LaneBitmask(0xFFFFFFFF), 18 }, { LaneBitmask::getNone(), 0 },   // Sequence 45
    { LaneBitmask(0xFFFFFFFF), 19 }, { LaneBitmask::getNone(), 0 },   // Sequence 47
    { LaneBitmask(0x00000001),  7 }, { LaneBitmask(0x00000080),  2 }, { LaneBitmask::getNone(), 0 },   // Sequence 49
    { LaneBitmask(0x00000001),  7 }, { LaneBitmask(0x00000080),  2 }, { LaneBitmask(0x00000200), 31 }, { LaneBitmask::getNone(), 0 },   // Sequence 52
    { LaneBitmask(0x00000001),  9 }, { LaneBitmask(0x00000080),  1 }, { LaneBitmask::getNone(), 0 },   // Sequence 56
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000080),  3 }, { LaneBitmask::getNone(), 0 },   // Sequence 59
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000380),  3 }, { LaneBitmask::getNone(), 0 },   // Sequence 62
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000280),  3 }, { LaneBitmask::getNone(), 0 },   // Sequence 65
    { LaneBitmask(0x00000001), 10 }, { LaneBitmask(0x00000400),  2 }, { LaneBitmask::getNone(), 0 },   // Sequence 68
    { LaneBitmask(0x00000001), 10 }, { LaneBitmask(0x00000400),  2 }, { LaneBitmask(0x00001000), 31 }, { LaneBitmask::getNone(), 0 },   // Sequence 71
    { LaneBitmask(0x00000001), 12 }, { LaneBitmask(0x00000400),  1 }, { LaneBitmask::getNone(), 0 },   // Sequence 75
    { LaneBitmask(0x00000001), 10 }, { LaneBitmask(0x00000080),  5 }, { LaneBitmask::getNone(), 0 },   // Sequence 78
    { LaneBitmask(0x00000001), 10 }, { LaneBitmask(0x00000080),  5 }, { LaneBitmask(0x00000200),  2 }, { LaneBitmask::getNone(), 0 },   // Sequence 81
    { LaneBitmask(0x00000001), 12 }, { LaneBitmask(0x00000080),  4 }, { LaneBitmask::getNone(), 0 },   // Sequence 85
    { LaneBitmask(0x00000010), 31 }, { LaneBitmask(0x00000020),  8 }, { LaneBitmask::getNone(), 0 },   // Sequence 88
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000080),  7 }, { LaneBitmask::getNone(), 0 },   // Sequence 91
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000400),  4 }, { LaneBitmask::getNone(), 0 },   // Sequence 94
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000080),  7 }, { LaneBitmask(0x00000100),  8 }, { LaneBitmask(0x00000200),  9 }, { LaneBitmask::getNone(), 0 },   // Sequence 97
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000080),  7 }, { LaneBitmask(0x00000200),  9 }, { LaneBitmask::getNone(), 0 },   // Sequence 102
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000400),  4 }, { LaneBitmask(0x00000800),  5 }, { LaneBitmask(0x00001000),  6 }, { LaneBitmask::getNone(), 0 },   // Sequence 106
    { LaneBitmask(0x00000001),  0 }, { LaneBitmask(0x00000400),  4 }, { LaneBitmask(0x00001000),  6 }, { LaneBitmask::getNone(), 0 },   // Sequence 111
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000040),  9 }, { LaneBitmask(0x0000C000),  4 }, { LaneBitmask::getNone(), 0 },   // Sequence 115
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000040),  9 }, { LaneBitmask(0x0000C000),  4 }, { LaneBitmask(0x000C0000), 30 }, { LaneBitmask::getNone(), 0 },   // Sequence 119
    { LaneBitmask(0x00000001), 18 }, { LaneBitmask(0x00000040), 13 }, { LaneBitmask(0x0000C000),  2 }, { LaneBitmask::getNone(), 0 },   // Sequence 124
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000080), 11 }, { LaneBitmask::getNone(), 0 },   // Sequence 128
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000080), 11 }, { LaneBitmask(0x00000200),  7 }, { LaneBitmask::getNone(), 0 },   // Sequence 131
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000400),  8 }, { LaneBitmask::getNone(), 0 },   // Sequence 135
    { LaneBitmask(0x00000001), 14 }, { LaneBitmask(0x00000400),  8 }, { LaneBitmask(0x00001000),  4 }, { LaneBitmask::getNone(), 0 },   // Sequence 138
    { LaneBitmask(0x00000001), 18 }, { LaneBitmask(0x00000080),  9 }, { LaneBitmask::getNone(), 0 },   // Sequence 142
    { LaneBitmask(0x00000001), 18 }, { LaneBitmask(0x00000400),  6 }, { LaneBitmask::getNone(), 0 }  // Sequence 145
  };
  static const MaskRolOp *const CompositeSequences[] = {
    &LaneMaskComposeSequences[0], // to bsub
    &LaneMaskComposeSequences[0], // to dsub
    &LaneMaskComposeSequences[0], // to dsub0
    &LaneMaskComposeSequences[2], // to dsub1
    &LaneMaskComposeSequences[4], // to dsub2
    &LaneMaskComposeSequences[6], // to dsub3
    &LaneMaskComposeSequences[0], // to hsub
    &LaneMaskComposeSequences[8], // to qhisub
    &LaneMaskComposeSequences[10], // to qsub
    &LaneMaskComposeSequences[0], // to qsub0
    &LaneMaskComposeSequences[12], // to qsub1
    &LaneMaskComposeSequences[14], // to qsub2
    &LaneMaskComposeSequences[16], // to qsub3
    &LaneMaskComposeSequences[0], // to ssub
    &LaneMaskComposeSequences[18], // to sub_32
    &LaneMaskComposeSequences[20], // to sube32
    &LaneMaskComposeSequences[0], // to sube64
    &LaneMaskComposeSequences[22], // to subo32
    &LaneMaskComposeSequences[12], // to subo64
    &LaneMaskComposeSequences[0], // to zsub
    &LaneMaskComposeSequences[0], // to zsub0
    &LaneMaskComposeSequences[24], // to zsub1
    &LaneMaskComposeSequences[27], // to zsub2
    &LaneMaskComposeSequences[30], // to zsub3
    &LaneMaskComposeSequences[33], // to zsub_hi
    &LaneMaskComposeSequences[2], // to dsub1_then_bsub
    &LaneMaskComposeSequences[2], // to dsub1_then_hsub
    &LaneMaskComposeSequences[2], // to dsub1_then_ssub
    &LaneMaskComposeSequences[6], // to dsub3_then_bsub
    &LaneMaskComposeSequences[6], // to dsub3_then_hsub
    &LaneMaskComposeSequences[6], // to dsub3_then_ssub
    &LaneMaskComposeSequences[4], // to dsub2_then_bsub
    &LaneMaskComposeSequences[4], // to dsub2_then_hsub
    &LaneMaskComposeSequences[4], // to dsub2_then_ssub
    &LaneMaskComposeSequences[12], // to qsub1_then_bsub
    &LaneMaskComposeSequences[12], // to qsub1_then_dsub
    &LaneMaskComposeSequences[12], // to qsub1_then_hsub
    &LaneMaskComposeSequences[12], // to qsub1_then_ssub
    &LaneMaskComposeSequences[16], // to qsub3_then_bsub
    &LaneMaskComposeSequences[16], // to qsub3_then_dsub
    &LaneMaskComposeSequences[16], // to qsub3_then_hsub
    &LaneMaskComposeSequences[16], // to qsub3_then_ssub
    &LaneMaskComposeSequences[14], // to qsub2_then_bsub
    &LaneMaskComposeSequences[14], // to qsub2_then_dsub
    &LaneMaskComposeSequences[14], // to qsub2_then_hsub
    &LaneMaskComposeSequences[14], // to qsub2_then_ssub
    &LaneMaskComposeSequences[35], // to subo64_then_sub_32
    &LaneMaskComposeSequences[37], // to zsub1_then_bsub
    &LaneMaskComposeSequences[37], // to zsub1_then_dsub
    &LaneMaskComposeSequences[37], // to zsub1_then_hsub
    &LaneMaskComposeSequences[37], // to zsub1_then_ssub
    &LaneMaskComposeSequences[37], // to zsub1_then_zsub
    &LaneMaskComposeSequences[39], // to zsub1_then_zsub_hi
    &LaneMaskComposeSequences[41], // to zsub3_then_bsub
    &LaneMaskComposeSequences[41], // to zsub3_then_dsub
    &LaneMaskComposeSequences[41], // to zsub3_then_hsub
    &LaneMaskComposeSequences[41], // to zsub3_then_ssub
    &LaneMaskComposeSequences[41], // to zsub3_then_zsub
    &LaneMaskComposeSequences[43], // to zsub3_then_zsub_hi
    &LaneMaskComposeSequences[45], // to zsub2_then_bsub
    &LaneMaskComposeSequences[45], // to zsub2_then_dsub
    &LaneMaskComposeSequences[45], // to zsub2_then_hsub
    &LaneMaskComposeSequences[45], // to zsub2_then_ssub
    &LaneMaskComposeSequences[45], // to zsub2_then_zsub
    &LaneMaskComposeSequences[47], // to zsub2_then_zsub_hi
    &LaneMaskComposeSequences[0], // to dsub0_dsub1
    &LaneMaskComposeSequences[0], // to dsub0_dsub1_dsub2
    &LaneMaskComposeSequences[49], // to dsub1_dsub2
    &LaneMaskComposeSequences[52], // to dsub1_dsub2_dsub3
    &LaneMaskComposeSequences[56], // to dsub2_dsub3
    &LaneMaskComposeSequences[59], // to dsub_qsub1_then_dsub
    &LaneMaskComposeSequences[62], // to dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
    &LaneMaskComposeSequences[65], // to dsub_qsub1_then_dsub_qsub2_then_dsub
    &LaneMaskComposeSequences[0], // to qsub0_qsub1
    &LaneMaskComposeSequences[0], // to qsub0_qsub1_qsub2
    &LaneMaskComposeSequences[68], // to qsub1_qsub2
    &LaneMaskComposeSequences[71], // to qsub1_qsub2_qsub3
    &LaneMaskComposeSequences[75], // to qsub2_qsub3
    &LaneMaskComposeSequences[78], // to qsub1_then_dsub_qsub2_then_dsub
    &LaneMaskComposeSequences[81], // to qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
    &LaneMaskComposeSequences[85], // to qsub2_then_dsub_qsub3_then_dsub
    &LaneMaskComposeSequences[88], // to sub_32_subo64_then_sub_32
    &LaneMaskComposeSequences[91], // to dsub_zsub1_then_dsub
    &LaneMaskComposeSequences[94], // to zsub_zsub1_then_zsub
    &LaneMaskComposeSequences[97], // to dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
    &LaneMaskComposeSequences[102], // to dsub_zsub1_then_dsub_zsub2_then_dsub
    &LaneMaskComposeSequences[106], // to zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
    &LaneMaskComposeSequences[111], // to zsub_zsub1_then_zsub_zsub2_then_zsub
    &LaneMaskComposeSequences[0], // to zsub0_zsub1
    &LaneMaskComposeSequences[0], // to zsub0_zsub1_zsub2
    &LaneMaskComposeSequences[115], // to zsub1_zsub2
    &LaneMaskComposeSequences[119], // to zsub1_zsub2_zsub3
    &LaneMaskComposeSequences[124], // to zsub2_zsub3
    &LaneMaskComposeSequences[128], // to zsub1_then_dsub_zsub2_then_dsub
    &LaneMaskComposeSequences[131], // to zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
    &LaneMaskComposeSequences[135], // to zsub1_then_zsub_zsub2_then_zsub
    &LaneMaskComposeSequences[138], // to zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
    &LaneMaskComposeSequences[142], // to zsub2_then_dsub_zsub3_then_dsub
    &LaneMaskComposeSequences[145] // to zsub2_then_zsub_zsub3_then_zsub
  };

LaneBitmask AArch64GenRegisterInfo::composeSubRegIndexLaneMaskImpl(unsigned IdxA, LaneBitmask LaneMask) const {
  --IdxA; assert(IdxA < 99 && "Subregister index out of bounds");
  LaneBitmask Result;
  for (const MaskRolOp *Ops = CompositeSequences[IdxA]; Ops->Mask.any(); ++Ops) {
    LaneBitmask::Type M = LaneMask.getAsInteger() & Ops->Mask.getAsInteger();
    if (unsigned S = Ops->RotateLeft)
      Result |= LaneBitmask((M << S) | (M >> (LaneBitmask::BitWidth - S)));
    else
      Result |= LaneBitmask(M);
  }
  return Result;
}

LaneBitmask AArch64GenRegisterInfo::reverseComposeSubRegIndexLaneMaskImpl(unsigned IdxA,  LaneBitmask LaneMask) const {
  LaneMask &= getSubRegIndexLaneMask(IdxA);
  --IdxA; assert(IdxA < 99 && "Subregister index out of bounds");
  LaneBitmask Result;
  for (const MaskRolOp *Ops = CompositeSequences[IdxA]; Ops->Mask.any(); ++Ops) {
    LaneBitmask::Type M = LaneMask.getAsInteger();
    if (unsigned S = Ops->RotateLeft)
      Result |= LaneBitmask((M >> S) | (M << (LaneBitmask::BitWidth - S)));
    else
      Result |= LaneBitmask(M);
  }
  return Result;
}

const TargetRegisterClass *AArch64GenRegisterInfo::getSubClassWithSubReg(const TargetRegisterClass *RC, unsigned Idx) const {
  static const uint8_t Table[100][99] = {
    {	// FPR8
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// FPR16
      2,	// bsub -> FPR16
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// PPR
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// PPR_3b
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR32all
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// FPR32
      6,	// bsub -> FPR32
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      6,	// hsub -> FPR32
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR32
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR32sp
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR32common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// CCR
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR32sponly
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// WSeqPairsClass
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      12,	// sube32 -> WSeqPairsClass
      0,	// sube64
      12,	// subo32 -> WSeqPairsClass
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// WSeqPairsClass_with_sube32_in_GPR32common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      13,	// sube32 -> WSeqPairsClass_with_sube32_in_GPR32common
      0,	// sube64
      13,	// subo32 -> WSeqPairsClass_with_sube32_in_GPR32common
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// WSeqPairsClass_with_subo32_in_GPR32common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      14,	// sube32 -> WSeqPairsClass_with_subo32_in_GPR32common
      0,	// sube64
      14,	// subo32 -> WSeqPairsClass_with_subo32_in_GPR32common
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      0,	// sub_32
      15,	// sube32 -> WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common
      0,	// sube64
      15,	// subo32 -> WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR64all
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      16,	// sub_32 -> GPR64all
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// FPR64
      17,	// bsub -> FPR64
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      17,	// hsub -> FPR64
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      17,	// ssub -> FPR64
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR64
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      18,	// sub_32 -> GPR64
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR64sp
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      19,	// sub_32 -> GPR64sp
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR64common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      20,	// sub_32 -> GPR64common
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// tcGPR64
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      21,	// sub_32 -> tcGPR64
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// GPR64sponly
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      22,	// sub_32 -> GPR64sponly
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// DD
      23,	// bsub -> DD
      0,	// dsub
      23,	// dsub0 -> DD
      23,	// dsub1 -> DD
      0,	// dsub2
      0,	// dsub3
      23,	// hsub -> DD
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      23,	// ssub -> DD
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      23,	// dsub1_then_bsub -> DD
      23,	// dsub1_then_hsub -> DD
      23,	// dsub1_then_ssub -> DD
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      24,	// sub_32 -> XSeqPairsClass
      0,	// sube32
      24,	// sube64 -> XSeqPairsClass
      0,	// subo32
      24,	// subo64 -> XSeqPairsClass
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      24,	// subo64_then_sub_32 -> XSeqPairsClass
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      24,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      25,	// sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// sube32
      25,	// sube64 -> XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// subo32
      25,	// subo64 -> XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      25,	// subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      25,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_subo64_in_GPR64common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      26,	// sub_32 -> XSeqPairsClass_with_subo64_in_GPR64common
      0,	// sube32
      26,	// sube64 -> XSeqPairsClass_with_subo64_in_GPR64common
      0,	// subo32
      26,	// subo64 -> XSeqPairsClass_with_subo64_in_GPR64common
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      26,	// subo64_then_sub_32 -> XSeqPairsClass_with_subo64_in_GPR64common
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      26,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_subo64_in_GPR64common
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      27,	// sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// sube32
      27,	// sube64 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// subo32
      27,	// subo64 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      27,	// subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      27,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      28,	// sub_32 -> XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// sube32
      28,	// sube64 -> XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// subo32
      28,	// subo64 -> XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      28,	// subo64_then_sub_32 -> XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      28,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_sube64_in_tcGPR64
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      29,	// sub_32 -> XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// sube32
      29,	// sube64 -> XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// subo32
      29,	// subo64 -> XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      29,	// subo64_then_sub_32 -> XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      29,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// bsub
      0,	// dsub
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      0,	// hsub
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      0,	// ssub
      30,	// sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// sube32
      30,	// sube64 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// subo32
      30,	// subo64 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      30,	// subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      30,	// sub_32_subo64_then_sub_32 -> XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// FPR128
      31,	// bsub -> FPR128
      31,	// dsub -> FPR128
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      31,	// hsub -> FPR128
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      31,	// ssub -> FPR128
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR
      32,	// bsub -> ZPR
      32,	// dsub -> ZPR
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      32,	// hsub -> ZPR
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      32,	// ssub -> ZPR
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      32,	// zsub -> ZPR
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      32,	// zsub_hi -> ZPR
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// FPR128_lo
      33,	// bsub -> FPR128_lo
      33,	// dsub -> FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      33,	// hsub -> FPR128_lo
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      33,	// ssub -> FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR_4b
      34,	// bsub -> ZPR_4b
      34,	// dsub -> ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      34,	// hsub -> ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      34,	// ssub -> ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      34,	// zsub -> ZPR_4b
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      34,	// zsub_hi -> ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR_3b
      35,	// bsub -> ZPR_3b
      35,	// dsub -> ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      35,	// hsub -> ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      35,	// ssub -> ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      35,	// zsub -> ZPR_3b
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      35,	// zsub_hi -> ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// DDD
      36,	// bsub -> DDD
      0,	// dsub
      36,	// dsub0 -> DDD
      36,	// dsub1 -> DDD
      36,	// dsub2 -> DDD
      0,	// dsub3
      36,	// hsub -> DDD
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      36,	// ssub -> DDD
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      36,	// dsub1_then_bsub -> DDD
      36,	// dsub1_then_hsub -> DDD
      36,	// dsub1_then_ssub -> DDD
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      36,	// dsub2_then_bsub -> DDD
      36,	// dsub2_then_hsub -> DDD
      36,	// dsub2_then_ssub -> DDD
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      36,	// dsub0_dsub1 -> DDD
      0,	// dsub0_dsub1_dsub2
      36,	// dsub1_dsub2 -> DDD
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// DDDD
      37,	// bsub -> DDDD
      0,	// dsub
      37,	// dsub0 -> DDDD
      37,	// dsub1 -> DDDD
      37,	// dsub2 -> DDDD
      37,	// dsub3 -> DDDD
      37,	// hsub -> DDDD
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      37,	// ssub -> DDDD
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      37,	// dsub1_then_bsub -> DDDD
      37,	// dsub1_then_hsub -> DDDD
      37,	// dsub1_then_ssub -> DDDD
      37,	// dsub3_then_bsub -> DDDD
      37,	// dsub3_then_hsub -> DDDD
      37,	// dsub3_then_ssub -> DDDD
      37,	// dsub2_then_bsub -> DDDD
      37,	// dsub2_then_hsub -> DDDD
      37,	// dsub2_then_ssub -> DDDD
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      37,	// dsub0_dsub1 -> DDDD
      37,	// dsub0_dsub1_dsub2 -> DDDD
      37,	// dsub1_dsub2 -> DDDD
      37,	// dsub1_dsub2_dsub3 -> DDDD
      37,	// dsub2_dsub3 -> DDDD
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQ
      38,	// bsub -> QQ
      38,	// dsub -> QQ
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      38,	// hsub -> QQ
      0,	// qhisub
      0,	// qsub
      38,	// qsub0 -> QQ
      38,	// qsub1 -> QQ
      0,	// qsub2
      0,	// qsub3
      38,	// ssub -> QQ
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      38,	// qsub1_then_bsub -> QQ
      38,	// qsub1_then_dsub -> QQ
      38,	// qsub1_then_hsub -> QQ
      38,	// qsub1_then_ssub -> QQ
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      38,	// dsub_qsub1_then_dsub -> QQ
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2
      39,	// bsub -> ZPR2
      39,	// dsub -> ZPR2
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      39,	// hsub -> ZPR2
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      39,	// ssub -> ZPR2
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      39,	// zsub -> ZPR2
      39,	// zsub0 -> ZPR2
      39,	// zsub1 -> ZPR2
      0,	// zsub2
      0,	// zsub3
      39,	// zsub_hi -> ZPR2
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      39,	// zsub1_then_bsub -> ZPR2
      39,	// zsub1_then_dsub -> ZPR2
      39,	// zsub1_then_hsub -> ZPR2
      39,	// zsub1_then_ssub -> ZPR2
      39,	// zsub1_then_zsub -> ZPR2
      39,	// zsub1_then_zsub_hi -> ZPR2
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      39,	// dsub_zsub1_then_dsub -> ZPR2
      39,	// zsub_zsub1_then_zsub -> ZPR2
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQ_with_qsub0_in_FPR128_lo
      40,	// bsub -> QQ_with_qsub0_in_FPR128_lo
      40,	// dsub -> QQ_with_qsub0_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      40,	// hsub -> QQ_with_qsub0_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      40,	// qsub0 -> QQ_with_qsub0_in_FPR128_lo
      40,	// qsub1 -> QQ_with_qsub0_in_FPR128_lo
      0,	// qsub2
      0,	// qsub3
      40,	// ssub -> QQ_with_qsub0_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      40,	// qsub1_then_bsub -> QQ_with_qsub0_in_FPR128_lo
      40,	// qsub1_then_dsub -> QQ_with_qsub0_in_FPR128_lo
      40,	// qsub1_then_hsub -> QQ_with_qsub0_in_FPR128_lo
      40,	// qsub1_then_ssub -> QQ_with_qsub0_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      40,	// dsub_qsub1_then_dsub -> QQ_with_qsub0_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQ_with_qsub1_in_FPR128_lo
      41,	// bsub -> QQ_with_qsub1_in_FPR128_lo
      41,	// dsub -> QQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      41,	// hsub -> QQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      41,	// qsub0 -> QQ_with_qsub1_in_FPR128_lo
      41,	// qsub1 -> QQ_with_qsub1_in_FPR128_lo
      0,	// qsub2
      0,	// qsub3
      41,	// ssub -> QQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      41,	// qsub1_then_bsub -> QQ_with_qsub1_in_FPR128_lo
      41,	// qsub1_then_dsub -> QQ_with_qsub1_in_FPR128_lo
      41,	// qsub1_then_hsub -> QQ_with_qsub1_in_FPR128_lo
      41,	// qsub1_then_ssub -> QQ_with_qsub1_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      41,	// dsub_qsub1_then_dsub -> QQ_with_qsub1_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub1_in_ZPR_4b
      42,	// bsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// dsub -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      42,	// hsub -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      42,	// ssub -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      42,	// zsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub0 -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1 -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// zsub2
      0,	// zsub3
      42,	// zsub_hi -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      42,	// zsub1_then_bsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1_then_dsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1_then_hsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1_then_ssub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1_then_zsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub1_then_zsub_hi -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      42,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub1_in_ZPR_4b
      42,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub_in_FPR128_lo
      43,	// bsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// dsub -> ZPR2_with_zsub_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      43,	// hsub -> ZPR2_with_zsub_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      43,	// ssub -> ZPR2_with_zsub_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      43,	// zsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub0 -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1 -> ZPR2_with_zsub_in_FPR128_lo
      0,	// zsub2
      0,	// zsub3
      43,	// zsub_hi -> ZPR2_with_zsub_in_FPR128_lo
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      43,	// zsub1_then_bsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1_then_hsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1_then_ssub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub1_then_zsub_hi -> ZPR2_with_zsub_in_FPR128_lo
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      43,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo
      43,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// bsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// dsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      44,	// hsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      44,	// qsub0 -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// qsub1 -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// qsub2
      0,	// qsub3
      44,	// ssub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      44,	// qsub1_then_bsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// qsub1_then_dsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// qsub1_then_hsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      44,	// qsub1_then_ssub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      44,	// dsub_qsub1_then_dsub -> QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// bsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      45,	// hsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      45,	// ssub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      45,	// zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub0 -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1 -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// zsub2
      0,	// zsub3
      45,	// zsub_hi -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      45,	// zsub1_then_bsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1_then_hsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1_then_ssub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub1_then_zsub_hi -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      45,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      45,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub0_in_ZPR_3b
      46,	// bsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// dsub -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      46,	// hsub -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      46,	// ssub -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      46,	// zsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub0 -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1 -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// zsub2
      0,	// zsub3
      46,	// zsub_hi -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      46,	// zsub1_then_bsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1_then_dsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1_then_hsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1_then_ssub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1_then_zsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub1_then_zsub_hi -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      46,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub0_in_ZPR_3b
      46,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub0_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub1_in_ZPR_3b
      47,	// bsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// dsub -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      47,	// hsub -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      47,	// ssub -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      47,	// zsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub0 -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1 -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// zsub2
      0,	// zsub3
      47,	// zsub_hi -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      47,	// zsub1_then_bsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1_then_dsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1_then_hsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1_then_ssub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1_then_zsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub1_then_zsub_hi -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      47,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub1_in_ZPR_3b
      47,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// bsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      48,	// hsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      48,	// ssub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      48,	// zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub0 -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1 -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// zsub2
      0,	// zsub3
      48,	// zsub_hi -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      48,	// zsub1_then_bsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1_then_hsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1_then_ssub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub1_then_zsub_hi -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      48,	// dsub_zsub1_then_dsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      48,	// zsub_zsub1_then_zsub -> ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ
      49,	// bsub -> QQQ
      49,	// dsub -> QQQ
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      49,	// hsub -> QQQ
      0,	// qhisub
      0,	// qsub
      49,	// qsub0 -> QQQ
      49,	// qsub1 -> QQQ
      49,	// qsub2 -> QQQ
      0,	// qsub3
      49,	// ssub -> QQQ
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      49,	// qsub1_then_bsub -> QQQ
      49,	// qsub1_then_dsub -> QQQ
      49,	// qsub1_then_hsub -> QQQ
      49,	// qsub1_then_ssub -> QQQ
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      49,	// qsub2_then_bsub -> QQQ
      49,	// qsub2_then_dsub -> QQQ
      49,	// qsub2_then_hsub -> QQQ
      49,	// qsub2_then_ssub -> QQQ
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      49,	// dsub_qsub1_then_dsub -> QQQ
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      49,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ
      49,	// qsub0_qsub1 -> QQQ
      0,	// qsub0_qsub1_qsub2
      49,	// qsub1_qsub2 -> QQQ
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      49,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3
      50,	// bsub -> ZPR3
      50,	// dsub -> ZPR3
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      50,	// hsub -> ZPR3
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      50,	// ssub -> ZPR3
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      50,	// zsub -> ZPR3
      50,	// zsub0 -> ZPR3
      50,	// zsub1 -> ZPR3
      50,	// zsub2 -> ZPR3
      0,	// zsub3
      50,	// zsub_hi -> ZPR3
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      50,	// zsub1_then_bsub -> ZPR3
      50,	// zsub1_then_dsub -> ZPR3
      50,	// zsub1_then_hsub -> ZPR3
      50,	// zsub1_then_ssub -> ZPR3
      50,	// zsub1_then_zsub -> ZPR3
      50,	// zsub1_then_zsub_hi -> ZPR3
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      50,	// zsub2_then_bsub -> ZPR3
      50,	// zsub2_then_dsub -> ZPR3
      50,	// zsub2_then_hsub -> ZPR3
      50,	// zsub2_then_ssub -> ZPR3
      50,	// zsub2_then_zsub -> ZPR3
      50,	// zsub2_then_zsub_hi -> ZPR3
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      50,	// dsub_zsub1_then_dsub -> ZPR3
      50,	// zsub_zsub1_then_zsub -> ZPR3
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      50,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      50,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3
      50,	// zsub0_zsub1 -> ZPR3
      0,	// zsub0_zsub1_zsub2
      50,	// zsub1_zsub2 -> ZPR3
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      50,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      50,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub0_in_FPR128_lo
      51,	// bsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// dsub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      51,	// hsub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      51,	// qsub0 -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub1 -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub2 -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qsub3
      51,	// ssub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      51,	// qsub1_then_bsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub1_then_hsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub1_then_ssub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      51,	// qsub2_then_bsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub2_then_hsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub2_then_ssub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      51,	// dsub_qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      51,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo
      51,	// qsub0_qsub1 -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      51,	// qsub1_qsub2 -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      51,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub1_in_FPR128_lo
      52,	// bsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// dsub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      52,	// hsub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      52,	// qsub0 -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub1 -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub2 -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub3
      52,	// ssub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      52,	// qsub1_then_bsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub1_then_dsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub1_then_hsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub1_then_ssub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      52,	// qsub2_then_bsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub2_then_hsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub2_then_ssub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      52,	// dsub_qsub1_then_dsub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      52,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo
      52,	// qsub0_qsub1 -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      52,	// qsub1_qsub2 -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      52,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub2_in_FPR128_lo
      53,	// bsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// dsub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      53,	// hsub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      53,	// qsub0 -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub1 -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub2 -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3
      53,	// ssub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      53,	// qsub1_then_bsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub1_then_dsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub1_then_hsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub1_then_ssub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      53,	// qsub2_then_bsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub2_then_dsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub2_then_hsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub2_then_ssub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      53,	// dsub_qsub1_then_dsub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      53,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub2_in_FPR128_lo
      53,	// qsub0_qsub1 -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      53,	// qsub1_qsub2 -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      53,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub1_in_ZPR_4b
      54,	// bsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// dsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      54,	// hsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      54,	// ssub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      54,	// zsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub0 -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1 -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2 -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub3
      54,	// zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      54,	// zsub1_then_bsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1_then_hsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1_then_ssub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub1_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      54,	// zsub2_then_bsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2_then_hsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2_then_ssub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub2_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      54,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      54,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      54,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b
      54,	// zsub0_zsub1 -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub0_zsub1_zsub2
      54,	// zsub1_zsub2 -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      54,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      54,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub2_in_ZPR_4b
      55,	// bsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// dsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      55,	// hsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      55,	// ssub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      55,	// zsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub0 -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1 -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2 -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3
      55,	// zsub_hi -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      55,	// zsub1_then_bsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1_then_dsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1_then_hsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1_then_ssub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1_then_zsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub1_then_zsub_hi -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      55,	// zsub2_then_bsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2_then_hsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2_then_ssub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub2_then_zsub_hi -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      55,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      55,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      55,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_4b
      55,	// zsub0_zsub1 -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub0_zsub1_zsub2
      55,	// zsub1_zsub2 -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      55,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      55,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub_in_FPR128_lo
      56,	// bsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// dsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      56,	// hsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      56,	// ssub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      56,	// zsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub0 -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1 -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2 -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub3
      56,	// zsub_hi -> ZPR3_with_zsub_in_FPR128_lo
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      56,	// zsub1_then_bsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1_then_hsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1_then_ssub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub1_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      56,	// zsub2_then_bsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2_then_hsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2_then_ssub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub2_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      56,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      56,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      56,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo
      56,	// zsub0_zsub1 -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub0_zsub1_zsub2
      56,	// zsub1_zsub2 -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      56,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      56,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      57,	// hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      57,	// qsub0 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub1 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub2 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub3
      57,	// ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      57,	// qsub1_then_bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub1_then_hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub1_then_ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      57,	// qsub2_then_bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub2_then_hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub2_then_ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      57,	// dsub_qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      57,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      57,	// qsub0_qsub1 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      57,	// qsub1_qsub2 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      57,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// bsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      58,	// hsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      58,	// qsub0 -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub1 -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub2 -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3
      58,	// ssub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      58,	// qsub1_then_bsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub1_then_dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub1_then_hsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub1_then_ssub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      58,	// qsub2_then_bsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub2_then_hsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub2_then_ssub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      58,	// dsub_qsub1_then_dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      58,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      58,	// qsub0_qsub1 -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      58,	// qsub1_qsub2 -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      58,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// bsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      59,	// hsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      59,	// ssub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      59,	// zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub0 -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1 -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2 -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3
      59,	// zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      59,	// zsub1_then_bsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1_then_hsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1_then_ssub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub1_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      59,	// zsub2_then_bsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2_then_hsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2_then_ssub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub2_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      59,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      59,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      59,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      59,	// zsub0_zsub1 -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub0_zsub1_zsub2
      59,	// zsub1_zsub2 -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      59,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      59,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      60,	// hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      60,	// ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      60,	// zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub0 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub3
      60,	// zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      60,	// zsub1_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub1_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      60,	// zsub2_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub2_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      60,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      60,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      60,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      60,	// zsub0_zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub0_zsub1_zsub2
      60,	// zsub1_zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      60,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      60,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      61,	// hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      61,	// qsub0 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub1 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub2 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3
      61,	// ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      61,	// qsub1_then_bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub1_then_hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub1_then_ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      61,	// qsub2_then_bsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub2_then_hsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub2_then_ssub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      61,	// dsub_qsub1_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      61,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      61,	// qsub0_qsub1 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub0_qsub1_qsub2
      61,	// qsub1_qsub2 -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      61,	// qsub1_then_dsub_qsub2_then_dsub -> QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      62,	// hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      62,	// ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      62,	// zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub0 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3
      62,	// zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      62,	// zsub1_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub1_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      62,	// zsub2_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub2_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      62,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      62,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      62,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      62,	// zsub0_zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub0_zsub1_zsub2
      62,	// zsub1_zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      62,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      62,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub0_in_ZPR_3b
      63,	// bsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// dsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      63,	// hsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      63,	// ssub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      63,	// zsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub0 -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1 -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2 -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub3
      63,	// zsub_hi -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      63,	// zsub1_then_bsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1_then_dsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1_then_hsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1_then_ssub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1_then_zsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub1_then_zsub_hi -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      63,	// zsub2_then_bsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2_then_dsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2_then_hsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2_then_ssub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2_then_zsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub2_then_zsub_hi -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      63,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      63,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      63,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub0_in_ZPR_3b
      63,	// zsub0_zsub1 -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      63,	// zsub1_zsub2 -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      63,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      63,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub0_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub1_in_ZPR_3b
      64,	// bsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// dsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      64,	// hsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      64,	// ssub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      64,	// zsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub0 -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1 -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2 -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub3
      64,	// zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      64,	// zsub1_then_bsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1_then_hsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1_then_ssub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub1_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      64,	// zsub2_then_bsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2_then_hsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2_then_ssub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub2_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      64,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      64,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      64,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b
      64,	// zsub0_zsub1 -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      64,	// zsub1_zsub2 -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      64,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      64,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub2_in_ZPR_3b
      65,	// bsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// dsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      65,	// hsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      65,	// ssub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      65,	// zsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub0 -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1 -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2 -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3
      65,	// zsub_hi -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      65,	// zsub1_then_bsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1_then_dsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1_then_hsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1_then_ssub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1_then_zsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub1_then_zsub_hi -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      65,	// zsub2_then_bsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2_then_hsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2_then_ssub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub2_then_zsub_hi -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      65,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      65,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      65,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_3b
      65,	// zsub0_zsub1 -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      65,	// zsub1_zsub2 -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      65,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      65,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// bsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      66,	// hsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      66,	// ssub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      66,	// zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub0 -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1 -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2 -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3
      66,	// zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      66,	// zsub1_then_bsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1_then_hsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1_then_ssub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub1_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      66,	// zsub2_then_bsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2_then_hsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2_then_ssub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub2_then_zsub_hi -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      66,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      66,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      66,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      66,	// zsub0_zsub1 -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      66,	// zsub1_zsub2 -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      66,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      66,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      67,	// hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      67,	// ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      67,	// zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub0 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub3
      67,	// zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      67,	// zsub1_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub1_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      67,	// zsub2_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub2_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      67,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      67,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      67,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      67,	// zsub0_zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      67,	// zsub1_zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      67,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      67,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      68,	// hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      68,	// ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      68,	// zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub0 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3
      68,	// zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      68,	// zsub1_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub1_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      68,	// zsub2_then_bsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2_then_hsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2_then_ssub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub2_then_zsub_hi -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      68,	// dsub_zsub1_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub_zsub1_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      68,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      68,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      68,	// zsub0_zsub1 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub0_zsub1_zsub2
      68,	// zsub1_zsub2 -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      68,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      68,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ
      69,	// bsub -> QQQQ
      69,	// dsub -> QQQQ
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      69,	// hsub -> QQQQ
      0,	// qhisub
      0,	// qsub
      69,	// qsub0 -> QQQQ
      69,	// qsub1 -> QQQQ
      69,	// qsub2 -> QQQQ
      69,	// qsub3 -> QQQQ
      69,	// ssub -> QQQQ
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      69,	// qsub1_then_bsub -> QQQQ
      69,	// qsub1_then_dsub -> QQQQ
      69,	// qsub1_then_hsub -> QQQQ
      69,	// qsub1_then_ssub -> QQQQ
      69,	// qsub3_then_bsub -> QQQQ
      69,	// qsub3_then_dsub -> QQQQ
      69,	// qsub3_then_hsub -> QQQQ
      69,	// qsub3_then_ssub -> QQQQ
      69,	// qsub2_then_bsub -> QQQQ
      69,	// qsub2_then_dsub -> QQQQ
      69,	// qsub2_then_hsub -> QQQQ
      69,	// qsub2_then_ssub -> QQQQ
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      69,	// dsub_qsub1_then_dsub -> QQQQ
      69,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ
      69,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ
      69,	// qsub0_qsub1 -> QQQQ
      69,	// qsub0_qsub1_qsub2 -> QQQQ
      69,	// qsub1_qsub2 -> QQQQ
      69,	// qsub1_qsub2_qsub3 -> QQQQ
      69,	// qsub2_qsub3 -> QQQQ
      69,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ
      69,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ
      69,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR4
      70,	// bsub -> ZPR4
      70,	// dsub -> ZPR4
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      70,	// hsub -> ZPR4
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      70,	// ssub -> ZPR4
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      70,	// zsub -> ZPR4
      70,	// zsub0 -> ZPR4
      70,	// zsub1 -> ZPR4
      70,	// zsub2 -> ZPR4
      70,	// zsub3 -> ZPR4
      70,	// zsub_hi -> ZPR4
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      70,	// zsub1_then_bsub -> ZPR4
      70,	// zsub1_then_dsub -> ZPR4
      70,	// zsub1_then_hsub -> ZPR4
      70,	// zsub1_then_ssub -> ZPR4
      70,	// zsub1_then_zsub -> ZPR4
      70,	// zsub1_then_zsub_hi -> ZPR4
      70,	// zsub3_then_bsub -> ZPR4
      70,	// zsub3_then_dsub -> ZPR4
      70,	// zsub3_then_hsub -> ZPR4
      70,	// zsub3_then_ssub -> ZPR4
      70,	// zsub3_then_zsub -> ZPR4
      70,	// zsub3_then_zsub_hi -> ZPR4
      70,	// zsub2_then_bsub -> ZPR4
      70,	// zsub2_then_dsub -> ZPR4
      70,	// zsub2_then_hsub -> ZPR4
      70,	// zsub2_then_ssub -> ZPR4
      70,	// zsub2_then_zsub -> ZPR4
      70,	// zsub2_then_zsub_hi -> ZPR4
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      70,	// dsub_zsub1_then_dsub -> ZPR4
      70,	// zsub_zsub1_then_zsub -> ZPR4
      70,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4
      70,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4
      70,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4
      70,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4
      70,	// zsub0_zsub1 -> ZPR4
      70,	// zsub0_zsub1_zsub2 -> ZPR4
      70,	// zsub1_zsub2 -> ZPR4
      70,	// zsub1_zsub2_zsub3 -> ZPR4
      70,	// zsub2_zsub3 -> ZPR4
      70,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4
      70,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4
      70,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4
      70,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4
      70,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4
      70,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4
    },
    {	// QQQQ_with_qsub0_in_FPR128_lo
      71,	// bsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// dsub -> QQQQ_with_qsub0_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      71,	// hsub -> QQQQ_with_qsub0_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      71,	// qsub0 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub3 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// ssub -> QQQQ_with_qsub0_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      71,	// qsub1_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub3_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub3_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub3_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      71,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub0_qsub1 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      71,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub1_in_FPR128_lo
      72,	// bsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// dsub -> QQQQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      72,	// hsub -> QQQQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      72,	// qsub0 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub3 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// ssub -> QQQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      72,	// qsub1_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub3_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub3_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub3_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      72,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub0_qsub1 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      72,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub2_in_FPR128_lo
      73,	// bsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// dsub -> QQQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      73,	// hsub -> QQQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      73,	// qsub0 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub3 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// ssub -> QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      73,	// qsub1_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub3_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub3_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub3_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      73,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub0_qsub1 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_qsub2 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_qsub3 -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      73,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub3_in_FPR128_lo
      74,	// bsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// dsub -> QQQQ_with_qsub3_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      74,	// hsub -> QQQQ_with_qsub3_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      74,	// qsub0 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub3 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// ssub -> QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      74,	// qsub1_then_bsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_then_hsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_then_ssub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub3_then_bsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub3_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub3_then_hsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub3_then_ssub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_then_bsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_then_hsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_then_ssub -> QQQQ_with_qsub3_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      74,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub0_qsub1 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_qsub2 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_qsub3 -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      74,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR4_with_zsub1_in_ZPR_4b
      75,	// bsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// dsub -> ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      75,	// hsub -> ZPR4_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      75,	// ssub -> ZPR4_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      75,	// zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      75,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      75,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b
      75,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b
    },
    {	// ZPR4_with_zsub2_in_ZPR_4b
      76,	// bsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// dsub -> ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      76,	// hsub -> ZPR4_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      76,	// ssub -> ZPR4_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      76,	// zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub0 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      76,	// zsub1_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub3_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      76,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub0_zsub1 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b
      76,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b
    },
    {	// ZPR4_with_zsub3_in_ZPR_4b
      77,	// bsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// dsub -> ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      77,	// hsub -> ZPR4_with_zsub3_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      77,	// ssub -> ZPR4_with_zsub3_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      77,	// zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub0 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub_hi -> ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      77,	// zsub1_then_bsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_hsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_ssub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_bsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_hsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_ssub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub3_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_bsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_hsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_ssub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      77,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub0_zsub1 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_zsub2 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_zsub3 -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_4b
      77,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_4b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo
      78,	// bsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// dsub -> ZPR4_with_zsub_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      78,	// hsub -> ZPR4_with_zsub_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      78,	// ssub -> ZPR4_with_zsub_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      78,	// zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      78,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      78,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo
      78,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo
    },
    {	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      79,	// hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      79,	// qsub0 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      79,	// qsub1_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub3_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub3_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub3_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      79,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub0_qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      79,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      80,	// hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      80,	// qsub0 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      80,	// qsub1_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub3_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub3_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub3_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      80,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub0_qsub1 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      80,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// bsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      81,	// hsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      81,	// qsub0 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub3 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// ssub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      81,	// qsub1_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub3_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub3_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub3_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_then_bsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_then_hsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_then_ssub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      81,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub0_qsub1 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_qsub2 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_qsub3 -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      81,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      82,	// hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      82,	// ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      82,	// zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      82,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      82,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
      82,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
    },
    {	// ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// bsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      83,	// hsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      83,	// ssub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      83,	// zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub0 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      83,	// zsub1_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub3_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_bsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_hsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_ssub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      83,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub0_zsub1 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      83,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      84,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      84,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      84,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      84,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      84,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
      84,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
    },
    {	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      85,	// hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      85,	// qsub0 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      85,	// qsub1_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub3_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub3_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub3_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      85,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub0_qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      85,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      86,	// hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      86,	// qsub0 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      86,	// qsub1_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub3_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub3_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub3_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_then_bsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_then_hsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_then_ssub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      86,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub0_qsub1 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_qsub2 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_qsub3 -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      86,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      87,	// hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      87,	// ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      87,	// zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      87,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      87,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
      87,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      88,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      88,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      88,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      88,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      88,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
      88,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
    },
    {	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      89,	// hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// qhisub
      0,	// qsub
      89,	// qsub0 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      0,	// zsub
      0,	// zsub0
      0,	// zsub1
      0,	// zsub2
      0,	// zsub3
      0,	// zsub_hi
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      89,	// qsub1_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub3_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub3_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub3_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_then_bsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_then_hsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_then_ssub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// subo64_then_sub_32
      0,	// zsub1_then_bsub
      0,	// zsub1_then_dsub
      0,	// zsub1_then_hsub
      0,	// zsub1_then_ssub
      0,	// zsub1_then_zsub
      0,	// zsub1_then_zsub_hi
      0,	// zsub3_then_bsub
      0,	// zsub3_then_dsub
      0,	// zsub3_then_hsub
      0,	// zsub3_then_ssub
      0,	// zsub3_then_zsub
      0,	// zsub3_then_zsub_hi
      0,	// zsub2_then_bsub
      0,	// zsub2_then_dsub
      0,	// zsub2_then_hsub
      0,	// zsub2_then_ssub
      0,	// zsub2_then_zsub
      0,	// zsub2_then_zsub_hi
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      89,	// dsub_qsub1_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// dsub_qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub0_qsub1 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub0_qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_qsub2 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_qsub3 -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_then_dsub_qsub2_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      89,	// qsub2_then_dsub_qsub3_then_dsub -> QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
      0,	// sub_32_subo64_then_sub_32
      0,	// dsub_zsub1_then_dsub
      0,	// zsub_zsub1_then_zsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// dsub_zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub_zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub0_zsub1
      0,	// zsub0_zsub1_zsub2
      0,	// zsub1_zsub2
      0,	// zsub1_zsub2_zsub3
      0,	// zsub2_zsub3
      0,	// zsub1_then_dsub_zsub2_then_dsub
      0,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub1_then_zsub_zsub2_then_zsub
      0,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub
      0,	// zsub2_then_dsub_zsub3_then_dsub
      0,	// zsub2_then_zsub_zsub3_then_zsub
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      90,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      90,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      90,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      90,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      90,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
      90,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
    },
    {	// ZPR4_with_zsub0_in_ZPR_3b
      91,	// bsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// dsub -> ZPR4_with_zsub0_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      91,	// hsub -> ZPR4_with_zsub0_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      91,	// ssub -> ZPR4_with_zsub0_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      91,	// zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub0 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub_hi -> ZPR4_with_zsub0_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      91,	// zsub1_then_bsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_hsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_ssub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_zsub_hi -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_bsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_hsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_ssub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub3_then_zsub_hi -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_bsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_hsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_ssub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_zsub_hi -> ZPR4_with_zsub0_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      91,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub0_zsub1 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_zsub2 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_zsub3 -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub0_in_ZPR_3b
      91,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub0_in_ZPR_3b
    },
    {	// ZPR4_with_zsub1_in_ZPR_3b
      92,	// bsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// dsub -> ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      92,	// hsub -> ZPR4_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      92,	// ssub -> ZPR4_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      92,	// zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      92,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      92,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b
      92,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b
    },
    {	// ZPR4_with_zsub2_in_ZPR_3b
      93,	// bsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// dsub -> ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      93,	// hsub -> ZPR4_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      93,	// ssub -> ZPR4_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      93,	// zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub0 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      93,	// zsub1_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub3_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      93,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub0_zsub1 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b
      93,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b
    },
    {	// ZPR4_with_zsub3_in_ZPR_3b
      94,	// bsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// dsub -> ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      94,	// hsub -> ZPR4_with_zsub3_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      94,	// ssub -> ZPR4_with_zsub3_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      94,	// zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub0 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub_hi -> ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      94,	// zsub1_then_bsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_hsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_ssub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_bsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_hsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_ssub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub3_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_bsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_hsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_ssub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_zsub_hi -> ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      94,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub0_zsub1 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_zsub2 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_zsub3 -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub3_in_ZPR_3b
      94,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub3_in_ZPR_3b
    },
    {	// ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      95,	// hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      95,	// ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      95,	// zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      95,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      95,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
      95,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
    },
    {	// ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// bsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      96,	// hsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      96,	// ssub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      96,	// zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub0 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      96,	// zsub1_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub3_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_bsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_hsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_ssub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_zsub_hi -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      96,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub0_zsub1 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_zsub2 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_zsub3 -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      96,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      97,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      97,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      97,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      97,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      97,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
      97,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
    },
    {	// ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      98,	// hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      98,	// ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      98,	// zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub0 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      98,	// zsub1_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub3_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_bsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_hsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_ssub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_zsub_hi -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      98,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub0_zsub1 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_zsub2 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_zsub3 -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
      98,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      99,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      99,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      99,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      99,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      99,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
      99,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
    },
    {	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0
      0,	// dsub1
      0,	// dsub2
      0,	// dsub3
      100,	// hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// qhisub
      0,	// qsub
      0,	// qsub0
      0,	// qsub1
      0,	// qsub2
      0,	// qsub3
      100,	// ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// sub_32
      0,	// sube32
      0,	// sube64
      0,	// subo32
      0,	// subo64
      100,	// zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub0 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub1_then_bsub
      0,	// dsub1_then_hsub
      0,	// dsub1_then_ssub
      0,	// dsub3_then_bsub
      0,	// dsub3_then_hsub
      0,	// dsub3_then_ssub
      0,	// dsub2_then_bsub
      0,	// dsub2_then_hsub
      0,	// dsub2_then_ssub
      0,	// qsub1_then_bsub
      0,	// qsub1_then_dsub
      0,	// qsub1_then_hsub
      0,	// qsub1_then_ssub
      0,	// qsub3_then_bsub
      0,	// qsub3_then_dsub
      0,	// qsub3_then_hsub
      0,	// qsub3_then_ssub
      0,	// qsub2_then_bsub
      0,	// qsub2_then_dsub
      0,	// qsub2_then_hsub
      0,	// qsub2_then_ssub
      0,	// subo64_then_sub_32
      100,	// zsub1_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub3_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_bsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_hsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_ssub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_zsub_hi -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      0,	// dsub0_dsub1
      0,	// dsub0_dsub1_dsub2
      0,	// dsub1_dsub2
      0,	// dsub1_dsub2_dsub3
      0,	// dsub2_dsub3
      0,	// dsub_qsub1_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// dsub_qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub0_qsub1
      0,	// qsub0_qsub1_qsub2
      0,	// qsub1_qsub2
      0,	// qsub1_qsub2_qsub3
      0,	// qsub2_qsub3
      0,	// qsub1_then_dsub_qsub2_then_dsub
      0,	// qsub1_then_dsub_qsub2_then_dsub_qsub3_then_dsub
      0,	// qsub2_then_dsub_qsub3_then_dsub
      0,	// sub_32_subo64_then_sub_32
      100,	// dsub_zsub1_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub_zsub1_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// dsub_zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// dsub_zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub_zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub_zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub0_zsub1 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub0_zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_zsub2 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_zsub3 -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_dsub_zsub2_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_dsub_zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_zsub_zsub2_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub1_then_zsub_zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_dsub_zsub3_then_dsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
      100,	// zsub2_then_zsub_zsub3_then_zsub -> ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
    },
  };
  assert(RC && "Missing regclass");
  if (!Idx) return RC;
  --Idx;
  assert(Idx < 99 && "Bad subreg");
  unsigned TV = Table[RC->getID()][Idx];
  return TV ? getRegClass(TV - 1) : nullptr;
}

/// Get the weight in units of pressure for this register class.
const RegClassWeight &AArch64GenRegisterInfo::
getRegClassWeight(const TargetRegisterClass *RC) const {
  static const RegClassWeight RCWeightTable[] = {
    {1, 32},  	// FPR8
    {1, 32},  	// FPR16
    {1, 16},  	// PPR
    {1, 8},  	// PPR_3b
    {1, 33},  	// GPR32all
    {1, 32},  	// FPR32
    {1, 32},  	// GPR32
    {1, 32},  	// GPR32sp
    {1, 31},  	// GPR32common
    {0, 0},  	// CCR
    {1, 1},  	// GPR32sponly
    {2, 32},  	// WSeqPairsClass
    {2, 32},  	// WSeqPairsClass_with_sube32_in_GPR32common
    {2, 32},  	// WSeqPairsClass_with_subo32_in_GPR32common
    {2, 31},  	// WSeqPairsClass_with_sube32_in_GPR32common_and_WSeqPairsClass_with_subo32_in_GPR32common
    {1, 33},  	// GPR64all
    {1, 32},  	// FPR64
    {1, 32},  	// GPR64
    {1, 32},  	// GPR64sp
    {1, 31},  	// GPR64common
    {1, 19},  	// tcGPR64
    {1, 1},  	// GPR64sponly
    {2, 32},  	// DD
    {2, 32},  	// XSeqPairsClass
    {2, 32},  	// XSeqPairsClass_with_sub_32_in_GPR32common
    {2, 32},  	// XSeqPairsClass_with_subo64_in_GPR64common
    {2, 31},  	// XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_GPR64common
    {2, 20},  	// XSeqPairsClass_with_sube64_in_tcGPR64
    {2, 20},  	// XSeqPairsClass_with_subo64_in_tcGPR64
    {2, 19},  	// XSeqPairsClass_with_sub_32_in_GPR32common_and_XSeqPairsClass_with_subo64_in_tcGPR64
    {1, 32},  	// FPR128
    {2, 64},  	// ZPR
    {1, 16},  	// FPR128_lo
    {2, 32},  	// ZPR_4b
    {2, 16},  	// ZPR_3b
    {3, 32},  	// DDD
    {4, 32},  	// DDDD
    {2, 32},  	// QQ
    {4, 64},  	// ZPR2
    {2, 17},  	// QQ_with_qsub0_in_FPR128_lo
    {2, 17},  	// QQ_with_qsub1_in_FPR128_lo
    {4, 34},  	// ZPR2_with_zsub1_in_ZPR_4b
    {4, 34},  	// ZPR2_with_zsub_in_FPR128_lo
    {2, 16},  	// QQ_with_qsub0_in_FPR128_lo_and_QQ_with_qsub1_in_FPR128_lo
    {4, 32},  	// ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_4b
    {4, 18},  	// ZPR2_with_zsub0_in_ZPR_3b
    {4, 18},  	// ZPR2_with_zsub1_in_ZPR_3b
    {4, 16},  	// ZPR2_with_zsub_in_FPR128_lo_and_ZPR2_with_zsub1_in_ZPR_3b
    {3, 32},  	// QQQ
    {6, 64},  	// ZPR3
    {3, 18},  	// QQQ_with_qsub0_in_FPR128_lo
    {3, 18},  	// QQQ_with_qsub1_in_FPR128_lo
    {3, 18},  	// QQQ_with_qsub2_in_FPR128_lo
    {6, 36},  	// ZPR3_with_zsub1_in_ZPR_4b
    {6, 36},  	// ZPR3_with_zsub2_in_ZPR_4b
    {6, 36},  	// ZPR3_with_zsub_in_FPR128_lo
    {3, 17},  	// QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub1_in_FPR128_lo
    {3, 17},  	// QQQ_with_qsub1_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
    {6, 34},  	// ZPR3_with_zsub1_in_ZPR_4b_and_ZPR3_with_zsub2_in_ZPR_4b
    {6, 34},  	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_4b
    {3, 16},  	// QQQ_with_qsub0_in_FPR128_lo_and_QQQ_with_qsub2_in_FPR128_lo
    {6, 32},  	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_4b
    {6, 20},  	// ZPR3_with_zsub0_in_ZPR_3b
    {6, 20},  	// ZPR3_with_zsub1_in_ZPR_3b
    {6, 20},  	// ZPR3_with_zsub2_in_ZPR_3b
    {6, 18},  	// ZPR3_with_zsub1_in_ZPR_3b_and_ZPR3_with_zsub2_in_ZPR_3b
    {6, 18},  	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub1_in_ZPR_3b
    {6, 16},  	// ZPR3_with_zsub_in_FPR128_lo_and_ZPR3_with_zsub2_in_ZPR_3b
    {4, 32},  	// QQQQ
    {8, 64},  	// ZPR4
    {4, 19},  	// QQQQ_with_qsub0_in_FPR128_lo
    {4, 19},  	// QQQQ_with_qsub1_in_FPR128_lo
    {4, 19},  	// QQQQ_with_qsub2_in_FPR128_lo
    {4, 19},  	// QQQQ_with_qsub3_in_FPR128_lo
    {8, 38},  	// ZPR4_with_zsub1_in_ZPR_4b
    {8, 38},  	// ZPR4_with_zsub2_in_ZPR_4b
    {8, 38},  	// ZPR4_with_zsub3_in_ZPR_4b
    {8, 38},  	// ZPR4_with_zsub_in_FPR128_lo
    {4, 18},  	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub1_in_FPR128_lo
    {4, 18},  	// QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
    {4, 18},  	// QQQQ_with_qsub2_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
    {8, 36},  	// ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub2_in_ZPR_4b
    {8, 36},  	// ZPR4_with_zsub2_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
    {8, 36},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_4b
    {4, 17},  	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub2_in_FPR128_lo
    {4, 17},  	// QQQQ_with_qsub1_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
    {8, 34},  	// ZPR4_with_zsub1_in_ZPR_4b_and_ZPR4_with_zsub3_in_ZPR_4b
    {8, 34},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_4b
    {4, 16},  	// QQQQ_with_qsub0_in_FPR128_lo_and_QQQQ_with_qsub3_in_FPR128_lo
    {8, 32},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_4b
    {8, 22},  	// ZPR4_with_zsub0_in_ZPR_3b
    {8, 22},  	// ZPR4_with_zsub1_in_ZPR_3b
    {8, 22},  	// ZPR4_with_zsub2_in_ZPR_3b
    {8, 22},  	// ZPR4_with_zsub3_in_ZPR_3b
    {8, 20},  	// ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub2_in_ZPR_3b
    {8, 20},  	// ZPR4_with_zsub2_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
    {8, 20},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub1_in_ZPR_3b
    {8, 18},  	// ZPR4_with_zsub1_in_ZPR_3b_and_ZPR4_with_zsub3_in_ZPR_3b
    {8, 18},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub2_in_ZPR_3b
    {8, 16},  	// ZPR4_with_zsub_in_FPR128_lo_and_ZPR4_with_zsub3_in_ZPR_3b
  };
  return RCWeightTable[RC->getID()];
}

/// Get the weight in units of pressure for this register unit.
unsigned AArch64GenRegisterInfo::
getRegUnitWeight(unsigned RegUnit) const {
  assert(RegUnit < 115 && "invalid register unit");
  // All register units have unit weight.
  return 1;
}


// Get the number of dimensions of register pressure.
unsigned AArch64GenRegisterInfo::getNumRegPressureSets() const {
  return 30;
}

// Get the name of this register unit pressure set.
const char *AArch64GenRegisterInfo::
getRegPressureSetName(unsigned Idx) const {
  static const char *const PressureNameTable[] = {
    "GPR32sponly",
    "PPR_3b",
    "PPR",
    "tcGPR64",
    "FPR128_lo",
    "ZPR_3b",
    "FPR128_lo+ZPR_3b",
    "QQ_with_qsub1_in_FPR128_lo+ZPR_3b",
    "QQQ_with_qsub2_in_FPR128_lo+ZPR_3b",
    "QQQ_with_qsub2_in_FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b",
    "QQQQ_with_qsub3_in_FPR128_lo+ZPR_3b",
    "QQQQ_with_qsub3_in_FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b",
    "QQQQ_with_qsub3_in_FPR128_lo+ZPR4_with_zsub2_in_ZPR_3b",
    "FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b",
    "FPR8",
    "FPR128_lo+ZPR4_with_zsub2_in_ZPR_3b",
    "GPR32",
    "FPR128_lo+ZPR4_with_zsub3_in_ZPR_3b",
    "ZPR4_with_zsub3_in_ZPR_4b",
    "ZPR4_with_zsub_in_FPR128_lo",
    "FPR8+ZPR_3b",
    "FPR8+ZPR4_with_zsub1_in_ZPR_3b",
    "FPR8+ZPR4_with_zsub2_in_ZPR_3b",
    "FPR8+ZPR4_with_zsub3_in_ZPR_3b",
    "ZPR_4b",
    "FPR8+ZPR_4b",
    "FPR8+ZPR4_with_zsub2_in_ZPR_4b",
    "FPR8+ZPR4_with_zsub3_in_ZPR_4b",
    "FPR8+ZPR4_with_zsub_in_FPR128_lo",
    "ZPR",
  };
  return PressureNameTable[Idx];
}

// Get the register unit pressure limit for this dimension.
// This limit must be adjusted dynamically for reserved registers.
unsigned AArch64GenRegisterInfo::
getRegPressureSetLimit(const MachineFunction &MF, unsigned Idx) const {
  static const uint8_t PressureLimitTable[] = {
    1,  	// 0: GPR32sponly
    8,  	// 1: PPR_3b
    16,  	// 2: PPR
    21,  	// 3: tcGPR64
    22,  	// 4: FPR128_lo
    28,  	// 5: ZPR_3b
    30,  	// 6: FPR128_lo+ZPR_3b
    30,  	// 7: QQ_with_qsub1_in_FPR128_lo+ZPR_3b
    30,  	// 8: QQQ_with_qsub2_in_FPR128_lo+ZPR_3b
    30,  	// 9: QQQ_with_qsub2_in_FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b
    30,  	// 10: QQQQ_with_qsub3_in_FPR128_lo+ZPR_3b
    30,  	// 11: QQQQ_with_qsub3_in_FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b
    30,  	// 12: QQQQ_with_qsub3_in_FPR128_lo+ZPR4_with_zsub2_in_ZPR_3b
    31,  	// 13: FPR128_lo+ZPR4_with_zsub1_in_ZPR_3b
    32,  	// 14: FPR8
    32,  	// 15: FPR128_lo+ZPR4_with_zsub2_in_ZPR_3b
    33,  	// 16: GPR32
    33,  	// 17: FPR128_lo+ZPR4_with_zsub3_in_ZPR_3b
    41,  	// 18: ZPR4_with_zsub3_in_ZPR_4b
    41,  	// 19: ZPR4_with_zsub_in_FPR128_lo
    43,  	// 20: FPR8+ZPR_3b
    43,  	// 21: FPR8+ZPR4_with_zsub1_in_ZPR_3b
    43,  	// 22: FPR8+ZPR4_with_zsub2_in_ZPR_3b
    43,  	// 23: FPR8+ZPR4_with_zsub3_in_ZPR_3b
    44,  	// 24: ZPR_4b
    51,  	// 25: FPR8+ZPR_4b
    51,  	// 26: FPR8+ZPR4_with_zsub2_in_ZPR_4b
    51,  	// 27: FPR8+ZPR4_with_zsub3_in_ZPR_4b
    51,  	// 28: FPR8+ZPR4_with_zsub_in_FPR128_lo
    64,  	// 29: ZPR
  };
  return PressureLimitTable[Idx];
}

/// Table of pressure sets per register class or unit.
static const int RCSetsTable[] = {
  /* 0 */ 1, 2, -1,
  /* 3 */ 0, 16, -1,
  /* 6 */ 3, 16, -1,
  /* 9 */ 24, 25, 29, -1,
  /* 13 */ 24, 26, 29, -1,
  /* 17 */ 24, 25, 26, 29, -1,
  /* 22 */ 18, 24, 27, 29, -1,
  /* 27 */ 5, 17, 18, 23, 24, 27, 29, -1,
  /* 35 */ 18, 24, 26, 27, 29, -1,
  /* 41 */ 5, 12, 15, 18, 22, 24, 26, 27, 29, -1,
  /* 51 */ 5, 12, 15, 17, 18, 22, 23, 24, 26, 27, 29, -1,
  /* 63 */ 18, 24, 25, 26, 27, 29, -1,
  /* 70 */ 5, 9, 11, 13, 18, 21, 24, 25, 26, 27, 29, -1,
  /* 82 */ 5, 9, 11, 12, 13, 15, 18, 21, 22, 24, 25, 26, 27, 29, -1,
  /* 97 */ 5, 9, 11, 12, 13, 15, 17, 18, 21, 22, 23, 24, 25, 26, 27, 29, -1,
  /* 114 */ 19, 24, 28, 29, -1,
  /* 119 */ 19, 24, 25, 28, 29, -1,
  /* 125 */ 19, 24, 25, 26, 28, 29, -1,
  /* 132 */ 14, 20, 21, 22, 23, 25, 26, 27, 28, 29, -1,
  /* 143 */ 18, 19, 24, 25, 26, 27, 28, 29, -1,
  /* 152 */ 5, 6, 7, 8, 10, 18, 19, 20, 24, 25, 26, 27, 28, 29, -1,
  /* 167 */ 5, 6, 7, 8, 9, 10, 11, 13, 18, 19, 20, 21, 24, 25, 26, 27, 28, 29, -1,
  /* 186 */ 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, -1,
  /* 208 */ 4, 10, 11, 12, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 227 */ 4, 5, 10, 11, 12, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 247 */ 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 271 */ 4, 8, 9, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 290 */ 4, 8, 9, 10, 11, 12, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 312 */ 4, 5, 8, 9, 10, 11, 12, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 335 */ 4, 6, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 354 */ 4, 7, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 373 */ 4, 6, 7, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 393 */ 4, 7, 8, 9, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 414 */ 4, 6, 7, 8, 9, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 436 */ 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 460 */ 4, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 485 */ 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  /* 510 */ 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
};

/// Get the dimensions of register pressure impacted by this register class.
/// Returns a -1 terminated array of pressure set IDs
const int* AArch64GenRegisterInfo::
getRegClassPressureSets(const TargetRegisterClass *RC) const {
  static const uint16_t RCSetStartTable[] = {
    132,132,1,0,4,132,4,4,4,2,3,4,4,4,4,4,132,4,4,4,6,3,132,4,4,4,4,6,6,6,132,11,485,143,247,132,132,132,11,414,436,63,125,485,143,186,97,247,132,11,373,393,290,17,35,119,414,436,63,125,485,143,167,82,51,97,186,247,132,11,335,354,271,208,9,13,22,114,373,393,290,17,35,119,414,436,63,125,485,143,152,70,41,27,82,51,167,97,186,247,};
  return &RCSetsTable[RCSetStartTable[RC->getID()]];
}

/// Get the dimensions of register pressure impacted by this register unit.
/// Returns a -1 terminated array of pressure set IDs
const int* AArch64GenRegisterInfo::
getRegUnitPressureSets(unsigned RegUnit) const {
  assert(RegUnit < 115 && "invalid register unit");
  static const uint16_t RUSetStartTable[] = {
    2,4,4,2,3,6,510,510,510,510,510,510,510,510,510,510,510,485,485,485,485,485,414,373,335,132,132,132,132,132,132,132,132,132,132,227,312,460,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,4,4,4,4,247,247,247,247,247,247,247,247,186,167,152,143,143,143,143,143,125,119,114,11,11,11,11,11,11,11,11,11,11,27,51,97,};
  return &RCSetsTable[RUSetStartTable[RegUnit]];
}

extern const MCRegisterDesc AArch64RegDesc[];
extern const MCPhysReg AArch64RegDiffLists[];
extern const LaneBitmask AArch64LaneMaskLists[];
extern const char AArch64RegStrings[];
extern const char AArch64RegClassStrings[];
extern const MCPhysReg AArch64RegUnitRoots[][2];
extern const uint16_t AArch64SubRegIdxLists[];
extern const MCRegisterInfo::SubRegCoveredBits AArch64SubRegIdxRanges[];
extern const uint16_t AArch64RegEncodingTable[];
// AArch64 Dwarf<->LLVM register mappings.
extern const MCRegisterInfo::DwarfLLVMRegPair AArch64DwarfFlavour0Dwarf2L[];
extern const unsigned AArch64DwarfFlavour0Dwarf2LSize;

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64EHFlavour0Dwarf2L[];
extern const unsigned AArch64EHFlavour0Dwarf2LSize;

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64DwarfFlavour0L2Dwarf[];
extern const unsigned AArch64DwarfFlavour0L2DwarfSize;

extern const MCRegisterInfo::DwarfLLVMRegPair AArch64EHFlavour0L2Dwarf[];
extern const unsigned AArch64EHFlavour0L2DwarfSize;

AArch64GenRegisterInfo::
AArch64GenRegisterInfo(unsigned RA, unsigned DwarfFlavour, unsigned EHFlavour,
      unsigned PC, unsigned HwMode)
  : TargetRegisterInfo(AArch64RegInfoDesc, RegisterClasses, RegisterClasses+100,
             SubRegIndexNameTable, SubRegIndexLaneMaskTable,
             LaneBitmask(0xFFFFFFB6), RegClassInfos, HwMode) {
  InitMCRegisterInfo(AArch64RegDesc, 661, RA, PC,
                     AArch64MCRegisterClasses, 100,
                     AArch64RegUnitRoots,
                     115,
                     AArch64RegDiffLists,
                     AArch64LaneMaskLists,
                     AArch64RegStrings,
                     AArch64RegClassStrings,
                     AArch64SubRegIdxLists,
                     100,
                     AArch64SubRegIdxRanges,
                     AArch64RegEncodingTable);

  switch (DwarfFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    mapDwarfRegsToLLVMRegs(AArch64DwarfFlavour0Dwarf2L, AArch64DwarfFlavour0Dwarf2LSize, false);
    break;
  }
  switch (EHFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    mapDwarfRegsToLLVMRegs(AArch64EHFlavour0Dwarf2L, AArch64EHFlavour0Dwarf2LSize, true);
    break;
  }
  switch (DwarfFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    mapLLVMRegsToDwarfRegs(AArch64DwarfFlavour0L2Dwarf, AArch64DwarfFlavour0L2DwarfSize, false);
    break;
  }
  switch (EHFlavour) {
  default:
    llvm_unreachable("Unknown DWARF flavour");
  case 0:
    mapLLVMRegsToDwarfRegs(AArch64EHFlavour0L2Dwarf, AArch64EHFlavour0L2DwarfSize, true);
    break;
  }
}

static const MCPhysReg CSR_AArch64_AAPCS_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, 0 };
static const uint32_t CSR_AArch64_AAPCS_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00fff000, 0x001ff800, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013ffc00, 0x001ff000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AAPCS_SCS_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X18, 0 };
static const uint32_t CSR_AArch64_AAPCS_SCS_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00fff800, 0x001ffc00, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013ffe00, 0x001ff800, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AAPCS_SwiftError_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, 0 };
static const uint32_t CSR_AArch64_AAPCS_SwiftError_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00ffb000, 0x001fd800, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013fe400, 0x001f9000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AAPCS_SwiftError_SCS_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X18, 0 };
static const uint32_t CSR_AArch64_AAPCS_SwiftError_SCS_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00ffb800, 0x001fdc00, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013fe600, 0x001f9800, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AAPCS_ThisReturn_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X0, 0 };
static const uint32_t CSR_AArch64_AAPCS_ThisReturn_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x020001fe, 0x01fff000, 0x001ff800, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013ffc00, 0x001ff000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AllRegs_SaveList[] = { AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WSP, AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::SP, AArch64::B0, AArch64::B1, AArch64::B2, AArch64::B3, AArch64::B4, AArch64::B5, AArch64::B6, AArch64::B7, AArch64::B8, AArch64::B9, AArch64::B10, AArch64::B11, AArch64::B12, AArch64::B13, AArch64::B14, AArch64::B15, AArch64::B16, AArch64::B17, AArch64::B18, AArch64::B19, AArch64::B20, AArch64::B21, AArch64::B22, AArch64::B23, AArch64::B24, AArch64::B25, AArch64::B26, AArch64::B27, AArch64::B28, AArch64::B29, AArch64::B30, AArch64::B31, AArch64::H0, AArch64::H1, AArch64::H2, AArch64::H3, AArch64::H4, AArch64::H5, AArch64::H6, AArch64::H7, AArch64::H8, AArch64::H9, AArch64::H10, AArch64::H11, AArch64::H12, AArch64::H13, AArch64::H14, AArch64::H15, AArch64::H16, AArch64::H17, AArch64::H18, AArch64::H19, AArch64::H20, AArch64::H21, AArch64::H22, AArch64::H23, AArch64::H24, AArch64::H25, AArch64::H26, AArch64::H27, AArch64::H28, AArch64::H29, AArch64::H30, AArch64::H31, AArch64::S0, AArch64::S1, AArch64::S2, AArch64::S3, AArch64::S4, AArch64::S5, AArch64::S6, AArch64::S7, AArch64::S8, AArch64::S9, AArch64::S10, AArch64::S11, AArch64::S12, AArch64::S13, AArch64::S14, AArch64::S15, AArch64::S16, AArch64::S17, AArch64::S18, AArch64::S19, AArch64::S20, AArch64::S21, AArch64::S22, AArch64::S23, AArch64::S24, AArch64::S25, AArch64::S26, AArch64::S27, AArch64::S28, AArch64::S29, AArch64::S30, AArch64::S31, AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 0 };
static const uint32_t CSR_AArch64_AllRegs_RegMask[] = { 0xfffffe6c, 0xffffffff, 0xffffffff, 0xfe0001ff, 0xffffffff, 0xffffffff, 0xffffffff, 0x001fffff, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff9fffff, 0xff3fffff, 0x001fffff, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_AllRegs_SCS_SaveList[] = { AArch64::W0, AArch64::W1, AArch64::W2, AArch64::W3, AArch64::W4, AArch64::W5, AArch64::W6, AArch64::W7, AArch64::W8, AArch64::W9, AArch64::W10, AArch64::W11, AArch64::W12, AArch64::W13, AArch64::W14, AArch64::W15, AArch64::W16, AArch64::W17, AArch64::W18, AArch64::W19, AArch64::W20, AArch64::W21, AArch64::W22, AArch64::W23, AArch64::W24, AArch64::W25, AArch64::W26, AArch64::W27, AArch64::W28, AArch64::W29, AArch64::W30, AArch64::WSP, AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::LR, AArch64::SP, AArch64::B0, AArch64::B1, AArch64::B2, AArch64::B3, AArch64::B4, AArch64::B5, AArch64::B6, AArch64::B7, AArch64::B8, AArch64::B9, AArch64::B10, AArch64::B11, AArch64::B12, AArch64::B13, AArch64::B14, AArch64::B15, AArch64::B16, AArch64::B17, AArch64::B18, AArch64::B19, AArch64::B20, AArch64::B21, AArch64::B22, AArch64::B23, AArch64::B24, AArch64::B25, AArch64::B26, AArch64::B27, AArch64::B28, AArch64::B29, AArch64::B30, AArch64::B31, AArch64::H0, AArch64::H1, AArch64::H2, AArch64::H3, AArch64::H4, AArch64::H5, AArch64::H6, AArch64::H7, AArch64::H8, AArch64::H9, AArch64::H10, AArch64::H11, AArch64::H12, AArch64::H13, AArch64::H14, AArch64::H15, AArch64::H16, AArch64::H17, AArch64::H18, AArch64::H19, AArch64::H20, AArch64::H21, AArch64::H22, AArch64::H23, AArch64::H24, AArch64::H25, AArch64::H26, AArch64::H27, AArch64::H28, AArch64::H29, AArch64::H30, AArch64::H31, AArch64::S0, AArch64::S1, AArch64::S2, AArch64::S3, AArch64::S4, AArch64::S5, AArch64::S6, AArch64::S7, AArch64::S8, AArch64::S9, AArch64::S10, AArch64::S11, AArch64::S12, AArch64::S13, AArch64::S14, AArch64::S15, AArch64::S16, AArch64::S17, AArch64::S18, AArch64::S19, AArch64::S20, AArch64::S21, AArch64::S22, AArch64::S23, AArch64::S24, AArch64::S25, AArch64::S26, AArch64::S27, AArch64::S28, AArch64::S29, AArch64::S30, AArch64::S31, AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 0 };
static const uint32_t CSR_AArch64_AllRegs_SCS_RegMask[] = { 0xfffffe6c, 0xffffffff, 0xffffffff, 0xfe0001ff, 0xffffffff, 0xffffffff, 0xffffffff, 0x001fffff, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff9fffff, 0xff3fffff, 0x001fffff, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_CXX_TLS_Darwin_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, 0 };
static const uint32_t CSR_AArch64_CXX_TLS_Darwin_RegMask[] = { 0xfffffe0c, 0xffffffff, 0xffffffff, 0x000001ff, 0xfe000000, 0xfdffffff, 0xfefff0ff, 0x001ff87f, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0x001fffff, 0x00000000, 0x00000000, 0xff000000, 0xfd3ffc1f, 0x001ff07f, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_CXX_TLS_Darwin_PE_SaveList[] = { AArch64::LR, AArch64::FP, 0 };
static const uint32_t CSR_AArch64_CXX_TLS_Darwin_PE_RegMask[] = { 0x0000000c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00c00000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00300000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_CXX_TLS_Darwin_SCS_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, AArch64::X18, 0 };
static const uint32_t CSR_AArch64_CXX_TLS_Darwin_SCS_RegMask[] = { 0xfffffe0c, 0xffffffff, 0xffffffff, 0x000001ff, 0xfe000000, 0xfdffffff, 0xfefff8ff, 0x001ffc7f, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0x001fffff, 0x00000000, 0x00000000, 0xff000000, 0xfd3ffe1f, 0x001ff87f, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_CXX_TLS_Darwin_ViaCopy_SaveList[] = { AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::D0, AArch64::D1, AArch64::D2, AArch64::D3, AArch64::D4, AArch64::D5, AArch64::D6, AArch64::D7, AArch64::D16, AArch64::D17, AArch64::D18, AArch64::D19, AArch64::D20, AArch64::D21, AArch64::D22, AArch64::D23, AArch64::D24, AArch64::D25, AArch64::D26, AArch64::D27, AArch64::D28, AArch64::D29, AArch64::D30, AArch64::D31, 0 };
static const uint32_t CSR_AArch64_CXX_TLS_Darwin_ViaCopy_RegMask[] = { 0xfffffe00, 0xffffffff, 0xffffffff, 0x000001ff, 0xfe000000, 0xfdffffff, 0xfe3ff0ff, 0x001ff87f, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0x001fffff, 0x00000000, 0x00000000, 0xff000000, 0xfc07fc1f, 0x001ff07f, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_NoRegs_SaveList[] = { 0 };
static const uint32_t CSR_AArch64_NoRegs_RegMask[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_NoRegs_SCS_SaveList[] = { AArch64::X18, 0 };
static const uint32_t CSR_AArch64_NoRegs_SCS_RegMask[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000800, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_RT_MostRegs_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, 0 };
static const uint32_t CSR_AArch64_RT_MostRegs_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00fff1fc, 0x001ff8fe, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013ffc3f, 0x001ff0fc, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_RT_MostRegs_SCS_SaveList[] = { AArch64::LR, AArch64::FP, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::D8, AArch64::D9, AArch64::D10, AArch64::D11, AArch64::D12, AArch64::D13, AArch64::D14, AArch64::D15, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X18, 0 };
static const uint32_t CSR_AArch64_RT_MostRegs_SCS_RegMask[] = { 0x01fe000c, 0x01fe0000, 0x01fe0000, 0x00000000, 0x00000000, 0x000001fe, 0x00fff9fc, 0x001ffcfe, 0x00000000, 0xe0000000, 0xe000000f, 0xe0000003, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x013ffe3f, 0x001ff8fc, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_StackProbe_Windows_SaveList[] = { AArch64::X0, AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::SP, AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 0 };
static const uint32_t CSR_AArch64_StackProbe_Windows_RegMask[] = { 0xfffffe64, 0xffffffff, 0xffffffff, 0xfe0001ff, 0xffffffff, 0xffffffff, 0xff7ff9ff, 0x001ffcff, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff9fffff, 0xff0ffe3f, 0x001ff8ff, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_TLS_Darwin_SaveList[] = { AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 0 };
static const uint32_t CSR_AArch64_TLS_Darwin_RegMask[] = { 0xfffffe04, 0xffffffff, 0xffffffff, 0xfe0001ff, 0xffffffff, 0xfdffffff, 0xfe7ff9ff, 0x001ffcff, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff1fffff, 0xfd0ffe3f, 0x001ff8ff, 0x00000000, 0x00000000, 0x00000000, };
static const MCPhysReg CSR_AArch64_TLS_ELF_SaveList[] = { AArch64::X1, AArch64::X2, AArch64::X3, AArch64::X4, AArch64::X5, AArch64::X6, AArch64::X7, AArch64::X8, AArch64::X9, AArch64::X10, AArch64::X11, AArch64::X12, AArch64::X13, AArch64::X14, AArch64::X15, AArch64::X16, AArch64::X17, AArch64::X18, AArch64::X19, AArch64::X20, AArch64::X21, AArch64::X22, AArch64::X23, AArch64::X24, AArch64::X25, AArch64::X26, AArch64::X27, AArch64::X28, AArch64::FP, AArch64::Q0, AArch64::Q1, AArch64::Q2, AArch64::Q3, AArch64::Q4, AArch64::Q5, AArch64::Q6, AArch64::Q7, AArch64::Q8, AArch64::Q9, AArch64::Q10, AArch64::Q11, AArch64::Q12, AArch64::Q13, AArch64::Q14, AArch64::Q15, AArch64::Q16, AArch64::Q17, AArch64::Q18, AArch64::Q19, AArch64::Q20, AArch64::Q21, AArch64::Q22, AArch64::Q23, AArch64::Q24, AArch64::Q25, AArch64::Q26, AArch64::Q27, AArch64::Q28, AArch64::Q29, AArch64::Q30, AArch64::Q31, 0 };
static const uint32_t CSR_AArch64_TLS_ELF_RegMask[] = { 0xfffffe04, 0xffffffff, 0xffffffff, 0xfe0001ff, 0xffffffff, 0xfdffffff, 0xfe7fffff, 0x001fffff, 0x00000000, 0xffe00000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff1fffff, 0xfd0fffff, 0x001fffff, 0x00000000, 0x00000000, 0x00000000, };


ArrayRef<const uint32_t *> AArch64GenRegisterInfo::getRegMasks() const {
  static const uint32_t *const Masks[] = {
    CSR_AArch64_AAPCS_RegMask,
    CSR_AArch64_AAPCS_SCS_RegMask,
    CSR_AArch64_AAPCS_SwiftError_RegMask,
    CSR_AArch64_AAPCS_SwiftError_SCS_RegMask,
    CSR_AArch64_AAPCS_ThisReturn_RegMask,
    CSR_AArch64_AllRegs_RegMask,
    CSR_AArch64_AllRegs_SCS_RegMask,
    CSR_AArch64_CXX_TLS_Darwin_RegMask,
    CSR_AArch64_CXX_TLS_Darwin_PE_RegMask,
    CSR_AArch64_CXX_TLS_Darwin_SCS_RegMask,
    CSR_AArch64_CXX_TLS_Darwin_ViaCopy_RegMask,
    CSR_AArch64_NoRegs_RegMask,
    CSR_AArch64_NoRegs_SCS_RegMask,
    CSR_AArch64_RT_MostRegs_RegMask,
    CSR_AArch64_RT_MostRegs_SCS_RegMask,
    CSR_AArch64_StackProbe_Windows_RegMask,
    CSR_AArch64_TLS_Darwin_RegMask,
    CSR_AArch64_TLS_ELF_RegMask,
  };
  return makeArrayRef(Masks);
}

ArrayRef<const char *> AArch64GenRegisterInfo::getRegMaskNames() const {
  static const char *const Names[] = {
    "CSR_AArch64_AAPCS",
    "CSR_AArch64_AAPCS_SCS",
    "CSR_AArch64_AAPCS_SwiftError",
    "CSR_AArch64_AAPCS_SwiftError_SCS",
    "CSR_AArch64_AAPCS_ThisReturn",
    "CSR_AArch64_AllRegs",
    "CSR_AArch64_AllRegs_SCS",
    "CSR_AArch64_CXX_TLS_Darwin",
    "CSR_AArch64_CXX_TLS_Darwin_PE",
    "CSR_AArch64_CXX_TLS_Darwin_SCS",
    "CSR_AArch64_CXX_TLS_Darwin_ViaCopy",
    "CSR_AArch64_NoRegs",
    "CSR_AArch64_NoRegs_SCS",
    "CSR_AArch64_RT_MostRegs",
    "CSR_AArch64_RT_MostRegs_SCS",
    "CSR_AArch64_StackProbe_Windows",
    "CSR_AArch64_TLS_Darwin",
    "CSR_AArch64_TLS_ELF",
  };
  return makeArrayRef(Names);
}

const AArch64FrameLowering *
AArch64GenRegisterInfo::getFrameLowering(const MachineFunction &MF) {
  return static_cast<const AArch64FrameLowering *>(
      MF.getSubtarget().getFrameLowering());
}

} // end namespace llvm

#endif // GET_REGINFO_TARGET_DESC

