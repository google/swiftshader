@echo on

SETLOCAL ENABLEDELAYEDEXPANSION

SET PATH=C:\python36;C:\Program Files\cmake\bin;%PATH%
set SRC=%cd%\github\marl

cd %SRC%
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

git submodule update --init
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

SET MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild"
SET CONFIG=Release

mkdir %SRC%\build
cd %SRC%\build
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

cmake .. -G "Visual Studio 15 2017 Win64" -Thost=x64 "-DMARL_BUILD_TESTS=1" "-DMARL_BUILD_EXAMPLES=1" "-DMARL_WARNINGS_AS_ERRORS=1"
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

%MSBUILD% /p:Configuration=%CONFIG% Marl.sln
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

Release\marl-unittests.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

Release\fractal.exe
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
