# Wasm Prototype Experiment Notes

Here's the command I use to test:

```
LD_LIBRARY_PATH=~/nacl/v8/out/native/lib.target make -j48 \
    -f Makefile.standalone WASM=1 && \
LD_LIBRARY_PATH=~/nacl/v8/out/native/lib.target ./pnacl-sz -O2 -filetype=asm \
    -target=arm32 ./torture-s2wasm-sexpr-wasm/20000112-1.c.s.wast.wasm
```

You'll probably need to adjust your `LD_LIBRARY_PATH` to point to where your v8
libraries are.

You'll need to build v8 as a shared library. Build it like this:

```
make -j48 native component=shared_library
```

`wasm-run-torture-tests.py` can be used to run all the tests, or some subset.
Running a subset will enable verbose output. You can download the torture tests
from the [WebAssembly waterfall](https://wasm-stat.us/console).
