//TODO: copyrights

#include "context.h"
#include "device_interface.h"
#include "propertylist.h"
#include "platform.h"

#include <cstring>
#include <cstdlib>

//#include <llvm/Support/TargetSelect.h>

using namespace Devices;

static void __stdcall default_pfn_notify(const char *, const void *, size_t, void *)
{
	return;
}

Context::Context(const cl_context_properties *properties,
	cl_uint num_devices,
	const cl_device_id *devices,
	void (CL_CALLBACK *pfn_notify)(const char *, const void *,
	size_t, void *),
	void *user_data,
	cl_int *errcode_ret)
	: Object(Object::T_Context, 0), p_properties(0), p_pfn_notify(pfn_notify),
	p_user_data(user_data), p_devices(0), p_num_devices(0), p_props_len(0),
	p_platform(0), wgl_handle(0), opengl_context_handle(0)
{
	if(!p_pfn_notify)
		p_pfn_notify = &default_pfn_notify;

	// Intialize LLVM, this can be done more than one time per program
	/*llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();*/

	// Explore the properties
	if(properties)
	{
		const unsigned char *props = (const unsigned char *)properties;
		cl_context_properties prop;
		size_t props_len = 0;

#define GET_PROP(type, var)     \
    var = *(const type *)props;       \
    props += sizeof(type);      \
    props_len += sizeof(type);

		while(true)
		{
			GET_PROP(cl_context_properties, prop)

				if(!prop)
					break;

			switch(prop)
			{
			case CL_CONTEXT_PLATFORM:
				GET_PROP(cl_platform_id, p_platform);
				break;

			case CL_WGL_HDC_KHR:
				GET_PROP(cl_context_properties, wgl_handle);
				break;

			case CL_GL_CONTEXT_KHR:
				GET_PROP(cl_context_properties, opengl_context_handle);
				break;

			default:
				*errcode_ret = CL_INVALID_PROPERTY;
				return;
			}
		}

		// properties may be allocated on the stack of the client application
		// copy it into a real buffer
		p_properties = (cl_context_properties *)std::malloc(props_len);
		p_props_len = props_len;

		if(!p_properties)
		{
			*errcode_ret = CL_OUT_OF_HOST_MEMORY;
			return;
		}

		std::memcpy((void *)p_properties, (const void *)properties, props_len);
	}

	// Verify that the platform is good
	if(p_platform != DEFAULT_PLATFORM)
	{
		*errcode_ret = CL_INVALID_PLATFORM;
		return;
	}

	// Explore the devices
	p_devices = (DeviceInterface **)std::malloc(num_devices * sizeof(DeviceInterface *));
	p_num_devices = num_devices;

	if(!p_devices)
	{
		*errcode_ret = CL_OUT_OF_HOST_MEMORY;
		return;
	}

	for(cl_uint i = 0; i<num_devices; ++i)
	{
		cl_device_id device = devices[i];

		if(device == 0)
		{
			*errcode_ret = CL_INVALID_DEVICE;
			return;
		}

		// Verify that the device is available
		cl_bool device_available;

		*errcode_ret = device->info(CL_DEVICE_AVAILABLE,
			sizeof(device_available),
			&device_available,
			0);

		if(*errcode_ret != CL_SUCCESS)
			return;

		if(!device_available)
		{
			*errcode_ret = CL_DEVICE_NOT_AVAILABLE;
			return;
		}

		// Add the device to the list
		p_devices[i] = (DeviceInterface *)device;
	}
}

Context::~Context()
{
	if(p_properties)
		std::free((void *)p_properties);

	if(p_devices)
		std::free((void *)p_devices);
}

cl_int Context::info(cl_context_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret) const
{
	void *value = 0;
	size_t value_length = 0;

	union {
		cl_uint cl_uint_var;
	};

	switch(param_name)
	{
	case CL_CONTEXT_REFERENCE_COUNT:
		SIMPLE_ASSIGN(cl_uint, references());
		break;

	case CL_CONTEXT_NUM_DEVICES:
		SIMPLE_ASSIGN(cl_uint, p_num_devices);
		break;

	case CL_CONTEXT_DEVICES:
		MEM_ASSIGN(p_num_devices * sizeof(DeviceInterface *), p_devices);
		break;

	case CL_CONTEXT_PROPERTIES:
		MEM_ASSIGN(p_props_len, p_properties);
		break;

	default:
		return CL_INVALID_VALUE;
	}

	if(param_value && param_value_size < value_length)
		return CL_INVALID_VALUE;

	if(param_value_size_ret)
		*param_value_size_ret = value_length;

	if(param_value && value_length /* CONTEXT_PROPERTIES can be of length 0 */)
		std::memcpy(param_value, value, value_length);

	return CL_SUCCESS;
}

bool Context::hasDevice(DeviceInterface *device) const
{
	for(unsigned int i = 0; i<p_num_devices; ++i)
		if(p_devices[i] == device)
			return true;

	return false;
}
