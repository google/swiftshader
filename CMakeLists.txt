cmake_minimum_required(VERSION 2.8)

project(SwiftShaders CXX)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The type of build: Debug Release RelWithDebInfo." )

option(BUILD_EGL "Build the EGL library" 1)
if(WIN32)
    option(BUILD_GL32 "Build the OpenGL 32 library" 1)
endif()
option(BUILD_GLESv2 "Build the OpenGL ES 2 library" 1)
option(BUILD_GLES_CM "Build the OpenGL ES 1.1 library" 1)

option(USE_GROUP_SOURCES "Group the source files in a folder tree for visual studio" 1)

# LLVM disallows calling cmake . from the main LLVM dir, the reason is that
# it builds header files that could overwrite the orignal ones. Here we
# want to include LLVM as a subdirectory and even though it wouldn't cause
# the problem, if cmake . is called from the main dir, the condition that
# LLVM checkes, "CMAKE_SOURCE_DIR == CMAKE_BINARY_DIR" will be true. So we
# disallow it ourselves too to. In addition if there are remining CMakeFiles
# and CMakeCache in the directory, cmake .. from a subdirectory will still
# try to build from the main directory so we instruct users to delete these
# files when they get the error.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In source builds are not allowed by LLVM, please create a build/ directory and build from there. You may have to delete the CMakeCache.txt file and CMakeFiles directory that are next to the CMakeLists.txt.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###########################################################
# Detect system
###########################################################

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX ON)
elseif(WIN32)
elseif(APPLE)
else()
    message(FATAL_ERROR "Platform is not supported")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x86_64")
else()
    set(ARCH "x86")
endif()

###########################################################
# Convenience macros
###########################################################

