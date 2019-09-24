#!/bin/bash

set -e # Fail on any error.
set -x # Display commands being run.

BUILD_ROOT=$PWD

cd github/marl

git submodule update --init

if [ "$BUILD_SYSTEM" == "cmake" ]; then
    mkdir build
    cd build

    build_and_run() {
        cmake .. -DMARL_BUILD_EXAMPLES=1 -DMARL_BUILD_TESTS=1 -DMARL_WARNINGS_AS_ERRORS=1 $1
        make --jobs=$(nproc)

        ./marl-unittests
        ./fractal
        ./primes > /dev/null
    }

    if [ "$BUILD_SANITIZER" == "asan" ]; then
        build_and_run "-DMARL_ASAN=1"
    elif [ "$BUILD_SANITIZER" == "msan" ]; then
        build_and_run "-DMARL_MSAN=1"
    elif [ "$BUILD_SANITIZER" == "tsan" ]; then
        build_and_run "-DMARL_TSAN=1"
    else
        build_and_run
    fi
elif [ "$BUILD_SYSTEM" == "bazel" ]; then
    # Get bazel
    curl -L -k -O -s https://github.com/bazelbuild/bazel/releases/download/0.29.1/bazel-0.29.1-installer-linux-x86_64.sh
    mkdir $BUILD_ROOT/bazel
    bash bazel-0.29.1-installer-linux-x86_64.sh --prefix=$BUILD_ROOT/bazel
    rm bazel-0.29.1-installer-linux-x86_64.sh
    # Build and run
    $BUILD_ROOT/bazel/bin/bazel test //:tests
    $BUILD_ROOT/bazel/bin/bazel run //examples:fractal
    $BUILD_ROOT/bazel/bin/bazel run //examples:primes > /dev/null
else
    echo "Unknown build system: $BUILD_SYSTEM"
    exit 1
fi