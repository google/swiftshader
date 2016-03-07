//===- subzero/src/IceMangling.h - Name mangling for crosstests -*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares utility functions for name mangling for cross tests.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEMANGLING_H
#define SUBZERO_SRC_ICEMANGLING_H

#include "IceDefs.h"

namespace Ice {

IceString mangleName(const IceString &Name);

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEMANGLING_H
