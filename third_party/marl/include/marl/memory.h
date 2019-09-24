// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef marl_memory_h
#define marl_memory_h

#include "debug.h"

#include <stdint.h>
#include <cstdlib>
#include <memory>
#include <utility>  // std::forward

namespace marl {

template <typename T>
inline T alignUp(T val, T alignment) {
  return alignment * ((val + alignment - 1) / alignment);
}

// aligned_malloc() allocates size bytes of uninitialized storage with the
// specified minimum byte alignment. The pointer returned must be freed with
// aligned_free().
inline void* aligned_malloc(size_t alignment, size_t size) {
  MARL_ASSERT(alignment < 256, "alignment must less than 256");
  auto allocation = new uint8_t[size + sizeof(uint8_t) + alignment];
  auto aligned = allocation;
  aligned += sizeof(uint8_t);  // Make space for the base-address offset.
  aligned = reinterpret_cast<uint8_t*>(
      alignUp(reinterpret_cast<uintptr_t>(aligned), alignment));  // align
  auto offset = static_cast<uint8_t>(aligned - allocation);
  aligned[-1] = offset;
  return aligned;
}

// aligned_free() frees memory allocated by aligned_malloc.
inline void aligned_free(void* ptr) {
  auto aligned = reinterpret_cast<uint8_t*>(ptr);
  auto offset = aligned[-1];
  auto allocation = aligned - offset;
  delete[] allocation;
}

// aligned_new() allocates and constructs an object of type T, respecting the
// alignment of the type.
// The pointer returned by aligned_new() must be deleted with aligned_delete().
template <typename T, typename... ARGS>
T* aligned_new(ARGS&&... args) {
  auto ptr = aligned_malloc(alignof(T), sizeof(T));
  new (ptr) T(std::forward<ARGS>(args)...);
  return reinterpret_cast<T*>(ptr);
}

// aligned_delete() destructs and frees the object allocated with aligned_new().
template <typename T>
void aligned_delete(T* object) {
  object->~T();
  aligned_free(object);
}

// make_aligned_shared() returns a new object wrapped in a std::shared_ptr that
// respects the alignemnt of the type.
template <typename T, typename... ARGS>
inline std::shared_ptr<T> make_aligned_shared(ARGS&&... args) {
  auto ptr = aligned_new<T>(std::forward<ARGS>(args)...);
  return std::shared_ptr<T>(ptr, aligned_delete<T>);
}

// aligned_storage() is a replacement for std::aligned_storage that isn't busted
// on older versions of MSVC.
template <size_t SIZE, size_t ALIGNMENT>
struct aligned_storage {
  struct alignas(ALIGNMENT) type {
    unsigned char data[SIZE];
  };
};

}  // namespace marl

#endif  // marl_memory_h
