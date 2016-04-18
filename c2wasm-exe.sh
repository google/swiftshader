#!/bin/bash

# TODO (eholk): This script is a hack for debugging and development
# that should be removed.

./wasm-install/bin/emscripten/emcc "$1" -s BINARYEN=1 \
  -s 'BINARYEN_METHOD="native-wasm"' -O2 \
  --em-config wasm-install/emscripten_config_vanilla && \
./wasm-install/bin/sexpr-wasm a.out.wast -o a.out.wasm && \
./pnacl-sz a.out.wasm -o a.out.o -filetype=obj -O2 && \
clang -m32 a.out.o runtime/wasm-runtime.c -g
