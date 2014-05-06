###########################################################################

                               TransGaming Inc.

                     SwiftShader DX9 Software GPU Toolkit

                                 Build 3689

                              October 6, 2011
  
           SwiftShader is Copyright(c) 2003-2011 TransGaming Inc.
###########################################################################


Thank you for purchasing TransGaming's SwiftShader Software GPU Toolkit, 
TransGaming's high-speed pure-software 3D renderer. This release includes
support for Shader Model 3.0 level features with a DirectX front end API,
and a pre-release of an OpenGL ES 2.0 front end API, based in part on code from the ANGLE project.

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
achieve performance that surpasses integrated graphics hardware – 
a modern quad-core Core i7 CPU at 3.2 GHz running SwiftShader scores 
620 in 3DMark2006. SwiftShader achieves this unprecedented level of 
performance by dynamically compiling highly optimized code specific to 
an application's 3D rendering needs, and executing that code across all 
available CPU cores in parallel.

NOTE: Use of SwiftShader is subject to the the SwiftShader License 
agreement signed between TransGaming Inc. and your company.


System requirements
-------------------

* x86 CPU, SSE2 support required
* Basic 2D video card - no 3D card necessary!
* Microsoft Windows 98SE, Windows 2000, Windows XP, Windows Vista, or Windows 7 
* 128 MB RAM
* 10 MB free hard disk space


Installation - DirectX
----------------------

The SwiftShader DX9 Shader Model 3.0 API is packaged as a single DLL file:
 d3d9.dll

Both 32-bit and 64-bit versions of this DLL are included.


These DLL files are a drop-in replacements for the standard Microsoft D3D9 
DLL. They may be renamed and copied into a directory containing an application 
that uses Direct3D, and SwiftShader will automatically be used in place 
of the built-in OS version of Direct3D.

For applications which wish to use SwiftShader as an optional fallback
in case some D3D capability is not available in hardware, the SwiftShader 
DLLs can be renamed to swiftshader_d3d9.dll, and loaded directly with the 
Win32 LoadLibrary function as follows:

static HMODULE s_hSwiftShaderModule;

// Load SwiftShader DLL
static LPDIRECT3DCREATE9 LoadSwiftShader( void )
{
    s_hSwiftShaderModule = LoadLibrary( "swiftshader_d3d9.dll" );
    if( s_hSwiftShaderModule == NULL ) 
        return NULL;
    return (LPDIRECT3DCREATE9)GetProcAddress( s_hSwiftShaderModule, "Direct3DCreate9" );
}


Installation - OpenGL ES 2.0
----------------------------

The SwiftShader OpenGL ES 2.0 API is packaged as two DLL files and two lib files:
 libEGL.dll
 libGLESv2.dll
 libEGL.lib
 libGLESv2.lib
 
These libraries can be loaded and used within OpenGL ES 2.0 programs. For
more information on building OpenGL ES 2.0 based applications on Windows, 
please see the website of the ANGLE project, and look for "Application
Development with ANGLE":

 http://code.google.com/p/angleproject/wiki/DevSetup


License Authentication - C++ - DirectX
--------------------------------------

Your licensed version of SwiftShader requires your application to provide 
an authorization key to disable the SwiftShader Demonstration Logo Overlay.

Incorporating the license check in your C/C++ application requires a small 
change in your application's Direct3D startup sequence. For hints on 
License Authentication with C#/Managed clode see the Manager DirectX
section below.

For a C++ application the SwiftShader.h header file must first be copied 
into your application source code tree and included as follows:

#include "SwiftShader.h"

Next, the following function needs to be added to your program, and called
once the SwiftShader D3D interface has been created.  You will need to 
replace the License Key with the one provided to you by TransGaming.
Note that this License key is only valid for the single application that
has been licensed by your organization for distribution with SwiftShader.
Use of the SwiftShader DLLs with a different application requires a 
separate license key purchase.

