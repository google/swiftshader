# Testing with ANGLE/SwiftShaderVK (SWANGLE)

## About

[ANGLE](https://chromium.googlesource.com/angle/angle/) is a driver that translates OpenGL ES to native 3D backends depending on the target system, such as Vulkan.

SwiftShader includes ANGLE as an optional submodule, and using the [CMake build](../development/build-systems.md?cl=amaiorano%2F79#cmake-open-source), is able to install and build ANGLE, allowing us to test GL applications against ANGLE over SwiftShader Vulkan, a.k.a. _SWANGLE_.

## Setup

In order to build ANGLE, you will need to install [depot_tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up). You will also need Python 2 installed.

To be useful, we will enable building the PowerVR examples along with ANGLE (this will also init the submodules if missing):

```bash
cd SwiftShader/build
cmake -DSWIFTSHADER_GET_PVR=1 -DSWIFTSHADER_BUILD_PVR=1 -DSWIFTSHADER_BUILD_ANGLE=1 ..
cmake --build . --target angle
```

Note that you don't have to explicitly build the `angle` target as it's part of the `ALL` target. Also note that the first time you build `angle`, it will run the `angle-setup` target, which can take some time.

The `angle` target invokes Chromium-development tools, such as [gclient](https://www.chromium.org/developers/how-tos/depottools/gclient) and [ninja](https://ninja-build.org/), which are part of `depot_tools`.

## Running PowerVR examples on SWANGLE

Once the `angle` target is built, you can now go to the `bin-angle` directory under the CMake binary directory (e.g. `build`), set up the environment variables, and start running tests:

```bash
cd SwiftShader/build/bin-angle
source export-swangle-env.sh
./OpenGLESBumpmap
```

On Windows, the process is very similar:

```bash
cd SwiftShader\build\bin-angle
export-swangle-env.bat
.\Debug\OpenGLESBumpmap
```

If you're using Visual Studio's CMake integration, after enabling `SWIFTSHADER_BUILD_PVR` in the CMake settings, and building the `angle` target, open cmd.exe and follow similar steps:

```bash
cd SwiftShader\out\build\x64-Debug\bin-angle
export-swangle-env.bat
OpenGLESBumpmap
```

## Running gles-unittests on SWANGLE

On Linux, we can set `LD_LIBRARY_PATH` to point at the folder containing ANGLE binaries when running `gles-unittests`:

```bash
cd SwiftShader\build
cmake --build . --target gles-unittests
source ./bin-angle/export-swangle-env.sh
LD_LIBRARY_PATH=`realpath ./bin-angle` ./gles-unittests
```

On Windows, we need to copy `gles-unittests.exe` to the folder containing the ANGLE binaries, and run from there:

```bash
cd SwiftShader\build
cmake --build . --target gles-unittests
copy Debug\gles-unittests.exe bin-angle\
cd bin-angle
export-swangle-env.bat
gles-unittests
```

## How it works

The `angle-setup` target should run only once if the `.gclient` file does not exist, and the `angle` target will build if any source file under angle/src is modfied.

The `angle` target builds libEGL and libGLESv2 into `${CMAKE_BINARY_DIR}/bin-angle`. If building PowerVR examples are enabled (`SWIFTSHADER_BUILD_PVR`), the PVR output folder, `${CMAKE_BINARY_DIR}/bin`,
gets copied to `${CMAKE_BINARY_DIR}/bin-angle` first.

Finally, a script named `export-swangle-env.bat/sh` also gets copied to `${CMAKE_BINARY_DIR}/bin-angle`, which sets environment variables so that the PowerVR examples will run on SWANGLE.
