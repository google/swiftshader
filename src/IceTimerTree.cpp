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

#include "llvm/Support/Timer.h"

#include "IceDefs.h"
#include "IceTimerTree.h"

namespace Ice {

TimerStack::TimerStack(const IceString &Name)
    : Name(Name), FirstTimestamp(timestamp()), LastTimestamp(FirstTimestamp),
      StateChangeCount(0), StackTop(0) {
  Nodes.resize(1); // Reserve Nodes[0] for the root node.
  IDs.resize(TT__num);
#define STR(s) #s
#define X(tag)                                                                 \
  IDs[TT_##tag] = STR(tag);                                                    \
  IDsIndex[STR(tag)] = TT_##tag;
  TIMERTREE_TABLE;
#undef X
#undef STR
}

// Returns the unique timer ID for the given Name, creating a new ID
// if needed.
TimerIdT TimerStack::getTimerID(const IceString &Name) {
  if (IDsIndex.find(Name) == IDsIndex.end()) {
    IDsIndex[Name] = IDs.size();
    IDs.push_back(Name);
  }
  return IDsIndex[Name];
}

// Pushes a new marker onto the timer stack.
void TimerStack::push(TimerIdT ID) {
  const bool UpdateCounts = false;
  update(UpdateCounts);
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
  const bool UpdateCounts = true;
  update(UpdateCounts);
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
void TimerStack::update(bool UpdateCounts) {
  ++StateChangeCount;
  // Whenever the stack is about to change, we grab the time delta
  // since the last change and add it to all active cumulative
  // elements and to the flat element for the top of the stack.
  double Current = timestamp();
  double Delta = Current - LastTimestamp;
  if (StackTop) {
    TimerIdT Leaf = Nodes[StackTop].Interior;
    if (Leaf >= LeafTimes.size()) {
      LeafTimes.resize(Leaf + 1);
      LeafCounts.resize(Leaf + 1);
    }
    LeafTimes[Leaf] += Delta;
    if (UpdateCounts)
      ++LeafCounts[Leaf];
  }
  TTindex Prefix = StackTop;
  while (Prefix) {
    Nodes[Prefix].Time += Delta;
    // Only update a leaf node count, not the internal node counts.
    if (UpdateCounts && Prefix == StackTop)
      ++Nodes[Prefix].UpdateCount;
    TTindex Next = Nodes[Prefix].Parent;
    assert(Next < Prefix);
    Prefix = Next;
  }
  // Capture the next timestamp *after* the updates are finished.
  // This minimizes how much the timer can perturb the reported
  // timing.  The numbers may not sum to 100%, and the missing amount
  // is indicative of the overhead of timing.
  LastTimestamp = timestamp();
}

void TimerStack::reset() {
  StateChangeCount = 0;
  FirstTimestamp = LastTimestamp = timestamp();
  LeafTimes.assign(LeafTimes.size(), 0);
  LeafCounts.assign(LeafCounts.size(), 0);
  for (TimerTreeNode &Node : Nodes) {
    Node.Time = 0;
    Node.UpdateCount = 0;
  }
}

namespace {

typedef std::multimap<double, IceString> DumpMapType;

// Dump the Map items in reverse order of their time contribution.
void dumpHelper(Ostream &Str, const DumpMapType &Map, double TotalTime) {
  if (!ALLOW_DUMP)
    return;
  // TODO(stichnot): Use llvm::make_range with LLVM 3.5.
  for (auto I = Map.rbegin(), E = Map.rend(); I != E; ++I) {
    char buf[80];
    snprintf(buf, llvm::array_lengthof(buf), "  %10.6f (%4.1f%%): ", I->first,
             I->first * 100 / TotalTime);
    Str << buf << I->second << "\n";
  }
}

// Write a printf() format string into Buf[], in the format "[%5lu] ",
// where "5" is actually the number of digits in MaxVal.  E.g.,
//   MaxVal=0     ==> "[%1lu] "
//   MaxVal=5     ==> "[%1lu] "
//   MaxVal=9876  ==> "[%4lu] "
void makePrintfFormatString(char *Buf, size_t BufLen, size_t MaxVal) {
  if (!ALLOW_DUMP)
    return;
  int NumDigits = 0;
  do {
    ++NumDigits;
    MaxVal /= 10;
  } while (MaxVal);
  snprintf(Buf, BufLen, "[%%%dlu] ", NumDigits);
}

} // end of anonymous namespace

void TimerStack::dump(Ostream &Str, bool DumpCumulative) {
  if (!ALLOW_DUMP)
    return;
  const bool UpdateCounts = true;
  update(UpdateCounts);
  double TotalTime = LastTimestamp - FirstTimestamp;
  assert(TotalTime);
  char FmtString[30], PrefixStr[30];
  if (DumpCumulative) {
    Str << Name << " - Cumulative times:\n";
    size_t MaxInternalCount = 0;
    for (TimerTreeNode &Node : Nodes)
      MaxInternalCount = std::max(MaxInternalCount, Node.UpdateCount);
    makePrintfFormatString(FmtString, llvm::array_lengthof(FmtString),
                           MaxInternalCount);

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
      snprintf(PrefixStr, llvm::array_lengthof(PrefixStr), FmtString,
               Nodes[i].UpdateCount);
      CumulativeMap.insert(std::make_pair(Nodes[i].Time, PrefixStr + Suffix));
    }
    dumpHelper(Str, CumulativeMap, TotalTime);
  }
  Str << Name << " - Flat times:\n";
  size_t MaxLeafCount = 0;
  for (size_t Count : LeafCounts)
    MaxLeafCount = std::max(MaxLeafCount, Count);
  makePrintfFormatString(FmtString, llvm::array_lengthof(FmtString),
                         MaxLeafCount);
  DumpMapType FlatMap;
  for (TimerIdT i = 0; i < LeafTimes.size(); ++i) {
    if (LeafCounts[i]) {
      snprintf(PrefixStr, llvm::array_lengthof(PrefixStr), FmtString,
               LeafCounts[i]);
      FlatMap.insert(std::make_pair(LeafTimes[i], PrefixStr + IDs[i]));
    }
  }
  dumpHelper(Str, FlatMap, TotalTime);
  Str << "Number of timer updates: " << StateChangeCount << "\n";
}

double TimerStack::timestamp() {
  // TODO: Implement in terms of std::chrono for C++11.
  return llvm::TimeRecord::getCurrentTime(false).getWallTime();
}

} // end of namespace Ice
