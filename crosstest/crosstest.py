#!/usr/bin/env python2

import argparse
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, '../pydir')
from utils import shellcmd

if __name__ == '__main__':
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
    # arch_map maps a Subzero target string to an llvm-mc -arch string.
    arch_map = { 'x8632':'x86', 'x8664':'x86-64', 'arm':'arm' }
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
                           help='Translation target architecture')
    argparser.add_argument('-O', required=False, default='2', dest='optlevel',
                           choices=['m1', '-1', '0', '1', '2'],
                           metavar='OPTLEVEL',
                           help='Optimization level ' +
                                '(m1 and -1 are equivalent)')
    argparser.add_argument('--prefix', required=True,
                           metavar='SZ_PREFIX',
                           help='String prepended to Subzero symbol names')
    argparser.add_argument('--output', '-o', required=True,
                           metavar='EXECUTABLE',
                           help='Executable to produce')
    argparser.add_argument('--dir', required=False, default='.',
                           metavar='OUTPUT_DIR',
                           help='Output directory for all files')
    argparser.add_argument('--llvm-bin-path', required=False,
                           default=os.environ.get('LLVM_BIN_PATH'),
                           metavar='PATH',
                           help='Path to LLVM executables like llc ' +
                                '(defaults to $LLVM_BIN_PATH)')
    args = argparser.parse_args()

    objs = []
    remove_internal = re.compile('^define internal ')
    fix_target = re.compile('le32-unknown-nacl')
    llvm_bin_path = args.llvm_bin_path
    for arg in args.test:
        base, ext = os.path.splitext(arg)
        if ext == '.ll':
            bitcode = arg
        else:
            bitcode = os.path.join(args.dir, base + '.pnacl.ll')
            shellcmd(['../pydir/build-pnacl-ir.py', '--disable-verify',
                      '--dir', args.dir, arg])
            # Read in the bitcode file, fix it up, and rewrite the file.
            f = open(bitcode)
            ll_lines = f.readlines()
            f.close()
            f = open(bitcode, 'w')
            for line in ll_lines:
                line = remove_internal.sub('define ', line)
                line = fix_target.sub('i686-pc-linux-gnu', line)
                f.write(line)
            f.close()

        asm_sz = os.path.join(args.dir, base + '.sz.s')
        obj_sz = os.path.join(args.dir, base + '.sz.o')
        obj_llc = os.path.join(args.dir, base + '.llc.o')
        shellcmd(['../llvm2ice',
                  '-O' + args.optlevel,
                  '--target=' + args.target,
                  '--prefix=' + args.prefix,
                  '-o=' + asm_sz,
                  bitcode])
        shellcmd([os.path.join(llvm_bin_path, 'llvm-mc'),
                  '-arch=' + arch_map[args.target],
                  '-x86-asm-syntax=intel',
                  '-filetype=obj',
                  '-o=' + obj_sz,
                  asm_sz])
        objs.append(obj_sz)
        # Each original bitcode file needs to be translated by the
        # LLVM toolchain and have its object file linked in.  There
        # are two ways to do this: explicitly use llc, or include the
        # .ll file in the link command.  It turns out that these two
        # approaches can produce different semantics on some undefined
        # bitcode behavior.  Specifically, LLVM produces different
        # results for overflowing fptoui instructions for i32 and i64
        # on x86-32.  As it turns out, Subzero lowering was based on
        # inspecting the object code produced by the direct llc
        # command, so we need to directly run llc on the bitcode, even
        # though it makes this script longer, to avoid spurious
        # failures.  This behavior can be inspected by switching
        # use_llc between True and False.
        use_llc = False
        if use_llc:
            shellcmd([os.path.join(llvm_bin_path, 'llc'),
                      '-filetype=obj',
                      '-o=' + obj_llc,
                      bitcode])
            objs.append(obj_llc)
        else:
            objs.append(bitcode)

    linker = 'clang' if os.path.splitext(args.driver)[1] == '.c' else 'clang++'
    shellcmd([os.path.join(llvm_bin_path, linker), '-g', '-m32', args.driver] +
             objs +
             ['-lm', '-o', os.path.join(args.dir, args.output)])
