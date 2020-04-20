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

set(LIBGLES_CM_SRC_FILES
    Buffer.cpp
    Buffer.h
    Context.cpp
    Context.h
    Device.cpp
    Device.hpp
    Framebuffer.cpp
    Framebuffer.h
    IndexDataManager.cpp
    IndexDataManager.h
    libGLES_CM.cpp
    libGLES_CM.hpp
    main.cpp
    main.h
    mathutil.h
    Renderbuffer.cpp
    Renderbuffer.h
    resource.h
    ResourceManager.cpp
    ResourceManager.h
    Texture.cpp
    Texture.h
    utilities.cpp
    utilities.h
    VertexDataManager.cpp
    VertexDataManager.h
)

if(WIN32)
    list(APPEND LIBGLES_CM_SRC_FILES
        libGLES_CM.rc
    )
endif()

add_library(libGLES_CM SHARED
    ${LIBGLES_CM_SRC_FILES}
)

set_target_properties(libGLES_CM PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(libGLES_CM
    PRIVATE
        ".."
        "../.."
)

target_compile_definitions(libGLES_CM
    PRIVATE
        "GL_GLEXT_PROTOTYPES"
        "EGLAPI="
        "GL_API="
        "GL_APICALL="
        "GLAPI="
)

target_compile_options(libGLES_CM
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(libGLES_CM
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(libGLES_CM
    PRIVATE
        libGLESCommon
        gl_swiftshader_core
        ${ROOT_PROJECT_LINK_LIBRARIES}
)

if (ANDROID)
    set_target_properties(libGLES_CM PROPERTIES SUFFIX "_swiftshader.so")
endif ()

set_shared_library_export_map(libGLES_CM ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PREFIX "lib64")
else()
    set(LIB_PREFIX "lib")
endif()

add_custom_command(
    TARGET libGLES_CM
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLES_CM>/translator
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLES_CM> $<TARGET_FILE_DIR:libGLES_CM>/translator/${LIB_PREFIX}GLES_CM_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
)
