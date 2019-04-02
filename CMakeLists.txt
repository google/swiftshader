cmake_minimum_required(VERSION 2.8)

project(SwiftShader C CXX)

# On Windows we use custom solution and project files, except for certain third-
# party projects which use CMake-generated ones. They are manually (re)generated
# and made path relative using build/cmake.sh, so they can be checked into the
# repository. Therefore they should not be auto-regenerated and left using
# absolute paths by CMake's ZERO_CHECK.
if(WIN32)
    # Disable automatically regenerating project files on CMakeLists.txt changes.
    set(CMAKE_SUPPRESS_REGENERATION true)
endif()

###########################################################
# Detect system
###########################################################

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX ON)
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
    set(ANDROID ON)
    set(CMAKE_CXX_FLAGS "-DANDROID_NDK_BUILD")
elseif(WIN32)
elseif(APPLE)
else()
    message(FATAL_ERROR "Platform is not supported")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "aarch64")
    else()
        set(ARCH "arm")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips*")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "mips64el")
    else()
        set(ARCH "mipsel")
    endif()
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "x86_64")
    else()
        set(ARCH "x86")
    endif()
endif()

set(CMAKE_MACOSX_RPATH ON)

if ((CMAKE_GENERATOR MATCHES "Visual Studio") AND (CMAKE_GENERATOR_TOOLSET STREQUAL ""))
  message(WARNING "Visual Studio generators use the x86 host compiler by "
                  "default, even for 64-bit targets. This can result in linker "
                  "instability and out of memory errors. To use the 64-bit "
                  "host compiler, pass -Thost=x64 on the CMake command line.")
endif()

###########################################################
# Options
###########################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The type of build: Debug Release MinSizeRel RelWithDebInfo." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)

option(BUILD_EGL "Build the EGL library" 1)
option(BUILD_GLESv2 "Build the OpenGL ES 2 library" 1)
option(BUILD_GLES_CM "Build the OpenGL ES 1.1 library" 1)
option(BUILD_VULKAN "Build the Vulkan library" 1)

option(USE_GROUP_SOURCES "Group the source files in a folder tree for Visual Studio" 1)

option(BUILD_SAMPLES "Build sample programs" 1)
option(BUILD_TESTS "Build test programs" 1)

option (MSAN "Build with memory sanitizer" 0)
option (ASAN "Build with address sanitizer" 0)
option (TSAN "Build with thread sanitizer" 0)
option (UBSAN "Build with undefined behavior sanitizer" 0)
option (WARNINGS_AS_ERRORS "Treat all warnings as errors" 1)
option (DCHECK_ALWAYS_ON "Check validation macros even in release builds" 0)
option (REACTOR_EMIT_DEBUG_INFO "Emit debug info for JIT functions" 0)

if(ARCH STREQUAL "arm")
    set(DEFAULT_REACTOR_BACKEND "Subzero")
else()
    set(DEFAULT_REACTOR_BACKEND "LLVM")
endif()

set(REACTOR_BACKEND ${DEFAULT_REACTOR_BACKEND} CACHE STRING "JIT compiler back-end used by Reactor")
set_property(CACHE REACTOR_BACKEND PROPERTY STRINGS LLVM Subzero)

# LLVM disallows calling cmake . from the main LLVM dir, the reason is that
# it builds header files that could overwrite the orignal ones. Here we
# want to include LLVM as a subdirectory and even though it wouldn't cause
# the problem, if cmake . is called from the main dir, the condition that
# LLVM checkes, "CMAKE_CURRENT_SOURCE_DIR == CMAKE_CURRENT_BINARY_DIR" will be true. So we
# disallow it ourselves too to. In addition if there are remining CMakeFiles
# and CMakeCache in the directory, cmake .. from a subdirectory will still
# try to build from the main directory so we instruct users to delete these
# files when they get the error.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR "In source builds are not allowed by LLVM, please create a build/ directory and build from there. You may have to delete the CMakeCache.txt file and CMakeFiles directory that are next to the CMakeLists.txt.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###########################################################
# Initialize submodules
###########################################################

if (NOT TARGET gtest)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/.git)
        message(WARNING "
      third_party/googletest submodule missing.
      Running 'git submodule update --init' to download it:
        ")

        execute_process(COMMAND git submodule update --init)
    endif()
endif()

if (NOT TARGET libbacktrace)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace/src/.git)
        message(WARNING "
      third_party/libbacktrace/src submodule missing.
      Running 'git submodule update --init' to download it:
        ")

        execute_process(COMMAND git submodule update --init)
    endif()
endif()

###########################################################
# Convenience macros
###########################################################

# Recursively calls source_group on the files of the directory
# so that Visual Studio has the files in a folder tree
macro(group_all_sources directory)
    file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${directory} ${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*)
    foreach(file ${files})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${directory}/${file})
            group_all_sources(${directory}/${file})
        else()
            string(REPLACE "/" "\\" groupname ${directory})
            source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${directory}/${file})
        endif()
    endforeach()
endmacro()

# Takes target library and a directory where the export map is
# and add the linker options so that only the API symbols are
# exported.
macro(set_shared_library_export_map TARGET DIR)
    if(MSVC)
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " /DEF:\"${DIR}/${TARGET}.def\"")
    elseif(LINUX)
        # The version script only exports the API functions and
        # hides all the others.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--version-script=${DIR}/${TARGET}.lds")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_DEPENDS "${DIR}/${TARGET}.lds;")

        # Don't allow symbols to be overridden by another module.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY COMPILE_FLAGS " -fvisibility=protected")

        if(ARCH STREQUAL "mipsel" OR ARCH STREQUAL "mips64el")
          # MIPS supports sysv hash-style only.
          set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--hash-style=sysv")
        else()
          # Both hash-style are needed, because we want both gold and
          # GNU ld to be able to read our libraries.
          set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--hash-style=both")
        endif()

        # Gc sections is used in combination with each functions being
        # in its own section, to reduce the binary size.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--gc-sections")

        # Don't allow undefined symbols, unless it's a Sanitizer build.
        if(NOT MSAN AND NOT ASAN AND NOT TSAN AND NOT UBSAN)
            set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--no-undefined")
        endif()
    endif()
endmacro()

if(USE_GROUP_SOURCES)
    group_all_sources(src)
endif()

###########################################################
# Directories
###########################################################

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(OPENGL_DIR ${SOURCE_DIR}/OpenGL)
set(OPENGL_COMPILER_DIR ${OPENGL_DIR}/compiler)
set(VULKAN_DIR ${SOURCE_DIR}/Vulkan)
set(LLVM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/llvm-7.0/llvm)
set(LLVM_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/llvm-7.0/configs)
set(LIBBACKTRACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace/src)
set(LIBBACKTRACE_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace/config)
set(LIBBACKTRACE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace/src)
set(SUBZERO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/subzero)
set(SUBZERO_LLVM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/llvm-subzero)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(HELLO2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/PowerVR_SDK/Examples/Beginner/01_HelloAPI/OGLES2)

###########################################################
# Compile flags
###########################################################

# Flags for project code (non 3rd party)
set(SWIFTSHADER_COMPILE_OPTIONS "")

macro(set_cpp_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_CXX_FLAGS_${ARGV1} "${CMAKE_CXX_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
    endif()
endmacro()

if(MSVC)
    set_cpp_flag("/MP")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SBCS)  # Single Byte Character Set (ASCII)
