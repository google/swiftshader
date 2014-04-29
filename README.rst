Subzero - Fast code generator for PNaCl bitcode
===============================================

Building
--------

You must have LLVM trunk source code available and built.  See
http://llvm.org/docs/GettingStarted.html#getting-started-quickly-a-summary for
guidance.

Set variables ``LLVM_SRC_PATH`` and ``LLVM_BIN_PATH`` to point to the
appropriate directories in the LLVM source and build directories.  These can be
set as environment variables, or you can modify the top-level Makefile.

Run ``make`` at the top level to build the main target ``llvm2ice``.

``llvm2ice``
------------

The ``llvm2ice`` program uses the LLVM infrastructure to parse an LLVM bitcode
file and translate it into ICE.  It then invokes ICE's translate method to lower
it to target-specific machine code, dumping the IR at various stages of the
translation.

The program can be run as follows::

    ../llvm2ice ./ir_samples/<file>.ll
    ../llvm2ice ./tests_lit/llvm2ice_tests/<file>.ll

At this time, ``llvm2ice`` accepts a few arguments:

    ``-help`` -- Show available arguments and possible values.

    ``-notranslate`` -- Suppress the ICE translation phase, which is useful if
    ICE is missing some support.

    ``-target=<TARGET>`` -- Set the target architecture.  The default is x8632,
    and x8632fast (generate x8632 code as fast as possible at the cost of code
    quality) is also available.  Future targets include x8664, arm32, and arm64.

    ``-verbose=<list>`` -- Set verbosity flags.  This argument allows a
    comma-separated list of values.  The default is ``none``, and the value
    ``inst,pred`` will roughly match the .ll bitcode file.  Of particular use
    are ``all`` and ``none``.

See ir_samples/README.rst for more details.

Running the test suite
----------------------

Subzero uses the LLVM ``lit`` testing tool for its test suite, which lives in
``tests_lit``. To execute the test suite, first build Subzero, and then run::

    python <path_to_lit.py> -sv tests_lit

``path_to_lit`` is the direct path to the lit script in the LLVM source
(``$LLVM_SRC_PATH/utils/lit/lit.py``).

The above ``lit`` execution also needs the LLVM binary path in the
``LLVM_BIN_PATH`` env var.

Assuming the LLVM paths are set up, ``make check`` is a convenient way to run
the test suite.

Assembling ``llvm2ice`` output
------------------------------

Currently ``llvm2ice`` produces textual assembly code in a structure suitable
for input to ``llvm-mc`` and currently using "intel" assembly syntax.  The first
line of output is a convenient comment indicating how to pipe the output to
``llvm-mc`` to produce object code.
