//TODO: copyrights

#include "worker.h"
#include "device.h"
#include "buffer.h"
#include "kernel.h"
#include "builtins.h"

#include "commandqueue.h"
#include "events.h"
#include "memobject.h"
#include "kernel.h"


#include <cstring>
#include <iostream>

using namespace Devices;

void worker(void *data)
{
	CPUDevice *device = (CPUDevice *)data;
	bool stop = false;
	cl_int errcode;
	Event *event;

	// Initialize TLS
	setWorkItemsData(0, 0);

	while(true)
	{
		event = device->getEvent(stop);

		// Ensure we have a good event and we don't have to stop
		if(stop) break;
		if(!event) continue;

		// Get info about the event and its command queue
		Event::Type t = event->type();
		CommandQueue *queue = 0;
		cl_command_queue_properties queue_props = 0;

		errcode = CL_SUCCESS;

		event->info(CL_EVENT_COMMAND_QUEUE, sizeof(CommandQueue *), &queue, 0);

		if(queue)
			queue->info(CL_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties),
			&queue_props, 0);

		if(queue_props & CL_QUEUE_PROFILING_ENABLE)
			event->updateTiming(Event::Start);

		// Execute the action
		switch(t)
		{
		case Event::ReadBuffer:
		case Event::WriteBuffer:
		{
			ReadWriteBufferEvent *e = (ReadWriteBufferEvent *)event;
			CPUBuffer *buf = (CPUBuffer *)e->buffer()->deviceBuffer(device);
			char *data = (char *)buf->data();

			data += e->offset();

			if(t == Event::ReadBuffer)
				std::memcpy(e->ptr(), data, e->cb());
			else
				std::memcpy(data, e->ptr(), e->cb());

			break;
		}
		case Event::CopyBuffer:
		{
			CopyBufferEvent *e = (CopyBufferEvent *)event;
			CPUBuffer *src = (CPUBuffer *)e->source()->deviceBuffer(device);
			CPUBuffer *dst = (CPUBuffer *)e->destination()->deviceBuffer(device);

			std::memcpy(dst->data(), src->data(), e->cb());

			break;
		}
		case Event::ReadBufferRect:
		case Event::WriteBufferRect:
		case Event::CopyBufferRect:
		case Event::ReadImage:
		case Event::WriteImage:
		case Event::CopyImage:
		case Event::CopyBufferToImage:
		case Event::CopyImageToBuffer:
		{
			// src = buffer and dst = mem if note copy
			ReadWriteCopyBufferRectEvent *e = (ReadWriteCopyBufferRectEvent *)event;
			CPUBuffer *src_buf = (CPUBuffer *)e->source()->deviceBuffer(device);

			unsigned char *src = (unsigned char *)src_buf->data();
			unsigned char *dst;

			switch(t)
			{
			case Event::CopyBufferRect:
			case Event::CopyImage:
			case Event::CopyImageToBuffer:
			case Event::CopyBufferToImage:
			{
				CopyBufferRectEvent *cbre = (CopyBufferRectEvent *)e;
				CPUBuffer *dst_buf =
					(CPUBuffer *)cbre->destination()->deviceBuffer(device);

				dst = (unsigned char *)dst_buf->data();
				break;
			}
			default:
			{
				// dst = host memory location
				ReadWriteBufferRectEvent *rwbre = (ReadWriteBufferRectEvent *)e;

				dst = (unsigned char *)rwbre->ptr();
			}
			}

			// Iterate over the lines to copy and use memcpy
			for(size_t z = 0; z<e->region(2); ++z)
			{
				for(size_t y = 0; y<e->region(1); ++y)
				{
					unsigned char *s;
					unsigned char *d;

					d = imageData(dst,
						e->dst_origin(0),
						y + e->dst_origin(1),
						z + e->dst_origin(2),
						e->dst_row_pitch(),
						e->dst_slice_pitch(),
						1);

					s = imageData(src,
						e->src_origin(0),
						y + e->src_origin(1),
						z + e->src_origin(2),
						e->src_row_pitch(),
						e->src_slice_pitch(),
						1);

					// Copying and image to a buffer may need to add an offset
					// to the buffer address (its rectangular origin is
					// always (0, 0, 0)).
					if(t == Event::CopyBufferToImage)
					{
						CopyBufferToImageEvent *cptie = (CopyBufferToImageEvent *)e;
						s += cptie->offset();
					}
					else if(t == Event::CopyImageToBuffer)
					{
						CopyImageToBufferEvent *citbe = (CopyImageToBufferEvent *)e;
						d += citbe->offset();
					}

					if(t == Event::WriteBufferRect || t == Event::WriteImage)
						std::memcpy(s, d, e->region(0)); // Write dest (memory) in src
					else
						std::memcpy(d, s, e->region(0)); // Write src (buffer) in dest (memory), or copy the buffers
				}
			}

			break;
		}
		case Event::MapBuffer:
		case Event::MapImage:
			// All was already done in CPUBuffer::initEventDeviceData()
			break;

		case Event::NativeKernel:
		{
			NativeKernelEvent *e = (NativeKernelEvent *)event;
			void(*func)(void *) = (void(*)(void *))e->function();
			void *args = e->args();

			func(args);

			break;
		}
		case Event::NDRangeKernel:
		case Event::TaskKernel:
		{
			KernelEvent *e = (KernelEvent *)event;
			CPUKernelEvent *ke = (CPUKernelEvent *)e->deviceData();

			// Take an instance
			CPUKernelWorkGroup *instance = ke->takeInstance();
			ke = 0;     // Unlocked, don't use anymore

			if(!instance->run())
				errcode = CL_INVALID_PROGRAM_EXECUTABLE;

			delete instance;

			break;
		}
		default:
			break;
		}

		// Cleanups
		if(errcode == CL_SUCCESS)
		{
			bool finished = true;

			if(event->type() == Event::NDRangeKernel ||
				event->type() == Event::TaskKernel)
			{
				CPUKernelEvent *ke = (CPUKernelEvent *)event->deviceData();
				finished = ke->finished();
			}

			if(finished)
			{
				event->setStatus(Event::Complete);

				if(queue_props & CL_QUEUE_PROFILING_ENABLE)
					event->updateTiming(Event::End);

				// Clean the queue
				if(queue)
					queue->cleanEvents();
			}
		}
		else
		{
			// The event failed
			event->setStatus((Event::Status)errcode);

			if(queue_props & CL_QUEUE_PROFILING_ENABLE)
				event->updateTiming(Event::End);
		}
	}

	// Free mmapped() data if needed
	size_t mapped_size;
	void *mapped_data = getWorkItemsData(mapped_size);

	/*if(mapped_data)
		munmap(mapped_data, mapped_size);*/
}
