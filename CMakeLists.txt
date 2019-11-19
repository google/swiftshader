cmake_minimum_required(VERSION 3.6.3)

project(SwiftShader C CXX ASM)

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
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "ppc64le")
    else()
        message(FATAL_ERROR "Architecture is not supported")
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

# Use CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "Using ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

###########################################################
# Host libraries
###########################################################

find_library(X11 X11)
find_library(XCB xcb)

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

option(MSAN "Build with memory sanitizer" 0)
option(ASAN "Build with address sanitizer" 0)
option(TSAN "Build with thread sanitizer" 0)
option(UBSAN "Build with undefined behavior sanitizer" 0)
option(WARNINGS_AS_ERRORS "Treat all warnings as errors" 1)
option(DCHECK_ALWAYS_ON "Check validation macros even in release builds" 0)
option(REACTOR_EMIT_DEBUG_INFO "Emit debug info for JIT functions" 0)
option(REACTOR_EMIT_PRINT_LOCATION "Emit printing of location info for JIT functions" 0)
option(REACTOR_VERIFY_LLVM_IR "Check reactor-generated LLVM IR is valid even in release builds" 0)
option(LESS_DEBUG_INFO "Generate less debug info to reduce file size" 0)

set(BUILD_MARL ${BUILD_VULKAN})

set(DEFAULT_REACTOR_BACKEND "LLVM")

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

# Boost is downloaded if necessary

# From https://www.boost.org/users/download/
set(BOOST_VER 1.70.0)
set(BOOST_HASH_TARGZ "882b48708d211a5f48e60b0124cf5863c1534cd544ecd0664bb534a4b5d506e9")
set(BOOST_HASH_ZIP   "48f379b2e90dd1084429aae87d6bdbde9670139fa7569ee856c8c86dd366039d")

string(REPLACE "." "_" BOOST_VER_FNAME ${BOOST_VER})
set(BOOST_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost_${BOOST_VER_FNAME})

function(DownloadBoost)
    if (LINUX)
        set(BOOST_EXT "tar.gz")
        set(BOOST_HASH ${BOOST_HASH_TARGZ})
    else()
        set(BOOST_EXT "zip")
        set(BOOST_HASH ${BOOST_HASH_ZIP})
    endif()

    # Note: bintray.com has rate limiting, so use the sourceforge mirror
    # set(BOOST_URL https://dl.bintray.com/boostorg/release/${BOOST_VER}/source/boost_${BOOST_VER_FNAME}.${BOOST_EXT})
    set(BOOST_URL https://iweb.dl.sourceforge.net/project/boost/boost/${BOOST_VER}/boost_${BOOST_VER_FNAME}.${BOOST_EXT})

    if (NOT TARGET Boost::boost)
        if(NOT EXISTS ${BOOST_THIRD_PARTY_DIR})
            message(WARNING "
          third_party/boost is missing.
          Downloading and extracting boost:
            ")

            set(BOOST_ARCHIVE ${CMAKE_BINARY_DIR}/temp/boost_archive)
            message(STATUS "Downloading ${BOOST_URL} to ${BOOST_ARCHIVE}...")
            file(DOWNLOAD ${BOOST_URL} ${BOOST_ARCHIVE} EXPECTED_HASH SHA256=${BOOST_HASH})
            message(STATUS "Extracting ${BOOST_ARCHIVE} to ${BOOST_THIRD_PARTY_DIR}...")
            execute_process(
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third_party
                COMMAND cmake -E tar xf ${BOOST_ARCHIVE}
            )
        endif()
    endif()
endfunction()

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
    elseif(APPLE)
        # The exported symbols list only exports the API functions and
        # hides all the others.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS "-exported_symbols_list ${DIR}/${TARGET}.exports")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_DEPENDS "${DIR}/${TARGET}.exports;")
        # Don't allow undefined symbols, unless it's a Sanitizer build.
        if(NOT MSAN AND NOT ASAN AND NOT TSAN AND NOT UBSAN)
            set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-undefined,error")
        endif()
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
set(MARL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/marl/include)
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

macro(set_linker_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_EXE_LINKER_FLAGS ${ARGV1} "${CMAKE_EXE_LINKER_FLAGS ${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAG}")
    endif()
endmacro()

if(MSVC)
    set_cpp_flag("/MP")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_SBCS)  # Single Byte Character Set (ASCII)
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)  # Disable MSVC warnings about std::aligned_storage being broken before VS 2017 15.8

    if(WARNINGS_AS_ERRORS)
        set_cpp_flag("/WX") # Treat all warnings as errors
    endif()

    # Disable specific warnings
    # TODO: Not all of these should be disabled, but for now, we want a warning-free msvc build. Remove these one by one
    #       and fix the actual warnings in code.
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "/wd4005" # 'identifier' : macro redefinition
        "/wd4018" # 'expression' : signed/unsigned mismatch
        "/wd4065" # switch statement contains 'default' but no 'case' labels
        "/wd4141" # 'modifier' : used more than once
        "/wd4244" # 'conversion' conversion from 'type1' to 'type2', possible loss of data
        "/wd4267" # 'var' : conversion from 'size_t' to 'type', possible loss of data
        "/wd4291" # 'void X new(size_t,unsigned int,unsigned int)': no matching operator delete found; memory will not be freed if initialization throws an exception
        "/wd4309" # 'conversion' : truncation of constant value
        "/wd4624" # 'derived class' : destructor was implicitly defined as deleted because a base class destructor is inaccessible or deleted
        "/wd4800" # 'type' : forcing value to bool 'true' or 'false' (performance warning)
        "/wd4838" # conversion from 'type_1' to 'type_2' requires a narrowing conversion
        "/wd5030" # attribute 'attribute' is not recognized
        "/wd5038" # data member 'member1' will be initialized after data member 'member2' data member 'member' will be initialized after base class 'base_class'
    )

    # Treat specific warnings as errors
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "/we4018" # 'expression' : signed/unsigned mismatch
        "/we4471" # 'enumeration': a forward declaration of an unscoped enumeration must have an underlying type (int assumed)
        "/we4838" # conversion from 'type_1' to 'type_2' requires a narrowing conversion
        "/we5038" # data member 'member1' will be initialized after data member 'member2' data member 'member' will be initialized after base class 'base_class'
    )

else()
    set_cpp_flag("--std=c++11")
    set_cpp_flag("-fno-exceptions")

    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "-Wall"
        "-Wreorder"
        "-Wsign-compare"
        "-Wmissing-braces"
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wunused-lambda-capture"
            "-Wstring-conversion"
            "-Wextra-semi"
            "-Wignored-qualifiers"
            # Silence errors caused by unknown warnings when building with older
            # versions of Clang. This demands checking that warnings added above
            # are spelled correctly and work as intended!
            "-Wno-unknown-warning-option"
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

    if(LESS_DEBUG_INFO)
        # Use -g1 to be able to get stack traces
        set_cpp_flag("-g -g1" DEBUG)
        set_cpp_flag("-g -g1" RELWITHDEBINFO)
    else()
        # Use -g3 to have even more debug info
        set_cpp_flag("-g -g3" DEBUG)
        set_cpp_flag("-g -g3" RELWITHDEBINFO)
    endif()

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

    if(MSAN)
        set_cpp_flag("-fsanitize=memory")
        set_linker_flag("-fsanitize=memory")
    elseif(ASAN)
        set_cpp_flag("-fsanitize=address")
        set_linker_flag("-fsanitize=address")
    elseif(TSAN)
        set_cpp_flag("-fsanitize=thread")
        set_linker_flag("-fsanitize=thread")
    elseif(UBSAN)
        set_cpp_flag("-fsanitize=undefined")
        set_linker_flag("-fsanitize=undefined")
    endif()
