# Marl

Marl is a hybrid thread / fiber task scheduler written in C++ 11.

## About

Marl is a C++ 11 library that provides a fluent interface for running tasks across a number of threads.

Marl uses a combination of fibers and threads to allow efficient execution of tasks that can block, while keeping a fixed number of hardware threads.

Marl supports Windows, macOS, Linux, Fuchsia and Android (arm, aarch64, ppc64 (ELFv2), x86 and x64).

Marl has no dependencies on other libraries (with an exception on googletest for building the optional unit tests).

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

You will also want to add the `marl` public headers to your project's include search paths so you can `#include` the marl headers:

```cmake
target_include_directories($<target> PRIVATE "${MARL_DIR}/include") # replace <target> with the name of your project's target
```

---

Note: This is not an officially supported Google product
