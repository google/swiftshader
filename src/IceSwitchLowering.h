//===- subzero/src/IceSwitchLowering.h - Switch lowering --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief The file contains helpers for switch lowering.
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICESWITCHLOWERING_H
#define SUBZERO_SRC_ICESWITCHLOWERING_H

#include "IceCfgNode.h"
#include "IceInst.h"

namespace Ice {

class CaseCluster;

typedef std::vector<CaseCluster, CfgLocalAllocator<CaseCluster>>
    CaseClusterArray;

/// A cluster of cases can be tested by a common method during switch lowering.
class CaseCluster {
  CaseCluster() = delete;

public:
  enum CaseClusterKind {
    Range,     /// Numerically adjacent case values with same target.
    JumpTable, /// Different targets and possibly sparse.
  };

  CaseCluster(const CaseCluster &) = default;
  CaseCluster &operator=(const CaseCluster &) = default;

  /// Create a cluster of a single case represented by a unitary range.
  CaseCluster(uint64_t Value, CfgNode *Label)
      : Kind(Range), Low(Value), High(Value), Label(Label) {}
  /// Create a case consisting of a jump table.
  CaseCluster(uint64_t Low, uint64_t High, InstJumpTable *JT)
      : Kind(JumpTable), Low(Low), High(High), JT(JT) {}

  CaseClusterKind getKind() const { return Kind; }
  uint64_t getLow() const { return Low; }
  uint64_t getHigh() const { return High; }
  CfgNode *getLabel() const {
    assert(Kind == Range);
    return Label;
  }
  InstJumpTable *getJumpTable() const {
    assert(Kind == JumpTable);
    return JT;
  }

  /// Discover cases which can be clustered together and return the clusters
  /// ordered by case value.
  static CaseClusterArray clusterizeSwitch(Cfg *Func, const InstSwitch *Inst);

private:
  CaseClusterKind Kind;
  uint64_t Low;
  uint64_t High;
  union {
    CfgNode *Label;    /// Target for a range.
    InstJumpTable *JT; /// Jump table targets.
  };

  /// Try and append a cluster returning whether or not it was successful.
  bool tryAppend(const CaseCluster &New);
};

} // end of namespace Ice

#endif //  SUBZERO_SRC_ICESWITCHLOWERING_H
