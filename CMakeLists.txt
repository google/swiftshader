# Copyright 2020 The SwiftShader Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.6.3)

set(CMAKE_CXX_STANDARD 14)

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
    set(LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
    set(ANDROID TRUE)
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

set(CMAKE_MACOSX_RPATH TRUE)

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
endif()

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

function (option_if_not_defined name description default)
    if(NOT DEFINED ${name})
        option(${name} ${description} ${default})
    endif()
endfunction()

function (set_if_not_defined name value)
    if(NOT DEFINED ${name})
        set(${name} ${value} PARENT_SCOPE)
    endif()
endfunction()

option_if_not_defined(SWIFTSHADER_BUILD_EGL "Build the EGL library" TRUE)
option_if_not_defined(SWIFTSHADER_BUILD_GLESv2 "Build the OpenGL ES 2 library" TRUE)
option_if_not_defined(SWIFTSHADER_BUILD_GLES_CM "Build the OpenGL ES 1.1 library" TRUE)
option_if_not_defined(SWIFTSHADER_BUILD_VULKAN "Build the Vulkan library" TRUE)
option_if_not_defined(SWIFTSHADER_BUILD_PVR "Build the PowerVR examples" TRUE)
option_if_not_defined(SWIFTSHADER_GET_PVR "Check out the PowerVR submodule" FALSE)

option_if_not_defined(SWIFTSHADER_USE_GROUP_SOURCES "Group the source files in a folder tree for Visual Studio" TRUE)

option_if_not_defined(SWIFTSHADER_BUILD_TESTS "Build unit tests" TRUE)
option_if_not_defined(SWIFTSHADER_BUILD_BENCHMARKS "Build benchmarks" FALSE)

option_if_not_defined(SWIFTSHADER_MSAN "Build with memory sanitizer" FALSE)
option_if_not_defined(SWIFTSHADER_ASAN "Build with address sanitizer" FALSE)
option_if_not_defined(SWIFTSHADER_TSAN "Build with thread sanitizer" FALSE)
option_if_not_defined(SWIFTSHADER_UBSAN "Build with undefined behavior sanitizer" FALSE)
option_if_not_defined(SWIFTSHADER_EMIT_COVERAGE "Emit code coverage information" FALSE)
option_if_not_defined(SWIFTSHADER_WARNINGS_AS_ERRORS "Treat all warnings as errors" TRUE)
option_if_not_defined(SWIFTSHADER_DCHECK_ALWAYS_ON "Check validation macros even in release builds" FALSE)
option_if_not_defined(REACTOR_EMIT_DEBUG_INFO "Emit debug info for JIT functions" FALSE)
option_if_not_defined(REACTOR_EMIT_PRINT_LOCATION "Emit printing of location info for JIT functions" FALSE)
option_if_not_defined(REACTOR_ENABLE_PRINT "Enable RR_PRINT macros" FALSE)
option_if_not_defined(REACTOR_VERIFY_LLVM_IR "Check reactor-generated LLVM IR is valid even in release builds" FALSE)
option_if_not_defined(SWIFTSHADER_LESS_DEBUG_INFO "Generate less debug info to reduce file size" FALSE)
option_if_not_defined(SWIFTSHADER_ENABLE_VULKAN_DEBUGGER "Enable Vulkan debugger support" FALSE)
option_if_not_defined(SWIFTSHADER_ENABLE_ASTC "Enable ASTC compressed textures support" TRUE)  # TODO(b/150130101)

set(BUILD_MARL ${SWIFTSHADER_BUILD_VULKAN})

if(${SWIFTSHADER_BUILD_VULKAN} AND ${SWIFTSHADER_ENABLE_VULKAN_DEBUGGER})
    set_if_not_defined(SWIFTSHADER_BUILD_CPPDAP TRUE)
else()
    set_if_not_defined(SWIFTSHADER_BUILD_CPPDAP FALSE)
endif()

set(DEFAULT_REACTOR_BACKEND "LLVM")
set(REACTOR_BACKEND ${DEFAULT_REACTOR_BACKEND} CACHE STRING "JIT compiler back-end used by Reactor")
set_property(CACHE REACTOR_BACKEND PROPERTY STRINGS LLVM Subzero)

set(DEFAULT_SWIFTSHADER_LLVM_VERSION "7.0")
set(SWIFTSHADER_LLVM_VERSION ${DEFAULT_SWIFTSHADER_LLVM_VERSION} CACHE STRING "LLVM version to use")
set_property(CACHE SWIFTSHADER_LLVM_VERSION PROPERTY STRINGS "7.0" "10.0")

# If defined, overrides the default optimization level of the current reactor backend.
# Set to one of the rr::Optimization::Level enum values.
set(REACTOR_DEFAULT_OPT_LEVEL "Default" CACHE STRING "Reactor default optimization level")
set_property(CACHE REACTOR_DEFAULT_OPT_LEVEL PROPERTY STRINGS "None" "Less" "Default" "Aggressive")

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

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

###########################################################
# Initialize submodules
###########################################################

if (SWIFTSHADER_BUILD_TESTS)
    if (NOT TARGET gtest)
        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/.git)
            message(WARNING "
        third_party/googletest submodule missing.
        Running 'git submodule update --init' to download it:
            ")

            execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest)
        endif()
    endif()
endif()

