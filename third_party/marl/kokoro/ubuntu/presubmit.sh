#!/bin/bash

set -e # Fail on any error.
set -x # Display commands being run.

BUILD_ROOT=$PWD
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"
UBUNTU_VERSION=`cat /etc/os-release | grep -oP "Ubuntu \K([0-9]+\.[0-9]+)"`

cd github/marl

git submodule update --init

# Always update gcc so we get a newer standard library.
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install -y gcc-9-multilib g++-9-multilib linux-libc-dev:i386
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 100 --slave /usr/bin/g++ g++ /usr/bin/g++-9
sudo update-alternatives --set gcc "/usr/bin/gcc-9"

if [ "$BUILD_SYSTEM" == "cmake" ]; then
    mkdir build
    cd build

    if [ "$BUILD_TOOLCHAIN" == "clang" ]; then
        # Download clang tar
        CLANG_TAR="/tmp/clang-8.tar.xz"
        curl -L "https://releases.llvm.org/8.0.0/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-${UBUNTU_VERSION}.tar.xz" > ${CLANG_TAR}
        
        # Verify clang tar
        sudo apt-get install pgpgpg
        gpg --import "${SCRIPT_DIR}/clang-8.pubkey.asc"
        gpg --verify "${SCRIPT_DIR}/clang-8-ubuntu-${UBUNTU_VERSION}.sig" ${CLANG_TAR}
        if [ $? -ne 0 ]; then
            echo "clang download failed PGP check"
            exit 1
        fi

        # Untar into tmp
        CLANG_DIR=/tmp/clang-8
        mkdir ${CLANG_DIR}
        tar -xf ${CLANG_TAR} -C ${CLANG_DIR}

        # Use clang as compiler
        export CC="${CLANG_DIR}/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-${UBUNTU_VERSION}/bin/clang"
        export CXX="${CLANG_DIR}/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-${UBUNTU_VERSION}/bin/clang++"
    fi

    extra_cmake_flags=""
    if [ "$BUILD_TARGET_ARCH" == "x86" ]; then
        extra_cmake_flags="-DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32 -DCMAKE_ASM_FLAGS=-m32"
    fi

    build_and_run() {
        cmake .. ${extra_cmake_flags} \
                 -DMARL_BUILD_EXAMPLES=1 \
                 -DMARL_BUILD_TESTS=1 \
                 -DMARL_BUILD_BENCHMARKS=1 \
                 -DMARL_WARNINGS_AS_ERRORS=1 \
                 $1

        make --jobs=$(nproc)

        ./marl-unittests
        ./fractal
        ./hello_task
        ./primes > /dev/null
        ./tasks_in_tasks
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
    $BUILD_ROOT/bazel/bin/bazel test //:tests --test_output=all
    $BUILD_ROOT/bazel/bin/bazel run //examples:fractal
    $BUILD_ROOT/bazel/bin/bazel run //examples:primes > /dev/null
else
    echo "Unknown build system: $BUILD_SYSTEM"
    exit 1
fi