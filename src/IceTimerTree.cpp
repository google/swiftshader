//===- subzero/src/IceTimerTree.cpp - Pass timer defs ---------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the TimerTree class, which tracks flat and
// cumulative execution time collection of call chains.
//
//===----------------------------------------------------------------------===//

#include "IceDefs.h"
#include "IceTimerTree.h"

namespace Ice {

std::vector<IceString> TimerStack::IDs;

TimerStack::TimerStack(const IceString &TopLevelName)
    : FirstTimestamp(timestamp()), LastTimestamp(FirstTimestamp),
      StateChangeCount(0), StackTop(0) {
  Nodes.resize(1); // Reserve Nodes[0] for the root node.
  push(getTimerID(TopLevelName));
}

// Returns the unique timer ID for the given Name, creating a new ID
// if needed.  For performance reasons, it's best to make only one
// call per Name and cache the result, e.g. via a static initializer.
TimerIdT TimerStack::getTimerID(const IceString &Name) {
  TimerIdT Size = IDs.size();
  for (TimerIdT i = 0; i < Size; ++i) {
    if (IDs[i] == Name)
      return i;
  }
  IDs.push_back(Name);
  return Size;
}

// Pushes a new marker onto the timer stack.
void TimerStack::push(TimerIdT ID) {
  update();
  if (Nodes[StackTop].Children.size() <= ID)
    Nodes[StackTop].Children.resize(ID + 1);
  if (Nodes[StackTop].Children[ID] == 0) {
    TTindex Size = Nodes.size();
    Nodes[StackTop].Children[ID] = Size;
    Nodes.resize(Size + 1);
    Nodes[Size].Parent = StackTop;
    Nodes[Size].Interior = ID;
  }
  StackTop = Nodes[StackTop].Children[ID];
}

// Pop the top marker from the timer stack.  Validates via assert()
// that the expected marker is popped.
void TimerStack::pop(TimerIdT ID) {
  update();
  assert(StackTop);
  assert(Nodes[StackTop].Parent < StackTop);
  // Verify that the expected ID is being popped.
  assert(Nodes[StackTop].Interior == ID);
  (void)ID;
  // Verify that the parent's child points to the current stack top.
  assert(Nodes[Nodes[StackTop].Parent].Children[ID] == StackTop);
  StackTop = Nodes[StackTop].Parent;
}

// At a state change (e.g. push or pop), updates the flat and
// cumulative timings for everything on the timer stack.
void TimerStack::update() {
  ++StateChangeCount;
  // Whenever the stack is about to change, we grab the time delta
  // since the last change and add it to all active cumulative
  // elements and to the flat element for the top of the stack.
  double Current = timestamp();
  double Delta = Current - LastTimestamp;
  LastTimestamp = Current;
  if (StackTop) {
    TimerIdT Leaf = Nodes[StackTop].Interior;
    if (Leaf >= LeafTimes.size())
      LeafTimes.resize(Leaf + 1);
    LeafTimes[Leaf] += Delta;
  }
  TTindex Prefix = StackTop;
  while (Prefix) {
    Nodes[Prefix].Time += Delta;
    TTindex Next = Nodes[Prefix].Parent;
    assert(Next < Prefix);
    Prefix = Next;
  }
}

namespace {

typedef std::multimap<double, IceString> DumpMapType;

// Dump the Map items in reverse order of their time contribution.
void dumpHelper(Ostream &Str, const DumpMapType &Map, double TotalTime) {
  for (DumpMapType::const_reverse_iterator I = Map.rbegin(), E = Map.rend();
       I != E; ++I) {
    char buf[80];
    snprintf(buf, llvm::array_lengthof(buf), "  %10.6f (%4.1f%%): ", I->first,
             I->first * 100 / TotalTime);
    Str << buf << I->second << "\n";
  }
}

} // end of anonymous namespace

void TimerStack::dump(Ostream &Str) {
  update();
  double TotalTime = LastTimestamp - FirstTimestamp;
  assert(TotalTime);
  Str << "Cumulative function times:\n";
  DumpMapType CumulativeMap;
  for (TTindex i = 1; i < Nodes.size(); ++i) {
    TTindex Prefix = i;
    IceString Suffix = "";
    while (Prefix) {
      if (Suffix.empty())
        Suffix = IDs[Nodes[Prefix].Interior];
      else
        Suffix = IDs[Nodes[Prefix].Interior] + "." + Suffix;
      assert(Nodes[Prefix].Parent < Prefix);
      Prefix = Nodes[Prefix].Parent;
    }
    CumulativeMap.insert(std::make_pair(Nodes[i].Time, Suffix));
  }
  dumpHelper(Str, CumulativeMap, TotalTime);
  Str << "Flat function times:\n";
  DumpMapType FlatMap;
  for (TimerIdT i = 0; i < LeafTimes.size(); ++i) {
    FlatMap.insert(std::make_pair(LeafTimes[i], IDs[i]));
  }
  dumpHelper(Str, FlatMap, TotalTime);
  Str << "Number of timer updates: " << StateChangeCount << "\n";
}

} // end of namespace Ice
