//===- subzero/src/IceFixups.h - Assembler fixup kinds ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares generic fixup types.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEFIXUPS_H
#define SUBZERO_SRC_ICEFIXUPS_H

#include "IceDefs.h"

namespace Ice {

/// Each target and container format has a different namespace of relocations.
/// This holds the specific target+container format's relocation number.
using FixupKind = uint32_t;

/// Assembler fixups are positions in generated code/data that hold relocation
/// information that needs to be processed before finalizing the code/data.
struct AssemblerFixup {
  AssemblerFixup &operator=(const AssemblerFixup &) = delete;

public:
  AssemblerFixup() = default;
  AssemblerFixup(const AssemblerFixup &) = default;
  intptr_t position() const { return position_; }
  void set_position(intptr_t Position) { position_ = Position; }

  FixupKind kind() const { return kind_; }
  void set_kind(FixupKind Kind) { kind_ = Kind; }

  RelocOffsetT offset() const;
  IceString symbol(const GlobalContext *Ctx) const;

  static const Constant *NullSymbol;
  bool isNullSymbol() const { return value_ == NullSymbol; }

  void set_value(const Constant *Value) { value_ = Value; }

  /// Emits fixup, then returns the number of bytes to skip.
  virtual size_t emit(GlobalContext *Ctx, const Assembler &Asm) const;

private:
  intptr_t position_ = 0;
  FixupKind kind_ = 0;
  const Constant *value_ = nullptr;
};

/// Extends a fixup to be textual. That is, it emits text instead of a sequence
/// of bytes. This class is used as a fallback for unimplemented emitIAS
/// methods, allowing them to generate compilable assembly code.
class AssemblerTextFixup : public AssemblerFixup {
  AssemblerTextFixup() = delete;
  AssemblerTextFixup(const AssemblerTextFixup &) = delete;
  AssemblerTextFixup &operator=(const AssemblerTextFixup &) = delete;

public:
  AssemblerTextFixup(const std::string &Message, size_t NumBytes)
      : AssemblerFixup(), Message(Message), NumBytes(NumBytes) {}
  ~AssemblerTextFixup() = default;
  size_t emit(GlobalContext *Ctx, const Assembler &Asm) const override;

private:
  const std::string Message;
  const size_t NumBytes;
};

using FixupList = std::vector<AssemblerFixup>;
using FixupRefList = std::vector<AssemblerFixup *>;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEFIXUPS_H
