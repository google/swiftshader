
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
