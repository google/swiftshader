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

set(LIBGLESV2_SRC_FILES
    Buffer.cpp
    Buffer.h
    Context.cpp
    Context.h
    Device.cpp
    Device.hpp
    entry_points.cpp
    entry_points.h
    Fence.cpp
    Fence.h
    Framebuffer.cpp
    Framebuffer.h
    IndexDataManager.cpp
    IndexDataManager.h
    libGLESv2.cpp
    libGLESv2.hpp
    libGLESv3.cpp
    main.cpp
    main.h
    mathutil.h
    Program.cpp
    Program.h
    Query.cpp
    Query.h
    Renderbuffer.cpp
    Renderbuffer.h
    resource.h
    ResourceManager.cpp
    ResourceManager.h
    Sampler.h
    Shader.cpp
    Shader.h
    Texture.cpp
    Texture.h
    TransformFeedback.cpp
    TransformFeedback.h
    utilities.cpp
    utilities.h
    VertexArray.cpp
    VertexArray.h
    VertexDataManager.cpp
    VertexDataManager.h
)

if(WIN32)
    list(APPEND LIBGLESV2_SRC_FILES
        libGLESv2.rc
    )
endif()

add_library(libGLESv2 SHARED
    ${LIBGLESV2_SRC_FILES}
)

set_target_properties(libGLESv2 PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(libGLESv2
    PRIVATE
        ".."
        "../.."
)

target_compile_definitions(libGLESv2
    PRIVATE
        "GL_GLEXT_PROTOTYPES"
        "GL_API="
        "GL_APICALL="
        "GLAPI="
        "NO_SANITIZE_FUNCTION="
)

target_compile_options(libGLESv2
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(libGLESv2
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(libGLESv2
    PRIVATE
        libGLESCommon
        gl_swiftshader_core
        GLCompiler
        ${ROOT_PROJECT_LINK_LIBRARIES}
)

if (ANDROID)
    set_target_properties(libGLESv2 PROPERTIES SUFFIX "_swiftshader.so")
endif ()

set_shared_library_export_map(libGLESv2 ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PREFIX "lib64")
else()
    set(LIB_PREFIX "lib")
endif()

add_custom_command(
    TARGET libGLESv2
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:libGLESv2>/translator
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> $<TARGET_FILE_DIR:libGLESv2>/translator/${LIB_PREFIX}GLES_V2_translator${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/

    # Copy to ${CMAKE_BINARY_DIR}/bin so the library sits next to the PVR samples.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libGLESv2> ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:libGLESv2>
)