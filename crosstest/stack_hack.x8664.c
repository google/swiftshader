//===- subzero/crosstest/stack_hack.x8664.c - X8664 stack hack ------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements main() for crosstests in x86-64.
//
//===----------------------------------------------------------------------===//
#include <assert.h>
#include <stdint.h>

#include <sys/mman.h>

// X8664_STACK_HACK needs to be defined before xdefs.h is included.
#define X8664_STACK_HACK
#include "xdefs.h"

/// xSetStack is used to set %rsp to NewRsp. OldRsp is a pointer that will be
/// used to save the old %rsp value.
#define xSetStack(NewRsp, OldRsp)                                              \
  do {                                                                         \
    __asm__ volatile("xchgq   %1, %%rsp\n\t"                                   \
                     "xchgq   %1, %0"                                          \
                     : "=r"(*(OldRsp))                                         \
                     : "r"(NewRsp));                                           \
  } while (0)

extern int wrapped_main(int argc, char *argv[]);

unsigned char *xStackStart(uint32 StackEnd, uint32 Size) {
  const uint32 PageBoundary = 4 << 20; // 4 MB.
  const uint64 StackStart = StackEnd - Size;
  assert(StackStart + (PageBoundary - 1) & ~(PageBoundary - 1) &&
         "StackStart not aligned to page boundary.");
  (void)PageBoundary;
  assert((StackStart & 0xFFFFFFFF00000000ull) == 0 && "StackStart wraps.");
  return (unsigned char *)StackStart;
}

unsigned char *xAllocStack(uint64 StackEnd, uint32 Size) {
  assert((StackEnd & 0xFFFFFFFF00000000ull) == 0 && "Invalid StackEnd.");
  void *Stack =
      mmap(xStackStart(StackEnd, Size), Size, PROT_READ | PROT_WRITE,
           MAP_FIXED | MAP_PRIVATE | MAP_GROWSDOWN | MAP_ANONYMOUS, -1, 0);
  assert(Stack != MAP_FAILED && "mmap failed. no stack.");
  return Stack;
}

void xDeallocStack(uint64 StackEnd, uint32 Size) {
  assert((StackEnd & 0xFFFFFFFF00000000ull) == 0 && "Invalid StackEnd.");
  munmap(xStackStart(StackEnd, Size), Size);
}

int main(int argc, char *argv[]) {
  // These "locals" need to live **NOT** in the stack.
  static int Argc;
  static char **Argv;
  static const uint32_t StackEnd = 0x80000000;
  static const uint32_t StackSize = 40 * 1024 * 1024;
  static unsigned char *new_rsp;
  static unsigned char *old_rsp;
  static unsigned char *dummy_rsp;
  static int Failures;
  Argc = argc;
  Argv = argv;
  new_rsp = xAllocStack(StackEnd, StackSize) + StackSize;
  xSetStack(new_rsp, &old_rsp);
  Failures = wrapped_main(Argc, Argv);
  xSetStack(old_rsp, &new_rsp);
  xDeallocStack(StackEnd, StackSize);
  return Failures;
}
