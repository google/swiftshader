#!/bin/bash

cd git/SwiftShader

# Validate commit message
git log -1 --pretty=%B | grep -E '^Bug:|^Issue:|^Fixes:|^Regres:'

if [ $? -ne 0 ]
then
  echo "error: Git commit message must have a Bug: line."
  exit 1
fi

# Fail on any error.
set -e
# Display commands being run.
set -x

# Download all submodules
git submodule update --init

mkdir -p build && cd build

if [[ -z "${REACTOR_BACKEND}" ]]; then
  REACTOR_BACKEND="LLVM"
fi

cmake .. "-DREACTOR_BACKEND=${REACTOR_BACKEND}" "-DREACTOR_VERIFY_LLVM_IR=1"
make --jobs=$(nproc)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests

if [ "${REACTOR_BACKEND}" != "Subzero" ]; then
  build/vk-unittests # Currently vulkan does not work with Subzero.
fi