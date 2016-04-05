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
import sys

success_count = 0
fail_count = 0
failures = []

def run_test(test_file, verbose=False):
  global success_count
  global fail_count

  cmd = """LD_LIBRARY_PATH=../../../../v8/out/native/lib.target ./pnacl-sz \
               -filetype=asm -target=arm32 {} -threads=0 -O2 \
               -verbose=wasm""".format(test_file)

  if not verbose:
    cmd += " &> /dev/null"

  sys.stdout.write(test_file + "...");
  status = os.system(cmd);
  if status != 0:
    fail_count += 1
    print('\033[1;31m[fail]\033[1;m')
    failures.append(test_file)
  else:
    success_count += 1
    print('\033[1;32m[ok]\033[1;m')


verbose = False

if len(sys.argv) > 1:
  test_files = sys.argv[1:]
  verbose = True
else:
  test_files = glob.glob("./torture-s2wasm-sexpr-wasm.old/*.wasm")

for test_file in test_files:
  run_test(test_file, verbose)

if len(failures) > 0:
  print("Failures:")
  for f in failures:
    print("  \033[1;31m" + f + "\033[1;m")
print("{} / {} tests passed".format(success_count, success_count + fail_count))
