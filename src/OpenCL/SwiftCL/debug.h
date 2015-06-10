#include <assert.h>
#include <stdio.h>

#define UNIMPLEMENTED() do { \
	static FILE* file = nullptr; \
	file = fopen("C:/Users/mgregoire/opencl.txt", "w"); \
	fprintf(file, "hi\n"); \
	fclose(file); \
    assert(false); \
} while(0)