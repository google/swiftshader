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

set(SHADER_SRC_FILES
    Constants.cpp
    PixelPipeline.cpp
    PixelProgram.cpp
    PixelRoutine.cpp
    PixelShader.cpp
    SamplerCore.cpp
    SetupRoutine.cpp
    Shader.cpp
    ShaderCore.cpp
    VertexPipeline.cpp
    VertexProgram.cpp
    VertexRoutine.cpp
    VertexShader.cpp
    Constants.hpp
    PixelPipeline.hpp
    PixelProgram.hpp
    PixelRoutine.hpp
    PixelShader.hpp
    SamplerCore.hpp
    SetupRoutine.hpp
    Shader.hpp
    ShaderCore.hpp
    VertexPipeline.hpp
    VertexProgram.hpp
    VertexRoutine.hpp
    VertexShader.hpp
)

add_library(gl_shader EXCLUDE_FROM_ALL
    ${SHADER_SRC_FILES}
)

set_target_properties(gl_shader PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "SwiftShader GL"
)

target_include_directories(gl_shader
    PUBLIC
        ".."
)

target_compile_definitions(gl_shader
    PRIVATE
        "NO_SANITIZE_FUNCTION="
)

target_compile_options(gl_shader
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(gl_shader
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(gl_shader
    PUBLIC
        gl_main
)
