@echo on

SETLOCAL ENABLEDELAYEDEXPANSION

SET PATH=C:\python36;C:\Program Files\cmake\bin;%PATH%
set SRC=%cd%\git\SwiftShader

cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

git submodule update --init
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

SET MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild"
SET CONFIG=Debug

cd %SRC%\build
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake .. -G "Visual Studio 15 2017 Win64" -Thost=x64 "-DREACTOR_BACKEND=%REACTOR_BACKEND%" "-DREACTOR_VERIFY_LLVM_IR=1"
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

%MSBUILD% /p:Configuration=%CONFIG% SwiftShader.sln
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

REM Run the unit tests. Some must be run from project root
cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
SET SWIFTSHADER_DISABLE_DEBUGGER_WAIT_DIALOG=1

build\Debug\gles-unittests.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

IF NOT "%REACTOR_BACKEND%"=="Subzero" (
    REM Currently vulkan does not work with Subzero.
    build\Debug\vk-unittests.exe
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)