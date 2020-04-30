// Copyright 2020 The Marl Authors.
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

#ifndef marl_parallelize_h
#define marl_parallelize_h

#include "scheduler.h"
#include "waitgroup.h"

namespace marl {

namespace detail {

void parallelizeChain(WaitGroup&) {}

template <typename F, typename... L>
void parallelizeChain(WaitGroup& wg, F&& f, L&&... l) {
  schedule([=] {
    f();
    wg.done();
  });
  parallelizeChain(wg, std::forward<L>(l)...);
}

}  // namespace detail

// parallelize() schedules all the function parameters and waits for them to
// complete. These functions may execute concurrently.
// Each function must take no parameters.
template <typename... FUNCTIONS>
inline void parallelize(FUNCTIONS&&... functions) {
  WaitGroup wg(sizeof...(FUNCTIONS));
  detail::parallelizeChain(wg, functions...);
  wg.wait();
}

}  // namespace marl

#endif  // marl_parallelize_h
