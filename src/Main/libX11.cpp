#include "libX11.hpp"

#include "Common/SharedLibrary.hpp"

#define Bool int

LibX11exports::LibX11exports(void *libX11, void *libXext)
{
    XOpenDisplay = (Display *(*)(char*))getProcAddress(libX11, "XOpenDisplay");
    XGetWindowAttributes = (Status (*)(Display*, Window, XWindowAttributes*))getProcAddress(libX11, "XGetWindowAttributes");
    XDefaultScreenOfDisplay = (Screen *(*)(Display*))getProcAddress(libX11, "XDefaultScreenOfDisplay");
    XWidthOfScreen = (int (*)(Screen*))getProcAddress(libX11, "XWidthOfScreen");
    XHeightOfScreen = (int (*)(Screen*))getProcAddress(libX11, "XHeightOfScreen");
    XPlanesOfScreen = (int (*)(Screen*))getProcAddress(libX11, "XPlanesOfScreen");
    XDefaultGC = (GC (*)(Display*, int))getProcAddress(libX11, "XDefaultGC");
    XDefaultDepth = (int (*)(Display*, int))getProcAddress(libX11, "XDefaultDepth");
    XMatchVisualInfo = (Status (*)(Display*, int, int, int, XVisualInfo*))getProcAddress(libX11, "XMatchVisualInfo");
    XDefaultVisual = (Visual *(*)(Display*, int screen_number))getProcAddress(libX11, "XDefaultVisual");
    XSetErrorHandler = (int (*(*)(int (*)(Display*, XErrorEvent*)))(Display*, XErrorEvent*))getProcAddress(libX11, "XSetErrorHandler");
    XSync = (int (*)(Display*, Bool))getProcAddress(libX11, "XSync");
    XCreateImage = (XImage *(*)(Display*, Visual*, unsigned int, int, int, char*, unsigned int, unsigned int, int, int))getProcAddress(libX11, "XCreateImage");
    XCloseDisplay = (int (*)(Display*))getProcAddress(libX11, "XCloseDisplay");
    XPutImage = (int (*)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int))getProcAddress(libX11, "XPutImage");

    XShmQueryExtension = (Bool (*)(Display*))getProcAddress(libXext, "XShmQueryExtension");
    XShmCreateImage = (XImage *(*)(Display*, Visual*, unsigned int, int, char*, XShmSegmentInfo*, unsigned int, unsigned int))getProcAddress(libXext, "XShmCreateImage");
    XShmAttach = (Bool (*)(Display*, XShmSegmentInfo*))getProcAddress(libXext, "XShmAttach");
    XShmDetach = (Bool (*)(Display*, XShmSegmentInfo*))getProcAddress(libXext, "XShmDetach");
    XShmPutImage = (int (*)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int, bool))getProcAddress(libXext, "XShmPutImage");
}

LibX11exports *LibX11::operator->()
{
    static void *libX11 = nullptr;
    static void *libXext = nullptr;
    static LibX11exports *libX11exports = nullptr;

    if(!libX11)
    {
        libX11 = loadLibrary("libX11.so");
        libXext = loadLibrary("libXext.so");
        libX11exports = new LibX11exports(libX11, libXext);
    }

    return libX11exports;
}

LibX11 libX11;
