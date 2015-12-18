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
/// \brief Declares the registers and their encodings for ARM32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGISTERSARM32_H
#define SUBZERO_SRC_ICEREGISTERSARM32_H

#include "IceDefs.h"
#include "IceInstARM32.def"
#include "IceOperand.h" // RC_Target
#include "IceTypes.h"

namespace Ice {

class RegARM32 {
public:
  /// An enum of every register. The enum value may not match the encoding used
  /// to binary encode register operands in instructions.
  enum AllRegisters {
#define X(val, encode, name, cc_arg, scratch, preserved, stackptr, frameptr,   \
          isInt, isI64Pair, isFP32, isFP64, isVec128, alias_init)              \
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
#define X(val, encode, name, cc_arg, scratch, preserved, stackptr, frameptr,   \
          isInt, isI64Pair, isFP32, isFP64, isVec128, alias_init)              \
  Encoded_##val = encode,
    REGARM32_GPR_TABLE
#undef X
        Encoded_Not_GPR = -1
  };

  /// An enum of FP32 S-Registers. The enum value does match the encoding used
  /// to binary encode register operands in instructions.
  enum SRegister {
#define X(val, encode, name, cc_arg, scratch, preserved, stackptr, frameptr,   \
          isInt, isI64Pair, isFP32, isFP64, isVec128, alias_init)              \
  Encoded_##val = encode,
    REGARM32_FP32_TABLE
#undef X
        Encoded_Not_SReg = -1
  };

  /// An enum of FP64 D-Registers. The enum value does match the encoding used
  /// to binary encode register operands in instructions.
  enum DRegister {
#define X(val, encode, name, cc_arg, scratch, preserved, stackptr, frameptr,   \
          isInt, isI64Pair, isFP32, isFP64, isVec128, alias_init)              \
  Encoded_##val = encode,
    REGARM32_FP64_TABLE
#undef X
        Encoded_Not_DReg = -1
  };

  /// An enum of 128-bit Q-Registers. The enum value does match the encoding
  /// used to binary encode register operands in instructions.
  enum QRegister {
#define X(val, encode, name, cc_arg, scratch, preserved, stackptr, frameptr,   \
          isInt, isI64Pair, isFP32, isFP64, isVec128, alias_init)              \
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

  static inline GPRRegister getI64PairFirstGPRNum(int32_t RegNum) {
    assert(Reg_I64PAIR_First <= RegNum);
    assert(RegNum <= Reg_I64PAIR_Last);
    return GPRRegister(2 * (RegNum - Reg_I64PAIR_First + Reg_GPR_First));
  }

  static inline GPRRegister getI64PairSecondGPRNum(int32_t RegNum) {
    assert(Reg_I64PAIR_First <= RegNum);
    assert(RegNum <= Reg_I64PAIR_Last);
    return GPRRegister(2 * (RegNum - Reg_I64PAIR_First + Reg_GPR_First) + 1);
  }

  static inline bool isI64RegisterPair(int32_t RegNum) {
    return Reg_I64PAIR_First <= RegNum && RegNum <= Reg_I64PAIR_Last;
  }

  static inline bool isEncodedSReg(int32_t RegNum) {
    return Reg_SREG_First <= RegNum && RegNum <= Reg_SREG_Last;
  }

  static inline SizeT getNumSRegs() {
    return Reg_SREG_Last + 1 - Reg_SREG_First;
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

  static const char *RegNames[];
};

// Extend enum RegClass with ARM32-specific register classes (if any).
enum RegClassARM32 : uint8_t { RCARM32_NUM = RC_Target };

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGISTERSARM32_H
