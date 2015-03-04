#!/usr/bin/env python2

import argparse
import os
import shutil
import tempfile
from utils import shellcmd
from utils import FindBaseNaCl

def Translate(ll_files, extra_args, obj, verbose):
    """Translate a set of input bitcode files into a single object file.

    Use pnacl-llc to translate textual bitcode input ll_files into object file
    obj, using extra_args as the architectural flags.
    """
    shellcmd(['cat'] + ll_files + ['|',
              'pnacl-llc',
              '-externalize',
              '-function-sections',
              '-O2',
              '-filetype=obj',
              '-bitcode-format=llvm',
              '-o', obj
          ] + extra_args, echo=verbose)
    shellcmd(['objcopy',
              '--localize-symbol=nacl_tp_tdb_offset',
              '--localize-symbol=nacl_tp_tls_offset',
              obj
        ], echo=verbose)

def main():
    """Build the Subzero runtime support library for all architectures.
    """
    argparser = argparse.ArgumentParser(
        description='    ' + main.__doc__,
        formatter_class=argparse.RawTextHelpFormatter)
    argparser.add_argument('--verbose', '-v', dest='verbose',
                           action='store_true',
                           help='Display some extra debugging output')
    args = argparser.parse_args()
    nacl_root = FindBaseNaCl()
    os.environ['PATH'] = (
        '{root}/toolchain/linux_x86/pnacl_newlib/bin{sep}' +
        '{path}'
        ).format(root=nacl_root, sep=os.pathsep, path=os.environ['PATH'])
    srcdir = (
        '{root}/toolchain_build/src/subzero/runtime'
        ).format(root=nacl_root)
    rtdir = (
        '{root}/toolchain_build/src/subzero/build/runtime'
        ).format(root=nacl_root)
    try:
        tempdir = tempfile.mkdtemp()
        if os.path.exists(rtdir) and not os.path.isdir(rtdir):
            os.remove(rtdir)
        if not os.path.exists(rtdir):
            os.makedirs(rtdir)
        # Compile srcdir/szrt.c to tempdir/szrt.ll
        shellcmd(['pnacl-clang',
                  '-O2',
                  '-c',
                  '{srcdir}/szrt.c'.format(srcdir=srcdir),
                  '-o', '{dir}/szrt.tmp.bc'.format(dir=tempdir)
            ], echo=args.verbose)
        shellcmd(['pnacl-opt',
                  '-pnacl-abi-simplify-preopt',
                  '-pnacl-abi-simplify-postopt',
                  '-pnaclabi-allow-debug-metadata',
                  '{dir}/szrt.tmp.bc'.format(dir=tempdir),
                  '-S',
                  '-o', '{dir}/szrt.ll'.format(dir=tempdir)
            ], echo=args.verbose)
        ll_files = ['{dir}/szrt.ll'.format(dir=tempdir),
                    '{srcdir}/szrt_ll.ll'.format(srcdir=srcdir)]
        # Translate tempdir/szrt.ll and srcdir/szrt_ll.ll to szrt_native_x8632.o
        Translate(ll_files,
                  ['-mtriple=i686', '-mcpu=pentium4m'],
                  '{rtdir}/szrt_native_x8632.o'.format(rtdir=rtdir),
                  args.verbose)
        # Translate tempdir/szrt.ll and srcdir/szrt_ll.ll to szrt_sb_x8632.o
        Translate(ll_files,
                  ['-mtriple=i686-nacl', '-mcpu=pentium4m'],
                  '{rtdir}/szrt_sb_x8632.o'.format(rtdir=rtdir),
                  args.verbose)
    finally:
        try:
            shutil.rmtree(tempdir)
        except OSError as exc:
            if exc.errno != errno.ENOENT: # ENOENT - no such file or directory
                raise # re-raise exception

if __name__ == '__main__':
    main()
