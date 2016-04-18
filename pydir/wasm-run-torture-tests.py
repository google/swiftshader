#!/usr/bin/env python2

#===- subzero/wasm-run-torture-tests.py - Subzero WASM Torture Test Driver ===//
#
#                        The Subzero Code Generator
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===-----------------------------------------------------------------------===//

import glob
import os
import shutil
import sys

IGNORED_TESTS = [
  'loop-2f.c.wasm',       # mmap not in MVP
  'loop-2g.c.wasm',       # mmap not in MVP

  '960521-1.c.wasm',      # sbrk not in MVP

  'pr36765.c.wasm',       # __builtin_malloc not allowed

  'pr24716.c.wasm',       # infinite loop
  'vla-dealloc-1.c.wasm', # infinite loop
  '20040811-1.c.wasm',    # infinite loop
  '961125-1.c.wasm',      # infinite loop
  '980506-1.c.wasm',      # infinite loop
  '20070824-1.c.wasm',    # infinite loop
  '20140212-1.c.wasm',    # infinite loop
  'pr59014.c.wasm',       # infinite loop
  'pr58277-2.c.wasm',     # infinite loop
  'pr43560.c.wasm',       # infinite loop
  '20000818-1.c.wasm',    # infinite loop
  '20010409-1.c.wasm',    # infinite loop
  'loop-7.c.wasm',        # infinite loop
  'pr34415.c.wasm',       # infinite loop
  '20011126-2.c.wasm',    # infinite loop
  'postmod-1.c.wasm',     # infinite loop
  'pr17133.c.wasm',       # infinite loop
  '20021024-1.c.wasm',    # infinite loop
  'pr15296.c.wasm',       # infinite loop
  '990524-1.c.wasm',      # infinite loop
  'loop-12.c.wasm',       # infinite loop
  '961125-1.c.wasm',      # infinite loop
]

OUT_DIR = "./build/wasm-torture"

compile_count = 0
compile_failures = []

run_count = 0
run_failures = []

def run_test(test_file, verbose=False):
  global compile_count
  global compile_failures
  global run_count
  global run_failures
  global OUT_DIR
  global IGNORED_TESTS

  test_name = os.path.basename(test_file)
  obj_file = os.path.join(OUT_DIR, test_name + ".o")
  exe_file = os.path.join(OUT_DIR, test_name + ".exe")

  if not verbose and test_name in IGNORED_TESTS:
    print("\033[1;34mSkipping {}\033[1;m".format(test_file))
    return

  cmd = """LD_LIBRARY_PATH=../../../../v8/out/native/lib.target ./pnacl-sz \
               -filetype=obj -target=x8632 {} -threads=0 -O2 \
               -verbose=wasm -o {}""".format(test_file, obj_file)

  if not verbose:
    cmd += " &> /dev/null"

  sys.stdout.write(test_file + " ...");
  status = os.system(cmd);
  if status != 0:
    print('\033[1;31m[compile fail]\033[1;m')
    compile_failures.append(test_file)
  else:
    compile_count += 1

    # Try to link and run the program.
    cmd = "clang -g -m32 {} -o {} ./runtime/wasm-runtime.c".format(obj_file,
                                                                   exe_file)
    if os.system(cmd) == 0:
      if os.system(exe_file) == 0:
        run_count += 1
        print('\033[1;32m[ok]\033[1;m')
      else:
        run_failures.append(test_file)
        print('\033[1;33m[run fail]\033[1;m')
    else:
      run_failures.append(test_file)
      print('\033[1;33m[run fail]\033[1;m')

verbose = False

if len(sys.argv) > 1:
  test_files = sys.argv[1:]
  verbose = True
else:
  test_files = glob.glob("./emwasm-torture-out/*.wasm")

if os.path.exists(OUT_DIR):
  shutil.rmtree(OUT_DIR)
os.mkdir(OUT_DIR)

for test_file in test_files:
  run_test(test_file, verbose)

if len(compile_failures) > 0:
  print
  print("Compilation failures:")
  print("=====================\n")
  for f in compile_failures:
    print("    \033[1;31m" + f + "\033[1;m")

if len(run_failures) > 0:
  print
  print("Run failures:")
  print("=============\n")
  for f in run_failures:
    print("    \033[1;33m" + f + "\033[1;m")

print("\n\033[1;32m{}\033[1;m / \033[1;33m{}\033[1;m / {} tests passed"
      .format(run_count, compile_count - run_count,
              run_count + len(compile_failures) + len(run_failures)))
