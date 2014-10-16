//===- subzero/src/IceMemoryRegion.cpp - Memory region --------------------===//
// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
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
// This file defines the MemoryRegion class. It tracks a pointer plus its
// bounds for bounds-checking in debug mode.
//===----------------------------------------------------------------------===//

#include "IceMemoryRegion.h"

namespace Ice {

void MemoryRegion::CopyFrom(uintptr_t offset, const MemoryRegion &from) const {
  assert(from.pointer() != NULL && from.size() > 0);
  assert(this->size() >= from.size());
  assert(offset <= this->size() - from.size());
  memmove(reinterpret_cast<void *>(start() + offset), from.pointer(),
          from.size());
}

} // end of namespace Ice
