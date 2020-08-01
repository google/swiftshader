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

cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)

project(SwiftShader C CXX ASM)

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
elseif(FUCHSIA)
    # NOTE: Building for Fuchsia requires a Fuchsia CMake-based SDK.
    # See https://fuchsia-review.googlesource.com/c/fuchsia/+/379673
    find_package(FuchsiaLibraries)
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
if(SWIFTSHADER_BUILD_WSI_WAYLAND)
    find_library(WAYLAND wayland-client)
endif(SWIFTSHADER_BUILD_WSI_WAYLAND)
if(SWIFTSHADER_BUILD_WSI_DIRECTFB)
    find_library(DIRECTFB directfb)
    find_path(DIRECTFB_INCLUDE_DIR directfb/directfb.h)
endif(SWIFTSHADER_BUILD_WSI_DIRECTFB)

###########################################################
# Options
###########################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The type of build: Debug Release MinSizeRel RelWithDebInfo." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)
endif()

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
option_if_not_defined(SWIFTSHADER_BUILD_WSI_WAYLAND "Build the Wayland WSI support" FALSE)
option_if_not_defined(SWIFTSHADER_BUILD_WSI_DIRECTFB "Build the DirectFB WSI support" FALSE)
option_if_not_defined(SWIFTSHADER_BUILD_PVR "Build the PowerVR examples" TRUE)
option_if_not_defined(SWIFTSHADER_GET_PVR "Check out the PowerVR submodule" FALSE)
option_if_not_defined(SWIFTSHADER_BUILD_ANGLE "Build angle" FALSE)

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

set(DEFAULT_SWIFTSHADER_LLVM_VERSION "10.0")
set(SWIFTSHADER_LLVM_VERSION ${DEFAULT_SWIFTSHADER_LLVM_VERSION} CACHE STRING "LLVM version to use")
set_property(CACHE SWIFTSHADER_LLVM_VERSION PROPERTY STRINGS "10.0")

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
# Directories
###########################################################

set(SWIFTSHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_DIR ${SWIFTSHADER_DIR}/src)
set(THIRD_PARTY_DIR ${SWIFTSHADER_DIR}/third_party)
set(TESTS_DIR ${SWIFTSHADER_DIR}/tests)

###########################################################
# Initialize submodules
###########################################################

