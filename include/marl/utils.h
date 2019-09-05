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

#ifndef marl_util_h
#define marl_util_h

#include "scheduler.h"
#include "waitgroup.h"

namespace marl {

// parallelize() is used to split a number of work items into N smaller batches
// which can be processed in parallel with the function f().
// numTotal is the total number of work items to process.
// numPerTask is the maximum number of work items to process per call to f().
// There will always be at least one call to f().
// F must be a function with the signature:
//    void(COUNTER taskIndex, COUNTER first, COUNTER count)
// COUNTER is any integer type.
template <typename F, typename COUNTER>
inline void parallelize(COUNTER numTotal, COUNTER numPerTask, const F& f) {
  auto numTasks = (numTotal + numPerTask - 1) / numPerTask;
  WaitGroup wg(numTasks - 1);
  for (unsigned int task = 1; task < numTasks; task++) {
    schedule([=] {
      auto first = task * numPerTask;
      auto count = std::min(first + numPerTask, numTotal) - first;
      f(task, first, count);
      wg.done();
    });
  }

  // Run the first chunk on this fiber to reduce the amount of time spent
  // waiting.
  f(0, 0, std::min(numPerTask, numTotal));
  wg.wait();
}

}  // namespace marl

#endif  // marl_util_h
