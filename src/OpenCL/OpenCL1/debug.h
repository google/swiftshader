#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#define TRACE_OUTPUT_FILE "C:/Users/mgregoire/opencl.txt"

void trace(const char *format, ...);

#define UNIMPLEMENTED() trace("\t! Unimplemented: %s(%d)\n", __FUNCTION__, __LINE__)

#define TRACE(message, ...) trace("trace: %s(%d): " message "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)