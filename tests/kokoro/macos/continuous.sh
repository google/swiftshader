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

cmake .. "-DASAN=ON" "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DCMAKE_BUILD_TYPE=RelWithDebInfo" "-DREACTOR_VERIFY_LLVM_IR=1"
make -j$(sysctl -n hw.logicalcpu)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests

if [ "${REACTOR_BACKEND}" != "Subzero" ]; then
  build/vk-unittests # Currently vulkan does not work with Subzero.
fi
