// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Register.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#define SERIAL_PREFIX "SS"
#define CHECKSUM_KEY "ShaderCore"

// The App name ***MUST*** be in lowercase!
const char registeredApp[32] = SCRAMBLE31("chrome\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", APPNAME_SCRAMBLE);

char validationKey[32];   // Serial provided by application
char validationApp[32];   // Application name

void InitValidationApp(void)
{
	char exePath[MAX_PATH*10];
	char fileName[_MAX_FNAME];
	GetModuleFileName(NULL, exePath, sizeof(exePath));
	_splitpath(exePath, 0, 0, fileName, 0);
	_strlwr(fileName);
	memset(validationApp, '\0', sizeof(validationApp));
	strncpy(validationApp, fileName, strlen(fileName));
}

extern "C"
{
	void __stdcall Register(char *licenseKey)
	{
		InitValidationApp();
		memset(validationKey, '\0', sizeof(validationKey));
		strncpy(validationKey, licenseKey, strlen(licenseKey));
	}
}
