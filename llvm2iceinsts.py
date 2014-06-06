#!/usr/bin/env python2

import argparse
import itertools
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'pydir'))

from utils import shellcmd

if __name__ == '__main__':
    desc = 'Run llvm2ice on llvm file to produce ICE instructions.'
    argparser = argparse.ArgumentParser(
        description=desc,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        epilog='''
        Runs in two modes, depending on whether the flag '--pnacl' is specified.

        If flag '--pnacl' is omitted, it runs llvm2ice to (directly) generate
        the corresponding ICE instructions.

        If flag '--pnacl' is given, it first assembles and freezes the
        llvm source file generating the corresponding PNaCl bitcode
        file. The PNaCl bitcode file is then piped into llvm2ice to
        generate the corresponding ICE instructions.
        ''')
    argparser.add_argument(
        '--llvm2ice', required=False, default='./llvm2ice', metavar='LLVM2ICE',
        help='Path to llvm2ice driver program')
    argparser.add_argument('--llvm-bin-path', required=False,
                           default=None, metavar='LLVM_BIN_PATH',
                           help='Path to LLVM executables ' +
                                '(for building PNaCl files)')
    argparser.add_argument('--pnacl', required=False,
                           action='store_true',
                           help='Convert llvm source to PNaCl bitcode ' +
                                'file first')
    argparser.add_argument('--echo-cmd', required=False,
                           action='store_true',
                           help='Trace command that generates ICE instructions')
    argparser.add_argument('llfile', nargs=1,
                           metavar='LLVM_FILE',
                           help='Llvm source file')

    args = argparser.parse_args()
    llvm_bin_path = args.llvm_bin_path
    llfile = args.llfile[0]

    cmd = []
    if args.pnacl:
      cmd = [os.path.join(llvm_bin_path, 'llvm-as'), llfile, '-o', '-', '|',
             os.path.join(llvm_bin_path, 'pnacl-freeze'),
             '--allow-local-symbol-tables', '|']
    cmd += [args.llvm2ice, '-verbose', 'inst', '-notranslate']
    if args.pnacl:
      cmd += ['--allow-local-symbol-tables', '--bitcode-format=pnacl']
    else:
      cmd.append(llfile)

    stdout_result = shellcmd(cmd, echo=args.echo_cmd)
    if not args.echo_cmd:
      sys.stdout.write(stdout_result)
