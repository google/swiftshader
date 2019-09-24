@echo on

SETLOCAL ENABLEDELAYEDEXPANSION

SET BUILD_ROOT=%cd%
SET PATH=C:\python36;C:\Program Files\cmake\bin;%PATH%
SET SRC=%cd%\github\marl

cd %SRC%
if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!

git submodule update --init
if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!

SET MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild"
SET CONFIG=Release

mkdir %SRC%\build
cd %SRC%\build
if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!

IF /I "%BUILD_SYSTEM%"=="cmake" (
    cmake .. -G "%BUILD_GENERATOR%" "-DMARL_BUILD_TESTS=1" "-DMARL_BUILD_EXAMPLES=1" "-DMARL_WARNINGS_AS_ERRORS=1"
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    %MSBUILD% /p:Configuration=%CONFIG% Marl.sln
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    Release\marl-unittests.exe
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    Release\fractal.exe
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    Release\primes.exe > nul
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
) ELSE IF /I "%BUILD_SYSTEM%"=="bazel" (
    REM Fix up the MSYS environment.
    wget -q http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-7.3.0-2-any.pkg.tar.xz
    wget -q http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-libs-7.3.0-2-any.pkg.tar.xz
    c:\tools\msys64\usr\bin\bash --login -c "pacman -R --noconfirm catgets libcatgets"
    c:\tools\msys64\usr\bin\bash --login -c "pacman -Syu --noconfirm"
    c:\tools\msys64\usr\bin\bash --login -c "pacman -Sy --noconfirm mingw-w64-x86_64-crt-git patch"
    c:\tools\msys64\usr\bin\bash --login -c "pacman -U --noconfirm mingw-w64-x86_64-gcc*-7.3.0-2-any.pkg.tar.xz"
    set PATH=C:\tools\msys64\mingw64\bin;c:\tools\msys64\usr\bin;!PATH!
    set BAZEL_SH=C:\tools\msys64\usr\bin\bash.exe

    REM Install Bazel
    SET BAZEL_DIR=!BUILD_ROOT!\bazel
    mkdir !BAZEL_DIR!
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    wget -q https://github.com/bazelbuild/bazel/releases/download/0.29.1/bazel-0.29.1-windows-x86_64.zip
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    unzip -q bazel-0.29.1-windows-x86_64.zip -d !BAZEL_DIR!
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!

    REM Build and run
    !BAZEL_DIR!\bazel test //:tests --test_output=all
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    !BAZEL_DIR!\bazel run //examples:fractal
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
    !BAZEL_DIR!\bazel run //examples:primes > nul
    if !ERRORLEVEL! neq 0 exit !ERRORLEVEL!
) ELSE (
    echo "Unknown build system: %BUILD_SYSTEM%"
    exit /b 1
)
