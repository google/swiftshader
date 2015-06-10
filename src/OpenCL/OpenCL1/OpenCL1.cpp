//TODO: Copyrights

#include <windows.h>
#include <cstring>

#include "opencl.h"
#include "device.h"
#include "debug.h"
#include "context.h"
#include "platform.h"
#include "commandqueue.h"
#include "buffer.h"
#include "memobject.h"
#include "events.h"
#include "dllmain.h"

static Devices::CPUDevice cpudevice;
//static Devices::GPUDevice gpudevice;

static inline cl_int queueEvent(Devices::CommandQueue *queue,
	Devices::Event *command,
	cl_event *event,
	cl_bool blocking)
{
	cl_int rs;

	rs = queue->queueEvent(command);

	if(rs != CL_SUCCESS)
	{
		delete command;
		return rs;
	}

	if(event)
	{
		*event = (cl_event)command;
		command->reference();
	}

	if(blocking)
	{
		rs = clWaitForEvents(1, (cl_event *)&command);

		if(rs != CL_SUCCESS)
		{
			delete command;
			return rs;
		}
	}

	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformIDs(cl_uint num_entries, cl_platform_id * platforms, cl_uint * num_platforms) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_uint num_entries = %u, cl_platform_id * platforms = %p, cl_uint * num_platforms = %p)", num_entries, platforms, num_platforms);

	if(num_platforms)
	{
		*num_platforms = MAX_PLATFORMS;
	}
	else if(!platforms)
	{
		return CL_INVALID_VALUE;
	}

	if(!num_entries && platforms)
	{
		return CL_INVALID_VALUE;
	}

	if(platforms != 0)
	{
		*platforms = DEFAULT_PLATFORM;
	}

	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_platform_id platform = %p, cl_platform_info param_name = %u, size_t param_value_size = %lu, void * param_value = %p, size_t * param_value_size_ret = %lu)", platform, param_name, param_value_size, param_value, param_value_size_ret);
	
	static const char platform_profile[] = "FULL_PROFILE";
	static const char platform_version[] = "OpenCL 1.1 CUDA 7.0.28";
	static const char platform_name[] = "NVIDIA CUDA";
	static const char platform_vendor[] = "NVIDIA Corporation";	//TODO: get whitelisted by Adobe
	static const char platform_extensions[] = "cl_khr_fp64 cl_khr_int64_base_atomics cl_khr_int64_extended_atomics";

	const char *string = 0;
	unsigned long length = 0;

	if(platform != DEFAULT_PLATFORM)
	{
		return CL_INVALID_PLATFORM;
	}

	switch(param_name) 
	{
	case CL_PLATFORM_PROFILE:
		string = platform_profile;
		length = sizeof(platform_profile);
		break;

	case CL_PLATFORM_VERSION:
		string = platform_version;
		length = sizeof(platform_version);
		break;

	case CL_PLATFORM_NAME:
		string = platform_name;
		length = sizeof(platform_name);
		break;

	case CL_PLATFORM_VENDOR:
		string = platform_vendor;
		length = sizeof(platform_vendor);
		break;

	case CL_PLATFORM_EXTENSIONS:
		string = platform_extensions;
		length = sizeof(platform_extensions);
		break;

	default:
		return CL_INVALID_VALUE;
	}

	if(param_value_size < length && param_value != 0)
	{
		return CL_INVALID_VALUE;
	}

	if(param_value != 0)
		std::memcpy(param_value, string, length);

	if(param_value_size_ret)
	{
		*param_value_size_ret = length;
	}

	return CL_SUCCESS;
}

/* Device APIs */
CL_API_ENTRY cl_int CL_API_CALL 
clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id * devices, cl_uint * num_devices) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_platform_id platform = %p, cl_device_type device_type = %u, cl_uint num_entries = %u, cl_device_id * devices = %p, cl_uint * num_devices = %p)", platform, device_type, num_entries, devices, num_devices);

	if(platform != DEFAULT_PLATFORM)
	{
		return CL_INVALID_PLATFORM;
	}

	if((num_entries == 0 && devices != 0) || (num_devices == 0 && devices == 0))
	{
		return CL_INVALID_VALUE;
	}

	switch(device_type)
	{
	case CL_DEVICE_TYPE_DEFAULT:
	case CL_DEVICE_TYPE_CPU:
		cpudevice.init();

		if(devices)
		devices[0] = (cl_device_id)(&cpudevice);

		if(num_devices)
			*num_devices = 1;
		break;
	case CL_DEVICE_TYPE_GPU:
		cpudevice.init();

		if(devices)
		devices[0] = (cl_device_id)(&cpudevice);

		if(num_devices)
			*num_devices = 1;
		break;
	case CL_DEVICE_TYPE_ACCELERATOR:
	case CL_DEVICE_TYPE_ALL:
		UNIMPLEMENTED();
	default:
		return CL_DEVICE_NOT_FOUND;
	}
	
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(cl_device_id device, cl_device_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_device_id device = %p, cl_device_info param_name = %u, size_t param_value_size = %lu, void * param_value = %p, size_t * param_value_size_ret = %p)", device, param_name, param_value_size, param_value, param_value_size_ret);

	UNIMPLEMENTED();

	return CL_DEVICE_NOT_FOUND;
}

