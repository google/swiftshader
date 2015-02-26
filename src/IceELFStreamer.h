//===- subzero/src/IceELFStreamer.h - Low level ELF writing -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Interface for serializing bits for common ELF types (words, extended words,
// etc.), based on the ELF Class.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEELFSTREAMER_H
#define SUBZERO_SRC_ICEELFSTREAMER_H

#include "IceDefs.h"

namespace Ice {

// Low level writer that can that can handle ELFCLASS32/64.
// Little endian only for now.
class ELFStreamer {
  ELFStreamer() = delete;
  ELFStreamer(const ELFStreamer &) = delete;
  ELFStreamer &operator=(const ELFStreamer &) = delete;

public:
  explicit ELFStreamer(Fdstream &Out) : Out(Out) {}

  void write8(uint8_t Value) { Out << char(Value); }

  void writeLE16(uint16_t Value) {
    write8(uint8_t(Value));
    write8(uint8_t(Value >> 8));
  }

  void writeLE32(uint32_t Value) {
    writeLE16(uint16_t(Value));
    writeLE16(uint16_t(Value >> 16));
  }

  void writeLE64(uint64_t Value) {
    writeLE32(uint32_t(Value));
    writeLE32(uint32_t(Value >> 32));
  }

  template <bool IsELF64, typename T> void writeAddrOrOffset(T Value) {
    if (IsELF64)
      writeLE64(Value);
    else
      writeLE32(Value);
  }

  template <bool IsELF64, typename T> void writeELFWord(T Value) {
    writeLE32(Value);
  }

  template <bool IsELF64, typename T> void writeELFXword(T Value) {
    if (IsELF64)
      writeLE64(Value);
    else
      writeLE32(Value);
  }

  void writeBytes(llvm::StringRef Bytes) { Out << Bytes; }

  void writeZeroPadding(SizeT N) {
    static const char Zeros[16] = {0};

    for (SizeT i = 0, e = N / 16; i != e; ++i)
      Out << llvm::StringRef(Zeros, 16);

    Out << llvm::StringRef(Zeros, N % 16);
  }

  uint64_t tell() const { return Out.tell(); }

  void seek(uint64_t Off) { Out.seek(Off); }

private:
  Fdstream &Out;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEELFSTREAMER_H
