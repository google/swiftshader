//===- subzero/src/IceELFObjectWriter.h - ELF object writer -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Abstraction for a writer that is responsible for writing an ELF file.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEELFOBJECTWRITER_H
#define SUBZERO_SRC_ICEELFOBJECTWRITER_H

#include "IceDefs.h"
#include "IceELFSection.h"
#include "IceELFStreamer.h"

using namespace llvm::ELF;

namespace Ice {

// Higher level ELF object writer.  Manages section information and writes
// the final ELF object.  The object writer will write to file the code
// and data as it is being defined (rather than keep a copy).
// After all definitions are written out, it will finalize the bookkeeping
// sections and write them out.  Expected usage:
//
// (1) writeInitialELFHeader
// (2) writeDataInitializer*
// (3) writeFunctionCode*
// (4) writeNonUserSections
class ELFObjectWriter {
  ELFObjectWriter(const ELFObjectWriter &) = delete;
  ELFObjectWriter &operator=(const ELFObjectWriter &) = delete;

public:
  ELFObjectWriter(GlobalContext &Ctx, ELFStreamer &Out);

  // Write the initial ELF header. This is just to reserve space in the ELF
  // file. Reserving space allows the other functions to write text
  // and data directly to the file and get the right file offsets.
  void writeInitialELFHeader();

  // Copy data of a function's text section to file and note the offset of the
  // symbol's definition in the symbol table.
  // TODO(jvoung): This also needs the relocations to adjust the
  // section-relative offsets and hook them up to the symbol table references.
  void writeFunctionCode(const IceString &FuncName, bool IsInternal,
                         const llvm::StringRef Data);

  // Copy initializer data for a global to file and note the offset and
  // size of the global's definition in the symbol table.
  // TODO(jvoung): This needs to know which section. This also needs the
  // relocations to hook them up to the symbol table references. Also
  // TODO is handling BSS (which just needs to note the size).
  void writeDataInitializer(const IceString &VarName,
                            const llvm::StringRef Data);

  // Do final layout and write out the rest of the object file, then
  // patch up the initial ELF header with the final info.
  void writeNonUserSections();

private:
  GlobalContext &Ctx;
  ELFStreamer &Str;
  bool SectionNumbersAssigned;

  // All created sections, separated into different pools.
  typedef std::vector<ELFSection *> SectionList;
  typedef std::vector<ELFTextSection *> TextSectionList;
  typedef std::vector<ELFDataSection *> DataSectionList;
  typedef std::vector<ELFRelocationSectionBase *> RelSectionList;
  TextSectionList TextSections;
  RelSectionList RelTextSections;
  DataSectionList DataSections;
  RelSectionList RelDataSections;
  DataSectionList RoDataSections;
  RelSectionList RelRoDataSections;

  // Handles to special sections that need incremental bookkeeping.
  ELFSection *NullSection;
  ELFStringTableSection *ShStrTab;
  ELFSymbolTableSection *SymTab;
  ELFStringTableSection *StrTab;

  template <typename T>
  T *createSection(const IceString &Name, Elf64_Word ShType,
                   Elf64_Xword ShFlags, Elf64_Xword ShAddralign,
                   Elf64_Xword ShEntsize);

  // Align the file position before writing out a section's data,
  // and return the position of the file.
  Elf64_Off alignFileOffset(Elf64_Xword Align);

  // Assign an ordering / section numbers to each section.
  // Fill in other information that is only known near the end
  // (such as the size, if it wasn't already incrementally updated).
  // This then collects all sections in the decided order, into one vector,
  // for conveniently writing out all of the section headers.
  void assignSectionNumbersInfo(SectionList &AllSections);

  // This function assigns .foo and .rel.foo consecutive section numbers.
  // It also sets the relocation section's sh_info field to the related
  // section's number.
  template <typename UserSectionList>
  void assignRelSectionNumInPairs(SizeT &CurSectionNumber,
                                  UserSectionList &UserSections,
                                  RelSectionList &RelSections,
                                  SectionList &AllSections);

  // Link the relocation sections to the symbol table.
  void assignRelLinkNum(SizeT SymTabNumber, RelSectionList &RelSections);

  // Write the ELF file header with the given information about sections.
  template <bool IsELF64>
  void writeELFHeaderInternal(Elf64_Off SectionHeaderOffset,
                              SizeT SectHeaderStrIndex, SizeT NumSections);
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEELFOBJECTWRITER_H
