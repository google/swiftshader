#!/bin/bash

cd git/SwiftShader

set -e # Fail on any error.
set -x # Display commands being run.

# Download all submodules
git submodule update --init

mkdir -p build && cd build

if [[ -z "${REACTOR_BACKEND}" ]]; then
  REACTOR_BACKEND="LLVM"
fi

cmake .. "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DREACTOR_VERIFY_LLVM_IR=1"
make --jobs=$(nproc)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests
build/vk-unittests