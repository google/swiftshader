//===- subzero/src/IceMemoryRegion.h - Memory region ------------*- C++ -*-===//
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
// This file declares the MemoryRegion class. It tracks a pointer
// plus its bounds for bounds-checking in debug mode.
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICE_MEMORY_REGION_H_
#define SUBZERO_SRC_ICE_MEMORY_REGION_H_

#include "IceDefs.h"
#include "IceUtils.h"

namespace Ice {

// Memory regions are useful for accessing memory with bounds check in
// debug mode. They can be safely passed by value and do not assume ownership
// of the region.
class MemoryRegion {
public:
  MemoryRegion(const MemoryRegion &other) = default;
  MemoryRegion &operator=(const MemoryRegion &other) = default;
  MemoryRegion() : pointer_(NULL), size_(0) {}
  MemoryRegion(void *pointer, size_t size) : pointer_(pointer), size_(size) {}

  void *pointer() const { return pointer_; }
  size_t size() const { return size_; }

  size_t start() const { return reinterpret_cast<size_t>(pointer_); }
  size_t end() const { return start() + size_; }

  template <typename T> T Load(size_t offset) const {
    return *ComputeInternalPointer<T>(offset);
  }

  template <typename T> void Store(size_t offset, T value) const {
    *ComputeInternalPointer<T>(offset) = value;
  }

  template <typename T> T *PointerTo(size_t offset) const {
    return ComputeInternalPointer<T>(offset);
  }

  bool Contains(size_t address) const {
    return (address >= start()) && (address < end());
  }

  void CopyFrom(size_t offset, const MemoryRegion &from) const;

  // Compute a sub memory region based on an existing one.
  void Subregion(const MemoryRegion &from, size_t offset, size_t size) {
    assert(from.size() >= size);
    assert(offset <= (from.size() - size));
    pointer_ = reinterpret_cast<void *>(from.start() + offset);
    size_ = size;
  }

  // Compute an extended memory region based on an existing one.
  void Extend(const MemoryRegion &region, size_t extra) {
    pointer_ = region.pointer();
    size_ = (region.size() + extra);
  }

private:
  template <typename T> T *ComputeInternalPointer(size_t offset) const {
    assert(size() >= sizeof(T));
    assert(offset <= size() - sizeof(T));
    return reinterpret_cast<T *>(start() + offset);
  }

  void *pointer_;
  size_t size_;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICE_MEMORY_REGION_H_
