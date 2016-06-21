Using AddressSanitizer in Subzero
=================================

AddressSanitizer is a powerful compile-time tool used to detect and report
illegal memory accesses. For a full description of the tool, see the original
`paper
<https://www.usenix.org/system/files/conference/atc12/atc12-final39.pdf>`_.
AddressSanitizer is only supported on native builds of .pexe files and cannot be
used in production.

In Subzero, AddressSanitizer depends on being able to find and instrument calls
to various functions such as malloc() and free(), and as such the .pexe file
being translated must not have had those symbols stripped. Subzero will not
complain if it is told to translate a .pexe file with its symbols stripped, but
it will not be able to find calls to malloc() and free(), so AddressSanitizer
will not work correctly in the final executable.

These are the steps to compile hello.c to an instrumented object file::

    pnacl-clang -o hello.nonfinal.pexe hello.c
    pnacl-finalize --no-strip-syms -o hello.pexe hello.nonfinal.pexe
    pnacl-sz -fsanitize-address -filetype=obj -o hello.o hello.pexe

The resulting object file must be linked with the Subzero-specific
AddressSanitizer runtime to work correctly. A .pexe file can be compiled with
AddressSanitizer and properly linked into a final executable using
subzero/pydir/szbuild.py with the --fsanitize-address flag, i.e.::

    pydir/szbuild.py --fsanitize-address hello.pexe