if(SWIFTSHADER_BUILD_BENCHMARKS)
    if (NOT TARGET benchmark::benchmark)
        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/benchmark/.git)
            message(WARNING "
        third_party/benchmark submodule missing.
        Running 'git submodule update --init' to download it:
            ")

            execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/third_party/benchmark)
        endif()
    endif()
endif(SWIFTSHADER_BUILD_BENCHMARKS)

if (NOT TARGET libbacktrace)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace/src/.git)
        message(WARNING "
      third_party/libbacktrace/src submodule missing.
      Running 'git submodule update --init' to download it:
        ")

        execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbacktrace)
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

if(SWIFTSHADER_GET_PVR)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/PowerVR_Examples/.git)
        message(WARNING "
        third_party/PowerVR_Examples submodule missing.
        Running 'git submodule update --init' to download it:
        ")

        execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/third_party/PowerVR_Examples)
    endif()
    set(SWIFTSHADER_GET_PVR FALSE CACHE BOOL "Check out the PowerVR submodule" FORCE)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/PowerVR_Examples/.git)
    set(HAVE_PVR_SUBMODULE TRUE)
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
    elseif(APPLE)
        # The exported symbols list only exports the API functions and
        # hides all the others.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS "-exported_symbols_list ${DIR}/${TARGET}.exports")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_DEPENDS "${DIR}/${TARGET}.exports;")
        # Don't allow undefined symbols, unless it's a Sanitizer build.
        if(NOT SWIFTSHADER_MSAN AND NOT SWIFTSHADER_ASAN AND NOT SWIFTSHADER_TSAN AND NOT SWIFTSHADER_UBSAN)
            set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-undefined,error")
        endif()
    elseif(LINUX)
        # The version script only exports the API functions and
        # hides all the others.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--version-script=${DIR}/${TARGET}.lds")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_DEPENDS "${DIR}/${TARGET}.lds;")

        # -Bsymbolic binds symbol references to their global definitions within
        # a shared object, thereby preventing symbol preemption.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS "  -Wl,-Bsymbolic")

        if(ARCH STREQUAL "mipsel" OR ARCH STREQUAL "mips64el")
          # MIPS supports sysv hash-style only.
          set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--hash-style=sysv")
        else()
          # Both hash-style are needed, because we want both gold and
          # GNU ld to be able to read our libraries.
          set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--hash-style=both")
        endif()

        if(NOT ${SWIFTSHADER_EMIT_COVERAGE})
            # Gc sections is used in combination with each functions being
            # in its own section, to reduce the binary size.
            set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--gc-sections")
        endif()

        # Don't allow undefined symbols, unless it's a Sanitizer build.
        if(NOT SWIFTSHADER_MSAN AND NOT SWIFTSHADER_ASAN AND NOT SWIFTSHADER_TSAN AND NOT SWIFTSHADER_UBSAN)
            set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--no-undefined")
        endif()
    endif()
endmacro()

if(SWIFTSHADER_USE_GROUP_SOURCES)
    group_all_sources(src)
endif()

###########################################################
# Directories
###########################################################

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(OPENGL_DIR ${SOURCE_DIR}/OpenGL)
set(OPENGL_COMPILER_DIR ${OPENGL_DIR}/compiler)
set(VULKAN_DIR ${SOURCE_DIR}/Vulkan)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(ASTC_DIR ${THIRD_PARTY_DIR}/astc-encoder)
set(LIBBACKTRACE_DIR ${THIRD_PARTY_DIR}/libbacktrace/src)
set(LIBBACKTRACE_CONFIG_DIR ${THIRD_PARTY_DIR}/libbacktrace/config)
set(LIBBACKTRACE_INCLUDE_DIR ${THIRD_PARTY_DIR}/libbacktrace/src)
set(CPPDAP_DIR ${THIRD_PARTY_DIR}/cppdap)
set(CPPDAP_INCLUDE_DIR ${CPPDAP_DIR}/include)
set(JSON_INCLUDE_DIR ${THIRD_PARTY_DIR}/json/include)
set(SUBZERO_DIR ${THIRD_PARTY_DIR}/subzero)
set(SUBZERO_LLVM_DIR ${THIRD_PARTY_DIR}/llvm-subzero)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(HELLO2_DIR ${THIRD_PARTY_DIR}/PowerVR_SDK/Examples/Beginner/01_HelloAPI/OGLES2)

###########################################################
# Compile flags
###########################################################