else()
    set_cpp_flag("--std=c++11")
    set_cpp_flag("-fno-exceptions")

    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "-Wall"
        "-Werror=reorder"
        "-Werror=sign-compare"
        "-Werror=missing-braces"
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Werror=unused-lambda-capture"
            "-Werror=string-conversion"
            "-Werror=extra-semi"
            "-Werror=ignored-qualifiers"
        )
    endif()

    # Remove xor, and, or and friends from the list of keywords, they are used
    # by Reactor
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "-fno-operator-names"
    )

    # LLVM headers requires these flags to be set
    set_cpp_flag("-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS")

    if(ARCH STREQUAL "x86")
        set_cpp_flag("-m32")
        set_cpp_flag("-msse2")
        set_cpp_flag("-mfpmath=sse")
        set_cpp_flag("-march=pentium4")
        set_cpp_flag("-mtune=generic")
    endif()
    if(ARCH STREQUAL "x86_64")
        set_cpp_flag("-m64")
        set_cpp_flag("-fPIC")
        set_cpp_flag("-march=x86-64")
        set_cpp_flag("-mtune=generic")
    endif()
    if(ARCH STREQUAL "mipsel")
        set_cpp_flag("-march=mips32r2")
        set_cpp_flag("-fPIC")
        set_cpp_flag("-mhard-float")
        set_cpp_flag("-mfp32")
    endif()
    if(ARCH STREQUAL "mips64el")
        set_cpp_flag("-march=mips64r2")
        set_cpp_flag("-mabi=64")
        set_cpp_flag("-fPIC")
    endif()

    if(LINUX)
        set_cpp_flag("-DUSE_X11=1")
        set_cpp_flag("-DVK_USE_PLATFORM_XLIB_KHR")
    endif()

    # Use -g3 to have even more debug info
    set_cpp_flag("-g -g3" DEBUG)
    set_cpp_flag("-g -g3" RELWITHDEBINFO)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Treated as an unused argument with clang
        set_cpp_flag("-s" RELEASE)
    endif()

    if(WARNINGS_AS_ERRORS)
        set_cpp_flag("-Werror") # Treat all warnings as errors
    endif()

    if(DCHECK_ALWAYS_ON)
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DDCHECK_ALWAYS_ON")
    endif()

    if(REACTOR_EMIT_DEBUG_INFO)
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_DEBUG_INFO")
    endif()

    # Disable pedanitc warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wno-ignored-attributes"   # ignoring attributes on template argument 'X'
            "-Wno-attributes"           # 'X' attribute ignored
            "-Wno-strict-aliasing"      # dereferencing type-punned pointer will break strict-aliasing rules
            "-Wno-comment"              # multi-line comment
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wno-unneeded-internal-declaration"  # function 'X' is not needed and will not be emitted
            "-Wno-unused-private-field"           # private field 'offset' is not used - TODO: Consider enabling this once Vulkan is further implemented.
            "-Wno-comment"                        # multi-line comment
            "-Wno-undefined-var-template"         # instantiation of variable 'X' required here, but no definition is available
        )
    endif()

    # For distribution it is more important to be slim than super optimized
    set_cpp_flag("-Os" RELEASE)
    set_cpp_flag("-Os" RELWITHDEBINFO)

    set_cpp_flag("-DNDEBUG" RELEASE)
    set_cpp_flag("-DNDEBUG" RELWITHDEBINFO)
    set_cpp_flag("-DANGLE_DISABLE_TRACE" RELEASE)
    set_cpp_flag("-DANGLE_DISABLE_TRACE" RELWITHDEBINFO)

    # Put each variable and function in its own section so that when linking
    # with -gc-sections unused functions and variables are removed.
    set_cpp_flag("-ffunction-sections" RELEASE)
    set_cpp_flag("-fdata-sections" RELEASE)
    set_cpp_flag("-fomit-frame-pointer" RELEASE)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(MSAN)
            set_cpp_flag("-fsanitize=memory")
        elseif(ASAN)
            set_cpp_flag("-fsanitize=address")
        elseif(TSAN)
            set_cpp_flag("-fsanitize=thread")
        elseif(UBSAN)
            set_cpp_flag("-fsanitize=undefined")
        endif()
    endif()
endif()

if(WIN32)
    add_definitions(-DWINVER=0x501 -DNOMINMAX -DSTRICT)
    set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "" "lib")
endif()

###########################################################
# LLVM
###########################################################

