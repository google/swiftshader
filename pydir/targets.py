#!/usr/bin/env python2

from collections import namedtuple

TargetInfo = namedtuple('TargetInfo',
                        ['target', 'triple', 'llc_flags', 'ld_emu'])

X8632Target = TargetInfo(target='x8632',
                         triple='i686-none-linux',
                         llc_flags=['-mcpu=pentium4m'],
                         ld_emu='elf_i386_nacl')

X8664Target = TargetInfo(target='x8664',
                         triple='x86_64-none-linux',
                         llc_flags=['-mcpu=x86-64'],
                         ld_emu='elf_x86_64_nacl')

ARM32Target = TargetInfo(target='arm32',
                         triple='armv7a-none-linux-gnueabihf',
                         llc_flags=['-mcpu=cortex-a9',
                                    '-float-abi=hard',
                                    '-mattr=+neon'],
                         ld_emu='armelf_nacl')


def ConvertTripleToNaCl(nonsfi_triple):
  return nonsfi_triple.replace('linux', 'nacl')
