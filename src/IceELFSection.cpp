//===- subzero/src/IceELFSection.cpp - Representation of ELF sections -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines how ELF sections are represented.
///
//===----------------------------------------------------------------------===//

#include "IceELFSection.h"

#include "IceDefs.h"
#include "IceELFStreamer.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm::ELF;

namespace Ice {

// Text sections.

void ELFTextSection::appendData(ELFStreamer &Str,
                                const llvm::StringRef MoreData) {
  Str.writeBytes(MoreData);
  Header.sh_size += MoreData.size();
}

// Data sections.

void ELFDataSection::appendData(ELFStreamer &Str,
                                const llvm::StringRef MoreData) {
  Str.writeBytes(MoreData);
  Header.sh_size += MoreData.size();
}

void ELFDataSection::appendZeros(ELFStreamer &Str, SizeT NumBytes) {
  Str.writeZeroPadding(NumBytes);
  Header.sh_size += NumBytes;
}

void ELFDataSection::appendRelocationOffset(ELFStreamer &Str, bool IsRela,
                                            RelocOffsetT RelocOffset) {
  if (IsRela) {
    appendZeros(Str, RelocAddrSize);
    return;
  }
  static_assert(RelocAddrSize == 4, " writeLE32 assumes RelocAddrSize is 4");
  Str.writeLE32(RelocOffset);
  Header.sh_size += RelocAddrSize;
}

void ELFDataSection::padToAlignment(ELFStreamer &Str, Elf64_Xword Align) {
  assert(llvm::isPowerOf2_32(Align));
  Elf64_Xword AlignDiff = Utils::OffsetToAlignment(Header.sh_size, Align);
  if (AlignDiff == 0)
    return;
  if (Header.sh_type != llvm::ELF::SHT_NOBITS)
    Str.writeZeroPadding(AlignDiff);
  Header.sh_size += AlignDiff;
}

// Relocation sections.

void ELFRelocationSection::addRelocations(RelocOffsetT BaseOff,
                                          const FixupRefList &FixupRefs) {
  for (const AssemblerFixup *FR : FixupRefs) {
    Fixups.push_back(*FR);
    AssemblerFixup &F = Fixups.back();
    F.set_position(BaseOff + F.position());
  }
}

size_t ELFRelocationSection::getSectionDataSize() const {
  return Fixups.size() * Header.sh_entsize;
}

// Symbol tables.

void ELFSymbolTableSection::createNullSymbol(ELFSection *NullSection) {
  // The first entry in the symbol table should be a NULL entry, so make sure
  // the map is still empty.
  assert(LocalSymbols.empty());
  const IceString NullSymName("");
  createDefinedSym(NullSymName, STT_NOTYPE, STB_LOCAL, NullSection, 0, 0);
  NullSymbol = findSymbol(NullSymName);
}

void ELFSymbolTableSection::createDefinedSym(const IceString &Name,
                                             uint8_t Type, uint8_t Binding,
                                             ELFSection *Section,
                                             RelocOffsetT Offset, SizeT Size) {
  ELFSym NewSymbol = ELFSym();
  NewSymbol.Sym.setBindingAndType(Binding, Type);
  NewSymbol.Sym.st_value = Offset;
  NewSymbol.Sym.st_size = Size;
  NewSymbol.Section = Section;
  NewSymbol.Number = ELFSym::UnknownNumber;
  bool Unique;
  if (Binding == STB_LOCAL)
    Unique = LocalSymbols.insert(std::make_pair(Name, NewSymbol)).second;
  else
    Unique = GlobalSymbols.insert(std::make_pair(Name, NewSymbol)).second;
  assert(Unique);
  (void)Unique;
}

void ELFSymbolTableSection::noteUndefinedSym(const IceString &Name,
                                             ELFSection *NullSection) {
  ELFSym NewSymbol = ELFSym();
  NewSymbol.Sym.setBindingAndType(STB_GLOBAL, STT_NOTYPE);
  NewSymbol.Section = NullSection;
  NewSymbol.Number = ELFSym::UnknownNumber;
  bool Unique = GlobalSymbols.insert(std::make_pair(Name, NewSymbol)).second;
  if (!Unique) {
    std::string Buffer;
    llvm::raw_string_ostream StrBuf(Buffer);
    StrBuf << "Symbol external and defined: " << Name;
    llvm::report_fatal_error(StrBuf.str());
  }
  (void)Unique;
}

const ELFSym *ELFSymbolTableSection::findSymbol(const IceString &Name) const {
  auto I = LocalSymbols.find(Name);
  if (I != LocalSymbols.end())
    return &I->second;
  I = GlobalSymbols.find(Name);
  if (I != GlobalSymbols.end())
    return &I->second;
  return nullptr;
}

void ELFSymbolTableSection::updateIndices(const ELFStringTableSection *StrTab) {
  SizeT SymNumber = 0;
  for (auto &KeyValue : LocalSymbols) {
    const IceString &Name = KeyValue.first;
    ELFSection *Section = KeyValue.second.Section;
    Elf64_Sym &SymInfo = KeyValue.second.Sym;
    if (!Name.empty())
      SymInfo.st_name = StrTab->getIndex(Name);
    SymInfo.st_shndx = Section->getNumber();
    KeyValue.second.setNumber(SymNumber++);
  }
  for (auto &KeyValue : GlobalSymbols) {
    const IceString &Name = KeyValue.first;
    ELFSection *Section = KeyValue.second.Section;
    Elf64_Sym &SymInfo = KeyValue.second.Sym;
    if (!Name.empty())
      SymInfo.st_name = StrTab->getIndex(Name);
    SymInfo.st_shndx = Section->getNumber();
    KeyValue.second.setNumber(SymNumber++);
  }
}

void ELFSymbolTableSection::writeData(ELFStreamer &Str, bool IsELF64) {
  if (IsELF64) {
    writeSymbolMap<true>(Str, LocalSymbols);
    writeSymbolMap<true>(Str, GlobalSymbols);
  } else {
    writeSymbolMap<false>(Str, LocalSymbols);
    writeSymbolMap<false>(Str, GlobalSymbols);
  }
}

// String tables.

void ELFStringTableSection::add(const IceString &Str) {
  assert(!isLaidOut());
  assert(!Str.empty());
  StringToIndexMap.insert(std::make_pair(Str, UnknownIndex));
}

size_t ELFStringTableSection::getIndex(const IceString &Str) const {
  assert(isLaidOut());
  StringToIndexType::const_iterator It = StringToIndexMap.find(Str);
  if (It == StringToIndexMap.end()) {
    llvm_unreachable("String index not found");
    return UnknownIndex;
  }
  return It->second;
}

bool ELFStringTableSection::SuffixComparator::
operator()(const IceString &StrA, const IceString &StrB) const {
  size_t LenA = StrA.size();
  size_t LenB = StrB.size();
  size_t CommonLen = std::min(LenA, LenB);
  // If there is a difference in the common suffix, use that diff to sort.
  for (size_t i = 0; i < CommonLen; ++i) {
    char a = StrA[LenA - i - 1];
    char b = StrB[LenB - i - 1];
    if (a != b)
      return a > b;
  }
  // If the common suffixes are completely equal, let the longer one come
  // first, so that it can be laid out first and its characters shared.
  return LenA > LenB;
}

void ELFStringTableSection::doLayout() {
  assert(!isLaidOut());
  llvm::StringRef Prev;

  // String table starts with 0 byte.
  StringData.push_back(0);

  for (auto &StringIndex : StringToIndexMap) {
    assert(StringIndex.second == UnknownIndex);
    llvm::StringRef Cur = llvm::StringRef(StringIndex.first);
    if (Prev.endswith(Cur)) {
      // Prev is already in the StringData, and Cur is shorter than Prev based
      // on the sort.
      StringIndex.second = StringData.size() - Cur.size() - 1;
      continue;
    }
    StringIndex.second = StringData.size();
    std::copy(Cur.begin(), Cur.end(), back_inserter(StringData));
    StringData.push_back(0);
    Prev = Cur;
  }
}

} // end of namespace Ice
