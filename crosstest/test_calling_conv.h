//===- subzero/crosstest/test_calling_conv.h - Test prototypes --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the function prototypes for crosstesting the calling
// convention.
//
//===----------------------------------------------------------------------===//

#include "test_calling_conv.def"
#include "vectors.h"

typedef void (*CalleePtrTy)();
extern CalleePtrTy Callee;
extern size_t ArgNum;
extern char *Buf;

void caller_i();
void caller_alloca_i();
typedef void callee_i_Ty(int);
callee_i_Ty callee_i;
callee_i_Ty callee_alloca_i;

void caller_vvvvv();
typedef void (callee_vvvvv_Ty)(v4si32, v4si32, v4si32, v4si32, v4si32);
callee_vvvvv_Ty callee_vvvvv;

void caller_vlvlivfvdviv();
typedef void(callee_vlvlivfvdviv_Ty)(v4f32, int64_t, v4f32, int64_t, int, v4f32,
                                    float, v4f32, double, v4f32, int, v4f32);
callee_vlvlivfvdviv_Ty callee_vlvlivfvdviv;
