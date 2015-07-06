//===- subzero/src/IceTargetLoweringX8664.cpp - lowering for x86-64 -------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implements the Target Lowering for x86-64.
///
//===----------------------------------------------------------------------===//

#include "IceDefs.h"
#include "IceTargetLoweringX8664.h"

namespace Ice {

TargetX8664 *TargetX8664::create(Cfg *) {
  llvm::report_fatal_error("Not yet implemented");
}
void TargetDataX8664::lowerGlobals(const VariableDeclarationList &,
                                   const IceString &) {
  llvm::report_fatal_error("Not yet implemented");
}

void TargetDataX8664::lowerConstants() {
  llvm::report_fatal_error("Not yet implemented");
}

} // end of namespace Ice
