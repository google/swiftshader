#!/usr/bin/env python2

import argparse
import os
import sys
import tempfile
from utils import shellcmd

if __name__ == '__main__':
    argparser = argparse.ArgumentParser()
    argparser.add_argument('cfile', nargs='+', type=str,
        help='C file(s) to convert')
    argparser.add_argument('--nacl_sdk_root', nargs='?', type=str,
        help='Path to NACL_SDK_ROOT')
    argparser.add_argument('--dir', nargs='?', type=str, default='.',
                           help='Output directory')
    argparser.add_argument('--disable-verify', action='store_true')
    args = argparser.parse_args()

    nacl_sdk_root = os.environ.get('NACL_SDK_ROOT', None)
    if args.nacl_sdk_root:
        nacl_sdk_root = os.path.expanduser(args.nacl_sdk_root)

    if not nacl_sdk_root or not os.path.exists(nacl_sdk_root):
        print '''\
Please set the NACL_SDK_ROOT environment variable or pass the path through
--nacl_sdk_root to point to a valid Native Client SDK installation.'''
        sys.exit(1)

    includes_path = os.path.join(nacl_sdk_root, 'include')
    toolchain_path = os.path.join(nacl_sdk_root, 'toolchain', 'linux_pnacl')
    clang_path = os.path.join(toolchain_path, 'bin64', 'pnacl-clang')
    opt_path = os.path.join(toolchain_path, 'host_x86_64', 'bin', 'opt')

    tempdir = tempfile.mkdtemp()

    for cname in args.cfile:
        basename = os.path.splitext(cname)[0]
        llname = os.path.join(tempdir, basename + '.ll')
        pnaclname = basename + '.pnacl.ll'
        pnaclname = os.path.join(args.dir, pnaclname)

        shellcmd(clang_path + ' -O2 -I{0} -c {1} -o {2}'.format(
            includes_path, cname, llname))
        shellcmd(opt_path +
            ' -pnacl-abi-simplify-preopt -pnacl-abi-simplify-postopt' +
            ('' if args.disable_verify else
             ' -verify-pnaclabi-module -verify-pnaclabi-functions') +
            ' -pnaclabi-allow-debug-metadata -disable-simplify-libcalls'
            ' {0} -S -o {1}'.format(llname, pnaclname))
