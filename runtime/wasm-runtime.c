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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char WASM_MEMORY[];

void env$$abort() {
  fprintf(stderr, "Aborting...\n");
  abort();
}

void env$$_abort() { env$$abort(); }

#define UNIMPLEMENTED(f)                                                       \
  void env$$##f() {                                                            \
    fprintf(stderr, "Unimplemented: " #f "\n");                                \
    abort();                                                                   \
  }

UNIMPLEMENTED(sbrk)
UNIMPLEMENTED(pthread_cleanup_push)
UNIMPLEMENTED(pthread_cleanup_pop)
UNIMPLEMENTED(pthread_self)
UNIMPLEMENTED(__lock)
UNIMPLEMENTED(__unlock)
UNIMPLEMENTED(__syscall6)
UNIMPLEMENTED(__syscall140)

void *wasmPtr(int Index) {
  // TODO (eholk): get the mask from the WASM file.
  const int MASK = 0x3fffff;
  Index &= MASK;

  return WASM_MEMORY + Index;
}

extern int __szwasm_main(int, char **);

#define WASM_REF(Type, Index) ((Type *)wasmPtr(Index))
#define WASM_DEREF(Type, Index) (*WASM_REF(Type, Index))

int main(int argc, char **argv) { return __szwasm_main(argc, argv); }

/// sys_write
int env$$__syscall4(int Which, int VarArgs) {
  int Fd = WASM_DEREF(int, VarArgs + 0 * sizeof(int));
  int Buffer = WASM_DEREF(int, VarArgs + 1 * sizeof(int));
  int Length = WASM_DEREF(int, VarArgs + 2 * sizeof(int));

  return write(Fd, WASM_REF(char *, Buffer), Length);
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
