#!/bin/sh

# TODO: Retire this script and move the individual tests into the lit
# framework, to leverage parallel testing and other lit goodness.

set -eux

OPTLEVELS="m1 2"
ATTRIBUTES="sse2 sse4.1"
OUTDIR=Output
# Clean the output directory to avoid reusing stale results.
rm -rf "${OUTDIR}"
mkdir -p "${OUTDIR}"

for optlevel in ${OPTLEVELS} ; do
    for attribute in ${ATTRIBUTES} ; do

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=simple_loop.c \
            --driver=simple_loop_main.c \
            --output=simple_loop_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=mem_intrin.cpp \
            --driver=mem_intrin_main.cpp \
            --output=mem_intrin_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_arith.cpp \
            --test=test_arith_frem.ll \
            --test=test_arith_sqrt.ll \
            --driver=test_arith_main.cpp \
            --output=test_arith_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_bitmanip.cpp --test=test_bitmanip_intrin.ll \
            --driver=test_bitmanip_main.cpp \
            --output=test_bitmanip_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_calling_conv.cpp \
            --driver=test_calling_conv_main.cpp \
            --output=test_calling_conv_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_cast.cpp --test=test_cast_to_u1.ll \
            --driver=test_cast_main.cpp \
            --output=test_cast_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_fcmp.pnacl.ll \
            --driver=test_fcmp_main.cpp \
            --output=test_fcmp_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_global.cpp \
            --driver=test_global_main.cpp \
            --output=test_global_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_icmp.cpp --test=test_icmp_i1vec.ll \
            --driver=test_icmp_main.cpp \
            --output=test_icmp_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_select.ll \
            --driver=test_select_main.cpp \
            --output=test_select_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_stacksave.c \
            --driver=test_stacksave_main.c \
            --output=test_stacksave_O${optlevel}_${attribute}

        # Compile the non-subzero object files straight from source
        # since the native LLVM backend does not understand how to
        # lower NaCl-specific intrinsics.
        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ \
            --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_sync_atomic.cpp \
            --crosstest-bitcode=0 \
            --driver=test_sync_atomic_main.cpp \
            --output=test_sync_atomic_O${optlevel}_${attribute}

        ./crosstest.py -O${optlevel} --mattr ${attribute} \
            --prefix=Subzero_ --target=x8632 \
            --dir="${OUTDIR}" \
            --test=test_vector_ops.ll \
            --driver=test_vector_ops_main.cpp \
            --output=test_vector_ops_O${optlevel}_${attribute}

    done
done

for optlevel in ${OPTLEVELS} ; do
    for attribute in ${ATTRIBUTES}; do
        "${OUTDIR}"/simple_loop_O${optlevel}_${attribute}
        "${OUTDIR}"/mem_intrin_O${optlevel}_${attribute}
        "${OUTDIR}"/test_arith_O${optlevel}_${attribute}
        "${OUTDIR}"/test_bitmanip_O${optlevel}_${attribute}
        "${OUTDIR}"/test_calling_conv_O${optlevel}_${attribute}
        "${OUTDIR}"/test_cast_O${optlevel}_${attribute}
        "${OUTDIR}"/test_fcmp_O${optlevel}_${attribute}
        "${OUTDIR}"/test_global_O${optlevel}_${attribute}
        "${OUTDIR}"/test_icmp_O${optlevel}_${attribute}
        "${OUTDIR}"/test_select_O${optlevel}_${attribute}
        "${OUTDIR}"/test_stacksave_O${optlevel}_${attribute}
        "${OUTDIR}"/test_sync_atomic_O${optlevel}_${attribute}
        "${OUTDIR}"/test_vector_ops_O${optlevel}_${attribute}
    done
done