set(LLVM_LIST
    ${LLVM_DIR}/lib/Analysis/AliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/AliasAnalysisSummary.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/AliasSetTracker.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/AssumptionCache.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/BasicAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/BlockFrequencyInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/BlockFrequencyInfoImpl.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/BranchProbabilityInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CFG.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CFLAndersAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CFLSteensAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CallGraph.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CallGraphSCCPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CaptureTracking.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CmpInstAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/CodeMetrics.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ConstantFolding.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/DemandedBits.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/DependenceAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/DivergenceAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/DominanceFrontier.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/EHPersonalities.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/GlobalsModRef.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/IVUsers.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/InlineCost.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/InstructionSimplify.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/IteratedDominanceFrontier.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LazyBlockFrequencyInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LazyBranchProbabilityInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LazyValueInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/Loads.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LoopAnalysisManager.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LoopInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/LoopPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/MemoryBuiltins.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/MemoryDependenceAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/MemoryLocation.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/MemorySSA.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/MustExecute.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ObjCARCAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ObjCARCAnalysisUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ObjCARCInstKind.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/OptimizationRemarkEmitter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/OrderedBasicBlock.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/PHITransAddr.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/PhiValues.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/PostDominators.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ProfileSummaryInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/PtrUseVisitor.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ScalarEvolution.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionExpander.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionNormalization.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ScopedNoAliasAA.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/TargetLibraryInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/TargetTransformInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/TypeBasedAliasAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ValueLattice.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ValueLatticeUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/ValueTracking.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Analysis/VectorUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/AsmParser/LLLexer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/AsmParser/LLParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/AsmParser/Parser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/BinaryFormat/Dwarf.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/BinaryFormat/Magic.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/BinaryFormat/Wasm.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Bitcode/Reader/BitcodeReader.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Bitcode/Reader/BitstreamReader.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Bitcode/Reader/MetadataLoader.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Bitcode/Reader/ValueList.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AggressiveAntiDepBreaker.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AllocationOrder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/Analysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/ARMException.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AccelTable.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AddressPool.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinterDwarf.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinterInlineAsm.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/CodeViewDebug.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DIE.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DIEHash.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DbgValueHistoryCalculator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DebugHandlerBase.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DebugLocStream.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfCFIException.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfCompileUnit.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfDebug.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfExpression.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfStringPool.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfUnit.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/EHStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/WinCFGuard.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/WinException.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/AtomicExpandPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/BasicTargetTransformInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/BranchFolding.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/BranchRelaxation.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/BreakFalseDeps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CFIInstrInserter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CalcSpillWeights.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CallingConvLower.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CodeGen.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CodeGenPrepare.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/CriticalAntiDepBreaker.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/DFAPacketizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/DeadMachineInstructionElim.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/DetectDeadLanes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/DwarfEHPrepare.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/EarlyIfConversion.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/EdgeBundles.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ExecutionDomainFix.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ExpandISelPseudos.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ExpandMemCmp.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ExpandPostRAPseudos.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ExpandReductions.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/FEntryInserter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/FaultMaps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/FuncletLayout.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GCMetadata.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GCMetadataPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GCRootLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GCStrategy.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/CallLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/GlobalISel.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/IRTranslator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/InstructionSelect.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/InstructionSelector.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalityPredicates.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizeMutations.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Legalizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizerHelper.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizerInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Localizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/MachineIRBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegBankSelect.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegisterBank.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegisterBankInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Utils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/IfConversion.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ImplicitNullChecks.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/IndirectBrExpandPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/InlineSpiller.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/InterferenceCache.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/InterleavedAccessPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/IntrinsicLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LLVMTargetMachine.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LatencyPriorityQueue.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LazyMachineBlockFrequencyInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LexicalScopes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveDebugValues.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveDebugVariables.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveInterval.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveIntervalUnion.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveIntervals.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LivePhysRegs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveRangeCalc.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveRangeEdit.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveRangeShrink.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveRegMatrix.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveRegUnits.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveStacks.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LiveVariables.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LocalStackSlotAllocation.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LoopTraversal.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LowLevelType.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/LowerEmuTLS.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MIRCanonicalizerPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MIRPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MIRPrintingPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineBasicBlock.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineBlockFrequencyInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineBlockPlacement.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineBranchProbabilityInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineCSE.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineCombiner.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineCopyPropagation.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineDominanceFrontier.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineDominators.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineFrameInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineFunction.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineFunctionPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineFunctionPrinterPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineInstr.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineInstrBundle.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineLICM.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineLoopInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineModuleInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineModuleInfoImpls.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineOperand.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineOptimizationRemarkEmitter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineOutliner.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachinePassRegistry.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachinePipeliner.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachinePostDominators.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineRegionInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineRegisterInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineSSAUpdater.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineScheduler.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineSink.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineTraceMetrics.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MachineVerifier.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/MacroFusion.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/OptimizePHIs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PHIElimination.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PHIEliminationUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PatchableFunction.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PeepholeOptimizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PostRAHazardRecognizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PostRASchedulerList.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PreISelIntrinsicLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ProcessImplicitDefs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PrologEpilogInserter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/PseudoSourceValue.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ReachingDefAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegAllocBase.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegAllocBasic.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegAllocFast.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegAllocGreedy.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegUsageInfoCollector.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegUsageInfoPropagate.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegisterClassInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegisterCoalescer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegisterPressure.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegisterScavenging.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RegisterUsageInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/RenameIndependentSubregs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ResetMachineFunctionPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SafeStack.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SafeStackColoring.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SafeStackLayout.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ScalarizeMaskedMemIntrin.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAG.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAGInstrs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAGPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ScoreboardHazardRecognizer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/DAGCombiner.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/FastISel.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/FunctionLoweringInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/InstrEmitter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeDAG.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeFloatTypes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeIntegerTypes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeTypes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeTypesGeneric.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeVectorOps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeVectorTypes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ResourcePriorityQueue.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGRRList.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGSDNodes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGVLIW.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAG.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGAddressAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGDumper.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGISel.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGTargetInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/StatepointLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/TargetLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ShadowStackGCLowering.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ShrinkWrap.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SjLjEHPrepare.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SlotIndexes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SpillPlacement.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/SplitKit.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/StackColoring.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/StackMapLivenessAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/StackMaps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/StackProtector.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/StackSlotColoring.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TailDuplication.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TailDuplicator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetFrameLoweringImpl.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetInstrInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetLoweringBase.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetLoweringObjectFileImpl.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetOptionsImpl.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetPassConfig.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetRegisterInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetSchedule.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TargetSubtargetInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/TwoAddressInstructionPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/UnreachableBlockElim.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/ValueTypes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/VirtRegMap.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/WasmEHPrepare.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/WinEHPrepare.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/CodeGen/XRayInstrumentation.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CVTypeVisitor.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CodeViewError.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CodeViewRecordIO.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/ContinuationRecordBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/Formatters.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/GlobalTypeTableBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/Line.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/RecordName.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/RecordSerialization.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/SimpleTypeSerializer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/SymbolRecordMapping.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeDumpVisitor.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeHashing.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeIndex.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeIndexDiscovery.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeRecordMapping.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeTableCollection.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/ExecutionEngine.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/GDBRegistrationListener.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/Core.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/Legacy.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/OrcError.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/JITSymbol.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RTDyldMemoryManager.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyld.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldCOFF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldChecker.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldELF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldMachO.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/Targets/RuntimeDyldELFMips.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/SectionMemoryManager.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ExecutionEngine/TargetSelect.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/AsmWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Attributes.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/AutoUpgrade.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/BasicBlock.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Comdat.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/ConstantFold.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/ConstantRange.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Constants.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DIBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DataLayout.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DebugInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DebugInfoMetadata.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DebugLoc.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DiagnosticHandler.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DiagnosticInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/DiagnosticPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Dominators.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Function.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/GVMaterializer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Globals.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/IRBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/IRPrintingPasses.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/InlineAsm.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Instruction.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Instructions.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/IntrinsicInst.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/LLVMContext.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/LLVMContextImpl.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/LegacyPassManager.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/MDBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Mangler.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Metadata.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Module.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Operator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/OptBisect.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Pass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/PassManager.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/PassRegistry.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/ProfileSummary.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Statepoint.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Type.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/TypeFinder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Use.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/User.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Value.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/ValueSymbolTable.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/IR/Verifier.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/ELFObjectWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmBackend.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmInfoCOFF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmInfoDarwin.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmInfoELF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmMacro.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAsmStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCAssembler.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCCodeEmitter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCCodePadder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCCodeView.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCContext.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCDisassembler/MCRelocationInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCDwarf.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCELFObjectTargetWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCELFStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCExpr.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCFragment.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCInst.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCInstPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCInstrAnalysis.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCInstrDesc.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCLinkerOptimizationHint.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCMachOStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCMachObjectTargetWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCNullStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCObjectFileInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCObjectStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCObjectWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/AsmLexer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/AsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/COFFAsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/DarwinAsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/ELFAsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmLexer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmParserExtension.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCParser/MCTargetAsmParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCRegisterInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSchedule.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSection.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSectionCOFF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSectionELF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSectionMachO.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSectionWasm.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSubtargetInfo.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSymbol.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCSymbolELF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCTargetOptions.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCValue.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCWasmStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCWin64EH.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCWinCOFFStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MCWinEH.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/MachObjectWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/StringTableBuilder.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/SubtargetFeature.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/WasmObjectWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/MC/WinCOFFObjectWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/Archive.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/Binary.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/COFFObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/ELF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/ELFObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/Error.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/IRObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/IRSymtab.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/MachOObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/MachOUniversal.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/ModuleSymbolTable.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/ObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/RecordStreamer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/SymbolicFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/WasmObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Object/WindowsResource.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/ProfileData/InstrProf.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/APFloat.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/APInt.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ARMAttributeParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ARMBuildAttrs.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BinaryStreamError.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BinaryStreamReader.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BinaryStreamRef.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BinaryStreamWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BlockFrequency.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/BranchProbability.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Chrono.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/CodeGenCoverage.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/CommandLine.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Compression.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ConvertUTF.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ConvertUTFWrapper.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/DJB.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/DataExtractor.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Debug.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/DebugCounter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/DynamicLibrary.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Errno.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Error.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ErrorHandling.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/FoldingSet.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/FormatVariadic.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/FormattedStream.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/GraphWriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Hashing.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Host.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/IntEqClasses.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/IntervalMap.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/JamCRC.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/KnownBits.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/LEB128.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/LineIterator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Locale.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/LowLevelType.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/MD5.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ManagedStatic.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/MathExtras.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Memory.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/MemoryBuffer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Mutex.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/NativeFormatting.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Path.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/PrettyStackTrace.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Process.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Program.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/RWMutex.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/RandomNumberGenerator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Regex.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/SHA1.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ScaledNumber.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ScopedPrinter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Signals.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/SmallPtrSet.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/SmallVector.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/SourceMgr.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Statistic.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/StringExtras.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/StringMap.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/StringRef.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/StringSaver.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/TargetParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/TargetRegistry.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Threading.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Timer.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/ToolOutputFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Triple.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Twine.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Unicode.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/UnicodeCaseFold.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/Valgrind.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/YAMLParser.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/YAMLTraits.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/circular_raw_ostream.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/raw_ostream.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/regcomp.c # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/regerror.c # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/regexec.c # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/regfree.c # CHECK_NEEDED
    ${LLVM_DIR}/lib/Support/regstrlcpy.c # CHECK_NEEDED
    ${LLVM_DIR}/lib/Target/TargetLoweringObjectFile.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Target/TargetMachine.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineAddSub.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineAndOrXor.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCalls.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCasts.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCompares.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineLoadStoreAlloca.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineMulDivRem.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombinePHI.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineSelect.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineShifts.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineSimplifyDemanded.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineVectorOps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstructionCombining.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/ADCE.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/ConstantHoisting.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/DeadStoreElimination.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/GVN.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/LICM.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/LoopStrengthReduce.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/MergeICmps.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/PartiallyInlineLibCalls.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/Reassociate.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/SCCP.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/SROA.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Scalar/SimplifyCFGPass.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/BasicBlockUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/BreakCriticalEdges.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/BuildLibCalls.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/BypassSlowDivision.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/CloneFunction.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/DemoteRegToStack.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/EntryExitInstrumenter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/EscapeEnumerator.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/InlineFunction.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/LCSSA.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/Local.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/LoopSimplify.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/LoopUtils.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/LowerInvoke.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/OrderedInstructions.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/PromoteMemoryToRegister.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/SSAUpdater.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/SimplifyCFG.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/SimplifyLibCalls.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/SymbolRewriter.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/VNCoercion.cpp # CHECK_NEEDED
    ${LLVM_DIR}/lib/Transforms/Utils/ValueMapper.cpp # CHECK_NEEDED
)

