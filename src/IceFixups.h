//===- subzero/src/IceFixups.h - Assembler fixup kinds ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares generic fixup types.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEFIXUPS_H
#define SUBZERO_SRC_ICEFIXUPS_H

#include "IceDefs.h"

namespace Ice {

// Each target and container format has a different namespace of relocations.
// This holds the specific target+container format's relocation number.
typedef uint32_t FixupKind;

// Assembler fixups are positions in generated code/data that hold relocation
// information that needs to be processed before finalizing the code/data.
struct AssemblerFixup {
  AssemblerFixup &operator=(const AssemblerFixup &) = delete;

public:
  AssemblerFixup() : position_(0), kind_(0), value_(nullptr) {}
  AssemblerFixup(const AssemblerFixup &) = default;
  intptr_t position() const { return position_; }
  void set_position(intptr_t Position) { position_ = Position; }

  FixupKind kind() const { return kind_; }
  void set_kind(FixupKind Kind) { kind_ = Kind; }

  RelocOffsetT offset() const;
  IceString symbol(const GlobalContext *Ctx) const;
  void set_value(const Constant *Value) { value_ = Value; }

  void emit(GlobalContext *Ctx) const;

private:
  intptr_t position_;
  FixupKind kind_;
  const Constant *value_;
};

typedef std::vector<AssemblerFixup> FixupList;
typedef std::vector<AssemblerFixup *> FixupRefList;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEFIXUPS_H
