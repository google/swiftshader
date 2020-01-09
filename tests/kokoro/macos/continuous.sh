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

# Disable ASAN checks for debug builds to work around Kokoro timeouts that
# affect llvm-debug builds. See b/147355576 for more information.
ASAN="ON"
if [[ "${BUILD_TYPE}" == "Debug" ]]; then
  ASAN="OFF"
fi

cmake .. "-DASAN=${ASAN}" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DREACTOR_VERIFY_LLVM_IR=1"
make -j$(sysctl -n hw.logicalcpu)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests
build/vk-unittests