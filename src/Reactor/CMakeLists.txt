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

set(ROOT_PROJECT_COMPILE_OPTIONS
    ${SWIFTSHADER_COMPILE_OPTIONS}
    ${WARNINGS_AS_ERRORS}
)

set(REACTOR_SRC_FILES
    Debug.cpp
    Debug.hpp
    EmulatedIntrinsics.cpp
    EmulatedIntrinsics.hpp
    ExecutableMemory.cpp
    ExecutableMemory.hpp
    Nucleus.hpp
    OptimalIntrinsics.cpp
    OptimalIntrinsics.hpp
    Print.hpp
    Reactor.cpp
    Reactor.hpp
    ReactorDebugInfo.cpp
    ReactorDebugInfo.hpp
    Routine.hpp
)

set(SUBZERO_SRC_FILES
    Optimizer.cpp
    SubzeroReactor.cpp
)

set(LLVM_SRC_FILES
    CPUID.cpp
    CPUID.hpp
    LLVMJIT.cpp
    LLVMReactor.cpp
    LLVMReactor.hpp
    LLVMReactorDebugInfo.cpp
    LLVMReactorDebugInfo.hpp
)

if(REACTOR_EMIT_DEBUG_INFO)
    if(WIN32)
        # Boost stacktrace uses COM on Windows.
        # On Windows, cache COM instances in TLS for performance.
        list(APPEND REACTOR_PRIVATE_COMPILE_DEFINITIONS "BOOST_STACKTRACE_USE_WINDBG_CACHED")
    else()
        # Boost stacktrace uses libbacktrace
        list(APPEND REACTOR_PRIVATE_COMPILE_DEFINITIONS "BOOST_STACKTRACE_USE_BACKTRACE")
        list(APPEND REACTOR_PRIVATE_LINK_LIBRARIES libbacktrace)
    endif()

    list(APPEND REACTOR_PRIVATE_LINK_LIBRARIES Boost::boost)
endif(REACTOR_EMIT_DEBUG_INFO)


# SubzeroReactor library

add_library(ReactorSubzero STATIC EXCLUDE_FROM_ALL
    ${REACTOR_SRC_FILES}
    ${SUBZERO_SRC_FILES}
)

set_target_properties(ReactorSubzero PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Reactor"
)

target_include_directories(ReactorSubzero
    PUBLIC
        .
)

target_compile_definitions(ReactorSubzero
    PRIVATE
        ${REACTOR_PRIVATE_COMPILE_DEFINITIONS}
)

target_compile_options(ReactorSubzero
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_libraries(ReactorSubzero
    PRIVATE
        subzero
        marl
        ${REACTOR_PRIVATE_LINK_LIBRARIES}
)


# ReactorLLVM library

add_library(ReactorLLVM STATIC EXCLUDE_FROM_ALL
    ${REACTOR_SRC_FILES}
    ${LLVM_SRC_FILES}
)

set_target_properties(ReactorLLVM PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Reactor"
)

target_include_directories(ReactorLLVM
    PUBLIC
        .
)

target_compile_definitions(ReactorLLVM
    PRIVATE
        ${REACTOR_PRIVATE_COMPILE_DEFINITIONS}
)

target_compile_options(ReactorLLVM
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_libraries(ReactorLLVM
    PRIVATE
        llvm
        ${REACTOR_PRIVATE_LINK_LIBRARIES}
)
