# Copyright 2019 The Marl Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

project(Marl C CXX ASM)

###########################################################
# Options
###########################################################
option(MARL_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(MARL_BUILD_EXAMPLES "Build example applications" OFF)
option(MARL_BUILD_TESTS "Build tests" OFF)
option(MARL_ASAN "Build marl with address sanitizer" OFF)
option(MARL_MSAN "Build marl with memory sanitizer" OFF)
option(MARL_TSAN "Build marl with thread sanitizer" OFF)
option(MARL_INSTALL "Create marl install target" OFF)

###########################################################
# Directories
###########################################################
set(MARL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MARL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(GOOGLETEST_DIR ${THIRD_PARTY_DIR}/googletest)

###########################################################
# Submodules
###########################################################
if(MARL_BUILD_TESTS)
    if(NOT EXISTS ${THIRD_PARTY_DIR}/googletest/.git)
        message(WARNING "third_party/googletest submodule missing.")
        message(WARNING "Run: `git submodule update --init` to build tests.")
        set(MARL_BUILD_TESTS OFF)
    endif()
endif(MARL_BUILD_TESTS)

###########################################################
# File lists
###########################################################
set(MARL_LIST
    ${MARL_SRC_DIR}/debug.cpp
    ${MARL_SRC_DIR}/memory.cpp
    ${MARL_SRC_DIR}/scheduler.cpp
    ${MARL_SRC_DIR}/thread.cpp
    ${MARL_SRC_DIR}/trace.cpp
)
if(NOT MSVC)
    list(APPEND MARL_LIST
        ${MARL_SRC_DIR}/osfiber_aarch64.c
        ${MARL_SRC_DIR}/osfiber_arm.c
        ${MARL_SRC_DIR}/osfiber_asm_aarch64.S
        ${MARL_SRC_DIR}/osfiber_asm_arm.S
        ${MARL_SRC_DIR}/osfiber_asm_ppc64.S
        ${MARL_SRC_DIR}/osfiber_asm_x64.S
        ${MARL_SRC_DIR}/osfiber_asm_x86.S
        ${MARL_SRC_DIR}/osfiber_ppc64.c
        ${MARL_SRC_DIR}/osfiber_x64.c
        ${MARL_SRC_DIR}/osfiber_x86.c
    )
endif(NOT MSVC)

###########################################################
# OS libraries
###########################################################
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(MARL_OS_LIBS Kernel32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(MARL_OS_LIBS pthread)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(MARL_OS_LIBS)
endif()

###########################################################
# Functions
###########################################################

function(marl_set_target_options target)
    # Enable all warnings
    if(MSVC)
        target_compile_options(${target} PRIVATE
            "-W4"
            "/wd4127" # conditional expression is constant
        )
    else()
        target_compile_options(${target} PRIVATE "-Wall")
    endif()

    # Disable specific, pedantic warnings
    if(MSVC)
        target_compile_options(${target} PRIVATE "-D_CRT_SECURE_NO_WARNINGS")
    endif()

    # Treat all warnings as errors
    if(MARL_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${target} PRIVATE "/WX")
        else()
            target_compile_options(${target} PRIVATE "-Werror")
        endif()
    endif(MARL_WARNINGS_AS_ERRORS)

    if(MARL_ASAN)
        target_compile_options(${target} PUBLIC "-fsanitize=address")
        target_link_libraries(${target} "-fsanitize=address")
    elseif(MARL_MSAN)
        target_compile_options(${target} PUBLIC "-fsanitize=memory")
        target_link_libraries(${target} "-fsanitize=memory")
    elseif(MARL_TSAN)
        target_compile_options(${target} PUBLIC "-fsanitize=thread")
        target_link_libraries(${target} "-fsanitize=thread")
    endif()

    target_include_directories(${target} PRIVATE ${MARL_INCLUDE_DIR})
endfunction(marl_set_target_options)

###########################################################
# Targets
###########################################################

# marl
add_library(marl STATIC ${MARL_LIST})
set_target_properties(marl PROPERTIES
    POSITION_INDEPENDENT_CODE 1
)

marl_set_target_options(marl)

target_link_libraries(marl "${MARL_OS_LIBS}")

# install
if(MARL_INSTALL)
    include(GNUInstallDirs)

    install(DIRECTORY ${MARL_INCLUDE_DIR}/marl
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        USE_SOURCE_PERMISSIONS
    )

    install(TARGETS marl
        EXPORT marl-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT marl-targets
        FILE marl-config.cmake
        NAMESPACE marl::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/marl
    )
endif(MARL_INSTALL)

# tests
if(MARL_BUILD_TESTS)
    set(MARL_TEST_LIST
        ${MARL_SRC_DIR}/blockingcall_test.cpp
        ${MARL_SRC_DIR}/conditionvariable_test.cpp
        ${MARL_SRC_DIR}/containers_test.cpp
        ${MARL_SRC_DIR}/defer_test.cpp
        ${MARL_SRC_DIR}/event_test.cpp
        ${MARL_SRC_DIR}/marl_test.cpp
        ${MARL_SRC_DIR}/marl_test.h
        ${MARL_SRC_DIR}/memory_test.cpp
        ${MARL_SRC_DIR}/osfiber_test.cpp
        ${MARL_SRC_DIR}/pool_test.cpp
        ${MARL_SRC_DIR}/scheduler_test.cpp
        ${MARL_SRC_DIR}/ticket_test.cpp
        ${MARL_SRC_DIR}/waitgroup_test.cpp
        ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    )

    set(MARL_TEST_INCLUDE_DIR
        ${GOOGLETEST_DIR}/googletest/include/
        ${GOOGLETEST_DIR}/googlemock/include/
        ${GOOGLETEST_DIR}/googletest/
    )

    add_executable(marl-unittests ${MARL_TEST_LIST})

    set_target_properties(marl-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${MARL_TEST_INCLUDE_DIR}"
        FOLDER "Tests"
    )

    marl_set_target_options(marl-unittests)

    target_link_libraries(marl-unittests marl "${MARL_OS_LIBS}")
endif(MARL_BUILD_TESTS)

# examples
if(MARL_BUILD_EXAMPLES)
    function(build_example target)
        add_executable(${target} "${CMAKE_CURRENT_SOURCE_DIR}/examples/${target}.cpp")
        set_target_properties(${target} PROPERTIES
            FOLDER "Examples"
        )
        marl_set_target_options(${target})
        target_link_libraries(${target} marl "${MARL_OS_LIBS}")
    endfunction(build_example)

    build_example(fractal)
    build_example(primes)

endif(MARL_BUILD_EXAMPLES)
