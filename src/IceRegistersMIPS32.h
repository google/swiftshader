//===- subzero/src/IceRegistersMIPS32.h - Register information --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the registers and their encodings for MIPS32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGISTERSMIPS32_H
#define SUBZERO_SRC_ICEREGISTERSMIPS32_H

#include "IceDefs.h"
#include "IceInstMIPS32.def"
#include "IceOperand.h" // RC_Target
#include "IceTypes.h"

namespace Ice {

namespace RegMIPS32 {

/// An enum of every register. The enum value may not match the encoding
/// used to binary encode register operands in instructions.
enum AllRegisters {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
  val,
  REGMIPS32_TABLE
#undef X
      Reg_NUM,
#define X(val, init) val init,
  REGMIPS32_TABLE_BOUNDS
#undef X
};

/// An enum of GPR Registers. The enum value does match the encoding used
/// to binary encode register operands in instructions.
enum GPRRegister {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
                                                                               \
  Encoded_##val encode,
  REGMIPS32_GPR_TABLE
#undef X
      Encoded_Not_GPR = -1
};

// TODO(jvoung): Floating point and vector registers...
// Need to model overlap and difference in encoding too.

static inline GPRRegister getEncodedGPR(int32_t RegNum) {
  assert(Reg_GPR_First <= RegNum && RegNum <= Reg_GPR_Last);
  return GPRRegister(RegNum - Reg_GPR_First);
}

} // end of namespace RegMIPS32

// Extend enum RegClass with MIPS32-specific register classes (if any).
enum RegClassMIPS32 : uint8_t { RCMIPS32_NUM = RC_Target };

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGISTERSMIPS32_H
