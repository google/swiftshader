#ifndef __CPU_KERNEL_H__
#define __CPU_KERNEL_H__

#include "device_interface.h"

//#include <llvm/ExecutionEngine/GenericValue.h>
#include <vector>
#include <string>
#include "pthread.h"

#include <stdint.h>

#ifndef MAX_WORK_DIMS
#define MAX_WORK_DIMS 3
#endif

namespace llvm
{
	class Function;
}

namespace Devices
{

	class CPUDevice;
	class Kernel;
	class KernelEvent;
	class Image2D;
	class Image3D;

	/**
	* \brief CPU kernel
	*
	* This class holds passive information about a kernel (\c Coal::Kernel object
	* and device on which it is run) and provides the \c callFunction() function.
	*
	* This function is described at the end of \ref llvm .
	*
	* \see Coal::CPUKernelWorkGroup
	*/
	class CPUKernel : public DeviceKernel
	{
	public:
		/**
		* \brief Constructor
		* \param device device on which the kernel will be run
		* \param kernel \c Coal::Kernel object holding information about this
		*               kernel
		* \param function \c llvm::Function to run
		*/
		CPUKernel(CPUDevice *device, Kernel *kernel, llvm::Function *function);
		~CPUKernel();

		size_t workGroupSize() const;
		cl_ulong localMemSize() const;
		cl_ulong privateMemSize() const;
		size_t preferredWorkGroupSizeMultiple() const;
		size_t guessWorkGroupSize(cl_uint num_dims, cl_uint dim,
			size_t global_work_size) const;

		Kernel *kernel() const;     /*!< \brief \c Coal::Kernel object this kernel will run */
		CPUDevice *device() const;  /*!< \brief device on which the kernel will be run */

		llvm::Function *function() const;   /*!< \brief \c llvm::Function representing the kernel but <strong>not to be run</strong> */
		llvm::Function *callFunction();     /*!< \brief stub function used to run the kernel, see \ref llvm */

		/**
		* \brief Calculate where to place a value in an array
		*
		* This function is used to calculate where to place a value in an
		* array given its size, properly aligning it.
		*
		* This function is called repeatedly to obtain the aligned position of
		* each value that must be place in the array
		*
		* \code
		* size_t array_len = 0, array_offset = 0;
		* void *array;
		*
		* // First, get the array size given alignment constraints
		* typeOffset(array_len, sizeof(int));
		* typeOffset(array_len, sizeof(float));
		* typeOffset(array_len, sizeof(void *));
		*
		* // Then, allocate memory
		* array = malloc(array_len)
		*
		* // Finally, place the arguments
		* *(int *)((char *)array + typeOffset(array_offset, sizeof(int))) = 1337;
		* *(float *)((char *)array + typeOffset(array_offset, sizeof(int))) = 3.1415f;
		* *(void **)((char *)array + typeOffset(array_offset, sizeof(int))) = array;
		* \endcode
		*
		* \param offset offset at which the value will be placed. This variable
		*               gets incremented by <tt>type_len + padding</tt>.
		* \param type_len size in bytes of the value that will be stored
		* \return offset at which the value will be stored (equal to \p offset
		*         before incrementation.
		*/
		static size_t typeOffset(size_t &offset, size_t type_len);

	private:
		CPUDevice *p_device;
		Kernel *p_kernel;
		llvm::Function *p_function, *p_call_function;
		pthread_mutex_t p_call_function_mutex;
	};

	class CPUKernelEvent;

	/**
	* \brief CPU kernel work-group
	*
	* This class represent a bulk of work-items that will be run. It is the one
	* to actually run the kernel of its elements.
	*
	* \see \ref llvm
	* \nosubgrouping
	*/
	class CPUKernelWorkGroup
	{
	public:
		/**
		* \brief Constructor
		* \param kernel kernel to run
		* \param event event containing information about the kernel run
		* \param cpu_event CPU-specific information and cache about \p event
		* \param work_group_index index of this work-group in the kernel
		*/
		CPUKernelWorkGroup(CPUKernel *kernel, KernelEvent *event,
			CPUKernelEvent *cpu_event,
			const size_t *work_group_index);
		~CPUKernelWorkGroup();

		/**
		* \brief Build a structure of arguments
		*
		* As C doesn't support calling functions with variable arguments
		* unknown at the compilation, this function builds the list of
		* arguments in memory. This array will then be passed to a LLVM stub
		* function reading it and passing its values to the actuel kernel.
		*
		* \see \ref llvm
		* \param locals_to_free if this kernel takes \c __local arguments, they
		*                       must be \c malloc()'ed for every work-group.
		*                       They are placed in this vector to be
		*                       \c free()'ed at the end of \c run().
		* \return address of a memory location containing the arguments
		*/
		void *callArgs(std::vector<void *> &locals_to_free);

