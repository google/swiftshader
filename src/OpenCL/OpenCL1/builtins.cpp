//TODO: copyrights

#include "builtins.h"
#include "kernel.h"
#include "buffer.h"

#include "events.h"
#include "memobject.h"

#include <signal.h>

//#include <llvm/Function.h>

#include <iostream>
#include <cstring>
#include <cmath>
#include <windows.h>

#include <stdio.h>

using namespace Devices;

unsigned char *imageData(unsigned char *base, size_t x, size_t y, size_t z,
	size_t row_pitch, size_t slice_pitch,
	unsigned int bytes_per_pixel)
{
	unsigned char *result = base;

	result += (z * slice_pitch) +
		(y * row_pitch) +
		(x * bytes_per_pixel);

	return result;
}

/*
* TLS-related functions
*/
//__thread Devices::CPUKernelWorkGroup *g_work_group;    /*!< \brief \c Coal::CPUKernelWorkGroup currently running on this thread */
//__thread void *work_items_data;                     /*!< \brief Space allocated for work-items stacks, see \ref barrier */
//__thread size_t work_items_size;                    /*!< \brief Size of \c work_items_data, see \ref barrier */

void setThreadLocalWorkGroup(Devices::CPUKernelWorkGroup *current)
{
	//g_work_group = current;
}

void *getWorkItemsData(size_t &size)
{
	//size = work_items_size;
	return 0;// work_items_data;
}

void setWorkItemsData(void *ptr, size_t size)
{
	/*work_items_data = ptr;
	work_items_size = size;*/
}

/*
* Actual built-ins implementations
*/
cl_uint CPUKernelWorkGroup::getWorkDim() const
{
	return p_work_dim;
}

size_t CPUKernelWorkGroup::getGlobalId(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 0;

	return p_global_id_start_offset[dimindx] + p_current_context->local_id[dimindx];
}

size_t CPUKernelWorkGroup::getGlobalSize(cl_uint dimindx) const
{
	if(dimindx >p_work_dim)
		return 1;

	return p_event->global_work_size(dimindx);
}

size_t CPUKernelWorkGroup::getLocalSize(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 1;

	return p_event->local_work_size(dimindx);
}

size_t CPUKernelWorkGroup::getLocalID(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 0;

	return p_current_context->local_id[dimindx];
}

size_t CPUKernelWorkGroup::getNumGroups(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 1;

	return (p_event->global_work_size(dimindx) /
		p_event->local_work_size(dimindx));
}

size_t CPUKernelWorkGroup::getGroupID(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 0;

	return p_index[dimindx];
}

size_t CPUKernelWorkGroup::getGlobalOffset(cl_uint dimindx) const
{
	if(dimindx > p_work_dim)
		return 0;

	return p_event->global_work_offset(dimindx);
}

