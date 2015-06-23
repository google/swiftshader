//===- subzero/src/IceTargetLoweringX8632.cpp - x86-32 lowering -----------===//
//
//                        The Subzero Code Generator
//
//===----------------------------------------------------------------------===//
//
// This file implements the TargetLoweringX8632 class, which
// consists almost entirely of the lowering sequence for each
// high-level instruction.
//
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8632.h"

#include "IceTargetLoweringX86Base.h"

namespace Ice {
namespace X86Internal {
template <> struct MachineTraits<TargetX8632> {
  using InstructionSet = TargetX8632::X86InstructionSet;

  // The following table summarizes the logic for lowering the fcmp
  // instruction.  There is one table entry for each of the 16 conditions.
  //
  // The first four columns describe the case when the operands are
  // floating point scalar values.  A comment in lowerFcmp() describes the
  // lowering template.  In the most general case, there is a compare
  // followed by two conditional branches, because some fcmp conditions
  // don't map to a single x86 conditional branch.  However, in many cases
  // it is possible to swap the operands in the comparison and have a
  // single conditional branch.  Since it's quite tedious to validate the
  // table by hand, good execution tests are helpful.
  //
  // The last two columns describe the case when the operands are vectors
  // of floating point values.  For most fcmp conditions, there is a clear
  // mapping to a single x86 cmpps instruction variant.  Some fcmp
  // conditions require special code to handle and these are marked in the
  // table with a Cmpps_Invalid predicate.
  static const struct TableFcmpType {
    uint32_t Default;
    bool SwapScalarOperands;
    CondX86::BrCond C1, C2;
    bool SwapVectorOperands;
    CondX86::CmppsCond Predicate;
  } TableFcmp[];
  static const size_t TableFcmpSize;

  // The following table summarizes the logic for lowering the icmp instruction
  // for i32 and narrower types.  Each icmp condition has a clear mapping to an
  // x86 conditional branch instruction.

  static const struct TableIcmp32Type {
    CondX86::BrCond Mapping;
  } TableIcmp32[];
  static const size_t TableIcmp32Size;

  // The following table summarizes the logic for lowering the icmp instruction
  // for the i64 type.  For Eq and Ne, two separate 32-bit comparisons and
  // conditional branches are needed.  For the other conditions, three separate
  // conditional branches are needed.
  static const struct TableIcmp64Type {
    CondX86::BrCond C1, C2, C3;
  } TableIcmp64[];
  static const size_t TableIcmp64Size;

  static CondX86::BrCond getIcmp32Mapping(InstIcmp::ICond Cond) {
    size_t Index = static_cast<size_t>(Cond);
    assert(Index < TableIcmp32Size);
    return TableIcmp32[Index].Mapping;
  }

  static const struct TableTypeX8632AttributesType {
    Type InVectorElementType;
  } TableTypeX8632Attributes[];
  static const size_t TableTypeX8632AttributesSize;

  // Return the type which the elements of the vector have in the X86
  // representation of the vector.
  static Type getInVectorElementType(Type Ty) {
    assert(isVectorType(Ty));
    size_t Index = static_cast<size_t>(Ty);
    (void)Index;
    assert(Index < TableTypeX8632AttributesSize);
    return TableTypeX8632Attributes[Ty].InVectorElementType;
  }

  // The maximum number of arguments to pass in XMM registers
  static constexpr uint32_t X86_MAX_XMM_ARGS = 4;
  // The number of bits in a byte
  static constexpr uint32_t X86_CHAR_BIT = 8;
  // Stack alignment
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  // Size of the return address on the stack
  static constexpr uint32_t X86_RET_IP_SIZE_BYTES = 4;
  // The number of different NOP instructions
  static constexpr uint32_t X86_NUM_NOP_VARIANTS = 5;

  // Value is in bytes. Return Value adjusted to the next highest multiple
  // of the stack alignment.
  static uint32_t applyStackAlignment(uint32_t Value) {
    return Utils::applyAlignment(Value, X86_STACK_ALIGNMENT_BYTES);
  }
};

const MachineTraits<TargetX8632>::TableFcmpType
    MachineTraits<TargetX8632>::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  { dflt, swapS, CondX86::C1, CondX86::C2, swapV, CondX86::pred }              \
  ,
        FCMPX8632_TABLE
#undef X
};

constexpr size_t MachineTraits<TargetX8632>::TableFcmpSize =
    llvm::array_lengthof(TableFcmp);

const MachineTraits<TargetX8632>::TableIcmp32Type
    MachineTraits<TargetX8632>::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { CondX86::C_32 }                                                            \
  ,
        ICMPX8632_TABLE
#undef X
};

constexpr size_t MachineTraits<TargetX8632>::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const MachineTraits<TargetX8632>::TableIcmp64Type
    MachineTraits<TargetX8632>::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { CondX86::C1_64, CondX86::C2_64, CondX86::C3_64 }                           \
  ,
        ICMPX8632_TABLE
#undef X
};

constexpr size_t MachineTraits<TargetX8632>::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const MachineTraits<TargetX8632>::TableTypeX8632AttributesType
    MachineTraits<TargetX8632>::TableTypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { elementty }                                                                \
  ,
        ICETYPEX8632_TABLE
#undef X
};

constexpr size_t MachineTraits<TargetX8632>::TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

