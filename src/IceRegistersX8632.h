//===- subzero/src/IceRegistersX8632.h - Register information ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the registers and their encodings for x86-32.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGISTERSX8632_H
#define SUBZERO_SRC_ICEREGISTERSX8632_H

#include "IceDefs.h"
#include "IceInstX8632.def"
#include "IceTypes.h"

namespace Ice {

class RegX8632 {
public:
  // An enum of every register. The enum value may not match the encoding
  // used to binary encode register operands in instructions.
  enum AllRegisters {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  val,
    REGX8632_TABLE
#undef X
        Reg_NUM,
#define X(val, init) val init,
    REGX8632_TABLE_BOUNDS
#undef X
  };

  // An enum of GPR Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum GPRRegister {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  Encoded_##val encode,
    REGX8632_GPR_TABLE
#undef X
        Encoded_Not_GPR = -1
  };

  // An enum of XMM Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum XmmRegister {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  Encoded_##val encode,
    REGX8632_XMM_TABLE
#undef X
        Encoded_Not_Xmm = -1
  };

  // An enum of Byte Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum ByteRegister {
#define X(val, encode) Encoded_##val encode,
    REGX8632_BYTEREG_TABLE
#undef X
        Encoded_Not_ByteReg = -1
  };

  // An enum of X87 Stack Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum X87STRegister {
#define X(val, encode, name) Encoded_##val encode,
    X87ST_REGX8632_TABLE
#undef X
        Encoded_Not_X87STReg = -1
  };

  static inline GPRRegister getEncodedGPR(int32_t RegNum) {
    assert(Reg_GPR_First <= RegNum && RegNum <= Reg_GPR_Last);
    return GPRRegister(RegNum - Reg_GPR_First);
  }

  static inline XmmRegister getEncodedXmm(int32_t RegNum) {
    assert(Reg_XMM_First <= RegNum && RegNum <= Reg_XMM_Last);
    return XmmRegister(RegNum - Reg_XMM_First);
  }

  static inline ByteRegister getEncodedByteReg(int32_t RegNum) {
    assert(RegNum == Reg_ah || (Reg_GPR_First <= RegNum && RegNum <= Reg_ebx));
    if (RegNum == Reg_ah)
      return Encoded_Reg_ah;
    return ByteRegister(RegNum - Reg_GPR_First);
  }

  static inline GPRRegister getEncodedByteRegOrGPR(Type Ty, int32_t RegNum) {
    if (isByteSizedType(Ty))
      return GPRRegister(getEncodedByteReg(RegNum));
    else
      return getEncodedGPR(RegNum);
  }

  static inline X87STRegister getEncodedSTReg(int32_t RegNum) {
    assert(Encoded_X87ST_First <= RegNum && RegNum <= Encoded_X87ST_Last);
    return X87STRegister(RegNum);
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGISTERSX8632_H
