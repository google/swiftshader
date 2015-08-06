#!/usr/bin/env python2

import argparse
import os
import pipes
import re
import sys

from utils import shellcmd
from utils import FindBaseNaCl

def NewerThanOrNotThere(old_path, new_path):
    """Returns whether old_path is newer than new_path.

    Also returns true if either path doesn't exist.
    """
    if not (os.path.exists(old_path) and os.path.exists(new_path)):
        return True
    return os.path.getmtime(old_path) > os.path.getmtime(new_path)

def BuildRegex(patterns, syms):
    """Build a regular expression string for inclusion or exclusion.

    Creates a regex string from an array of patterns and an array
    of symbol names.  Each element in the patterns array is either a
    regex, or a range of entries in the symbol name array, e.g. '2:9'.
    """
    pattern_list = []
    for pattern in patterns:
        if pattern[0].isdigit() or pattern[0] == ':':
            # Legitimate symbols or regexes shouldn't start with a
            # digit or a ':', so interpret the pattern as a range.
            interval = pattern.split(':')
            if len(interval) == 1:
                # Treat singleton 'n' as 'n:n+1'.
                lower = int(interval[0])
                upper = lower + 1
            elif len(interval) == 2:
                # Handle 'a:b', 'a:', and ':b' with suitable defaults.
                lower = int(interval[0]) if len(interval[0]) else 0
                upper = int(interval[1]) if len(interval[1]) else len(syms)
            else:
                print 'Invalid range syntax: {p}'.format(p=pattern)
                exit(1)
            pattern = '$|^'.join([re.escape(p) for p in syms[lower:upper]])
        pattern_list.append('^' + pattern + '$')
    return '|'.join(pattern_list) if len(pattern_list) else '^$'

def MatchSymbol(sym, re_include, re_exclude, default_match):
    """Match a symbol name against inclusion/exclusion rules.

    Returns True or False depending on whether the given symbol
    matches the compiled include or exclude regexes.  The default is
    returned if neither the include nor the exclude regex matches.
    """
    if re_exclude.match(sym):
        # Always honor an explicit exclude before considering
        # includes.
        return False
    if re_include.match(sym):
        return True
    return default_match

def AddOptionalArgs(argparser):
    argparser.add_argument('--force', dest='force', action='store_true',
                           help='Force all re-translations of the pexe')
    argparser.add_argument('--include', '-i', default=[], dest='include',
                           action='append',
                           help='Subzero symbols to include ' +
                                '(regex or line range)')
    argparser.add_argument('--exclude', '-e', default=[], dest='exclude',
                           action='append',
                           help='Subzero symbols to exclude ' +
                                '(regex or line range)')
    argparser.add_argument('--output', '-o', default='a.out', dest='output',
                           action='store',
                           help='Output executable. Default %(default)s.')
    argparser.add_argument('-O', default='2', dest='optlevel',
                           choices=['m1', '-1', '0', '1', '2'],
                           help='Optimization level ' +
                                '(m1 and -1 are equivalent).' +
                                ' Default %(default)s.')
    argparser.add_argument('--filetype', default='iasm', dest='filetype',
                           choices=['obj', 'asm', 'iasm'],
                           help='Output file type.  Default %(default)s.')
    argparser.add_argument('--sandbox', dest='sandbox', action='store_true',
                           help='Enable sandboxing in the translator')
    argparser.add_argument('--enable-block-profile',
                           dest='enable_block_profile', action='store_true',
                           help='Enable basic block profiling.')
    argparser.add_argument('--verbose', '-v', dest='verbose',
                           action='store_true',
                           help='Display some extra debugging output')
    argparser.add_argument('--sz', dest='sz_args', action='append', default=[],
                           help='Extra arguments for Subzero')
    argparser.add_argument('--llc', dest='llc_args', action='append',
                           default=[], help='Extra arguments for llc')

def main():
    """Create a hybrid translation from Subzero and llc.

    Takes a finalized pexe and builds a native executable as a hybrid of Subzero
    and llc translated bitcode.  Linker tricks are used to determine whether
    Subzero or llc generated symbols are used, on a per-symbol basis.

    By default, for every symbol, its Subzero version is used.  Subzero and llc
    symbols can be selectively enabled/disabled via regular expressions on the
    symbol name, or by ranges of lines in this program's auto-generated symbol
    file.

    For each symbol, the --exclude arguments are first checked (the symbol is
    'rejected' on a match), followed by the --include arguments (the symbol is
    'accepted' on a match), followed by unconditional 'rejection'.  The Subzero
    version is used for an 'accepted' symbol, and the llc version is used for a
    'rejected' symbol.

    Each --include and --exclude argument can be a regular expression or a range
    of lines in the symbol file.  Each regular expression is wrapped inside
    '^$', so if you want a substring match on 'foo', use '.*foo.*' instead.
    Ranges use python-style 'first:last' notation, so e.g. use '0:10' or ':10'
    for the first 10 lines of the file, or '1' for the second line of the file.

    If no --include or --exclude arguments are given, the executable is produced
    entirely using Subzero, without using llc or linker tricks.

    This script uses file modification timestamps to determine whether llc and
    Subzero re-translation are needed.  It checks timestamps of llc, pnacl-sz,
    and the pexe against the translated object files to determine the minimal
    work necessary.  The --force option suppresses those checks and
    re-translates everything.

    This script augments PATH so that various PNaCl and LLVM tools can be run.
    These extra paths are within the native_client tree.  When changes are made
    to these tools, copy them this way:
      cd native_client
      toolchain_build/toolchain_build_pnacl.py llvm_x86_64_linux \\
      --install=toolchain/linux_x86/pnacl_newlib_raw
    """
    argparser = argparse.ArgumentParser(
        description='    ' + main.__doc__,
        formatter_class=argparse.RawTextHelpFormatter)
    AddOptionalArgs(argparser)
    argparser.add_argument('pexe', help='Finalized pexe to translate')
    args = argparser.parse_args()
    pexe = args.pexe
    exe = args.output
    ProcessPexe(args, pexe, exe)

