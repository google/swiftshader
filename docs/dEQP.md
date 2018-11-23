dEQP
====

These steps are specifically for testing SwiftShader's OpenGL ES 3.0 implementation using dEQP on Windows (Linux differences at the bottom).

Prerequisites
-------------

1. Install the latest [Python 2.X](https://www.python.org/downloads/)
2. Install [Visual Studio](https://visualstudio.microsoft.com/vs/community/)
3. Install [CMake](https://cmake.org/download/)
4. Install [Go](https://golang.org/doc/install) 32-bit (Important: must be 32 bit)
5. Install [MinGW](http://www.mingw.org/)
6. Install [Git](https://git-scm.com/download/win)
7. Install [Android Studio](https://developer.android.com/studio/index.html)
8. Run Android Studio and install Android SDK.
9. Set environment variables: Config Panel -> System and Security -> System -> Advanced system settigns -> Environment Variables
  * Add `<path to python>` to your PATH environment variable
  * Add `<path to MinGW>\bin` to your PATH environment variable
  * Add `<path to adb>` to your PATH environment variable 

    Note: abd is in the Android SDK, typically in `C:\Users\<username>\AppData\Local\Android\sdk\platform-tools`

10. Install GCC. In 'cmd', run:

    `mingw-get install gcc`

    Note: Using Cygwin GCC currently doesn't work.

11. (Optional) Install [TortoiseGit](https://tortoisegit.org/)

Getting the Code
----------------

12. Get dEQP (either in 'cmd' or by using TortoiseGit):

    `git clone https://android.googlesource.com/platform/external/deqp`

13. Get dEQP's dependencies. In your dEQP root directory, open 'cmd' and run:

    `python external\fetch_sources.py`

14. Get Cherry (either in 'cmd' or by using TortoiseGit):

    `git clone https://android.googlesource.com/platform/external/cherry`

15. Set environment variable (see point 9):

    Add new variable GOPATH='`<path to cherry>`'

Building the code
-----------------

16. Build dEQP's Visual Studio files using the CMake GUI, or, in the dEQP root dir, run:
    ```
    mkdir build 
    cd build
    cmake ..
    ```
    Note: If you have multiple versions of Visual Studio installed and you want to make sure cmake is using the correct version of Visual Studio, you can specify it by calling, for example:

    `cmake .. -G "Visual Studio 15 2017 Win64"`

    Also note: don't call 'cmake .' directly in the root directory. It will make things fails later on. If you do, simply erase the files created by CMake and follow the steps above.

17. Build dEQP:

    Open `<path to dEQP>\build\dEQP-Core-default.sln` in Visual Studio and Build Solution

    Note: Choose a 'Release' build, unless you really mean to debug dEQP

18. Generate test cases:
    ```
    mkdir <path to cherry>\data
    cd <path to dEQP>
    python scripts\build_caselists.py <path to cherry>\data
    ```

Preparing the server
--------------------

19. Edit `<path to cherry>\cherry\data.go`
* Search for `../candy-build/deqp-wgl` and replace that by `<path to deqp>/build`
* Just above, add an option to CommandLine: `--deqp-gl-context-type=egl`
* Just below, modify the BinaryPath from 'Debug' to 'Release' if you did a Release build at step 17

Testing OpenGL ES
-----------------

20. a) Assuming you already built SwiftShader, copy these two files:

    `libEGL.dll`  
    `libGLESv2.dll`

    From:

    `<path to SwiftShader>\out\Release_x64` or  
    `<path to SwiftShader>\out\Debug_x64`

    To:

    `<path to dEQP>\build\modules\gles3\Release` (Again, assuming you did a Release build at step 17)

Testing Vulkan
--------------

20. b) Assuming you already built SwiftShader, copy and rename this file:

    `<path to SwiftShader>\out\Release_x64\vk_swiftshader.dll` or  
    `<path to SwiftShader>\out\Debug_x64\vk_swiftshader.dll`

    To:

    `<path to dEQP>\build\external\vulkancts\modules\vulkan\vulkan-1.dll`

    This will cause dEQP to load SwiftShader's Vulkan implementatin directly, without going through a system-provided [loader](https://github.com/KhronosGroup/Vulkan-Loader/blob/master/loader/LoaderAndLayerInterface.md#the-loader) library or any layers.

    To use SwiftShader as an [Installable Client Driver](https://github.com/KhronosGroup/Vulkan-Loader/blob/master/loader/LoaderAndLayerInterface.md#installable-client-drivers) (ICD) instead:
    * Edit environment variables:
      * Define VK_ICD_FILENAMES to `<path to SwiftShader>\src\Vulkan\vk_swiftshader_icd.json`
    * If the location of `vk_swiftshader.dll` you're using is different than the one specified in `src\Vulkan\vk_swiftshader_icd.json`, modify it to point to the `vk_swiftshader.dll` file you want to use.

Running the tests
-----------------

21. Start the test server. Go to `<path to cherry>` and run:

    `go run server.go`

22. Open your favorite browser and navigate to `localhost:8080`

    Get Started -> Choose Device 'localhost' -> Select Tests 'dEQP-GLES3' -> Execute tests!

Mustpass sets
-------------

dEQP contains more tests than what is expected to pass by a conformant implementation (e.g. some tests are considered too strict, or assume certain undefined behavior). The [android\cts\master\gles3-master.txt](https://android.googlesource.com/platform/external/deqp/+/master/android/cts/master/gles3-master.txt) text file which can be loaded in Cherry's 'Test sets' tab to only run the latest tests expected to pass by certified Android devices.

Running dEQP on Linux
---------------------

Differences to the steps above:

1. Instead of copying the .dll files, you need to set LD_LIBRARY_PATH to point to SwiftShader's build directory.
2. Use `make` instead of Visual Studio.
3. There are no Debug/Release directories or .exe suffixes, so remove them from DeviceConfig in data.go.