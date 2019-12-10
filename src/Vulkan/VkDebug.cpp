// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#include "VkDebug.hpp"

#include <string>
#include <atomic>
#include <stdarg.h>

#if defined(__unix__)
#define PTRACE
#include <sys/types.h>
#include <sys/ptrace.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__)
#include <unistd.h>
#include <sys/sysctl.h>
#endif

namespace {

bool IsUnderDebugger()
{
#if defined(PTRACE) && !defined(__APPLE__) && !defined(__MACH__)
	static bool checked = false;
	static bool res = false;

	if (!checked)
	{
		// If a debugger is attached then we're already being ptraced and ptrace
		// will return a non-zero value.
		checked = true;
		if (ptrace(PTRACE_TRACEME, 0, 1, 0) != 0)
		{
			res = true;
		}
		else
		{
			ptrace(PTRACE_DETACH, 0, 1, 0);
		}
	}

	return res;
#elif defined(_WIN32) || defined(_WIN64)
	return IsDebuggerPresent() != 0;
#elif defined(__APPLE__) || defined(__MACH__)
	// Code comes from the Apple Technical Q&A QA1361

	// Tell sysctl what info we're requestion. Specifically we're asking for
	// info about this our PID.
	int res = 0;
	int request[4] = {
		CTL_KERN,
		KERN_PROC,
		KERN_PROC_PID,
		getpid()
	};
	struct kinfo_proc info;
	size_t size = sizeof(info);

	info.kp_proc.p_flag = 0;

	// Get the info we're requesting, if sysctl fails then info.kp_proc.p_flag will remain 0.
	res = sysctl(request, sizeof(request) / sizeof(*request), &info, &size, NULL, 0);
	ASSERT_MSG(res == 0, "syscl returned %d", res);

	// We're being debugged if the P_TRACED flag is set
	return ((info.kp_proc.p_flag & P_TRACED) != 0);
#else
	return false;
#endif
}

}  // anonymous namespace

namespace vk {

void tracev(const char *format, va_list args)
{
#ifndef SWIFTSHADER_DISABLE_TRACE
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

void trace_assert(const char *format, ...)
{
	static std::atomic<bool> asserted = {false};
	va_list vararg;
	va_start(vararg, format);

	if (IsUnderDebugger() && !asserted.exchange(true))
	{
		// Abort after tracing and printing to stderr
		tracev(format, vararg);
		va_end(vararg);

		va_start(vararg, format);
		vfprintf(stderr, format, vararg);
		va_end(vararg);

		::abort();
	}
	else if (!asserted)
	{
		tracev(format, vararg);
		va_end(vararg);
	}
}

}  // namespace vk