# Flags for project code (non 3rd party)
set(SWIFTSHADER_COMPILE_OPTIONS "")
set(SWIFTSHADER_LINK_FLAGS "")
set(SWIFTSHADER_LIBS "")

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

    set_cpp_flag("/DEBUG:FASTLINK" DEBUG)
    set_cpp_flag("/DEBUG:FASTLINK" RELWITHDEBINFO)

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
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "-fno-exceptions"
    )

    # Explicitly enable these warnings.
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
        )
    endif()

    if (SWIFTSHADER_EMIT_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            list(APPEND SWIFTSHADER_COMPILE_OPTIONS "--coverage")
            list(APPEND SWIFTSHADER_LIBS "gcov")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-fprofile-instr-generate" "-fcoverage-mapping")
            list(APPEND SWIFTSHADER_LINK_FLAGS "-fprofile-instr-generate" "-fcoverage-mapping")
        else()
            message(FATAL_ERROR "Coverage generation not supported for the ${CMAKE_CXX_COMPILER_ID} toolchain")
        endif()
    endif()

    # Disable pedanitc warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wno-ignored-attributes"   # ignoring attributes on template argument 'X'
            "-Wno-attributes"           # 'X' attribute ignored
            "-Wno-strict-aliasing"      # dereferencing type-punned pointer will break strict-aliasing rules
            "-Wno-comment"              # multi-line comment
        )
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9)
            list(APPEND SWIFTSHADER_COMPILE_OPTIONS
                "-Wno-init-list-lifetime"  # assignment from temporary initializer_list does not extend the lifetime of the underlying array
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wno-unneeded-internal-declaration"  # function 'X' is not needed and will not be emitted
            "-Wno-unused-private-field"           # private field 'offset' is not used - TODO: Consider enabling this once Vulkan is further implemented.
            "-Wno-comment"                        # multi-line comment
            "-Wno-undefined-var-template"         # instantiation of variable 'X' required here, but no definition is available
            "-Wno-extra-semi"                     # extra ';' after member function definition

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
        set_cpp_flag("-EL")
        set_cpp_flag("-march=mips32r2")
        set_cpp_flag("-fPIC")
        set_cpp_flag("-mhard-float")
        set_cpp_flag("-mfp32")
        set_cpp_flag("-mxgot")
    endif()
    if(ARCH STREQUAL "mips64el")
        set_cpp_flag("-EL")
        set_cpp_flag("-march=mips64r2")
        set_cpp_flag("-mabi=64")
        set_cpp_flag("-fPIC")
        set_cpp_flag("-mxgot")
    endif()

    if(SWIFTSHADER_LESS_DEBUG_INFO)
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

    if(SWIFTSHADER_MSAN)
        set_cpp_flag("-fsanitize=memory")
        set_linker_flag("-fsanitize=memory")
    elseif(SWIFTSHADER_ASAN)
        set_cpp_flag("-fsanitize=address")
        set_linker_flag("-fsanitize=address")
    elseif(SWIFTSHADER_TSAN)
        set_cpp_flag("-fsanitize=thread")
        set_linker_flag("-fsanitize=thread")
    elseif(SWIFTSHADER_UBSAN)
        set_cpp_flag("-fsanitize=undefined")
        set_linker_flag("-fsanitize=undefined")
    endif()
endif()

if(SWIFTSHADER_DCHECK_ALWAYS_ON)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DDCHECK_ALWAYS_ON")
endif()

if(SWIFTSHADER_WARNINGS_AS_ERRORS)
    if(MSVC)
        set(WARNINGS_AS_ERRORS "/WX")  # Treat all warnings as errors
    else()
        set(WARNINGS_AS_ERRORS "-Werror")  # Treat all warnings as errors
    endif()
endif()

if(REACTOR_EMIT_PRINT_LOCATION)
    # This feature depends on REACTOR_EMIT_DEBUG_INFO and REACTOR_ENABLE_PRINT
    set(REACTOR_EMIT_DEBUG_INFO "On")
    set(REACTOR_ENABLE_PRINT "On")
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_EMIT_PRINT_LOCATION")
endif()

if(REACTOR_EMIT_DEBUG_INFO)
    message(WARNING "REACTOR_EMIT_DEBUG_INFO is enabled. This will likely affect performance.")
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_DEBUG_INFO")
endif()

if(REACTOR_ENABLE_PRINT)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_PRINT")
endif()

if(REACTOR_VERIFY_LLVM_IR)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_RR_LLVM_IR_VERIFICATION")
endif()

if(REACTOR_DEFAULT_OPT_LEVEL)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DREACTOR_DEFAULT_OPT_LEVEL=${REACTOR_DEFAULT_OPT_LEVEL}")
endif()

if(WIN32)
    add_definitions(-DWINVER=0x501 -DNOMINMAX -DSTRICT)
    set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "" "lib")
endif()

# Transform SWIFTSHADER_LINK_FLAGS from semicolon delimited to whitespace
# delimited (what is expected by LINK_FLAGS)
string(REPLACE ";" " " SWIFTSHADER_LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}")

###########################################################
# LLVM
###########################################################
add_subdirectory(third_party/llvm-${SWIFTSHADER_LLVM_VERSION})


###########################################################
# marl
###########################################################
if(BUILD_MARL)
    set(MARL_THIRD_PARTY_DIR ${THIRD_PARTY_DIR})
    add_subdirectory(third_party/marl)
endif()


###########################################################
# cppdap
###########################################################
if(SWIFTSHADER_BUILD_CPPDAP)
    set(CPPDAP_THIRD_PARTY_DIR ${THIRD_PARTY_DIR})
    add_subdirectory(${CPPDAP_DIR})
endif()


