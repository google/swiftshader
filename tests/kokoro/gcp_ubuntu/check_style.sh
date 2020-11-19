#!/bin/bash

set -x # Display commands being run.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"

# Download Clang tar
CLANG_PACKAGE="clang+llvm-11.0.1-x86_64-linux-gnu-ubuntu-16.04"
curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.1/${CLANG_PACKAGE}.tar.xz > /tmp/clang.tar.xz
# Verify Clang tar
sudo apt-get install pgpgpg
gpg --import "${SCRIPT_DIR}/tstellar-gpg-key.asc"
gpg --verify "${SCRIPT_DIR}/${CLANG_PACKAGE}.tar.xz.sig" /tmp/clang.tar.xz
if [ $? -ne 0 ]
then
  echo "clang download failed PGP check"
  exit 1
fi

set -e # Fail on any error

# Untar into tmp
mkdir /tmp/clang
tar -xf /tmp/clang.tar.xz -C /tmp/clang

# Set up env vars
export CLANG_FORMAT=/tmp/clang/${CLANG_PACKAGE}/bin/clang-format

# Run presubmit tests
cd git/SwiftShader
./tests/presubmit.sh
