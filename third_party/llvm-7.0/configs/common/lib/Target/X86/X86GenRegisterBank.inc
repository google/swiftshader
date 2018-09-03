/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Register Bank Source Fragments                                             *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_REGBANK_DECLARATIONS
#undef GET_REGBANK_DECLARATIONS
namespace llvm {
namespace X86 {
enum {
  GPRRegBankID,
  VECRRegBankID,
  NumRegisterBanks,
};
} // end namespace X86
} // end namespace llvm
#endif // GET_REGBANK_DECLARATIONS

#ifdef GET_TARGET_REGBANK_CLASS
#undef GET_TARGET_REGBANK_CLASS
private:
  static RegisterBank *RegBanks[];

protected:
  X86GenRegisterBankInfo();

#endif // GET_TARGET_REGBANK_CLASS

#ifdef GET_TARGET_REGBANK_IMPL
#undef GET_TARGET_REGBANK_IMPL
namespace llvm {
namespace X86 {
const uint32_t GPRRegBankCoverageData[] = {
    // 0-31
    (1u << (X86::GR8RegClassID - 0)) |
    (1u << (X86::GR16RegClassID - 0)) |
    (1u << (X86::LOW32_ADDR_ACCESS_RBPRegClassID - 0)) |
    (1u << (X86::LOW32_ADDR_ACCESSRegClassID - 0)) |
    (1u << (X86::LOW32_ADDR_ACCESS_RBP_with_sub_8bitRegClassID - 0)) |
    (1u << (X86::GR32RegClassID - 0)) |
    (1u << (X86::GR32_NOSPRegClassID - 0)) |
    (1u << (X86::GR8_NOREXRegClassID - 0)) |
    (1u << (X86::GR8_ABCD_HRegClassID - 0)) |
    (1u << (X86::GR8_ABCD_LRegClassID - 0)) |
    (1u << (X86::GR16_NOREXRegClassID - 0)) |
    (1u << (X86::GR16_ABCDRegClassID - 0)) |
    (1u << (X86::LOW32_ADDR_ACCESS_RBP_with_sub_16bit_in_GR16_NOREXRegClassID - 0)) |
    (1u << (X86::GR32_NOREXRegClassID - 0)) |
    0,
    // 32-63
    (1u << (X86::GR64RegClassID - 32)) |
    (1u << (X86::GR64_with_sub_8bitRegClassID - 32)) |
    (1u << (X86::GR64_NOSPRegClassID - 32)) |
    (1u << (X86::GR64_NOSP_and_GR64_TCRegClassID - 32)) |
    (1u << (X86::GR64_TC_and_GR64_NOSP_and_GR64_TCW64RegClassID - 32)) |
    (1u << (X86::GR32_NOREX_NOSPRegClassID - 32)) |
    (1u << (X86::GR32_ABCDRegClassID - 32)) |
    (1u << (X86::GR32_TCRegClassID - 32)) |
    (1u << (X86::GR32_ADRegClassID - 32)) |
    (1u << (X86::GR64_NOREX_NOSP_and_GR64_TCRegClassID - 32)) |
    (1u << (X86::GR64_NOREX_NOSPRegClassID - 32)) |
    (1u << (X86::GR64_NOSP_and_GR64_TCW64RegClassID - 32)) |
    (1u << (X86::GR64_with_sub_16bit_in_GR16_NOREXRegClassID - 32)) |
    (1u << (X86::GR64_NOREXRegClassID - 32)) |
    (1u << (X86::GR64_NOREX_and_GR64_TCRegClassID - 32)) |
    (1u << (X86::GR64_TCRegClassID - 32)) |
    (1u << (X86::GR64_TC_and_GR64_TCW64RegClassID - 32)) |
    (1u << (X86::GR64_TCW64RegClassID - 32)) |
    0,
    // 64-95
    (1u << (X86::GR64_with_sub_32bit_in_GR32_TCRegClassID - 64)) |
    (1u << (X86::GR64_ADRegClassID - 64)) |
    (1u << (X86::GR64_ABCDRegClassID - 64)) |
    (1u << (X86::GR64_NOREX_NOSP_and_LOW32_ADDR_ACCESS_RBPRegClassID - 64)) |
    (1u << (X86::GR64_NOREX_and_GR64_TCW64RegClassID - 64)) |
    (1u << (X86::GR64_and_LOW32_ADDR_ACCESSRegClassID - 64)) |
    (1u << (X86::GR64_and_LOW32_ADDR_ACCESS_RBPRegClassID - 64)) |
    0,
};
const uint32_t VECRRegBankCoverageData[] = {
    // 0-31
    (1u << (X86::FR32XRegClassID - 0)) |
    (1u << (X86::FR32RegClassID - 0)) |
    0,
    // 32-63
    (1u << (X86::FR64XRegClassID - 32)) |
    (1u << (X86::FR64RegClassID - 32)) |
    0,
    // 64-95
    (1u << (X86::VR512RegClassID - 64)) |
    (1u << (X86::VR128XRegClassID - 64)) |
    (1u << (X86::VR256XRegClassID - 64)) |
    (1u << (X86::VR512_with_sub_xmm_in_FR32RegClassID - 64)) |
    (1u << (X86::VR128RegClassID - 64)) |
    (1u << (X86::VR256RegClassID - 64)) |
    (1u << (X86::VR512_with_sub_xmm_in_VR128HRegClassID - 64)) |
    (1u << (X86::VR128HRegClassID - 64)) |
    (1u << (X86::VR256HRegClassID - 64)) |
    (1u << (X86::VR512_with_sub_xmm_in_VR128LRegClassID - 64)) |
    (1u << (X86::VR128LRegClassID - 64)) |
    (1u << (X86::VR256LRegClassID - 64)) |
    0,
};

RegisterBank GPRRegBank(/* ID */ X86::GPRRegBankID, /* Name */ "GPR", /* Size */ 64, /* CoveredRegClasses */ GPRRegBankCoverageData, /* NumRegClasses */ 86);
RegisterBank VECRRegBank(/* ID */ X86::VECRRegBankID, /* Name */ "VECR", /* Size */ 512, /* CoveredRegClasses */ VECRRegBankCoverageData, /* NumRegClasses */ 86);
} // end namespace X86

RegisterBank *X86GenRegisterBankInfo::RegBanks[] = {
    &X86::GPRRegBank,
    &X86::VECRRegBank,
};

X86GenRegisterBankInfo::X86GenRegisterBankInfo()
    : RegisterBankInfo(RegBanks, X86::NumRegisterBanks) {
  // Assert that RegBank indices match their ID's
#ifndef NDEBUG
  unsigned Index = 0;
  for (const auto &RB : RegBanks)
    assert(Index++ == RB->getID() && "Index != ID");
#endif // NDEBUG
}
} // end namespace llvm
#endif // GET_TARGET_REGBANK_IMPL
