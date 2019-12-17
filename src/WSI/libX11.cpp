// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "libX11.hpp"

#include "System/SharedLibrary.hpp"

#define Bool int

namespace {

template<typename FPTR>
void getFuncAddress(void *lib, const char *name, FPTR *out)
{
	*out = reinterpret_cast<FPTR>(getProcAddress(lib, name));
}

}  // anonymous namespace

LibX11exports::LibX11exports(void *libX11, void *libXext)
{
	getFuncAddress(libX11, "XOpenDisplay", &XOpenDisplay);
	getFuncAddress(libX11, "XGetWindowAttributes", &XGetWindowAttributes);
	getFuncAddress(libX11, "XDefaultScreenOfDisplay", &XDefaultScreenOfDisplay);
	getFuncAddress(libX11, "XWidthOfScreen", &XWidthOfScreen);
	getFuncAddress(libX11, "XHeightOfScreen", &XHeightOfScreen);
	getFuncAddress(libX11, "XPlanesOfScreen", &XPlanesOfScreen);
	getFuncAddress(libX11, "XDefaultGC", &XDefaultGC);
	getFuncAddress(libX11, "XDefaultDepth", &XDefaultDepth);
	getFuncAddress(libX11, "XMatchVisualInfo", &XMatchVisualInfo);
	getFuncAddress(libX11, "XDefaultVisual", &XDefaultVisual);
	getFuncAddress(libX11, "XSetErrorHandler", &XSetErrorHandler);
	getFuncAddress(libX11, "XSync", &XSync);
	getFuncAddress(libX11, "XCreateImage", &XCreateImage);
	getFuncAddress(libX11, "XCloseDisplay", &XCloseDisplay);
	getFuncAddress(libX11, "XPutImage", &XPutImage);
	getFuncAddress(libX11, "XDrawString", &XDrawString);

	getFuncAddress(libXext, "XShmQueryExtension", &XShmQueryExtension);
	getFuncAddress(libXext, "XShmCreateImage", &XShmCreateImage);
	getFuncAddress(libXext, "XShmAttach", &XShmAttach);
	getFuncAddress(libXext, "XShmDetach", &XShmDetach);
	getFuncAddress(libXext, "XShmPutImage", &XShmPutImage);
}

LibX11exports *LibX11::operator->()
{
	return loadExports();
}

LibX11exports *LibX11::loadExports()
{
	static void *libX11 = nullptr;
	static void *libXext = nullptr;
	static LibX11exports *libX11exports = nullptr;

	if(!libX11)
	{
		if(getProcAddress(RTLD_DEFAULT, "XOpenDisplay"))  // Search the global scope for pre-loaded X11 library.
		{
			libX11exports = new LibX11exports(RTLD_DEFAULT, RTLD_DEFAULT);
			libX11 = (void *)-1;  // No need to load it.
		}
		else
		{
			libX11 = loadLibrary("libX11.so");

			if(libX11)
			{
				libXext = loadLibrary("libXext.so");
				libX11exports = new LibX11exports(libX11, libXext);
			}
			else
			{
				libX11 = (void *)-1;  // Don't attempt loading more than once.
			}
		}
	}

	return libX11exports;
}

LibX11 libX11;
