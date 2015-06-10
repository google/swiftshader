#include "cl_ext.h"
#include "debug.h"

cl_int  CL_API_ENTRY clSetMemObjectDestructorAPPLE(cl_mem /* memobj */,
	void(* /*pfn_notify*/)(cl_mem /* memobj */, void* /*user_data*/),
	void * /*user_data */)             CL_EXT_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern void CL_API_ENTRY clLogMessagesToSystemLogAPPLE(const char * /* errstr */,
	const void * /* private_info */,
	size_t       /* cb */,
	void *       /* user_data */)  CL_EXT_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
}

extern void CL_API_ENTRY clLogMessagesToStdoutAPPLE(const char * /* errstr */,
	const void * /* private_info */,
	size_t       /* cb */,
	void *       /* user_data */)    CL_EXT_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
}

extern void CL_API_ENTRY clLogMessagesToStderrAPPLE(const char * /* errstr */,
	const void * /* private_info */,
	size_t       /* cb */,
	void *       /* user_data */)    CL_EXT_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
}

extern CL_API_ENTRY cl_int CL_API_CALL
clIcdGetPlatformIDsKHR(cl_uint          /* num_entries */,
cl_platform_id * /* platforms */,
cl_uint *        /* num_platforms */)
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseDeviceEXT(cl_device_id /*device*/) CL_EXT_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clRetainDeviceEXT(cl_device_id /*device*/) CL_EXT_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clCreateSubDevicesEXT(cl_device_id /*in_device*/,
const cl_device_partition_property_ext * /* properties */,
cl_uint /*num_entries*/,
cl_device_id * /*out_devices*/,
cl_uint * /*num_devices*/) CL_EXT_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceImageInfoQCOM(cl_device_id             device,
size_t                   image_width,
size_t                   image_height,
const cl_image_format   *image_format,
cl_image_pitch_info_qcom param_name,
size_t                   param_value_size,
void                    *param_value,
size_t                  *param_value_size_ret)
{
	UNIMPLEMENTED();
	return 0;
}