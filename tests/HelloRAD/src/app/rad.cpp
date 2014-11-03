/******************************************************************************

 @File         OGLES2HelloAPI_Windows.cpp

 @Title        OpenGL ES 2.0 HelloAPI Tutorial

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform

 @Description  Basic Tutorial that shows step-by-step how to initialize OpenGL ES
               2.0, use it for drawing a triangle and terminate it.

******************************************************************************/
#include <stdio.h>
#include <windows.h>
#include <TCHAR.h>

#include <EGL/egl.h>
#include <RAD/rad.h>

/******************************************************************************
 Defines
******************************************************************************/
// Windows class name to register
#define	WINDOW_CLASS _T("PVRShellClass")

// Width and height of the window
#define WINDOW_WIDTH	500
#define WINDOW_HEIGHT	500

// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY	0

/******************************************************************************
 Global variables
******************************************************************************/

// Variable set in the message handler to finish the demo
bool	g_bDemoDone = false;

/*!****************************************************************************
 @Function		WndProc
 @Input			hWnd		Handle to the window
 @Input			message		Specifies the message
 @Input			wParam		Additional message information
 @Input			lParam		Additional message information
 @Return		LRESULT		result code to OS
 @Description	Processes messages for the main window
******************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/*
			Here we are handling 2 system messages: screen saving and monitor power.
			They are especially relevent on mobile devices.
		*/
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:					// Screensaver trying to start ?
				case SC_MONITORPOWER:				// Monitor trying to enter powersave ?
				return 0;							// Prevent this from happening
			}
			break;
		}

		// Handles the close message when a user clicks the quit icon of the window
		case WM_CLOSE:
			g_bDemoDone = true;
			PostQuitMessage(0);
			return 1;

		default:
			break;
	}

	// Calls the default window procedure for messages we did not handle
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*!****************************************************************************
 @Function		TestEGLError
 @Input			pszLocation		location in the program where the error took
								place. ie: function name
 @Return		bool			true if no EGL error was detected
 @Description	Tests for an EGL error and prints it
