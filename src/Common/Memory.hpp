// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef Memory_hpp
#define Memory_hpp

#include <stddef.h>

size_t memoryPageSize();

void *allocate(size_t bytes, size_t alignment = 16);
void *allocateZero(size_t bytes, size_t alignment = 16);
void deallocate(void *memory);

void *allocateExecutable(size_t bytes);   // Allocates memory that can be made executable using markExecutable()
void markExecutable(void *memory, size_t bytes);
void deallocateExecutable(void *memory, size_t bytes);

void *allocate(size_t bytes, const char *function);
void *allocateZero(size_t bytes, const char *function);
void deallocate(void *memory, const char *function);

#ifndef NDEBUG
	#define allocate(bytes) allocate((bytes), __FUNCTION__)
	#define allocateZero(bytes) allocateZero((bytes), __FUNCTION__)
	#define deallocate(memory) deallocate((memory), __FUNCTION__)
#endif

#endif   // Memory_hpp