###########################################################
# Subzero
###########################################################

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
    ${SOURCE_DIR}/Reactor/EmulatedReactor.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
    ${SOURCE_DIR}/Reactor/Nucleus.hpp
    ${SOURCE_DIR}/Reactor/Optimizer.cpp
    ${SOURCE_DIR}/Reactor/Print.hpp
    ${SOURCE_DIR}/Reactor/Reactor.cpp
    ${SOURCE_DIR}/Reactor/Reactor.hpp
    ${SOURCE_DIR}/Reactor/ReactorDebugInfo.cpp
    ${SOURCE_DIR}/Reactor/ReactorDebugInfo.hpp
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
    list(APPEND SUBZERO_DEPENDENCIES_COMPILE_OPTIONS
        ${SUBZERO_COMPILE_OPTIONS}
        "/wd4267"  # '=': conversion from 'size_t' to 'sopno', possible loss of data
        "/wd4244"  # '=': conversion from '__int64' to 'llvm_regoff_t', possible loss of data
        "/wd4141"  # 'inline': used more than once
        "/wd4291"  # 'void *operator new(size_t,const `anonymous-namespace'::NamedBufferAlloc &)': no matching operator delete found; memory will not be freed if initialization throws an exception
    )
endif()

add_library(SubzeroDependencies STATIC EXCLUDE_FROM_ALL
    ${SUBZERO_DEPENDENCIES_LIST}
)
set_target_properties(SubzeroDependencies PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    INCLUDE_DIRECTORIES "${SUBZERO_INCLUDE_DIR}"
    COMPILE_OPTIONS "${SUBZERO_DEPENDENCIES_COMPILE_OPTIONS}"
    FOLDER "Subzero"
)

add_library(ReactorSubzero STATIC EXCLUDE_FROM_ALL
    ${SUBZERO_LIST}
    ${SUBZERO_REACTOR_LIST}
)
set_target_properties(ReactorSubzero PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    INCLUDE_DIRECTORIES "${SUBZERO_INCLUDE_DIR}"
    COMPILE_OPTIONS "${SUBZERO_COMPILE_OPTIONS};${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
    COMPILE_DEFINITIONS "SZTARGET=${SUBZERO_TARGET}; ALLOW_DUMP=0; ALLOW_TIMERS=0; ALLOW_LLVM_CL=0; ALLOW_LLVM_IR=0; ALLOW_LLVM_IR_AS_INPUT=0; ALLOW_MINIMAL_BUILD=0; ALLOW_WASM=0; ICE_THREAD_LOCAL_HACK=0;"
    FOLDER "Subzero"
)
target_link_libraries(ReactorSubzero SubzeroDependencies marl)

if(WIN32)
    target_compile_definitions(ReactorSubzero PRIVATE SUBZERO_USE_MICROSOFT_ABI)
endif()

###########################################################
# Include Directories
###########################################################

set(COMMON_INCLUDE_DIR
    ${SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIBBACKTRACE_INCLUDE_DIR}
)
set(OPENGL_INCLUDE_DIR
    ${OPENGL_DIR}
    ${COMMON_INCLUDE_DIR}
)

