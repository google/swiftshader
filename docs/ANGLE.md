# ANGLE

## About

[ANGLE](https://chromium.googlesource.com/angle/angle/) is a driver implementation that translates OpenGL ES to native 3D backends depending on the target system. Such backends include D3D on Windows, OpenGL Desktop, and Vulkan.

While SwiftShader initially offered a GLES frontend, this has been deprecated and replaced by a Vulkan frontend. In order to use SwiftShader for GLES, we now recommend using ANGLE on top of SwiftShader Vulkan.

## Why use ANGLE instead of SwiftShader's GLES frontend?

There are a few reasons why SwiftShader's GLES frontend has been deprecated, and that we now recommend using ANGLE:

1. GLES 3.1 support - ANGLE supports GLES 3.1, while SwiftShader supports 3.0.

2. [ANGLE's GLES 3.1](https://www.khronos.org/conformance/adopters/conformant-products/opengles#submission_907) implementation, along with [SwiftShader's Vulkan 1.1](https://www.khronos.org/conformance/adopters/conformant-products#submission_403) implementation, are both Khronos-certified conformant.

3. ANGLE's GLES validation is more complete than SwiftShader's.

4. Dropping support SwiftShader's GLES frontend allows our team to focus our efforts on implementing a solid and conformant Vulkan frontend instead.

## How to use ANGLE with SwiftShader's Vulkan frontend

In order to build ANGLE, you will need to install
[depot_tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up).
You will also need Python 2 installed. Note that ANGLE does not offer a CMake build at this time. Follow the official [dev setup](https://chromium.googlesource.com/angle/angle/+/HEAD/doc/DevSetup.md) to install all required tools, clone ANGLE and its dependencies, and build it. This will look something like:

```
cd angle
gn gen out/Release
autoninja -C out/Release
```

This will build all ANGLE targets into the `out/Release` directory, including:

* `libEGL.so`
* `libGLESv2.so`
* `libvk_swiftshader.so`

As long as your application uses ANGLE's `libEGL` and `libGLESv2`, ANGLE will take care of translating to a default 3D renderer for the current platform.

There are multiple ways to have ANGLE use SwiftShader Vulkan as its backend:

1. Set `ANGLE_DEFAULT_PLATFORM=swiftshader` environment variable. When running your GLES application, ANGLE will use `libvk_swiftshader` as its Vulkan driver.

2. Set both `ANGLE_DEFAULT_PLATFORM=vulkan` and `VK_ICD_FILENAMES=path/to/angle/out/Debug/libvk_swiftshader_icd.json` environment variables. When running your GLES application, ANGLE will use it's Vulkan backend, and the Vulkan Loader will load `libvk_swiftshader` via the Vulkan Loader.

3. Using the `EGL_ANGLE_platform_angle` extension. As described in [this section](https://chromium.googlesource.com/angle/angle/+/HEAD/doc/DevSetup.md#choosing-a-backend) of ANGLE's setup documentation, you can use the `EGL_ANGLE_platform_angle` extension to select the renderer to use at EGL initialization time. Use the `ANGLE_platform_angle_device_type_swiftshader` device type to select SwiftShader specifically (see [documentation here](https://chromium.googlesource.com/angle/angle/+/master/extensions/EGL_ANGLE_platform_angle_device_type_swiftshader.txt)).
