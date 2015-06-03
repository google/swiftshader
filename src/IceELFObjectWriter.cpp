//===- subzero/src/IceELFObjectWriter.cpp - ELF object file writer --------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the writer for ELF relocatable object files.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/MathExtras.h"

#include "assembler.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceELFSection.h"
#include "IceELFStreamer.h"
#include "IceGlobalContext.h"
#include "IceGlobalInits.h"
#include "IceOperand.h"

using namespace llvm::ELF;

namespace Ice {

namespace {

struct {
  bool IsELF64;
  uint16_t ELFMachine;
  uint32_t ELFFlags;
} ELFTargetInfo[] = {
#define X(tag, str, is_elf64, e_machine, e_flags)                              \
  { is_elf64, e_machine, e_flags }                                             \
  ,
    TARGETARCH_TABLE
#undef X
};

bool isELF64(TargetArch Arch) {
  if (Arch < TargetArch_NUM)
    return ELFTargetInfo[Arch].IsELF64;
  llvm_unreachable("Invalid target arch for isELF64");
  return false;
}

uint16_t getELFMachine(TargetArch Arch) {
  if (Arch < TargetArch_NUM)
    return ELFTargetInfo[Arch].ELFMachine;
  llvm_unreachable("Invalid target arch for getELFMachine");
  return EM_NONE;
}

uint32_t getELFFlags(TargetArch Arch) {
  if (Arch < TargetArch_NUM)
    return ELFTargetInfo[Arch].ELFFlags;
  llvm_unreachable("Invalid target arch for getELFFlags");
  return 0;
}

} // end of anonymous namespace

ELFObjectWriter::ELFObjectWriter(GlobalContext &Ctx, ELFStreamer &Out)
    : Ctx(Ctx), Str(Out), SectionNumbersAssigned(false),
      ELF64(isELF64(Ctx.getFlags().getTargetArch())) {
  // Create the special bookkeeping sections now.
  const IceString NullSectionName("");
  NullSection = new (Ctx.allocate<ELFSection>())
      ELFSection(NullSectionName, SHT_NULL, 0, 0, 0);

  const IceString ShStrTabName(".shstrtab");
  ShStrTab = new (Ctx.allocate<ELFStringTableSection>())
      ELFStringTableSection(ShStrTabName, SHT_STRTAB, 0, 1, 0);
  ShStrTab->add(ShStrTabName);

  const IceString SymTabName(".symtab");
  const Elf64_Xword SymTabAlign = ELF64 ? 8 : 4;
  const Elf64_Xword SymTabEntSize =
      ELF64 ? sizeof(Elf64_Sym) : sizeof(Elf32_Sym);
  static_assert(sizeof(Elf64_Sym) == 24 && sizeof(Elf32_Sym) == 16,
                "Elf_Sym sizes cannot be derived from sizeof");
  SymTab = createSection<ELFSymbolTableSection>(SymTabName, SHT_SYMTAB, 0,
                                                SymTabAlign, SymTabEntSize);
  SymTab->createNullSymbol(NullSection);

  const IceString StrTabName(".strtab");
  StrTab =
      createSection<ELFStringTableSection>(StrTabName, SHT_STRTAB, 0, 1, 0);
}

template <typename T>
T *ELFObjectWriter::createSection(const IceString &Name, Elf64_Word ShType,
                                  Elf64_Xword ShFlags, Elf64_Xword ShAddralign,
                                  Elf64_Xword ShEntsize) {
  assert(!SectionNumbersAssigned);
  T *NewSection =
      new (Ctx.allocate<T>()) T(Name, ShType, ShFlags, ShAddralign, ShEntsize);
  ShStrTab->add(Name);
  return NewSection;
}

ELFRelocationSection *
ELFObjectWriter::createRelocationSection(const ELFSection *RelatedSection) {
  // Choice of RELA vs REL is actually separate from elf64 vs elf32,
  // but in practice we've only had .rela for elf64 (x86-64).
  // In the future, the two properties may need to be decoupled
  // and the ShEntSize can vary more.
  const Elf64_Word ShType = ELF64 ? SHT_RELA : SHT_REL;
  IceString RelPrefix = ELF64 ? ".rela" : ".rel";
  IceString RelSectionName = RelPrefix + RelatedSection->getName();
  const Elf64_Xword ShAlign = ELF64 ? 8 : 4;
  const Elf64_Xword ShEntSize = ELF64 ? sizeof(Elf64_Rela) : sizeof(Elf32_Rel);
  static_assert(sizeof(Elf64_Rela) == 24 && sizeof(Elf32_Rel) == 8,
                "Elf_Rel/Rela sizes cannot be derived from sizeof");
  const Elf64_Xword ShFlags = 0;
  ELFRelocationSection *RelSection = createSection<ELFRelocationSection>(
      RelSectionName, ShType, ShFlags, ShAlign, ShEntSize);
  RelSection->setRelatedSection(RelatedSection);
  return RelSection;
}

template <typename UserSectionList>
void ELFObjectWriter::assignRelSectionNumInPairs(SizeT &CurSectionNumber,
                                                 UserSectionList &UserSections,
                                                 RelSectionList &RelSections,
                                                 SectionList &AllSections) {
  RelSectionList::iterator RelIt = RelSections.begin();
  RelSectionList::iterator RelE = RelSections.end();
  for (ELFSection *UserSection : UserSections) {
    UserSection->setNumber(CurSectionNumber++);
    UserSection->setNameStrIndex(ShStrTab->getIndex(UserSection->getName()));
    AllSections.push_back(UserSection);
    if (RelIt != RelE) {
      ELFRelocationSection *RelSection = *RelIt;
      if (RelSection->getRelatedSection() == UserSection) {
        RelSection->setInfoNum(UserSection->getNumber());
        RelSection->setNumber(CurSectionNumber++);
        RelSection->setNameStrIndex(ShStrTab->getIndex(RelSection->getName()));
        AllSections.push_back(RelSection);
        ++RelIt;
      }
    }
  }
  // Should finish with UserIt at the same time as RelIt.
  assert(RelIt == RelE);
  return;
}

void ELFObjectWriter::assignRelLinkNum(SizeT SymTabNumber,
                                       RelSectionList &RelSections) {
  for (ELFRelocationSection *S : RelSections) {
    S->setLinkNum(SymTabNumber);
  }
}

void ELFObjectWriter::assignSectionNumbersInfo(SectionList &AllSections) {
  // Go through each section, assigning them section numbers and
  // and fill in the size for sections that aren't incrementally updated.
  assert(!SectionNumbersAssigned);
  SizeT CurSectionNumber = 0;
  NullSection->setNumber(CurSectionNumber++);
  // The rest of the fields are initialized to 0, and stay that way.
  AllSections.push_back(NullSection);

  assignRelSectionNumInPairs<TextSectionList>(CurSectionNumber, TextSections,
                                              RelTextSections, AllSections);
  assignRelSectionNumInPairs<DataSectionList>(CurSectionNumber, DataSections,
                                              RelDataSections, AllSections);
  for (ELFSection *BSSSection : BSSSections) {
    BSSSection->setNumber(CurSectionNumber++);
    BSSSection->setNameStrIndex(ShStrTab->getIndex(BSSSection->getName()));
    AllSections.push_back(BSSSection);
  }
  assignRelSectionNumInPairs<DataSectionList>(CurSectionNumber, RODataSections,
                                              RelRODataSections, AllSections);

  ShStrTab->setNumber(CurSectionNumber++);
  ShStrTab->setNameStrIndex(ShStrTab->getIndex(ShStrTab->getName()));
  AllSections.push_back(ShStrTab);

  SymTab->setNumber(CurSectionNumber++);
  SymTab->setNameStrIndex(ShStrTab->getIndex(SymTab->getName()));
  AllSections.push_back(SymTab);

  StrTab->setNumber(CurSectionNumber++);
  StrTab->setNameStrIndex(ShStrTab->getIndex(StrTab->getName()));
  AllSections.push_back(StrTab);

  SymTab->setLinkNum(StrTab->getNumber());
  SymTab->setInfoNum(SymTab->getNumLocals());

  assignRelLinkNum(SymTab->getNumber(), RelTextSections);
  assignRelLinkNum(SymTab->getNumber(), RelDataSections);
  assignRelLinkNum(SymTab->getNumber(), RelRODataSections);
  SectionNumbersAssigned = true;
}

Elf64_Off ELFObjectWriter::alignFileOffset(Elf64_Xword Align) {
  Elf64_Off OffsetInFile = Str.tell();
  Elf64_Xword AlignDiff = Utils::OffsetToAlignment(OffsetInFile, Align);
  if (AlignDiff == 0)
    return OffsetInFile;
  Str.writeZeroPadding(AlignDiff);
  OffsetInFile += AlignDiff;
  return OffsetInFile;
}

void ELFObjectWriter::writeFunctionCode(const IceString &FuncName,
                                        bool IsInternal, const Assembler *Asm) {
  assert(!SectionNumbersAssigned);
  ELFTextSection *Section = nullptr;
  ELFRelocationSection *RelSection = nullptr;
  const bool FunctionSections = Ctx.getFlags().getFunctionSections();
  if (TextSections.empty() || FunctionSections) {
    IceString SectionName = ".text";
    if (FunctionSections)
      SectionName += "." + FuncName;
    const Elf64_Xword ShFlags = SHF_ALLOC | SHF_EXECINSTR;
    const Elf64_Xword ShAlign = 1 << Asm->getBundleAlignLog2Bytes();
    Section = createSection<ELFTextSection>(SectionName, SHT_PROGBITS, ShFlags,
                                            ShAlign, 0);
    Elf64_Off OffsetInFile = alignFileOffset(Section->getSectionAlign());
    Section->setFileOffset(OffsetInFile);
    TextSections.push_back(Section);
    RelSection = createRelocationSection(Section);
    RelTextSections.push_back(RelSection);
  } else {
    Section = TextSections[0];
    RelSection = RelTextSections[0];
  }
  RelocOffsetT OffsetInSection = Section->getCurrentSize();
  // Function symbols are set to 0 size in the symbol table,
  // in contrast to data symbols which have a proper size.
  SizeT SymbolSize = 0;
  Section->appendData(Str, Asm->getBufferView());
  uint8_t SymbolType;
  uint8_t SymbolBinding;
  if (IsInternal && !Ctx.getFlags().getDisableInternal()) {
    SymbolType = STT_NOTYPE;
    SymbolBinding = STB_LOCAL;
  } else {
    SymbolType = STT_FUNC;
    SymbolBinding = STB_GLOBAL;
  }
  SymTab->createDefinedSym(FuncName, SymbolType, SymbolBinding, Section,
                           OffsetInSection, SymbolSize);
  StrTab->add(FuncName);

  // Copy the fixup information from per-function Assembler memory to the
  // object writer's memory, for writing later.
  if (!Asm->fixups().empty()) {
    RelSection->addRelocations(OffsetInSection, Asm->fixups());
  }
}

namespace {

ELFObjectWriter::SectionType
classifyGlobalSection(const VariableDeclaration *Var) {
  if (Var->getIsConstant())
    return ELFObjectWriter::ROData;
  if (Var->hasNonzeroInitializer())
    return ELFObjectWriter::Data;
  return ELFObjectWriter::BSS;
}

// Partition the Vars list by SectionType into VarsBySection.
// If TranslateOnly is non-empty, then only the TranslateOnly variable
// is kept for emission.
void partitionGlobalsBySection(const VariableDeclarationList &Vars,
                               VariableDeclarationList VarsBySection[],
                               const IceString &TranslateOnly) {
  for (VariableDeclaration *Var : Vars) {
    if (GlobalContext::matchSymbolName(Var->getName(), TranslateOnly)) {
      size_t Section = classifyGlobalSection(Var);
      assert(Section < ELFObjectWriter::NumSectionTypes);
      VarsBySection[Section].push_back(Var);
    }
  }
}

} // end of anonymous namespace

void ELFObjectWriter::writeDataSection(const VariableDeclarationList &Vars,
                                       FixupKind RelocationKind) {
  assert(!SectionNumbersAssigned);
  VariableDeclarationList VarsBySection[ELFObjectWriter::NumSectionTypes];
  for (auto &SectionList : VarsBySection)
    SectionList.reserve(Vars.size());
  partitionGlobalsBySection(Vars, VarsBySection,
                            Ctx.getFlags().getTranslateOnly());
  size_t I = 0;
  for (auto &SectionList : VarsBySection) {
    writeDataOfType(static_cast<SectionType>(I++), SectionList, RelocationKind);
  }
}

void ELFObjectWriter::writeDataOfType(SectionType ST,
                                      const VariableDeclarationList &Vars,
                                      FixupKind RelocationKind) {
  if (Vars.empty())
    return;
  ELFDataSection *Section;
  ELFRelocationSection *RelSection;
  // TODO(jvoung): Handle fdata-sections.
  IceString SectionName;
  Elf64_Xword ShAddralign = 1;
  for (VariableDeclaration *Var : Vars) {
    Elf64_Xword Align = Var->getAlignment();
    ShAddralign = std::max(ShAddralign, Align);
  }
  const Elf64_Xword ShEntsize = 0; // non-uniform data element size.
  // Lift this out, so it can be re-used if we do fdata-sections?
  switch (ST) {
  case ROData: {
    SectionName = ".rodata";
    // Only expecting to write the data sections all in one shot for now.
    assert(RODataSections.empty());
    const Elf64_Xword ShFlags = SHF_ALLOC;
    Section = createSection<ELFDataSection>(SectionName, SHT_PROGBITS, ShFlags,
                                            ShAddralign, ShEntsize);
    Section->setFileOffset(alignFileOffset(ShAddralign));
    RODataSections.push_back(Section);
    RelSection = createRelocationSection(Section);
    RelRODataSections.push_back(RelSection);
    break;
  }
  case Data: {
    SectionName = ".data";
    assert(DataSections.empty());
    const Elf64_Xword ShFlags = SHF_ALLOC | SHF_WRITE;
    Section = createSection<ELFDataSection>(SectionName, SHT_PROGBITS, ShFlags,
                                            ShAddralign, ShEntsize);
    Section->setFileOffset(alignFileOffset(ShAddralign));
    DataSections.push_back(Section);
    RelSection = createRelocationSection(Section);
    RelDataSections.push_back(RelSection);
    break;
  }
  case BSS: {
    SectionName = ".bss";
    assert(BSSSections.empty());
    const Elf64_Xword ShFlags = SHF_ALLOC | SHF_WRITE;
    Section = createSection<ELFDataSection>(SectionName, SHT_NOBITS, ShFlags,
                                            ShAddralign, ShEntsize);
    Section->setFileOffset(alignFileOffset(ShAddralign));
    BSSSections.push_back(Section);
    break;
  }
  case NumSectionTypes:
    llvm::report_fatal_error("Unknown SectionType");
    break;
  }

  const uint8_t SymbolType = STT_OBJECT;
  for (VariableDeclaration *Var : Vars) {
    // If the variable declaration does not have an initializer, its symtab
    // entry will be created separately.
    if (!Var->hasInitializer())
      continue;
    Elf64_Xword Align = Var->getAlignment();
    const Elf64_Xword MinAlign = 1;
    Align = std::max(Align, MinAlign);
    Section->padToAlignment(Str, Align);
    SizeT SymbolSize = Var->getNumBytes();
    bool IsExternal = Var->isExternal() || Ctx.getFlags().getDisableInternal();
    const uint8_t SymbolBinding = IsExternal ? STB_GLOBAL : STB_LOCAL;
    IceString MangledName = Var->mangleName(&Ctx);
    SymTab->createDefinedSym(MangledName, SymbolType, SymbolBinding, Section,
                             Section->getCurrentSize(), SymbolSize);
    StrTab->add(MangledName);
    if (!Var->hasNonzeroInitializer()) {
      assert(ST == BSS || ST == ROData);
      if (ST == ROData)
        Section->appendZeros(Str, SymbolSize);
      else
        Section->setSize(Section->getCurrentSize() + SymbolSize);
    } else {
      assert(ST != BSS);
      for (VariableDeclaration::Initializer *Init : Var->getInitializers()) {
        switch (Init->getKind()) {
        case VariableDeclaration::Initializer::DataInitializerKind: {
          const auto Data =
              llvm::cast<VariableDeclaration::DataInitializer>(Init)
                  ->getContents();
          Section->appendData(Str, llvm::StringRef(Data.data(), Data.size()));
          break;
        }
        case VariableDeclaration::Initializer::ZeroInitializerKind:
          Section->appendZeros(Str, Init->getNumBytes());
          break;
        case VariableDeclaration::Initializer::RelocInitializerKind: {
          const auto Reloc =
              llvm::cast<VariableDeclaration::RelocInitializer>(Init);
          AssemblerFixup NewFixup;
          NewFixup.set_position(Section->getCurrentSize());
          NewFixup.set_kind(RelocationKind);
          const bool SuppressMangling = true;
          NewFixup.set_value(Ctx.getConstantSym(
              Reloc->getOffset(), Reloc->getDeclaration()->mangleName(&Ctx),
              SuppressMangling));
          RelSection->addRelocation(NewFixup);
          Section->appendRelocationOffset(Str, RelSection->isRela(),
                                          Reloc->getOffset());
          break;
        }
        }
      }
    }
  }
}

void ELFObjectWriter::writeInitialELFHeader() {
  assert(!SectionNumbersAssigned);
  const Elf64_Off DummySHOffset = 0;
  const SizeT DummySHStrIndex = 0;
  const SizeT DummyNumSections = 0;
  if (ELF64) {
    writeELFHeaderInternal<true>(DummySHOffset, DummySHStrIndex,
                                 DummyNumSections);
  } else {
    writeELFHeaderInternal<false>(DummySHOffset, DummySHStrIndex,
                                  DummyNumSections);
  }
}

template <bool IsELF64>
void ELFObjectWriter::writeELFHeaderInternal(Elf64_Off SectionHeaderOffset,
                                             SizeT SectHeaderStrIndex,
                                             SizeT NumSections) {
  // Write the e_ident: magic number, class, etc.
  // The e_ident is byte order and ELF class independent.
  Str.writeBytes(llvm::StringRef(ElfMagic, strlen(ElfMagic)));
  Str.write8(IsELF64 ? ELFCLASS64 : ELFCLASS32);
  Str.write8(ELFDATA2LSB);
  Str.write8(EV_CURRENT);
  Str.write8(ELFOSABI_NONE);
  const uint8_t ELF_ABIVersion = 0;
  Str.write8(ELF_ABIVersion);
  Str.writeZeroPadding(EI_NIDENT - EI_PAD);

  // TODO(jvoung): Handle and test > 64K sections.  See the generic ABI doc:
  // https://refspecs.linuxbase.org/elf/gabi4+/ch4.eheader.html
  // e_shnum should be 0 and then actual number of sections is
  // stored in the sh_size member of the 0th section.
  assert(NumSections < SHN_LORESERVE);
  assert(SectHeaderStrIndex < SHN_LORESERVE);

  const TargetArch Arch = Ctx.getFlags().getTargetArch();
  // Write the rest of the file header, which does depend on byte order
  // and ELF class.
  Str.writeLE16(ET_REL);                                        // e_type
  Str.writeLE16(getELFMachine(Ctx.getFlags().getTargetArch())); // e_machine
  Str.writeELFWord<IsELF64>(1);                                 // e_version
  // Since this is for a relocatable object, there is no entry point,
  // and no program headers.
  Str.writeAddrOrOffset<IsELF64>(0);                                // e_entry
  Str.writeAddrOrOffset<IsELF64>(0);                                // e_phoff
  Str.writeAddrOrOffset<IsELF64>(SectionHeaderOffset);              // e_shoff
  Str.writeELFWord<IsELF64>(getELFFlags(Arch));                     // e_flags
  Str.writeLE16(IsELF64 ? sizeof(Elf64_Ehdr) : sizeof(Elf32_Ehdr)); // e_ehsize
  static_assert(sizeof(Elf64_Ehdr) == 64 && sizeof(Elf32_Ehdr) == 52,
                "Elf_Ehdr sizes cannot be derived from sizeof");
  Str.writeLE16(0); // e_phentsize
  Str.writeLE16(0); // e_phnum
  Str.writeLE16(IsELF64 ? sizeof(Elf64_Shdr)
                        : sizeof(Elf32_Shdr)); // e_shentsize
  static_assert(sizeof(Elf64_Shdr) == 64 && sizeof(Elf32_Shdr) == 40,
                "Elf_Shdr sizes cannot be derived from sizeof");
  Str.writeLE16(static_cast<Elf64_Half>(NumSections));        // e_shnum
  Str.writeLE16(static_cast<Elf64_Half>(SectHeaderStrIndex)); // e_shstrndx
}

template <typename ConstType> void ELFObjectWriter::writeConstantPool(Type Ty) {
  ConstantList Pool = Ctx.getConstantPool(Ty);
  if (Pool.empty()) {
    return;
  }
  SizeT Align = typeAlignInBytes(Ty);
  size_t EntSize = typeWidthInBytes(Ty);
  char Buf[20];
  SizeT WriteAmt = std::min(EntSize, llvm::array_lengthof(Buf));
  assert(WriteAmt == EntSize);
  // Assume that writing WriteAmt bytes at a time allows us to avoid aligning
  // between entries.
  assert(WriteAmt % Align == 0);
  // Check that we write the full PrimType.
  assert(WriteAmt == sizeof(typename ConstType::PrimType));
  const Elf64_Xword ShFlags = SHF_ALLOC | SHF_MERGE;
  std::string SecBuffer;
  llvm::raw_string_ostream SecStrBuf(SecBuffer);
  SecStrBuf << ".rodata.cst" << WriteAmt;
  ELFDataSection *Section = createSection<ELFDataSection>(
      SecStrBuf.str(), SHT_PROGBITS, ShFlags, Align, WriteAmt);
  RODataSections.push_back(Section);
  SizeT OffsetInSection = 0;
  // The symbol table entry doesn't need to know the defined symbol's
  // size since this is in a section with a fixed Entry Size.
  const SizeT SymbolSize = 0;
  Section->setFileOffset(alignFileOffset(Align));

  // Write the data.
  for (Constant *C : Pool) {
    auto Const = llvm::cast<ConstType>(C);
    std::string SymBuffer;
    llvm::raw_string_ostream SymStrBuf(SymBuffer);
    Const->emitPoolLabel(SymStrBuf);
    std::string &SymName = SymStrBuf.str();
    SymTab->createDefinedSym(SymName, STT_NOTYPE, STB_LOCAL, Section,
                             OffsetInSection, SymbolSize);
    StrTab->add(SymName);
    typename ConstType::PrimType Value = Const->getValue();
    memcpy(Buf, &Value, WriteAmt);
    Str.writeBytes(llvm::StringRef(Buf, WriteAmt));
    OffsetInSection += WriteAmt;
  }
  Section->setSize(OffsetInSection);
}

// Instantiate known needed versions of the template, since we are
// defining the function in the .cpp file instead of the .h file.
// We may need to instantiate constant pools for integers as well
// if we do constant-pooling of large integers to remove them
// from the instruction stream (fewer bytes controlled by an attacker).
template void ELFObjectWriter::writeConstantPool<ConstantFloat>(Type Ty);

template void ELFObjectWriter::writeConstantPool<ConstantDouble>(Type Ty);

void ELFObjectWriter::writeAllRelocationSections() {
  writeRelocationSections(RelTextSections);
  writeRelocationSections(RelDataSections);
  writeRelocationSections(RelRODataSections);
}

void ELFObjectWriter::setUndefinedSyms(const ConstantList &UndefSyms) {
  for (const Constant *S : UndefSyms) {
    const auto Sym = llvm::cast<ConstantRelocatable>(S);
    const IceString &Name = Sym->getName();
    bool BadIntrinsic;
    const Intrinsics::FullIntrinsicInfo *Info =
        Ctx.getIntrinsicsInfo().find(Name, BadIntrinsic);
    if (Info)
      continue;
    assert(!BadIntrinsic);
    assert(Sym->getOffset() == 0);
    assert(Sym->getSuppressMangling());
    SymTab->noteUndefinedSym(Name, NullSection);
    StrTab->add(Name);
  }
}

void ELFObjectWriter::writeRelocationSections(RelSectionList &RelSections) {
  for (ELFRelocationSection *RelSec : RelSections) {
    Elf64_Off Offset = alignFileOffset(RelSec->getSectionAlign());
    RelSec->setFileOffset(Offset);
    RelSec->setSize(RelSec->getSectionDataSize());
    if (ELF64) {
      RelSec->writeData<true>(Ctx, Str, SymTab);
    } else {
      RelSec->writeData<false>(Ctx, Str, SymTab);
    }
  }
}

void ELFObjectWriter::writeNonUserSections() {
  // Write out the shstrtab now that all sections are known.
  ShStrTab->doLayout();
  ShStrTab->setSize(ShStrTab->getSectionDataSize());
  Elf64_Off ShStrTabOffset = alignFileOffset(ShStrTab->getSectionAlign());
  ShStrTab->setFileOffset(ShStrTabOffset);
  Str.writeBytes(ShStrTab->getSectionData());

  SectionList AllSections;
  assignSectionNumbersInfo(AllSections);

  // Finalize the regular StrTab and fix up references in the SymTab.
  StrTab->doLayout();
  StrTab->setSize(StrTab->getSectionDataSize());

  SymTab->updateIndices(StrTab);

  Elf64_Off SymTabOffset = alignFileOffset(SymTab->getSectionAlign());
  SymTab->setFileOffset(SymTabOffset);
  SymTab->setSize(SymTab->getSectionDataSize());
  SymTab->writeData(Str, ELF64);

  Elf64_Off StrTabOffset = alignFileOffset(StrTab->getSectionAlign());
  StrTab->setFileOffset(StrTabOffset);
  Str.writeBytes(StrTab->getSectionData());

  writeAllRelocationSections();

  // Write out the section headers.
  const size_t ShdrAlign = ELF64 ? 8 : 4;
  Elf64_Off ShOffset = alignFileOffset(ShdrAlign);
  for (const auto S : AllSections) {
    if (ELF64)
      S->writeHeader<true>(Str);
    else
      S->writeHeader<false>(Str);
  }

  // Finally write the updated ELF header w/ the correct number of sections.
  Str.seek(0);
  if (ELF64) {
    writeELFHeaderInternal<true>(ShOffset, ShStrTab->getNumber(),
                                 AllSections.size());
  } else {
    writeELFHeaderInternal<false>(ShOffset, ShStrTab->getNumber(),
                                  AllSections.size());
  }
}

} // end of namespace Ice
