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

// Simple "hello world" example that uses marl::Event.

#include "marl/defer.h"
#include "marl/event.h"
#include "marl/scheduler.h"

#include <cstdio>

int main() {
  // Create a marl scheduler using the 4 hardware threads.
  // Bind this scheduler to the main thread so we can call marl::schedule()
  marl::Scheduler scheduler;
  scheduler.bind();
  scheduler.setWorkerThreadCount(4);
  defer(scheduler.unbind());  // Automatically unbind before returning.

  // Create an event that automatically resets itself.
  marl::Event sayHellow(marl::Event::Mode::Auto);
  marl::Event saidHellow(marl::Event::Mode::Auto);

  // Schedule some tasks to run asynchronously.
  for (int i = 0; i < 10; i++) {
    // Each task will run on one of the 4 worker threads.
    marl::schedule([=] {  // All marl primitives are capture-by-value.
      printf("Task %d waiting to say hello!\n", i);

      // Blocking in a task?
      // The scheduler will find something else for this thread to do.
      sayHellow.wait();

      printf("Hello from task %d!\n", i);

      saidHellow.signal();
    });
  }

  // Unblock the tasks one by one.
  for (int i = 0; i < 10; i++) {
    sayHellow.signal();
    saidHellow.wait();
  }

  // All tasks are guaranteed to completed before the scheduler is destructed.
}
