#include "DebugAndroid.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <cutils/properties.h>

void AndroidEnterDebugger()
{
	ALOGE(__FUNCTION__);
#ifndef NDEBUG
	static volatile int * const makefault = nullptr;
	char value[PROPERTY_VALUE_MAX];
	property_get("debug.db.uid", value, "-1");
	int debug_uid = atoi(value);
	if((debug_uid >= 0) && (geteuid() < static_cast<uid_t>(debug_uid)))
	{
		ALOGE("Waiting for debugger: gdbserver :${PORT} --attach %u. Look for thread %u", getpid(), gettid());
		volatile int waiting = 1;
		while (waiting) {
			sleep(1);
		}
	}
	else
	{
		ALOGE("No debugger");
	}
#endif
}

void trace(const char *format, ...)
{
#ifndef NDEBUG
	va_list vararg;
	va_start(vararg, format);
	android_vprintLog(ANDROID_LOG_VERBOSE, NULL, LOG_TAG, format, vararg);
	va_end(vararg);
#endif
}
