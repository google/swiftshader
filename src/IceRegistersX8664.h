//===- subzero/src/IceRegistersX8664.h - Register information ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the registers and their encodings for x86-64.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGISTERSX8664_H
#define SUBZERO_SRC_ICEREGISTERSX8664_H

#include "IceDefs.h"
#include "IceInstX8664.def"
#include "IceTypes.h"

namespace Ice {

class RegX8664 {
public:
  // An enum of every register. The enum value may not match the encoding
  // used to binary encode register operands in instructions.
  enum AllRegisters {
#define X(val, encode, name64, name, name16, name8, scratch, preserved,        \
          stackptr, frameptr, isInt, isFP)                                     \
  val,
    REGX8664_TABLE
#undef X
        Reg_NUM,
#define X(val, init) val init,
    REGX8664_TABLE_BOUNDS
#undef X
  };

  // An enum of GPR Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum GPRRegister {
#define X(val, encode, name64, name, name16, name8, scratch, preserved,        \
          stackptr, frameptr, isInt, isFP)                                     \
  Encoded_##val encode,
    REGX8664_GPR_TABLE
#undef X
        Encoded_Not_GPR = -1
  };

  // An enum of XMM Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum XmmRegister {
#define X(val, encode, name64, name, name16, name8, scratch, preserved,        \
          stackptr, frameptr, isInt, isFP)                                     \
  Encoded_##val encode,
    REGX8664_XMM_TABLE
#undef X
        Encoded_Not_Xmm = -1
  };

  // An enum of Byte Registers. The enum value does match the encoding used
  // to binary encode register operands in instructions.
  enum ByteRegister {
#define X(val, encode) Encoded_##val encode,
    REGX8664_BYTEREG_TABLE
#undef X
        Encoded_Not_ByteReg = -1
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
    // In x86-64, AH is not encodable when the REX prefix is used; the same
    // encoding is used for spl. Therefore, ah needs special handling.
    if (RegNum == Reg_ah)
      return Encoded_Reg_spl;
    return ByteRegister(RegNum - Reg_GPR_First);
  }

  static inline GPRRegister getEncodedByteRegOrGPR(Type Ty, int32_t RegNum) {
    if (isByteSizedType(Ty))
      return GPRRegister(getEncodedByteReg(RegNum));
    else
      return getEncodedGPR(RegNum);
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGISTERSX8664_H
