
ifndef SUBZERO_LEVEL
# Top-level, not included from a subdir
SUBZERO_LEVEL := .
DIRS := src
PARALLEL_DIRS :=
endif

# Set LLVM source root level.
LEVEL := $(SUBZERO_LEVEL)/../..

# Include LLVM common makefile.
include $(LEVEL)/Makefile.common

CXX.Flags += -std=c++11 -Wextra -Werror -Wno-error=unused-parameter

CPP.Defines += -DALLOW_TEXT_ASM=1 -DALLOW_DUMP=1 -DALLOW_LLVM_CL=1 \
               -DALLOW_LLVM_IR=1 -DALLOW_LLVM_IR_AS_INPUT=1

