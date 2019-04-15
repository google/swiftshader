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

#include "Debug.hpp"

#include <string>
#include <stdarg.h>

namespace rr
{

void tracev(const char *format, va_list args)
{
#ifndef RR_DISABLE_TRACE
	if(false)
	{
		FILE *file = fopen(TRACE_OUTPUT_FILE, "a");

		if(file)
		{
			vfprintf(file, format, args);
			fclose(file);
		}
	}
#endif
}

void trace(const char *format, ...)
{
	va_list vararg;
	va_start(vararg, format);
	tracev(format, vararg);
	va_end(vararg);
}

void warn(const char *format, ...)
{
	va_list vararg;
	va_start(vararg, format);
	tracev(format, vararg);
	va_end(vararg);

	va_start(vararg, format);
	vfprintf(stderr, format, vararg);
	va_end(vararg);
}

void abort(const char *format, ...)
{
	va_list vararg;

	va_start(vararg, format);
	tracev(format, vararg);
	va_end(vararg);

	va_start(vararg, format);
	vfprintf(stderr, format, vararg);
	va_end(vararg);

	::abort();
}

} // namespace rr
