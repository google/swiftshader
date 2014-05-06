// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Register.hpp"

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <stdlib.h>
#else
	#include <unistd.h>
	#include <stdio.h>
	#include <libgen.h>
	#include <string.h>
#endif

#define SERIAL_PREFIX "SS"
#define CHECKSUM_KEY "ShaderCore"

// The App name ***MUST*** be in lowercase!
const char registeredApp[32] = SCRAMBLE31("chrome\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", APPNAME_SCRAMBLE);

char validationKey[32];   // Serial provided by application
char validationApp[32];   // Application name

void InitValidationApp(void)
{
	memset(validationApp, '\0', sizeof(validationApp));

	char exePath[4096];

	#if defined(_WIN32)
		GetModuleFileName(NULL, exePath, sizeof(exePath));
		char exeName[256];
		_splitpath(exePath, 0, 0, exeName, 0);
		_strlwr(exeName);
		strncpy(validationApp, exeName, strlen(exeName));
	#else
		char linkPath[4096];
		sprintf(linkPath, "/proc/%d/exe", getpid());
		int bytes = readlink(linkPath, exePath, sizeof(exePath));
		exePath[bytes] = '\0';
		const char *exeName = basename(exePath);
		strncpy(validationApp, exeName, strlen(exeName));
	#endif
}

extern "C"
{
	void REGISTERAPI Register(char *licenseKey)
	{
		InitValidationApp();
		memset(validationKey, '\0', sizeof(validationKey));
		strncpy(validationKey, licenseKey, strlen(licenseKey));
	}
}
