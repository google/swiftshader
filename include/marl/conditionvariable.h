// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef marl_condition_variable_h
#define marl_condition_variable_h

#include "containers.h"
#include "debug.h"
#include "scheduler.h"

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace marl {

// ConditionVariable is a synchronization primitive that can be used to block
// one or more fibers or threads, until another fiber or thread modifies a
// shared variable (the condition) and notifies the ConditionVariable.
//
// If the ConditionVariable is blocked on a thread with a Scheduler bound, the
// thread will work on other tasks until the ConditionVariable is unblocked.
class ConditionVariable {
 public:
  // Notifies and potentially unblocks one waiting fiber or thread.
  inline void notify_one();

  // Notifies and potentially unblocks all waiting fibers and/or threads.
  inline void notify_all();

  // Blocks the current fiber or thread until the predicate is satisfied
  // and the ConditionVariable is notified.
  template <typename Predicate>
  inline void wait(std::unique_lock<std::mutex>& lock, Predicate pred);

 private:
  std::mutex mutex;
  containers::vector<Scheduler::Fiber*, 4> waiting;
  std::condition_variable condition;
  std::atomic<int> numWaiting = {0};
  std::atomic<int> numWaitingOnCondition = {0};
};

void ConditionVariable::notify_one() {
  if (numWaiting == 0) {
    return;
  }
  std::unique_lock<std::mutex> lock(mutex);
  if (waiting.size() > 0) {
    auto fiber = waiting.back();
    waiting.pop_back();
    fiber->schedule();
  }
  lock.unlock();
  if (numWaitingOnCondition > 0) {
    condition.notify_one();
  }
}

void ConditionVariable::notify_all() {
  if (numWaiting == 0) {
    return;
  }
  std::unique_lock<std::mutex> lock(mutex);
  while (waiting.size() > 0) {
    auto fiber = waiting.back();
    waiting.pop_back();
    fiber->schedule();
  }
  lock.unlock();
  if (numWaitingOnCondition > 0) {
    condition.notify_all();
  }
}

template <typename Predicate>
void ConditionVariable::wait(std::unique_lock<std::mutex>& dataLock,
                             Predicate pred) {
  if (pred()) {
    return;
  }
  numWaiting++;
  if (auto fiber = Scheduler::Fiber::current()) {
    // Currently executing on a scheduler fiber.
    // Yield to let other tasks run that can unblock this fiber.
    while (!pred()) {
      mutex.lock();
      waiting.push_back(fiber);
      mutex.unlock();

      dataLock.unlock();
      fiber->yield();
      dataLock.lock();
    }
  } else {
    // Currently running outside of the scheduler.
    // Delegate to the std::condition_variable.
    numWaitingOnCondition++;
    condition.wait(dataLock, pred);
    numWaitingOnCondition--;
  }
  numWaiting--;
}

}  // namespace marl

#endif  // marl_condition_variable_h