if(ARCH STREQUAL "x86" OR ARCH STREQUAL "x86_64")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/X86/AsmParser/X86AsmInstrumentation.cpp
        ${LLVM_DIR}/lib/Target/X86/AsmParser/X86AsmParser.cpp
        ${LLVM_DIR}/lib/Target/X86/InstPrinter/X86ATTInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/X86/InstPrinter/X86InstComments.cpp
        ${LLVM_DIR}/lib/Target/X86/InstPrinter/X86InstPrinterCommon.cpp
        ${LLVM_DIR}/lib/Target/X86/InstPrinter/X86IntelInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86AsmBackend.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86ELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/X86/ShadowCallStack.cpp
        ${LLVM_DIR}/lib/Target/X86/TargetInfo/X86TargetInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/Utils/X86ShuffleDecode.cpp
        ${LLVM_DIR}/lib/Target/X86/X86AsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/X86/X86AvoidStoreForwardingBlocks.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallFrameOptimization.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallLowering.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallingConv.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CmovConversion.cpp
        ${LLVM_DIR}/lib/Target/X86/X86DomainReassignment.cpp
        ${LLVM_DIR}/lib/Target/X86/X86EvexToVex.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ExpandPseudo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FastISel.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FixupBWInsts.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FixupLEAs.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FixupSetCC.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FlagsCopyLowering.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FloatingPoint.cpp
        ${LLVM_DIR}/lib/Target/X86/X86FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/X86/X86IndirectBranchTracking.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrFMA3Info.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrFoldTables.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InterleavedAccess.cpp
        ${LLVM_DIR}/lib/Target/X86/X86LegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MCInstLower.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MachineFunctionInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MacroFusion.cpp
        ${LLVM_DIR}/lib/Target/X86/X86OptimizeLEAs.cpp
        ${LLVM_DIR}/lib/Target/X86/X86PadShortFunction.cpp
        ${LLVM_DIR}/lib/Target/X86/X86RegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86RegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86RetpolineThunks.cpp
        ${LLVM_DIR}/lib/Target/X86/X86SelectionDAGInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ShuffleDecodeConstantPool.cpp
        ${LLVM_DIR}/lib/Target/X86/X86SpeculativeLoadHardening.cpp
        ${LLVM_DIR}/lib/Target/X86/X86Subtarget.cpp
        ${LLVM_DIR}/lib/Target/X86/X86TargetMachine.cpp
        ${LLVM_DIR}/lib/Target/X86/X86TargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/X86/X86TargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86VZeroUpper.cpp
        ${LLVM_DIR}/lib/Target/X86/X86WinAllocaExpander.cpp
        ${LLVM_DIR}/lib/Target/X86/X86WinEHState.cpp
    )
elseif(ARCH STREQUAL "mipsel" OR ARCH STREQUAL "mips64el")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/Mips/AsmParser/MipsAsmParser.cpp
        ${LLVM_DIR}/lib/Target/Mips/InstPrinter/MipsInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsABIFlagsSection.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsABIInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsAsmBackend.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsOptionRecord.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCExpr.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsNaClELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16HardFloat.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16HardFloatInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16RegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsAnalyzeImmediate.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsAsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsCallLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsCCState.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsConstantIslandPass.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsDelaySlotFiller.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsExpandPseudo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsFastISel.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsInstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsISelLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsLegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsBranchExpansion.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsMCInstLower.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsMachineFunction.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsModuleISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsOptimizePICCall.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsOs16.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsRegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSEFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSEInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSEISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSEISelLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSERegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsSubtarget.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsTargetMachine.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsTargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/Mips/MicroMipsSizeReduction.cpp
        ${LLVM_DIR}/lib/Target/Mips/TargetInfo/MipsTargetInfo.cpp
    )
elseif(ARCH STREQUAL "aarch64")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64ELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64WinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64WinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64TargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64ELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64AsmBackend.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCExpr.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64A57FPLoadBalancing.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FastISel.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64PromoteConstant.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FalkorHWPFFix.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Utils/AArch64BaseInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64InstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64SelectionDAGInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64A53Fix835769.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetMachine.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CallLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64SIMDInstrOpt.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64Subtarget.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ConditionOptimizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Disassembler/AArch64Disassembler.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Disassembler/AArch64ExternalSymbolizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/TargetInfo/AArch64TargetInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64AdvSIMDScalarPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ConditionalCompares.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64DeadRegisterDefinitionsPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64LegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64StorePairSuppress.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64PBQPRegAlloc.cpp
        ${LLVM_DIR}/lib/Target/AArch64/InstPrinter/AArch64InstPrinter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CleanupLocalDynamicTLSPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AsmParser/AArch64AsmParser.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64MCInstLower.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RedundantCopyElimination.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ExpandPseudoInsts.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CollectLOH.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64AsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64LoadStoreOptimizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CondBrTuning.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64MacroFusion.cpp
    )
elseif(ARCH STREQUAL "arm")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/ARM/ARMConstantIslandPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMWinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMachORelocationInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCExpr.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMAsmBackend.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMWinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMUnwindOpAsm.cpp
        ${LLVM_DIR}/lib/Target/ARM/ThumbRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMachineFunctionInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMBaseRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMCallLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetMachine.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMBaseInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb1FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMRegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Utils/ARMBaseInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMComputeBlockSize.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMSelectionDAGInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMInstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMLoadStoreOptimizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMISelLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/Disassembler/ARMDisassembler.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMExpandPseudoInsts.cpp
        ${LLVM_DIR}/lib/Target/ARM/TargetInfo/ARMTargetInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/MLxExpansionPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2SizeReduction.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMConstantPoolValue.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/InstPrinter/ARMInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMSubtarget.cpp
        ${LLVM_DIR}/lib/Target/ARM/AsmParser/ARMAsmParser.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMParallelDSP.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/ARM/A15SDOptimizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2ITBlockPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMFastISel.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMacroFusion.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMAsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMCInstLower.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMHazardRecognizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMCodeGenPrepare.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb1InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMLegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMOptimizeBarriersPass.cpp
    )
endif()

if(REACTOR_EMIT_DEBUG_INFO)
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Demangle/ItaniumDemangle.cpp
    )
endif(REACTOR_EMIT_DEBUG_INFO)

set(LLVM_INCLUDE_DIR "")

if(WIN32)
    list(APPEND LLVM_INCLUDE_DIR ${LLVM_CONFIG_DIR}/windows/include)
    list(APPEND LIBBACKTRACE_INCLUDE_DIR ${LIBBACKTRACE_CONFIG_DIR}/windows/include)
