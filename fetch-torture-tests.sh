#!/bin/bash

BUILDID=4915
BUILD_PATH=https://storage.googleapis.com/wasm-llvm/builds/git

wget -O - /wasm-torture-s-$BUILDID.tbz2 \
  | tar xj

wget -O - $BUILD_PATH/wasm-torture-s2wasm-sexpr-wasm-$BUILDID.tbz2 \
  | tar xj

wget -O - $BUILD_PATH/wasm-binaries-$BUILDID.tbz2 \
  | tar xj
