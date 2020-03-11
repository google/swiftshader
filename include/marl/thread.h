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

#ifndef marl_thread_h
#define marl_thread_h

#include <functional>

namespace marl {

// Thread provides an OS abstraction for threads of execution.
// Thread is used by marl instead of std::thread as Windows does not naturally
// scale beyond 64 logical threads on a single CPU, unless you use the Win32
// API.
// Thread alsocontains static methods that abstract OS-specific thread / cpu
// queries and control.
class Thread {
 public:
  using Func = std::function<void()>;

  Thread() = default;
  Thread(Thread&&);
  Thread& operator=(Thread&&);

  // Start a new thread that calls func.
  // logicalCpuHint is a hint to run the thread on the specified logical CPU.
  // logicalCpuHint may be entirely ignored.
  Thread(unsigned int logicalCpuHint, const Func& func);

  ~Thread();

  // join() blocks until the thread completes.
  void join();

  // setName() sets the name of the currently executing thread for displaying
  // in a debugger.
  static void setName(const char* fmt, ...);

  // numLogicalCPUs() returns the number of available logical CPU cores for
  // the system.
  static unsigned int numLogicalCPUs();

 private:
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  class Impl;
  Impl* impl = nullptr;
};

}  // namespace marl

#endif  // marl_thread_h