def ProcessPexe(args, pexe, exe):
    [pexe_base, ext] = os.path.splitext(pexe)
    if ext != '.pexe':
        pexe_base = pexe
    pexe_base_unescaped = pexe_base
    pexe_base = pipes.quote(pexe_base)
    pexe = pipes.quote(pexe)

    nacl_root = FindBaseNaCl()
    path_addition = (
        '{root}/toolchain/linux_x86/pnacl_newlib_raw/bin'
        ).format(root=nacl_root)
    os.environ['PATH'] = (
        '{dir}{sep}{path}'
        ).format(dir=path_addition, sep=os.pathsep, path=os.environ['PATH'])
    obj_llc = pexe_base + '.llc.o'
    obj_sz = pexe_base + '.sz.o'
    asm_sz = pexe_base + '.sz.s'
    obj_llc_weak = pexe_base + '.weak.llc.o'
    obj_sz_weak = pexe_base + '.weak.sz.o'
    obj_partial = obj_sz  # overridden for hybrid mode
    sym_llc = pexe_base + '.sym.llc.txt'
    sym_sz = pexe_base + '.sym.sz.txt'
    sym_sz_unescaped = pexe_base_unescaped + '.sym.sz.txt'
    whitelist_sz = pexe_base + '.wl.sz.txt'
    whitelist_sz_unescaped = pexe_base_unescaped + '.wl.sz.txt'
    pnacl_sz = (
        '{root}/toolchain_build/src/subzero/pnacl-sz'
        ).format(root=nacl_root)
    llcbin = '{base}/pnacl-llc'.format(base=path_addition)
    gold = 'le32-nacl-ld.gold'
    objcopy = 'le32-nacl-objcopy'
    opt_level = args.optlevel
    opt_level_map = { 'm1':'0', '-1':'0', '0':'0', '1':'1', '2':'2' }
    hybrid = args.include or args.exclude

    if hybrid and (args.force or
                   NewerThanOrNotThere(pexe, obj_llc) or
                   NewerThanOrNotThere(llcbin, obj_llc)):
        # Only run pnacl-translate in hybrid mode.
        shellcmd(['pnacl-translate',
                  '-split-module=1',
                  '-ffunction-sections',
                  '-fdata-sections',
                  '-c',
                  '-arch', 'x86-32' if args.sandbox else 'x86-32-linux',
                  '-O' + opt_level_map[opt_level],
                  '--pnacl-driver-append-LLC_FLAGS_EXTRA=-externalize',
                  '-o', obj_llc] +
                 (['--pnacl-driver-verbose'] if args.verbose else []) +
                 args.llc_args +
                 [pexe],
                 echo=args.verbose)
        if not args.sandbox:
            shellcmd((
                '{objcopy} --redefine-sym _start=_user_start {obj}'
                ).format(objcopy=objcopy, obj=obj_llc), echo=args.verbose)
        # Generate llc syms file for consistency, even though it's not used.
        shellcmd((
            'nm {obj} | sed -n "s/.* [a-zA-Z] //p" > {sym}'
            ).format(obj=obj_llc, sym=sym_llc), echo=args.verbose)

    if (args.force or
        NewerThanOrNotThere(pexe, obj_sz) or
        NewerThanOrNotThere(pnacl_sz, obj_sz)):
        # Run pnacl-sz regardless of hybrid mode.
        shellcmd([pnacl_sz,
                  '-O' + opt_level,
                  '-bitcode-format=pnacl',
                  '-filetype=' + args.filetype,
                  '-o', obj_sz if args.filetype == 'obj' else asm_sz] +
                 (['-externalize',
                   '-ffunction-sections',
                   '-fdata-sections'] if hybrid else []) +
                 (['-sandbox'] if args.sandbox else []) +
                 (['-enable-block-profile'] if
                      args.enable_block_profile and not args.sandbox else []) +
                 args.sz_args +
                 [pexe],
                 echo=args.verbose)
        if args.filetype != 'obj':
            shellcmd((
                'llvm-mc -triple={triple} -filetype=obj -o {obj} {asm}'
                ).format(asm=asm_sz, obj=obj_sz,
                         triple='i686-nacl' if args.sandbox else 'i686'),
                     echo=args.verbose)
        if not args.sandbox:
            shellcmd((
                '{objcopy} --redefine-sym _start=_user_start {obj}'
                ).format(objcopy=objcopy, obj=obj_sz), echo=args.verbose)
        if hybrid:
            shellcmd((
                'nm {obj} | sed -n "s/.* [a-zA-Z] //p" > {sym}'
                ).format(obj=obj_sz, sym=sym_sz), echo=args.verbose)

    if hybrid:
        with open(sym_sz_unescaped) as f:
            sz_syms = f.read().splitlines()
        re_include_str = BuildRegex(args.include, sz_syms)
        re_exclude_str = BuildRegex(args.exclude, sz_syms)
        re_include = re.compile(re_include_str)
        re_exclude = re.compile(re_exclude_str)
        # If a symbol doesn't explicitly match re_include or re_exclude,
        # the default MatchSymbol() result is True, unless some --include
        # args are provided.
        default_match = not args.include

        whitelist_has_items = False
        with open(whitelist_sz_unescaped, 'w') as f:
            for sym in sz_syms:
                if MatchSymbol(sym, re_include, re_exclude, default_match):
                    f.write(sym + '\n')
                    whitelist_has_items = True
        shellcmd((
            '{objcopy} --weaken {obj} {weak}'
            ).format(objcopy=objcopy, obj=obj_sz, weak=obj_sz_weak),
            echo=args.verbose)
        if whitelist_has_items:
            # objcopy returns an error if the --weaken-symbols file is empty.
            shellcmd((
                '{objcopy} --weaken-symbols={whitelist} {obj} {weak}'
                ).format(objcopy=objcopy,
                         whitelist=whitelist_sz, obj=obj_llc,
                         weak=obj_llc_weak),
                     echo=args.verbose)
        else:
            shellcmd((
                '{objcopy} {obj} {weak}'
                ).format(objcopy=objcopy, obj=obj_llc, weak=obj_llc_weak),
                echo=args.verbose)
        obj_partial = pexe_base + '.o'
        shellcmd((
            'ld -r -m elf_i386 -o {partial} {sz} {llc}'
            ).format(partial=obj_partial, sz=obj_sz_weak, llc=obj_llc_weak),
                 echo=args.verbose)
        shellcmd((
            '{objcopy} -w --localize-symbol="*" {partial}'
            ).format(objcopy=objcopy, partial=obj_partial),
            echo=args.verbose)
        shellcmd((
            '{objcopy} --globalize-symbol={start} ' +
            '--globalize-symbol=__Sz_block_profile_info {partial}'
            ).format(objcopy=objcopy, partial=obj_partial,
                     start='_start' if args.sandbox else '_user_start'),
                 echo=args.verbose)

    # Run the linker regardless of hybrid mode.
    linker = (
        '{root}/../third_party/llvm-build/Release+Asserts/bin/clang'
        ).format(root=nacl_root)
    if args.sandbox:
        linklib = ('{root}/toolchain/linux_x86/pnacl_newlib_raw/translator/' +
                   'x86-32/lib').format(root=nacl_root)
        shellcmd((
            '{gold} -nostdlib --no-fix-cortex-a8 --eh-frame-hdr -z text ' +
            '--build-id --entry=__pnacl_start -static ' +
            '{linklib}/crtbegin.o {partial} ' +
            '{root}/toolchain_build/src/subzero/build/runtime/' +
            'szrt_sb_x8632.o ' +
            '{linklib}/libpnacl_irt_shim_dummy.a --start-group ' +
            '{linklib}/libgcc.a {linklib}/libcrt_platform.a ' +
            '--end-group {linklib}/crtend.o --undefined=_start ' +
            '--defsym=__Sz_AbsoluteZero=0 ' +
            '-o {exe}'
            ).format(gold=gold, linklib=linklib, partial=obj_partial, exe=exe,
                     root=nacl_root),
                 echo=args.verbose)
    else:
        shellcmd((
            '{ld} -m32 {partial} -o {exe} ' +
            # Keep the rest of this command line (except szrt_native_x8632.o) in
            # sync with RunHostLD() in pnacl-translate.py.
            '{root}/toolchain/linux_x86/pnacl_newlib_raw/translator/' +
            'x86-32-linux/lib/' +
            '{{unsandboxed_irt,irt_random,irt_query_list}}.o ' +
            '{root}/toolchain_build/src/subzero/build/runtime/' +
            'szrt_native_x8632.o -lpthread -lrt ' +
            '-Wl,--defsym=__Sz_AbsoluteZero=0'
            ).format(ld=linker, partial=obj_partial, exe=exe, root=nacl_root),
                 echo=args.verbose)

    # Put the extra verbose printing at the end.
    if args.verbose:
        print 'PATH: {path}'.format(path=path_addition)
        if hybrid:
            print 'include={regex}'.format(regex=re_include_str)
            print 'exclude={regex}'.format(regex=re_exclude_str)
            print 'default_match={dm}'.format(dm=default_match)
            print 'Number of Subzero syms = {num}'.format(num=len(sz_syms))

if __name__ == '__main__':
    main()
