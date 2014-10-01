// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
//
// Modified by the Subzero authors.
//
//===- subzero/src/assembler.h - Integrated assembler -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Assembler base class.  Instructions are assembled
// by architecture-specific assemblers that derive from this base class.
// This base class manages buffers and fixups for emitting code, etc.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ASSEMBLER_H
#define SUBZERO_SRC_ASSEMBLER_H

#include "IceDefs.h"

#include "IceFixups.h"
#include "llvm/Support/Allocator.h"

namespace Ice {

// Forward declarations.
class Assembler;
class AssemblerFixup;
class AssemblerBuffer;
class ConstantRelocatable;
class MemoryRegion;

// Assembler fixups are positions in generated code that hold relocation
// information that needs to be processed before finalizing the code
// into executable memory.
class AssemblerFixup {
public:
  virtual void Process(const MemoryRegion &region, intptr_t position) = 0;

  // It would be ideal if the destructor method could be made private,
  // but the g++ compiler complains when this is subclassed.
  virtual ~AssemblerFixup() { llvm_unreachable("~AssemblerFixup used"); }

  intptr_t position() const { return position_; }

  FixupKind kind() const { return kind_; }

  const ConstantRelocatable *value() const { return value_; }

protected:
  AssemblerFixup(FixupKind Kind, const ConstantRelocatable *Value)
      : position_(0), kind_(Kind), value_(Value) {}

private:
  intptr_t position_;
  FixupKind kind_;
  const ConstantRelocatable *value_;

  void set_position(intptr_t position) { position_ = position; }

  AssemblerFixup(const AssemblerFixup &) = delete;
  AssemblerFixup &operator=(const AssemblerFixup &) = delete;
  friend class AssemblerBuffer;
};

// Assembler buffers are used to emit binary code. They grow on demand.
class AssemblerBuffer {
public:
  AssemblerBuffer(Assembler &);
  ~AssemblerBuffer();

  // Basic support for emitting, loading, and storing.
  template <typename T> void Emit(T value) {
    assert(HasEnsuredCapacity());
    *reinterpret_cast<T *>(cursor_) = value;
    cursor_ += sizeof(T);
  }

  template <typename T> T Load(intptr_t position) const {
    assert(position >= 0 &&
           position <= (Size() - static_cast<intptr_t>(sizeof(T))));
    return *reinterpret_cast<T *>(contents_ + position);
  }

  template <typename T> void Store(intptr_t position, T value) {
    assert(position >= 0 &&
           position <= (Size() - static_cast<intptr_t>(sizeof(T))));
    *reinterpret_cast<T *>(contents_ + position) = value;
  }

  // Emit a fixup at the current location.
  void EmitFixup(AssemblerFixup *fixup) {
    fixup->set_position(Size());
    fixups_.push_back(fixup);
  }

  // Get the size of the emitted code.
  intptr_t Size() const { return cursor_ - contents_; }
  uintptr_t contents() const { return contents_; }

  // Copy the assembled instructions into the specified memory block
  // and apply all fixups.
  // TODO(jvoung): This will be different. We'll be writing the text
  // and reloc section to a file?
  void FinalizeInstructions(const MemoryRegion &region);

// To emit an instruction to the assembler buffer, the EnsureCapacity helper
// must be used to guarantee that the underlying data area is big enough to
// hold the emitted instruction. Usage:
//
//     AssemblerBuffer buffer;
//     AssemblerBuffer::EnsureCapacity ensured(&buffer);
//     ... emit bytes for single instruction ...

#if defined(DEBUG)
  class EnsureCapacity {
  public:
    explicit EnsureCapacity(AssemblerBuffer *buffer);
    ~EnsureCapacity();

  private:
    AssemblerBuffer *buffer_;
    intptr_t gap_;

    intptr_t ComputeGap() { return buffer_->Capacity() - buffer_->Size(); }
  };

  bool has_ensured_capacity_;
  bool HasEnsuredCapacity() const { return has_ensured_capacity_; }
#else
  class EnsureCapacity {
  public:
    explicit EnsureCapacity(AssemblerBuffer *buffer) {
      if (buffer->cursor() >= buffer->limit())
        buffer->ExtendCapacity();
    }
  };

  // When building the C++ tests, assertion code is enabled. To allow
  // asserting that the user of the assembler buffer has ensured the
  // capacity needed for emitting, we add a dummy method in non-debug mode.
  bool HasEnsuredCapacity() const { return true; }
#endif

  // Returns the position in the instruction stream.
  intptr_t GetPosition() const { return cursor_ - contents_; }

  // For bringup only.
  AssemblerFixup *GetLatestFixup() const;

private:
  // The limit is set to kMinimumGap bytes before the end of the data area.
  // This leaves enough space for the longest possible instruction and allows
  // for a single, fast space check per instruction.
  static const intptr_t kMinimumGap = 32;

  uintptr_t contents_;
  uintptr_t cursor_;
  uintptr_t limit_;
  Assembler &assembler_;
  std::vector<AssemblerFixup *> fixups_;
#if defined(DEBUG)
  bool fixups_processed_;
#endif

  uintptr_t cursor() const { return cursor_; }
  uintptr_t limit() const { return limit_; }
  intptr_t Capacity() const {
    assert(limit_ >= contents_);
    return (limit_ - contents_) + kMinimumGap;
  }

  // Process the fixup chain.
  void ProcessFixups(const MemoryRegion &region);

  // Compute the limit based on the data area and the capacity. See
  // description of kMinimumGap for the reasoning behind the value.
  static uintptr_t ComputeLimit(uintptr_t data, intptr_t capacity) {
    return data + capacity - kMinimumGap;
  }

  void ExtendCapacity();

  friend class AssemblerFixup;
};

class Assembler {
public:
  Assembler() {}
  ~Assembler() {}

  // Allocate a chunk of bytes using the per-Assembler allocator.
  uintptr_t AllocateBytes(size_t bytes) {
    // For now, alignment is not related to NaCl bundle alignment, since
    // the buffer's GetPosition is relative to the base. So NaCl bundle
    // alignment checks can be relative to that base. Later, the buffer
    // will be copied out to a ".text" section (or an in memory-buffer
    // that can be mprotect'ed with executable permission), and that
    // second buffer should be aligned for NaCl.
    const size_t Alignment = 16;
    return reinterpret_cast<uintptr_t>(Allocator.Allocate(bytes, Alignment));
  }

  // Allocate data of type T using the per-Assembler allocator.
  template <typename T> T *Allocate() { return Allocator.Allocate<T>(); }

private:
  llvm::BumpPtrAllocator Allocator;

  Assembler(const Assembler &) = delete;
  Assembler &operator=(const Assembler &) = delete;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ASSEMBLER_H_
