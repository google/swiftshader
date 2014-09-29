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
    argparser.add_argument('--verbose', '-v', dest='verbose',
                           action='store_true',
                           help='Display some extra debugging output')
    argparser.add_argument('--sz', dest='sz_args', action='append', default=[],
                           help='Extra arguments for Subzero')
    argparser.add_argument('--llc', dest='llc_args', action='append',
                           default=[], help='Extra arguments for llc')

def main():
    """Create a hybrid translation from Subzero and llc.

    Takes a finalized pexe and builds a native executable as a
    hybrid of Subzero and llc translated bitcode.  Linker tricks are
    used to determine whether Subzero or llc generated symbols are
    used, on a per-symbol basis.

    By default, for every symbol, its llc version is used.  Subzero
    symbols can be enabled by regular expressions on the symbol name,
    or by ranges of lines in this program's auto-generated symbol
    file.

    For each symbol, the --exclude arguments are first checked (the
    symbol is 'rejected' on a match), followed by the --include
    arguments (the symbol is 'accepted' on a match), followed by
    unconditional 'rejection'.  The Subzero version is used for an
    'accepted' symbol, and the llc version is used for a 'rejected'
    symbol.

    Each --include and --exclude argument can be a regular expression
    or a range of lines in the symbol file.  Each regular expression
    is wrapped inside '^$', so if you want a substring match on 'foo',
    use '.*foo.*' instead.  Ranges use python-style 'first:last'
    notation, so e.g. use '0:10' or ':10' for the first 10 lines of
    the file, or '1' for the second line of the file.

    This script uses file modification timestamps to determine whether
    llc and Subzero re-translation are needed.  It checks timestamps
    of llc, llvm2ice, and the pexe against the translated object files
    to determine the minimal work necessary.  The --force option
    suppresses those checks and re-translates everything.

    This script augments PATH so that various PNaCl and LLVM tools can
    be run.  These extra paths are within the native_client tree.
    When changes are made to these tools, copy them this way:
      cd native_client
      toolchain_build/toolchain_build_pnacl.py llvm_x86_64_linux \\
      --install=toolchain/linux_x86/pnacl_newlib
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
    os.environ['PATH'] = (
        '{root}/toolchain/linux_x86/pnacl_newlib/bin{sep}' +
        '{path}'
        ).format(root=nacl_root, sep=os.pathsep, path=os.environ['PATH'])
    obj_llc = pexe_base + '.llc.o'
    obj_sz = pexe_base + '.sz.o'
    asm_sz = pexe_base + '.sz.s'
    obj_llc_weak = pexe_base + '.weak.llc.o'
    obj_sz_weak = pexe_base + '.weak.sz.o'
    obj_partial = pexe_base + '.o'
    sym_llc = pexe_base + '.sym.llc.txt'
    sym_sz = pexe_base + '.sym.sz.txt'
    sym_sz_unescaped = pexe_base_unescaped + '.sym.sz.txt'
    whitelist_sz = pexe_base + '.wl.sz.txt'
    whitelist_sz_unescaped = pexe_base_unescaped + '.wl.sz.txt'
    llvm2ice = (
        '{root}/toolchain_build/src/subzero/llvm2ice'
        ).format(root=nacl_root)
    llcbin = (
        '{root}/toolchain/linux_x86/pnacl_newlib/bin/llc'
        ).format(root=nacl_root)
    opt_level = args.optlevel

    if args.force or NewerThanOrNotThere(pexe, obj_llc) or \
            NewerThanOrNotThere(llcbin, obj_llc):
        opt_level_map = { 'm1':'0', '-1':'0', '0':'0', '1':'1', '2':'2' }
        shellcmd(['pnacl-translate',
                  '-ffunction-sections',
                  '-c',
                  '-arch', 'x86-32-linux',
                  '-O' + opt_level_map[opt_level],
                  '--pnacl-driver-append-LLC_FLAGS_EXTRA=-externalize',
                  '-o', obj_llc] +
                 args.llc_args +
                 [pexe],
                 echo=args.verbose)
        shellcmd((
            'objcopy --redefine-sym _start=_user_start {obj}'
            ).format(obj=obj_llc), echo=args.verbose)
        shellcmd((
            'nm {obj} | sed -n "s/.* [a-zA-Z] //p" > {sym}'
            ).format(obj=obj_llc, sym=sym_llc), echo=args.verbose)
    if args.force or NewerThanOrNotThere(pexe, obj_sz) or \
            NewerThanOrNotThere(llvm2ice, obj_sz):
        shellcmd([llvm2ice,
                  '-O' + opt_level,
                  '-bitcode-format=pnacl',
                  '-disable-globals',
                  '-externalize',
                  '-ffunction-sections',
                  '-o', asm_sz] +
                 args.sz_args +
                 [pexe],
                 echo=args.verbose)
        shellcmd((
            'llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj -o {obj} ' +
            '{asm}'
            ).format(asm=asm_sz, obj=obj_sz), echo=args.verbose)
        shellcmd((
            'objcopy --redefine-sym _start=_user_start {obj}'
            ).format(obj=obj_sz), echo=args.verbose)
        shellcmd((
            'nm {obj} | sed -n "s/.* [a-zA-Z] //p" > {sym}'
            ).format(obj=obj_sz, sym=sym_sz), echo=args.verbose)

    with open(sym_sz_unescaped) as f:
        sz_syms = f.read().splitlines()
    re_include_str = BuildRegex(args.include, sz_syms)
    re_exclude_str = BuildRegex(args.exclude, sz_syms)
    re_include = re.compile(re_include_str)
    re_exclude = re.compile(re_exclude_str)
    # If a symbol doesn't explicitly match re_include or re_exclude,
    # the default MatchSymbol() result is True, unless some --include
    # args are provided.
    default_match = not len(args.include)

    whitelist_has_items = False
    with open(whitelist_sz_unescaped, 'w') as f:
        for sym in sz_syms:
            if MatchSymbol(sym, re_include, re_exclude, default_match):
                f.write(sym + '\n')
                whitelist_has_items = True
    shellcmd((
        'objcopy --weaken {obj} {weak}'
        ).format(obj=obj_sz, weak=obj_sz_weak), echo=args.verbose)
    if whitelist_has_items:
        # objcopy returns an error if the --weaken-symbols file is empty.
        shellcmd((
            'objcopy --weaken-symbols={whitelist} {obj} {weak}'
            ).format(whitelist=whitelist_sz, obj=obj_llc, weak=obj_llc_weak),
                 echo=args.verbose)
    else:
        shellcmd((
            'objcopy {obj} {weak}'
            ).format(obj=obj_llc, weak=obj_llc_weak), echo=args.verbose)
    shellcmd((
        'ld -r -m elf_i386 -o {partial} {sz} {llc}'
        ).format(partial=obj_partial, sz=obj_sz_weak, llc=obj_llc_weak),
             echo=args.verbose)
    shellcmd((
        'objcopy -w --localize-symbol="*" {partial}'
        ).format(partial=obj_partial), echo=args.verbose)
    shellcmd((
        'objcopy --globalize-symbol=_user_start {partial}'
        ).format(partial=obj_partial), echo=args.verbose)
    shellcmd((
        'gcc -m32 {partial} -o {exe} ' +
        # Keep the rest of this command line (except szrt.c) in sync
        # with RunHostLD() in pnacl-translate.py.
        '{root}/toolchain/linux_x86/pnacl_newlib/translator/x86-32-linux/lib/' +
        '{{unsandboxed_irt,irt_query_list}}.o ' +
        '{root}/toolchain_build/src/subzero/runtime/szrt.c ' +
        '-lpthread -lrt'
        ).format(partial=obj_partial, exe=exe, root=nacl_root),
             echo=args.verbose)
    # Put the extra verbose printing at the end.
    if args.verbose:
        print 'PATH={path}'.format(path=os.environ['PATH'])
        print 'include={regex}'.format(regex=re_include_str)
        print 'exclude={regex}'.format(regex=re_exclude_str)
        print 'default_match={dm}'.format(dm=default_match)
        print 'Number of Subzero syms = {num}'.format(num=len(sz_syms))

if __name__ == '__main__':
    main()