/* Context APIs  */
CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(const cl_context_properties * properties, cl_uint num_devices, const cl_device_id * devices, void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),void * user_data, cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(const cl_context_properties * properties = %p, cl_uint num_devices = %u, const cl_device_id * devices = %p, void (CL_CALLBACK * pfn_notify) = %p, void * user_data = %p, cl_int * errcode_ret = %p)", properties, num_devices, devices, pfn_notify, user_data, errcode_ret);

	cl_int default_errcode_ret;

	// No errcode_ret ?
	if(!errcode_ret)
		errcode_ret = &default_errcode_ret;

	if(!devices ||
		!num_devices ||
		(!pfn_notify && user_data))
	{
		*errcode_ret = CL_INVALID_VALUE;
		return 0;
	}

	*errcode_ret = CL_SUCCESS;
	Devices::Context *ctx = new Devices::Context(properties, num_devices, devices,
		pfn_notify, user_data, errcode_ret);

	if(*errcode_ret != CL_SUCCESS)
	{
		// Initialization failed, destroy context
		delete ctx;
		return 0;
	}

	cl::makeCurrent(DEFAULT_PLATFORM, ctx);
	return (_cl_context *)ctx;
}

CL_API_ENTRY cl_context CL_API_CALL
clCreateContextFromType(const cl_context_properties * /* properties */,
cl_device_type                /* device_type */,
void (CL_CALLBACK *     /* pfn_notify*/)(const char *, const void *, size_t, void *),
void *                        /* user_data */,
cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainContext(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseContext(cl_context context) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_context context = %p)", context);

	if(!context->isA(Devices::Object::T_Context))
	{
		return CL_INVALID_CONTEXT;
	}

	if(context->dereference())
	{
		delete context;
		cl::makeCurrent(NULL, NULL);
	}

	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetContextInfo(cl_context         /* context */,
cl_context_info    /* param_name */,
size_t             /* param_value_size */,
void *             /* param_value */,
size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Command Queue APIs */
CL_API_ENTRY cl_command_queue CL_API_CALL
clCreateCommandQueue(cl_context context, cl_device_id device, cl_command_queue_properties properties, cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_context context = %p, cl_device_id device = %p, cl_command_queue_properties properties = %lu, cl_int * errcode_ret = %p)", context, device, properties, errcode_ret);

	cl_int default_errcode_ret;

	// No errcode_ret ?
	if(!errcode_ret)
		errcode_ret = &default_errcode_ret;

	if(!device->isA(Devices::Object::T_Device))
	{
		*errcode_ret = CL_INVALID_DEVICE;
		return NULL;
	}

	if(!context->isA(Devices::Object::T_Context))
	{
		*errcode_ret = CL_INVALID_CONTEXT;
		return NULL;
	}

	*errcode_ret = CL_SUCCESS;
	Devices::CommandQueue *queue = new Devices::CommandQueue(
		(Devices::Context *)context,
		(Devices::DeviceInterface *)device,
		properties,
		errcode_ret);

	if(*errcode_ret != CL_SUCCESS)
	{
		// Initialization failed, destroy context
		delete queue;
		return NULL;
	}

	return (_cl_command_queue *)queue;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseCommandQueue(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_command_queue command_queue = %p)", command_queue);

	if(!command_queue->isA(Devices::Object::T_CommandQueue))
		return CL_INVALID_COMMAND_QUEUE;

	command_queue->flush();

	if(command_queue->dereference())
		delete command_queue;

	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetCommandQueueInfo(cl_command_queue      /* command_queue */,
cl_command_queue_info /* param_name */,
size_t                /* param_value_size */,
void *                /* param_value */,
size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Memory Object APIs */
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(cl_context context, cl_mem_flags flags, size_t size, void * host_ptr, cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_context context = %p, cl_mem_flags flags = %lu, size_t size = %lu, void * host_ptr %p, cl_int * errcode_ret = %p)", context, flags, size, host_ptr, errcode_ret);

	cl_int dummy_errcode;

	if(!errcode_ret)
		errcode_ret = &dummy_errcode;

	if(!context->isA(Devices::Object::T_Context))
	{
		*errcode_ret = CL_INVALID_CONTEXT;
		return 0;
	}

	*errcode_ret = CL_SUCCESS;

	Devices::Buffer *buf = new Devices::Buffer(context, size, host_ptr, flags,
		errcode_ret);

	if(*errcode_ret != CL_SUCCESS || (*errcode_ret = buf->init()) != CL_SUCCESS)
	{
		delete buf;
		return 0;
	}

	return (cl_mem)buf;
}

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateSubBuffer(cl_mem                   /* buffer */,
cl_mem_flags             /* flags */,
cl_buffer_create_type    /* buffer_create_type */,
const void *             /* buffer_create_info */,
cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateImage2D(cl_context              /* context */,
cl_mem_flags            /* flags */,
const cl_image_format * /* image_format */,
size_t                  /* image_width */,
size_t                  /* image_height */,
size_t                  /* image_row_pitch */,
void *                  /* host_ptr */,
cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateImage3D(cl_context              /* context */,
cl_mem_flags            /* flags */,
const cl_image_format * /* image_format */,
size_t                  /* image_width */,
size_t                  /* image_height */,
size_t                  /* image_depth */,
size_t                  /* image_row_pitch */,
size_t                  /* image_slice_pitch */,
void *                  /* host_ptr */,
cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern "C" CL_API_ENTRY cl_int CL_API_CALL
clRetainMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseMemObject(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_mem memobj = %p)", memobj);

	if(!memobj->isA(Devices::Object::T_MemObject))
		return CL_INVALID_MEM_OBJECT;

	if(memobj->dereference())
		delete memobj;

	return CL_SUCCESS;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetSupportedImageFormats(cl_context           /* context */,
cl_mem_flags         /* flags */,
cl_mem_object_type   /* image_type */,
cl_uint              /* num_entries */,
cl_image_format *    /* image_formats */,
cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(cl_mem           /* memobj */,
cl_mem_info      /* param_name */,
size_t           /* param_value_size */,
void *           /* param_value */,
size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetImageInfo(cl_mem           /* image */,
cl_image_info    /* param_name */,
size_t           /* param_value_size */,
void *           /* param_value */,
size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clSetMemObjectDestructorCallback(cl_mem /* memobj */,
void (CL_CALLBACK * /*pfn_notify*/)(cl_mem /* memobj */, void* /*user_data*/),
void * /*user_data */)             CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

/* Sampler APIs  */
extern CL_API_ENTRY cl_sampler CL_API_CALL
clCreateSampler(cl_context          /* context */,
cl_bool             /* normalized_coords */,
cl_addressing_mode  /* addressing_mode */,
cl_filter_mode      /* filter_mode */,
cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clRetainSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetSamplerInfo(cl_sampler         /* sampler */,
cl_sampler_info    /* param_name */,
size_t             /* param_value_size */,
void *             /* param_value */,
size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Program Object APIs  */
extern CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(cl_context        /* context */,
cl_uint           /* count */,
const char **     /* strings */,
const size_t *    /* lengths */,
cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithBinary(cl_context                     /* context */,
cl_uint                        /* num_devices */,
const cl_device_id *           /* device_list */,
const size_t *                 /* lengths */,
const unsigned char **         /* binaries */,
cl_int *                       /* binary_status */,
cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clRetainProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern "C" CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(cl_program           /* program */,
cl_uint              /* num_devices */,
const cl_device_id * /* device_list */,
const char *         /* options */,
void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clUnloadCompiler(void) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetProgramInfo(cl_program         /* program */,
cl_program_info    /* param_name */,
size_t             /* param_value_size */,
void *             /* param_value */,
size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetProgramBuildInfo(cl_program            /* program */,
cl_device_id          /* device */,
cl_program_build_info /* param_name */,
size_t                /* param_value_size */,
void *                /* param_value */,
size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Kernel Object APIs */
extern CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(cl_program      /* program */,
const char *    /* kernel_name */,
cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clCreateKernelsInProgram(cl_program     /* program */,
cl_uint        /* num_kernels */,
cl_kernel *    /* kernels */,
cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clRetainKernel(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(cl_kernel    /* kernel */,
cl_uint      /* arg_index */,
size_t       /* arg_size */,
const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetKernelInfo(cl_kernel       /* kernel */,
cl_kernel_info  /* param_name */,
size_t          /* param_value_size */,
void *          /* param_value */,
size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetKernelWorkGroupInfo(cl_kernel                  /* kernel */,
cl_device_id               /* device */,
cl_kernel_work_group_info  /* param_name */,
size_t                     /* param_value_size */,
void *                     /* param_value */,
size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Event Object APIs  */
extern CL_API_ENTRY cl_int CL_API_CALL
clWaitForEvents(cl_uint             /* num_events */,
const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetEventInfo(cl_event         /* event */,
cl_event_info    /* param_name */,
size_t           /* param_value_size */,
void *           /* param_value */,
size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_event CL_API_CALL
clCreateUserEvent(cl_context    /* context */,
cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clRetainEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseEvent(cl_event event) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_event event = %p)", event);
	
	if(!event->isA(Devices::Object::T_Event))
	return CL_INVALID_EVENT;

	if(event->dereference())
	{
		event->freeDeviceData();
		delete event;
	}

	return CL_SUCCESS;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clSetUserEventStatus(cl_event   /* event */,
cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clSetEventCallback(cl_event    /* event */,
cl_int      /* command_exec_callback_type */,
void (CL_CALLBACK * /* pfn_notify */)(cl_event, cl_int, void *),
void *      /* user_data */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

/* Profiling APIs  */
extern CL_API_ENTRY cl_int CL_API_CALL
clGetEventProfilingInfo(cl_event event, cl_profiling_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_event event = %p, cl_profiling_info param_name = %u, size_t param_value_size = %lu, void * param_value = %p, size_t * param_value_size_ret = %lu)", event, param_name, param_value_size, param_value, param_value_size_ret);
	if(!event->isA(Devices::Object::T_Event))
	{
		return CL_INVALID_EVENT;
	}

	return event->profilingInfo(param_name, param_value_size, param_value,
		param_value_size_ret);
}

/* Flush and Finish APIs */
extern CL_API_ENTRY cl_int CL_API_CALL
clFlush(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clFinish(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_command_queue command_queue = %p)", command_queue);

	if(!command_queue->isA(Devices::Object::T_CommandQueue))
	{
		return CL_INVALID_COMMAND_QUEUE;
	}

	command_queue->finish();

	return CL_SUCCESS;
}

/* Enqueued Commands APIs */
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBuffer(cl_command_queue    /* command_queue */,
cl_mem              /* buffer */,
cl_bool             /* blocking_read */,
size_t              /* offset */,
size_t              /* cb */,
void *              /* ptr */,
cl_uint             /* num_events_in_wait_list */,
const cl_event *    /* event_wait_list */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBufferRect(cl_command_queue    /* command_queue */,
cl_mem              /* buffer */,
cl_bool             /* blocking_read */,
const size_t *      /* buffer_origin */,
const size_t *      /* host_origin */,
const size_t *      /* region */,
size_t              /* buffer_row_pitch */,
size_t              /* buffer_slice_pitch */,
size_t              /* host_row_pitch */,
size_t              /* host_slice_pitch */,
void *              /* ptr */,
cl_uint             /* num_events_in_wait_list */,
const cl_event *    /* event_wait_list */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBuffer(cl_command_queue   /* command_queue */,
cl_mem             /* buffer */,
cl_bool            /* blocking_write */,
size_t             /* offset */,
size_t             /* cb */,
const void *       /* ptr */,
cl_uint            /* num_events_in_wait_list */,
const cl_event *   /* event_wait_list */,
cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBufferRect(cl_command_queue    /* command_queue */,
cl_mem              /* buffer */,
cl_bool             /* blocking_write */,
const size_t *      /* buffer_origin */,
const size_t *      /* host_origin */,
const size_t *      /* region */,
size_t              /* buffer_row_pitch */,
size_t              /* buffer_slice_pitch */,
size_t              /* host_row_pitch */,
size_t              /* host_slice_pitch */,
const void *        /* ptr */,
cl_uint             /* num_events_in_wait_list */,
const cl_event *    /* event_wait_list */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBuffer(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, size_t src_offset, size_t dst_offset, size_t cb, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) CL_API_SUFFIX__VERSION_1_0
{
	TRACE("(cl_command_queue command_queue = %p, cl_mem src_buffer = %p, cl_mem dst_buffer = %p, size_t src_offset = %ul, size_t dst_offset = %ul, size_t cb = %ul, cl_uint num_events_in_wait_list = %u, const cl_event * event_wait_list = %p, cl_event * event = %p)", command_queue, src_buffer, dst_buffer, src_offset, dst_offset, cb, num_events_in_wait_list, event_wait_list, event);
	
	cl_int rs = CL_SUCCESS;

	if(!command_queue->isA(Devices::Object::T_CommandQueue))
		return CL_INVALID_COMMAND_QUEUE;

	Devices::CopyBufferEvent *command = new Devices::CopyBufferEvent(
		(Devices::CommandQueue *)command_queue,
		(Devices::MemObject *)src_buffer,
		(Devices::MemObject *)dst_buffer,
		src_offset, dst_offset, cb,
		num_events_in_wait_list, (const Devices::Event **)event_wait_list, &rs
		);

	if(rs != CL_SUCCESS)
	{
		delete command;
		return rs;
	}

	return queueEvent(command_queue, command, event, false);
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBufferRect(cl_command_queue    /* command_queue */,
cl_mem              /* src_buffer */,
cl_mem              /* dst_buffer */,
const size_t *      /* src_origin */,
const size_t *      /* dst_origin */,
const size_t *      /* region */,
size_t              /* src_row_pitch */,
size_t              /* src_slice_pitch */,
size_t              /* dst_row_pitch */,
size_t              /* dst_slice_pitch */,
cl_uint             /* num_events_in_wait_list */,
const cl_event *    /* event_wait_list */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadImage(cl_command_queue     /* command_queue */,
cl_mem               /* image */,
cl_bool              /* blocking_read */,
const size_t *       /* origin[3] */,
const size_t *       /* region[3] */,
size_t               /* row_pitch */,
size_t               /* slice_pitch */,
void *               /* ptr */,
cl_uint              /* num_events_in_wait_list */,
const cl_event *     /* event_wait_list */,
cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteImage(cl_command_queue    /* command_queue */,
cl_mem              /* image */,
cl_bool             /* blocking_write */,
const size_t *      /* origin[3] */,
const size_t *      /* region[3] */,
size_t              /* input_row_pitch */,
size_t              /* input_slice_pitch */,
const void *        /* ptr */,
cl_uint             /* num_events_in_wait_list */,
const cl_event *    /* event_wait_list */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyImage(cl_command_queue     /* command_queue */,
cl_mem               /* src_image */,
cl_mem               /* dst_image */,
const size_t *       /* src_origin[3] */,
const size_t *       /* dst_origin[3] */,
const size_t *       /* region[3] */,
cl_uint              /* num_events_in_wait_list */,
const cl_event *     /* event_wait_list */,
cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyImageToBuffer(cl_command_queue /* command_queue */,
cl_mem           /* src_image */,
cl_mem           /* dst_buffer */,
const size_t *   /* src_origin[3] */,
const size_t *   /* region[3] */,
size_t           /* dst_offset */,
cl_uint          /* num_events_in_wait_list */,
const cl_event * /* event_wait_list */,
cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBufferToImage(cl_command_queue /* command_queue */,
cl_mem           /* src_buffer */,
cl_mem           /* dst_image */,
size_t           /* src_offset */,
const size_t *   /* dst_origin[3] */,
const size_t *   /* region[3] */,
cl_uint          /* num_events_in_wait_list */,
const cl_event * /* event_wait_list */,
cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY void * CL_API_CALL
clEnqueueMapBuffer(cl_command_queue /* command_queue */,
cl_mem           /* buffer */,
cl_bool          /* blocking_map */,
cl_map_flags     /* map_flags */,
size_t           /* offset */,
size_t           /* cb */,
cl_uint          /* num_events_in_wait_list */,
const cl_event * /* event_wait_list */,
cl_event *       /* event */,
cl_int *         /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY void * CL_API_CALL
clEnqueueMapImage(cl_command_queue  /* command_queue */,
cl_mem            /* image */,
cl_bool           /* blocking_map */,
cl_map_flags      /* map_flags */,
const size_t *    /* origin[3] */,
const size_t *    /* region[3] */,
size_t *          /* image_row_pitch */,
size_t *          /* image_slice_pitch */,
cl_uint           /* num_events_in_wait_list */,
const cl_event *  /* event_wait_list */,
cl_event *        /* event */,
cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueUnmapMemObject(cl_command_queue /* command_queue */,
cl_mem           /* memobj */,
void *           /* mapped_ptr */,
cl_uint          /* num_events_in_wait_list */,
const cl_event *  /* event_wait_list */,
cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNDRangeKernel(cl_command_queue /* command_queue */,
cl_kernel        /* kernel */,
cl_uint          /* work_dim */,
const size_t *   /* global_work_offset */,
const size_t *   /* global_work_size */,
const size_t *   /* local_work_size */,
cl_uint          /* num_events_in_wait_list */,
const cl_event * /* event_wait_list */,
cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueTask(cl_command_queue  /* command_queue */,
cl_kernel         /* kernel */,
cl_uint           /* num_events_in_wait_list */,
const cl_event *  /* event_wait_list */,
cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNativeKernel(cl_command_queue  /* command_queue */,
void (CL_CALLBACK *user_func)(void *),
void *            /* args */,
size_t            /* cb_args */,
cl_uint           /* num_mem_objects */,
const cl_mem *    /* mem_list */,
const void **     /* args_mem_loc */,
cl_uint           /* num_events_in_wait_list */,
const cl_event *  /* event_wait_list */,
cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueMarker(cl_command_queue    /* command_queue */,
cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
cl_uint          /* num_events */,
const cl_event * /* event_list */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueBarrier(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

/* Extension function access
*
* Returns the extension function address for the given function name,
* or NULL if a valid function can not be found.  The client must
* check to make sure the address is not NULL, before using or
* calling the returned function address.
*/
extern CL_API_ENTRY void * CL_API_CALL clGetExtensionFunctionAddress(const char * /* func_name */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clSetCommandQueueProperty(cl_command_queue              /* command_queue */,
cl_command_queue_properties   /* properties */,
cl_bool                        /* enable */,
cl_command_queue_properties * /* old_properties */) CL_EXT_SUFFIX__VERSION_1_0_DEPRECATED
{
	UNIMPLEMENTED();
	return 0;
}

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

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLBuffer(cl_context     /* context */,
cl_mem_flags   /* flags */,
cl_GLuint      /* bufobj */,
int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}


extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLTexture2D(cl_context      /* context */,
cl_mem_flags    /* flags */,
cl_GLenum       /* target */,
cl_GLint        /* miplevel */,
cl_GLuint       /* texture */,
cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLTexture3D(cl_context      /* context */,
cl_mem_flags    /* flags */,
cl_GLenum       /* target */,
cl_GLint        /* miplevel */,
cl_GLuint       /* texture */,
cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLRenderbuffer(cl_context   /* context */,
cl_mem_flags /* flags */,
cl_GLuint    /* renderbuffer */,
cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetGLObjectInfo(cl_mem                /* memobj */,
cl_gl_object_type *   /* gl_object_type */,
cl_GLuint *              /* gl_object_name */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetGLTextureInfo(cl_mem               /* memobj */,
cl_gl_texture_info   /* param_name */,
size_t               /* param_value_size */,
void *               /* param_value */,
size_t *             /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireGLObjects(cl_command_queue      /* command_queue */,
cl_uint               /* num_objects */,
const cl_mem *        /* mem_objects */,
cl_uint               /* num_events_in_wait_list */,
const cl_event *      /* event_wait_list */,
cl_event *            /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseGLObjects(cl_command_queue      /* command_queue */,
cl_uint               /* num_objects */,
const cl_mem *        /* mem_objects */,
cl_uint               /* num_events_in_wait_list */,
const cl_event *      /* event_wait_list */,
cl_event *            /* event */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetGLContextInfoKHR(const cl_context_properties * /* properties */,
cl_gl_context_info            /* param_name */,
size_t                        /* param_value_size */,
void *                        /* param_value */,
size_t *                      /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
{
	UNIMPLEMENTED();
	return 0;
}

extern CL_API_ENTRY cl_event CL_API_CALL
clCreateEventFromGLsyncKHR(cl_context           /* context */,
cl_GLsync            /* cl_GLsync */,
cl_int *             /* errcode_ret */) CL_EXT_SUFFIX__VERSION_1_1
{
	UNIMPLEMENTED();
	return 0;
}