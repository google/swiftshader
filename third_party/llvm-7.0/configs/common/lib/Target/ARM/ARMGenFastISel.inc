/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* "Fast" Instruction Selector for the ARM target                             *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


// FastEmit Immediate Predicate functions.
static bool Predicate_mod_imm(int64_t Imm) {

    return ARM_AM::getSOImmVal(Imm) != -1;
  
}
static bool Predicate_imm0_65535(int64_t Imm) {

  return Imm >= 0 && Imm < 65536;

}
static bool Predicate_imm0_7(int64_t Imm) {

  return Imm >= 0 && Imm < 8;

}
static bool Predicate_imm8_255(int64_t Imm) {

  return Imm >= 8 && Imm < 256;

}
static bool Predicate_imm0_255(int64_t Imm) {
 return Imm >= 0 && Imm < 256; 
}
static bool Predicate_t2_so_imm(int64_t Imm) {

    return ARM_AM::getT2SOImmVal(Imm) != -1;
  
}
static bool Predicate_imm0_4095(int64_t Imm) {

  return Imm >= 0 && Imm < 4096;

}
static bool Predicate_imm1_31(int64_t Imm) {
 return Imm > 0 && Imm < 32; 
}
static bool Predicate_imm0_31(int64_t Imm) {

  return Imm >= 0 && Imm < 32;

}
static bool Predicate_shr_imm8(int64_t Imm) {
 return Imm > 0 && Imm <= 8; 
}
static bool Predicate_shr_imm16(int64_t Imm) {
 return Imm > 0 && Imm <= 16; 
}
static bool Predicate_shr_imm32(int64_t Imm) {
 return Imm > 0 && Imm <= 32; 
}
static bool Predicate_VectorIndex32(int64_t Imm) {

  return ((uint64_t)Imm) < 2;

}
static bool Predicate_t2_so_imm_neg(int64_t Imm) {

  return Imm && ARM_AM::getT2SOImmVal(-(uint32_t)Imm) != -1;

}


// FastEmit functions for ARMISD::CALL.

unsigned fastEmit_ARMISD_CALL_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->hasV5TOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::BLX, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CALL_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CALL_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CALL_NOLINK.

