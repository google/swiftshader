//===- subzero/src/IceELFSection.cpp - Representation of ELF sections -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines how ELF sections are represented.
//
//===----------------------------------------------------------------------===//

#include "IceDefs.h"
#include "IceELFSection.h"
#include "IceELFStreamer.h"

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

// Relocation sections.

// Symbol tables.

void ELFSymbolTableSection::createDefinedSym(const IceString &Name,
                                             uint8_t Type, uint8_t Binding,
                                             ELFSection *Section,
                                             RelocOffsetT Offset, SizeT Size) {
  ELFSym NewSymbol = ELFSym();
  NewSymbol.Sym.setBindingAndType(Binding, Type);
  NewSymbol.Sym.st_value = Offset;
  NewSymbol.Sym.st_size = Size;
  NewSymbol.Number = ELFSym::UnknownNumber;
  SymtabKey Key = {Name, Section};
  bool Unique;
  if (Type == STB_LOCAL)
    Unique = LocalSymbols.insert(std::make_pair(Key, NewSymbol)).second;
  else
    Unique = GlobalSymbols.insert(std::make_pair(Key, NewSymbol)).second;
  assert(Unique);
  (void)Unique;
}

void ELFSymbolTableSection::noteUndefinedSym(const IceString &Name,
                                             ELFSection *NullSection) {
  ELFSym NewSymbol = ELFSym();
  NewSymbol.Sym.setBindingAndType(STB_GLOBAL, STT_NOTYPE);
  NewSymbol.Number = ELFSym::UnknownNumber;
  SymtabKey Key = {Name, NullSection};
  GlobalSymbols.insert(std::make_pair(Key, NewSymbol));
}

void ELFSymbolTableSection::updateIndices(const ELFStringTableSection *StrTab) {
  SizeT SymNumber = 0;
  for (auto &KeyValue : LocalSymbols) {
    const IceString &Name = KeyValue.first.first;
    ELFSection *Section = KeyValue.first.second;
    Elf64_Sym &SymInfo = KeyValue.second.Sym;
    if (!Name.empty())
      SymInfo.st_name = StrTab->getIndex(Name);
    SymInfo.st_shndx = Section->getNumber();
    KeyValue.second.setNumber(SymNumber++);
  }
  for (auto &KeyValue : GlobalSymbols) {
    const IceString &Name = KeyValue.first.first;
    ELFSection *Section = KeyValue.first.second;
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
      // Prev is already in the StringData, and Cur is shorter than Prev
      // based on the sort.
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