		/**
		* \brief Run the work-group
		*
		* This function is the core of CPU-acceleration. It runs the work-items
		* of this work-group given the correct arguments.
		*
		* \see \ref llvm
		* \see \ref barrier
		* \see callArgs()
		* \return true if success, false in case of an error
		*/
		bool run();

		/**
		* \name Native implementation of built-in OpenCL C functions
		* @{
		*/
		size_t getGlobalId(cl_uint dimindx) const;
		cl_uint getWorkDim() const;
		size_t getGlobalSize(cl_uint dimindx) const;
		size_t getLocalSize(cl_uint dimindx) const;
		size_t getLocalID(cl_uint dimindx) const;
		size_t getNumGroups(cl_uint dimindx) const;
		size_t getGroupID(cl_uint dimindx) const;
		size_t getGlobalOffset(cl_uint dimindx) const;

		void barrier(unsigned int flags);

		void *getImageData(Image2D *image, int x, int y, int z) const;

		void writeImage(Image2D *image, int x, int y, int z, float *color) const;
		void writeImage(Image2D *image, int x, int y, int z, int32_t *color) const;
		void writeImage(Image2D *image, int x, int y, int z, uint32_t *color) const;

		void readImage(float *result, Image2D *image, int x, int y, int z,
			uint32_t sampler) const;
		void readImage(int32_t *result, Image2D *image, int x, int y, int z,
			uint32_t sampler) const;
		void readImage(uint32_t *result, Image2D *image, int x, int y, int z,
			uint32_t sampler) const;

		void readImage(float *result, Image2D *image, float x, float y, float z,
			uint32_t sampler) const;
		void readImage(int32_t *result, Image2D *image, float x, float y, float z,
			uint32_t sampler) const;
		void readImage(uint32_t *result, Image2D *image, float x, float y, float z,
			uint32_t sampler) const;
		/**
		* @}
		*/

		/**
		* \brief Function called when a built-in name cannot be found
		*/
		void builtinNotFound(const std::string &name) const;

	private:
		template<typename T>
		void writeImageImpl(Image2D *image, int x, int y, int z, T *color) const;
		template<typename T>
		void readImageImplI(T *result, Image2D *image, int x, int y, int z,
			uint32_t sampler) const;
		template<typename T>
		void readImageImplF(T *result, Image2D *image, float x, float y, float z,
			uint32_t sampler) const;
		template<typename T>
		void linear3D(T *result, float a, float b, float c,
			int i0, int j0, int k0, int i1, int j1, int k1,
			Image3D *image) const;
		template<typename T>
		void linear2D(T *result, float a, float b, float c, int i0, int j0,
			int i1, int j1, Image2D *image) const;

	private:
		CPUKernel *p_kernel;
		CPUKernelEvent *p_cpu_event;
		KernelEvent *p_event;
		cl_uint p_work_dim;
		size_t p_index[MAX_WORK_DIMS],
			p_max_local_id[MAX_WORK_DIMS],
			p_global_id_start_offset[MAX_WORK_DIMS];

		void(*p_kernel_func_addr)(void *);
		void *p_args;

		// Machinery to have barrier() working
		struct Context
		{
			size_t local_id[MAX_WORK_DIMS];
			//ucontext_t context;
			unsigned int initialized;
		};

		Context *getContextAddr(unsigned int index);

		Context *p_current_context;
		Context p_dummy_context;
		void *p_contexts;
		size_t p_stack_size;
		unsigned int p_num_work_items, p_current_work_item;
		bool p_had_barrier;
	};

	/**
	* \brief CPU-specific information about a kernel event
	*
	* This class put in a \c Coal::KernelEvent device-data field
	* (see \c Coal::Event::setDeviceData()) is responsible for dispatching the
	* \c Coal::CPUKernelWorkGroup objects between the CPU worker threads.
	*/
	class CPUKernelEvent
	{
	public:
		/**
		* \brief Constructor
		* \param device device running the kernel
		* \param event \c Coal::KernelEvent holding device-agnostic data
		*              about the event
		*/
		CPUKernelEvent(CPUDevice *device, KernelEvent *event);
		~CPUKernelEvent();

		bool reserve();  /*!< \brief The next Work Group that will execute will be the last. Locks the event */
		bool finished(); /*!< \brief All the work groups have finished */
		CPUKernelWorkGroup *takeInstance(); /*!< \brief Must be called exactly one time after reserve(). Unlocks the event */

		void *kernelArgs() const;           /*!< \brief Return the cached kernel arguments */
		void cacheKernelArgs(void *args);   /*!< \brief Cache pre-built kernel arguments */

		void workGroupFinished();           /*!< \brief A work-group has just finished */

	private:
		CPUDevice *p_device;
		KernelEvent *p_event;
		size_t p_current_work_group[MAX_WORK_DIMS],
			p_max_work_groups[MAX_WORK_DIMS];
		size_t p_current_wg, p_finished_wg, p_num_wg;
		pthread_mutex_t p_mutex;
		void *p_kernel_args;
	};

}

#endif
