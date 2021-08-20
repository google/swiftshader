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

set(LIBEGL_SRC_FILES
    Config.cpp
    Config.h
    Context.hpp
    Display.cpp
    Display.h
    libEGL.cpp
    libEGL.hpp
    main.cpp
    main.h
    OSXUtils.hpp
    resource.h
    Surface.cpp
    Surface.hpp
    Sync.hpp
    Texture.hpp
)

if(WIN32)
    list(APPEND LIBEGL_SRC_FILES
        libEGL.rc
    )
elseif(APPLE)
    list(APPEND LIBEGL_SRC_FILES
        OSXUtils.mm
        OSXUtils.hpp
    )
endif()

# This library has been deprecated and will be removed.
# See https://issuetracker.google.com/147516027 for details.
add_library(libEGL_deprecated SHARED
    ${LIBEGL_SRC_FILES}
)

set_target_properties(libEGL_deprecated PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(libEGL_deprecated
    PRIVATE
        ".."
        "../.."
)

target_compile_definitions(libEGL_deprecated
    PRIVATE
        "EGL_EGLEXT_PROTOTYPES"
        "EGLAPI="
        "NO_SANITIZE_FUNCTION="
        $<$<CONFIG:Debug>:"DEBUGGER_WAIT_DIALOG">
)

target_compile_options(libEGL_deprecated
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(libEGL_deprecated
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(libEGL_deprecated
    PRIVATE
        libGLESCommon
        gl_swiftshader_core
        ${ROOT_PROJECT_LINK_LIBRARIES}
)

if(LINUX)
    if(X11)
        set_property(TARGET libEGL_deprecated APPEND PROPERTY COMPILE_DEFINITIONS "USE_X11=1")
    endif()
endif()

if (ANDROID)
    set_target_properties(libEGL_deprecated PROPERTIES SUFFIX "_swiftshader.so")
endif ()

set_shared_library_export_map(libEGL_deprecated ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PREFIX "lib64")
else()
    set(LIB_PREFIX "lib")
endif()

add_custom_command(
    TARGET libEGL_deprecated
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libEGL_deprecated>/translator
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL_deprecated> $<TARGET_FILE_DIR:libEGL_deprecated>/translator/${LIB_PREFIX}EGL_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL_deprecated> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/

    # Copy to ${CMAKE_BINARY_DIR}/bin so the library sits next to the PVR samples.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libEGL_deprecated> ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:libEGL_deprecated>
)