elseif(LINUX)
    list(APPEND LLVM_INCLUDE_DIR ${LLVM_CONFIG_DIR}/linux/include)
    list(APPEND LIBBACKTRACE_INCLUDE_DIR ${LIBBACKTRACE_CONFIG_DIR}/linux/include)
elseif(APPLE)
    list(APPEND LLVM_INCLUDE_DIR ${LLVM_CONFIG_DIR}/darwin/include)
    list(APPEND LIBBACKTRACE_INCLUDE_DIR ${LIBBACKTRACE_CONFIG_DIR}/darwin/include)
elseif(ANDROID)
    list(APPEND LLVM_INCLUDE_DIR ${LLVM_CONFIG_DIR}/android/include)
    list(APPEND LIBBACKTRACE_INCLUDE_DIR ${LIBBACKTRACE_CONFIG_DIR}/android/include)
endif()

list(APPEND LLVM_INCLUDE_DIR
    ${LLVM_DIR}/include
    ${LLVM_DIR}/lib/Target/AArch64
    ${LLVM_DIR}/lib/Target/ARM
    ${LLVM_DIR}/lib/Target/Mips
    ${LLVM_DIR}/lib/Target/X86
    ${LLVM_CONFIG_DIR}/common/include
    ${LLVM_CONFIG_DIR}/common/lib/IR
    ${LLVM_CONFIG_DIR}/common/lib/Target/AArch64
    ${LLVM_CONFIG_DIR}/common/lib/Target/ARM
    ${LLVM_CONFIG_DIR}/common/lib/Target/Mips
    ${LLVM_CONFIG_DIR}/common/lib/Target/X86
    ${LLVM_CONFIG_DIR}/common/lib/Transforms/InstCombine
)

# Suppress known LLVM warnings
set(LLVM_COMPILE_OPTIONS)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND LLVM_COMPILE_OPTIONS
        "-Wno-noexcept-type"           # mangled name for ‘X’ will change in C++17 because the exception specification is part of a function type
        "-Wno-maybe-uninitialized"     # ‘X’ may be used uninitialized in this function
        "-Wno-unused-but-set-variable" # variable ‘X’ set but not used
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(LLVM_DISABLE_WARNINGS
        "/wd4141" # 'inline': used more than once
        "/wd4146" # unary minus operator applied to unsigned type, result still unsigned
        "/wd4244" # 'X': conversion from 'uint64_t' to 'const unsigned int', possible loss of data
        "/wd4267" # 'X': conversion from 'size_t' to 'unsigned int', possible loss of data
        "/wd4291" # 'void X new(size_t,unsigned int,unsigned int)': no matching operator delete found; memory will not be freed if initialization throws an exception
        "/wd4319" # 'X': zero extending 'unsigned int' to 'uint64_t' of greater size
        "/wd4624" # 'X': destructor was implicitly defined as deleted
        "/wd4805" # 'X': unsafe mix of type 'unsigned int' and type 'bool' in operation
        "/wd4996" # 'X': Call to 'Y' with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
    )
    set(LLVM_COMPILE_OPTIONS "/D \"_SCL_SECURE_NO_WARNINGS\" ${LLVM_DISABLE_WARNINGS}")
endif()

add_library(llvm STATIC ${LLVM_LIST})
set_target_properties(llvm PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    INCLUDE_DIRECTORIES "${LLVM_INCLUDE_DIR}"
    COMPILE_OPTIONS "${LLVM_COMPILE_OPTIONS}"
    COMPILE_DEFINITIONS "__STDC_CONSTANT_MACROS; __STDC_LIMIT_MACROS;"
    FOLDER "LLVM"
)

# Add required libraries for LLVM
if(LINUX)
    target_link_libraries(llvm dl)
endif(LINUX)

###########################################################
# Subzero
###########################################################