void CPUKernelWorkGroup::barrier(unsigned int flags)
{
	p_had_barrier = true;

	// Allocate or reuse TLS memory for the stacks (it isn't freed between
	// the work groups, and even the kernels, so if we need less space than
	// allocated, it's good)
	if(!p_contexts)
	{
		if(p_current_work_item != 0)
		{
			// Completely abnormal, it means that not every work-items
			// encounter the barrier
			/*std::cerr << "*** Not every work-items of "
				<< p_kernel->function()->getNameStr()
				<< " calls barrier(); !" << std::endl;*/
			return;
		}

		// Allocate or reuse the stacks
		size_t contexts_size;
		p_contexts = getWorkItemsData(contexts_size);
		size_t needed_size = p_num_work_items * (p_stack_size + sizeof(Context));

		if(!p_contexts || contexts_size < needed_size)
		{
			// We must allocate a new space
			//if(p_contexts)
			//	munmap(p_contexts, contexts_size);

			//p_contexts = mmap(0, needed_size, PROT_EXEC | PROT_READ | PROT_WRITE, /* People say a stack must be executable */
			//	MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

			setWorkItemsData(p_contexts, contexts_size);
		}

		// Now that we have a real main context, initialize it
		p_current_context = getContextAddr(0);
		p_current_context->initialized = 1;
		std::memset(p_current_context->local_id, 0, p_work_dim * sizeof(size_t));

		//getcontext(&p_current_context->context);
	}

	// Take the next context
	p_current_work_item++;
	if(p_current_work_item == p_num_work_items) p_current_work_item = 0;

	Context *next = getContextAddr(p_current_work_item);
	Context *main = getContextAddr(0);  // The context not created with makecontext

	// If the next context isn't initialized, initialize it.
	// Note: mmap zeroes the memory, so next->initialized == 0 if it isn't initialized
	if(next->initialized == 0)
	{
		next->initialized = 1;

		// local-id of next is the one of the current context, but incVec'ed
		std::memcpy(next->local_id, p_current_context->local_id,
			MAX_WORK_DIMS * sizeof(size_t));

		incVec(p_work_dim, next->local_id, p_max_local_id);

		// Initialize the next context
		/*if(getcontext(&next->context) != 0)
			return;*/

		// Get its stack. It is located a next + sizeof(Context)
		char *stack = (char *)next;
		stack += sizeof(Context);

		/*next->context.uc_link = &main->context;
		next->context.uc_stack.ss_sp = stack;
		next->context.uc_stack.ss_size = p_stack_size;*/

		// Tell it to run the kernel function
		//makecontext(&next->context, (void(*)())p_kernel_func_addr, 1, p_args);
	}

	// Switch to the next context
	/*ucontext_t *cur = &p_current_context->context;
	p_current_context = next;

	swapcontext(cur, &next->context);*/

	// When we return here, it means that all the other work items encountered
	// a barrier and that we returned to this one. We can continue.
}

void CPUKernelWorkGroup::builtinNotFound(const std::string &name) const
{
	/*std::cout << "OpenCL: Non-existant builtin function " << name
		<< " found in kernel " << p_kernel->function()->getNameStr()
		<< '.' << std::endl;*/
}

/*
* Built-in functions
*/

static size_t get_global_id(cl_uint dimindx)
{
	return 0;// g_work_group->getGlobalId(dimindx);
}

static cl_uint get_work_dim()
{
	return 0;// g_work_group->getWorkDim();
}

static size_t get_global_size(unsigned int dimindx)
{
	return 0;// g_work_group->getGlobalSize(dimindx);
}

static size_t get_local_size(unsigned int dimindx)
{
	return 0;// g_work_group->getLocalSize(dimindx);
}

static size_t get_local_id(unsigned int dimindx)
{
	return 0;// g_work_group->getLocalID(dimindx);
}

static size_t get_num_groups(unsigned int dimindx)
{
	return 0;// g_work_group->getNumGroups(dimindx);
}

static size_t get_group_id(unsigned int dimindx)
{
	return 0;// g_work_group->getGroupID(dimindx);
}

static size_t get_global_offset(unsigned int dimindx)
{
	return 0;// g_work_group->getGlobalOffset(dimindx);
}

static void barrier(unsigned int flags)
{
	0;// g_work_group->barrier(flags);
}

// Images

static int get_image_width(Image2D *image)
{
	return image->width();
}

static int get_image_height(Image2D *image)
{
	return image->height();
}

static int get_image_depth(Image3D *image)
{
	if(image->type() != MemObject::Image3D)
		return 1;

	return image->depth();
}

static int get_image_channel_data_type(Image2D *image)
{
	return image->format().image_channel_data_type;
}

static int get_image_channel_order(Image2D *image)
{
	return image->format().image_channel_order;
}

static void *image_data(Image2D *image, int x, int y, int z, int *order, int *type)
{
	*order = image->format().image_channel_order;
	*type = image->format().image_channel_data_type;

	return 0;// g_work_group->getImageData(image, x, y, z);
}

