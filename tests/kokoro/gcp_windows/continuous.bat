@echo on

SET PATH=%PATH%;C:\python27

cd git\SwiftShader

git submodule update --init

"C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild" SwiftShader.sln
