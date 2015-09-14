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

  void emit(GlobalContext *Ctx, RelocOffsetT OverrideOffset) const;

private:
  intptr_t position_ = 0;
  FixupKind kind_ = 0;
  const Constant *value_ = nullptr;
};

using FixupList = std::vector<AssemblerFixup>;
using FixupRefList = std::vector<AssemblerFixup *>;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEFIXUPS_H
