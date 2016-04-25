//===- subzero/runtime/wasm-runtime.cpp - Subzero WASM runtime source -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the system calls required by the libc that is included
// in WebAssembly programs.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>
// TODO (eholk): change to cstdint
#include <stdint.h>

namespace {
uint32_t HeapBreak;

// TODO (eholk): make all of these constexpr.
const uint32_t PageSizeLog2 = 16;
const uint32_t PageSize = 1 << PageSizeLog2; // 64KB
const uint32_t StackPtrLoc = 1024;           // defined by emscripten

uint32_t pageNum(uint32_t Index) { return Index >> PageSizeLog2; }
} // end of anonymous namespace

namespace env {
double floor(double X) { return std::floor(X); }

float floor(float X) { return std::floor(X); }
} // end of namespace env

// TODO (eholk): move the C parts outside and use C++ name mangling.
extern "C" {
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern char WASM_MEMORY[];
extern uint32_t WASM_DATA_SIZE;
extern uint32_t WASM_NUM_PAGES;

void env$$abort() {
  fprintf(stderr, "Aborting...\n");
  abort();
}

void env$$_abort() { env$$abort(); }

double env$$floor_f(float X) { return env::floor(X); }
double env$$floor_d(double X) { return env::floor(X); }

void env$$exit(int Status) { exit(Status); }
void env$$_exit(int Status) { env$$exit(Status); }

#define UNIMPLEMENTED(f)                                                       \
  void env$$##f() {                                                            \
    fprintf(stderr, "Unimplemented: " #f "\n");                                \
    abort();                                                                   \
  }

int32_t env$$sbrk(int32_t Increment) {
  HeapBreak += Increment;
  return HeapBreak;
}

UNIMPLEMENTED(setjmp)
UNIMPLEMENTED(longjmp)
UNIMPLEMENTED(__assert_fail)
UNIMPLEMENTED(__builtin_malloc)
UNIMPLEMENTED(__builtin_isinff)
UNIMPLEMENTED(__builtin_isinfl)
UNIMPLEMENTED(__builtin_apply)
UNIMPLEMENTED(__builtin_apply_args)
UNIMPLEMENTED(pthread_cleanup_push)
UNIMPLEMENTED(pthread_cleanup_pop)
UNIMPLEMENTED(pthread_self)
UNIMPLEMENTED(__floatditf)
UNIMPLEMENTED(__floatsitf)
UNIMPLEMENTED(__fixtfdi)
UNIMPLEMENTED(__fixtfsi)
UNIMPLEMENTED(__fixsfti)
UNIMPLEMENTED(__netf2)
UNIMPLEMENTED(__getf2)
UNIMPLEMENTED(__eqtf2)
UNIMPLEMENTED(__lttf2)
UNIMPLEMENTED(__addtf3)
UNIMPLEMENTED(__subtf3)
UNIMPLEMENTED(__divtf3)
UNIMPLEMENTED(__multf3)
UNIMPLEMENTED(__multi3)
UNIMPLEMENTED(__lock)
UNIMPLEMENTED(__unlock)
UNIMPLEMENTED(__syscall6)   // sys_close
UNIMPLEMENTED(__syscall140) // sys_llseek
UNIMPLEMENTED(__unordtf2)
UNIMPLEMENTED(__fixunstfsi)
UNIMPLEMENTED(__floatunsitf)
UNIMPLEMENTED(__extenddftf2)

void *wasmPtr(uint32_t Index) {
  if (pageNum(Index) < WASM_NUM_PAGES) {
    return WASM_MEMORY + Index;
  }
  abort();
}

extern int __szwasm_main(int, const char **);

#define WASM_REF(Type, Index) ((Type *)wasmPtr(Index))
#define WASM_DEREF(Type, Index) (*WASM_REF(Type, Index))

int main(int argc, const char **argv) {
  // Initialize the break to the nearest page boundary after the data segment
  HeapBreak = (WASM_DATA_SIZE + PageSize - 1) & ~(PageSize - 1);

  // Initialize the stack pointer.
  WASM_DEREF(int32_t, StackPtrLoc) = WASM_NUM_PAGES << PageSizeLog2;

  return __szwasm_main(argc, argv);
}

int env$$abs(int a) { return abs(a); }

double env$$pow(double x, double y) { return pow(x, y); }

/// sys_write
int env$$__syscall4(int Which, int VarArgs) {
  int Fd = WASM_DEREF(int, VarArgs + 0 * sizeof(int));
  int Buffer = WASM_DEREF(int, VarArgs + 1 * sizeof(int));
  int Length = WASM_DEREF(int, VarArgs + 2 * sizeof(int));

  return write(Fd, WASM_REF(char *, Buffer), Length);
}

/// sys_open
int env$$__syscall5(int Which, int VarArgs) {
  int WasmPath = WASM_DEREF(int, VarArgs);
  int Flags = WASM_DEREF(int, VarArgs + 4);
  int Mode = WASM_DEREF(int, VarArgs + 8);
  const char *Path = WASM_REF(char, WasmPath);

  fprintf(stderr, "sys_open(%s, %d, %d)\n", Path, Flags, Mode);

  return open(Path, Flags, Mode);
}

/// sys_getpid
int env$$__syscall20(int Which, int VarArgs) {
  (void)Which;
  (void)VarArgs;

  return getpid();
}

/// sys_ioctl
int env$$__syscall54(int A, int B) {
  int Fd = WASM_DEREF(int, B + 0 * sizeof(int));
  int Op = WASM_DEREF(int, B + 1 * sizeof(int));
  int ArgP = WASM_DEREF(int, B + 2 * sizeof(int));
  // TODO (eholk): implement sys_ioctl
  return -ENOTTY;
}

/// sys_write
int env$$__syscall146(int Which, int VarArgs) {

  int Fd = WASM_DEREF(int, VarArgs);
  int Iov = WASM_DEREF(int, VarArgs + sizeof(int));
  int Iovcnt = WASM_DEREF(int, VarArgs + 2 * sizeof(int));

  int Count = 0;

  for (int I = 0; I < Iovcnt; ++I) {
    void *Ptr = WASM_REF(void, WASM_DEREF(int, Iov + I * 8));
    int Length = WASM_DEREF(int, Iov + I * 8 + 4);

    int Curr = write(Fd, Ptr, Length);

    if (Curr < 0) {
      return -1;
    }
    Count += Curr;
  }
  return Count;
}

/// sys_mmap_pgoff
int env$$__syscall192(int Which, int VarArgs) {
  (void)Which;
  (void)VarArgs;

  // TODO (eholk): figure out how to implement this.

  return -ENOMEM;
}
} // end of extern "C"
