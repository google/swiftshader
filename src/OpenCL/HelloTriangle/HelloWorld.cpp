// HelloTriangle.cpp : Defines the entry point for the console application.
//
#include "opencl.h"
#include <stdio.h>
#include <tchar.h>
#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

#define NUM_OF_EVENTS 100
#define BUFFER_SIZE 16777216

void getAttributes(cl_platform_id * platformIDs)
{
	size_t infoSize;
	char * info;

	const cl_platform_info attributeTypes[5] = {
		CL_PLATFORM_NAME,
		CL_PLATFORM_VENDOR,
		CL_PLATFORM_VERSION,
		CL_PLATFORM_PROFILE,
		CL_PLATFORM_EXTENSIONS };

	for(int j = 0; j < 5; j++)
	{
		// get platform attribute value size
		cl_int res5 = clGetPlatformInfo(platformIDs[0], attributeTypes[j], 0, NULL, &infoSize);
		info = (char*)malloc(infoSize);
		assert(res5 == CL_SUCCESS);

		// get platform attribute value
		cl_int res6 = clGetPlatformInfo(platformIDs[0], attributeTypes[j], infoSize, info, NULL);
		assert(res6 == CL_SUCCESS);
	}
}

int main(int argc, char **argv){
	cl_uint num_platforms1;
	cl_uint num_platforms2;
	cl_platform_id * platformIDs;

	cl_int res1 = clGetPlatformIDs(0, NULL, &num_platforms1);
	assert(res1 == CL_SUCCESS);
	cl_int res2 = clGetPlatformIDs(0, NULL, &num_platforms2);
	assert(res2 == CL_SUCCESS);

	platformIDs = (cl_platform_id *)alloca(
		sizeof(cl_platform_id) * num_platforms2);
	platformIDs[0] = 0;

	cl_int res3 = clGetPlatformIDs(num_platforms2, platformIDs, &num_platforms2);
	assert(res3 == CL_SUCCESS);

	getAttributes(platformIDs);
	size_t siz;
	const cl_platform_info attributeTypes[5] = {
		CL_PLATFORM_NAME,
		CL_PLATFORM_VENDOR,
		CL_PLATFORM_VERSION,
		CL_PLATFORM_PROFILE,
		CL_PLATFORM_EXTENSIONS };
	char * info;
	for(int j = 0; j < 5; j++)
	{
		// get platform attribute value size
		clGetPlatformInfo(platformIDs[0], attributeTypes[j], 0, NULL, &siz);
		info = (char*)malloc(siz);

		// get platform attribute value
		clGetPlatformInfo(platformIDs[0], attributeTypes[j], siz, info, NULL);
	}

	cl_uint num_devices;

	cl_int res4 = clGetDeviceIDs(platformIDs[0], 4, 0, NULL, &num_devices);
	assert(res4 == CL_SUCCESS);

	cl_device_id * devices;
	devices = (cl_device_id *)alloca(
		sizeof(cl_device_id) * num_devices);
	devices[0] = 0;

	cl_int res5 = clGetDeviceIDs(platformIDs[0], 4, 1, devices, NULL);
	assert(res5 == CL_SUCCESS);

	cl_int errcode_ret;
	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platformIDs[0],
		CL_WGL_HDC_KHR,
		0,
		CL_GL_CONTEXT_KHR,
		0,
		0
	};

	cl_context context = clCreateContext(
		contextProperties,
		num_devices,
		devices,
		NULL,
		NULL,
		&errcode_ret);
	assert(errcode_ret == CL_SUCCESS);

	cl_int errcodeQueue;

	cl_command_queue queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &errcodeQueue);

	char * tocopy = "foobar";
	cl_int errcodeBufferRead;
	cl_int errcodeBuffeWrite;

	cl_mem readBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, BUFFER_SIZE, NULL, &errcodeBufferRead);
	assert(errcodeBufferRead == CL_SUCCESS);
	cl_mem writeBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, BUFFER_SIZE, NULL, &errcodeBuffeWrite);
	assert(errcodeBuffeWrite == CL_SUCCESS);
	int res6 = clFinish(queue);
	assert(res6 == CL_SUCCESS);


	cl_event evnt[NUM_OF_EVENTS];

	int res7 = clEnqueueCopyBuffer(queue, readBuffer, writeBuffer, 0, 0, BUFFER_SIZE, 0, NULL, &evnt[0]);
	assert(res7 == CL_SUCCESS);

	for(int i = 1; i < NUM_OF_EVENTS; i++)
	{
		int res7 = clEnqueueCopyBuffer(queue, readBuffer, writeBuffer, 0, 0, BUFFER_SIZE, 0, NULL, &evnt[i]);
		assert(res7 == CL_SUCCESS);
	}

	int res8 = clFinish(queue);
	assert(res8 == CL_SUCCESS);

	cl_ulong time_start, time_end;
	double total_time[NUM_OF_EVENTS];

	for(int j = 0; j < NUM_OF_EVENTS; j++)
	{
		int res9 = clGetEventProfilingInfo(evnt[j], CL_PROFILING_COMMAND_END, 8, &time_end, NULL);
		assert(res9 == CL_SUCCESS);
		int res10 = clGetEventProfilingInfo(evnt[j], CL_PROFILING_COMMAND_START, 8, &time_start, NULL);
		assert(res10 == CL_SUCCESS);
		total_time[j] = time_end - time_start;
	}

	int res99 = clReleaseContext(context);

	return 0;
}