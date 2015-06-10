#include "debug.h"

static void output(const char *format, va_list vararg)
{
	if(true)
	{
		static FILE* file = nullptr;
		file = fopen(TRACE_OUTPUT_FILE, "a+");
		vfprintf(file, format, vararg);
		fclose(file);
		/*if(!file)
		{

		}

		if(file)
		{
		vfprintf(file, format, vararg);
		}*/
	}
}


void trace(const char *format, ...)
{
	va_list vararg;
	va_start(vararg, format);
	output(format, vararg);
	va_end(vararg);
}