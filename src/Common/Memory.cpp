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

#include "Memory.hpp"

#include "Types.hpp"
#include "Debug.hpp"

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <intrin.h>
#else
	#include <errno.h>
	#include <sys/mman.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

#include <memory.h>

#undef allocate
#undef deallocate

#if (defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined (_M_X64)) && !defined(__x86__)
#define __x86__
#endif

namespace sw
{
namespace
{
struct Allocation
{
//	size_t bytes;
	unsigned char *block;
};

void *allocateRaw(size_t bytes, size_t alignment)
{
	ASSERT((alignment & (alignment - 1)) == 0);   // Power of 2 alignment.

	#if defined(LINUX_ENABLE_NAMED_MMAP)
		if(alignment < sizeof(void*))
		{
			return malloc(bytes);
		}
		else
		{
			void *allocation;
			int result = posix_memalign(&allocation, alignment, bytes);
			if(result != 0)
			{
				errno = result;
				allocation = nullptr;
			}
			return allocation;
		}
	#else
		unsigned char *block = new unsigned char[bytes + sizeof(Allocation) + alignment];
		unsigned char *aligned = nullptr;

		if(block)
		{
			aligned = (unsigned char*)((uintptr_t)(block + sizeof(Allocation) + alignment - 1) & -(intptr_t)alignment);
			Allocation *allocation = (Allocation*)(aligned - sizeof(Allocation));

		//	allocation->bytes = bytes;
			allocation->block = block;
		}

		return aligned;
	#endif
}
}  // anonymous namespace

size_t memoryPageSize()
{
	static int pageSize = 0;

	if(pageSize == 0)
	{
		#if defined(_WIN32)
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);
			pageSize = systemInfo.dwPageSize;
		#else
			pageSize = sysconf(_SC_PAGESIZE);
		#endif
	}

	return pageSize;
}

void *allocate(size_t bytes, size_t alignment)
{
	void *memory = allocateRaw(bytes, alignment);

	if(memory)
	{
		memset(memory, 0, bytes);
	}

	return memory;
}

void deallocate(void *memory)
{
	#if defined(LINUX_ENABLE_NAMED_MMAP)
		free(memory);
	#else
		if(memory)
		{
			unsigned char *aligned = (unsigned char*)memory;
			Allocation *allocation = (Allocation*)(aligned - sizeof(Allocation));

			delete[] allocation->block;
		}
	#endif
}

void clear(uint16_t *memory, uint16_t element, size_t count)
{
	#if defined(_MSC_VER) && defined(__x86__) && !defined(MEMORY_SANITIZER)
		__stosw(memory, element, count);
	#elif defined(__GNUC__) && defined(__x86__) && !defined(MEMORY_SANITIZER)
		__asm__ __volatile__("rep stosw" : "+D"(memory), "+c"(count) : "a"(element) : "memory");
	#else
		for(size_t i = 0; i < count; i++)
		{
			memory[i] = element;
		}
	#endif
}

void clear(uint32_t *memory, uint32_t element, size_t count)
{
	#if defined(_MSC_VER) && defined(__x86__) && !defined(MEMORY_SANITIZER)
		__stosd((unsigned long*)memory, element, count);
	#elif defined(__GNUC__) && defined(__x86__) && !defined(MEMORY_SANITIZER)
		__asm__ __volatile__("rep stosl" : "+D"(memory), "+c"(count) : "a"(element) : "memory");
	#else
		for(size_t i = 0; i < count; i++)
		{
			memory[i] = element;
		}
	#endif
}

}
