//===- subzero/src/IceTimerTree.h - Pass timer defs -------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TimerTree class, which allows flat and
// cumulative execution time collection of call chains.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETIMERTREE_H
#define SUBZERO_SRC_ICETIMERTREE_H

#include "IceTimerTree.def"

namespace Ice {

class TimerStack {
  TimerStack &operator=(const TimerStack &) = delete;

  // Timer tree index type
  typedef std::vector<class TimerTreeNode>::size_type TTindex;

  // TimerTreeNode represents an interior or leaf node in the call tree.
  // It contains a list of children, a pointer to its parent, and the
  // timer ID for the node.  It also holds the cumulative time spent at
  // this node and below.  The children are always at a higher index in
  // the TimerTreeNode::Nodes array, and the parent is always at a lower
  // index.
  class TimerTreeNode {
    TimerTreeNode &operator=(const TimerTreeNode &) = delete;

  public:
    TimerTreeNode() : Parent(0), Interior(0), Time(0), UpdateCount(0) {}
    TimerTreeNode(const TimerTreeNode &) = default;
    std::vector<TTindex> Children; // indexed by TimerIdT
    TTindex Parent;
    TimerIdT Interior;
    double Time;
    size_t UpdateCount;
  };

public:
  enum TimerTag {
#define X(tag) TT_##tag,
    TIMERTREE_TABLE
#undef X
        TT__num
  };
  TimerStack(const IceString &Name);
  TimerStack(const TimerStack &) = default;
  TimerIdT getTimerID(const IceString &Name);
  void setName(const IceString &NewName) { Name = NewName; }
  void push(TimerIdT ID);
  void pop(TimerIdT ID);
  void reset();
  void dump(Ostream &Str, bool DumpCumulative);

private:
  void update(bool UpdateCounts);
  static double timestamp();
  IceString Name;
  double FirstTimestamp;
  double LastTimestamp;
  uint64_t StateChangeCount;
  // IDsIndex maps a symbolic timer name to its integer ID.
  std::map<IceString, TimerIdT> IDsIndex;
  std::vector<IceString> IDs;        // indexed by TimerIdT
  std::vector<TimerTreeNode> Nodes;  // indexed by TTindex
  std::vector<double> LeafTimes;     // indexed by TimerIdT
  std::vector<size_t> LeafCounts;    // indexed by TimerIdT
  TTindex StackTop;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETIMERTREE_H