# Recursively calls source_group on the files of the directory
# so that Visual Studio has the files in a folder tree
macro(group_all_sources directory)
    file(GLOB files RELATIVE ${CMAKE_SOURCE_DIR}/${directory} ${CMAKE_SOURCE_DIR}/${directory}/*)
    foreach(file ${files})
        if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${directory}/${file})
            group_all_sources(${directory}/${file})
        else()
            string(REPLACE "/" "\\" groupname ${directory})
            source_group(${groupname} FILES ${CMAKE_SOURCE_DIR}/${directory}/${file})
        endif()
    endforeach()
endmacro()

# Takes target library and a directory where the export map is
# and add the linker options so that only the API symbols are
# exported.
macro(set_target_export_map TARGET DIR)
    get_target_property(LINKFLAGS ${TARGET} LINK_FLAGS)
    if(LINKFLAGS MATCHES "NOTFOUND")
        set(LINKFLAGS "")
    endif()
    if(MSVC)
        set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/DEF:\"${DIR}/${TARGET}.def\"")
    elseif(LINUX)
        # Both hash-style are needed, because we want both gold and
        # GNU ld to be able to read our libraries (maybe?).
        # The version script only exports the API functions and
        # hides all the others. Gc sections is used in combination
        # with each functions being in its section, to reduce the
        # binary size.
        set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${LINKFLAGS} -Wl,--hash-style=both,--version-script=${DIR}/exports.map,--gc-sections")
    endif()
endmacro()

if(USE_GROUP_SOURCES)
    group_all_sources(src)
endif()

###########################################################
# Directories
###########################################################

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(OPENGL_DIR ${SOURCE_DIR}/OpenGL)
set(OPENGL_COMPILER_DIR ${OPENGL_DIR}/compiler)
set(LLVM_DIR ${SOURCE_DIR}/LLVM)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

###########################################################
# Compile flags
###########################################################

macro( set_cpp_flag FLAG )
    if( ${ARGC} GREATER 1 )
        set( CMAKE_CXX_FLAGS_${ARGV1} "${CMAKE_CXX_FLAGS_${ARGV1}} ${FLAG}" )
    else()
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}" )
    endif()
endmacro()

if(MSVC)
    set_cpp_flag("/MP")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set_cpp_flag("--std=c++11")
    set_cpp_flag("-Wall")
    set_cpp_flag("-fexceptions")

    # Remove xor, and, or and friends from the list of keywords, they are used
    # by Reactor
    set_cpp_flag("-fno-operator-names")

    # LLVM headers requires these flags to be set
    set_cpp_flag("-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS")

    if(ARCH EQUAL "x86")
        set_cpp_flag("-m32")
        set_cpp_flag("-msse2")
        set_cpp_flag("-march=i686")
    endif()
    if(ARCH EQUAL "x86_64")
        set_cpp_flag("-m64")
        set_cpp_flag("-fPIC")
        set_cpp_flag("-march=core2")
    endif()

    # Use -g3 to have even more debug info
    set_cpp_flag("-g -g3" DEBUG)
    set_cpp_flag("-g -g3" RELWITHDEBINFO)
    set_cpp_flag("-s" RELEASE)

    # For distribution it is more important to be slim than super optimized
    # so even in Release we us only -O2
    set_cpp_flag("-O2" RELEASE)
    set_cpp_flag("-O2" RELWITHDEBINFO)

    set_cpp_flag("-DNDEBUG" RELEASE)
    set_cpp_flag("-DNDEBUG" RELWITHDEBINFO)
    set_cpp_flag("-DANGLE_DISABLE_TRACE" RELEASE)
    set_cpp_flag("-DANGLE_DISABLE_TRACE" RELWITHDEBINFO)

    # Put each variable and function in its own section so that when linking
    # with -gc-sections unused functions and variables are removed.
    set_cpp_flag("-ffunction-sections" RELEASE)
    set_cpp_flag("-fdata-sections" RELEASE)
    set_cpp_flag("-fomit-frame-pointer" RELEASE)
endif()

if( WIN32 )
    add_definitions(-DWINVER=0x501 -DNOMINMAX -DSTRICT)
    set( CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "" "lib" )
endif()


###########################################################
# LLVM
###########################################################

# We want the code for all of LLVM except the code for non-X86 targets
file(GLOB_RECURSE LLVM_LIST ${LLVM_DIR}/lib/*.cpp)
file(GLOB_RECURSE LLVM_ALL_TARGET_LIST ${LLVM_DIR}/lib/Target/*.cpp)
file(GLOB_RECURSE LLVM_X86_TARGET_LIST ${LLVM_DIR}/lib/Target/X86/*.cpp)

list(REMOVE_ITEM LLVM_LIST ${LLVM_ALL_TARGET_LIST})
list(APPEND LLVM_LIST ${LLVM_X86_TARGET_LIST})
list(REMOVE_ITEM LLVM_LIST "${LLVM_DIR}/lib/Target/X86/X86MCInstLower.cpp")
list(APPEND LLVM_LIST
    ${LLVM_DIR}/lib/Target/Mangler.cpp
    ${LLVM_DIR}/lib/Target/TargetData.cpp
    ${LLVM_DIR}/lib/Target/TargetELFWriterInfo.cpp
    ${LLVM_DIR}/lib/Target/TargetFrameLowering.cpp
    ${LLVM_DIR}/lib/Target/TargetInstrInfo.cpp
    ${LLVM_DIR}/lib/Target/TargetLibraryInfo.cpp
    ${LLVM_DIR}/lib/Target/TargetLoweringObjectFile.cpp
    ${LLVM_DIR}/lib/Target/TargetMachine.cpp
    ${LLVM_DIR}/lib/Target/TargetRegisterInfo.cpp
    ${LLVM_DIR}/lib/Target/TargetSubtargetInfo.cpp
)

if(LINUX)
    set(LLVM_INCLUDE_DIR ${LLVM_DIR}/include-linux)
elseif(APPLE)
    set(LLVM_INCLUDE_DIR ${LLVM_DIR}/include-osx)
endif()

list(APPEND LLVM_INCLUDE_DIR
    ${LLVM_DIR}/include
    ${LLVM_DIR}/lib/Target/X86
)
add_library(llvm STATIC ${LLVM_LIST})
set_target_properties(llvm PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    INCLUDE_DIRECTORIES "${LLVM_INCLUDE_DIR}"
    FOLDER "LLVM"
)

###########################################################
# Include Directories
###########################################################

set(COMMON_INCLUDE_DIR
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Common
    ${SOURCE_DIR}/Main
    ${SOURCE_DIR}/Reactor
    ${SOURCE_DIR}/Renderer
    ${SOURCE_DIR}/Shader
    ${LLVM_INCLUDE_DIR}
)
set(OPENGL_INCLUDE_DIR
    ${OPENGL_DIR}
    ${OPENGL_DIR}/include
    ${COMMON_INCLUDE_DIR}
)

###########################################################
# File Lists
###########################################################

file(GLOB_RECURSE SWIFTSHADER_LIST
    ${SOURCE_DIR}/Common/*.cpp
    ${SOURCE_DIR}/Common/*.h
    ${SOURCE_DIR}/Common/*.hpp
    ${SOURCE_DIR}/Renderer/*.cpp
    ${SOURCE_DIR}/Renderer/*.h
    ${SOURCE_DIR}/Renderer/*.hpp
    ${SOURCE_DIR}/Shader/*.cpp
    ${SOURCE_DIR}/Shader/*.h
    ${SOURCE_DIR}/Shader/*.hpp
)
list(APPEND SWIFTSHADER_LIST
    ${SOURCE_DIR}/Main/Config.cpp
    ${SOURCE_DIR}/Main/Config.hpp
    ${SOURCE_DIR}/Main/crc.cpp
    ${SOURCE_DIR}/Main/crc.h
    ${SOURCE_DIR}/Main/FrameBuffer.cpp
    ${SOURCE_DIR}/Main/FrameBuffer.hpp
    ${SOURCE_DIR}/Main/Logo.cpp
    ${SOURCE_DIR}/Main/Register.cpp
    ${SOURCE_DIR}/Main/Register.hpp
    ${SOURCE_DIR}/Main/serialvalid.cpp
    ${SOURCE_DIR}/Main/serialvalid.h
    ${SOURCE_DIR}/Main/SwiftConfig.cpp
    ${SOURCE_DIR}/Main/SwiftConfig.hpp
)
list(REMOVE_ITEM SWIFTSHADER_LIST
    ${SOURCE_DIR}/Common/DebugAndroid.cpp
    ${SOURCE_DIR}/Common/DebugAndroid.hpp
    ${SOURCE_DIR}/Common/GrallocAndroid.cpp
    ${SOURCE_DIR}/Common/GrallocAndroid.hpp
)

set(REACTOR_LIST
    ${SOURCE_DIR}/Reactor/Nucleus.cpp
    ${SOURCE_DIR}/Reactor/Nucleus.hpp
    ${SOURCE_DIR}/Reactor/Routine.cpp
    ${SOURCE_DIR}/Reactor/Routine.hpp
    ${SOURCE_DIR}/Reactor/RoutineManager.cpp
    ${SOURCE_DIR}/Reactor/RoutineManager.hpp
)

file(GLOB_RECURSE EGL_LIST
    ${OPENGL_DIR}/libEGL/*.cpp
    ${OPENGL_DIR}/libEGL/*.h
    ${OPENGL_DIR}/libEGL/*.hpp
    ${OPENGL_DIR}/common/Object.cpp
    ${OPENGL_DIR}/common/Object.hpp
    ${OPENGL_DIR}/common/debug.cpp
    ${OPENGL_DIR}/common/debug.h
    ${OPENGL_DIR}/include/*.h
)

file(GLOB_RECURSE OPENGL_COMMON_LIST
    ${OPENGL_DIR}/common/*.cpp
    ${OPENGL_DIR}/common/*.h
    ${OPENGL_DIR}/common/*.hpp
    ${OPENGL_DIR}/include/*.h
)
list(REMOVE_ITEM OPENGL_COMMON_LIST
    ${OPENGL_DIR}/common/AndroidCommon.cpp
    ${OPENGL_DIR}/common/AndroidCommon.hpp
)

file(GLOB_RECURSE GL32_LIST
    ${OPENGL_DIR}/libGL/*.cpp
    ${OPENGL_DIR}/libGL/*.h
    ${OPENGL_DIR}/libGL/*.hpp
)

file(GLOB_RECURSE GLES2_LIST
    ${OPENGL_DIR}/libGLESv2/*.cpp
    ${OPENGL_DIR}/libGLESv2/*.h
    ${OPENGL_DIR}/libGLESv2/*.hpp
)

file(GLOB_RECURSE GLES_CM_LIST
    ${OPENGL_DIR}/libGLES_CM/*.cpp
    ${OPENGL_DIR}/libGLES_CM/*.h
    ${OPENGL_DIR}/libGLES_CM/*.hpp
)

file(GLOB_RECURSE OPENGL_COMPILER_LIST
    ${OPENGL_COMPILER_DIR}/*.cpp
    ${OPENGL_COMPILER_DIR}/*.h
    ${OPENGL_COMPILER_DIR}/*.hpp
)
file(GLOB_RECURSE OPENGL_COMPILER_OS_SPECIFIC_LIST
    ${OPENGL_COMPILER_DIR}/ossource*.cpp
    ${OPENGL_COMPILER_DIR}/ossource*.h
    ${OPENGL_COMPILER_DIR}/ossource*.hpp
)
list(REMOVE_ITEM OPENGL_COMPILER_LIST ${OPENGL_COMPILER_OS_SPECIFIC_LIST})

###########################################################
# Append OS specific files to lists
###########################################################

if(WIN32)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferDD.cpp
        ${SOURCE_DIR}/Main/FrameBufferDD.hpp
        ${SOURCE_DIR}/Main/FrameBufferGDI.cpp
        ${SOURCE_DIR}/Main/FrameBufferGDI.hpp
        ${SOURCE_DIR}/Main/FrameBufferWin.cpp
        ${SOURCE_DIR}/Main/FrameBufferWin.hpp
        ${SOURCE_DIR}/Reactor/DLL.cpp
        ${SOURCE_DIR}/Reactor/DLL.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST ${OPENGL_COMPILER_DIR}/ossource_win.cpp)
    list(APPEND EGL_LIST ${OPENGL_DIR}/libEGL/libEGL.rc)
    list(APPEND GL32_LIST ${OPENGL_DIR}/libGL/libGL.rc)
    list(APPEND GLES2_LIST ${OPENGL_DIR}/libGLESv2/libGLESv2.rc)
    list(APPEND GLES_CM_LIST ${OPENGL_DIR}/libGLES_CM/libGLES_CM.rc)
elseif(LINUX)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferX11.cpp
        ${SOURCE_DIR}/Main/FrameBufferX11.hpp
        ${SOURCE_DIR}/Main/libX11.cpp
        ${SOURCE_DIR}/Main/libX11.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST
        ${OPENGL_COMPILER_DIR}/ossource_posix.cpp
    )
elseif(APPLE)
    list(APPEND SWIFTSHADER_LIST
        ${SOURCE_DIR}/Main/FrameBufferOSX.mm
        ${SOURCE_DIR}/Main/FrameBufferOSX.hpp
    )
    list(APPEND OPENGL_COMPILER_LIST
        ${OPENGL_COMPILER_DIR}/ossource_posix.cpp
    )
endif()

if(WIN32)
    set(OS_LIBS odbc32 odbccp32 WS2_32 dxguid)
elseif(LINUX)
    set(OS_LIBS dl X11 Xext pthread)
endif()

###########################################################
# SwiftShaders Targets
###########################################################

add_library(SwiftShader STATIC ${SWIFTSHADER_LIST})
set_target_properties(SwiftShader PROPERTIES
    INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Core"
)
target_link_libraries(SwiftShader ${OS_LIBS})

add_library(Reactor STATIC ${REACTOR_LIST})
set_target_properties(Reactor PROPERTIES
    INCLUDE_DIRECTORIES "${COMMON_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "Core"
)
target_link_libraries(Reactor llvm ${OS_LIBS})

add_library(GLCommon STATIC ${OPENGL_COMMON_LIST})
set_target_properties(GLCommon PROPERTIES
    INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
)
target_link_libraries(GLCommon ${OS_LIBS})

add_library(GLCompiler STATIC ${OPENGL_COMPILER_LIST})
set_target_properties(GLCompiler PROPERTIES
    INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
    POSITION_INDEPENDENT_CODE 1
    FOLDER "OpenGL"
)
target_link_libraries(GLCompiler ${OS_LIBS})

if(BUILD_EGL)
    add_library(libEGL SHARED ${EGL_LIST})
    set_target_properties(libEGL PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_DEFINITIONS "EGL_EGLEXT_PROTOTYPES"
        PREFIX ""
    )
    set_target_export_map(libEGL ${SOURCE_DIR}/OpenGL/libEGL)
    target_link_libraries(libEGL ${OS_LIBS})
endif()

if(BUILD_GL32)
    add_library(libGL SHARED ${GL32_LIST})
    set_target_properties(libGL PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
        PREFIX ""
    )
    set_target_export_map(libGL ${SOURCE_DIR}/OpenGL/libGL)
    target_link_libraries(libGL SwiftShader Reactor GLCommon GLCompiler ${OS_LIBS})
endif()

if(BUILD_GLESv2)
    add_library(libGLESv2 SHARED ${GLES2_LIST})
    set_target_properties(libGLESv2 PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
        PREFIX ""
    )
    set_target_export_map(libGLESv2 ${SOURCE_DIR}/OpenGL/libGLESv2)
    target_link_libraries(libGLESv2 SwiftShader Reactor GLCommon GLCompiler ${OS_LIBS})
endif()

if(BUILD_GLES_CM)
    add_library(libGLES_CM SHARED ${GLES_CM_LIST})
    set_target_properties(libGLES_CM PROPERTIES
        INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        FOLDER "OpenGL"
        COMPILE_DEFINITIONS "GL_GLEXT_PROTOTYPES"
        PREFIX ""
    )
    set_target_export_map(libGLES_CM ${SOURCE_DIR}/OpenGL/libGLES_CM)
    target_link_libraries(libGLES_CM SwiftShader Reactor GLCommon GLCompiler ${OS_LIBS})
endif()

###########################################################
# Extra programs
###########################################################

if (LINUX)
    add_executable(OGLES2HelloAPI tests/third_party/PowerVR/Examples/Beginner/01_HelloAPI/OGLES2/OGLES2HelloAPI_LinuxX11.cpp)
    target_link_libraries(OGLES2HelloAPI dl X11 EGL GLESv2)
endif()