//-----------------------------------------------------------------------------
// Name: SwiftShaderAuth()
// Desc: Authorize License Key for SwiftShader
//-----------------------------------------------------------------------------
BOOL SwiftShaderAuth(LPDIRECT3D9 lpD3D)
{
	ISwiftShaderPrivateV1 *pSwiftShader;
	lpD3D->QueryInterface(IID_SwiftShaderPrivateV1, (void**)&pSwiftShader);
	if (pSwiftShader)
	{
		pSwiftShader->RegisterLicenseKey("xxxxxxxxxxxxxxx");
		pSwiftShader->Release();
		return TRUE;
	}
	return FALSE;
}


License Authentication - C - DirectX or OpenGL ES 2.0
-----------------------------------------------------

If so desired, a C based API for license registration is also available, with
the following function signature:

	void __stdcall Register(char *licenseKey)

As with the C++ code above the SwiftShader.h header file must first be copied 
into your application source code tree and #included. The GetProcAddress 
function can be used to look up the straight-C based Register function.


Additional Configuration
------------------------

An optional configuration file can be used with SwiftShader to control
rendering features and optimizations.  This file, SwiftShader.ini, should 
be put in the same directory as the application.

Additionally, by default whenever SwiftShader is running, a simple 
configuration web server will be started locally on port 8080 of the 
system. This server can be accessed via:
  
 http://localhost:8080/swiftconfig

Changes made via the webserver will be written to the SwiftShader.ini file
in the Application's working directory.  The webserver may be disabled using
the DisableServer field in the config file.

Some of the features exposed through the SwiftShader.ini file can also 
be controlled programmatically.  If a SwiftShader.ini file is present, the 
programmatic settings will override the settings from the ini file.

Programmatic settings must be made through the ISwiftShaderPrivateV1 
interface before a D3DDevice is created.  The SwiftShader.h header includes 
the required constants as used in the example below.  This example 
demonstrates the type of setup that might be used for a 2D game engine 
with minimal texture filtering or perspective correction needs.


void SwiftShaderAdjustQuality(LPDIRECT3D9 lpD3D)
{
	ISwiftShaderPrivateV1 *pSwiftShader;
	lpD3D->QueryInterface(IID_SwiftShaderPrivateV1, (void**)&pSwiftShader);
	if (pSwiftShader)
	{
		// Force the maximum texture filtering quality to use 2-point averaging
		pSwiftShader->SetControlSetting(SWS_MAXIMUMFILTERQUALITY, SWF_AVERAGE2);

		// Force all mipmap filtering to be completely disabled
		pSwiftShader->SetControlSetting(SWS_MAXIMUMMIPMAPQUALITY, SWF_NONE);

		// Disable perspective texture correction
		pSwiftShader->SetControlSetting(SWS_PERSPECTIVEQUALITY, SWP_NONE); 
		pSwiftShader->Release();
	}
}

Programmatic settings are not currently available with the OpenGL ES 2.0 API.


Multi-core Support
------------------

SwiftShader can use multiple CPU cores to speed up software rendering.  
Using SwiftConfig or the SwiftShader.ini file, you can determine how many 
threads will be used simultaneously.  By default, SwiftShader will use as 
many cores as possible.


Support for .Net and Managed Code
---------------------------------

While it is possible to enable the use of SwiftShader with Managed DirectX,
this API has been deprecated by Microsoft. Instead of Managed DirectX or XNA, 
we suggest using the Open Source SlimDX library:

	http;//slimdx.org/

SwiftShader works seamlessly with SlimDX, and SlimDX supports both 32-bit 
and 64-bit code.


Sample Software
---------------

Running the Direct3D Sample Code found in Microsoft's DirectX SDK is a
good way to experiment with the Shader Model 3.0 support available in 
this build of SwiftShader. 

Other DirectX example code can be downloaded separately from various 
web sites. To investigate the advanced shaders supported by SwiftShader, 
we recommend downloading ATI's RenderMonkey Shader IDE and copying in the 
SwiftShader d3d9.dll file. 


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
  Copyright(c)2003-2011 TransGaming Inc.


Changes since previous version
------------------------------

- Updates to the OpenGL ES 2.0 API; D3DX HLSL compiler still required
- Various bug fixes


CONTACTS
--------

For more information regarding SwiftShader or other TransGaming 
products, please contact us as follows:

Sales and Licensing Inquiries:

  sales@transgaming.com


Technical Questions and Information:

  swiftshader@transgaming.com
