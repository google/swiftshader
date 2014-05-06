// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Memory.hpp"

#include "Debug.hpp"

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#elif defined(__APPLE__)
	#include <sys/mman.h>
#else
	#error Unimplemented platform
#endif

#include <malloc.h>

#undef allocate
#undef deallocate
#undef allocateZero
#undef deallocateZero

void *allocate(size_t bytes)
{
	return _aligned_malloc(bytes, 16);
}

void *allocateZero(size_t bytes)
{
	void *memory = _aligned_malloc(bytes, 16);

	memset(memory, 0, bytes);

	return memory;
}

void deallocate(void *memory)
{
	_aligned_free(memory);
}

void *allocateExecutable(size_t bytes)
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return _aligned_malloc(bytes, systemInfo.dwPageSize);
}

void markExecutable(void *memory, size_t bytes)
{
	#if defined(_WIN32)
		unsigned long oldProtection;
		VirtualProtect(memory, bytes, PAGE_EXECUTE_READ, &oldProtection);
	#elif defined(__APPLE__)
		mprotect(memory, bytes, PROT_READ | PROT_WRITE | PROT_EXEC);
	#else
		#error Unimplemented platform
	#endif
}

void deallocateExecutable(void *memory, size_t bytes)
{
	#if defined(_WIN32)
		unsigned long oldProtection;
		VirtualProtect(memory, bytes, PAGE_READWRITE, &oldProtection);
	#elif defined(__APPLE__)
		mprotect(memory, bytes, PROT_READ | PROT_WRITE);
	#else
		#error Unimplemented platform
	#endif

	_aligned_free(memory);
}

void *allocate(size_t bytes, const char *function)
{
	trace("[0x%0.8X]%s(...)\n", 0xFFFFFFFF, function);

	void *memory = allocate(bytes);

	trace("\t0x%0.8X = allocate(%d)\n", memory, bytes);

	return memory;
}

void *allocateZero(size_t bytes, const char *function)
{
	trace("[0x%0.8X]%s(...)\n", 0xFFFFFFFF, function);

	void *memory = allocateZero(bytes);

	trace("\t0x%0.8X = allocateZero(%d)\n", memory, bytes);

	return memory;
}

void deallocate(void *memory, const char *function)
{
	if(memory)
	{
		trace("[0x%0.8X]%s(...)\n", 0xFFFFFFFF, function);
		trace("\tdeallocate(0x%0.8X)\n", memory);

		deallocate(memory);
	}
}
