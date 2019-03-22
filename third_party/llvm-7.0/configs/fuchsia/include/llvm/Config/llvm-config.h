/*===------- llvm/Config/llvm-config.h - llvm configuration -------*- C -*-===*/
/*                                                                            */
/*                     The LLVM Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file enumerates variables from the LLVM configuration so that they
   can be in exported headers and won't override package specific directives.
   This is a C header that can be included in the llvm-c headers. */

#ifndef LLVM_CONFIG_H
#define LLVM_CONFIG_H

/* Define if LLVM_ENABLE_DUMP is enabled */
/* #undef LLVM_ENABLE_DUMP */

/* Define if we link Polly to the tools */
/* #undef LINK_POLLY_INTO_TOOLS */

/* Target triple LLVM will generate code for by default */
#if defined(__x86_64__)
#define LLVM_DEFAULT_TARGET_TRIPLE "x86_64-unknown-fuchsia"
#elif defined(__aarch64__)
#define LLVM_DEFAULT_TARGET_TRIPLE "aarch64-unknown-fuchsia"
#else
#error "unknown architecture"
#endif

/* Define if threads enabled */
#define LLVM_ENABLE_THREADS 0

/* Has gcc/MSVC atomic intrinsics */
#define LLVM_HAS_ATOMICS 1

/* Host triple LLVM will be executed on */
#if defined(__x86_64__)
#define LLVM_HOST_TRIPLE "x86_64-unknown-fuchsia"
#elif defined(__aarch64__)
#define LLVM_HOST_TRIPLE "aarch64-unknown-fuchsia"
#else
#error "unknown architecture"
#endif

/* LLVM architecture name for the native architecture, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_ARCH AArch64
#elif defined(__x86_64__)
#define LLVM_NATIVE_ARCH X86
#else
#error "unknown architecture"
#endif

/* LLVM name for the native AsmParser init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_ASMPARSER LLVMInitializeAArch64AsmParser
#elif defined(__x86_64__)
#define LLVM_NATIVE_ASMPARSER LLVMInitializeX86AsmParser
#else
#error "unknown architecture"
#endif

/* LLVM name for the native AsmPrinter init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_ASMPRINTER LLVMInitializeAArch64AsmPrinter
#elif defined(__arm__)
#define LLVM_NATIVE_ASMPRINTER LLVMInitializeARMAsmPrinter
#elif defined(__x86_64__)
#define LLVM_NATIVE_ASMPRINTER LLVMInitializeX86AsmPrinter
#else
#error "unknown architecture"
#endif

/* LLVM name for the native Disassembler init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_DISASSEMBLER LLVMInitializeAArch64Disassembler
#elif defined(__x86_64__)
#define LLVM_NATIVE_DISASSEMBLER LLVMInitializeX86Disassembler
#else
#error "unknown architecture"
#endif

/* LLVM name for the native Target init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_TARGET LLVMInitializeAArch64Target
#elif defined(__x86_64__)
#define LLVM_NATIVE_TARGET LLVMInitializeX86Target
#else
#error "unknown architecture"
#endif

/* LLVM name for the native TargetInfo init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_TARGETINFO LLVMInitializeAArch64TargetInfo
#elif defined(__x86_64__)
#define LLVM_NATIVE_TARGETINFO LLVMInitializeX86TargetInfo
#else
#error "unknown architecture"
#endif

/* LLVM name for the native target MC init function, if available */
#if defined(__aarch64__)
#define LLVM_NATIVE_TARGETMC LLVMInitializeAArch64TargetMC
#elif defined(__x86_64__)
#define LLVM_NATIVE_TARGETMC LLVMInitializeX86TargetMC
#else
#error "unknown architecture"
#endif

/* Define if this is Unixish platform */
#define LLVM_ON_UNIX 1

/* Define if we have the Intel JIT API runtime support library */
#define LLVM_USE_INTEL_JITEVENTS 0

/* Define if we have the oprofile JIT-support library */
#define LLVM_USE_OPROFILE 0

/* Define if we have the perf JIT-support library */
#define LLVM_USE_PERF 0

/* Major version of the LLVM API */
#define LLVM_VERSION_MAJOR 7

/* Minor version of the LLVM API */
#define LLVM_VERSION_MINOR 0

/* Patch version of the LLVM API */
#define LLVM_VERSION_PATCH 0

/* LLVM version string */
#define LLVM_VERSION_STRING "7.0.0"

/* Whether LLVM records statistics for use with GetStatistics(),
 * PrintStatistics() or PrintStatisticsJSON()
 */
#define LLVM_FORCE_ENABLE_STATS 0

#endif
