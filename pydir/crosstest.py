#!/usr/bin/env python2

import argparse
import os
import subprocess
import sys
import tempfile

import targets
from utils import shellcmd
from utils import FindBaseNaCl

def main():
    """Builds a cross-test binary for comparing Subzero and llc translation.

    Each --test argument is compiled once by llc and once by Subzero.  C/C++
    tests are first compiled down to PNaCl bitcode using pnacl-clang and
    pnacl-opt.  The --prefix argument ensures that symbol names are different
    between the two object files, to avoid linking errors.

    There is also a --driver argument that specifies the C/C++ file that calls
    the test functions with a variety of interesting inputs and compares their
    results.

    """
    # arch_map maps a Subzero target string to TargetInfo (e.g., triple).
    arch_map = { 'x8632': targets.X8632Target,
                 'x8664': targets.X8664Target,
                 'arm32': targets.ARM32Target }
    arch_sz_flags = { 'x8632': [],
                      'x8664': [],
                      # TODO(jvoung): remove skip-unimplemented when
                      # implemented.
                      # For ARM, test a large stack offset as well, until we
                      # are more confident. +/- 4095 is the limit, so test
                      # somewhere near that boundary.
                      'arm32': ['--skip-unimplemented',
                                '--test-stack-extra', '4084']
    }
    arch_llc_flags_extra = {
        # Use sse2 instructions regardless of input -mattr
        # argument to avoid differences in (undefined) behavior of
        # converting NaN to int.
        'x8632': ['-mattr=sse2'],
        'x8664': ['-mattr=sse2'],
        'arm32': [],
    }
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
                           help='Optimization level for llc and Subzero ' +
                                '(m1 and -1 are equivalent).' +
                                ' Default %(default)s.')
    argparser.add_argument('--clang-opt', required=False, default=True,
                           dest='clang_opt')
    argparser.add_argument('--mattr',  required=False, default='sse2',
                           dest='attr', choices=['sse2', 'sse4.1',
                                                 'neon', 'hwdiv-arm'],
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
    argparser.add_argument('--sz', dest='sz_args', action='append', default=[],
                           help='Extra arguments to pass to pnacl-sz.')
    args = argparser.parse_args()

    nacl_root = FindBaseNaCl()
    bindir = ('{root}/toolchain/linux_x86/pnacl_newlib_raw/bin'
              .format(root=nacl_root))
    target_info = arch_map[args.target]
    triple = target_info.triple
    if args.sandbox:
        triple = targets.ConvertTripleToNaCl(triple)
    llc_flags = target_info.llc_flags + arch_llc_flags_extra[args.target]
    mypath = os.path.abspath(os.path.dirname(sys.argv[0]))

    objs = []
    for arg in args.test:
        # Construct a "unique key" for each test so that tests can be run in
        # parallel without race conditions on temporary file creation.
        key = '{target}.{sb}.O{opt}.{attr}'.format(
            target=args.target, sb='sb' if args.sandbox else 'nat',
            opt=args.optlevel, attr=args.attr)
        base, ext = os.path.splitext(arg)
        if ext == '.ll':
            bitcode = arg
        else:
            # Use pnacl-clang and pnacl-opt to produce PNaCl bitcode.
            bitcode_nonfinal = os.path.join(args.dir, base + '.' + key + '.bc')
            bitcode = os.path.join(args.dir, base + '.' + key + '.pnacl.ll')
            shellcmd(['{bin}/pnacl-clang'.format(bin=bindir),
                      ('-O2' if args.clang_opt else '-O0'),
                      ('-DARM32' if args.target == 'arm32' else ''), '-c', arg,
                      '-o', bitcode_nonfinal])
            shellcmd(['{bin}/pnacl-opt'.format(bin=bindir),
                      '-pnacl-abi-simplify-preopt',
                      '-pnacl-abi-simplify-postopt',
                      '-pnaclabi-allow-debug-metadata',
                      bitcode_nonfinal, '-S', '-o', bitcode])

        base_sz = '{base}.{key}'.format(base=base, key=key)
        asm_sz = os.path.join(args.dir, base_sz + '.sz.s')
        obj_sz = os.path.join(args.dir, base_sz + '.sz.o')
        obj_llc = os.path.join(args.dir, base_sz + '.llc.o')

        shellcmd(['{path}/pnacl-sz'.format(path=os.path.dirname(mypath)),
                  ] + args.sz_args + [
                  '-O' + args.optlevel,
                  '-mattr=' + args.attr,
                  '--target=' + args.target,
                  '--sandbox=' + str(args.sandbox),
                  '--prefix=' + args.prefix,
                  '-allow-uninitialized-globals',
                  '-externalize',
                  '-filetype=' + args.filetype,
                  '-o=' + (obj_sz if args.filetype == 'obj' else asm_sz),
                  bitcode] + arch_sz_flags[args.target])
        if args.filetype != 'obj':
            shellcmd(['{bin}/llvm-mc'.format(bin=bindir),
                      '-triple=' + triple,
                      '-filetype=obj',
                      '-o=' + obj_sz,
                      asm_sz])

        # Each separately translated Subzero object file contains its own
        # definition of the __Sz_block_profile_info profiling symbol.  Avoid
        # linker errors (multiply defined symbol) by making all copies weak.
        # (This could also be done by Subzero if it supported weak symbol
        # definitions.)  This approach should be OK because cross tests are
        # currently the only situation where multiple translated files are
        # linked into the executable, but when PNaCl supports shared nexe
        # libraries, this would need to change.
        shellcmd(['{bin}/le32-nacl-objcopy'.format(bin=bindir),
                  '--weaken-symbol=__Sz_block_profile_info', obj_sz])
        objs.append(obj_sz)
        if args.crosstest_bitcode:
            shellcmd(['{bin}/pnacl-llc'.format(bin=bindir),
                      '-mtriple=' + triple,
                      '-externalize',
                      '-filetype=obj',
                      '-o=' + obj_llc,
                      bitcode] + llc_flags)
            objs.append(obj_llc)
        else:
            objs.append(arg)

    # Add szrt_sb_${target}.o or szrt_native_${target}.o.
    objs.append((
            '{root}/toolchain_build/src/subzero/build/runtime/' +
            'szrt_{sb}_' + args.target + '.o'
            ).format(root=nacl_root, sb='sb' if args.sandbox else 'native'))
    pure_c = os.path.splitext(args.driver)[1] == '.c'

    # TODO(jpp): clean up stack hack related code.
    needs_stack_hack = False
    target_params = []
    if needs_stack_hack:
      shellcmd('{bin}/clang -g -o stack_hack.x8664.{key}.o -c '
               'stack_hack.x8664.c'.format(bin=bindir, key=key))
      target_params.append('-DX8664_STACK_HACK')
      target_params.append('stack_hack.x8664.{key}.o'.format(key=key))

    if args.target == 'arm32':
      target_params.append('-DARM32')
      target_params.append('-static')

    # Set compiler to clang, clang++, pnacl-clang, or pnacl-clang++.
    compiler = '{bin}/{prefix}{cc}'.format(
        bin=bindir, prefix='pnacl-' if args.sandbox else '',
        cc='clang' if pure_c else 'clang++')
    sb_native_args = (['-O0', '--pnacl-allow-native',
                       '-arch', target_info.compiler_arch,
                       '-Wn,-defsym=__Sz_AbsoluteZero=0']
                      if args.sandbox else
                      ['-g', '-target=' + triple,
                       '-lm', '-lpthread',
                       '-Wl,--defsym=__Sz_AbsoluteZero=0'] +
                      target_info.cross_headers)
    shellcmd([compiler] + target_params + [args.driver] + objs +
             ['-o', os.path.join(args.dir, args.output)] + sb_native_args)

if __name__ == '__main__':
    main()
