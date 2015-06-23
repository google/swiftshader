//===- subzero/src/IceAssemblerX8664.cpp ----------------------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Assembler class for x86-64.
//
//===----------------------------------------------------------------------===//

#include "IceAssemblerX8664.h"

namespace Ice {
namespace X8664 {

void AssemblerX8664::alignFunction() {
  llvm::report_fatal_error("Not yet implemented");
}

void AssemblerX8664::padWithNop(intptr_t) {
  llvm::report_fatal_error("Not yet implemented");
}

SizeT AssemblerX8664::getBundleAlignLog2Bytes() const {
  llvm::report_fatal_error("Not yet implemented");
}
const char *AssemblerX8664::getNonExecPadDirective() const {
  llvm::report_fatal_error("Not yet implemented");
}

llvm::ArrayRef<uint8_t> AssemblerX8664::getNonExecBundlePadding() const {
  llvm::report_fatal_error("Not yet implemented");
}

void AssemblerX8664::bindCfgNodeLabel(SizeT) {
  llvm::report_fatal_error("Not yet implemented");
}

bool AssemblerX8664::fixupIsPCRel(FixupKind) const {
  llvm::report_fatal_error("Not yet implemented");
}

} // namespace X8664
} // namespace Ice
