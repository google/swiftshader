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

set(ROOT_PROJECT_LINK_LIBRARIES
    ${OS_LIBS}
    ${SWIFTSHADER_LIBS}
)

set(COMMON_SRC_FILES
    Configurator.cpp
    CPUID.cpp
    Debug.cpp
    # DebugAndroid.cpp
    # GrallocAndroid.cpp
    Half.cpp
    Math.cpp
    Memory.cpp
    Resource.cpp
    SharedLibrary.cpp
    Socket.cpp
    Thread.cpp
    Timer.cpp
    Version.h
    Configurator.hpp
    CPUID.hpp
    Debug.hpp
    # DebugAndroid.hpp
    # GrallocAndroid.hpp
    Half.hpp
    Math.hpp
    Memory.hpp
    MutexLock.hpp
    RecursiveLock.hpp
    Resource.hpp
    SharedLibrary.hpp
    Socket.hpp
    Thread.hpp
    Timer.hpp
    Types.hpp
)

add_library(gl_common EXCLUDE_FROM_ALL
    ${COMMON_SRC_FILES}
)

set_target_properties(gl_common PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "SwiftShader GL"
)

target_include_directories(gl_common
    PUBLIC
        ".."
)

target_compile_definitions(gl_common
    PRIVATE
        "NO_SANITIZE_FUNCTION="
)

target_compile_options(gl_common
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(gl_common
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(gl_common
    PRIVATE
        ${ROOT_PROJECT_LINK_LIBRARIES}
)