endif()

if(REACTOR_EMIT_PRINT_LOCATION)
    # This feature depends on REACTOR_EMIT_DEBUG_INFO, so enable it
    set(REACTOR_EMIT_DEBUG_INFO "On")
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_EMIT_PRINT_LOCATION")
endif()

if(REACTOR_EMIT_DEBUG_INFO)
    message(WARNING "REACTOR_EMIT_DEBUG_INFO is enabled. This will likely affect performance.")
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_DEBUG_INFO")
endif()

if(REACTOR_VERIFY_LLVM_IR)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_LLVM_IR_VERIFICATION")
endif()

if(WIN32)
    add_definitions(-DWINVER=0x501 -DNOMINMAX -DSTRICT)
    set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "" "lib")
endif()

###########################################################
# LLVM
###########################################################

set(LLVM_LIST
    ${LLVM_DIR}/lib/Analysis/AliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/AliasAnalysisSummary.cpp
    ${LLVM_DIR}/lib/Analysis/AliasSetTracker.cpp
    ${LLVM_DIR}/lib/Analysis/AssumptionCache.cpp
    ${LLVM_DIR}/lib/Analysis/BasicAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/BlockFrequencyInfo.cpp
    ${LLVM_DIR}/lib/Analysis/BlockFrequencyInfoImpl.cpp
    ${LLVM_DIR}/lib/Analysis/BranchProbabilityInfo.cpp
    ${LLVM_DIR}/lib/Analysis/CallGraph.cpp
    ${LLVM_DIR}/lib/Analysis/CallGraphSCCPass.cpp
    ${LLVM_DIR}/lib/Analysis/CaptureTracking.cpp
    ${LLVM_DIR}/lib/Analysis/CFG.cpp
    ${LLVM_DIR}/lib/Analysis/CFLAndersAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/CFLSteensAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/CmpInstAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/CodeMetrics.cpp
    ${LLVM_DIR}/lib/Analysis/ConstantFolding.cpp
    ${LLVM_DIR}/lib/Analysis/DemandedBits.cpp
    ${LLVM_DIR}/lib/Analysis/DependenceAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/DivergenceAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/DominanceFrontier.cpp
    ${LLVM_DIR}/lib/Analysis/EHPersonalities.cpp
    ${LLVM_DIR}/lib/Analysis/GlobalsModRef.cpp
    ${LLVM_DIR}/lib/Analysis/InlineCost.cpp
    ${LLVM_DIR}/lib/Analysis/InstructionSimplify.cpp
    ${LLVM_DIR}/lib/Analysis/IteratedDominanceFrontier.cpp
    ${LLVM_DIR}/lib/Analysis/IVUsers.cpp
    ${LLVM_DIR}/lib/Analysis/LazyBlockFrequencyInfo.cpp
    ${LLVM_DIR}/lib/Analysis/LazyBranchProbabilityInfo.cpp
    ${LLVM_DIR}/lib/Analysis/LazyValueInfo.cpp
    ${LLVM_DIR}/lib/Analysis/Loads.cpp
    ${LLVM_DIR}/lib/Analysis/LoopAnalysisManager.cpp
    ${LLVM_DIR}/lib/Analysis/LoopInfo.cpp
    ${LLVM_DIR}/lib/Analysis/LoopPass.cpp
    ${LLVM_DIR}/lib/Analysis/MemoryBuiltins.cpp
    ${LLVM_DIR}/lib/Analysis/MemoryDependenceAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/MemoryLocation.cpp
    ${LLVM_DIR}/lib/Analysis/MemorySSA.cpp
    ${LLVM_DIR}/lib/Analysis/MemorySSAUpdater.cpp
    ${LLVM_DIR}/lib/Analysis/MustExecute.cpp
    ${LLVM_DIR}/lib/Analysis/ObjCARCAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/ObjCARCAnalysisUtils.cpp
    ${LLVM_DIR}/lib/Analysis/ObjCARCInstKind.cpp
    ${LLVM_DIR}/lib/Analysis/OptimizationRemarkEmitter.cpp
    ${LLVM_DIR}/lib/Analysis/OrderedBasicBlock.cpp
    ${LLVM_DIR}/lib/Analysis/PHITransAddr.cpp
    ${LLVM_DIR}/lib/Analysis/PhiValues.cpp
    ${LLVM_DIR}/lib/Analysis/PostDominators.cpp
    ${LLVM_DIR}/lib/Analysis/ProfileSummaryInfo.cpp
    ${LLVM_DIR}/lib/Analysis/PtrUseVisitor.cpp
    ${LLVM_DIR}/lib/Analysis/ScalarEvolution.cpp
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionExpander.cpp
    ${LLVM_DIR}/lib/Analysis/ScalarEvolutionNormalization.cpp
    ${LLVM_DIR}/lib/Analysis/ScopedNoAliasAA.cpp
    ${LLVM_DIR}/lib/Analysis/TargetLibraryInfo.cpp
    ${LLVM_DIR}/lib/Analysis/TargetTransformInfo.cpp
    ${LLVM_DIR}/lib/Analysis/TypeBasedAliasAnalysis.cpp
    ${LLVM_DIR}/lib/Analysis/ValueLattice.cpp
    ${LLVM_DIR}/lib/Analysis/ValueLatticeUtils.cpp
    ${LLVM_DIR}/lib/Analysis/ValueTracking.cpp
    ${LLVM_DIR}/lib/Analysis/VectorUtils.cpp
    ${LLVM_DIR}/lib/AsmParser/LLLexer.cpp
    ${LLVM_DIR}/lib/AsmParser/LLParser.cpp
    ${LLVM_DIR}/lib/AsmParser/Parser.cpp
    ${LLVM_DIR}/lib/BinaryFormat/Dwarf.cpp
    ${LLVM_DIR}/lib/BinaryFormat/Magic.cpp
    ${LLVM_DIR}/lib/BinaryFormat/Wasm.cpp
    ${LLVM_DIR}/lib/Bitcode/Reader/BitcodeReader.cpp
    ${LLVM_DIR}/lib/Bitcode/Reader/BitstreamReader.cpp
    ${LLVM_DIR}/lib/Bitcode/Reader/MetadataLoader.cpp
    ${LLVM_DIR}/lib/Bitcode/Reader/ValueList.cpp
    ${LLVM_DIR}/lib/CodeGen/AggressiveAntiDepBreaker.cpp
    ${LLVM_DIR}/lib/CodeGen/AllocationOrder.cpp
    ${LLVM_DIR}/lib/CodeGen/Analysis.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AccelTable.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AddressPool.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/ARMException.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinter.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinterDwarf.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/AsmPrinterInlineAsm.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/CodeViewDebug.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DbgValueHistoryCalculator.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DebugHandlerBase.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DebugLocStream.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DIE.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DIEHash.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfCFIException.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfCompileUnit.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfDebug.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfExpression.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfFile.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfStringPool.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/DwarfUnit.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/EHStreamer.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/WinCFGuard.cpp
    ${LLVM_DIR}/lib/CodeGen/AsmPrinter/WinException.cpp
    ${LLVM_DIR}/lib/CodeGen/AtomicExpandPass.cpp
    ${LLVM_DIR}/lib/CodeGen/BasicTargetTransformInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/BranchFolding.cpp
    ${LLVM_DIR}/lib/CodeGen/BranchRelaxation.cpp
    ${LLVM_DIR}/lib/CodeGen/BreakFalseDeps.cpp
    ${LLVM_DIR}/lib/CodeGen/CalcSpillWeights.cpp
    ${LLVM_DIR}/lib/CodeGen/CallingConvLower.cpp
    ${LLVM_DIR}/lib/CodeGen/CFIInstrInserter.cpp
    ${LLVM_DIR}/lib/CodeGen/CodeGen.cpp
    ${LLVM_DIR}/lib/CodeGen/CodeGenPrepare.cpp
    ${LLVM_DIR}/lib/CodeGen/CriticalAntiDepBreaker.cpp
    ${LLVM_DIR}/lib/CodeGen/DeadMachineInstructionElim.cpp
    ${LLVM_DIR}/lib/CodeGen/DetectDeadLanes.cpp
    ${LLVM_DIR}/lib/CodeGen/DFAPacketizer.cpp
    ${LLVM_DIR}/lib/CodeGen/DwarfEHPrepare.cpp
    ${LLVM_DIR}/lib/CodeGen/EarlyIfConversion.cpp
    ${LLVM_DIR}/lib/CodeGen/EdgeBundles.cpp
    ${LLVM_DIR}/lib/CodeGen/ExecutionDomainFix.cpp
    ${LLVM_DIR}/lib/CodeGen/ExpandISelPseudos.cpp
    ${LLVM_DIR}/lib/CodeGen/ExpandMemCmp.cpp
    ${LLVM_DIR}/lib/CodeGen/ExpandPostRAPseudos.cpp
    ${LLVM_DIR}/lib/CodeGen/ExpandReductions.cpp
    ${LLVM_DIR}/lib/CodeGen/FaultMaps.cpp
    ${LLVM_DIR}/lib/CodeGen/FEntryInserter.cpp
    ${LLVM_DIR}/lib/CodeGen/FuncletLayout.cpp
    ${LLVM_DIR}/lib/CodeGen/GCMetadata.cpp
    ${LLVM_DIR}/lib/CodeGen/GCMetadataPrinter.cpp
    ${LLVM_DIR}/lib/CodeGen/GCRootLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/GCStrategy.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/CallLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/GlobalISel.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/InstructionSelect.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/InstructionSelector.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/IRTranslator.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalityPredicates.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizeMutations.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Legalizer.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizerHelper.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/LegalizerInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Localizer.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/MachineIRBuilder.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegBankSelect.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegisterBank.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/RegisterBankInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/GlobalISel/Utils.cpp
    ${LLVM_DIR}/lib/CodeGen/IfConversion.cpp
    ${LLVM_DIR}/lib/CodeGen/ImplicitNullChecks.cpp
    ${LLVM_DIR}/lib/CodeGen/IndirectBrExpandPass.cpp
    ${LLVM_DIR}/lib/CodeGen/InlineSpiller.cpp
    ${LLVM_DIR}/lib/CodeGen/InterferenceCache.cpp
    ${LLVM_DIR}/lib/CodeGen/InterleavedAccessPass.cpp
    ${LLVM_DIR}/lib/CodeGen/IntrinsicLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/LatencyPriorityQueue.cpp
    ${LLVM_DIR}/lib/CodeGen/LazyMachineBlockFrequencyInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/LexicalScopes.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveDebugValues.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveDebugVariables.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveInterval.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveIntervals.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveIntervalUnion.cpp
    ${LLVM_DIR}/lib/CodeGen/LivePhysRegs.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveRangeCalc.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveRangeEdit.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveRangeShrink.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveRegMatrix.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveRegUnits.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveStacks.cpp
    ${LLVM_DIR}/lib/CodeGen/LiveVariables.cpp
    ${LLVM_DIR}/lib/CodeGen/LLVMTargetMachine.cpp
    ${LLVM_DIR}/lib/CodeGen/LocalStackSlotAllocation.cpp
    ${LLVM_DIR}/lib/CodeGen/LoopTraversal.cpp
    ${LLVM_DIR}/lib/CodeGen/LowerEmuTLS.cpp
    ${LLVM_DIR}/lib/CodeGen/LowLevelType.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineBasicBlock.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineBlockFrequencyInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineBlockPlacement.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineBranchProbabilityInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineCombiner.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineCopyPropagation.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineCSE.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineDominanceFrontier.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineDominators.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineFrameInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineFunction.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineFunctionPass.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineFunctionPrinterPass.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineInstr.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineInstrBundle.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineLICM.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineLoopInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineModuleInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineModuleInfoImpls.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineOperand.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineOptimizationRemarkEmitter.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineOutliner.cpp
    ${LLVM_DIR}/lib/CodeGen/MachinePassRegistry.cpp
    ${LLVM_DIR}/lib/CodeGen/MachinePipeliner.cpp
    ${LLVM_DIR}/lib/CodeGen/MachinePostDominators.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineRegionInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineRegisterInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineScheduler.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineSink.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineSSAUpdater.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineTraceMetrics.cpp
    ${LLVM_DIR}/lib/CodeGen/MachineVerifier.cpp
    ${LLVM_DIR}/lib/CodeGen/MacroFusion.cpp
    ${LLVM_DIR}/lib/CodeGen/MIRCanonicalizerPass.cpp
    ${LLVM_DIR}/lib/CodeGen/MIRPrinter.cpp
    ${LLVM_DIR}/lib/CodeGen/MIRPrintingPass.cpp
    ${LLVM_DIR}/lib/CodeGen/OptimizePHIs.cpp
    ${LLVM_DIR}/lib/CodeGen/PatchableFunction.cpp
    ${LLVM_DIR}/lib/CodeGen/PeepholeOptimizer.cpp
    ${LLVM_DIR}/lib/CodeGen/PHIElimination.cpp
    ${LLVM_DIR}/lib/CodeGen/PHIEliminationUtils.cpp
    ${LLVM_DIR}/lib/CodeGen/PostRAHazardRecognizer.cpp
    ${LLVM_DIR}/lib/CodeGen/PostRASchedulerList.cpp
    ${LLVM_DIR}/lib/CodeGen/PreISelIntrinsicLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/ProcessImplicitDefs.cpp
    ${LLVM_DIR}/lib/CodeGen/PrologEpilogInserter.cpp
    ${LLVM_DIR}/lib/CodeGen/PseudoSourceValue.cpp
    ${LLVM_DIR}/lib/CodeGen/ReachingDefAnalysis.cpp
    ${LLVM_DIR}/lib/CodeGen/RegAllocBase.cpp
    ${LLVM_DIR}/lib/CodeGen/RegAllocBasic.cpp
    ${LLVM_DIR}/lib/CodeGen/RegAllocFast.cpp
    ${LLVM_DIR}/lib/CodeGen/RegAllocGreedy.cpp
    ${LLVM_DIR}/lib/CodeGen/RegisterClassInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/RegisterCoalescer.cpp
    ${LLVM_DIR}/lib/CodeGen/RegisterPressure.cpp
    ${LLVM_DIR}/lib/CodeGen/RegisterScavenging.cpp
    ${LLVM_DIR}/lib/CodeGen/RegisterUsageInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/RegUsageInfoCollector.cpp
    ${LLVM_DIR}/lib/CodeGen/RegUsageInfoPropagate.cpp
    ${LLVM_DIR}/lib/CodeGen/RenameIndependentSubregs.cpp
    ${LLVM_DIR}/lib/CodeGen/ResetMachineFunctionPass.cpp
    ${LLVM_DIR}/lib/CodeGen/SafeStack.cpp
    ${LLVM_DIR}/lib/CodeGen/SafeStackColoring.cpp
    ${LLVM_DIR}/lib/CodeGen/SafeStackLayout.cpp
    ${LLVM_DIR}/lib/CodeGen/ScalarizeMaskedMemIntrin.cpp
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAG.cpp
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAGInstrs.cpp
    ${LLVM_DIR}/lib/CodeGen/ScheduleDAGPrinter.cpp
    ${LLVM_DIR}/lib/CodeGen/ScoreboardHazardRecognizer.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/DAGCombiner.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/FastISel.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/FunctionLoweringInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/InstrEmitter.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeDAG.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeFloatTypes.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeIntegerTypes.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeTypes.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeTypesGeneric.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeVectorOps.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/LegalizeVectorTypes.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ResourcePriorityQueue.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGRRList.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGSDNodes.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/ScheduleDAGVLIW.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAG.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGAddressAnalysis.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGBuilder.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGDumper.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGISel.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGPrinter.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/SelectionDAGTargetInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/StatepointLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/SelectionDAG/TargetLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/ShadowStackGCLowering.cpp
    ${LLVM_DIR}/lib/CodeGen/ShrinkWrap.cpp
    ${LLVM_DIR}/lib/CodeGen/SjLjEHPrepare.cpp
    ${LLVM_DIR}/lib/CodeGen/SlotIndexes.cpp
    ${LLVM_DIR}/lib/CodeGen/SpillPlacement.cpp
    ${LLVM_DIR}/lib/CodeGen/SplitKit.cpp
    ${LLVM_DIR}/lib/CodeGen/StackColoring.cpp
    ${LLVM_DIR}/lib/CodeGen/StackMapLivenessAnalysis.cpp
    ${LLVM_DIR}/lib/CodeGen/StackMaps.cpp
    ${LLVM_DIR}/lib/CodeGen/StackProtector.cpp
    ${LLVM_DIR}/lib/CodeGen/StackSlotColoring.cpp
    ${LLVM_DIR}/lib/CodeGen/TailDuplication.cpp
    ${LLVM_DIR}/lib/CodeGen/TailDuplicator.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetFrameLoweringImpl.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetInstrInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetLoweringBase.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetLoweringObjectFileImpl.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetOptionsImpl.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetPassConfig.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetRegisterInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetSchedule.cpp
    ${LLVM_DIR}/lib/CodeGen/TargetSubtargetInfo.cpp
    ${LLVM_DIR}/lib/CodeGen/TwoAddressInstructionPass.cpp
    ${LLVM_DIR}/lib/CodeGen/UnreachableBlockElim.cpp
    ${LLVM_DIR}/lib/CodeGen/ValueTypes.cpp
    ${LLVM_DIR}/lib/CodeGen/VirtRegMap.cpp
    ${LLVM_DIR}/lib/CodeGen/WasmEHPrepare.cpp
    ${LLVM_DIR}/lib/CodeGen/WinEHPrepare.cpp
    ${LLVM_DIR}/lib/CodeGen/XRayInstrumentation.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CodeViewError.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CodeViewRecordIO.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/ContinuationRecordBuilder.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/CVTypeVisitor.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/Formatters.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/GlobalTypeTableBuilder.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/Line.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/RecordName.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/RecordSerialization.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/SimpleTypeSerializer.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/SymbolRecordMapping.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeDumpVisitor.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeHashing.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeIndex.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeIndexDiscovery.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeRecordMapping.cpp
    ${LLVM_DIR}/lib/DebugInfo/CodeView/TypeTableCollection.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/ExecutionEngine.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/GDBRegistrationListener.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/Core.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/Legacy.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/Orc/OrcError.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/JITSymbol.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RTDyldMemoryManager.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyld.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldChecker.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldCOFF.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldELF.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/RuntimeDyldMachO.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/RuntimeDyld/Targets/RuntimeDyldELFMips.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/SectionMemoryManager.cpp
    ${LLVM_DIR}/lib/ExecutionEngine/TargetSelect.cpp
    ${LLVM_DIR}/lib/IR/AsmWriter.cpp
    ${LLVM_DIR}/lib/IR/Attributes.cpp
    ${LLVM_DIR}/lib/IR/AutoUpgrade.cpp
    ${LLVM_DIR}/lib/IR/BasicBlock.cpp
    ${LLVM_DIR}/lib/IR/Comdat.cpp
    ${LLVM_DIR}/lib/IR/ConstantFold.cpp
    ${LLVM_DIR}/lib/IR/ConstantRange.cpp
    ${LLVM_DIR}/lib/IR/Constants.cpp
    ${LLVM_DIR}/lib/IR/DataLayout.cpp
    ${LLVM_DIR}/lib/IR/DebugInfo.cpp
    ${LLVM_DIR}/lib/IR/DebugInfoMetadata.cpp
    ${LLVM_DIR}/lib/IR/DebugLoc.cpp
    ${LLVM_DIR}/lib/IR/DiagnosticHandler.cpp
    ${LLVM_DIR}/lib/IR/DiagnosticInfo.cpp
    ${LLVM_DIR}/lib/IR/DiagnosticPrinter.cpp
    ${LLVM_DIR}/lib/IR/DIBuilder.cpp
    ${LLVM_DIR}/lib/IR/Dominators.cpp
    ${LLVM_DIR}/lib/IR/Function.cpp
    ${LLVM_DIR}/lib/IR/Globals.cpp
    ${LLVM_DIR}/lib/IR/GVMaterializer.cpp
    ${LLVM_DIR}/lib/IR/InlineAsm.cpp
    ${LLVM_DIR}/lib/IR/Instruction.cpp
    ${LLVM_DIR}/lib/IR/Instructions.cpp
    ${LLVM_DIR}/lib/IR/IntrinsicInst.cpp
    ${LLVM_DIR}/lib/IR/IRBuilder.cpp
    ${LLVM_DIR}/lib/IR/IRPrintingPasses.cpp
    ${LLVM_DIR}/lib/IR/LegacyPassManager.cpp
    ${LLVM_DIR}/lib/IR/LLVMContext.cpp
    ${LLVM_DIR}/lib/IR/LLVMContextImpl.cpp
    ${LLVM_DIR}/lib/IR/Mangler.cpp
    ${LLVM_DIR}/lib/IR/MDBuilder.cpp
    ${LLVM_DIR}/lib/IR/Metadata.cpp
    ${LLVM_DIR}/lib/IR/Module.cpp
    ${LLVM_DIR}/lib/IR/Operator.cpp
    ${LLVM_DIR}/lib/IR/OptBisect.cpp
    ${LLVM_DIR}/lib/IR/Pass.cpp
    ${LLVM_DIR}/lib/IR/PassManager.cpp
    ${LLVM_DIR}/lib/IR/PassRegistry.cpp
    ${LLVM_DIR}/lib/IR/ProfileSummary.cpp
    ${LLVM_DIR}/lib/IR/Statepoint.cpp
    ${LLVM_DIR}/lib/IR/Type.cpp
    ${LLVM_DIR}/lib/IR/TypeFinder.cpp
    ${LLVM_DIR}/lib/IR/Use.cpp
    ${LLVM_DIR}/lib/IR/User.cpp
    ${LLVM_DIR}/lib/IR/Value.cpp
    ${LLVM_DIR}/lib/IR/ValueSymbolTable.cpp
    ${LLVM_DIR}/lib/IR/Verifier.cpp
    ${LLVM_DIR}/lib/MC/ELFObjectWriter.cpp
    ${LLVM_DIR}/lib/MC/MachObjectWriter.cpp
    ${LLVM_DIR}/lib/MC/MCAsmBackend.cpp
    ${LLVM_DIR}/lib/MC/MCAsmInfo.cpp
    ${LLVM_DIR}/lib/MC/MCAsmInfoCOFF.cpp
    ${LLVM_DIR}/lib/MC/MCAsmInfoDarwin.cpp
    ${LLVM_DIR}/lib/MC/MCAsmInfoELF.cpp
    ${LLVM_DIR}/lib/MC/MCAsmMacro.cpp
    ${LLVM_DIR}/lib/MC/MCAsmStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCAssembler.cpp
    ${LLVM_DIR}/lib/MC/MCCodeEmitter.cpp
    ${LLVM_DIR}/lib/MC/MCCodePadder.cpp
    ${LLVM_DIR}/lib/MC/MCCodeView.cpp
    ${LLVM_DIR}/lib/MC/MCContext.cpp
    ${LLVM_DIR}/lib/MC/MCDisassembler/MCRelocationInfo.cpp
    ${LLVM_DIR}/lib/MC/MCDwarf.cpp
    ${LLVM_DIR}/lib/MC/MCELFObjectTargetWriter.cpp
    ${LLVM_DIR}/lib/MC/MCELFStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCExpr.cpp
    ${LLVM_DIR}/lib/MC/MCFragment.cpp
    ${LLVM_DIR}/lib/MC/MCInst.cpp
    ${LLVM_DIR}/lib/MC/MCInstPrinter.cpp
    ${LLVM_DIR}/lib/MC/MCInstrAnalysis.cpp
    ${LLVM_DIR}/lib/MC/MCInstrDesc.cpp
    ${LLVM_DIR}/lib/MC/MCLinkerOptimizationHint.cpp
    ${LLVM_DIR}/lib/MC/MCMachObjectTargetWriter.cpp
    ${LLVM_DIR}/lib/MC/MCMachOStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCNullStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCObjectFileInfo.cpp
    ${LLVM_DIR}/lib/MC/MCObjectStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCObjectWriter.cpp
    ${LLVM_DIR}/lib/MC/MCParser/AsmLexer.cpp
    ${LLVM_DIR}/lib/MC/MCParser/AsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCParser/COFFAsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCParser/DarwinAsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCParser/ELFAsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmLexer.cpp
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCParser/MCAsmParserExtension.cpp
    ${LLVM_DIR}/lib/MC/MCParser/MCTargetAsmParser.cpp
    ${LLVM_DIR}/lib/MC/MCRegisterInfo.cpp
    ${LLVM_DIR}/lib/MC/MCSchedule.cpp
    ${LLVM_DIR}/lib/MC/MCSection.cpp
    ${LLVM_DIR}/lib/MC/MCSectionCOFF.cpp
    ${LLVM_DIR}/lib/MC/MCSectionELF.cpp
    ${LLVM_DIR}/lib/MC/MCSectionMachO.cpp
    ${LLVM_DIR}/lib/MC/MCSectionWasm.cpp
    ${LLVM_DIR}/lib/MC/MCStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCSubtargetInfo.cpp
    ${LLVM_DIR}/lib/MC/MCSymbol.cpp
    ${LLVM_DIR}/lib/MC/MCSymbolELF.cpp
    ${LLVM_DIR}/lib/MC/MCTargetOptions.cpp
    ${LLVM_DIR}/lib/MC/MCValue.cpp
    ${LLVM_DIR}/lib/MC/MCWasmStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCWin64EH.cpp
    ${LLVM_DIR}/lib/MC/MCWinCOFFStreamer.cpp
    ${LLVM_DIR}/lib/MC/MCWinEH.cpp
    ${LLVM_DIR}/lib/MC/StringTableBuilder.cpp
    ${LLVM_DIR}/lib/MC/SubtargetFeature.cpp
    ${LLVM_DIR}/lib/MC/WasmObjectWriter.cpp
    ${LLVM_DIR}/lib/MC/WinCOFFObjectWriter.cpp
    ${LLVM_DIR}/lib/Object/Archive.cpp
    ${LLVM_DIR}/lib/Object/Binary.cpp
    ${LLVM_DIR}/lib/Object/COFFObjectFile.cpp
    ${LLVM_DIR}/lib/Object/ELF.cpp
    ${LLVM_DIR}/lib/Object/ELFObjectFile.cpp
    ${LLVM_DIR}/lib/Object/Error.cpp
    ${LLVM_DIR}/lib/Object/IRObjectFile.cpp
    ${LLVM_DIR}/lib/Object/IRSymtab.cpp
    ${LLVM_DIR}/lib/Object/MachOObjectFile.cpp
    ${LLVM_DIR}/lib/Object/MachOUniversal.cpp
    ${LLVM_DIR}/lib/Object/ModuleSymbolTable.cpp
    ${LLVM_DIR}/lib/Object/ObjectFile.cpp
    ${LLVM_DIR}/lib/Object/RecordStreamer.cpp
    ${LLVM_DIR}/lib/Object/SymbolicFile.cpp
    ${LLVM_DIR}/lib/Object/WasmObjectFile.cpp
    ${LLVM_DIR}/lib/Object/WindowsResource.cpp
    ${LLVM_DIR}/lib/ProfileData/InstrProf.cpp
    ${LLVM_DIR}/lib/Support/APFloat.cpp
    ${LLVM_DIR}/lib/Support/APInt.cpp
    ${LLVM_DIR}/lib/Support/APSInt.cpp
    ${LLVM_DIR}/lib/Support/ARMAttributeParser.cpp
    ${LLVM_DIR}/lib/Support/ARMBuildAttrs.cpp
    ${LLVM_DIR}/lib/Support/Atomic.cpp
    ${LLVM_DIR}/lib/Support/BinaryStreamError.cpp
    ${LLVM_DIR}/lib/Support/BinaryStreamReader.cpp
    ${LLVM_DIR}/lib/Support/BinaryStreamRef.cpp
    ${LLVM_DIR}/lib/Support/BinaryStreamWriter.cpp
    ${LLVM_DIR}/lib/Support/BlockFrequency.cpp
    ${LLVM_DIR}/lib/Support/BranchProbability.cpp
    ${LLVM_DIR}/lib/Support/Chrono.cpp
    ${LLVM_DIR}/lib/Support/circular_raw_ostream.cpp
    ${LLVM_DIR}/lib/Support/CodeGenCoverage.cpp
    ${LLVM_DIR}/lib/Support/CommandLine.cpp
    ${LLVM_DIR}/lib/Support/Compression.cpp
    ${LLVM_DIR}/lib/Support/ConvertUTF.cpp
    ${LLVM_DIR}/lib/Support/ConvertUTFWrapper.cpp
    ${LLVM_DIR}/lib/Support/DataExtractor.cpp
    ${LLVM_DIR}/lib/Support/Debug.cpp
    ${LLVM_DIR}/lib/Support/DebugCounter.cpp
    ${LLVM_DIR}/lib/Support/DJB.cpp
    ${LLVM_DIR}/lib/Support/DynamicLibrary.cpp
    ${LLVM_DIR}/lib/Support/Errno.cpp
    ${LLVM_DIR}/lib/Support/Error.cpp
    ${LLVM_DIR}/lib/Support/ErrorHandling.cpp
    ${LLVM_DIR}/lib/Support/FoldingSet.cpp
    ${LLVM_DIR}/lib/Support/FormattedStream.cpp
    ${LLVM_DIR}/lib/Support/FormatVariadic.cpp
    ${LLVM_DIR}/lib/Support/GraphWriter.cpp
    ${LLVM_DIR}/lib/Support/Hashing.cpp
    ${LLVM_DIR}/lib/Support/Host.cpp
    ${LLVM_DIR}/lib/Support/IntEqClasses.cpp
    ${LLVM_DIR}/lib/Support/IntervalMap.cpp
    ${LLVM_DIR}/lib/Support/JamCRC.cpp
    ${LLVM_DIR}/lib/Support/KnownBits.cpp
    ${LLVM_DIR}/lib/Support/LEB128.cpp
    ${LLVM_DIR}/lib/Support/LineIterator.cpp
    ${LLVM_DIR}/lib/Support/Locale.cpp
    ${LLVM_DIR}/lib/Support/LowLevelType.cpp
    ${LLVM_DIR}/lib/Support/ManagedStatic.cpp
    ${LLVM_DIR}/lib/Support/MathExtras.cpp
    ${LLVM_DIR}/lib/Support/MD5.cpp
    ${LLVM_DIR}/lib/Support/Memory.cpp
    ${LLVM_DIR}/lib/Support/MemoryBuffer.cpp
    ${LLVM_DIR}/lib/Support/Mutex.cpp
    ${LLVM_DIR}/lib/Support/NativeFormatting.cpp
    ${LLVM_DIR}/lib/Support/Path.cpp
    ${LLVM_DIR}/lib/Support/PrettyStackTrace.cpp
    ${LLVM_DIR}/lib/Support/Process.cpp
    ${LLVM_DIR}/lib/Support/Program.cpp
    ${LLVM_DIR}/lib/Support/RandomNumberGenerator.cpp
    ${LLVM_DIR}/lib/Support/raw_ostream.cpp
    ${LLVM_DIR}/lib/Support/regcomp.c
    ${LLVM_DIR}/lib/Support/regerror.c
    ${LLVM_DIR}/lib/Support/Regex.cpp
    ${LLVM_DIR}/lib/Support/regexec.c
    ${LLVM_DIR}/lib/Support/regfree.c
    ${LLVM_DIR}/lib/Support/regstrlcpy.c
    ${LLVM_DIR}/lib/Support/RWMutex.cpp
    ${LLVM_DIR}/lib/Support/ScaledNumber.cpp
    ${LLVM_DIR}/lib/Support/ScopedPrinter.cpp
    ${LLVM_DIR}/lib/Support/SHA1.cpp
    ${LLVM_DIR}/lib/Support/Signals.cpp
    ${LLVM_DIR}/lib/Support/SmallPtrSet.cpp
    ${LLVM_DIR}/lib/Support/SmallVector.cpp
    ${LLVM_DIR}/lib/Support/SourceMgr.cpp
    ${LLVM_DIR}/lib/Support/Statistic.cpp
    ${LLVM_DIR}/lib/Support/StringExtras.cpp
    ${LLVM_DIR}/lib/Support/StringMap.cpp
    ${LLVM_DIR}/lib/Support/StringRef.cpp
    ${LLVM_DIR}/lib/Support/StringSaver.cpp
    ${LLVM_DIR}/lib/Support/TargetParser.cpp
    ${LLVM_DIR}/lib/Support/TargetRegistry.cpp
    ${LLVM_DIR}/lib/Support/Threading.cpp
    ${LLVM_DIR}/lib/Support/Timer.cpp
    ${LLVM_DIR}/lib/Support/ToolOutputFile.cpp
    ${LLVM_DIR}/lib/Support/Triple.cpp
    ${LLVM_DIR}/lib/Support/Twine.cpp
    ${LLVM_DIR}/lib/Support/Unicode.cpp
    ${LLVM_DIR}/lib/Support/UnicodeCaseFold.cpp
    ${LLVM_DIR}/lib/Support/Valgrind.cpp
    ${LLVM_DIR}/lib/Support/YAMLParser.cpp
    ${LLVM_DIR}/lib/Support/YAMLTraits.cpp
    ${LLVM_DIR}/lib/Target/TargetLoweringObjectFile.cpp
    ${LLVM_DIR}/lib/Target/TargetMachine.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/CoroCleanup.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/CoroEarly.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/CoroElide.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/CoroFrame.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/CoroSplit.cpp
    ${LLVM_DIR}/lib/Transforms/Coroutines/Coroutines.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineAddSub.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineAndOrXor.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCalls.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCasts.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineCompares.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineLoadStoreAlloca.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineMulDivRem.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombinePHI.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineSelect.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineShifts.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineSimplifyDemanded.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstCombineVectorOps.cpp
    ${LLVM_DIR}/lib/Transforms/InstCombine/InstructionCombining.cpp
    ${LLVM_DIR}/lib/Transforms/IPO/BarrierNoopPass.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/ADCE.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/ConstantHoisting.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/DeadStoreElimination.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/EarlyCSE.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/GVN.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/LICM.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/LoopDataPrefetch.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/LoopStrengthReduce.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/MergeICmps.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/PartiallyInlineLibCalls.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/Reassociate.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/SCCP.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/SeparateConstOffsetFromGEP.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/SimplifyCFGPass.cpp
    ${LLVM_DIR}/lib/Transforms/Scalar/SROA.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/BasicBlockUtils.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/BreakCriticalEdges.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/BuildLibCalls.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/BypassSlowDivision.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/CloneFunction.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/DemoteRegToStack.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/EntryExitInstrumenter.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/EscapeEnumerator.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/InlineFunction.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/LCSSA.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/Local.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/LoopSimplify.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/LoopUtils.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/LowerInvoke.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/OrderedInstructions.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/PromoteMemoryToRegister.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/SimplifyCFG.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/SimplifyLibCalls.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/SSAUpdater.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/SymbolRewriter.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/ValueMapper.cpp
    ${LLVM_DIR}/lib/Transforms/Utils/VNCoercion.cpp
    ${LLVM_DIR}/../stubs/Stubs.cpp
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
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86MCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/X86/MCTargetDesc/X86WinCOFFTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/X86/ShadowCallStack.cpp
        ${LLVM_DIR}/lib/Target/X86/TargetInfo/X86TargetInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/Utils/X86ShuffleDecode.cpp
        ${LLVM_DIR}/lib/Target/X86/X86AsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/X86/X86AvoidStoreForwardingBlocks.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallFrameOptimization.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallingConv.cpp
        ${LLVM_DIR}/lib/Target/X86/X86CallLowering.cpp
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
        ${LLVM_DIR}/lib/Target/X86/X86IndirectBranchTracking.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrFMA3Info.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrFoldTables.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/X86/X86InterleavedAccess.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/X86/X86ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/X86/X86LegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MachineFunctionInfo.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MacroFusion.cpp
        ${LLVM_DIR}/lib/Target/X86/X86MCInstLower.cpp
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
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCExpr.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsMCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsNaClELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsOptionRecord.cpp
        ${LLVM_DIR}/lib/Target/Mips/MCTargetDesc/MipsTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/Mips/MicroMipsSizeReduction.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16HardFloat.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16HardFloatInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/Mips16RegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsAnalyzeImmediate.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsAsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsBranchExpansion.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsCallLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsCCState.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsConstantIslandPass.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsDelaySlotFiller.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsExpandPseudo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsFastISel.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsInstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsISelLowering.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsLegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsMachineFunction.cpp
        ${LLVM_DIR}/lib/Target/Mips/MipsMCInstLower.cpp
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
        ${LLVM_DIR}/lib/Target/Mips/TargetInfo/MipsTargetInfo.cpp
    )
