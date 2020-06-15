#!/bin/bash

set -e # Fail on any error.

. /bin/using.sh # Declare the bash `using` function for configuring toolchains.

set -x # Display commands being run.

cd github/marl

git submodule update --init

using gcc-9 # Always update gcc so we get a newer standard library.

if [ "$BUILD_SYSTEM" == "cmake" ]; then
    using cmake-3.17.2

    mkdir build
    cd build

    if [ "$BUILD_TOOLCHAIN" == "clang" ]; then
        using clang-10.0.0
    fi

    EXTRA_CMAKE_FLAGS=""
    if [ "$BUILD_TARGET_ARCH" == "x86" ]; then
        EXTRA_CMAKE_FLAGS="-DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32 -DCMAKE_ASM_FLAGS=-m32"
    fi

    if [ "$BUILD_SANITIZER" == "asan" ]; then
        EXTRA_CMAKE_FLAGS="$EXTRA_CMAKE_FLAGS -DMARL_ASAN=1"
    elif [ "$BUILD_SANITIZER" == "msan" ]; then
        EXTRA_CMAKE_FLAGS="$EXTRA_CMAKE_FLAGS -DMARL_MSAN=1"
    elif [ "$BUILD_SANITIZER" == "tsan" ]; then
        EXTRA_CMAKE_FLAGS="$EXTRA_CMAKE_FLAGS -DMARL_TSAN=1"
    fi

    cmake .. ${EXTRA_CMAKE_FLAGS} \
            -DMARL_BUILD_EXAMPLES=1 \
            -DMARL_BUILD_TESTS=1 \
            -DMARL_BUILD_BENCHMARKS=1 \
            -DMARL_WARNINGS_AS_ERRORS=1 \
            -DMARL_DEBUG_ENABLED=1

    make --jobs=$(nproc)

    ./marl-unittests
    ./fractal
    ./hello_task
    ./primes > /dev/null
    ./tasks_in_tasks

elif [ "$BUILD_SYSTEM" == "bazel" ]; then
    using bazel-3.1.0

    bazel test //:tests --test_output=all
    bazel run //examples:fractal
    bazel run //examples:hello_task
    bazel run //examples:primes > /dev/null
    bazel run //examples:tasks_in_tasks
else
    echo "Unknown build system: $BUILD_SYSTEM"
    exit 1
fi