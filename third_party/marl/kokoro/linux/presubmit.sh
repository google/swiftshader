#!/bin/bash

set -e # Fail on any error.
set -x # Display commands being run.

cd github/marl

git submodule update --init

mkdir build
cd build

build_and_run() {
    cmake .. -DMARL_BUILD_EXAMPLES=1 -DMARL_BUILD_TESTS=1 -DMARL_WARNINGS_AS_ERRORS=1 $1
    make --jobs=$(nproc)

    ./marl-unittests
    ./fractal
}

build_and_run ""
build_and_run "-DMARL_ASAN=1"
build_and_run "-DMARL_MSAN=1"