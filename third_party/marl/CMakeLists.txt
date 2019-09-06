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
option(MARL_BUILD_EXAMPLES "Build example applications" OFF)
option(MARL_BUILD_TESTS "Build tests" ON)
option(MARL_ASAN "Build marl with address sanitizer" OFF)
option(MARL_TSAN "Build marl with thread sanitizer" OFF)

if (MARL_ASAN AND MARL_TSAN)
    message(FATAL_ERROR "MARL_ASAN and MARL_TSAN are mutually exclusive")
endif (MARL_ASAN AND MARL_TSAN)

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
file(GLOB MARL_FULL_LIST
    ${MARL_SRC_DIR}/*.cpp
    ${MARL_SRC_DIR}/*.h
    ${MARL_SRC_DIR}/*.c
)

if (NOT MSVC)
    file(GLOB MARL_ASSEMBLY_LIST ${MARL_SRC_DIR}/*.S)
    list(APPEND MARL_FULL_LIST ${MARL_ASSEMBLY_LIST})
endif(NOT MSVC)

set(MARL_LIST ${MARL_FULL_LIST})
set(MARL_TEST_LIST ${MARL_FULL_LIST})
list(FILTER MARL_LIST EXCLUDE REGEX ".*_test\\..*")
list(FILTER MARL_TEST_LIST INCLUDE REGEX ".*_test\\..*")

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
# Targets
###########################################################

# marl
add_library(marl STATIC ${MARL_LIST})
set_target_properties(marl PROPERTIES
    INCLUDE_DIRECTORIES "${MARL_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
)

if (MARL_ASAN)
    target_compile_options(marl PUBLIC "-fsanitize=address")
    target_link_libraries(marl "-fsanitize=address")
elseif (MARL_MSAN)
    target_compile_options(marl PUBLIC "-fsanitize=memory")
    target_link_libraries(marl "-fsanitize=memory")
endif ()

target_link_libraries(marl "${MARL_OS_LIBS}")

# tests
if(MARL_BUILD_TESTS)
    file(GLOB MARL_TEST_LIST
        ${MARL_SRC_DIR}/*_test.cpp
        ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    )

    set(MARL_TEST_INCLUDE_DIR
        ${GOOGLETEST_DIR}/googletest/include/
        ${GOOGLETEST_DIR}/googlemock/include/
        ${GOOGLETEST_DIR}/googletest/
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    add_executable(marl-unittests ${MARL_TEST_LIST})

    set_target_properties(marl-unittests PROPERTIES
        INCLUDE_DIRECTORIES "${MARL_TEST_INCLUDE_DIR}"
        FOLDER "Tests"
    )
    target_link_libraries(marl-unittests marl "${MARL_OS_LIBS}")
endif(MARL_BUILD_TESTS)

# examples
if(MARL_BUILD_EXAMPLES)
    function(BUILD_EXAMPLE name)
        add_executable(${name} "${CMAKE_CURRENT_SOURCE_DIR}/examples/${name}.cpp")
        set_target_properties(${name} PROPERTIES
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
            FOLDER "Examples"
        )
        target_link_libraries(${name} marl "${MARL_OS_LIBS}")
    endfunction(BUILD_EXAMPLE)

    BUILD_EXAMPLE(fractal)

endif(MARL_BUILD_EXAMPLES)
