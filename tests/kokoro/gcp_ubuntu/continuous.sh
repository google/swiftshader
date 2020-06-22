#!/bin/bash

cd git/SwiftShader

set -e # Fail on any error.
set -x # Display commands being run.

# Update CMake
sudo aptitude purge -yq cmake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main'
sudo aptitude update -yq
sudo aptitude install -yq cmake
cmake --version

# Specify we want to build with GCC 7
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo aptitude update -yq
sudo aptitude install -yq gcc-7 g++-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100 --slave /usr/bin/g++ g++ /usr/bin/g++-7
sudo update-alternatives --set gcc "/usr/bin/gcc-7"

mkdir -p build && cd build

if [[ -z "${REACTOR_BACKEND}" ]]; then
  REACTOR_BACKEND="LLVM"
fi

# Lower the amount of debug info, to reduce Kokoro build times.
SWIFTSHADER_LESS_DEBUG_INFO=1

cmake .. \
    "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" \
    "-DREACTOR_BACKEND=${REACTOR_BACKEND}" \
    "-DSWIFTSHADER_LLVM_VERSION=${LLVM_VERSION}" \
    "-DREACTOR_VERIFY_LLVM_IR=1" \
    "-DSWIFTSHADER_LESS_DEBUG_INFO=${SWIFTSHADER_LESS_DEBUG_INFO}"
cmake --build . -- -j $(nproc)

# Run unit tests

cd .. # Some tests must be run from project root

build/ReactorUnitTests
build/gles-unittests
build/system-unittests
build/vk-unittests

# Incrementally build and run rr::Print unit tests
cd build
cmake .. "-DREACTOR_ENABLE_PRINT=1"
cmake --build . --target ReactorUnitTests -- -j $(nproc)
cd ..
build/ReactorUnitTests --gtest_filter=ReactorUnitTests.Print*

if [ "${LLVM_VERSION}" -ne "10.0" ]; then
  # Incrementally build with REACTOR_EMIT_DEBUG_INFO to ensure it builds
  cd build
  cmake .. "-DREACTOR_EMIT_DEBUG_INFO=1"
  cmake --build . --target ReactorUnitTests -- -j $(nproc)
  cd ..

  # Incrementally build with REACTOR_EMIT_PRINT_LOCATION to ensure it builds
  cd build
  cmake .. "-DREACTOR_EMIT_PRINT_LOCATION=1"
  cmake --build . --target ReactorUnitTests -- -j $(nproc)
  cd ..
fi
