//===- subzero/src/IceUtils.h - Utility functions ---------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares some utility functions.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEUTILS_H
#define SUBZERO_SRC_ICEUTILS_H

#include <climits>
#include <condition_variable>

namespace Ice {

// Similar to bit_cast, but allows copying from types of unrelated
// sizes. This method was introduced to enable the strict aliasing
// optimizations of GCC 4.4. Basically, GCC mindlessly relies on
// obscure details in the C++ standard that make reinterpret_cast
// virtually useless.
template <class D, class S> inline D bit_copy(const S &source) {
  D destination;
  // This use of memcpy is safe: source and destination cannot overlap.
  memcpy(&destination, reinterpret_cast<const void *>(&source),
         sizeof(destination));
  return destination;
}

class Utils {
public:
  // Check whether an N-bit two's-complement representation can hold value.
  template <typename T> static inline bool IsInt(int N, T value) {
    assert((0 < N) &&
           (static_cast<unsigned int>(N) < (CHAR_BIT * sizeof(value))));
    T limit = static_cast<T>(1) << (N - 1);
    return (-limit <= value) && (value < limit);
  }

  template <typename T> static inline bool IsUint(int N, T value) {
    assert((0 < N) &&
           (static_cast<unsigned int>(N) < (CHAR_BIT * sizeof(value))));
    T limit = static_cast<T>(1) << N;
    return (0 <= value) && (value < limit);
  }

  template <typename T> static inline bool WouldOverflowAdd(T X, T Y) {
    return ((X > 0 && Y > 0 && (X > std::numeric_limits<T>::max() - Y)) ||
            (X < 0 && Y < 0 && (X < std::numeric_limits<T>::min() - Y)));
  }
};

// BoundedProducerConsumerQueue is a work queue that allows multiple
// producers and multiple consumers.  A producer adds entries using
// blockingPush(), and may block if the queue is "full".  A producer
// uses notifyEnd() to indicate that no more entries will be added.  A
// consumer removes an item using blockingPop(), which will return
// nullptr if notifyEnd() has been called and the queue is empty (it
// never returns nullptr if the queue contained any items).
//
// The MaxSize ctor arg controls the maximum size the queue can grow
// to (subject to a hard limit of MaxStaticSize-1).  The Sequential
// arg indicates purely sequential execution in which the single
// thread should never wait().
//
// Two condition variables are used in the implementation.
// GrewOrEnded signals a waiting worker that a producer has changed
// the state of the queue.  Shrunk signals a blocked producer that a
// consumer has changed the state of the queue.
//
// The methods begin with Sequential-specific code to be most clear.
// The lock and condition variables are not used in the Sequential
// case.
//
// Internally, the queue is implemented as a circular array of size
// MaxStaticSize, where the queue boundaries are denoted by the Front
// and Back fields.  Front==Back indicates an empty queue.
template <typename T, size_t MaxStaticSize = 128>
class BoundedProducerConsumerQueue {
  BoundedProducerConsumerQueue() = delete;
  BoundedProducerConsumerQueue(const BoundedProducerConsumerQueue &) = delete;
  BoundedProducerConsumerQueue &
  operator=(const BoundedProducerConsumerQueue &) = delete;

public:
  BoundedProducerConsumerQueue(size_t MaxSize, bool Sequential)
      : Back(0), Front(0), MaxSize(std::min(MaxSize, MaxStaticSize)),
        Sequential(Sequential), IsEnded(false) {}
  void blockingPush(T *Item) {
    {
      std::unique_lock<GlobalLockType> L(Lock);
      // If the work queue is already "full", wait for a consumer to
      // grab an element and shrink the queue.
      Shrunk.wait(L, [this] { return size() < MaxSize || Sequential; });
      push(Item);
    }
    GrewOrEnded.notify_one();
  }
  T *blockingPop() {
    T *Item = nullptr;
    bool ShouldNotifyProducer = false;
    {
      std::unique_lock<GlobalLockType> L(Lock);
      GrewOrEnded.wait(L, [this] { return IsEnded || !empty() || Sequential; });
      if (!empty()) {
        Item = pop();
        ShouldNotifyProducer = !IsEnded;
      }
    }
    if (ShouldNotifyProducer)
      Shrunk.notify_one();
    return Item;
  }
  void notifyEnd() {
    {
      std::lock_guard<GlobalLockType> L(Lock);
      IsEnded = true;
    }
    GrewOrEnded.notify_all();
  }

private:
  const static size_t MaxStaticSizeMask = MaxStaticSize - 1;
  static_assert(!(MaxStaticSize & (MaxStaticSize - 1)),
                "MaxStaticSize must be a power of 2");

  // WorkItems and Lock are read/written by all.
  ICE_CACHELINE_BOUNDARY;
  T *WorkItems[MaxStaticSize];
  ICE_CACHELINE_BOUNDARY;
  // Lock guards access to WorkItems, Front, Back, and IsEnded.
  GlobalLockType Lock;

  ICE_CACHELINE_BOUNDARY;
  // GrewOrEnded is written by the producers and read by the
  // consumers.  It is notified (by the producer) when something is
  // added to the queue, in case consumers are waiting for a non-empty
  // queue.
  std::condition_variable GrewOrEnded;
  // Back is the index into WorkItems[] of where the next element will
  // be pushed.  (More precisely, Back&MaxStaticSize is the index.)
  // It is written by the producers, and read by all via size() and
  // empty().
  size_t Back;

  ICE_CACHELINE_BOUNDARY;
  // Shrunk is notified (by the consumer) when something is removed
  // from the queue, in case a producer is waiting for the queue to
  // drop below maximum capacity.  It is written by the consumers and
  // read by the producers.
  std::condition_variable Shrunk;
  // Front is the index into WorkItems[] of the oldest element,
  // i.e. the next to be popped.  (More precisely Front&MaxStaticSize
  // is the index.)  It is written by the consumers, and read by all
  // via size() and empty().
  size_t Front;

  ICE_CACHELINE_BOUNDARY;

  // MaxSize and Sequential are read by all and written by none.
  const size_t MaxSize;
  const bool Sequential;
  // IsEnded is read by the consumers, and only written once by the
  // producer.
  bool IsEnded;

  // The lock must be held when the following methods are called.
  bool empty() const { return Front == Back; }
  size_t size() const { return Back - Front; }
  void push(T *Item) {
    WorkItems[Back++ & MaxStaticSizeMask] = Item;
    assert(size() <= MaxStaticSize);
  }
  T *pop() {
    assert(!empty());
    return WorkItems[Front++ & MaxStaticSizeMask];
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEUTILS_H
