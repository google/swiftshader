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

#ifndef marl_containers_h
#define marl_containers_h

#include "debug.h"
#include "memory.h"

#include <algorithm>  // std::max
#include <utility>    // std::move

#include <cstddef>  // size_t

namespace marl {
namespace containers {

////////////////////////////////////////////////////////////////////////////////
// vector<T, BASE_CAPACITY>
////////////////////////////////////////////////////////////////////////////////

// vector is a container of contiguously stored elements.
// Unlike std::vector, marl::containers::vector keeps the first BASE_CAPACITY
// elements internally, which will avoid dynamic heap allocations.
// Once the vector exceeds BASE_CAPACITY elements, vector will allocate storage
// from the heap.
template <typename T, int BASE_CAPACITY>
class vector {
 public:
  inline vector(Allocator* allocator = Allocator::Default);

  template <int BASE_CAPACITY_2>
  inline vector(const vector<T, BASE_CAPACITY_2>& other,
                Allocator* allocator = Allocator::Default);

  template <int BASE_CAPACITY_2>
  inline vector(vector<T, BASE_CAPACITY_2>&& other,
                Allocator* allocator = Allocator::Default);

  inline ~vector();

  template <int BASE_CAPACITY_2>
  inline vector<T, BASE_CAPACITY>& operator=(const vector<T, BASE_CAPACITY_2>&);

  template <int BASE_CAPACITY_2>
  inline vector<T, BASE_CAPACITY>& operator=(vector<T, BASE_CAPACITY_2>&&);

  inline void push_back(const T& el);
  inline void emplace_back(T&& el);
  inline void pop_back();
  inline T& front();
  inline T& back();
  inline T* begin();
  inline T* end();
  inline T& operator[](size_t i);
  inline const T& operator[](size_t i) const;
  inline size_t size() const;
  inline size_t cap() const;
  inline void resize(size_t n);
  inline void reserve(size_t n);

 private:
  using TStorage = typename marl::aligned_storage<sizeof(T), alignof(T)>::type;

  inline void free();

  Allocator* const allocator;
  size_t count = 0;
  size_t capacity = BASE_CAPACITY;
  TStorage buffer[BASE_CAPACITY];
  TStorage* elements = buffer;
  Allocation allocation;
};

template <typename T, int BASE_CAPACITY>
vector<T, BASE_CAPACITY>::vector(
    Allocator* allocator /* = Allocator::Default */)
    : allocator(allocator) {}

template <typename T, int BASE_CAPACITY>
template <int BASE_CAPACITY_2>
vector<T, BASE_CAPACITY>::vector(
    const vector<T, BASE_CAPACITY_2>& other,
    Allocator* allocator /* = Allocator::Default */)
    : allocator(allocator) {
  *this = other;
}

template <typename T, int BASE_CAPACITY>
template <int BASE_CAPACITY_2>
vector<T, BASE_CAPACITY>::vector(
    vector<T, BASE_CAPACITY_2>&& other,
    Allocator* allocator /* = Allocator::Default */)
    : allocator(allocator) {
  *this = std::move(other);
}

template <typename T, int BASE_CAPACITY>
vector<T, BASE_CAPACITY>::~vector() {
  free();
}

template <typename T, int BASE_CAPACITY>
template <int BASE_CAPACITY_2>
vector<T, BASE_CAPACITY>& vector<T, BASE_CAPACITY>::operator=(
    const vector<T, BASE_CAPACITY_2>& other) {
  free();
  reserve(other.size());
  count = other.size();
  for (size_t i = 0; i < count; i++) {
    new (&reinterpret_cast<T*>(elements)[i]) T(other[i]);
  }
  return *this;
}

template <typename T, int BASE_CAPACITY>
template <int BASE_CAPACITY_2>
vector<T, BASE_CAPACITY>& vector<T, BASE_CAPACITY>::operator=(
    vector<T, BASE_CAPACITY_2>&& other) {
  free();
  reserve(other.size());
  count = other.size();
  for (size_t i = 0; i < count; i++) {
    new (&reinterpret_cast<T*>(elements)[i]) T(std::move(other[i]));
  }
  other.resize(0);
  return *this;
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::push_back(const T& el) {
  reserve(count + 1);
  new (&reinterpret_cast<T*>(elements)[count]) T(el);
  count++;
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::emplace_back(T&& el) {
  reserve(count + 1);
  new (&reinterpret_cast<T*>(elements)[count]) T(std::move(el));
  count++;
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::pop_back() {
  MARL_ASSERT(count > 0, "pop_back() called on empty vector");
  count--;
  reinterpret_cast<T*>(elements)[count].~T();
}

template <typename T, int BASE_CAPACITY>
T& vector<T, BASE_CAPACITY>::front() {
  MARL_ASSERT(count > 0, "front() called on empty vector");
  return reinterpret_cast<T*>(elements)[0];
}

template <typename T, int BASE_CAPACITY>
T& vector<T, BASE_CAPACITY>::back() {
  MARL_ASSERT(count > 0, "back() called on empty vector");
  return reinterpret_cast<T*>(elements)[count - 1];
}

template <typename T, int BASE_CAPACITY>
T* vector<T, BASE_CAPACITY>::begin() {
  return reinterpret_cast<T*>(elements);
}

template <typename T, int BASE_CAPACITY>
T* vector<T, BASE_CAPACITY>::end() {
  return reinterpret_cast<T*>(elements) + count;
}

template <typename T, int BASE_CAPACITY>
T& vector<T, BASE_CAPACITY>::operator[](size_t i) {
  MARL_ASSERT(i < count, "index %d exceeds vector size %d", int(i), int(count));
  return reinterpret_cast<T*>(elements)[i];
}

template <typename T, int BASE_CAPACITY>
const T& vector<T, BASE_CAPACITY>::operator[](size_t i) const {
  MARL_ASSERT(i < count, "index %d exceeds vector size %d", int(i), int(count));
  return reinterpret_cast<T*>(elements)[i];
}

template <typename T, int BASE_CAPACITY>
size_t vector<T, BASE_CAPACITY>::size() const {
  return count;
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::resize(size_t n) {
  reserve(n);
  while (count < n) {
    new (&reinterpret_cast<T*>(elements)[count++]) T();
  }
  while (n < count) {
    reinterpret_cast<T*>(elements)[--count].~T();
  }
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::reserve(size_t n) {
  if (n > capacity) {
    capacity = std::max<size_t>(n * 2, 8);

    Allocation::Request request;
    request.size = sizeof(T) * capacity;
    request.alignment = alignof(T);
    request.usage = Allocation::Usage::Vector;

    auto alloc = allocator->allocate(request);
    auto grown = reinterpret_cast<TStorage*>(alloc.ptr);
    for (size_t i = 0; i < count; i++) {
      new (&reinterpret_cast<T*>(grown)[i])
          T(std::move(reinterpret_cast<T*>(elements)[i]));
    }
    free();
    elements = grown;
    allocation = alloc;
  }
}

template <typename T, int BASE_CAPACITY>
void vector<T, BASE_CAPACITY>::free() {
  for (size_t i = 0; i < count; i++) {
    reinterpret_cast<T*>(elements)[i].~T();
  }

  if (allocation.ptr != nullptr) {
    allocator->free(allocation);
    elements = nullptr;
  }
}

}  // namespace containers
}  // namespace marl

#endif  // marl_containers_h