set(VULKAN_INCLUDE_DIR
    ${COMMON_INCLUDE_DIR}
    ${CPPDAP_INCLUDE_DIR}
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
    ${SOURCE_DIR}/Reactor/EmulatedReactor.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.cpp
    ${SOURCE_DIR}/Reactor/ExecutableMemory.hpp
    ${SOURCE_DIR}/Reactor/LLVMJIT.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactor.hpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.cpp
    ${SOURCE_DIR}/Reactor/LLVMReactorDebugInfo.hpp
    ${SOURCE_DIR}/Reactor/Nucleus.hpp
    ${SOURCE_DIR}/Reactor/Print.hpp
    ${SOURCE_DIR}/Reactor/Reactor.cpp
    ${SOURCE_DIR}/Reactor/Reactor.hpp
    ${SOURCE_DIR}/Reactor/ReactorDebugInfo.cpp
    ${SOURCE_DIR}/Reactor/ReactorDebugInfo.hpp
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

file(GLOB VULKAN_LIST
    ${VULKAN_DIR}/*.cpp
    ${VULKAN_DIR}/*.h
    ${VULKAN_DIR}/*.hpp
    ${SOURCE_DIR}/System/Build.cpp
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

if(SWIFTSHADER_ENABLE_VULKAN_DEBUGGER)
    file(GLOB_RECURSE VULKAN_DEBUG_LIST
        ${VULKAN_DIR}/Debug/*.cpp
        ${VULKAN_DIR}/Debug/*.h
        ${VULKAN_DIR}/Debug/*.hpp
    )
    list(APPEND VULKAN_LIST ${VULKAN_DEBUG_LIST})
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DENABLE_VK_DEBUGGER")
endif()

if(SWIFTSHADER_ENABLE_ASTC)
    file(GLOB_RECURSE VULKAN_ASTC_LIST
        ${ASTC_DIR}/Source/*.cpp
        ${ASTC_DIR}/Source/*.h
    )
    list(APPEND VULKAN_LIST ${VULKAN_ASTC_LIST})
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-DSWIFTSHADER_ENABLE_ASTC")
endif()

if(LINUX OR ANDROID)
    list(APPEND VULKAN_LIST
        ${SOURCE_DIR}/System/Linux/MemFd.cpp
	${SOURCE_DIR}/System/Linux/MemFd.hpp)
endif()

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
    endif()

    if(XCB)
        list(APPEND VULKAN_LIST
            ${SOURCE_DIR}/WSI/XcbSurfaceKHR.cpp
            ${SOURCE_DIR}/WSI/XcbSurfaceKHR.hpp
        )
    endif()

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
    find_library(METAL_FRAMEWORK Metal)
    set(OS_LIBS "${COCOA_FRAMEWORK}" "${QUARTZ_FRAMEWORK}" "${CORE_FOUNDATION_FRAMEWORK}" "${IOSURFACE_FRAMEWORK}" "${METAL_FRAMEWORK}")
endif()

###########################################################
# SwiftShader Targets
###########################################################

add_library(SwiftShader STATIC EXCLUDE_FROM_ALL ${SWIFTSHADER_LIST})
set_target_properties(SwiftShader PROPERTIES
    INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Core"
    COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
    COMPILE_DEFINITIONS "NO_SANITIZE_FUNCTION=;"
    LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
)
target_link_libraries(SwiftShader ${OS_LIBS} ${SWIFTSHADER_LIBS})

add_library(ReactorLLVM STATIC EXCLUDE_FROM_ALL ${REACTOR_LLVM_LIST})
set_target_properties(ReactorLLVM PROPERTIES
    INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Core"
    COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
    LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
)
target_link_libraries(ReactorLLVM llvm ${OS_LIBS})

if(${REACTOR_BACKEND} STREQUAL "LLVM")
    set(Reactor ReactorLLVM)
elseif(${REACTOR_BACKEND} STREQUAL "Subzero")
    set(Reactor ReactorSubzero)
else()
    message(FATAL_ERROR "REACTOR_BACKEND must be 'LLVM' or 'Subzero'")
endif()

if(REACTOR_EMIT_DEBUG_INFO)
    if(WIN32)
        # Boost stacktrace uses COM on Windows.
        # On Windows, cache COM instances in TLS for performance.
        target_compile_definitions(${Reactor} PRIVATE BOOST_STACKTRACE_USE_WINDBG_CACHED)
    else()
        # Boost stacktrace uses libbacktrace
        target_compile_definitions(${Reactor} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)

        # Boost stacktrace uses exceptions, so force enable it
        set_cpp_flag("-fexceptions")

        add_library(Libbacktrace STATIC EXCLUDE_FROM_ALL ${LIBBACKTRACE_LIST})
        set_target_properties(Libbacktrace PROPERTIES
            INCLUDE_DIRECTORIES "${LIBBACKTRACE_INCLUDE_DIR}"
            POSITION_INDEPENDENT_CODE 1
            FOLDER "Core"
        )
        target_link_libraries(Libbacktrace ${OS_LIBS} ${SWIFTSHADER_LIBS})
        target_link_libraries(${Reactor} Libbacktrace)
    endif()

    # Download boost and use it
    DownloadBoost()
    set(BOOST_INCLUDEDIR ${BOOST_THIRD_PARTY_DIR})
    find_package(Boost REQUIRED)
    target_link_libraries(${Reactor} Boost::boost)
endif(REACTOR_EMIT_DEBUG_INFO)

add_library(GLCompiler STATIC EXCLUDE_FROM_ALL ${OPENGL_COMPILER_LIST})
set_target_properties(GLCompiler PROPERTIES
    INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
    LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
)
target_link_libraries(GLCompiler ${OS_LIBS} ${SWIFTSHADER_LIBS})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PREFIX "lib64")
else()
    set(LIB_PREFIX "lib")
endif()

if(SWIFTSHADER_BUILD_EGL)
    add_library(libEGL SHARED ${EGL_LIST})
    set_target_properties(libEGL PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "EGL_EGLEXT_PROTOTYPES; EGLAPI=; NO_SANITIZE_FUNCTION=;$<$<CONFIG:Debug>:DEBUGGER_WAIT_DIALOG>"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
        PREFIX ""
    )
    if(LINUX)
        if(X11)
            set_property(TARGET libEGL APPEND PROPERTY COMPILE_DEFINITIONS "USE_X11=1")
        endif()
    endif()
    if (ANDROID)
        set_target_properties(libEGL PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libEGL ${SOURCE_DIR}/OpenGL/libEGL)
    target_link_libraries(libEGL ${OS_LIBS} ${SWIFTSHADER_LIBS})
    add_custom_command(
        TARGET libEGL
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libEGL>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL> $<TARGET_FILE_DIR:libEGL>/translator/${LIB_PREFIX}EGL_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )
endif()

if(SWIFTSHADER_BUILD_GLESv2)
    add_library(libGLESv2 SHARED ${GLES2_LIST})
    set_target_properties(libGLESv2 PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES; GL_API=; GL_APICALL=; GLAPI=; NO_SANITIZE_FUNCTION=;"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
        PREFIX ""
    )
    if (ANDROID)
        set_target_properties(libGLESv2 PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libGLESv2 ${SOURCE_DIR}/OpenGL/libGLESv2)
    target_link_libraries(libGLESv2 SwiftShader ${Reactor} GLCompiler ${OS_LIBS} ${SWIFTSHADER_LIBS})
    add_custom_command(
        TARGET libGLESv2
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLESv2>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> $<TARGET_FILE_DIR:libGLESv2>/translator/${LIB_PREFIX}GLES_V2_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    )
endif()

if(SWIFTSHADER_BUILD_GLES_CM)
    add_library(libGLES_CM SHARED ${GLES_CM_LIST})
    set_target_properties(libGLES_CM PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES; EGLAPI=; GL_API=; GL_APICALL=; GLAPI=;"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
        PREFIX ""
    )
    if (ANDROID)
        set_target_properties(libGLES_CM PROPERTIES SUFFIX "_swiftshader.so")
    endif ()
    set_shared_library_export_map(libGLES_CM ${SOURCE_DIR}/OpenGL/libGLES_CM)
    target_link_libraries(libGLES_CM SwiftShader ${Reactor} GLCompiler ${OS_LIBS} ${SWIFTSHADER_LIBS})
    add_custom_command(
        TARGET libGLES_CM
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLES_CM>/translator
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLES_CM> $<TARGET_FILE_DIR:libGLES_CM>/translator/${LIB_PREFIX}GLES_CM_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
endif(SWIFTSHADER_BUILD_GLES_CM)

if(SWIFTSHADER_BUILD_VULKAN)
    add_library(vk_swiftshader SHARED ${VULKAN_LIST})

    if (NOT TARGET SPIRV-Tools)
        # This variable is also used by SPIRV-Tools to locate SPIRV-Headers
        set(SPIRV-Headers_SOURCE_DIR "${THIRD_PARTY_DIR}/SPIRV-Headers")
        list(APPEND VULKAN_INCLUDE_DIR "${SPIRV-Headers_SOURCE_DIR}/include")

        add_subdirectory(third_party/SPIRV-Tools)

        set_target_properties(core_tables PROPERTIES FOLDER "SPIRV-Tools build")
        set_target_properties(enum_string_mapping PROPERTIES FOLDER "SPIRV-Tools build")
        set_target_properties(extinst_tables PROPERTIES FOLDER "SPIRV-Tools build")
        set_target_properties(spirv-tools-pkg-config PROPERTIES FOLDER "SPIRV-Tools build")
        set_target_properties(spirv-tools-shared-pkg-config PROPERTIES FOLDER "SPIRV-Tools build")
    endif()

    # Copy the OpenCLDebugInfo100.h header that's generated by SPIRV-Tools
    # out to a separate directory that can be added to the include path.
    # Ideally, this header would just be pre-built and part of SPIRV-Headers.
    # See: https://github.com/KhronosGroup/SPIRV-Headers/issues/137
    set(SPIRV_TOOLS_EXT_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/spirv-tools-ext/include)
    add_custom_target(spirv_tools_ext_includes
        DEPENDS spirv-tools-header-OpenCLDebugInfo100
        COMMAND ${CMAKE_COMMAND} -E copy
            ${spirv-tools_BINARY_DIR}/OpenCLDebugInfo100.h
            ${SPIRV_TOOLS_EXT_INC_DIR}/spirv-tools/ext/OpenCLDebugInfo100.h
    )
    set_target_properties(spirv_tools_ext_includes PROPERTIES FOLDER "SPIRV-Tools build")
    list(APPEND VULKAN_INCLUDE_DIR "${SPIRV_TOOLS_EXT_INC_DIR}")
    add_dependencies(vk_swiftshader spirv_tools_ext_includes)

    set_target_properties(vk_swiftshader PROPERTIES
        INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
        FOLDER "Vulkan"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "VK_EXPORT=;NO_SANITIZE_FUNCTION=;$<$<CONFIG:Debug>:DEBUGGER_WAIT_DIALOG>"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
    )

    if(WIN32)
        set(VULKAN_API_LIBRARY_NAME "vulkan-1.dll")
        set_property(TARGET vk_swiftshader APPEND
                     PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_WIN32_KHR")
    elseif(LINUX)
        set(VULKAN_API_LIBRARY_NAME "libvulkan.so.1")
        if(X11)
            set_property(TARGET vk_swiftshader APPEND
                        PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_XLIB_KHR")
        endif()
        if(XCB)
            set_property(TARGET vk_swiftshader APPEND
                        PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_XCB_KHR")
        endif()
    elseif(APPLE)
        set(VULKAN_API_LIBRARY_NAME "libvulkan.dylib")
        set_property(TARGET vk_swiftshader APPEND
                     PROPERTY COMPILE_DEFINITIONS "VK_USE_PLATFORM_MACOS_MVK")
    else()
        message(FATAL_ERROR "Platform does not support Vulkan yet")
    endif()

    set_shared_library_export_map(vk_swiftshader ${SOURCE_DIR}/Vulkan)

    set(VK_SWIFTSHADER_LIBS ${Reactor} marl ${OS_LIBS} SPIRV-Tools SPIRV-Tools-opt ${SWIFTSHADER_LIBS})
    if(SWIFTSHADER_ENABLE_VULKAN_DEBUGGER)
        list(APPEND VK_SWIFTSHADER_LIBS cppdap)
    endif()
    target_link_libraries(vk_swiftshader ${VK_SWIFTSHADER_LIBS})

    add_custom_command(
        TARGET vk_swiftshader
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vk_swiftshader> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vk_swiftshader> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/${VULKAN_API_LIBRARY_NAME}
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
        "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/vk_swiftshader_icd.json"
    )

    if(SWIFTSHADER_EMIT_COVERAGE)
        add_executable(turbo-cov ${TESTS_DIR}/regres/cov/turbo-cov/main.cpp)
        target_link_libraries(turbo-cov llvm-with-cov)

        # Emit a coverage-toolchain.txt file next to the vk_swiftshader_icd.json
        # file so that regres can locate the LLVM toolchain used to build the
        # .so file. With this, the correct llvm-cov and llvm-profdata tools
        # from the same toolchain can be located.
        get_filename_component(COMPILER_TOOLCHAIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        file(GENERATE
            OUTPUT "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/coverage-toolchain.txt"
            CONTENT "{\"llvm\": \"${COMPILER_TOOLCHAIN_DIR}\", \"turbo-cov\": \"$<TARGET_FILE:turbo-cov>\"}"
        )
    endif()

endif()

###########################################################
# Sample programs and tests
###########################################################

if(SWIFTSHADER_BUILD_TESTS)
    set(REACTOR_UNIT_TESTS_LIST
        ${SOURCE_DIR}/Reactor/ReactorUnitTests.cpp
        ${THIRD_PARTY_DIR}/googletest/googletest/src/gtest-all.cc
    )

    set(REACTOR_UNIT_TESTS_INCLUDE_DIR
        ${THIRD_PARTY_DIR}/googletest/googletest/include
        ${THIRD_PARTY_DIR}/googletest/googletest/
    )

    add_executable(ReactorUnitTests ${REACTOR_UNIT_TESTS_LIST})
    set_target_properties(ReactorUnitTests PROPERTIES
        INCLUDE_DIRECTORIES "${REACTOR_UNIT_TESTS_INCLUDE_DIR}"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
        FOLDER "Tests"
    )

    if(NOT WIN32 AND ${REACTOR_BACKEND} STREQUAL "Subzero")
        target_link_libraries(ReactorUnitTests ${Reactor} pthread dl)
    else()
        target_link_libraries(ReactorUnitTests ${Reactor})
    endif()

    set(GLES_UNITTESTS_LIST
        ${TESTS_DIR}/GLESUnitTests/main.cpp
        ${TESTS_DIR}/GLESUnitTests/unittests.cpp
        ${THIRD_PARTY_DIR}/googletest/googletest/src/gtest-all.cc
    )

    set(GLES_UNITTESTS_INCLUDE_DIR
        ${THIRD_PARTY_DIR}/googletest/googletest/include/
        ${THIRD_PARTY_DIR}/googletest/googlemock/include/
        ${THIRD_PARTY_DIR}/googletest/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )

    add_executable(gles-unittests ${GLES_UNITTESTS_LIST})
    set_target_properties(gles-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${GLES_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "STANDALONE"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
    )

    target_link_libraries(gles-unittests libEGL libGLESv2 ${OS_LIBS} ${SWIFTSHADER_LIBS})
    if(ANDROID)
        target_link_libraries(gles-unittests -landroid)
    endif()

    # Math unit tests
    set(MATH_UNITTESTS_LIST
        ${TESTS_DIR}/MathUnitTests/main.cpp
        ${TESTS_DIR}/MathUnitTests/unittests.cpp
        ${THIRD_PARTY_DIR}/googletest/googletest/src/gtest-all.cc
    )

    set(MATH_UNITTESTS_INCLUDE_DIR
        ${THIRD_PARTY_DIR}/googletest/googletest/include/
        ${THIRD_PARTY_DIR}/googletest/googlemock/include/
        ${THIRD_PARTY_DIR}/googletest/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
    )

    add_executable(math-unittests ${MATH_UNITTESTS_LIST})
    set_target_properties(math-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${MATH_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
    )

    if(NOT WIN32)
        target_link_libraries(math-unittests pthread ${SWIFTSHADER_LIBS})
    endif()
endif(SWIFTSHADER_BUILD_TESTS)

if(SWIFTSHADER_BUILD_BENCHMARKS)
    if (NOT TARGET benchmark::benchmark)
        set(BENCHMARK_ENABLE_TESTING FALSE CACHE BOOL FALSE FORCE)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/benchmark)
    endif()

    set(REACTOR_BENCHMARK_LIST
        ${SOURCE_DIR}/Reactor/ReactorBenchmarks.cpp
    )

    add_executable(ReactorBenchmarks ${REACTOR_BENCHMARK_LIST})
    target_link_libraries(ReactorBenchmarks benchmark::benchmark marl ${Reactor})

    set_target_properties(ReactorBenchmarks PROPERTIES
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
        FOLDER "Benchmarks"
    )
endif(SWIFTSHADER_BUILD_BENCHMARKS)

if(SWIFTSHADER_BUILD_TESTS AND SWIFTSHADER_BUILD_VULKAN)
    set(VK_UNITTESTS_LIST
        ${TESTS_DIR}/VulkanUnitTests/Device.cpp
        ${TESTS_DIR}/VulkanUnitTests/Driver.cpp
        ${TESTS_DIR}/VulkanUnitTests/main.cpp
        ${TESTS_DIR}/VulkanUnitTests/unittests.cpp
        ${THIRD_PARTY_DIR}/googletest/googletest/src/gtest-all.cc
    )

    set(VK_UNITTESTS_INCLUDE_DIR
        ${THIRD_PARTY_DIR}/googletest/googletest/include/
        ${THIRD_PARTY_DIR}/googletest/googlemock/include/
        ${THIRD_PARTY_DIR}/googletest/googletest/
        ${THIRD_PARTY_DIR}/SPIRV-Tools/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )

    add_executable(vk-unittests ${VK_UNITTESTS_LIST})
    add_dependencies(vk-unittests vk_swiftshader)

    set_target_properties(vk-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${VK_UNITTESTS_INCLUDE_DIR}"
        FOLDER "Tests"
        COMPILE_OPTIONS "${SWIFTSHADER_COMPILE_OPTIONS};${WARNINGS_AS_ERRORS}"
        COMPILE_DEFINITIONS "STANDALONE"
        LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
    )

    target_link_libraries(vk-unittests ${OS_LIBS} SPIRV-Tools ${SWIFTSHADER_LIBS})
endif(SWIFTSHADER_BUILD_TESTS AND SWIFTSHADER_BUILD_VULKAN)

if(HAVE_PVR_SUBMODULE AND SWIFTSHADER_BUILD_PVR)
    if(UNIX AND NOT APPLE)
        set(PVR_WINDOW_SYSTEM XCB)

        # Set the RPATH of the next defined build targets to $ORIGIN,
        # allowing them to load shared libraries from the execution directory.
        set(CMAKE_BUILD_RPATH "$ORIGIN")
    endif()

    set(PVR_BUILD_EXAMPLES TRUE CACHE BOOL "Build the PowerVR SDK Examples" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/PowerVR_Examples)

    if(NOT APPLE)
        # Copy the 'loader' library to the bin/ directory
        # where the PowerVR executables are placed.
        add_custom_command(
            TARGET vk_swiftshader
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vk_swiftshader> ${CMAKE_BINARY_DIR}/bin/${VULKAN_API_LIBRARY_NAME}
        )
        add_custom_command(
            TARGET libGLESv2
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:libGLESv2>
        )
        add_custom_command(
            TARGET libEGL
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL> ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:libEGL>
        )
    endif()

    # Samples known to work well
    set(PVR_VULKAN_TARGET_GOOD
        VulkanBumpmap
        VulkanExampleUI
        VulkanGaussianBlur
        VulkanGlass
        VulkanGnomeHorde
        VulkanHelloAPI
        VulkanImageBasedLighting
        VulkanIntroducingPVRUtils
        VulkanMultiSampling
        VulkanNavigation2D
        VulkanParticleSystem
        VulkanSkinning
    )

    set(PVR_GLES_TARGET_GOOD
        OpenGLESHelloAPI
        OpenGLESIntroducingPVRUtils
        OpenGLESNavigation2D
    )

    set(PVR_VULKAN_TARGET_OTHER
        VulkanDeferredShading
        VulkanDeferredShadingPFX
        VulkanGameOfLife
        VulkanIBLMapsGenerator
        VulkanIMGTextureFilterCubic
        VulkanIntroducingPVRShell
        VulkanIntroducingPVRVk
        VulkanIntroducingUIRenderer
        VulkanMultithreading
        VulkanNavigation3D
        VulkanPostProcessing
        VulkanPVRScopeExample
        VulkanPVRScopeRemote
    )

    set(PVR_GLES_TARGET_OTHER
        OpenGLESDeferredShading
        OpenGLESGaussianBlur
        OpenGLESImageBasedLighting
        OpenGLESIMGFramebufferDownsample
        OpenGLESIMGTextureFilterCubic
        OpenGLESIntroducingPVRCamera
        OpenGLESIntroducingPVRShell
        OpenGLESIntroducingUIRenderer
        OpenGLESMultiviewVR
        OpenGLESNavigation3D
        OpenGLESOpenCLExample
        OpenGLESParticleSystem
        OpenGLESPostProcessing
        OpenGLESPVRScopeExample
        OpenGLESPVRScopeRemote
        OpenGLESSkinning
    )

    set(PVR_TARGET_OTHER
        glslang
        glslangValidator
        glslang-default-resource-limits
        OGLCompiler
        OSDependent
        OpenCLMatrixMultiplication
        pugixml
        PVRAssets
        PVRCamera
        PVRCore
        PVRPfx
        PVRShell
        PVRUtilsGles
        PVRUtilsVk
        PVRVk
        SPIRV
        spirv-remap
        SPVRemapper
        uninstall
    )

    set(PVR_VULKAN_TARGET
        ${PVR_VULKAN_TARGET_GOOD}
        ${PVR_VULKAN_TARGET_OTHER}
    )

    set(PVR_GLES_TARGET
        ${PVR_GLES_TARGET_GOOD}
        ${PVR_GLES_TARGET_OTHER}
    )

    foreach(pvr_target ${PVR_VULKAN_TARGET})
        add_dependencies(${pvr_target} vk_swiftshader)
    endforeach()

    foreach(pvr_target ${PVR_GLES_TARGET})
        add_dependencies(${pvr_target} libGLESv2)
        add_dependencies(${pvr_target} libEGL)
    endforeach()

    foreach(pvr_target ${PVR_VULKAN_TARGET_GOOD} ${PVR_GLES_TARGET_GOOD})
        set_target_properties(${pvr_target} PROPERTIES FOLDER Samples)
    endforeach()

    foreach(pvr_target ${PVR_TARGET_OTHER} ${PVR_VULKAN_TARGET_OTHER} ${PVR_GLES_TARGET_OTHER})
        set_target_properties(${pvr_target} PROPERTIES FOLDER Samples/PowerVR-Build)
    endforeach()
endif()
