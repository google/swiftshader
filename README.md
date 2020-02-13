# Marl

Marl is a hybrid thread / fiber task scheduler written in C++ 11.

## About

Marl is a C++ 11 library that provides a fluent interface for running tasks across a number of threads.

Marl uses a combination of fibers and threads to allow efficient execution of tasks that can block, while keeping a fixed number of hardware threads.

Marl supports Windows, macOS, Linux, Fuchsia and Android (arm, aarch64, mips64, ppc64 (ELFv2), x86 and x64).

Marl has no dependencies on other libraries (with an exception on googletest for building the optional unit tests).

Example:

```cpp
#include "marl/defer.h"
#include "marl/event.h"
#include "marl/scheduler.h"
#include "marl/waitgroup.h"

#include <cstdio>

int main() {
  // Create a marl scheduler using the 4 hardware threads.
  // Bind this scheduler to the main thread so we can call marl::schedule()
  marl::Scheduler scheduler;
  scheduler.bind();
  scheduler.setWorkerThreadCount(4);
  defer(scheduler.unbind());  // Automatically unbind before returning.

  constexpr int numTasks = 10;

  // Create an event that is manually reset.
  marl::Event sayHellow(marl::Event::Mode::Manual);

  // Create a WaitGroup with an initial count of numTasks.
  marl::WaitGroup saidHellow(numTasks);

  // Schedule some tasks to run asynchronously.
  for (int i = 0; i < numTasks; i++) {
    // Each task will run on one of the 4 worker threads.
    marl::schedule([=] {  // All marl primitives are capture-by-value.
      // Decrement the WaitGroup counter when the task has finished.
      defer(saidHellow.done());

      printf("Task %d waiting to say hello...\n", i);

      // Blocking in a task?
      // The scheduler will find something else for this thread to do.
      sayHellow.wait();

      printf("Hello from task %d!\n", i);
    });
  }

  sayHellow.signal();  // Unblock all the tasks.

  saidHellow.wait();  // Wait for all tasks to complete.

  printf("All tasks said hello.\n");

  // All tasks are guaranteed to complete before the scheduler is destructed.
}
```

## Building

Marl contains many unit tests and examples that can be built using CMake.

Unit tests require fetching the `googletest` external project, which can be done by typing the following in your terminal:

```bash
cd <path-to-marl>
git submodule update --init
```

### Linux and macOS

To build the unit tests and examples, type the following in your terminal:

```bash
cd <path-to-marl>
mkdir build
cd build
cmake .. -DMARL_BUILD_EXAMPLES=1 -DMARL_BUILD_TESTS=1
make
```

The resulting binaries will be found in `<path-to-marl>/build`

### Windows

Marl can be built using [Visual Studio 2019's CMake integration](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019).

### Using Marl in your CMake project

You can build and link Marl using `add_subdirectory()` in your project's `CMakeLists.txt` file:
```cmake
set(MARL_DIR <path-to-marl>) # example <path-to-marl>: "${CMAKE_CURRENT_SOURCE_DIR}/third_party/marl"
add_subdirectory(${MARL_DIR})
```

This will define the `marl` library target, which you can pass to `target_link_libraries()`:

```cmake
target_link_libraries(<target> marl) # replace <target> with the name of your project's target
```

You may also wish to specify your own paths to the third party libraries used by `marl`.
You can do this by setting any of the following variables before the call to `add_subdirectory()`:

```cmake
set(MARL_THIRD_PARTY_DIR <third-party-root-directory>) # defaults to ${MARL_DIR}/third_party
set(MARL_GOOGLETEST_DIR  <path-to-googletest>)         # defaults to ${MARL_THIRD_PARTY_DIR}/googletest
add_subdirectory(${MARL_DIR})
```

## Benchmarks

Graphs of several microbenchmarks can be found [here](https://google.github.io/marl/benchmarks).

---

Note: This is not an officially supported Google product
