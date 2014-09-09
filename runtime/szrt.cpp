//===- subzero/runtime/szrt.cpp - Subzero runtime source ------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a C++ wrapper to szrt.c since clang++ complains about
// .c files.
//
//===----------------------------------------------------------------------===//

extern "C" {
#include "szrt.c"
}
