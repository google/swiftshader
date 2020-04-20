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

set(RENDERER_SRC_FILES
    Blitter.cpp
    Clipper.cpp
    Color.cpp
    Context.cpp
    ETC_Decoder.cpp
    Matrix.cpp
    PixelProcessor.cpp
    Plane.cpp
    Point.cpp
    QuadRasterizer.cpp
    Renderer.cpp
    Sampler.cpp
    SetupProcessor.cpp
    Surface.cpp
    TextureStage.cpp
    Vector.cpp
    VertexProcessor.cpp
    Blitter.hpp
    Clipper.hpp
    Color.hpp
    Context.hpp
    ETC_Decoder.hpp
    LRUCache.hpp
    Matrix.hpp
    PixelProcessor.hpp
    Plane.hpp
    Point.hpp
    Polygon.hpp
    Primitive.hpp
    QuadRasterizer.hpp
    Rasterizer.hpp
    Renderer.hpp
    RoutineCache.hpp
    Sampler.hpp
    SetupProcessor.hpp
    Stream.hpp
    Surface.hpp
    TextureStage.hpp
    Triangle.hpp
    Vector.hpp
    Vertex.hpp
    VertexProcessor.hpp
)

add_library(gl_renderer EXCLUDE_FROM_ALL
    ${RENDERER_SRC_FILES}
)

set_target_properties(gl_renderer PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "SwiftShader GL"
)

target_include_directories(gl_renderer
    PUBLIC
        ".."
)

target_compile_definitions(gl_renderer
    PRIVATE
        "NO_SANITIZE_FUNCTION="
)

target_compile_options(gl_renderer
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(gl_renderer
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(gl_renderer
    PUBLIC
        gl_shader
)
