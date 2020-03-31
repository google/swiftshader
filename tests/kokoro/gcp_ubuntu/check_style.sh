#!/bin/bash

set -x # Display commands being run.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

# Download clang tar
CLANG_TAR="/tmp/clang-10.tar.xz"
curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz > ${CLANG_TAR}
# Verify clang tar
sudo apt-get install pgpgpg
gpg --import "${SCRIPT_DIR}/hans-gpg-key.asc"
gpg --verify "${SCRIPT_DIR}/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz.sig" ${CLANG_TAR}
if [ $? -ne 0 ]
then
  echo "clang download failed PGP check"
  exit 1
fi

set -e # Fail on any error

# Untar into tmp
CLANG_DIR=/tmp/clang-10
mkdir ${CLANG_DIR}
tar -xf ${CLANG_TAR} -C ${CLANG_DIR}

# Set up env vars
export CLANG_FORMAT=${CLANG_DIR}/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang-format

# Run presubmit tests
cd git/SwiftShader
./tests/presubmit.sh