const uint32_t MachineTraits<TargetX8632>::X86_STACK_ALIGNMENT_BYTES = 16;
} // end of namespace X86Internal

TargetX8632 *TargetX8632::create(Cfg *Func) {
  return X86Internal::TargetX86Base<TargetX8632>::create(Func);
}

TargetDataX8632::TargetDataX8632(GlobalContext *Ctx)
    : TargetDataLowering(Ctx) {}

namespace {
template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  typedef uint32_t PrimitiveIntType;
  typedef ConstantFloat IceType;
  static const Type Ty = IceType_f32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<float>::TypeName = "float";
const char *PoolTypeConverter<float>::AsmTag = ".long";
const char *PoolTypeConverter<float>::PrintfString = "0x%x";

template <> struct PoolTypeConverter<double> {
  typedef uint64_t PrimitiveIntType;
  typedef ConstantDouble IceType;
  static const Type Ty = IceType_f64;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<double>::TypeName = "double";
const char *PoolTypeConverter<double>::AsmTag = ".quad";
const char *PoolTypeConverter<double>::PrintfString = "0x%llx";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint32_t> {
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
  static const Type Ty = IceType_i32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint32_t>::TypeName = "i32";
const char *PoolTypeConverter<uint32_t>::AsmTag = ".long";
const char *PoolTypeConverter<uint32_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint16_t> {
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
  static const Type Ty = IceType_i16;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint16_t>::TypeName = "i16";
const char *PoolTypeConverter<uint16_t>::AsmTag = ".short";
const char *PoolTypeConverter<uint16_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint8_t> {
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
  static const Type Ty = IceType_i8;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint8_t>::TypeName = "i8";
const char *PoolTypeConverter<uint8_t>::AsmTag = ".byte";
const char *PoolTypeConverter<uint8_t>::PrintfString = "0x%x";
} // end of anonymous namespace

template <typename T>
void TargetDataX8632::emitConstantPool(GlobalContext *Ctx) {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Ctx->getStrEmit();
  Type Ty = T::Ty;
  SizeT Align = typeAlignInBytes(Ty);
  ConstantList Pool = Ctx->getConstantPool(Ty);

  Str << "\t.section\t.rodata.cst" << Align << ",\"aM\",@progbits," << Align
      << "\n";
  Str << "\t.align\t" << Align << "\n";
  for (Constant *C : Pool) {
    if (!C->getShouldBePooled())
      continue;
    typename T::IceType *Const = llvm::cast<typename T::IceType>(C);
    typename T::IceType::PrimType Value = Const->getValue();
    // Use memcpy() to copy bits from Value into RawValue in a way
    // that avoids breaking strict-aliasing rules.
    typename T::PrimitiveIntType RawValue;
    memcpy(&RawValue, &Value, sizeof(Value));
    char buf[30];
    int CharsPrinted =
        snprintf(buf, llvm::array_lengthof(buf), T::PrintfString, RawValue);
    assert(CharsPrinted >= 0 &&
           (size_t)CharsPrinted < llvm::array_lengthof(buf));
    (void)CharsPrinted; // avoid warnings if asserts are disabled
    Const->emitPoolLabel(Str);
    Str << ":\n\t" << T::AsmTag << "\t" << buf << "\t# " << T::TypeName << " "
        << Value << "\n";
  }
}

void TargetDataX8632::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  // No need to emit constants from the int pool since (for x86) they
  // are embedded as immediates in the instructions, just emit float/double.
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();

    Writer->writeConstantPool<ConstantInteger32>(IceType_i8);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i16);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i32);

    Writer->writeConstantPool<ConstantFloat>(IceType_f32);
    Writer->writeConstantPool<ConstantDouble>(IceType_f64);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    OstreamLocker L(Ctx);

    emitConstantPool<PoolTypeConverter<uint8_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint16_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint32_t>>(Ctx);

    emitConstantPool<PoolTypeConverter<float>>(Ctx);
    emitConstantPool<PoolTypeConverter<double>>(Ctx);
  } break;
  }
}

void TargetDataX8632::lowerGlobals(const VariableDeclarationList &Vars,
                                   const IceString &SectionSuffix) {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, llvm::ELF::R_386_32, SectionSuffix);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    const IceString &TranslateOnly = Ctx->getFlags().getTranslateOnly();
    OstreamLocker L(Ctx);
    for (const VariableDeclaration *Var : Vars) {
      if (GlobalContext::matchSymbolName(Var->getName(), TranslateOnly)) {
        emitGlobal(*Var, SectionSuffix);
      }
    }
  } break;
  }
}

TargetHeaderX8632::TargetHeaderX8632(GlobalContext *Ctx)
    : TargetHeaderLowering(Ctx) {}

// In some cases, there are x-macros tables for both high-level and
// low-level instructions/operands that use the same enum key value.
// The tables are kept separate to maintain a proper separation
// between abstraction layers.  There is a risk that the tables could
// get out of sync if enum values are reordered or if entries are
// added or deleted.  The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

namespace {
// Validate the enum values in FCMPX8632_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
ICEINSTFCMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
FCMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8632_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8632_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pack, width, fld) _tmp_##tag,
  ICETYPEX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, size, align, elts, elty, str)                                   \
  static const int _table1_##tag = tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPEX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, size, align, elts, elty, str)                                   \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace Ice
