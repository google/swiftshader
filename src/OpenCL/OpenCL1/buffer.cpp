//TODO: copyrights

#include "buffer.h"
#include "device.h"

#include "memobject.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace Devices;

CPUBuffer::CPUBuffer(CPUDevice *device, MemObject *buffer, cl_int *rs)
	: DeviceBuffer(), p_device(device), p_buffer(buffer), p_data(0),
	p_data_malloced(false)
{
	if(buffer->type() == MemObject::SubBuffer)
	{
		// We need to create this CPUBuffer based on the CPUBuffer of the
		// parent buffer
		SubBuffer *subbuf = (SubBuffer *)buffer;
		MemObject *parent = subbuf->parent();
		CPUBuffer *parentcpubuf = (CPUBuffer *)parent->deviceBuffer(device);

		char *tmp_data = (char *)parentcpubuf->data();
		tmp_data += subbuf->offset();

		p_data = (void *)tmp_data;
	}
	else if(buffer->flags() & CL_MEM_USE_HOST_PTR)
	{
		// We use the host ptr, we are already allocated
		p_data = buffer->host_ptr();
	}

	// NOTE: This function can also reject Image buffers by setting a value
	// != CL_SUCCESS in rs.
}

CPUBuffer::~CPUBuffer()
{
	if(p_data_malloced)
	{
		std::free((void *)p_data);
	}
}

void *CPUBuffer::data() const
{
	return p_data;
}

void *CPUBuffer::nativeGlobalPointer() const
{
	return data();
}

bool CPUBuffer::allocate()
{
	size_t buf_size = p_buffer->size();

	if(buf_size == 0)
		// Something went wrong...
		return false;

	if(!p_data)
	{
		// We don't use a host ptr, we need to allocate a buffer
		p_data = std::malloc(buf_size);

		if(!p_data)
			return false;

		p_data_malloced = true;
	}

	if(p_buffer->type() != MemObject::SubBuffer &&
		p_buffer->flags() & CL_MEM_COPY_HOST_PTR)
	{
		std::memcpy(p_data, p_buffer->host_ptr(), buf_size);
	}

	// Say to the memobject that we are allocated
	p_buffer->deviceAllocated(this);

	return true;
}

DeviceInterface *CPUBuffer::device() const
{
	return p_device;
}

bool CPUBuffer::allocated() const
{
	return p_data != 0;
}