elseif(ARCH STREQUAL "aarch64")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/CodeGen/GlobalMerge.cpp
        ${LLVM_DIR}/lib/CodeGen/RegAllocPBQP.cpp
        ${LLVM_DIR}/lib/MC/ConstantPools.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64A53Fix835769.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64A57FPLoadBalancing.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64AdvSIMDScalarPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64AsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CallLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CleanupLocalDynamicTLSPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CollectLOH.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64CondBrTuning.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ConditionalCompares.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ConditionOptimizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64DeadRegisterDefinitionsPass.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ExpandPseudoInsts.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FalkorHWPFFix.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FastISel.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64InstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64ISelLowering.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64LegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64LoadStoreOptimizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64MacroFusion.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64MCInstLower.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64PBQPRegAlloc.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64PromoteConstant.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RedundantCopyElimination.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64RegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64SelectionDAGInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64SIMDInstrOpt.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64StorePairSuppress.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64Subtarget.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetMachine.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AArch64TargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/AsmParser/AArch64AsmParser.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Disassembler/AArch64Disassembler.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Disassembler/AArch64ExternalSymbolizer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/InstPrinter/AArch64InstPrinter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64AsmBackend.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64ELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64ELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCExpr.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64MCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64TargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64WinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/AArch64/MCTargetDesc/AArch64WinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/AArch64/TargetInfo/AArch64TargetInfo.cpp
        ${LLVM_DIR}/lib/Target/AArch64/Utils/AArch64BaseInfo.cpp
        ${LLVM_DIR}/lib/Transforms/Scalar/LoopDataPrefetch.cpp
        ${LLVM_DIR}/lib/Transforms/Scalar/SeparateConstOffsetFromGEP.cpp
    )
