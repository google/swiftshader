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
#include "IceMemoryRegion.h"

namespace Ice {

static uintptr_t NewContents(Assembler &assembler, intptr_t capacity) {
  uintptr_t result = assembler.AllocateBytes(capacity);
  return result;
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
  fixups_processed_ = false;
#endif // !NDEBUG

  // Verify internal state.
  assert(Capacity() == kInitialBufferCapacity);
  assert(Size() == 0);
}

AssemblerBuffer::~AssemblerBuffer() {}

AssemblerFixup *AssemblerBuffer::GetLatestFixup() const {
  if (fixups_.empty())
    return NULL;
  return fixups_.back();
}

void AssemblerBuffer::ProcessFixups(const MemoryRegion &region) {
  for (SizeT I = 0; I < fixups_.size(); ++I) {
    AssemblerFixup *fixup = fixups_[I];
    fixup->Process(region, fixup->position());
  }
}

void AssemblerBuffer::FinalizeInstructions(const MemoryRegion &instructions) {
  // Copy the instructions from the buffer.
  MemoryRegion from(reinterpret_cast<void *>(contents()), Size());
  instructions.CopyFrom(0, from);

  // Process fixups in the instructions.
  ProcessFixups(instructions);
#ifndef NDEBUG
  fixups_processed_ = true;
#endif // !NDEBUG
}

void AssemblerBuffer::ExtendCapacity() {
  intptr_t old_size = Size();
  intptr_t old_capacity = Capacity();
  const intptr_t OneMB = 1 << 20;
  intptr_t new_capacity = std::min(old_capacity * 2, old_capacity + OneMB);
  if (new_capacity < old_capacity) {
    // FATAL
    llvm_unreachable("Unexpected overflow in AssemblerBuffer::ExtendCapacity");
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

} // end of namespace Ice
