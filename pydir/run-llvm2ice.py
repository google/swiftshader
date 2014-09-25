#!/usr/bin/env python2

import argparse
import itertools
import os
import re
import subprocess
import sys

from utils import shellcmd

def main():
    """Run the llvm2ice compiler on an llvm file.

    Takes an llvm input file, freezes it into a pexe file, converts
    it to a Subzero program, and finally compiles it.
    """
    argparser = argparse.ArgumentParser(
        description='    ' + main.__doc__,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    argparser.add_argument('--input', '-i', required=True,
                           help='LLVM source file to compile')
    argparser.add_argument('--insts', required=False,
                           action='store_true',
                           help='Stop after translating to ' +
                           'Subzero instructions')
    argparser.add_argument('--no-local-syms', required=False,
                           action='store_true',
                           help="Don't keep local symbols in the pexe file")
    argparser.add_argument('--llvm', required=False,
                           action='store_true',
                           help='Parse pexe into llvm IR first, then ' +
                           'convert to Subzero')
    argparser.add_argument('--llvm-source', required=False,
                           action='store_true',
                           help='Parse source directly into llvm IR ' +
                           '(without generating a pexe), then ' +
                           'convert to Subzero')
    argparser.add_argument(
        '--llvm2ice', required=False, default='./llvm2ice', metavar='LLVM2ICE',
        help="Subzero translator 'llvm2ice'")
    argparser.add_argument('--llvm-bin-path', required=False,
                           default=None, metavar='LLVM_BIN_PATH',
                           help='Path to LLVM executables ' +
                                '(for building PEXE files)')
    argparser.add_argument('--echo-cmd', required=False,
                           action='store_true',
                           help='Trace command that generates ICE instructions')
    argparser.add_argument('--args', '-a', nargs=argparse.REMAINDER,
                           help='Remaining arguments are passed to llvm2ice')

    args = argparser.parse_args()
    llvm_bin_path = args.llvm_bin_path
    llfile = args.input

    if args.llvm and args.llvm_source:
      raise RuntimeError("Can't specify both '--llvm' and '--llvm-source'")

    if args.llvm_source and args.no_local_syms:
      raise RuntimeError("Can't specify both '--llvm-source' and " +
                         "'--no-local-syms'")

    cmd = []
    if not args.llvm_source:
      cmd = [os.path.join(llvm_bin_path, 'llvm-as'), llfile, '-o', '-', '|',
             os.path.join(llvm_bin_path, 'pnacl-freeze')]
      if not args.no_local_syms:
        cmd += ['--allow-local-symbol-tables']
      cmd += ['|']
    cmd += [args.llvm2ice]
    if args.insts:
      cmd += ['-verbose', 'inst', '-notranslate']
    if not args.llvm_source:
      cmd += ['--bitcode-format=pnacl']
      if not args.no_local_syms:
        cmd += ['--allow-local-symbol-tables']
    if not (args.llvm or args.llvm_source):
      cmd += ['--build-on-read']
    if args.args:
      cmd += args.args
    if args.llvm_source:
      cmd += [llfile]

    stdout_result = shellcmd(cmd, echo=args.echo_cmd)
    if not args.echo_cmd:
      sys.stdout.write(stdout_result)

if __name__ == '__main__':
    main()
