#!/usr/bin/env python2

import argparse
import os
import sys

from utils import shellcmd

def GetFileAttributes(Filename):
  """Returns the set of names contained in file named Filename.
  """
  if not os.path.isfile(Filename):
    raise RuntimeError("Can't open: %s" % Filename)
  with open(Filename, 'r') as f:
    return f.read().split()

def HasFileAttributes(Filename, Attributes):
  """Returns true if the set of names in Attributes also appear
     in the set of names contained in file named Filename.
  """
  return set(Attributes) <= set(GetFileAttributes(Filename))

def main():
  """Run the specified command only if attributes are defined.

     Check if the fset of attributes (i.e. names), contained in FILE,
     contains the attributes defined by --att=ATTRIBUTE arguments. If
     so, runs in a shell the command defined by the remaining
     arguments.
  """
  argparser = argparse.ArgumentParser(
    description='    ' + main.__doc__,
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  argparser.add_argument('file', metavar='FILE',
                         help='File defining attributes to check against.')
  argparser.add_argument('--att', required=False, default=[],
                         action='append', metavar='ATTRIBUTE',
                         help='Attribute to check. May be repeated.')
  argparser.add_argument('--echo-cmd', required=False,
                         action='store_true',
                         help='Trace the command before running.')
  argparser.add_argument('--command', nargs=argparse.REMAINDER,
                         help='Command to run if attributes found.')

  args = argparser.parse_args()

  # Quit early if no command to run.
  if not args.command:
    raise RuntimeError("No command argument(s) specified for ifatts")

  if HasFileAttributes(args.file, args.att):
    stdout_result = shellcmd(args.command, echo=args.echo_cmd)
    if not args.echo_cmd:
      sys.stdout.write(stdout_result)

if __name__ == '__main__':
  main()
  sys.exit(0)
