#!/usr/bin/env python2

import argparse
import os
import subprocess
import sys
import tempfile

from utils import shellcmd
from utils import FindBaseNaCl

def main():
    """Builds a cross-test binary that allows functions translated by
    Subzero and llc to be compared.

    Each --test argument is compiled once by llc and once by Subzero.
    C/C++ tests are first compiled down to PNaCl bitcode by the
    build-pnacl-ir.py script.  The --prefix argument ensures that
    symbol names are different between the two object files, to avoid
    linking errors.

    There is also a --driver argument that specifies the C/C++ file
    that calls the test functions with a variety of interesting inputs
    and compares their results.
    """
    # arch_map maps a Subzero target string to an llvm-mc -triple string.
    arch_map = { 'x8632':'i686', 'x8664':'x86_64', 'arm':'armv7a' }
    desc = 'Build a cross-test that compares Subzero and llc translation.'
    argparser = argparse.ArgumentParser(description=desc)
    argparser.add_argument('--test', required=True, action='append',
                           metavar='TESTFILE_LIST',
                           help='List of C/C++/.ll files with test functions')
    argparser.add_argument('--driver', required=True,
                           metavar='DRIVER',
                           help='Driver program')
    argparser.add_argument('--target', required=False, default='x8632',
                           choices=arch_map.keys(),
                           metavar='TARGET',
                           help='Translation target architecture.' +
                                ' Default %(default)s.')
    argparser.add_argument('-O', required=False, default='2', dest='optlevel',
                           choices=['m1', '-1', '0', '1', '2'],
                           metavar='OPTLEVEL',
                           help='Optimization level ' +
                                '(m1 and -1 are equivalent).' +
                                ' Default %(default)s.')
    argparser.add_argument('--mattr',  required=False, default='sse2',
                           dest='attr', choices=['sse2', 'sse4.1'],
                           metavar='ATTRIBUTE',
                           help='Target attribute. Default %(default)s.')
    argparser.add_argument('--sandbox', required=False, default=0, type=int,
                           dest='sandbox',
                           help='Use sandboxing. Default "%(default)s".')
    argparser.add_argument('--prefix', required=True,
                           metavar='SZ_PREFIX',
                           help='String prepended to Subzero symbol names')
    argparser.add_argument('--output', '-o', required=True,
                           metavar='EXECUTABLE',
                           help='Executable to produce')
    argparser.add_argument('--dir', required=False, default='.',
                           metavar='OUTPUT_DIR',
                           help='Output directory for all files.' +
                                ' Default "%(default)s".')
    argparser.add_argument('--crosstest-bitcode', required=False,
                           default=1, type=int,
                           help='Compile non-subzero crosstest object file ' +
                           'from the same bitcode as the subzero object. ' +
                           'If 0, then compile it straight from source.' +
                           ' Default %(default)d.')
    argparser.add_argument('--filetype', default='obj', dest='filetype',
                           choices=['obj', 'asm', 'iasm'],
                           help='Output file type.  Default %(default)s.')
    args = argparser.parse_args()

    nacl_root = FindBaseNaCl()
    bindir = ('{root}/toolchain/linux_x86/pnacl_newlib/bin'
              .format(root=nacl_root))
    triple = arch_map[args.target] + ('-nacl' if args.sandbox else '')

    objs = []
    for arg in args.test:
        base, ext = os.path.splitext(arg)
        if ext == '.ll':
            bitcode = arg
        else:
            bitcode = os.path.join(args.dir, base + '.pnacl.ll')
            shellcmd(['../pydir/build-pnacl-ir.py', '--disable-verify',
                      '--dir', args.dir, arg])

        base_sz = '{base}.{sb}.O{opt}.{attr}.{target}'.format(
            base=base, sb='sb' if args.sandbox else 'nat', opt=args.optlevel,
            attr=args.attr, target=args.target)
        asm_sz = os.path.join(args.dir, base_sz + '.sz.s')
        obj_sz = os.path.join(args.dir, base_sz + '.sz.o')
        obj_llc = os.path.join(args.dir, base_sz + '.llc.o')
        shellcmd(['../pnacl-sz',
                  '-O' + args.optlevel,
                  '-mattr=' + args.attr,
                  '--target=' + args.target,
                  '--sandbox=' + str(args.sandbox),
                  '--prefix=' + args.prefix,
                  '-allow-uninitialized-globals',
                  '-externalize',
                  '-filetype=' + args.filetype,
                  '-o=' + (obj_sz if args.filetype == 'obj' else asm_sz),
                  bitcode])
        if args.filetype != 'obj':
            shellcmd(['{bin}/llvm-mc'.format(bin=bindir),
                      '-triple=' + triple,
                      '-filetype=obj',
                      '-o=' + obj_sz,
                      asm_sz])
        objs.append(obj_sz)
        if args.crosstest_bitcode:
            shellcmd(['{bin}/pnacl-llc'.format(bin=bindir),
                      '-mtriple=' + triple,
                      # Use sse2 instructions regardless of input -mattr
                      # argument to avoid differences in (undefined) behavior of
                      # converting NaN to int.
                      '-mattr=sse2',
                      '-externalize',
                      '-filetype=obj',
                      '-o=' + obj_llc,
                      bitcode])
            objs.append(obj_llc)
        else:
            objs.append(arg)

    # Add szrt_sb_x8632.o or szrt_native_x8632.o.
    objs.append((
            '{root}/toolchain_build/src/subzero/build/runtime/' +
            'szrt_{sb}_' + args.target + '.o'
            ).format(root=nacl_root, sb='sb' if args.sandbox else 'native'))
    pure_c = os.path.splitext(args.driver)[1] == '.c'
    # Set compiler to clang, clang++, pnacl-clang, or pnacl-clang++.
    compiler = '{bin}/{prefix}{cc}'.format(
        bin=bindir, prefix='pnacl-' if args.sandbox else '',
        cc='clang' if pure_c else 'clang++')
    sb_native_args = (['-O0', '--pnacl-allow-native', '-arch', 'x8632']
                      if args.sandbox else
                      ['-g', '-m32', '-lm', '-lpthread'])
    shellcmd([compiler, args.driver] + objs +
             ['-o', os.path.join(args.dir, args.output)] + sb_native_args)

if __name__ == '__main__':
    main()
