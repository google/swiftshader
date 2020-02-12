#!/bin/bash

# Fail on any error.
set -e
# Display commands being run.
set -x

cd git/SwiftShader

git submodule update --init

mkdir -p build && cd build

if [[ -z "${REACTOR_BACKEND}" ]]; then
  REACTOR_BACKEND="LLVM"
fi

# Lower the amount of debug info, to reduce Kokoro build times.
LESS_DEBUG_INFO=1

# Disable ASAN checks for debug builds, to reduce Kokoro build times.
# ASAN builds are recommended to be optimized.
ASAN="ON"
if [[ "${BUILD_TYPE}" == "Debug" ]]; then
  ASAN="OFF"
fi

cmake .. "-DSWIFTSHADER_ASAN=${ASAN}" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DREACTOR_VERIFY_LLVM_IR=1" "-DLESS_DEBUG_INFO=${LESS_DEBUG_INFO}"
cmake --build . -- -j$(sysctl -n hw.logicalcpu)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests
build/vk-unittests

# Incrementally build and run rr::Print unit tests
cd build
cmake .. "-DREACTOR_ENABLE_PRINT=1"
cmake --build . --target ReactorUnitTests -- -j$(sysctl -n hw.logicalcpu)
cd ..
build/ReactorUnitTests --gtest_filter=ReactorUnitTests.Print*
