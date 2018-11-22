// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef rr_ExecutableMemory_hpp
#define rr_ExecutableMemory_hpp

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr
{
size_t memoryPageSize();

void *allocateExecutable(size_t bytes);   // Allocates memory that can be made executable using markExecutable()
void markExecutable(void *memory, size_t bytes);
void deallocateExecutable(void *memory, size_t bytes);

template<typename P>
P unaligned_read(P *address)
{
	P value;
	memcpy(&value, address, sizeof(P));
	return value;
}

template<typename P, typename V>
void unaligned_write(P *address, V value)
{
	static_assert(sizeof(V) == sizeof(P), "value size must match pointee size");
	memcpy(address, &value, sizeof(P));
}

template<typename P>
class unaligned_ref
{
public:
	explicit unaligned_ref(void *ptr) : ptr((P*)ptr) {}

	template<typename V>
	P operator=(V value)
	{
		unaligned_write(ptr, value);
		return value;
	}

	operator P()
	{
		return unaligned_read((P*)ptr);
	}

private:
	P *ptr;
};

template<typename P>
class unaligned_ptr
{
	template<typename S>
	friend class unaligned_ptr;

public:
	unaligned_ptr(P *ptr) : ptr(ptr) {}

	unaligned_ref<P> operator*()
	{
		return unaligned_ref<P>(ptr);
	}

	template<typename S>
	operator S()
	{
		return S(ptr);
	}

private:
	void *ptr;
};
}

#endif   // rr_ExecutableMemory_hpp