unsigned fastEmit_ARMISD_CALL_NOLINK_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_r(ARM::tBX_CALL, &ARM::tGPRRegClass, Op0, Op0IsKill);
  }
  if ((!Subtarget->isThumb()) && (!Subtarget->hasV4TOps())) {
    return fastEmitInst_r(ARM::BMOVPCRX_CALL, &ARM::tGPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV4TOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::BX_CALL, &ARM::tGPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CALL_NOLINK_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CALL_NOLINK_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CALL_PRED.

unsigned fastEmit_ARMISD_CALL_PRED_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->hasV5TOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::BLX_pred, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CALL_PRED_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CALL_PRED_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::RRX.

unsigned fastEmit_ARMISD_RRX_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2RRX, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::RRX, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_RRX_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_RRX_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::SRA_FLAG.

unsigned fastEmit_ARMISD_SRA_FLAG_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2MOVsra_flag, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::MOVsra_flag, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_SRA_FLAG_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_SRA_FLAG_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::SRL_FLAG.

unsigned fastEmit_ARMISD_SRL_FLAG_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2MOVsrl_flag, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::MOVsrl_flag, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_SRL_FLAG_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_SRL_FLAG_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::TC_RETURN.

unsigned fastEmit_ARMISD_TC_RETURN_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  return fastEmitInst_r(ARM::TCRETURNri, &ARM::tcGPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ARMISD_TC_RETURN_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_TC_RETURN_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCEQZ.

unsigned fastEmit_ARMISD_VCEQZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv4f16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv8f16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv2f32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCEQzv4f32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCEQZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCEQZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCEQZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCEQZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCEQZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCEQZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCEQZ_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCEQZ_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCEQZ_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCEQZ_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGEZ.

unsigned fastEmit_ARMISD_VCGEZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv4f16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv8f16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv2f32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGEzv4f32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGEZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGEZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGEZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGEZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGEZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGEZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCGEZ_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCGEZ_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCGEZ_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCGEZ_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGTZ.

unsigned fastEmit_ARMISD_VCGTZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv4f16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv8f16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv2f32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCGTzv4f32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGTZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGTZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGTZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGTZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGTZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGTZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCGTZ_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCGTZ_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCGTZ_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCGTZ_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCLEZ.

unsigned fastEmit_ARMISD_VCLEZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv4f16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv8f16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv2f32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLEzv4f32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLEZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCLEZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCLEZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCLEZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCLEZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCLEZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCLEZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCLEZ_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCLEZ_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCLEZ_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCLEZ_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCLTZ.

unsigned fastEmit_ARMISD_VCLTZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv4f16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv8f16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv2f32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLTzv4f32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCLTZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCLTZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCLTZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCLTZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCLTZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCLTZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCLTZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCLTZ_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCLTZ_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCLTZ_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCLTZ_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VDUP.

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP8d, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP8q, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP16d, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP16q, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v2i32_r(unsigned Op0, bool Op0IsKill) {
  if ((!Subtarget->hasSlowVDUP32()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP32d, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_MVT_v4i32_r(unsigned Op0, bool Op0IsKill) {
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VDUP32q, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUP_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v2i32_r(Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VDUP_MVT_i32_MVT_v4i32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ARMISD_VDUP_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_VDUP_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMOVSR.

unsigned fastEmit_ARMISD_VMOVSR_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasVFP2()) && (Subtarget->preferVMOVSR() ||!Subtarget->useNEONForSinglePrecisionFP())) {
    return fastEmitInst_r(ARM::VMOVSR, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMOVSR_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_VMOVSR_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMOVhr.

unsigned fastEmit_ARMISD_VMOVhr_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_r(ARM::VMOVHR, &ARM::HPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMOVhr_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_VMOVhr_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMOVrh.

unsigned fastEmit_ARMISD_VMOVrh_MVT_f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_r(ARM::VMOVRH, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMOVrh_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ARMISD_VMOVrh_MVT_f16_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VREV16.

unsigned fastEmit_ARMISD_VREV16_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV16d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV16_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV16q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV16_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VREV16_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VREV16_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VREV32.

unsigned fastEmit_ARMISD_VREV32_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV32d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV32_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV32q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV32_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV32d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV32_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV32_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VREV32_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VREV32_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VREV32_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VREV32_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VREV64.

unsigned fastEmit_ARMISD_VREV64_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VREV64_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ARMISD_VREV64_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ARMISD_VREV64_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VREV64_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VREV64_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VREV64_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VREV64_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VREV64_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VREV64_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VREV64_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VREV64_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::WIN__DBZCHK.

unsigned fastEmit_ARMISD_WIN__DBZCHK_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  return fastEmitInst_r(ARM::WIN__DBZCHK, &ARM::tGPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ARMISD_WIN__DBZCHK_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_WIN__DBZCHK_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::ABS.

unsigned fastEmit_ISD_ABS_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ABS_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_ABS_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ISD_ABS_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_ABS_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_ABS_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_ABS_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_ABS_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::ANY_EXTEND.

unsigned fastEmit_ISD_ANY_EXTEND_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  return fastEmitInst_r(ARM::VMOVLuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ISD_ANY_EXTEND_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  return fastEmitInst_r(ARM::VMOVLuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ISD_ANY_EXTEND_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  return fastEmitInst_r(ARM::VMOVLuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill);
}

unsigned fastEmit_ISD_ANY_EXTEND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_ANY_EXTEND_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_ANY_EXTEND_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_ANY_EXTEND_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::BITCAST.

unsigned fastEmit_ISD_BITCAST_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasVFP2()) && (Subtarget->preferVMOVSR() ||!Subtarget->useNEONForSinglePrecisionFP())) {
    return fastEmitInst_r(ARM::VMOVSR, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VMOVRS, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f64_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f64_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f64_MVT_v2i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f64_MVT_v2f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_f64_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_f64_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_BITCAST_MVT_f64_MVT_v2i32_r(Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_BITCAST_MVT_f64_MVT_v2f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_MVT_f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV16d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v2i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v1i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v2f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::f64: return fastEmit_ISD_BITCAST_MVT_v8i8_MVT_f64_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v2i32_r(Op0, Op0IsKill);
  case MVT::v1i64: return fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v1i64_r(Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_BITCAST_MVT_v8i8_MVT_v2f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV16q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v4i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v2i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v4f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v2f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v4i32_r(Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v2i64_r(Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v4f32_r(Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v16i8_MVT_v2f64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_MVT_f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV16d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v2i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v1i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v2f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::f64: return fastEmit_ISD_BITCAST_MVT_v4i16_MVT_f64_r(Op0, Op0IsKill);
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v2i32_r(Op0, Op0IsKill);
  case MVT::v1i64: return fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v1i64_r(Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_BITCAST_MVT_v4i16_MVT_v2f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV16q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v4i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v2i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v4f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v2f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v4i32_r(Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v2i64_r(Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v4f32_r(Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v8i16_MVT_v2f64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i32_MVT_f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v1i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::f64: return fastEmit_ISD_BITCAST_MVT_v2i32_MVT_f64_r(Op0, Op0IsKill);
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v1i64: return fastEmit_ISD_BITCAST_MVT_v2i32_MVT_v1i64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v2i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v2f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v2i64_r(Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v4i32_MVT_v2f64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v2i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v2f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v1i64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v2i32_r(Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_BITCAST_MVT_v1i64_MVT_v2f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v4i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v4f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2i64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v4i32_r(Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v2i64_MVT_v4f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8f16_MVT_v4f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8f16_MVT_v2f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v8f16_MVT_v4f32_r(Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v8f16_MVT_v2f64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f32_MVT_f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v8i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v4i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32d16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v1i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64d32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::f64: return fastEmit_ISD_BITCAST_MVT_v2f32_MVT_f64_r(Op0, Op0IsKill);
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v8i8_r(Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v4i16_r(Op0, Op0IsKill);
  case MVT::v1i64: return fastEmit_ISD_BITCAST_MVT_v2f32_MVT_v1i64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV32q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v2i64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v2f64_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v2i64_r(Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v4f32_MVT_v2f64_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v16i8_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v8i16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v4i32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v8f16_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v4f32_r(unsigned Op0, bool Op0IsKill) {
  if ((MF->getDataLayout().isBigEndian())) {
    return fastEmitInst_r(ARM::VREV64q32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITCAST_MVT_v2f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
switch (RetVT.SimpleTy) {
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v16i8_r(Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v8i16_r(Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v4i32_r(Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v8f16_r(Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v2f64_MVT_v4f32_r(Op0, Op0IsKill);
  default: return 0;
}
}

unsigned fastEmit_ISD_BITCAST_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_BITCAST_MVT_i32_r(RetVT, Op0, Op0IsKill);
  case MVT::f32: return fastEmit_ISD_BITCAST_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_BITCAST_MVT_f64_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i8: return fastEmit_ISD_BITCAST_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ISD_BITCAST_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_BITCAST_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_BITCAST_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_BITCAST_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_BITCAST_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v1i64: return fastEmit_ISD_BITCAST_MVT_v1i64_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_BITCAST_MVT_v2i64_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ISD_BITCAST_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_BITCAST_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_BITCAST_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_BITCAST_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f64: return fastEmit_ISD_BITCAST_MVT_v2f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::BITREVERSE.

unsigned fastEmit_ISD_BITREVERSE_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2RBIT, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV6T2Ops()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::RBIT, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BITREVERSE_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_BITREVERSE_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::BRIND.

unsigned fastEmit_ISD_BRIND_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::tBRIND, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  if ((!Subtarget->isThumb()) && (!Subtarget->hasV4TOps())) {
    return fastEmitInst_r(ARM::MOVPCRX, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV4TOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::BX, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BRIND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_BRIND_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::BSWAP.

unsigned fastEmit_ISD_BSWAP_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2REV, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV6Ops()) && (Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_r(ARM::tREV, &ARM::tGPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV6Ops()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::REV, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_BSWAP_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_BSWAP_MVT_i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::CTLZ.

unsigned fastEmit_ISD_CTLZ_MVT_i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_r(ARM::t2CLZ, &ARM::rGPRRegClass, Op0, Op0IsKill);
  }
  if ((Subtarget->hasV5TOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_r(ARM::CLZ, &ARM::GPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv16i8, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCLZv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTLZ_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_CTLZ_MVT_i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i8: return fastEmit_ISD_CTLZ_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ISD_CTLZ_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_CTLZ_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_CTLZ_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_CTLZ_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_CTLZ_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::CTPOP.

unsigned fastEmit_ISD_CTPOP_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCNTd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTPOP_MVT_v16i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCNTq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_CTPOP_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_CTPOP_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v16i8: return fastEmit_ISD_CTPOP_MVT_v16i8_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FABS.

unsigned fastEmit_ISD_FABS_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->useNEONForSinglePrecisionFP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VABSS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VABSD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABShd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABShq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSfd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VABSfq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FABS_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FABS_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FABS_MVT_f64_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ISD_FABS_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_FABS_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_FABS_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_FABS_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FCEIL.

unsigned fastEmit_ISD_FCEIL_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTPS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FCEIL_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTPD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FCEIL_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FCEIL_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FCEIL_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FFLOOR.

unsigned fastEmit_ISD_FFLOOR_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTMS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FFLOOR_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTMD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FFLOOR_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FFLOOR_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FFLOOR_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FNEARBYINT.

unsigned fastEmit_ISD_FNEARBYINT_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTRS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEARBYINT_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTRD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEARBYINT_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FNEARBYINT_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FNEARBYINT_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FNEG.

unsigned fastEmit_ISD_FNEG_MVT_f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_r(ARM::VNEGH, &ARM::HPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->useNEONForSinglePrecisionFP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VNEGS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VNEGD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VNEGhd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VNEGhq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VNEGfd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VNEGf32q, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FNEG_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FNEG_MVT_f16_r(RetVT, Op0, Op0IsKill);
  case MVT::f32: return fastEmit_ISD_FNEG_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FNEG_MVT_f64_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f16: return fastEmit_ISD_FNEG_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_FNEG_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_FNEG_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_FNEG_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FP_EXTEND.

unsigned fastEmit_ISD_FP_EXTEND_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VCVTDS, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_EXTEND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FP_EXTEND_MVT_f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FP_ROUND.

unsigned fastEmit_ISD_FP_ROUND_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VCVTSD, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_ROUND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f64: return fastEmit_ISD_FP_ROUND_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FP_TO_SINT.

unsigned fastEmit_ISD_FP_TO_SINT_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTh2sd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_SINT_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTh2sq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_SINT_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTf2sd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_SINT_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTf2sq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_SINT_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4f16: return fastEmit_ISD_FP_TO_SINT_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_FP_TO_SINT_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_FP_TO_SINT_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_FP_TO_SINT_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FP_TO_UINT.

unsigned fastEmit_ISD_FP_TO_UINT_MVT_v4f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTh2ud, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_UINT_MVT_v8f16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTh2uq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_UINT_MVT_v2f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTf2ud, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_UINT_MVT_v4f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTf2uq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FP_TO_UINT_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4f16: return fastEmit_ISD_FP_TO_UINT_MVT_v4f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8f16: return fastEmit_ISD_FP_TO_UINT_MVT_v8f16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2f32: return fastEmit_ISD_FP_TO_UINT_MVT_v2f32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4f32: return fastEmit_ISD_FP_TO_UINT_MVT_v4f32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FRINT.

unsigned fastEmit_ISD_FRINT_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTXS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FRINT_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTXD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FRINT_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FRINT_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FRINT_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FROUND.

unsigned fastEmit_ISD_FROUND_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTAS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FROUND_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTAD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FROUND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FROUND_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FROUND_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FSQRT.

unsigned fastEmit_ISD_FSQRT_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VSQRTS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSQRT_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_r(ARM::VSQRTD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSQRT_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FSQRT_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FSQRT_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FTRUNC.

unsigned fastEmit_ISD_FTRUNC_MVT_f32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTZS, &ARM::SPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FTRUNC_MVT_f64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_r(ARM::VRINTZD, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FTRUNC_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f32: return fastEmit_ISD_FTRUNC_MVT_f32_r(RetVT, Op0, Op0IsKill);
  case MVT::f64: return fastEmit_ISD_FTRUNC_MVT_f64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SIGN_EXTEND.

unsigned fastEmit_ISD_SIGN_EXTEND_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SIGN_EXTEND_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SIGN_EXTEND_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLsv2i64, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SIGN_EXTEND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_SIGN_EXTEND_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_SIGN_EXTEND_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_SIGN_EXTEND_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SINT_TO_FP.

unsigned fastEmit_ISD_SINT_TO_FP_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTs2hd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SINT_TO_FP_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTs2hq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SINT_TO_FP_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTs2fd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SINT_TO_FP_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTs2fq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SINT_TO_FP_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4i16: return fastEmit_ISD_SINT_TO_FP_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_SINT_TO_FP_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_SINT_TO_FP_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_SINT_TO_FP_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::TRUNCATE.

unsigned fastEmit_ISD_TRUNCATE_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVNv8i8, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_TRUNCATE_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVNv4i16, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_TRUNCATE_MVT_v2i64_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVNv2i32, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_TRUNCATE_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ISD_TRUNCATE_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_TRUNCATE_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i64: return fastEmit_ISD_TRUNCATE_MVT_v2i64_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::UINT_TO_FP.

unsigned fastEmit_ISD_UINT_TO_FP_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTu2hd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UINT_TO_FP_MVT_v8i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTu2hq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UINT_TO_FP_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTu2fd, &ARM::DPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UINT_TO_FP_MVT_v4i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VCVTu2fq, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UINT_TO_FP_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4i16: return fastEmit_ISD_UINT_TO_FP_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v8i16: return fastEmit_ISD_UINT_TO_FP_MVT_v8i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_UINT_TO_FP_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i32: return fastEmit_ISD_UINT_TO_FP_MVT_v4i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::ZERO_EXTEND.

unsigned fastEmit_ISD_ZERO_EXTEND_MVT_v8i8_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ZERO_EXTEND_MVT_v4i16_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ZERO_EXTEND_MVT_v2i32_r(MVT RetVT, unsigned Op0, bool Op0IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_r(ARM::VMOVLuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ZERO_EXTEND_r(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_ZERO_EXTEND_MVT_v8i8_r(RetVT, Op0, Op0IsKill);
  case MVT::v4i16: return fastEmit_ISD_ZERO_EXTEND_MVT_v4i16_r(RetVT, Op0, Op0IsKill);
  case MVT::v2i32: return fastEmit_ISD_ZERO_EXTEND_MVT_v2i32_r(RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_r(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill) override {
  switch (Opcode) {
  case ARMISD::CALL: return fastEmit_ARMISD_CALL_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::CALL_NOLINK: return fastEmit_ARMISD_CALL_NOLINK_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::CALL_PRED: return fastEmit_ARMISD_CALL_PRED_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::RRX: return fastEmit_ARMISD_RRX_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::SRA_FLAG: return fastEmit_ARMISD_SRA_FLAG_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::SRL_FLAG: return fastEmit_ARMISD_SRL_FLAG_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::TC_RETURN: return fastEmit_ARMISD_TC_RETURN_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VCEQZ: return fastEmit_ARMISD_VCEQZ_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VCGEZ: return fastEmit_ARMISD_VCGEZ_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VCGTZ: return fastEmit_ARMISD_VCGTZ_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VCLEZ: return fastEmit_ARMISD_VCLEZ_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VCLTZ: return fastEmit_ARMISD_VCLTZ_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VDUP: return fastEmit_ARMISD_VDUP_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VMOVSR: return fastEmit_ARMISD_VMOVSR_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VMOVhr: return fastEmit_ARMISD_VMOVhr_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VMOVrh: return fastEmit_ARMISD_VMOVrh_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VREV16: return fastEmit_ARMISD_VREV16_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VREV32: return fastEmit_ARMISD_VREV32_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::VREV64: return fastEmit_ARMISD_VREV64_r(VT, RetVT, Op0, Op0IsKill);
  case ARMISD::WIN__DBZCHK: return fastEmit_ARMISD_WIN__DBZCHK_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::ABS: return fastEmit_ISD_ABS_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::ANY_EXTEND: return fastEmit_ISD_ANY_EXTEND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::BITCAST: return fastEmit_ISD_BITCAST_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::BITREVERSE: return fastEmit_ISD_BITREVERSE_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::BRIND: return fastEmit_ISD_BRIND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::BSWAP: return fastEmit_ISD_BSWAP_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::CTLZ: return fastEmit_ISD_CTLZ_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::CTPOP: return fastEmit_ISD_CTPOP_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FABS: return fastEmit_ISD_FABS_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FCEIL: return fastEmit_ISD_FCEIL_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FFLOOR: return fastEmit_ISD_FFLOOR_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FNEARBYINT: return fastEmit_ISD_FNEARBYINT_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FNEG: return fastEmit_ISD_FNEG_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FP_EXTEND: return fastEmit_ISD_FP_EXTEND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FP_ROUND: return fastEmit_ISD_FP_ROUND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FP_TO_SINT: return fastEmit_ISD_FP_TO_SINT_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FP_TO_UINT: return fastEmit_ISD_FP_TO_UINT_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FRINT: return fastEmit_ISD_FRINT_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FROUND: return fastEmit_ISD_FROUND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FSQRT: return fastEmit_ISD_FSQRT_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::FTRUNC: return fastEmit_ISD_FTRUNC_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::SIGN_EXTEND: return fastEmit_ISD_SIGN_EXTEND_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::SINT_TO_FP: return fastEmit_ISD_SINT_TO_FP_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::TRUNCATE: return fastEmit_ISD_TRUNCATE_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::UINT_TO_FP: return fastEmit_ISD_UINT_TO_FP_r(VT, RetVT, Op0, Op0IsKill);
  case ISD::ZERO_EXTEND: return fastEmit_ISD_ZERO_EXTEND_r(VT, RetVT, Op0, Op0IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMP.

unsigned fastEmit_ARMISD_CMP_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2CMPrr, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tCMPr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::CMPrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMP_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMP_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMPZ.

unsigned fastEmit_ARMISD_CMPZ_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2CMPrr, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tCMPr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::CMPrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMPZ_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMPZ_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::EH_SJLJ_LONGJMP.

unsigned fastEmit_ARMISD_EH_SJLJ_LONGJMP_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isTargetWindows())) {
    return fastEmitInst_rr(ARM::tInt_WIN_eh_sjlj_longjmp, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isTargetWindows()) && (Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::tInt_eh_sjlj_longjmp, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::Int_eh_sjlj_longjmp, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_EH_SJLJ_LONGJMP_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_EH_SJLJ_LONGJMP_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::EH_SJLJ_SETJMP.

unsigned fastEmit_ARMISD_EH_SJLJ_SETJMP_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2()) && (!Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::t2Int_eh_sjlj_setjmp_nofp, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasVFP2()) && (Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2Int_eh_sjlj_setjmp, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tInt_eh_sjlj_setjmp, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb()) && (!Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::Int_eh_sjlj_setjmp_nofp, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasVFP2()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::Int_eh_sjlj_setjmp, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_EH_SJLJ_SETJMP_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_EH_SJLJ_SETJMP_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::SMULWB.

unsigned fastEmit_ARMISD_SMULWB_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasDSP()) && (Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2SMULWB, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasV5TEOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::SMULWB, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_SMULWB_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_SMULWB_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::SMULWT.

unsigned fastEmit_ARMISD_SMULWT_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasDSP()) && (Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2SMULWT, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasV5TEOps()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::SMULWT, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_SMULWT_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_SMULWT_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCEQ.

unsigned fastEmit_ARMISD_VCEQ_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCEQfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCEQ_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCEQ_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCEQ_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCEQ_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCEQ_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCEQ_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCEQ_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCEQ_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCEQ_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCEQ_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCEQ_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGE.

unsigned fastEmit_ARMISD_VCGE_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGE_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGE_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGE_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGE_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGE_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGE_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGE_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCGE_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCGE_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCGE_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCGE_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGEU.

unsigned fastEmit_ARMISD_VCGEU_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGEuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGEU_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGEU_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGEU_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGEU_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGEU_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGEU_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGEU_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGT.

unsigned fastEmit_ARMISD_VCGT_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGThd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGThq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGT_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGT_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGT_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGT_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGT_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGT_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGT_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ARMISD_VCGT_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ARMISD_VCGT_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ARMISD_VCGT_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ARMISD_VCGT_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VCGTU.

unsigned fastEmit_ARMISD_VCGTU_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VCGTuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VCGTU_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VCGTU_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VCGTU_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VCGTU_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VCGTU_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VCGTU_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VCGTU_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMOVDRR.

unsigned fastEmit_ARMISD_VMOVDRR_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VMOVDRR, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMOVDRR_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_VMOVDRR_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMULLs.

unsigned fastEmit_ARMISD_VMULLs_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLs_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLs_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLsv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLs_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VMULLs_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VMULLs_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VMULLs_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VMULLu.

unsigned fastEmit_ARMISD_VMULLu_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLu_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLu_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULLuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VMULLu_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VMULLu_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VMULLu_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VMULLu_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VTBL1.

unsigned fastEmit_ARMISD_VTBL1_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTBL1, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTBL1_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VTBL1_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VTST.

unsigned fastEmit_ARMISD_VTST_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VTSTv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VTST_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VTST_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ARMISD_VTST_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ARMISD_VTST_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ARMISD_VTST_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ARMISD_VTST_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ARMISD_VTST_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2ADDrr, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tADDrr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::ADDrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v1i64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_MVT_v2i64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i8: return fastEmit_ISD_ADD_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_ADD_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_ADD_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_ADD_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_ADD_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_ADD_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v1i64: return fastEmit_ISD_ADD_MVT_v1i64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i64: return fastEmit_ISD_ADD_MVT_v2i64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::AND.

unsigned fastEmit_ISD_AND_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2ANDrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tAND, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::ANDrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_AND_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VANDd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_AND_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VANDq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_AND_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_AND_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_AND_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_AND_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FADD.

unsigned fastEmit_ISD_FADD_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VADDH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->useNEONForSinglePrecisionFP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VADDS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VADDD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VADDfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FADD_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FADD_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FADD_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FADD_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ISD_FADD_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FADD_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FADD_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FADD_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FDIV.

unsigned fastEmit_ISD_FDIV_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VDIVH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FDIV_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VDIVS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FDIV_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VDIVD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FDIV_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FDIV_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FDIV_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FDIV_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FMAXNAN.

unsigned fastEmit_ISD_FMAXNAN_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNAN_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNAN_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNAN_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNAN_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4f16: return fastEmit_ISD_FMAXNAN_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FMAXNAN_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FMAXNAN_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FMAXNAN_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FMAXNUM.

unsigned fastEmit_ISD_FMAXNUM_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VMAXNMH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_rr(ARM::VMAXNMS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_rr(ARM::VMAXNMD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMAXNMNDh, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMAXNMNQh, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMAXNMNDf, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMAXNMNQf, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMAXNUM_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FMAXNUM_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FMAXNUM_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FMAXNUM_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ISD_FMAXNUM_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FMAXNUM_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FMAXNUM_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FMAXNUM_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FMINNAN.

unsigned fastEmit_ISD_FMINNAN_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNAN_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNAN_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNAN_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNAN_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v4f16: return fastEmit_ISD_FMINNAN_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FMINNAN_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FMINNAN_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FMINNAN_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FMINNUM.

unsigned fastEmit_ISD_FMINNUM_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VMINNMH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((Subtarget->hasFPARMv8())) {
    return fastEmitInst_rr(ARM::VMINNMS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasFPARMv8())) {
    return fastEmitInst_rr(ARM::VMINNMD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMINNMNDh, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMINNMNQh, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMINNMNDf, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON()) && (Subtarget->hasV8Ops())) {
    return fastEmitInst_rr(ARM::VMINNMNQf, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMINNUM_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FMINNUM_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FMINNUM_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FMINNUM_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ISD_FMINNUM_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FMINNUM_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FMINNUM_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FMINNUM_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FMUL.

unsigned fastEmit_ISD_FMUL_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VMULH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->useNEONForSinglePrecisionFP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VMULS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VMULD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FMUL_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FMUL_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FMUL_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FMUL_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ISD_FMUL_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FMUL_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FMUL_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FMUL_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::FSUB.

unsigned fastEmit_ISD_FSUB_MVT_f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f16)
    return 0;
  if ((Subtarget->hasFullFP16())) {
    return fastEmitInst_rr(ARM::VSUBH, &ARM::HPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f32)
    return 0;
  if ((!Subtarget->useNEONForSinglePrecisionFP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VSUBS, &ARM::SPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_f64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::f64)
    return 0;
  if ((!Subtarget->isFPOnlySP()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_rr(ARM::VSUBD, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_v4f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBhd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_v8f16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8f16)
    return 0;
  if ((Subtarget->hasFullFP16()) && (Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBhq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_v2f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBfd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_MVT_v4f32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4f32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBfq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_FSUB_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::f16: return fastEmit_ISD_FSUB_MVT_f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f32: return fastEmit_ISD_FSUB_MVT_f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::f64: return fastEmit_ISD_FSUB_MVT_f64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f16: return fastEmit_ISD_FSUB_MVT_v4f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8f16: return fastEmit_ISD_FSUB_MVT_v8f16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2f32: return fastEmit_ISD_FSUB_MVT_v2f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4f32: return fastEmit_ISD_FSUB_MVT_v4f32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::MUL.

unsigned fastEmit_ISD_MUL_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2MUL, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tMUL, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb()) && (!Subtarget->hasV6Ops()) && (Subtarget->useMulOps())) {
    return fastEmitInst_rr(ARM::MULv5, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasV6Ops()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::MUL, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMULv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MUL_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_MUL_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i8: return fastEmit_ISD_MUL_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_MUL_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_MUL_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_MUL_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_MUL_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_MUL_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::MULHS.

unsigned fastEmit_ISD_MULHS_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasDSP()) && (Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2SMMUL, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasV6Ops()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::SMMUL, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_MULHS_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_MULHS_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::OR.

unsigned fastEmit_ISD_OR_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2ORRrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tORR, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::ORRrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_OR_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VORRd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_OR_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VORRq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_OR_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_OR_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_OR_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_OR_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::ROTR.

unsigned fastEmit_ISD_ROTR_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2RORrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tROR, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_ROTR_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ROTR_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SDIV.

unsigned fastEmit_ISD_SDIV_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasDivideInThumbMode()) && (Subtarget->hasV8MBaselineOps()) && (Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::t2SDIV, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasDivideInARMMode()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::SDIV, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SDIV_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SDIV_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SHL.

unsigned fastEmit_ISD_SHL_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2LSLrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tLSLrr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SHL_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SHL_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SMAX.

unsigned fastEmit_ISD_SMAX_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMAX_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_SMAX_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_SMAX_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_SMAX_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_SMAX_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_SMAX_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_SMAX_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SMIN.

unsigned fastEmit_ISD_SMIN_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SMIN_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_SMIN_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_SMIN_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_SMIN_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_SMIN_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_SMIN_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_SMIN_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SRA.

unsigned fastEmit_ISD_SRA_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2ASRrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tASRrr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SRA_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SRA_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SRL.

unsigned fastEmit_ISD_SRL_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2LSRrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tLSRrr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SRL_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SRL_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::SUB.

unsigned fastEmit_ISD_SUB_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2SUBrr, &ARM::GPRnopcRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tSUBrr, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::SUBrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v1i64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_MVT_v2i64_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VSUBv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SUB_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i8: return fastEmit_ISD_SUB_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_SUB_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_SUB_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_SUB_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_SUB_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_SUB_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v1i64: return fastEmit_ISD_SUB_MVT_v1i64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i64: return fastEmit_ISD_SUB_MVT_v2i64_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::UDIV.

unsigned fastEmit_ISD_UDIV_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasDivideInThumbMode()) && (Subtarget->hasV8MBaselineOps()) && (Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::t2UDIV, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->hasDivideInARMMode()) && (!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::UDIV, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UDIV_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_UDIV_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::UMAX.

unsigned fastEmit_ISD_UMAX_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMAXuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMAX_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_UMAX_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_UMAX_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_UMAX_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_UMAX_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_UMAX_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_UMAX_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::UMIN.

unsigned fastEmit_ISD_UMIN_MVT_v8i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_MVT_v16i8_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_MVT_v4i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_MVT_v8i16_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VMINuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_UMIN_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ISD_UMIN_MVT_v8i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v16i8: return fastEmit_ISD_UMIN_MVT_v16i8_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i16: return fastEmit_ISD_UMIN_MVT_v4i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v8i16: return fastEmit_ISD_UMIN_MVT_v8i16_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_UMIN_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_UMIN_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ISD::XOR.

unsigned fastEmit_ISD_XOR_MVT_i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_rr(ARM::t2EORrr, &ARM::rGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_rr(ARM::tEOR, &ARM::tGPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_rr(ARM::EORrr, &ARM::GPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_XOR_MVT_v2i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VEORd, &ARM::DPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_XOR_MVT_v4i32_rr(MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_rr(ARM::VEORq, &ARM::QPRRegClass, Op0, Op0IsKill, Op1, Op1IsKill);
  }
  return 0;
}

unsigned fastEmit_ISD_XOR_rr(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_XOR_MVT_i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v2i32: return fastEmit_ISD_XOR_MVT_v2i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case MVT::v4i32: return fastEmit_ISD_XOR_MVT_v4i32_rr(RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_rr(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, unsigned Op1, bool Op1IsKill) override {
  switch (Opcode) {
  case ARMISD::CMP: return fastEmit_ARMISD_CMP_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::CMPZ: return fastEmit_ARMISD_CMPZ_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::EH_SJLJ_LONGJMP: return fastEmit_ARMISD_EH_SJLJ_LONGJMP_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::EH_SJLJ_SETJMP: return fastEmit_ARMISD_EH_SJLJ_SETJMP_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::SMULWB: return fastEmit_ARMISD_SMULWB_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::SMULWT: return fastEmit_ARMISD_SMULWT_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VCEQ: return fastEmit_ARMISD_VCEQ_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VCGE: return fastEmit_ARMISD_VCGE_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VCGEU: return fastEmit_ARMISD_VCGEU_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VCGT: return fastEmit_ARMISD_VCGT_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VCGTU: return fastEmit_ARMISD_VCGTU_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VMOVDRR: return fastEmit_ARMISD_VMOVDRR_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VMULLs: return fastEmit_ARMISD_VMULLs_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VMULLu: return fastEmit_ARMISD_VMULLu_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VTBL1: return fastEmit_ARMISD_VTBL1_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ARMISD::VTST: return fastEmit_ARMISD_VTST_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::ADD: return fastEmit_ISD_ADD_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::AND: return fastEmit_ISD_AND_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FADD: return fastEmit_ISD_FADD_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FDIV: return fastEmit_ISD_FDIV_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FMAXNAN: return fastEmit_ISD_FMAXNAN_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FMAXNUM: return fastEmit_ISD_FMAXNUM_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FMINNAN: return fastEmit_ISD_FMINNAN_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FMINNUM: return fastEmit_ISD_FMINNUM_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FMUL: return fastEmit_ISD_FMUL_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::FSUB: return fastEmit_ISD_FSUB_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::MUL: return fastEmit_ISD_MUL_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::MULHS: return fastEmit_ISD_MULHS_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::OR: return fastEmit_ISD_OR_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::ROTR: return fastEmit_ISD_ROTR_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SDIV: return fastEmit_ISD_SDIV_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SHL: return fastEmit_ISD_SHL_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SMAX: return fastEmit_ISD_SMAX_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SMIN: return fastEmit_ISD_SMIN_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SRA: return fastEmit_ISD_SRA_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SRL: return fastEmit_ISD_SRL_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::SUB: return fastEmit_ISD_SUB_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::UDIV: return fastEmit_ISD_UDIV_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::UMAX: return fastEmit_ISD_UMAX_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::UMIN: return fastEmit_ISD_UMIN_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  case ISD::XOR: return fastEmit_ISD_XOR_rr(VT, RetVT, Op0, Op0IsKill, Op1, Op1IsKill);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::PIC_ADD.

unsigned fastEmit_ARMISD_PIC_ADD_MVT_i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::tPICADD, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::PICADD, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_PIC_ADD_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_PIC_ADD_MVT_i32_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VDUPLANE.

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN8d, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN16d, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN32d, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v2f32_MVT_v2f32_ri(unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  return fastEmitInst_ri(ARM::VDUPLN32d, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v2f32_MVT_v4f32_ri(unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  return fastEmitInst_ri(ARM::VDUPLN32q, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v2f32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
switch (RetVT.SimpleTy) {
  case MVT::v2f32: return fastEmit_ARMISD_VDUPLANE_MVT_v2f32_MVT_v2f32_ri(Op0, Op0IsKill, imm1);
  case MVT::v4f32: return fastEmit_ARMISD_VDUPLANE_MVT_v2f32_MVT_v4f32_ri(Op0, Op0IsKill, imm1);
  default: return 0;
}
}

unsigned fastEmit_ARMISD_VDUPLANE_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VDUPLANE_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VDUPLANE_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VDUPLANE_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2f32: return fastEmit_ARMISD_VDUPLANE_MVT_v2f32_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VGETLANEs.

unsigned fastEmit_ARMISD_VGETLANEs_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VGETLNs8, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VGETLANEs_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VGETLNs16, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VGETLANEs_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VGETLANEs_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VGETLANEs_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VGETLANEu.

unsigned fastEmit_ARMISD_VGETLANEu_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VGETLNu8, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VGETLANEu_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VGETLNu16, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VGETLANEu_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VGETLANEu_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VGETLANEu_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHLs.

unsigned fastEmit_ARMISD_VQSHLs_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsiv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLs_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VQSHLs_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VQSHLs_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VQSHLs_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VQSHLs_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VQSHLs_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VQSHLs_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VQSHLs_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VQSHLs_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHLsu.

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLsuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLsu_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VQSHLsu_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VQSHLsu_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VQSHLsu_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VQSHLsu_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VQSHLsu_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VQSHLsu_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VQSHLsu_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VQSHLsu_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHLu.

unsigned fastEmit_ARMISD_VQSHLu_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHLuiv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHLu_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VQSHLu_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VQSHLu_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VQSHLu_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VQSHLu_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VQSHLu_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VQSHLu_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VQSHLu_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VQSHLu_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VRSHRs.

unsigned fastEmit_ARMISD_VRSHRs_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRsv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRs_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VRSHRs_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VRSHRs_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VRSHRs_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VRSHRs_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VRSHRs_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VRSHRs_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VRSHRs_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VRSHRs_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VRSHRu.

unsigned fastEmit_ARMISD_VRSHRu_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRu_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VRSHRu_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VRSHRu_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VRSHRu_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VRSHRu_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VRSHRu_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VRSHRu_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VRSHRu_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VRSHRu_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VSHL.

unsigned fastEmit_ARMISD_VSHL_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHLiv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHL_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VSHL_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VSHL_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VSHL_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VSHL_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VSHL_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VSHL_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VSHL_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VSHL_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VSHRs.

unsigned fastEmit_ARMISD_VSHRs_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRsv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRs_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VSHRs_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VSHRs_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VSHRs_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VSHRs_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VSHRs_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VSHRs_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VSHRs_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VSHRs_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VSHRu.

unsigned fastEmit_ARMISD_VSHRu_MVT_v8i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v16i8_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv16i8, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v4i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v8i16_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv8i16, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v4i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv4i32, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v1i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v1i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv1i64, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_MVT_v2i64_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i64)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VSHRuv2i64, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VSHRu_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VSHRu_MVT_v8i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v16i8: return fastEmit_ARMISD_VSHRu_MVT_v16i8_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VSHRu_MVT_v4i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v8i16: return fastEmit_ARMISD_VSHRu_MVT_v8i16_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VSHRu_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i32: return fastEmit_ARMISD_VSHRu_MVT_v4i32_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v1i64: return fastEmit_ARMISD_VSHRu_MVT_v1i64_ri(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i64: return fastEmit_ARMISD_VSHRu_MVT_v2i64_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::EXTRACT_VECTOR_ELT.

unsigned fastEmit_ISD_EXTRACT_VECTOR_ELT_MVT_v2i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->hasSlowVGETLNi32()) && (Subtarget->hasVFP2())) {
    return fastEmitInst_ri(ARM::VGETLNi32, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_EXTRACT_VECTOR_ELT_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i32: return fastEmit_ISD_EXTRACT_VECTOR_ELT_MVT_v2i32_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::SHL.

unsigned fastEmit_ISD_SHL_MVT_i32_ri(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_ri(ARM::tLSLri, &ARM::tGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_SHL_ri(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SHL_MVT_i32_ri(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) override {
  if (VT == MVT::i32 && Predicate_mod_imm(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_mod_imm(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm0_7(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm0_7(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm8_255(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm8_255(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm0_255(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm0_255(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_t2_so_imm(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_t2_so_imm(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm0_4095(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm0_4095(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm1_31(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm1_31(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_imm0_31(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_imm0_31(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_shr_imm8(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_shr_imm8(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_shr_imm16(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_shr_imm16(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_shr_imm32(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_shr_imm32(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  if (VT == MVT::i32 && Predicate_VectorIndex32(imm1))
    if (unsigned Reg = fastEmit_ri_Predicate_VectorIndex32(VT, RetVT, Opcode, Op0, Op0IsKill, imm1))
      return Reg;

  switch (Opcode) {
  case ARMISD::PIC_ADD: return fastEmit_ARMISD_PIC_ADD_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VDUPLANE: return fastEmit_ARMISD_VDUPLANE_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VGETLANEs: return fastEmit_ARMISD_VGETLANEs_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VGETLANEu: return fastEmit_ARMISD_VGETLANEu_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHLs: return fastEmit_ARMISD_VQSHLs_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHLsu: return fastEmit_ARMISD_VQSHLsu_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHLu: return fastEmit_ARMISD_VQSHLu_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VRSHRs: return fastEmit_ARMISD_VRSHRs_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VRSHRu: return fastEmit_ARMISD_VRSHRu_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VSHL: return fastEmit_ARMISD_VSHL_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VSHRs: return fastEmit_ARMISD_VSHRs_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VSHRu: return fastEmit_ARMISD_VSHRu_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::EXTRACT_VECTOR_ELT: return fastEmit_ISD_EXTRACT_VECTOR_ELT_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::SHL: return fastEmit_ISD_SHL_ri(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMN.

unsigned fastEmit_ARMISD_CMN_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::CMNri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMN_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMN_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMP.

unsigned fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::CMPri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMP_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMPZ.

unsigned fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::CMPri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMPZ_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::ADDri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::AND.

unsigned fastEmit_ISD_AND_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::ANDri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_AND_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_AND_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::OR.

unsigned fastEmit_ISD_OR_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::ORRri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_OR_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_OR_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::SUB.

unsigned fastEmit_ISD_SUB_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::SUBri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SUB_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::XOR.

unsigned fastEmit_ISD_XOR_MVT_i32_ri_Predicate_mod_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((!Subtarget->isThumb())) {
    return fastEmitInst_ri(ARM::EORri, &ARM::GPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_XOR_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_XOR_MVT_i32_ri_Predicate_mod_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_mod_imm(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::CMN: return fastEmit_ARMISD_CMN_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::CMP: return fastEmit_ARMISD_CMP_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::CMPZ: return fastEmit_ARMISD_CMPZ_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::ADD: return fastEmit_ISD_ADD_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::AND: return fastEmit_ISD_AND_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::OR: return fastEmit_ISD_OR_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::SUB: return fastEmit_ISD_SUB_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::XOR: return fastEmit_ISD_XOR_ri_Predicate_mod_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm0_7(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_ri(ARM::tADDi3, &ARM::tGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_ri_Predicate_imm0_7(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm0_7(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm0_7(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ISD::ADD: return fastEmit_ISD_ADD_ri_Predicate_imm0_7(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm8_255(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_ri(ARM::tADDi8, &ARM::tGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_ri_Predicate_imm8_255(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm8_255(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm8_255(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ISD::ADD: return fastEmit_ISD_ADD_ri_Predicate_imm8_255(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMP.

unsigned fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_imm0_255(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_ri(ARM::tCMPi8, &ARM::tGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMP_ri_Predicate_imm0_255(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_imm0_255(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMPZ.

unsigned fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_imm0_255(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->isThumb1Only())) {
    return fastEmitInst_ri(ARM::tCMPi8, &ARM::tGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMPZ_ri_Predicate_imm0_255(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_imm0_255(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm0_255(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::CMP: return fastEmit_ARMISD_CMP_ri_Predicate_imm0_255(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::CMPZ: return fastEmit_ARMISD_CMPZ_ri_Predicate_imm0_255(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMP.

unsigned fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2CMPri, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMP_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMP_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::CMPZ.

unsigned fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::isVoid)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2CMPri, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_CMPZ_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ARMISD_CMPZ_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2ADDri, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::AND.

unsigned fastEmit_ISD_AND_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2ANDri, &ARM::rGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_AND_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_AND_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::OR.

unsigned fastEmit_ISD_OR_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2ORRri, &ARM::rGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_OR_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_OR_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::SUB.

unsigned fastEmit_ISD_SUB_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2SUBri, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SUB_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::XOR.

unsigned fastEmit_ISD_XOR_MVT_i32_ri_Predicate_t2_so_imm(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2EORri, &ARM::rGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_XOR_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_XOR_MVT_i32_ri_Predicate_t2_so_imm(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_t2_so_imm(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::CMP: return fastEmit_ARMISD_CMP_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::CMPZ: return fastEmit_ARMISD_CMPZ_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::ADD: return fastEmit_ISD_ADD_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::AND: return fastEmit_ISD_AND_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::OR: return fastEmit_ISD_OR_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::SUB: return fastEmit_ISD_SUB_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::XOR: return fastEmit_ISD_XOR_ri_Predicate_t2_so_imm(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ADD.

unsigned fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm0_4095(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2ADDri12, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ADD_ri_Predicate_imm0_4095(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ADD_MVT_i32_ri_Predicate_imm0_4095(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::SUB.

unsigned fastEmit_ISD_SUB_MVT_i32_ri_Predicate_imm0_4095(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2SUBri12, &ARM::GPRnopcRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_SUB_ri_Predicate_imm0_4095(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SUB_MVT_i32_ri_Predicate_imm0_4095(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm0_4095(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ISD::ADD: return fastEmit_ISD_ADD_ri_Predicate_imm0_4095(VT, RetVT, Op0, Op0IsKill, imm1);
  case ISD::SUB: return fastEmit_ISD_SUB_ri_Predicate_imm0_4095(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::SHL.

unsigned fastEmit_ISD_SHL_MVT_i32_ri_Predicate_imm1_31(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2LSLri, &ARM::rGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_SHL_ri_Predicate_imm1_31(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_SHL_MVT_i32_ri_Predicate_imm1_31(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm1_31(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ISD::SHL: return fastEmit_ISD_SHL_ri_Predicate_imm1_31(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::ROTR.

unsigned fastEmit_ISD_ROTR_MVT_i32_ri_Predicate_imm0_31(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb2())) {
    return fastEmitInst_ri(ARM::t2RORri, &ARM::rGPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ISD_ROTR_ri_Predicate_imm0_31(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_ROTR_MVT_i32_ri_Predicate_imm0_31(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_imm0_31(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ISD::ROTR: return fastEmit_ISD_ROTR_ri_Predicate_imm0_31(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNs.

unsigned fastEmit_ARMISD_VQRSHRNs_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQRSHRNs_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNsu.

unsigned fastEmit_ARMISD_VQRSHRNsu_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRUNv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQRSHRNsu_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNu.

unsigned fastEmit_ARMISD_VQRSHRNu_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQRSHRNu_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNs.

unsigned fastEmit_ARMISD_VQSHRNs_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNsv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQSHRNs_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNsu.

unsigned fastEmit_ARMISD_VQSHRNsu_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRUNv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQSHRNsu_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNu.

unsigned fastEmit_ARMISD_VQSHRNu_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNuv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VQSHRNu_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VRSHRN.

unsigned fastEmit_ARMISD_VRSHRN_MVT_v8i16_ri_Predicate_shr_imm8(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRNv8i8, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i16: return fastEmit_ARMISD_VRSHRN_MVT_v8i16_ri_Predicate_shr_imm8(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_shr_imm8(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::VQRSHRNs: return fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNsu: return fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNu: return fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNs: return fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNsu: return fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNu: return fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VRSHRN: return fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm8(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNs.

unsigned fastEmit_ARMISD_VQRSHRNs_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQRSHRNs_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNsu.

unsigned fastEmit_ARMISD_VQRSHRNsu_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRUNv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQRSHRNsu_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNu.

unsigned fastEmit_ARMISD_VQRSHRNu_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQRSHRNu_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNs.

unsigned fastEmit_ARMISD_VQSHRNs_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNsv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQSHRNs_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNsu.

unsigned fastEmit_ARMISD_VQSHRNsu_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRUNv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQSHRNsu_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNu.

unsigned fastEmit_ARMISD_VQSHRNu_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNuv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VQSHRNu_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VRSHRN.

unsigned fastEmit_ARMISD_VRSHRN_MVT_v4i32_ri_Predicate_shr_imm16(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRNv4i16, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v4i32: return fastEmit_ARMISD_VRSHRN_MVT_v4i32_ri_Predicate_shr_imm16(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_shr_imm16(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::VQRSHRNs: return fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNsu: return fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNu: return fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNs: return fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNsu: return fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNu: return fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VRSHRN: return fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm16(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNs.

unsigned fastEmit_ARMISD_VQRSHRNs_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQRSHRNs_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNsu.

unsigned fastEmit_ARMISD_VQRSHRNsu_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRUNv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQRSHRNsu_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQRSHRNu.

unsigned fastEmit_ARMISD_VQRSHRNu_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQRSHRNuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQRSHRNu_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNs.

unsigned fastEmit_ARMISD_VQSHRNs_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNsv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQSHRNs_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNsu.

unsigned fastEmit_ARMISD_VQSHRNsu_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRUNv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQSHRNsu_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VQSHRNu.

unsigned fastEmit_ARMISD_VQSHRNu_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VQSHRNuv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VQSHRNu_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VRSHRN.

unsigned fastEmit_ARMISD_VRSHRN_MVT_v2i64_ri_Predicate_shr_imm32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v2i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VRSHRNv2i32, &ARM::DPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v2i64: return fastEmit_ARMISD_VRSHRN_MVT_v2i64_ri_Predicate_shr_imm32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_shr_imm32(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::VQRSHRNs: return fastEmit_ARMISD_VQRSHRNs_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNsu: return fastEmit_ARMISD_VQRSHRNsu_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQRSHRNu: return fastEmit_ARMISD_VQRSHRNu_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNs: return fastEmit_ARMISD_VQSHRNs_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNsu: return fastEmit_ARMISD_VQSHRNsu_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VQSHRNu: return fastEmit_ARMISD_VQSHRNu_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  case ARMISD::VRSHRN: return fastEmit_ARMISD_VRSHRN_ri_Predicate_shr_imm32(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ARMISD::VDUPLANE.

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v8i8_ri_Predicate_VectorIndex32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v16i8)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN8q, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v4i16_ri_Predicate_VectorIndex32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v8i16)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN16q, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_MVT_v2i32_ri_Predicate_VectorIndex32(MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  if (RetVT.SimpleTy != MVT::v4i32)
    return 0;
  if ((Subtarget->hasNEON())) {
    return fastEmitInst_ri(ARM::VDUPLN32q, &ARM::QPRRegClass, Op0, Op0IsKill, imm1);
  }
  return 0;
}

unsigned fastEmit_ARMISD_VDUPLANE_ri_Predicate_VectorIndex32(MVT VT, MVT RetVT, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (VT.SimpleTy) {
  case MVT::v8i8: return fastEmit_ARMISD_VDUPLANE_MVT_v8i8_ri_Predicate_VectorIndex32(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v4i16: return fastEmit_ARMISD_VDUPLANE_MVT_v4i16_ri_Predicate_VectorIndex32(RetVT, Op0, Op0IsKill, imm1);
  case MVT::v2i32: return fastEmit_ARMISD_VDUPLANE_MVT_v2i32_ri_Predicate_VectorIndex32(RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_ri_Predicate_VectorIndex32(MVT VT, MVT RetVT, unsigned Opcode, unsigned Op0, bool Op0IsKill, uint64_t imm1) {
  switch (Opcode) {
  case ARMISD::VDUPLANE: return fastEmit_ARMISD_VDUPLANE_ri_Predicate_VectorIndex32(VT, RetVT, Op0, Op0IsKill, imm1);
  default: return 0;
  }
}

// FastEmit functions for ISD::Constant.

unsigned fastEmit_ISD_Constant_MVT_i32_i(MVT RetVT, uint64_t imm0) {
  if (RetVT.SimpleTy != MVT::i32)
    return 0;
  if ((Subtarget->isThumb()) && (Subtarget->useMovt(*MF))) {
    return fastEmitInst_i(ARM::t2MOVi32imm, &ARM::rGPRRegClass, imm0);
  }
  return 0;
}

unsigned fastEmit_ISD_Constant_i(MVT VT, MVT RetVT, uint64_t imm0) {
  switch (VT.SimpleTy) {
  case MVT::i32: return fastEmit_ISD_Constant_MVT_i32_i(RetVT, imm0);
  default: return 0;
  }
}

// Top-level FastEmit function.

unsigned fastEmit_i(MVT VT, MVT RetVT, unsigned Opcode, uint64_t imm0) override {
  switch (Opcode) {
  case ISD::Constant: return fastEmit_ISD_Constant_i(VT, RetVT, imm0);
  default: return 0;
  }
}

