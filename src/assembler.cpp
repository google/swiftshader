//===- subzero/src/assembler.cpp - Assembler base class -------------------===//
// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
//
// Modified by the Subzero authors.
//
//===----------------------------------------------------------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Assembler class.
//
//===----------------------------------------------------------------------===//

#include "assembler.h"
#include "IceGlobalContext.h"
#include "IceOperand.h"

namespace Ice {

static uintptr_t NewContents(Assembler &assembler, intptr_t capacity) {
  uintptr_t result = assembler.AllocateBytes(capacity);
  return result;
}

AssemblerFixup *AssemblerBuffer::createFixup(FixupKind Kind,
                                             const Constant *Value) {
  AssemblerFixup *F =
      new (assembler_.Allocate<AssemblerFixup>()) AssemblerFixup();
  F->set_position(0);
  F->set_kind(Kind);
  F->set_value(Value);
  fixups_.push_back(F);
  return F;
}

#ifndef NDEBUG
AssemblerBuffer::EnsureCapacity::EnsureCapacity(AssemblerBuffer *buffer) {
  if (buffer->cursor() >= buffer->limit())
    buffer->ExtendCapacity();
  // In debug mode, we save the assembler buffer along with the gap
  // size before we start emitting to the buffer. This allows us to
  // check that any single generated instruction doesn't overflow the
  // limit implied by the minimum gap size.
  buffer_ = buffer;
  gap_ = ComputeGap();
  // Make sure that extending the capacity leaves a big enough gap
  // for any kind of instruction.
  assert(gap_ >= kMinimumGap);
  // Mark the buffer as having ensured the capacity.
  assert(!buffer->HasEnsuredCapacity()); // Cannot nest.
  buffer->has_ensured_capacity_ = true;
}

AssemblerBuffer::EnsureCapacity::~EnsureCapacity() {
  // Unmark the buffer, so we cannot emit after this.
  buffer_->has_ensured_capacity_ = false;
  // Make sure the generated instruction doesn't take up more
  // space than the minimum gap.
  intptr_t delta = gap_ - ComputeGap();
  assert(delta <= kMinimumGap);
}
#endif // !NDEBUG

AssemblerBuffer::AssemblerBuffer(Assembler &assembler) : assembler_(assembler) {
  const intptr_t OneKB = 1024;
  static const intptr_t kInitialBufferCapacity = 4 * OneKB;
  contents_ = NewContents(assembler_, kInitialBufferCapacity);
  cursor_ = contents_;
  limit_ = ComputeLimit(contents_, kInitialBufferCapacity);
#ifndef NDEBUG
  has_ensured_capacity_ = false;
#endif // !NDEBUG

  // Verify internal state.
  assert(Capacity() == kInitialBufferCapacity);
  assert(Size() == 0);
}

AssemblerBuffer::~AssemblerBuffer() {}

void AssemblerBuffer::ExtendCapacity() {
  intptr_t old_size = Size();
  intptr_t old_capacity = Capacity();
  const intptr_t OneMB = 1 << 20;
  intptr_t new_capacity = std::min(old_capacity * 2, old_capacity + OneMB);
  if (new_capacity < old_capacity) {
    llvm::report_fatal_error(
        "Unexpected overflow in AssemblerBuffer::ExtendCapacity");
  }

  // Allocate the new data area and copy contents of the old one to it.
  uintptr_t new_contents = NewContents(assembler_, new_capacity);
  memmove(reinterpret_cast<void *>(new_contents),
          reinterpret_cast<void *>(contents_), old_size);

  // Compute the relocation delta and switch to the new contents area.
  intptr_t delta = new_contents - contents_;
  contents_ = new_contents;

  // Update the cursor and recompute the limit.
  cursor_ += delta;
  limit_ = ComputeLimit(new_contents, new_capacity);

  // Verify internal state.
  assert(Capacity() == new_capacity);
  assert(Size() == old_size);
}

llvm::StringRef Assembler::getBufferView() const {
  return llvm::StringRef(reinterpret_cast<const char *>(buffer_.contents()),
                         buffer_.Size());
}

void Assembler::emitIASBytes(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  intptr_t EndPosition = buffer_.Size();
  intptr_t CurPosition = 0;
  const intptr_t FixupSize = 4;
  for (const AssemblerFixup *NextFixup : fixups()) {
    intptr_t NextFixupLoc = NextFixup->position();
    for (intptr_t i = CurPosition; i < NextFixupLoc; ++i) {
      Str << "\t.byte 0x";
      Str.write_hex(buffer_.Load<uint8_t>(i));
      Str << "\n";
    }
    Str << "\t.long ";
    NextFixup->emit(Ctx);
    if (fixupIsPCRel(NextFixup->kind()))
      Str << " - (. + " << FixupSize << ")";
    Str << "\n";
    CurPosition = NextFixupLoc + FixupSize;
    assert(CurPosition <= EndPosition);
  }
  // Handle any bytes that are not prefixed by a fixup.
  for (intptr_t i = CurPosition; i < EndPosition; ++i) {
    Str << "\t.byte 0x";
    Str.write_hex(buffer_.Load<uint8_t>(i));
    Str << "\n";
  }
}

} // end of namespace Ice
