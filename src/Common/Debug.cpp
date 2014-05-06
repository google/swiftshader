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

#include "Debug.hpp"

#include <stdio.h>
#include <stdarg.h>

void trace(const char *format, ...)
{
	if(false)
	{
		FILE *file = fopen("debug.txt", "a");

		if(file)
		{
			va_list vararg;
			va_start(vararg, format);
			vfprintf(file, format, vararg);
			va_end(vararg);

			fclose(file);
		}
	}
}