function(InitSubmodule target submodule_dir)
    if (NOT TARGET ${target})
        if(NOT EXISTS ${submodule_dir}/.git)
            message(WARNING "
        Target ${target} from submodule ${submodule_dir} missing.
        Running 'git submodule update --init' to download it:
            ")

            execute_process(COMMAND git -C ${SWIFTSHADER_DIR} submodule update --init ${submodule_dir})
        endif()
    endif()
endfunction()

if (SWIFTSHADER_BUILD_TESTS)
    InitSubmodule(gtest ${THIRD_PARTY_DIR}/googletest)
endif()

if(SWIFTSHADER_BUILD_BENCHMARKS)
    InitSubmodule(benchmark::benchmark ${THIRD_PARTY_DIR}/benchmark)
    InitSubmodule(glslang ${THIRD_PARTY_DIR}/glslang)
endif()

if(REACTOR_EMIT_DEBUG_INFO)
    InitSubmodule(libbacktrace ${THIRD_PARTY_DIR}/libbacktrace/src)
endif()

if(SWIFTSHADER_GET_PVR)
    InitSubmodule(PVRCore ${THIRD_PARTY_DIR}/PowerVR_Examples)
    set(SWIFTSHADER_GET_PVR FALSE CACHE BOOL "Check out the PowerVR submodule" FORCE)
endif()
if(EXISTS ${THIRD_PARTY_DIR}/PowerVR_Examples/.git)
    set(HAVE_PVR_SUBMODULE TRUE)
endif()

if(SWIFTSHADER_BUILD_CPPDAP)
    InitSubmodule(json ${THIRD_PARTY_DIR}/json)
    InitSubmodule(cppdap ${THIRD_PARTY_DIR}/cppdap)
endif()

if(SWIFTSHADER_BUILD_ANGLE)
    InitSubmodule(angle ${THIRD_PARTY_DIR}/angle/angle)
endif()

###########################################################
# Convenience macros
###########################################################

# Recursively calls source_group on the files of the directory
# so that Visual Studio has the files in a folder tree
macro(group_all_sources directory)
    file(GLOB files RELATIVE ${SWIFTSHADER_DIR}/${directory} ${SWIFTSHADER_DIR}/${directory}/*)
    foreach(file ${files})
        if(IS_DIRECTORY ${SWIFTSHADER_DIR}/${directory}/${file})
            group_all_sources(${directory}/${file})
        else()
            string(REPLACE "/" "\\" groupname ${directory})
            source_group(${groupname} FILES ${SWIFTSHADER_DIR}/${directory}/${file})
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
    elseif(LINUX OR FUCHSIA)
        # NOTE: The Fuchsia linker script is needed to export the vk_icdInitializeConnectToServiceCallback
        # entry point (a private implementation detail betwen the Fuchsia Vulkan loader and the ICD).
        if ((FUCHSIA) AND ("${TARGET}" STREQUAL "vk_swiftshader"))
          set(LINKER_VERSION_SCRIPT "fuchsia_vk_swiftshader.lds")
        else()
          set(LINKER_VERSION_SCRIPT "${TARGET}.lds")
        endif()

        # The version script only exports the API functions and
        # hides all the others.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--version-script=${DIR}/${LINKER_VERSION_SCRIPT}")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_DEPENDS "${DIR}/${LINKER_VERSION_SCRIPT};")

        # -Bsymbolic binds symbol references to their global definitions within
        # a shared object, thereby preventing symbol preemption.
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS "  -Wl,-Bsymbolic")

        if(ARCH STREQUAL "mipsel" OR ARCH STREQUAL "mips64el")
          # MIPS supports sysv hash-style only.
          set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--hash-style=sysv")
        elseif(LINUX)
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
        set(CMAKE_EXE_LINKER_FLAGS_${ARGV1} "${CMAKE_EXE_LINKER_FLAGS_${ARGV1}} ${FLAG}")
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

    set_linker_flag("/DEBUG:FASTLINK" DEBUG)
    set_linker_flag("/DEBUG:FASTLINK" RELWITHDEBINFO)

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
    # Explicitly enable these warnings.
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS
        "-Wall"
        "-Wreorder"
        "-Wsign-compare"
        "-Wmissing-braces"
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS
            "-Wextra"
            "-Wunreachable-code-loop-increment"
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
            "-Wno-unused-parameter"               # unused parameter 'X'
            "-Wno-deprecated-copy"                # implicit copy constructor for 'X' is deprecated because of user-declared copy assignment operator.

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

set(USE_EXCEPTIONS
    ${REACTOR_EMIT_DEBUG_INFO} # boost::stacktrace uses exceptions
)
if(NOT MSVC)
    if (${USE_EXCEPTIONS})
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-fexceptions")
    else()
        list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-fno-exceptions")
    endif()
endif()
unset(USE_EXCEPTIONS)

###########################################################
# libbacktrace and boost
###########################################################
if(REACTOR_EMIT_DEBUG_INFO)
    add_subdirectory(${THIRD_PARTY_DIR}/libbacktrace EXCLUDE_FROM_ALL)
    add_subdirectory(${THIRD_PARTY_DIR}/boost EXCLUDE_FROM_ALL)
endif()

###########################################################
# LLVM
###########################################################
add_subdirectory(${THIRD_PARTY_DIR}/llvm-${SWIFTSHADER_LLVM_VERSION} EXCLUDE_FROM_ALL)
set_target_properties(llvm PROPERTIES FOLDER "third_party")

###########################################################
# Subzero
###########################################################
add_subdirectory(${THIRD_PARTY_DIR}/llvm-subzero EXCLUDE_FROM_ALL)
add_subdirectory(${THIRD_PARTY_DIR}/subzero EXCLUDE_FROM_ALL)
set_target_properties(llvm-subzero PROPERTIES FOLDER "third_party")
set_target_properties(subzero PROPERTIES FOLDER "third_party")

###########################################################
# marl
###########################################################
if(BUILD_MARL)
    set(MARL_THIRD_PARTY_DIR ${THIRD_PARTY_DIR})
    add_subdirectory(${THIRD_PARTY_DIR}/marl)
    set_target_properties(marl PROPERTIES FOLDER "third_party")
endif()

if(MARL_THREAD_SAFETY_ANALYSIS_SUPPORTED)
    list(APPEND SWIFTSHADER_COMPILE_OPTIONS "-Wthread-safety")
endif()

###########################################################
# cppdap
###########################################################
if(SWIFTSHADER_BUILD_CPPDAP)
    set(CPPDAP_THIRD_PARTY_DIR ${THIRD_PARTY_DIR})
    add_subdirectory(${THIRD_PARTY_DIR}/cppdap)
endif()

###########################################################
# astc-encoder
###########################################################
if(SWIFTSHADER_ENABLE_ASTC)
    add_subdirectory(${THIRD_PARTY_DIR}/astc-encoder)
    set_target_properties(astc-encoder PROPERTIES FOLDER "third_party")
endif()

###########################################################
# gtest and gmock
###########################################################
if(SWIFTSHADER_BUILD_TESTS)
    # For Win32, force gtest to match our CRT (shared)
    set(gtest_force_shared_crt TRUE CACHE BOOL "" FORCE)
    add_subdirectory(${THIRD_PARTY_DIR}/googletest EXCLUDE_FROM_ALL)
    # gtest finds python, which picks python 2 first, if present.
    # We need to undo this so that SPIR-V can later find python3.
    unset(PYTHON_EXECUTABLE CACHE)
    set_target_properties(gmock PROPERTIES FOLDER "third_party")
    set_target_properties(gmock_main PROPERTIES FOLDER "third_party")
    set_target_properties(gtest PROPERTIES FOLDER "third_party")
    set_target_properties(gtest_main PROPERTIES FOLDER "third_party")
endif()

###########################################################
# File Lists
###########################################################

###########################################################
# Append OS specific files to lists
###########################################################

if(WIN32)
    set(OS_LIBS odbc32 odbccp32 WS2_32 dxguid)
elseif(LINUX)
    set(OS_LIBS dl pthread)
    if(SWIFTSHADER_BUILD_WSI_WAYLAND)
        list(APPEND OS_LIBS "${WAYLAND}")
    endif(SWIFTSHADER_BUILD_WSI_WAYLAND)
    if(SWIFTSHADER_BUILD_WSI_DIRECTFB)
        list(APPEND OS_LIBS "${DIRECTFB}")
        include_directories(${DIRECTFB_INCLUDE_DIR}/directfb)
    endif(SWIFTSHADER_BUILD_WSI_DIRECTFB)
elseif(FUCHSIA)
    set(OS_LIBS zircon)
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

add_subdirectory(src/Reactor) # Add ReactorSubzero and ReactorLLVM targets

if(${REACTOR_BACKEND} STREQUAL "LLVM")
    set(Reactor ReactorLLVM)
elseif(${REACTOR_BACKEND} STREQUAL "Subzero")
    set(Reactor ReactorSubzero)
else()
    message(FATAL_ERROR "REACTOR_BACKEND must be 'LLVM' or 'Subzero'")
endif()

add_subdirectory(src/Common) # Add gl_common target
add_subdirectory(src/Main) # Add gl_main target
add_subdirectory(src/Shader) # Add gl_shader target
add_subdirectory(src/Renderer) # Add gl_renderer target

# Combine all gl_* targets into an interface target
# along with other dependencies
add_library(gl_swiftshader_core INTERFACE)
target_link_libraries(gl_swiftshader_core INTERFACE
    # Transitively depends on shader, main, core, Reactor,
    # OS_LIBS and SWIFTSHADER_LIBS
    gl_renderer
)

add_subdirectory(src/OpenGL/common EXCLUDE_FROM_ALL) # Add libGLESCommon target
add_subdirectory(src/OpenGL/compiler EXCLUDE_FROM_ALL) # Add GLCompiler target

if(SWIFTSHADER_BUILD_EGL)
    add_subdirectory(src/OpenGL/libEGL) # Add libEGL target
endif()

if(SWIFTSHADER_BUILD_GLESv2)
    add_subdirectory(src/OpenGL/libGLESv2) # Add libGLESv2 target
endif()

if(SWIFTSHADER_BUILD_GLES_CM)
    add_subdirectory(src/OpenGL/libGLES_CM) # Add libGLES_CM target
endif(SWIFTSHADER_BUILD_GLES_CM)

if(SWIFTSHADER_BUILD_VULKAN)
    if (NOT TARGET SPIRV-Tools)
        # This variable is also used by SPIRV-Tools to locate SPIRV-Headers
        set(SPIRV-Headers_SOURCE_DIR "${THIRD_PARTY_DIR}/SPIRV-Headers")
        set(SPIRV_SKIP_TESTS TRUE CACHE BOOL "" FORCE)
        add_subdirectory(${THIRD_PARTY_DIR}/SPIRV-Tools) # Add SPIRV-Tools target
    endif()

    # Add a vk_base interface library for shared vulkan build options.
    # TODO: Create src/Base and make this a lib target, and move stuff from
    # src/Vulkan into it that is needed by vk_pipeline, vk_device, and vk_wsi.
    add_library(vk_base INTERFACE)

    if(SWIFTSHADER_ENABLE_VULKAN_DEBUGGER)
        target_compile_definitions(vk_base INTERFACE "ENABLE_VK_DEBUGGER")
    endif()

    if(WIN32)
        target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_WIN32_KHR")
    elseif(LINUX)
        if(X11)
            target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_XLIB_KHR")
        endif()
        if(XCB)
            target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_XCB_KHR")
        endif()
        if(SWIFTSHADER_BUILD_WSI_WAYLAND)
            if(WAYLAND)
                target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_WAYLAND_KHR")
            endif()
        endif(SWIFTSHADER_BUILD_WSI_WAYLAND)
        if(SWIFTSHADER_BUILD_WSI_DIRECTFB)
            if(DIRECTFB AND DIRECTFB_INCLUDE_DIR)
                target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_DIRECTFB_EXT")
            endif()
        endif(SWIFTSHADER_BUILD_WSI_DIRECTFB)
    elseif(APPLE)
        target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_MACOS_MVK")
    elseif(FUCHSIA)
        target_compile_definitions(vk_base INTERFACE "VK_USE_PLATFORM_FUCHSIA")
    else()
        message(FATAL_ERROR "Platform does not support Vulkan yet")
    endif()

    add_subdirectory(src/System) # Add vk_system target
    add_subdirectory(src/Pipeline) # Add vk_pipeline target
    add_subdirectory(src/WSI) # Add vk_wsi target
    add_subdirectory(src/Device) # Add vk_device target
    add_subdirectory(src/Vulkan) # Add vk_swiftshader target

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND # turbo-cov is only useful for clang coverage info
       SWIFTSHADER_EMIT_COVERAGE)
        add_subdirectory(${TESTS_DIR}/regres/cov/turbo-cov)
    endif()

endif()

if(SWIFTSHADER_BUILD_ANGLE)
    add_subdirectory(${THIRD_PARTY_DIR}/angle)

    # Make angle depend on vk_swiftshader so we can test SWANGLE
    if (TARGET vk_swiftshader)
        add_dependencies(angle vk_swiftshader)
    endif()
endif()


###########################################################
# Sample programs and tests
###########################################################

# TODO(b/161976310): Add support for building PowerVR on MacOS
if(APPLE AND HAVE_PVR_SUBMODULE AND SWIFTSHADER_BUILD_PVR)
    message(WARNING "Building PowerVR examples for SwiftShader is not yet supported on Apple platforms.")
    set(SWIFTSHADER_BUILD_PVR FALSE)
endif()

if(HAVE_PVR_SUBMODULE AND SWIFTSHADER_BUILD_PVR)
    if(UNIX AND NOT APPLE)
        set(PVR_WINDOW_SYSTEM XCB)

        # Set the RPATH of the next defined build targets to $ORIGIN,
        # allowing them to load shared libraries from the execution directory.
        set(CMAKE_BUILD_RPATH "$ORIGIN")
    endif()

    set(PVR_BUILD_EXAMPLES TRUE CACHE BOOL "Build the PowerVR SDK Examples" FORCE)
    add_subdirectory(${THIRD_PARTY_DIR}/PowerVR_Examples)

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

    # Make angle target depend on PowerVR GL examples
    if (TARGET angle)
        foreach(pvr_target ${PVR_GLES_TARGET})
            add_dependencies(angle ${pvr_target})
        endforeach()
    endif()
endif()

if(SWIFTSHADER_BUILD_TESTS)
    add_subdirectory(${TESTS_DIR}/ReactorUnitTests) # Add ReactorUnitTests target
    add_subdirectory(${TESTS_DIR}/GLESUnitTests) # Add gles-unittests target
    add_subdirectory(${TESTS_DIR}/MathUnitTests) # Add math-unittests target
    add_subdirectory(${TESTS_DIR}/SystemUnitTests) # Add system-unittests target
endif()

if(SWIFTSHADER_BUILD_BENCHMARKS)
    if (NOT TARGET benchmark::benchmark)
        set(BENCHMARK_ENABLE_TESTING FALSE CACHE BOOL FALSE FORCE)
        add_subdirectory(${THIRD_PARTY_DIR}/benchmark)
        set_target_properties(benchmark PROPERTIES FOLDER "third_party")
        set_target_properties(benchmark_main PROPERTIES FOLDER "third_party")
    endif()

    if (NOT TARGET glslang)
        add_subdirectory(${THIRD_PARTY_DIR}/glslang)
    endif()

    add_subdirectory(${TESTS_DIR}/ReactorBenchmarks) # Add ReactorBenchmarks target
    add_subdirectory(${TESTS_DIR}/SystemBenchmarks) # Add system-benchmarks target
    add_subdirectory(${TESTS_DIR}/VulkanBenchmarks) # Add VulkanBenchmarks target
endif()

if(SWIFTSHADER_BUILD_TESTS AND SWIFTSHADER_BUILD_VULKAN)
    add_subdirectory(${TESTS_DIR}/VulkanUnitTests) # Add VulkanUnitTests target
endif()
