//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// debug.cpp: Debugging utilities.

#include "common/debug.h"

#include <stdio.h>
#include <stdarg.h>

namespace gl
{
	static void output(const char *format, va_list vararg)
	{
		#if 0
			FILE* file = fopen(TRACE_OUTPUT_FILE, "a");

			if(file)
			{
				vfprintf(file, format, vararg);
				fclose(file);
			}
		#endif
	}

	void trace(const char *format, ...)
	{
		va_list vararg;
		va_start(vararg, format);
		output(format, vararg);
		va_end(vararg);
	}
}
