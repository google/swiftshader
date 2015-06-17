#!/usr/bin/env python2

import argparse
import os
import shutil
import tempfile

import targets
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
    shellcmd(['le32-nacl-objcopy',
              '--strip-symbol=nacl_tp_tdb_offset',
              '--strip-symbol=nacl_tp_tls_offset',
              obj
        ], echo=verbose)


def PartialLink(obj_files, extra_args, lib, verbose):
    """Partially links a set of obj files into a final obj library."""
    shellcmd(['le32-nacl-ld',
              '-o', lib,
              '-r',
        ] + extra_args + obj_files, echo=verbose)


def MakeRuntimesForTarget(target_info, ll_files,
                          srcdir, tempdir, rtdir, verbose):
    def TmpFile(template):
        return template.format(dir=tempdir, target=target_info.target)
    def OutFile(template):
        return template.format(rtdir=rtdir, target=target_info.target)
    # Translate tempdir/szrt.ll and tempdir/szrt_ll.ll to
    # szrt_native_{target}.tmp.o.
    Translate(ll_files,
              ['-mtriple=' + target_info.triple] + target_info.llc_flags,
              TmpFile('{dir}/szrt_native_{target}.tmp.o'),
              verbose)
    # Compile srcdir/szrt_profiler.c to tempdir/szrt_profiler_native_{target}.o
    shellcmd(['clang',
              '-O2',
              '-target=' + target_info.triple,
              '-c',
              '{srcdir}/szrt_profiler.c'.format(srcdir=srcdir),
              '-o', TmpFile('{dir}/szrt_profiler_native_{target}.o')
    ], echo=verbose)
    # Writing full szrt_native_{target}.o.
    PartialLink([TmpFile('{dir}/szrt_native_{target}.tmp.o'),
                 TmpFile('{dir}/szrt_profiler_native_{target}.o')],
                ['-m {ld_emu}'.format(ld_emu=target_info.ld_emu)],
                OutFile('{rtdir}/szrt_native_{target}.o'),
                verbose)

    # Translate tempdir/szrt.ll and tempdir/szrt_ll.ll to szrt_sb_{target}.o
    # The sandboxed library does not get the profiler helper function as the
    # binaries are linked with -nostdlib.
    Translate(ll_files,
              ['-mtriple=' + targets.ConvertTripleToNaCl(target_info.triple)] +
              target_info.llc_flags,
              OutFile('{rtdir}/szrt_sb_{target}.o'),
              verbose)


def main():
    """Build the Subzero runtime support library for all architectures.
    """
    argparser = argparse.ArgumentParser(
        description='    ' + main.__doc__,
        formatter_class=argparse.RawTextHelpFormatter)
    argparser.add_argument('--verbose', '-v', dest='verbose',
                           action='store_true',
                           help='Display some extra debugging output')
    argparser.add_argument('--pnacl-root', dest='pnacl_root',
                           help='Path to PNaCl toolchain binaries.')
    args = argparser.parse_args()
    nacl_root = FindBaseNaCl()
    os.environ['PATH'] = ('{root}/bin{sep}{path}'
        ).format(root=args.pnacl_root, sep=os.pathsep, path=os.environ['PATH'])
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

        MakeRuntimesForTarget(targets.X8632Target, ll_files,
                              srcdir, tempdir, rtdir, args.verbose)
        MakeRuntimesForTarget(targets.ARM32Target, ll_files,
                              srcdir, tempdir, rtdir, args.verbose)

    finally:
        try:
            shutil.rmtree(tempdir)
        except OSError as exc:
            if exc.errno != errno.ENOENT: # ENOENT - no such file or directory
                raise # re-raise exception

if __name__ == '__main__':
    main()