******************************************************************************/
bool TestEGLError(HWND hWnd, char* pszLocation)
{
	/*
		eglGetError returns the last error that has happened using egl,
		not the status of the last called function. The user has to
		check after every single egl call or at least once every frame.
	*/
	EGLint iErr = eglGetError();
	if (iErr != EGL_SUCCESS)
	{
		TCHAR pszStr[256];
		_stprintf(pszStr, _T("%s failed (%d).\n"), pszLocation, iErr);
		MessageBox(hWnd, pszStr, _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

/*!****************************************************************************
 @Function		WinMain
 @Input			hInstance		Application instance from OS
 @Input			hPrevInstance	Always NULL
 @Input			lpCmdLine		command line from OS
 @Input			nCmdShow		Specifies how the window is to be shown
 @Return		int				result code to OS
 @Description	Main function of the program
******************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR *lpCmdLine, int nCmdShow)
{
	// Windows variables
	HWND				hWnd	= 0;
	HDC					hDC		= 0;

	// EGL variables
	EGLDisplay			eglDisplay	= 0;
	EGLConfig			eglConfig	= 0;
	EGLSurface			eglSurface	= 0;
	EGLContext			eglContext	= 0;
	EGLNativeWindowType	eglWindow	= 0;

	/*
		Step 0 - Create a EGLNativeWindowType that we can use for OpenGL ES output
	*/

	// Register the windows class
	WNDCLASS sWC;
    sWC.style = CS_HREDRAW | CS_VREDRAW;
	sWC.lpfnWndProc = WndProc;
    sWC.cbClsExtra = 0;
    sWC.cbWndExtra = 0;
    sWC.hInstance = hInstance;
    sWC.hIcon = 0;
    sWC.hCursor = 0;
    sWC.lpszMenuName = 0;
	sWC.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    sWC.lpszClassName = WINDOW_CLASS;
	unsigned int nWidth = WINDOW_WIDTH;
	unsigned int nHeight = WINDOW_HEIGHT;

	ATOM registerClass = RegisterClass(&sWC);
	if (!registerClass)
	{
		MessageBox(0, _T("Failed to register the window class"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
	}

	// Create the eglWindow
	RECT	sRect;
	SetRect(&sRect, 100, 100, 100 + nWidth, 100 + nHeight);
	AdjustWindowRect(&sRect, WS_CAPTION | WS_SYSMENU, false);
	hWnd = CreateWindow( WINDOW_CLASS, _T("RAD Test App"), WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
						 sRect.left, sRect.top, sRect.right - sRect.left, sRect.bottom - sRect.top, NULL, NULL, hInstance, NULL);
	eglWindow = hWnd;

	// Get the associated device context
	hDC = GetDC(hWnd);
	if (!hDC)
	{
		MessageBox(0, _T("Failed to create the device context"), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		goto cleanup;
	}

	/*
		Step 1 - Get the default display.
		EGL uses the concept of a "display" which in most environments
		corresponds to a single physical screen. Since we usually want
		to draw to the main screen or only have a single screen to begin
		with, we let EGL pick the default display.
		Querying other displays is platform specific.
	*/
	eglDisplay = eglGetDisplay(hDC);

    if(eglDisplay == EGL_NO_DISPLAY)
         eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);
	/*
		Step 2 - Initialize EGL.
		EGL has to be initialized with the display obtained in the
		previous step. We cannot use other EGL functions except
		eglGetDisplay and eglGetError before eglInitialize has been
		called.
		If we're not interested in the EGL version number we can just
		pass NULL for the second and third parameters.
	*/
	EGLint iMajorVersion, iMinorVersion;
	if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
	{
		MessageBox(0, _T("eglInitialize() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		goto cleanup;
	}

	/*
		Step 3 - Make OpenGL ES the current API.
		EGL provides ways to set up OpenGL ES and OpenVG contexts
		(and possibly other graphics APIs in the future), so we need
		to specify the "current API".
	*/
	eglBindAPI(EGL_OPENGL_ES_API);
	if (!TestEGLError(hWnd, "eglBindAPI"))
	{
		goto cleanup;
	}

	/*
		Step 4 - Specify the required configuration attributes.
		An EGL "configuration" describes the pixel format and type of
		surfaces that can be used for drawing.
		For now we just want to use the default Windows surface,
		i.e. it will be visible on screen. The list
		has to contain key/value pairs, terminated with EGL_NONE.
	 */
	const EGLint pi32ConfigAttribs[] =
	{
		EGL_LEVEL,				0,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NATIVE_RENDERABLE,	EGL_FALSE,
		EGL_DEPTH_SIZE,			EGL_DONT_CARE,
		EGL_NONE
	};

	/*
		Step 5 - Find a config that matches all requirements.
		eglChooseConfig provides a list of all available configurations
		that meet or exceed the requirements given as the second
		argument. In most cases we just want the first config that meets
		all criteria, so we can limit the number of configs returned to 1.
	*/
	EGLint iConfigs;
	if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		MessageBox(0, _T("eglChooseConfig() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		goto cleanup;
	}

	/*
		Step 6 - Create a surface to draw to.
		Use the config picked in the previous step and the native window
		handle when available to create a window surface. A window surface
		is one that will be visible on screen inside the native display (or
		fullscreen if there is no windowing system).
		Pixmaps and pbuffers are surfaces which only exist in off-screen
		memory.
	*/
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

    if(eglSurface == EGL_NO_SURFACE)
    {
        eglGetError(); // Clear error
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
	}

	if (!TestEGLError(hWnd, "eglCreateWindowSurface"))
	{
		goto cleanup;
	}

	/*
		Step 7 - Create a context.
		EGL has to create a context for OpenGL ES. Our OpenGL ES resources
		like textures will only be valid inside this context
		(or shared contexts)
	*/
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);
	if (!TestEGLError(hWnd, "eglCreateContext"))
	{
		goto cleanup;
	}

	/*
		Step 8 - Bind the context to the current thread and use our
		window surface for drawing and reading.
		Contexts are bound to a thread. This means you don't have to
		worry about other threads and processes interfering with your
		OpenGL ES application.
		We need to specify a surface that will be the target of all
		subsequent drawing operations, and one that will be the source
		of read operations. They can be the same surface.
	*/
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!TestEGLError(hWnd, "eglMakeCurrent"))
	{
		goto cleanup;
	}

	void InitRAD();
	InitRAD();

	// Draws a triangle for 800 frames
	for(int i = 0; i < 800; ++i)
	{
		// Check if the message handler finished the demo
		if (g_bDemoDone) break;

		void TestRAD();
		TestRAD();

		/*
			Swap Buffers.
			Brings to the native display the current render surface.
		*/
		//eglSwapBuffers(eglDisplay, eglSurface);
		//if (!TestEGLError(hWnd, "eglSwapBuffers"))
		//{
		//	goto cleanup;
		//}

		// Managing the window messages
		MSG msg;
		PeekMessage(&msg, hWnd, NULL, NULL, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	
cleanup:
	void CleanRAD();
	CleanRAD();

	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);

	/*
		Step 11 - Destroy the eglWindow.
		Again, this is platform specific and delegated to a separate function.
	*/

	// Release the device context
	if (hDC) ReleaseDC(hWnd, hDC);

	// Destroy the eglWindow
	return 0;
}

/******************************************************************************
 End of file (OGLES2HelloAPI_Windows.cpp)
******************************************************************************/