elseif(ARCH STREQUAL "arm")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/ARM/A15SDOptimizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMAsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMBaseInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMBaseRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMCallLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMCodeGenPrepare.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMComputeBlockSize.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMConstantIslandPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMConstantPoolValue.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMExpandPseudoInsts.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMFastISel.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMHazardRecognizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMInstructionSelector.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMISelLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMLegalizerInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMLoadStoreOptimizer.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMachineFunctionInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMacroFusion.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMMCInstLower.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMOptimizeBarriersPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMParallelDSP.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMRegisterBankInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMSelectionDAGInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMSubtarget.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetMachine.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/ARM/ARMTargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/AsmParser/ARMAsmParser.cpp
        ${LLVM_DIR}/lib/Target/ARM/Disassembler/ARMDisassembler.cpp
        ${LLVM_DIR}/lib/Target/ARM/InstPrinter/ARMInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMAsmBackend.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMELFStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMachORelocationInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCExpr.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMMCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMTargetStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMUnwindOpAsm.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMWinCOFFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/ARM/MCTargetDesc/ARMWinCOFFStreamer.cpp
        ${LLVM_DIR}/lib/Target/ARM/MLxExpansionPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/TargetInfo/ARMTargetInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb1FrameLowering.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb1InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2InstrInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2ITBlockPass.cpp
        ${LLVM_DIR}/lib/Target/ARM/Thumb2SizeReduction.cpp
        ${LLVM_DIR}/lib/Target/ARM/ThumbRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/ARM/Utils/ARMBaseInfo.cpp
    )
