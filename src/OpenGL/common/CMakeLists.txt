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

set(LIBGL_COMMON_SRC_FILES
    debug.cpp
    debug.h
    Image.cpp
    Image.hpp
    MatrixStack.cpp
    MatrixStack.hpp
    NameSpace.hpp
    Object.cpp
    Object.hpp
    Surface.hpp
)

add_library(libGLESCommon
    ${LIBGL_COMMON_SRC_FILES}
)

set_target_properties(libGLESCommon PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    LINK_FLAGS "${SWIFTSHADER_LINK_FLAGS}"
    PREFIX ""
)

target_include_directories(libGLESCommon
    PUBLIC
        ".."
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src"
)

target_compile_definitions(libGLESCommon
    PRIVATE
)

target_compile_options(libGLESCommon
    PUBLIC
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_libraries(libGLESCommon
    PUBLIC
        ${ROOT_PROJECT_LINK_LIBRARIES}
)
