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

set(OPENGL_COMPILER_SRC_FILES
    AnalyzeCallDepth.cpp
    AnalyzeCallDepth.h
    BaseTypes.h
    Common.h
    Compiler.cpp
    Compiler.h
    ConstantUnion.h
    debug.cpp
    debug.h
    Diagnostics.cpp
    Diagnostics.h
    DirectiveHandler.cpp
    DirectiveHandler.h
    ExtensionBehavior.h
    glslang_lex.cpp
    glslang_tab.cpp
    glslang_tab.h
    glslang.h
    InfoSink.cpp
    InfoSink.h
    Initialize.cpp
    Initialize.h
    InitializeGlobals.h
    InitializeParseContext.cpp
    InitializeParseContext.h
    Intermediate.cpp
    intermediate.h
    intermOut.cpp
    IntermTraverse.cpp
    localintermediate.h
    MMap.h
    osinclude.h
    OutputASM.cpp
    OutputASM.h
    parseConst.cpp
    ParseHelper.cpp
    ParseHelper.h
    PoolAlloc.cpp
    PoolAlloc.h
    Pragma.h
    preprocessor/DiagnosticsBase.cpp
    preprocessor/DiagnosticsBase.h
    preprocessor/DirectiveHandlerBase.cpp
    preprocessor/DirectiveHandlerBase.h
    preprocessor/DirectiveParser.cpp
    preprocessor/DirectiveParser.h
    preprocessor/ExpressionParser.cpp
    preprocessor/ExpressionParser.h
    preprocessor/Input.cpp
    preprocessor/Input.h
    preprocessor/length_limits.h
    preprocessor/Lexer.cpp
    preprocessor/Lexer.h
    preprocessor/Macro.cpp
    preprocessor/Macro.h
    preprocessor/MacroExpander.cpp
    preprocessor/MacroExpander.h
    preprocessor/numeric_lex.h
    preprocessor/pp_utils.h
    preprocessor/Preprocessor.cpp
    preprocessor/Preprocessor.h
    preprocessor/SourceLocation.h
    preprocessor/Token.cpp
    preprocessor/Token.h
    preprocessor/Tokenizer.cpp
    preprocessor/Tokenizer.h
    SymbolTable.cpp
    SymbolTable.h
    TranslatorASM.cpp
    TranslatorASM.h
    Types.h
    util.cpp
    util.h
    ValidateLimitations.cpp
    ValidateLimitations.h
    ValidateSwitch.cpp
    ValidateSwitch.h
)

if(WIN32)
    list(APPEND OPENGL_COMPILER_SRC_FILES
        ossource_win.cpp
    )
elseif(LINUX)
    list(APPEND OPENGL_COMPILER_SRC_FILES
        ossource_posix.cpp
    )
elseif(APPLE)
    list(APPEND OPENGL_COMPILER_SRC_FILES
        ossource_posix.cpp
    )
elseif(ANDROID)
    list(APPEND OPENGL_COMPILER_SRC_FILES
        ossource_posix.cpp
    )
endif()

add_library(GLCompiler
    ${OPENGL_COMPILER_SRC_FILES}
)

set_target_properties(GLCompiler PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
)

target_include_directories(GLCompiler
    PRIVATE
        ".."
        "${SWIFTSHADER_DIR}/include"
        "${SWIFTSHADER_DIR}/src"
)

target_compile_definitions(GLCompiler
    PRIVATE
)

target_compile_options(GLCompiler
    PRIVATE
        ${ROOT_PROJECT_COMPILE_OPTIONS}
)

target_link_options(GLCompiler
    PUBLIC
        ${SWIFTSHADER_LINK_FLAGS}
)

target_link_libraries(GLCompiler
    PRIVATE
        ${ROOT_PROJECT_LINK_LIBRARIES}
)
