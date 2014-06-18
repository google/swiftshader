#!/usr/bin/env python2

import argparse
import itertools
import re

if __name__ == '__main__':
    """Compares a LLVM file with a subzero file for differences.

    Before comparing, the LLVM file is massaged to remove comments,
    blank lines, global variable definitions, external function
    declarations, and possibly other patterns that llvm2ice does not
    handle.

    The subzero file and the massaged LLVM file are compared line by
    line for differences.  However, there is a regex defined such that
    if the regex matches a line in the LLVM file, that line and the
    corresponding line in the subzero file are ignored.  This lets us
    ignore minor differences such as inttoptr and ptrtoint, and
    printing of floating-point constants.

    On success, no output is produced.  On failure, each mismatch is
    printed as two lines, one starting with 'SZ' (subzero) and one
    starting with 'LL' (LLVM).
    """
    desc = 'Compare LLVM and subzero bitcode files.'
    argparser = argparse.ArgumentParser(description=desc)
    argparser.add_argument(
        'llfile', nargs=1,
        type=argparse.FileType('r'), metavar='LLVM_FILE',
        help='LLVM bitcode file')
    argparser.add_argument(
        'szfile', nargs='?', default='-',
        type=argparse.FileType('r'), metavar='SUBZERO_FILE',
        help='Subzero bitcode file [default stdin]')
    args = argparser.parse_args()
    bitcode = args.llfile[0].readlines()
    sz_out = [ line.rstrip() for line in args.szfile.readlines()]

    # Filter certain lines and patterns from the input, and collect
    # the remainder into llc_out.
    llc_out = []
    tail_call = re.compile(' tail call ');
    trailing_comment = re.compile(';.*')
    ignore_pattern = re.compile('^ *$|^declare|^@')
    prev_line = None
    for line in bitcode:
        if prev_line:
            line = prev_line + line
            prev_line = None
        # Convert tail call into regular (non-tail) call.
        line = tail_call.sub(' call ', line)
        # Remove trailing comments and spaces.
        line = trailing_comment.sub('', line).rstrip()
        # Ignore blanks lines, forward declarations, and variable definitions.
        if ignore_pattern.search(line):
          continue
        # SZ doesn't break up long lines, but LLVM does. Normalize to SZ.
        if line.endswith(','):
            prev_line = line
            continue
        llc_out.append(line)

    # Compare sz_out and llc_out line by line, but ignore pairs of
    # lines where the llc line matches a certain pattern.
    return_code = 0
    lines_total = 0
    lines_diff = 0
    ignore_pattern = re.compile(
        '|'.join([' -[0-9]',                 # negative constants
                  ' (float|double) [-0-9]',  # FP constants
                  ' (float|double) %\w+, [-0-9]',
                  ' @llvm\..*i\d+\*',        # intrinsic calls w/ pointer args
                  ' i\d+\* @llvm\.',         # intrinsic calls w/ pointer ret
                  ' inttoptr ',              # inttoptr pointer types
                  ' ptrtoint ',              # ptrtoint pointer types
                  ' bitcast .*\* .* to .*\*' # bitcast pointer types
                  ]))
    for (sz_line, llc_line) in itertools.izip_longest(sz_out, llc_out):
        lines_total += 1
        if sz_line == llc_line:
            continue
        if llc_line and ignore_pattern.search(llc_line):
            lines_diff += 1
            continue
        if sz_line: print 'SZ (%d)> %s' % (lines_total, sz_line)
        if llc_line: print 'LL (%d)> %s' % (lines_total, llc_line)
        return_code = 1

    if return_code == 0:
        message = 'Success (ignored %d diffs out of %d lines)'
        print message % (lines_diff, lines_total)
    exit(return_code)
