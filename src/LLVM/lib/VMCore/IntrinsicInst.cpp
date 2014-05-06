//===-- InstrinsicInst.cpp - Intrinsic Instruction Wrappers -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements methods that make it really easy to deal with intrinsic
// functions.
//
// All intrinsic function calls are instances of the call instruction, so these
// are all subclasses of the CallInst class.  Note that none of these classes
// has state or virtual methods, which is an important part of this gross/neat
// hack working.
// 
// In some cases, arguments to intrinsics need to be generic and are defined as
// type pointer to empty struct { }*.  To access the real item of interest the
// cast instruction needs to be stripped away. 
//
//===----------------------------------------------------------------------===//

#include "llvm/IntrinsicInst.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Metadata.h"
using namespace llvm;

static Value *CastOperand(Value *C) {
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(C))
    if (CE->isCast())
      return CE->getOperand(0);
  return NULL;
}
