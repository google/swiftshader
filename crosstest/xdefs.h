//===- subzero/crosstest/xdefs.h - Definitions for the crosstests. --------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Defines the int64 and uint64 types to avoid link-time errors when compiling
// the crosstests in LP64.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_CROSSTEST_XDEFS_H_
#define SUBZERO_CROSSTEST_XDEFS_H_

typedef unsigned int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int SizeT;

#ifdef X8664_STACK_HACK

// the X86_STACK_HACK is an intrusive way of getting the crosstests to run in
// x86_64 LP64 even with an ILP32 model. This hack allocates a new stack for
// running the tests in the low 4GB of the address space.

#ifdef __cplusplus
#define XTEST_EXTERN extern "C"
#else // !defined(__cplusplus)
#define XTEST_EXTERN extern
#endif // __cplusplus

/// xAllocStack allocates the memory chunk [StackEnd - Size - 1, StackEnd). It
/// requires StackEnd to be less than 32-bits long. Conversely, xDeallocStack
/// frees that memory chunk.
/// {@
XTEST_EXTERN unsigned char *xAllocStack(uint64 StackEnd, uint32 Size);
XTEST_EXTERN void xDeallocStack(uint64 StackEnd, uint32 Size);
/// @}

// wrapped_main is invoked by the x86-64 stack hack main. We declare a prototype
// so the compiler (and not the linker) can yell if a test's wrapped_main
// prototype does not match what we want.
XTEST_EXTERN int wrapped_main(int argc, char *argv[]);

#undef XTEST_EXTERN

#endif // X8664_STACK_HACK

#endif // SUBZERO_CROSSTEST_XDEFS_H_