elseif(ARCH STREQUAL "ppc64le")
    list(APPEND LLVM_LIST
        ${LLVM_DIR}/lib/Target/PowerPC/AsmParser/PPCAsmParser.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/Disassembler/PPCDisassembler.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/InstPrinter/PPCInstPrinter.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCAsmBackend.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCELFObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCMachObjectWriter.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCMCAsmInfo.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCMCCodeEmitter.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCMCExpr.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCMCTargetDesc.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/MCTargetDesc/PPCPredicates.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCAsmPrinter.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCBoolRetToInt.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCBranchCoalescing.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCBranchSelector.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCCCState.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCCTRLoops.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCEarlyReturn.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCExpandISEL.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCFastISel.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCFrameLowering.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCHazardRecognizers.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCInstrInfo.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCISelDAGToDAG.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCISelLowering.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCLoopPreIncPrep.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCMachineFunctionInfo.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCMCInstLower.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCMIPeephole.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCPreEmitPeephole.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCQPXLoadSplat.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCReduceCRLogicals.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCRegisterInfo.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCSubtarget.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCTargetMachine.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCTargetObjectFile.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCTargetTransformInfo.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCTLSDynamicCall.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCTOCRegDeps.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCVSXCopy.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCVSXFMAMutate.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/PPCVSXSwapRemoval.cpp
        ${LLVM_DIR}/lib/Target/PowerPC/TargetInfo/PowerPCTargetInfo.cpp
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
    ${LLVM_DIR}/lib/Target/PowerPC
    ${LLVM_DIR}/lib/Target/X86
    ${LLVM_CONFIG_DIR}/common/include
    ${LLVM_CONFIG_DIR}/common/lib/IR
    ${LLVM_CONFIG_DIR}/common/lib/Target/AArch64
    ${LLVM_CONFIG_DIR}/common/lib/Target/ARM
    ${LLVM_CONFIG_DIR}/common/lib/Target/Mips
    ${LLVM_CONFIG_DIR}/common/lib/Target/PowerPC
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
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        list(APPEND LLVM_COMPILE_OPTIONS
            "-Wno-class-memaccess"  # memset/memcmp used on non-trivial class object
            "-Wno-attributes"	# declared inline but not inline
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    list(APPEND LLVM_COMPILE_OPTIONS
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
        ${SOURCE_DIR}/Reactor/Debug.cpp
        ${SOURCE_DIR}/Reactor/Debug.hpp
        ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
        ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
        ${SOURCE_DIR}/Reactor/Nucleus.hpp
        ${SOURCE_DIR}/Reactor/Optimizer.cpp
        ${SOURCE_DIR}/Reactor/Print.hpp
        ${SOURCE_DIR}/Reactor/Reactor.cpp
        ${SOURCE_DIR}/Reactor/Reactor.hpp
        ${SOURCE_DIR}/Reactor/Routine.hpp
        ${SOURCE_DIR}/Reactor/SubzeroReactor.cpp
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

    if(WIN32)
        list(APPEND SUBZERO_COMPILE_OPTIONS
            "/wd4146" # unary minus operator applied to unsigned type, result still unsigned
            "/wd4334" # ''operator' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
            "/wd4996" # The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: new_name.
        )
    endif()

    add_library(ReactorSubzero STATIC
        ${SUBZERO_LIST}
        ${SUBZERO_DEPENDENCIES_LIST}
        ${SUBZERO_REACTOR_LIST}
    )
    set_target_properties(ReactorSubzero PROPERTIES
        POSITION_INDEPENDENT_CODE 1
        INCLUDE_DIRECTORIES "${SUBZERO_INCLUDE_DIR}"
        COMPILE_OPTIONS "${SUBZERO_COMPILE_OPTIONS};${SWIFTSHADER_COMPILE_OPTIONS}"
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
    ${MARL_INCLUDE_DIR}
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
    ${SOURCE_DIR}/Reactor/CPUID.cpp
    ${SOURCE_DIR}/Reactor/CPUID.hpp
    ${SOURCE_DIR}/Reactor/Debug.cpp
    ${SOURCE_DIR}/Reactor/Debug.hpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.hpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.hpp
    ${SOURCE_DIR}/Reactor/Nucleus.hpp
    ${SOURCE_DIR}/Reactor/Print.hpp
    ${SOURCE_DIR}/Reactor/Reactor.cpp
    ${SOURCE_DIR}/Reactor/Reactor.hpp
    ${SOURCE_DIR}/Reactor/Routine.hpp
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
    ${SOURCE_DIR}/System/Build.hpp
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
    ${SOURCE_DIR}/System/Socket.cpp
    ${SOURCE_DIR}/System/Socket.hpp
    ${SOURCE_DIR}/System/Synchronization.hpp
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

if (LINUX OR ANDROID)
    list(APPEND VULKAN_LIST
        ${SOURCE_DIR}/System/Linux/MemFd.cpp
	${SOURCE_DIR}/System/Linux/MemFd.hpp)
endif(LINUX OR ANDROID)

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
    list(APPEND VULKAN_LIST
        ${VULKAN_DIR}/Vulkan.rc
        ${SOURCE_DIR}/WSI/Win32SurfaceKHR.cpp
        ${SOURCE_DIR}/WSI/Win32SurfaceKHR.hpp
    )
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

    if(X11)
        list(APPEND VULKAN_LIST
            ${SOURCE_DIR}/WSI/XlibSurfaceKHR.cpp
            ${SOURCE_DIR}/WSI/XlibSurfaceKHR.hpp
            ${SOURCE_DIR}/WSI/libX11.cpp
            ${SOURCE_DIR}/WSI/libX11.hpp
        )
    endif(X11)

    if(XCB)
        list(APPEND VULKAN_LIST
            ${SOURCE_DIR}/WSI/XcbSurfaceKHR.cpp
            ${SOURCE_DIR}/WSI/XcbSurfaceKHR.hpp
        )
    endif(XCB)

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

    list(APPEND VULKAN_LIST
        ${SOURCE_DIR}/WSI/MetalSurface.mm
        ${SOURCE_DIR}/WSI/MetalSurface.h
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
        if(WIN32)
            # Boost stacktrace uses COM on Windows.
            # On Windows, cache COM instances in TLS for performance.
            target_compile_definitions(ReactorLLVM PRIVATE BOOST_STACKTRACE_USE_WINDBG_CACHED)
        else()
            # Boost stacktrace uses libbacktrace
            target_compile_definitions(ReactorLLVM PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)

            # Boost stacktrace uses exceptions, so force enable it
            set_cpp_flag("-fexceptions")

            add_library(Libbacktrace STATIC ${LIBBACKTRACE_LIST})
            set_target_properties(Libbacktrace PROPERTIES
                INCLUDE_DIRECTORIES "${LIBBACKTRACE_INCLUDE_DIR}"
                POSITION_INDEPENDENT_CODE 1
                FOLDER "Core"
            )
            target_link_libraries(Libbacktrace ${OS_LIBS})
            target_link_libraries(ReactorLLVM Libbacktrace)
        endif()

        # Download boost and use it
        DownloadBoost()
        set(BOOST_INCLUDEDIR ${BOOST_THIRD_PARTY_DIR})
        find_package(Boost REQUIRED)
        target_link_libraries(ReactorLLVM Boost::boost)

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
        COMPILE_DEFINITIONS "EGL_EGLEXT_PROTOTYPES; EGLAPI=; NO_SANITIZE_FUNCTION=;$<$<CONFIG:Debug>:DEBUGGER_WAIT_DIALOG>"
        PREFIX ""
    )
    if(LINUX)
        if(X11)
            set_property(TARGET libEGL APPEND PROPERTY COMPILE_DEFINITIONS "USE_X11=1")
        endif(X11)
    endif()
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

if(BUILD_MARL)
    add_subdirectory(third_party/marl)
endif(BUILD_MARL)

if(BUILD_VULKAN)
    if (NOT TARGET SPIRV-Tools)
        # This variable is also used by SPIRV-Tools to locate SPIRV-Headers
        set(SPIRV-Headers_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV-Headers")
        list(APPEND VULKAN_INCLUDE_DIR "${SPIRV-Headers_SOURCE_DIR}/include")

        add_subdirectory(third_party/SPIRV-Tools)
    endif()

    add_library(vk_swiftshader SHARED ${VULKAN_LIST})

    set_target_properties(vk_swiftshader PROPERTIES
        INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
        FOLDER "Vulkan"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "VK_EXPORT=;NO_SANITIZE_FUNCTION=;$<$<CONFIG:Debug>:DEBUGGER_WAIT_DIALOG>"
    )

    if(WIN32)
        set_property(TARGET vk_swiftshader APPEND
                     PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_WIN32_KHR")
    elseif(LINUX)
        if(X11)
            set_property(TARGET vk_swiftshader APPEND
                        PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_XLIB_KHR")
        endif(X11)
        if(XCB)
            set_property(TARGET vk_swiftshader APPEND
                        PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_XCB_KHR")
        endif(XCB)
    elseif(APPLE)
        set_property(TARGET vk_swiftshader APPEND
                     PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_MACOS_MVK")
    endif()

    set_shared_library_export_map(vk_swiftshader ${SOURCE_DIR}/Vulkan)
    target_link_libraries(vk_swiftshader ${Reactor} marl ${OS_LIBS} SPIRV-Tools SPIRV-Tools-opt)
    add_custom_command(
        TARGET vk_swiftshader
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:vk_swiftshader>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vk_swiftshader> $<TARGET_FILE_DIR:vk_swiftshader>/translator/${LIB_PREFIX}Vulkan_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vk_swiftshader> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )

    # The vk_swiftshader_icd.json manifest file will point to ICD_LIBRARY_PATH.
    # Set ICD_LIBRARY_PATH to be a relative path similar to "./libvk_swiftshader.so", so both files can be moved.
    # A relative path is relative to the manifest file.
    set(ICD_LIBRARY_PATH "${CMAKE_SHARED_LIBRARY_PREFIX}vk_swiftshader${CMAKE_SHARED_LIBRARY_SUFFIX}")
    if(WIN32)
        # The path is output to a JSON file, which requires backslashes to be escaped.
        set(ICD_LIBRARY_PATH ".\\\\${ICD_LIBRARY_PATH}")
    else()
        set(ICD_LIBRARY_PATH "./${ICD_LIBRARY_PATH}")
    endif()
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
        target_link_libraries(OGLES2HelloAPI dl ${X11} libEGL libGLESv2)   # Explicitly link our "lib*" targets, not the platform provided "EGL" and "GLESv2"
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
endif(BUILD_TESTS)

if(BUILD_TESTS)
    # Math unit tests
    set(MATH_UNITTESTS_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/MathUnitTests/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/MathUnitTests/unittests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/src/gtest-all.cc
    )

    set(MATH_UNITTESTS_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
    )

    add_executable(math-unittests ${MATH_UNITTESTS_LIST})
    set_target_properties(math-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${MATH_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
    )

    if(NOT WIN32)
        target_link_libraries(math-unittests pthread)
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
    add_dependencies(vk-unittests vk_swiftshader)

    set_target_properties(vk-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${VK_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS}"
        COMPILE_DEFINITIONS "STANDALONE"
    )

    target_link_libraries(vk-unittests ${OS_LIBS} SPIRV-Tools)
endif(BUILD_TESTS AND BUILD_VULKAN)
