#!/usr/bin/env python2

import argparse
import itertools
import os
import re
import subprocess
import sys
import tempfile

from utils import shellcmd

def main():
    """Run the pnacl-sz compiler on an llvm file.

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
        '--pnacl-sz', required=False, default='./pnacl-sz', metavar='PNACL-SZ',
        help="Subzero translator 'pnacl-sz'")
    argparser.add_argument('--llvm-bin-path', required=False,
                           default=None, metavar='LLVM_BIN_PATH',
                           help='Path to LLVM executables ' +
                                '(for building PEXE files)')
    argparser.add_argument('--binutils-bin-path', required=False,
                           default=None, metavar='BINUTILS_BIN_PATH',
                           help='Path to Binutils executables')
    argparser.add_argument('--assemble', required=False,
                           action='store_true',
                           help='Assemble the output')
    argparser.add_argument('--disassemble', required=False,
                           action='store_true',
                           help='Disassemble the assembled output')
    argparser.add_argument('--dis-flags', required=False,
                           action='append', default=[],
                           help='Add a disassembler flag')
    argparser.add_argument('--filetype', default='iasm', dest='filetype',
                           choices=['obj', 'asm', 'iasm'],
                           help='Output file type.  Default %(default)s.')
    argparser.add_argument('--echo-cmd', required=False,
                           action='store_true',
                           help='Trace command that generates ICE instructions')
    argparser.add_argument('--args', '-a', nargs=argparse.REMAINDER,
                           default=[],
                           help='Remaining arguments are passed to pnacl-sz')

    args = argparser.parse_args()
    llvm_bin_path = args.llvm_bin_path
    binutils_bin_path = args.binutils_bin_path
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
    cmd += [args.pnacl_sz]
    if args.insts:
      # If the tests are based on '-verbose inst' output, force
      # single-threaded translation because dump output does not get
      # reassembled into order.
      cmd += ['-verbose', 'inst', '-notranslate', '-threads=0']
    if not args.llvm_source:
      cmd += ['--bitcode-format=pnacl']
      if not args.no_local_syms:
        cmd += ['--allow-local-symbol-tables']
    if args.llvm or args.llvm_source:
      cmd += ['--build-on-read=0']
    else:
      cmd += ['--build-on-read=1']
    cmd += ['--filetype=' + args.filetype]
    cmd += args.args
    if args.llvm_source:
      cmd += [llfile]
    asm_temp = None
    if args.assemble or args.disassemble:
      # On windows we may need to close the file first before it can be
      # re-opened by the other tools, so don't do delete-on-close,
      # and instead manually delete.
      asm_temp = tempfile.NamedTemporaryFile(delete=False)
      asm_temp.close()
    if args.assemble and args.filetype != 'obj':
      cmd += ['|', os.path.join(llvm_bin_path, 'llvm-mc'),
              # TODO(stichnot): -triple=i686-nacl should be used for a
              # sandboxing test.  This means there should be an args.sandbox
              # argument that also gets passed through to pnacl-sz.
              '-triple=i686',
              '-filetype=obj', '-o', asm_temp.name]
    elif asm_temp:
      cmd += ['-o', asm_temp.name]
    if args.disassemble:
      # Show wide instruction encodings, diassemble, and show relocs.
      cmd += (['&&', os.path.join(binutils_bin_path, 'le32-nacl-objdump')] +
              args.dis_flags +
              ['-w', '-d', '-r', '-Mintel', asm_temp.name])

    stdout_result = shellcmd(cmd, echo=args.echo_cmd)
    if not args.echo_cmd:
      sys.stdout.write(stdout_result)
    if asm_temp:
      os.remove(asm_temp.name)

if __name__ == '__main__':
    main()
