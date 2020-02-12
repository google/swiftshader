@echo on

SETLOCAL ENABLEDELAYEDEXPANSION

SET PATH=C:\python36;C:\Program Files\cmake\bin;%PATH%
set SRC=%cd%\git\SwiftShader

cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

git submodule update --init
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

# Lower the amount of debug info, to reduce Kokoro build times.
SET LESS_DEBUG_INFO=1

cd %SRC%\build
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake .. -G "Visual Studio 15 2017 Win64" -Thost=x64 "-DCMAKE_BUILD_TYPE=%BUILD_TYPE%" "-DREACTOR_BACKEND=%REACTOR_BACKEND%" "-DREACTOR_VERIFY_LLVM_IR=1" "-DLESS_DEBUG_INFO=%LESS_DEBUG_INFO%"
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake --build .
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

REM Run the unit tests. Some must be run from project root
cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
SET SWIFTSHADER_DISABLE_DEBUGGER_WAIT_DIALOG=1

build\Debug\ReactorUnitTests.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

build\Debug\gles-unittests.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

build\Debug\vk-unittests.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

Rem Incrementally build and run rr::Print unit tests
cd %SRC%\build
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake "-DREACTOR_ENABLE_PRINT=1" ..
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake --build . --target ReactorUnitTests
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
SET SWIFTSHADER_DISABLE_DEBUGGER_WAIT_DIALOG=1

build\Debug\ReactorUnitTests.exe --gtest_filter=ReactorUnitTests.Print*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
