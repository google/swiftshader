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

set(MAIN_SRC_FILES
    Config.cpp
    Config.hpp
    FrameBuffer.cpp
    FrameBuffer.hpp
    SwiftConfig.cpp
    SwiftConfig.hpp
)

if(WIN32)
    list(APPEND MAIN_SRC_FILES
        FrameBufferDD.cpp
        FrameBufferDD.hpp
        FrameBufferGDI.cpp
        FrameBufferGDI.hpp
        FrameBufferWin.cpp
        FrameBufferWin.hpp
    )
elseif(LINUX)
    list(APPEND MAIN_SRC_FILES
        FrameBufferX11.cpp
        FrameBufferX11.hpp
        libX11.cpp
        libX11.hpp
    )
elseif(APPLE)
    list(APPEND MAIN_SRC_FILES
        FrameBufferOSX.mm
        FrameBufferOSX.hpp
    )
elseif(ANDROID)
    list(APPEND MAIN_SRC_FILES
        FrameBufferAndroid.cpp
        FrameBufferAndroid.hpp
    )
endif()

add_library(gl_main EXCLUDE_FROM_ALL
    ${MAIN_SRC_FILES}
)

set_target_properties(gl_main PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "SwiftShader GL"
)

target_include_directories(gl_main
    PUBLIC
        ".."
    PRIVATE
        ${SWIFTSHADER_DIR}/include # For EGL
)

target_compile_definitions(gl_main
    PRIVATE
        "NO_SANITIZE_FUNCTION="
)

target_compile_options(gl_main
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(gl_main
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(gl_main
    PUBLIC
        gl_common
        ${Reactor}
)
