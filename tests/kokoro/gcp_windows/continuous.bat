@echo on

SET PATH=%PATH%;C:\python27

cd git\SwiftShader

git submodule update --init

SET MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild"
SET CONFIG=Debug

%MSBUILD% /p:Configuration=%CONFIG% SwiftShader.sln

SET PATH=%PATH%;T:\src\git\SwiftShader\out\Debug_x64
SET SWIFTSHADER_DISABLE_DEBUGGER_WAIT_DIALOG=1

REM Run the GLES unit tests. TODO(capn): move to different directory (build?).
bin\GLESUnitTests\x64\Debug\GLESUnitTests.exe