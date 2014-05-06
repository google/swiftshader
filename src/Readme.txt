###########################################################################

                               TransGaming Inc.

                     SwiftShader Software GPU Toolkit

                                 Build 5139

                              Feburary 6, 2013
  
           SwiftShader is Copyright(c) 2003-2013 TransGaming Inc.
###########################################################################


Thank you for purchasing TransGaming's SwiftShader Software GPU Toolkit, 
TransGaming's high-speed pure-software 3D renderer. This release includes
support for the OpenGL ES 2.0 and EGL 1.4 APIs on Windows and Linux.

TransGaming's SwiftShader Software GPU Toolkit is the world's fastest
and most flexible general-purpose pure software 3D rendering technology.
SwiftShader's modular architecture is capable of supporting multiple 
application programming interfaces, such as DirectX(tm) 9.0, and OpenGL(tm) 
ES 2.0, the same APIs that developers are already using for existing 
games and applications. This makes it possible to directly integrate
SwiftShader into applications without any changes to source code. 
SwiftShader technology can also support custom front-end APIs that have 
been explicitly built for a specific application. 

Rendering features available in SwiftShader range from basic fixed 
function rendering through to Shader Model 3.0 level capabilities such 
as advanced shaders, floating point rendering, multi-sample anti-aliasing, 
and much more. 

SwiftShader performs as much as 100 times faster than traditional 
software renderers such as Microsoft's Direct3D(tm) Reference Rasterizer. 
In benchmark tests on a modern CPU, SwiftShader-based renderers can 
achieve performance that surpasses integrated graphics hardware -
a modern quad-core Core i7 CPU at 3.2 GHz running SwiftShader scores 
620 in 3DMark2006. SwiftShader achieves this unprecedented level of 
performance by dynamically compiling highly optimized code specific to 
an application's 3D rendering needs, and executing that code across all 
available CPU cores in parallel.

NOTE: Use of SwiftShader is subject to the the SwiftShader License 
agreement signed between TransGaming Inc. and your company.


System requirements
-------------------

* x86 CPU, SSE support required
* Basic 2D video card - no 3D card necessary!
* Operating systems:
 - Microsoft Windows 98SE, Windows 2000, Windows XP, Windows Vista, or Windows 7
 - Linux
* 128 MB RAM
* 10 MB free hard disk space


Compilation and Installation - OpenGL ES 2.0
--------------------------------------------

* Windows:
Open the SwiftShader.sln file using Visual C++ 2010. Select the "Release" solution
configuration for builds which show the SwiftShader logo on non-licensed
applications. Build all the projects (by pressing F7). After compilation has
completed, you will find libEGL.dll and libGLESv2.dll in lib\Release\.

These DLLs can be copied into the directory of an OpenGL ES 2.0 application, or
can be loaded explicitly using the Win32 LoadLibrary function.

* Linux:
Install a recent nightly of Code::Blocks and open SwiftShader.workspace. Select the
"Release" target and build the entire workspace. After compilation has completed,
you will find libEGL.so and libGLESv2.so in bin\Release\.

To use these with existing binaries, such as the PowerVR SDK samples, you can use
the 'patchelf' tool to change the RPATH to ".\". This way you can place the .so
files in the same directory and have the application use them instead of the
default libraries. 

License Authentication - C/C++
--------------------------------------

Your licensed version of SwiftShader requires your application to provide 
an authorization key to disable the SwiftShader Demonstration Logo Overlay.

Incorporating the license check in your C/C++ application requires a small 
change in your application's startup sequence. The "Register" function is
exported from the libGLESv2 library. It has the following signature:

    void Register(char *licenseKey)

Calling this function with your license key as a C string parameter will ensure
that the logo rendering is disabled.


Copyright & Logo Requirement
----------------------------

Your SwiftShader license includes a requirement that your application 
display the SwiftShader logo and include appropriate Copyright notices 
in its associated documentation.

The enclosed SwiftShader_Logo.png is a high resolution transparent 
PNG of the SwiftShader logo that you may use for this purpose.

The documentation or license file for your application must include
the following text:

  This product includes SwiftShader Software GPU Tookit,
  Copyright(c)2003-2013 TransGaming Inc.


Changes since previous version
------------------------------

Build 5139 - February 6, 2013
- 64-bit Linux compatible.
- Adopted the new shader preprocessor from ANGLE.
- Fixed declaring used samplers.
- Fixed enabling SSE3 support.
- Fixed varying/uniform count robustness.
- Fixed buffer update synchronization.
- Implemented native 8-bit ALPHA texture support.
- Implemented GL_OES_depth_texture.
- Implemented GL_OES_depth_texture_cube_map.
- Implemented GL_EXT_texture_filter_anisotropic.

Build 5086 - August 10, 2012
- Initial port to Linux.

Build 4079 - April 18, 2012
- Eliminated the use of the Microsoft HLSL compiler.
- Added support for CPUs without SSE2 extensions.
- Optimized the renderer for OpenGL operations.


CONTACTS
--------

For more information regarding SwiftShader or other TransGaming 
products, please contact us as follows:

Sales and Licensing Inquiries:

  sales@transgaming.com


Technical Questions and Information:

  swiftshader@transgaming.com
