//===- subzero/src/IceTargetLoweringX8632.cpp - x86-32 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the TargetLoweringX8632 class, which
/// consists almost entirely of the lowering sequence for each
/// high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8632.h"

#include "IceTargetLoweringX8632Traits.h"
#include "IceTargetLoweringX86Base.h"

namespace Ice {

namespace X86Internal {
const MachineTraits<TargetX8632>::TableFcmpType
    MachineTraits<TargetX8632>::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8632::Traits::Cond::C1, X8632::Traits::Cond::C2, swapV,      \
        X8632::Traits::Cond::pred                                              \
  }                                                                            \
  ,
        FCMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableFcmpSize =
    llvm::array_lengthof(TableFcmp);

const MachineTraits<TargetX8632>::TableIcmp32Type
    MachineTraits<TargetX8632>::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8632::Traits::Cond::C_32 }                                                \
  ,
        ICMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const MachineTraits<TargetX8632>::TableIcmp64Type
    MachineTraits<TargetX8632>::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8632::Traits::Cond::C1_64, X8632::Traits::Cond::C2_64,                    \
        X8632::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
        ICMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const MachineTraits<TargetX8632>::TableTypeX8632AttributesType
    MachineTraits<TargetX8632>::TableTypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { elementty }                                                                \
  ,
        ICETYPEX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

const uint32_t MachineTraits<TargetX8632>::X86_STACK_ALIGNMENT_BYTES = 16;
const char *MachineTraits<TargetX8632>::TargetName = "X8632";

} // end of namespace X86Internal

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

void TargetX8632::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  IceString MangledName = Ctx->mangleName(Func->getFunctionName());
  Str << "\t.section\t.rodata." << MangledName
      << "$jumptable,\"a\",@progbits\n";
  Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
  Str << InstJumpTable::makeName(MangledName, JumpTable->getId()) << ":";

  // On X8632 pointers are 32-bit hence the use of .long
  for (SizeT I = 0; I < JumpTable->getNumTargets(); ++I)
    Str << "\n\t.long\t" << JumpTable->getTarget(I)->getAsmName();
  Str << "\n";
}

template <typename T>
void TargetDataX8632::emitConstantPool(GlobalContext *Ctx) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Type Ty = T::Ty;
  SizeT Align = typeAlignInBytes(Ty);
  ConstantList Pool = Ctx->getConstantPool(Ty);

  Str << "\t.section\t.rodata.cst" << Align << ",\"aM\",@progbits," << Align
      << "\n";
  Str << "\t.align\t" << Align << "\n";

  // If reorder-pooled-constants option is set to true, we need to shuffle the
  // constant pool before emitting it.
  if (Ctx->getFlags().shouldReorderPooledConstants())
    RandomShuffle(Pool.begin(), Pool.end(), [Ctx](uint64_t N) {
      return (uint32_t)Ctx->getRNG().next(N);
    });

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

void TargetDataX8632::lowerJumpTables() {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    for (const JumpTableData &JumpTable : *Ctx->getJumpTables())
      Writer->writeJumpTable(JumpTable, llvm::ELF::R_386_32);
  } break;
  case FT_Asm:
    // Already emitted from Cfg
    break;
  case FT_Iasm: {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Ctx->getStrEmit();
    for (const JumpTableData &JT : *Ctx->getJumpTables()) {
      Str << "\t.section\t.rodata." << JT.getFunctionName()
          << "$jumptable,\"a\",@progbits\n";
      Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
      Str << InstJumpTable::makeName(JT.getFunctionName(), JT.getId()) << ":";

      // On X8632 pointers are 32-bit hence the use of .long
      for (intptr_t TargetOffset : JT.getTargetOffsets())
        Str << "\n\t.long\t" << JT.getFunctionName() << "+" << TargetOffset;
      Str << "\n";
    }
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
#define X(tag, sizeLog2, align, elts, elty, str)                               \
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
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace Ice
