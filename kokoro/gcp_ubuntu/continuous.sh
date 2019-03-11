#!/bin/bash

# Fail on any error.
set -e
# Display commands being run.
set -x

cd git/SwiftShader

git submodule update --init

mkdir -p build && cd build

cmake ..
make --jobs=$(nproc)

# Run the reactor unit tests.
./ReactorUnitTests

# Run the GLES unit tests. TODO(capn): rename.
./unittests

# Run the Vulkan unit tests.
cd .. # Must be run from project root
build/vk-unittests
