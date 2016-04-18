//===- subzero/runtime/wasm-runtime.c - Subzero WASM runtime source -------===//
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern char WASM_MEMORY[];

void env$$abort() {
  fprintf(stderr, "Aborting...\n");
  abort();
}

void env$$_abort() { env$$abort(); }

void env$$exit(int Status) { exit(Status); }
void env$$_exit(int Status) { env$$exit(Status); }

#define UNIMPLEMENTED(f)                                                       \
  void env$$##f() {                                                            \
    fprintf(stderr, "Unimplemented: " #f "\n");                                \
    abort();                                                                   \
  }

UNIMPLEMENTED(sbrk)
UNIMPLEMENTED(setjmp)
UNIMPLEMENTED(longjmp)
UNIMPLEMENTED(__assert_fail)
UNIMPLEMENTED(__builtin_malloc)
UNIMPLEMENTED(__builtin_apply)
UNIMPLEMENTED(__builtin_apply_args)
UNIMPLEMENTED(pthread_cleanup_push)
UNIMPLEMENTED(pthread_cleanup_pop)
UNIMPLEMENTED(pthread_self)
UNIMPLEMENTED(abs)
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
UNIMPLEMENTED(__syscall6)
UNIMPLEMENTED(__syscall20)
UNIMPLEMENTED(__syscall140)
UNIMPLEMENTED(__syscall192)

void *wasmPtr(int Index) {
  // TODO (eholk): get the mask from the WASM file.
  const int MASK = 0x3fffff;
  Index &= MASK;

  return WASM_MEMORY + Index;
}

extern int __szwasm_main(int, const char **);

#define WASM_REF(Type, Index) ((Type *)wasmPtr(Index))
#define WASM_DEREF(Type, Index) (*WASM_REF(Type, Index))

int main(int argc, const char **argv) { return __szwasm_main(argc, argv); }

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

/// sys_ioctl
int env$$__syscall54(int A, int B) {
  int Fd = WASM_DEREF(int, B + 0 * sizeof(int));
  int Op = WASM_DEREF(int, B + 1 * sizeof(int));
  int ArgP = WASM_DEREF(int, B + 2 * sizeof(int));
  // TODO (eholk): implement sys_ioctl
  return -ENOTTY;
}

int env$$__syscall146(int Which, int VarArgs) {
  fprintf(stderr, "syscall146\n");

  int Fd = WASM_DEREF(int, VarArgs);
  int Iov = WASM_DEREF(int, VarArgs + sizeof(int));
  int Iovcnt = WASM_DEREF(int, VarArgs + 2 * sizeof(int));

  fprintf(stderr, "  Fd=%d, Iov=%d (%p), Iovcnt=%d\n", Fd, Iov, wasmPtr(Iov),
          Iovcnt);

  int Count = 0;

  for (int I = 0; I < Iovcnt; ++I) {
    void *Ptr = WASM_REF(void, WASM_DEREF(int, Iov + I * 8));
    int Length = WASM_DEREF(int, Iov + I * 8 + 4);

    fprintf(stderr, "  [%d] write(%d, %p, %d) = ", I, Fd, Ptr, Length);
    int Curr = write(Fd, Ptr, Length);

    fprintf(stderr, "%d\n", Curr);

    if (Curr < 0) {
      return -1;
    }
    Count += Curr;
  }
  fprintf(stderr, "  Count = %d\n", Count);
  return Count;
}
