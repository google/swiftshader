//===------ subzero/src/IcePhiLoweringImpl.h - Phi lowering -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This contains utilities for targets to lower Phis.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEPHILOWERINGIMPL_H
#define SUBZERO_SRC_ICEPHILOWERINGIMPL_H

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"

namespace Ice {
namespace PhiLowering {

// Turn an i64 Phi instruction into a pair of i32 Phi instructions, to
// preserve integrity of liveness analysis.  This is needed for 32-bit
// targets.  This assumes the 32-bit target has loOperand, hiOperand,
// and legalizeUndef methods.  Undef values are also legalized, since
// loOperand() and hiOperand() don't expect Undef input.
template <class TargetT>
void prelowerPhis32Bit(TargetT *Target, CfgNode *Node, Cfg *Func) {
  for (Inst &I : Node->getPhis()) {
    auto Phi = llvm::dyn_cast<InstPhi>(&I);
    if (Phi->isDeleted())
      continue;
    Variable *Dest = Phi->getDest();
    if (Dest->getType() == IceType_i64) {
      Variable *DestLo = llvm::cast<Variable>(Target->loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(Target->hiOperand(Dest));
      InstPhi *PhiLo = InstPhi::create(Func, Phi->getSrcSize(), DestLo);
      InstPhi *PhiHi = InstPhi::create(Func, Phi->getSrcSize(), DestHi);
      for (SizeT I = 0; I < Phi->getSrcSize(); ++I) {
        Operand *Src = Phi->getSrc(I);
        CfgNode *Label = Phi->getLabel(I);
        Src = Target->legalizeUndef(Src);
        PhiLo->addArgument(Target->loOperand(Src), Label);
        PhiHi->addArgument(Target->hiOperand(Src), Label);
      }
      Node->getPhis().push_back(PhiLo);
      Node->getPhis().push_back(PhiHi);
      Phi->setDeleted();
    }
  }
}

} // end of namespace PhiLowering
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEPHILOWERINGIMPL_H
