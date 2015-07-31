//===- subzero/src/IceInstX8664.h - x86-64 machine instructions -*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file used to house all the X8664 instructions. Subzero has been
/// modified to use templates for X86 instructions, so all those definitions are
/// are in IceInstX86Base.h
///
/// When interacting with the X8664 target (which should only happen in the
/// X8664 TargetLowering) clients have should use the Ice::X8664::Traits::Insts
/// traits, which hides all the template verboseness behind a type alias.
///
/// For example, to create an X8664 MOV Instruction, clients should do
///
/// ::Ice::X8664::Traits::Insts::Mov::create
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTX8664_H
#define SUBZERO_SRC_ICEINSTX8664_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstX86Base.h"
#include "IceOperand.h"
#include "IceTargetLoweringX8664Traits.h"

#endif // SUBZERO_SRC_ICEINSTX8664_H