static bool is_image_3d(Image3D *image)
{
	return (image->type() == MemObject::Image3D ? 1 : 0);
}

static void write_imagef(Image2D *image, int x, int y, int z, float *color)
{
	0;// g_work_group->writeImage(image, x, y, z, color);
}

static void write_imagei(Image2D *image, int x, int y, int z, int32_t *color)
{
	0;// g_work_group->writeImage(image, x, y, z, color);
}

static void write_imageui(Image2D *image, int x, int y, int z, uint32_t *color)
{
	0;// g_work_group->writeImage(image, x, y, z, color);
}

static void read_imagefi(float *result, Image2D *image, int x, int y, int z,
	int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

static void read_imageii(int32_t *result, Image2D *image, int x, int y, int z,
	int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

static void read_imageuii(uint32_t *result, Image2D *image, int x, int y, int z,
	int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

static void read_imageff(float *result, Image2D *image, float x, float y,
	float z, int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

static void read_imageif(int32_t *result, Image2D *image, float x, float y,
	float z, int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

static void read_imageuif(uint32_t *result, Image2D *image, float x, float y,
	float z, int32_t sampler)
{
	0;// g_work_group->readImage(result, image, x, y, z, sampler);
}

/*
* Built-in functions generated by src/runtime/builtins.py
*/

#define REPL(x) for (unsigned int i=0; i<x; ++i)

//#include <runtime/builtins_impl.h>

/*
* Bridge between LLVM and us
*/
static void unimplemented_stub()
{
}

void *getBuiltin(const std::string &name)
{
	if(name == "get_global_id")
		return (void *)&get_global_id;
	else if(name == "get_work_dim")
		return (void *)&get_work_dim;
	else if(name == "get_global_size")
		return (void *)&get_global_size;
	else if(name == "get_local_size")
		return (void *)&get_local_size;
	else if(name == "get_local_id")
		return (void *)&get_local_id;
	else if(name == "get_num_groups")
		return (void *)&get_num_groups;
	else if(name == "get_group_id")
		return (void *)&get_group_id;
	else if(name == "get_global_offset")
		return (void *)&get_global_offset;
	else if(name == "barrier")
		return (void *)&barrier;

	else if(name == "__cpu_get_image_width")
		return (void *)&get_image_width;
	else if(name == "__cpu_get_image_height")
		return (void *)&get_image_height;
	else if(name == "__cpu_get_image_depth")
		return (void *)&get_image_depth;
	else if(name == "__cpu_get_image_channel_data_type")
		return (void *)&get_image_channel_data_type;
	else if(name == "__cpu_get_image_channel_order")
		return (void *)&get_image_channel_order;
	else if(name == "__cpu_image_data")
		return (void *)&image_data;
	else if(name == "__cpu_is_image_3d")
		return (void *)&is_image_3d;
	else if(name == "__cpu_write_imagef")
		return (void *)&write_imagef;
	else if(name == "__cpu_write_imagei")
		return (void *)&write_imagei;
	else if(name == "__cpu_write_imageui")
		return (void *)&write_imageui;
	else if(name == "__cpu_read_imagefi")
		return (void *)&read_imagefi;
	else if(name == "__cpu_read_imageii")
		return (void *)&read_imageii;
	else if(name == "__cpu_read_imageuii")
		return (void *)&read_imageuii;
	else if(name == "__cpu_read_imageff")
		return (void *)&read_imageff;
	else if(name == "__cpu_read_imageif")
		return (void *)&read_imageif;
	else if(name == "__cpu_read_imageuif")
		return (void *)&read_imageuif;

	// Built-in functions generated by src/runtime/builtins.py
//#include <runtime/builtins_def.h>

	else if(name == "debug")
		return (void *)&printf;

	// Function not found
	//g_work_group->builtinNotFound(name);

	return (void *)&unimplemented_stub;
}
