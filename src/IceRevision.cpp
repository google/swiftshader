//===- subzero/src/IceRevision.cpp - Revision string embedding ------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the function for returning the Subzero revision string.
///
//===----------------------------------------------------------------------===//

#include "IceRevision.h"

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x

#ifndef SUBZERO_REVISION
#define SUBZERO_REVISION unknown
#endif // !SUBZERO_REVISION

namespace Ice {
const char *getSubzeroRevision() {
  return "Subzero_revision_" XSTRINGIFY(SUBZERO_REVISION);
}
} // end of namespace Ice
