#!/bin/bash

# This shell script is for (re)generating Visual Studio project files from CMake
# files, making them path relative so they can be checked into the repository.

# exit when any command fails
set -e

if [[ "$OSTYPE" != "msys" ]]; then
    echo This script is meant for generation of path relative Visual Studio project
    echo files from CMake. It should be run from an MSYS/MinGW bash shell, such as
    echo the one that comes with Git for Windows.
    exit 1
fi

CMAKE_GENERATOR="Visual Studio 15 2017 Win64"

CMAKE_BUILD_PATH="build/$CMAKE_GENERATOR"

if [ ! -d "$CMAKE_BUILD_PATH" ]; then
    mkdir -p "$CMAKE_BUILD_PATH"
fi
cd "$CMAKE_BUILD_PATH"

cmake -G"$CMAKE_GENERATOR" \
      -Thost=x64 \
      -DSPIRV-Headers_SOURCE_DIR="${CMAKE_HOME_DIRECTORY}/third_party/SPIRV-Headers" \
      -DCMAKE_CONFIGURATION_TYPES="Debug;Release" \
      -DSKIP_SPIRV_TOOLS_INSTALL=true \
      -DSPIRV_SKIP_EXECUTABLES=true \
      -DSPIRV_SKIP_TESTS=true \
      ../..

cd ../..

echo Making project files path relative. This might take a minute.

# Current directory with forward slashes
CD=$(pwd -W)/
# Current directory with (escaped) backslashes
CD2=$(echo $(pwd -W) | sed 's?/?\\\\?g')\\\\
# Phython executable path
PYTHON=$(where python | head --lines=1 | sed 's?\\?\\\\?g')
# CMake executable path
CMAKE=$(where cmake | head --lines=1 | sed 's?\\?\\\\?g')

find . -type f \( -name \*.vcxproj -o -name \*.vcxproj.filters -o -name \*.sln \) \
     -execdir sed --in-place --binary --expression="s?$CD?\$(SolutionDir)?g" {} \
                                      --expression="s?$CD2?\$(SolutionDir)?g" {} \
                                      --expression="s?$PYTHON?python?g" {} \
                                      --expression="s?$CMAKE?cmake?g" {} \
                                      --expression="s?MultiThreadedDebugDLL?MultiThreadedDebug?g" {} \
                                      --expression="s?MultiThreadedDLL?MultiThreaded?g" {} \;