if(${REACTOR_BACKEND} STREQUAL "Subzero")
    set(SUBZERO_LIST
        ${SUBZERO_DIR}/src/IceAssembler.cpp
        ${SUBZERO_DIR}/src/IceCfg.cpp
        ${SUBZERO_DIR}/src/IceCfgNode.cpp
        ${SUBZERO_DIR}/src/IceClFlags.cpp
        ${SUBZERO_DIR}/src/IceELFObjectWriter.cpp
        ${SUBZERO_DIR}/src/IceELFSection.cpp
        ${SUBZERO_DIR}/src/IceFixups.cpp
        ${SUBZERO_DIR}/src/IceGlobalContext.cpp
        ${SUBZERO_DIR}/src/IceGlobalInits.cpp
        ${SUBZERO_DIR}/src/IceInst.cpp
        ${SUBZERO_DIR}/src/IceInstrumentation.cpp
        ${SUBZERO_DIR}/src/IceIntrinsics.cpp
        ${SUBZERO_DIR}/src/IceLiveness.cpp
        ${SUBZERO_DIR}/src/IceLoopAnalyzer.cpp
        ${SUBZERO_DIR}/src/IceMangling.cpp
        ${SUBZERO_DIR}/src/IceMemory.cpp
        ${SUBZERO_DIR}/src/IceOperand.cpp
        ${SUBZERO_DIR}/src/IceRangeSpec.cpp
        ${SUBZERO_DIR}/src/IceRegAlloc.cpp
        ${SUBZERO_DIR}/src/IceRevision.cpp
        ${SUBZERO_DIR}/src/IceRNG.cpp
        ${SUBZERO_DIR}/src/IceSwitchLowering.cpp
        ${SUBZERO_DIR}/src/IceTargetLowering.cpp
        ${SUBZERO_DIR}/src/IceThreading.cpp
        ${SUBZERO_DIR}/src/IceTimerTree.cpp
        ${SUBZERO_DIR}/src/IceTypes.cpp
        ${SUBZERO_DIR}/src/IceVariableSplitting.cpp
    )

    # FIXME: Shouldn't depend on external source files directly.
    list(APPEND SUBZERO_LIST
        ${SOURCE_DIR}/Common/Memory.cpp
    )

    if(ARCH STREQUAL "x86_64")
        list(APPEND SUBZERO_LIST
            ${SUBZERO_DIR}/src/IceTargetLoweringX86.cpp
            ${SUBZERO_DIR}/src/IceInstX8664.cpp
            ${SUBZERO_DIR}/src/IceTargetLoweringX8664.cpp
        )
        set(SUBZERO_TARGET X8664)
    elseif(ARCH STREQUAL "x86")
        list(APPEND SUBZERO_LIST
            ${SUBZERO_DIR}/src/IceTargetLoweringX86.cpp
            ${SUBZERO_DIR}/src/IceInstX8632.cpp
            ${SUBZERO_DIR}/src/IceTargetLoweringX8632.cpp
        )
        set(SUBZERO_TARGET X8632)
    elseif(ARCH STREQUAL "arm")
        list(APPEND SUBZERO_LIST
            ${SUBZERO_DIR}/src/IceAssemblerARM32.cpp
            ${SUBZERO_DIR}/src/IceInstARM32.cpp
            ${SUBZERO_DIR}/src/IceTargetLoweringARM32.cpp
        )
        set(SUBZERO_TARGET ARM32)
    elseif(ARCH STREQUAL "mipsel")
        list(APPEND SUBZERO_LIST
            ${SUBZERO_DIR}/src/IceAssemblerMIPS32.cpp
            ${SUBZERO_DIR}/src/IceInstMIPS32.cpp
            ${SUBZERO_DIR}/src/IceTargetLoweringMIPS32.cpp
        )
        set(SUBZERO_TARGET MIPS32)
    else()
        message(FATAL_ERROR "Architecture '${ARCH}' not supported by Subzero")
    endif()

    file(GLOB_RECURSE SUBZERO_DEPENDENCIES_LIST
        ${SUBZERO_LLVM_DIR}/*.cpp
        ${SUBZERO_LLVM_DIR}/*.c
        ${SUBZERO_LLVM_DIR}/*.h
    )

    set(SUBZERO_REACTOR_LIST
        ${SOURCE_DIR}/Reactor/Reactor.cpp
        ${SOURCE_DIR}/Reactor/SubzeroReactor.cpp
        ${SOURCE_DIR}/Reactor/Routine.cpp
        ${SOURCE_DIR}/Reactor/Optimizer.cpp
        ${SOURCE_DIR}/Reactor/Nucleus.hpp
        ${SOURCE_DIR}/Reactor/Routine.hpp
        ${SOURCE_DIR}/Reactor/Debug.cpp
        ${SOURCE_DIR}/Reactor/Debug.hpp
        ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
        ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
    )

    set(SUBZERO_INCLUDE_DIR
        ${SUBZERO_DIR}/
        ${SUBZERO_LLVM_DIR}/include/
        ${SUBZERO_DIR}/pnacl-llvm/include/
    )

    if(WIN32)
        list(APPEND SUBZERO_INCLUDE_DIR ${SUBZERO_LLVM_DIR}/build/Windows/include/)
    elseif(LINUX)
        list(APPEND SUBZERO_INCLUDE_DIR ${SUBZERO_LLVM_DIR}/build/Linux/include/)
    elseif(APPLE)
        list(APPEND SUBZERO_INCLUDE_DIR ${SUBZERO_LLVM_DIR}/build/MacOS/include/)
    endif()

    add_library(ReactorSubzero STATIC
        ${SUBZERO_LIST}
        ${SUBZERO_DEPENDENCIES_LIST}
        ${SUBZERO_REACTOR_LIST}
    )
    set_target_properties(ReactorSubzero PROPERTIES
        POSITION_INDEPENDENT_CODE 1
        INCLUDE_DIRECTORIES "${SUBZERO_INCLUDE_DIR}"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "SZTARGET=${SUBZERO_TARGET}; ALLOW_DUMP=0; ALLOW_TIMERS=0; ALLOW_LLVM_CL=0; ALLOW_LLVM_IR=0; ALLOW_LLVM_IR_AS_INPUT=0; ALLOW_MINIMAL_BUILD=0; ALLOW_WASM=0; ICE_THREAD_LOCAL_HACK=0;"
        FOLDER "Subzero"
    )

    if(WIN32)
        target_compile_definitions(ReactorSubzero PRIVATE SUBZERO_USE_MICROSOFT_ABI)
    endif()
endif()

###########################################################
# Include Directories
###########################################################

set(COMMON_INCLUDE_DIR
    ${SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIR}
    ${LIBBACKTRACE_INCLUDE_DIR}
)
set(OPENGL_INCLUDE_DIR
    ${OPENGL_DIR}
    ${COMMON_INCLUDE_DIR}
)

set(VULKAN_INCLUDE_DIR
    ${COMMON_INCLUDE_DIR}
)

###########################################################
# File Lists
###########################################################

file(GLOB_RECURSE SWIFTSHADER_LIST
    ${SOURCE_DIR}/Common/*.cpp
    ${SOURCE_DIR}/Common/*.h
    ${SOURCE_DIR}/Common/*.hpp
    ${SOURCE_DIR}/Renderer/*.cpp
    ${SOURCE_DIR}/Renderer/*.h
    ${SOURCE_DIR}/Renderer/*.hpp
    ${SOURCE_DIR}/Shader/*.cpp
    ${SOURCE_DIR}/Shader/*.h
    ${SOURCE_DIR}/Shader/*.hpp
)
list(APPEND SWIFTSHADER_LIST
    ${SOURCE_DIR}/Main/Config.cpp
    ${SOURCE_DIR}/Main/Config.hpp
    ${SOURCE_DIR}/Main/FrameBuffer.cpp
    ${SOURCE_DIR}/Main/FrameBuffer.hpp
    ${SOURCE_DIR}/Main/SwiftConfig.cpp
    ${SOURCE_DIR}/Main/SwiftConfig.hpp
)
list(REMOVE_ITEM SWIFTSHADER_LIST
    ${SOURCE_DIR}/Common/DebugAndroid.cpp
    ${SOURCE_DIR}/Common/DebugAndroid.hpp
    ${SOURCE_DIR}/Common/GrallocAndroid.cpp
    ${SOURCE_DIR}/Common/GrallocAndroid.hpp
)

if(REACTOR_EMIT_DEBUG_INFO)
    set(LIBBACKTRACE_LIST
        ${LIBBACKTRACE_DIR}/atomic.c
        ${LIBBACKTRACE_DIR}/backtrace.c
        ${LIBBACKTRACE_DIR}/backtrace.h
        ${LIBBACKTRACE_DIR}/dwarf.c
        ${LIBBACKTRACE_DIR}/fileline.c
        ${LIBBACKTRACE_DIR}/filenames.h
        ${LIBBACKTRACE_DIR}/internal.h
        ${LIBBACKTRACE_DIR}/mmap.c
        ${LIBBACKTRACE_DIR}/mmapio.c
        ${LIBBACKTRACE_DIR}/posix.c
        ${LIBBACKTRACE_DIR}/print.c
        ${LIBBACKTRACE_DIR}/simple.c
        ${LIBBACKTRACE_DIR}/sort.c
        ${LIBBACKTRACE_DIR}/state.c
        ${LIBBACKTRACE_INCLUDE_DIR}/backtrace-supported.h
        ${LIBBACKTRACE_INCLUDE_DIR}/config.h
    )

    if(WIN32)
        list(APPEND LIBBACKTRACE_LIST ${LIBBACKTRACE_DIR}/pecoff.c)
    elseif(LINUX)
        list(APPEND LIBBACKTRACE_LIST ${LIBBACKTRACE_DIR}/elf.c)
    elseif(APPLE)
        message(FATAL_ERROR "libbacktrace does not support mach-o yet")
    endif()
endif(REACTOR_EMIT_DEBUG_INFO)

set(REACTOR_LLVM_LIST
    ${SOURCE_DIR}/Reactor/Reactor.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.hpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.hpp
    ${SOURCE_DIR}/Reactor/Nucleus.hpp
    ${SOURCE_DIR}/Reactor/Routine.cpp
    ${SOURCE_DIR}/Reactor/Routine.hpp
    ${SOURCE_DIR}/Reactor/LLVMRoutine.cpp
    ${SOURCE_DIR}/Reactor/LLVMRoutine.hpp
    ${SOURCE_DIR}/Reactor/CPUID.cpp
    ${SOURCE_DIR}/Reactor/CPUID.hpp
    ${SOURCE_DIR}/Reactor/Debug.cpp
    ${SOURCE_DIR}/Reactor/Debug.hpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
)

file(GLOB_RECURSE EGL_LIST
    ${OPENGL_DIR}/libEGL/*.cpp
    ${OPENGL_DIR}/libEGL/*.h
    ${OPENGL_DIR}/libEGL/*.hpp
    ${OPENGL_DIR}/common/Object.cpp
    ${OPENGL_DIR}/common/Object.hpp
    ${OPENGL_DIR}/common/debug.cpp
    ${OPENGL_DIR}/common/debug.h
    ${SOURCE_DIR}/Common/SharedLibrary.cpp
    ${SOURCE_DIR}/Common/SharedLibrary.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE GLES2_LIST
    ${OPENGL_DIR}/libGLESv2/*.cpp
    ${OPENGL_DIR}/libGLESv2/*.h
    ${OPENGL_DIR}/libGLESv2/*.hpp
    ${OPENGL_DIR}/common/*.cpp
    ${OPENGL_DIR}/common/*.h
    ${OPENGL_DIR}/common/*.hpp
    ${SOURCE_DIR}/Common/SharedLibrary.cpp
    ${SOURCE_DIR}/Common/SharedLibrary.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLES2/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLES3/*.h
)

file(GLOB_RECURSE GLES_CM_LIST
    ${OPENGL_DIR}/libGLES_CM/*.cpp
    ${OPENGL_DIR}/libGLES_CM/*.h
    ${OPENGL_DIR}/libGLES_CM/*.hpp
    ${OPENGL_DIR}/common/*.cpp
    ${OPENGL_DIR}/common/*.h
    ${OPENGL_DIR}/common/*.hpp
    ${SOURCE_DIR}/Common/SharedLibrary.cpp
    ${SOURCE_DIR}/Common/SharedLibrary.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLES/*.h
)

file(GLOB_RECURSE OPENGL_COMPILER_LIST
    ${OPENGL_COMPILER_DIR}/*.cpp
    ${OPENGL_COMPILER_DIR}/*.h
    ${OPENGL_COMPILER_DIR}/*.hpp
)
file(GLOB_RECURSE OPENGL_COMPILER_OS_SPECIFIC_LIST
    ${OPENGL_COMPILER_DIR}/ossource*.cpp
    ${OPENGL_COMPILER_DIR}/ossource*.h
    ${OPENGL_COMPILER_DIR}/ossource*.hpp
)
list(REMOVE_ITEM OPENGL_COMPILER_LIST ${OPENGL_COMPILER_OS_SPECIFIC_LIST})

file(GLOB_RECURSE VULKAN_LIST
    ${VULKAN_DIR}/*.cpp
    ${VULKAN_DIR}/*.h
    ${VULKAN_DIR}/*.hpp
    ${SOURCE_DIR}/System/CPUID.cpp
    ${SOURCE_DIR}/System/CPUID.hpp
    ${SOURCE_DIR}/System/Configurator.cpp
    ${SOURCE_DIR}/System/Configurator.hpp
    ${SOURCE_DIR}/System/Debug.cpp
    ${SOURCE_DIR}/System/Debug.hpp
    ${SOURCE_DIR}/System/Half.cpp
    ${SOURCE_DIR}/System/Half.hpp
    ${SOURCE_DIR}/System/Math.cpp
    ${SOURCE_DIR}/System/Math.hpp
    ${SOURCE_DIR}/System/Memory.cpp
    ${SOURCE_DIR}/System/Memory.hpp
    ${SOURCE_DIR}/System/Resource.cpp
    ${SOURCE_DIR}/System/Resource.hpp
    ${SOURCE_DIR}/System/Socket.cpp
    ${SOURCE_DIR}/System/Socket.hpp
    ${SOURCE_DIR}/System/Thread.cpp
    ${SOURCE_DIR}/System/Thread.hpp
    ${SOURCE_DIR}/System/Timer.cpp
    ${SOURCE_DIR}/System/Timer.hpp
    ${SOURCE_DIR}/Device/*.cpp
    ${SOURCE_DIR}/Device/*.hpp
    ${SOURCE_DIR}/Pipeline/*.cpp
    ${SOURCE_DIR}/Pipeline/*.hpp
    ${SOURCE_DIR}/WSI/VkSurfaceKHR.cpp
    ${SOURCE_DIR}/WSI/VkSurfaceKHR.hpp
    ${SOURCE_DIR}/WSI/VkSwapchainKHR.cpp
    ${SOURCE_DIR}/WSI/VkSwapchainKHR.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/*.h}
)

###########################################################
# Append OS specific files to lists
###########################################################

if(WIN32)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferDD.cpp
        ${SOURCE_DIR}/Main/FrameBufferDD.hpp
        ${SOURCE_DIR}/Main/FrameBufferGDI.cpp
        ${SOURCE_DIR}/Main/FrameBufferGDI.hpp
        ${SOURCE_DIR}/Main/FrameBufferWin.cpp
        ${SOURCE_DIR}/Main/FrameBufferWin.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST ${OPENGL_COMPILER_DIR}/ossource_win.cpp)
    list(APPEND EGL_LIST ${OPENGL_DIR}/libEGL/libEGL.rc)
    list(APPEND GLES2_LIST ${OPENGL_DIR}/libGLESv2/libGLESv2.rc)
    list(APPEND GLES_CM_LIST ${OPENGL_DIR}/libGLES_CM/libGLES_CM.rc)
elseif(LINUX)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferX11.cpp
        ${SOURCE_DIR}/Main/FrameBufferX11.hpp
        ${SOURCE_DIR}/Common/SharedLibrary.hpp
        ${SOURCE_DIR}/Main/libX11.cpp
        ${SOURCE_DIR}/Main/libX11.hpp
    )
    list(APPEND EGL_LIST
        ${SOURCE_DIR}/Common/SharedLibrary.hpp
        ${SOURCE_DIR}/Main/libX11.cpp
        ${SOURCE_DIR}/Main/libX11.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST
        ${OPENGL_COMPILER_DIR}/ossource_posix.cpp
    )

    list(APPEND VULKAN_LIST
        ${SOURCE_DIR}/WSI/libX11.cpp
        ${SOURCE_DIR}/WSI/libX11.hpp
        ${SOURCE_DIR}/WSI/XlibSurfaceKHR.cpp
        ${SOURCE_DIR}/WSI/XlibSurfaceKHR.hpp
    )
elseif(APPLE)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferOSX.mm
        ${SOURCE_DIR}/Main/FrameBufferOSX.hpp
    )
    list(APPEND EGL_LIST
        ${OPENGL_DIR}/libEGL/OSXUtils.mm
        ${OPENGL_DIR}/libEGL/OSXUtils.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST
        ${OPENGL_COMPILER_DIR}/ossource_posix.cpp
    )
elseif(ANDROID)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferAndroid.cpp
        ${SOURCE_DIR}/Main/FrameBufferAndroid.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST
        ${OPENGL_COMPILER_DIR}/ossource_posix.cpp
    )
endif()

if(WIN32)
    set(OS_LIBS odbc32 odbccp32 WS2_32 dxguid)
elseif(LINUX)
    set(OS_LIBS dl pthread)
elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(QUARTZ_FRAMEWORK Quartz)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(IOSURFACE_FRAMEWORK IOSurface)
    set(OS_LIBS "${COCOA_FRAMEWORK}" "${QUARTZ_FRAMEWORK}" "${CORE_FOUNDATION_FRAMEWORK}" "${IOSURFACE_FRAMEWORK}")
endif()

###########################################################
# SwiftShader Targets
###########################################################

add_library(SwiftShader STATIC ${SWIFTSHADER_LIST})
set_target_properties(SwiftShader PROPERTIES
    INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Core"
    COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
    COMPILE_DEFINITIONS "NO_SANITIZE_FUNCTION=;"
)
target_link_libraries(SwiftShader ${OS_LIBS})

if(${REACTOR_BACKEND} STREQUAL "LLVM")
    add_library(ReactorLLVM STATIC ${REACTOR_LLVM_LIST})
    set_target_properties(ReactorLLVM PROPERTIES
        INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
        POSITION_INDEPENDENT_CODE 1
        FOLDER "Core"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
    )
    target_link_libraries(ReactorLLVM llvm ${OS_LIBS})

    if(REACTOR_EMIT_DEBUG_INFO)
        add_library(Libbacktrace STATIC ${LIBBACKTRACE_LIST})
        set_target_properties(Libbacktrace PROPERTIES
            INCLUDE_DIRECTORIES "${LIBBACKTRACE_INCLUDE_DIR}"
            POSITION_INDEPENDENT_CODE 1
            FOLDER "Core"
        )
        target_link_libraries(Libbacktrace ${OS_LIBS})
        target_link_libraries(ReactorLLVM Libbacktrace)
    endif(REACTOR_EMIT_DEBUG_INFO)

    set(Reactor ReactorLLVM)
elseif(${REACTOR_BACKEND} STREQUAL "Subzero")
    set(Reactor ReactorSubzero)
else()
    message(FATAL_ERROR "REACTOR_BACKEND must be 'LLVM' or 'Subzero'")
endif()

add_library(GLCompiler STATIC ${OPENGL_COMPILER_LIST})
set_target_properties(GLCompiler PROPERTIES
    INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
)
target_link_libraries(GLCompiler ${OS_LIBS})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PREFIX "lib64")
else()
    set(LIB_PREFIX "lib")
endif()

if(BUILD_EGL)
    add_library(libEGL SHARED ${EGL_LIST})
    set_target_properties(libEGL PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "EGL_EGLEXT_PROTOTYPES; EGLAPI=; NO_SANITIZE_FUNCTION=;"
        PREFIX ""
    )
    if (ANDROID)
        set_target_properties(libEGL PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libEGL ${SOURCE_DIR}/OpenGL/libEGL)
    target_link_libraries(libEGL ${OS_LIBS})
    add_custom_command(
        TARGET libEGL
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libEGL>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL> $<TARGET_FILE_DIR:libEGL>/translator/${LIB_PREFIX}EGL_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )
endif()

if(BUILD_GLESv2)
    add_library(libGLESv2 SHARED ${GLES2_LIST})
    set_target_properties(libGLESv2 PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES; GL_API=; GL_APICALL=; GLAPI=; NO_SANITIZE_FUNCTION=;"
        PREFIX ""
    )
    if (ANDROID)
        set_target_properties(libGLESv2 PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libGLESv2 ${SOURCE_DIR}/OpenGL/libGLESv2)
    target_link_libraries(libGLESv2 SwiftShader ${Reactor} GLCompiler ${OS_LIBS})
    add_custom_command(
        TARGET libGLESv2
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLESv2>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> $<TARGET_FILE_DIR:libGLESv2>/translator/${LIB_PREFIX}GLES_V2_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )
endif()

if(BUILD_GLES_CM)
    add_library(libGLES_CM SHARED ${GLES_CM_LIST})
    set_target_properties(libGLES_CM PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES; EGLAPI=; GL_API=; GL_APICALL=; GLAPI=;"
        PREFIX ""
    )
    if (ANDROID)
        set_target_properties(libGLES_CM PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libGLES_CM ${SOURCE_DIR}/OpenGL/libGLES_CM)
    target_link_libraries(libGLES_CM SwiftShader ${Reactor} GLCompiler ${OS_LIBS})
    add_custom_command(
        TARGET libGLES_CM
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLES_CM>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLES_CM> $<TARGET_FILE_DIR:libGLES_CM>/translator/${LIB_PREFIX}GLES_CM_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
endif()

if(BUILD_VULKAN)
    if (NOT TARGET SPIRV-Tools)
        # This variable is also used by SPIRV-Tools to locate SPIRV-Headers
        set(SPIRV-Headers_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV-Headers")
        list(APPEND VULKAN_INCLUDE_DIR "${SPIRV-Headers_SOURCE_DIR}/include")

        add_subdirectory(third_party/SPIRV-Tools)
    endif()

    add_library(libvk_swiftshader SHARED ${VULKAN_LIST})
    set_target_properties(libvk_swiftshader PROPERTIES
        INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
        FOLDER "Vulkan"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "NO_SANITIZE_FUNCTION=;"
        PREFIX ""
    )
    set_shared_library_export_map(libvk_swiftshader ${SOURCE_DIR}/Vulkan)
    target_link_libraries(libvk_swiftshader ${Reactor} ${OS_LIBS} SPIRV-Tools SPIRV-Tools-opt)
    add_custom_command(
        TARGET libvk_swiftshader
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libvk_swiftshader>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libvk_swiftshader> $<TARGET_FILE_DIR:libvk_swiftshader>/translator/${LIB_PREFIX}Vulkan_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libvk_swiftshader> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )
    configure_file(
        "${VULKAN_DIR}/vk_swiftshader_icd.json.tmpl"
        "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/vk_swiftshader_icd.json")
endif()

###########################################################
# Sample programs
###########################################################

if(BUILD_SAMPLES)
    if(WIN32)
        add_executable(OGLES2HelloAPI ${HELLO2_DIR}/OGLES2HelloAPI_Windows.cpp)
        set_target_properties(OGLES2HelloAPI PROPERTIES
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
            COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
            COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
            FOLDER "Samples"
        )
        target_link_libraries(OGLES2HelloAPI libEGL libGLESv2)
        set_property(TARGET OGLES2HelloAPI APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS")
        set(CMAKE_DEFAULT_STARTUP_PROJECT OGLES2HelloAPI)
    elseif(LINUX)
        add_executable(OGLES2HelloAPI ${HELLO2_DIR}/OGLES2HelloAPI_LinuxX11.cpp)
        set_target_properties(OGLES2HelloAPI PROPERTIES
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
            COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
            COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
        )
        target_link_libraries(OGLES2HelloAPI dl X11 libEGL libGLESv2)   # Explicitly link our "lib*" targets, not the platform provided "EGL" and "GLESv2"
    elseif(APPLE)
        add_executable(OGLES2HelloAPI MACOSX_BUNDLE
            ${HELLO2_DIR}/OGLES2HelloAPI_OSX.mm
            ${HELLO2_DIR}/Build/OSX/en.lproj/MainMenu.xib
        )
        set_target_properties(OGLES2HelloAPI PROPERTIES
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
            COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};-Wno-deprecated-declarations" # 'NSTitledWindowMask', 'NSClosableWindowMask' is deprecated
            COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
            MACOSX_BUNDLE_INFO_PLIST "${HELLO2_DIR}/Build/OSX/Info.plist"
        )
        target_link_libraries(OGLES2HelloAPI libEGL libGLESv2 ${OS_LIBS})
        set_source_files_properties(${HELLO2_DIR}/Build/OSX/en.lproj/MainMenu.xib PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
    endif()
endif()

if(BUILD_TESTS)
    set(REACTOR_UNIT_TESTS_LIST
        ${SOURCE_DIR}/Reactor/ReactorUnitTests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/src/gtest-all.cc
    )

    set(REACTOR_UNIT_TESTS_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/
    )

    add_executable(ReactorUnitTests ${REACTOR_UNIT_TESTS_LIST})
    set_target_properties(ReactorUnitTests PROPERTIES
        INCLUDE_DIRECTORIES "${REACTOR_UNIT_TESTS_INCLUDE_DIR}"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        FOLDER "Tests"
    )

    if(NOT WIN32 AND ${REACTOR_BACKEND} STREQUAL "Subzero")
        target_link_libraries(ReactorUnitTests ${Reactor} pthread dl)
    else()
        target_link_libraries(ReactorUnitTests ${Reactor})
    endif()
endif()

if(BUILD_TESTS)
    set(GLES_UNITTESTS_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/GLESUnitTests/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/GLESUnitTests/unittests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/src/gtest-all.cc
    )

    set(GLES_UNITTESTS_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )

    add_executable(gles-unittests ${GLES_UNITTESTS_LIST})
    set_target_properties(gles-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${GLES_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "STANDALONE"
    )

    target_link_libraries(gles-unittests libEGL libGLESv2 ${OS_LIBS})
    if(ANDROID)
        target_link_libraries(gles-unittests -landroid)
    endif()
endif()

if(BUILD_TESTS AND BUILD_VULKAN)
    set(VK_UNITTESTS_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/VulkanUnitTests/Device.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/VulkanUnitTests/Driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/VulkanUnitTests/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/VulkanUnitTests/unittests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/src/gtest-all.cc
    )

    set(VK_UNITTESTS_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV-Tools/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )

    add_executable(vk-unittests ${VK_UNITTESTS_LIST})
    set_target_properties(vk-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${VK_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "STANDALONE"
    )

    target_link_libraries(vk-unittests ${OS_LIBS} SPIRV-Tools)
endif()
