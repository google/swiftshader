//===- subzero/src/IceFixups.h - Assembler fixup kinds ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares generic fixup types.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEFIXUPS_H
#define SUBZERO_SRC_ICEFIXUPS_H

#include "IceTypes.def"

namespace Ice {

enum FixupKind {
  // Specify some of the most common relocation types.
  FK_Abs_4 = 0,
  FK_PcRel_4 = 1,

  // Target specific relocation types follow this.
  FK_FirstTargetSpecific = 1 << 4
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEFIXUPS_H
