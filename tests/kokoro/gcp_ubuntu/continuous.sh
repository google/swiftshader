#!/bin/bash

cd git/SwiftShader

set -e # Fail on any error.
set -x # Display commands being run.

# Specify we want to build with GCC 7
sudo apt-get update
sudo apt-get install -y gcc-7 g++-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100 --slave /usr/bin/g++ g++ /usr/bin/g++-7
sudo update-alternatives --set gcc "/usr/bin/gcc-7"

# Download all submodules
git submodule update --init

mkdir -p build && cd build

if [[ -z "${REACTOR_BACKEND}" ]]; then
  REACTOR_BACKEND="LLVM"
fi

# Lower the amount of debug info, to reduce Kokoro build times.
LESS_DEBUG_INFO=1

cmake .. "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DREACTOR_VERIFY_LLVM_IR=1" "-DLESS_DEBUG_INFO=${LESS_DEBUG_INFO}"
cmake --build . -- -j $(nproc)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests
build/vk-unittests

# Incrementally build and run rr::Print unit tests
cd build
cmake .. "-DREACTOR_ENABLE_PRINT=1"
cmake --build . --target ReactorUnitTests -- -j $(nproc)
cd ..
build/ReactorUnitTests --gtest_filter=ReactorUnitTests.Print*
