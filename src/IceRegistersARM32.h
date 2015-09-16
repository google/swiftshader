//===- subzero/src/IceRegistersARM32.h - Register information ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the registers and their encodings for ARM32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGISTERSARM32_H
#define SUBZERO_SRC_ICEREGISTERSARM32_H

#include "IceDefs.h"
#include "IceInstARM32.def"
#include "IceTypes.h"

namespace Ice {

class RegARM32 {
public:
  /// An enum of every register. The enum value may not match the encoding used
  /// to binary encode register operands in instructions.
  enum AllRegisters {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  val,
    REGARM32_TABLE
#undef X
        Reg_NUM,
#define X(val, init) val init,
    REGARM32_TABLE_BOUNDS
#undef X
  };

  /// An enum of GPR Registers. The enum value does match the encoding used to
  /// binary encode register operands in instructions.
  enum GPRRegister {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  Encoded_##val = encode,
    REGARM32_GPR_TABLE
#undef X
        Encoded_Not_GPR = -1
  };

  /// An enum of FP32 S-Registers. The enum value does match the encoding used
  /// to binary encode register operands in instructions.
  enum SRegister {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  Encoded_##val = encode,
    REGARM32_FP32_TABLE
#undef X
        Encoded_Not_SReg = -1
  };

  /// An enum of FP64 D-Registers. The enum value does match the encoding used
  /// to binary encode register operands in instructions.
  enum DRegister {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  Encoded_##val = encode,
    REGARM32_FP64_TABLE
#undef X
        Encoded_Not_DReg = -1
  };

  /// An enum of 128-bit Q-Registers. The enum value does match the encoding
  /// used to binary encode register operands in instructions.
  enum QRegister {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  Encoded_##val = encode,
    REGARM32_VEC128_TABLE
#undef X
        Encoded_Not_QReg = -1
  };

  static inline GPRRegister getEncodedGPR(int32_t RegNum) {
    assert(Reg_GPR_First <= RegNum);
    assert(RegNum <= Reg_GPR_Last);
    return GPRRegister(RegNum - Reg_GPR_First);
  }

  static inline SRegister getEncodedSReg(int32_t RegNum) {
    assert(Reg_SREG_First <= RegNum);
    assert(RegNum <= Reg_SREG_Last);
    return SRegister(RegNum - Reg_SREG_First);
  }

  static inline DRegister getEncodedDReg(int32_t RegNum) {
    assert(Reg_DREG_First <= RegNum);
    assert(RegNum <= Reg_DREG_Last);
    return DRegister(RegNum - Reg_DREG_First);
  }

  static inline QRegister getEncodedQReg(int32_t RegNum) {
    assert(Reg_QREG_First <= RegNum);
    assert(RegNum <= Reg_QREG_Last);
    return QRegister(RegNum - Reg_QREG_First);
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGISTERSARM32_H
