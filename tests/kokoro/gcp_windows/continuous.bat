@echo on

SETLOCAL ENABLEDELAYEDEXPANSION

SET PATH=C:\python36;C:\Program Files\cmake\bin;%PATH%
SET SRC=%cd%\git\SwiftShader

cd %SRC% || goto :error

REM Lower the amount of debug info, to reduce Kokoro build times.
SET LESS_DEBUG_INFO=1

cd %SRC%\build || goto :error

REM The currently used OS image comes with CMake 3.17.3. If a newer version is
REM required one can update the image (go/radial/kokoro_windows_image), or
REM uncomment the line below.
REM choco upgrade cmake -y --limit-output --no-progress
cmake --version

cmake .. ^
    -G "%CMAKE_GENERATOR_TYPE%" ^
    -Thost=x64 ^
    "-DREACTOR_BACKEND=%REACTOR_BACKEND%" ^
    "-DSWIFTSHADER_LLVM_VERSION=%LLVM_VERSION%" ^
    "-DREACTOR_VERIFY_LLVM_IR=1" ^
    "-DLESS_DEBUG_INFO=%LESS_DEBUG_INFO%" || goto :error

cmake --build . --config %BUILD_TYPE%   || goto :error

REM Run the unit tests. Some must be run from project root
cd %SRC% || goto :error
SET SWIFTSHADER_DISABLE_DEBUGGER_WAIT_DIALOG=1

build\%BUILD_TYPE%\ReactorUnitTests.exe || goto :error
build\%BUILD_TYPE%\system-unittests.exe || goto :error
build\%BUILD_TYPE%\vk-unittests.exe || goto :error

REM Incrementally build and run rr::Print unit tests
cd %SRC%\build || goto :error
cmake "-DREACTOR_ENABLE_PRINT=1" .. || goto :error
cmake --build . --config %BUILD_TYPE% --target ReactorUnitTests || goto :error
%BUILD_TYPE%\ReactorUnitTests.exe --gtest_filter=ReactorUnitTests.Print* || goto :error
cmake "-DREACTOR_ENABLE_PRINT=0" .. || goto :error

REM Incrementally build with REACTOR_EMIT_ASM_FILE and run unit test
cd %SRC%\build || goto :error
cmake "-DREACTOR_EMIT_ASM_FILE=1" .. || goto :error
cmake --build . --config %BUILD_TYPE% --target ReactorUnitTests || goto :error
%BUILD_TYPE%\ReactorUnitTests.exe --gtest_filter=ReactorUnitTests.EmitAsm || goto :error
cmake "-DREACTOR_EMIT_ASM_FILE=0" .. || goto :error

REM Incrementally build with REACTOR_EMIT_DEBUG_INFO to ensure it builds
cd %SRC%\build || goto :error
cmake "-DREACTOR_EMIT_DEBUG_INFO=1" .. || goto :error
cmake --build . --config %BUILD_TYPE% --target ReactorUnitTests || goto :error
cmake "-DREACTOR_EMIT_DEBUG_INFO=0" .. || goto :error

REM Incrementally build with REACTOR_EMIT_PRINT_LOCATION to ensure it builds
cd %SRC%\build || goto :error
cmake "-DREACTOR_EMIT_PRINT_LOCATION=1" .. || goto :error
cmake --build . --config %BUILD_TYPE% --target ReactorUnitTests || goto :error
cmake "-DREACTOR_EMIT_PRINT_LOCATION=0" .. || goto :error

exit /b 0

:error
exit /b !ERRORLEVEL!
