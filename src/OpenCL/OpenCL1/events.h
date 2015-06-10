//TODO: copyrights

#ifndef __EVENTS_H__
#define __EVENTS_H__

#ifndef MAX_WORK_DIMS
#define MAX_WORK_DIMS 3
#endif

#include "commandqueue.h"

#include <vector>

namespace Devices
{

	class MemObject;
	class Image2D;
	class Kernel;
	class DeviceKernel;
	class DeviceInterface;

	/**
	* \brief Buffer-related event
	*/
	class BufferEvent : public Event
	{
	public:
		BufferEvent(CommandQueue *parent,
			MemObject *buffer,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		MemObject *buffer() const; /*!< \brief Buffer on which to operate */

		/**
		* \brief Check that a buffer is correctly aligned for a device
		*
		* OpenCL supports sub-buffers of buffers (\c Coal::SubBuffer). They
		* have to be aligned on a certain device-dependent boundary.
		*
		* This function checks that \p buffer is correctly aligned for
		* \p device. If \p buffer is not a \c Coal::SubBuffer, this function
		* returns true.
		*
		* \return true if the buffer is aligned or not a \c Coal::SubBuffer
		*/
		static bool isSubBufferAligned(const MemObject *buffer,
			const DeviceInterface *device);

	private:
		MemObject *p_buffer;
	};

	/**
	* \brief Reading or writing to a buffer
	*/
	class ReadWriteBufferEvent : public BufferEvent
	{
	public:
		ReadWriteBufferEvent(CommandQueue *parent,
			MemObject *buffer,
			size_t offset,
			size_t cb,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		size_t offset() const; /*!< \brief Offset in the buffer of the operation, in bytes */
		size_t cb() const;     /*!< \brief Number of bytes to read or write */
		void *ptr() const;     /*!< \brief Pointer in host memory at which to put the data */

	private:
		size_t p_offset, p_cb;
		void *p_ptr;
	};

	/**
	* \brief Reading a buffer
	*/
	class ReadBufferEvent : public ReadWriteBufferEvent
	{
	public:
		ReadBufferEvent(CommandQueue *parent,
			MemObject *buffer,
			size_t offset,
			size_t cb,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::ReadBuffer one */
	};

	/**
	* \brief Writing a buffer
	*/
	class WriteBufferEvent : public ReadWriteBufferEvent
	{
	public:
		WriteBufferEvent(CommandQueue *parent,
			MemObject *buffer,
			size_t offset,
			size_t cb,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::WriteBuffer one */
	};

	/**
	* \brief Mapping a buffer
	*/
	class MapBufferEvent : public BufferEvent
	{
	public:
		MapBufferEvent(CommandQueue *parent,
			MemObject *buffer,
			size_t offset,
			size_t cb,
			cl_map_flags map_flags,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::MapBuffer one */

		size_t offset() const;      /*!< \brief Offset in the buffer at which the mapping begins, in bytes */
		size_t cb() const;          /*!< \brief Number of bytes to map */
		cl_map_flags flags() const; /*!< \brief Flags of the mapping */
		void *ptr() const;          /*!< \brief Pointer at which the data has been mapped */

		/**
		* \brief Set the memory location at which the data has been mapped
		*
		* This function is called by the device when it has successfully mapped
		* the buffer. It must be called inside
		* \c Coal::DeviceInterface::initEventDeviceData().
		*
		* \param ptr the address at which the buffer has been mapped
		*/
		void setPtr(void *ptr);

	private:
		size_t p_offset, p_cb;
		cl_map_flags p_map_flags;
		void *p_ptr;
	};

	/**
	* \brief Mapping an image
	*/
	class MapImageEvent : public BufferEvent
	{
	public:
		MapImageEvent(CommandQueue *parent,
			Image2D *image,
			cl_map_flags map_flags,
			const size_t origin[3],
			const size_t region[3],
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::MapImage one */

		/**
		* \brief Origin of the mapping, in pixels, for the given dimension
		* \param index dimension for which the origin is retrieved
		* \return origin of the mapping for the given dimension
		*/
		size_t origin(unsigned int index) const;

		/**
		* \brief Region of the mapping, in pixels, for the given dimension
		* \param index dimension for which the region is retrieved
		* \return region of the mapping for the given dimension
		*/
		size_t region(unsigned int index) const;
		cl_map_flags flags() const; /*!< \brief Flags of the mapping */

		void *ptr() const;          /*!< \brief Pointer at which the data is mapped */
		size_t row_pitch() const;   /*!< \brief Row pitch of the mapped data */
		size_t slice_pitch() const; /*!< \brief Slice pitch of the mapped data */

		/**
		* \brief Set the memory location at which the image is mapped
		*
		* This function must be called by
		* \c Coal::DeviceInterface::initEventDeviceData(). Row and slice pitches
		* must also be set by this function by calling \c setRowPitch() and
		* \c setSlicePitch().
		*
		* \param ptr pointer at which the data is available
		*/
		void setPtr(void *ptr);
		void setRowPitch(size_t row_pitch);     /*!< \brief Set row pitch */
		void setSlicePitch(size_t slice_pitch); /*!< \brief Set slice pitch */

	private:
		cl_map_flags p_map_flags;
		size_t p_origin[3], p_region[3];
		void *p_ptr;
		size_t p_slice_pitch, p_row_pitch;
	};

	/**
	* \brief Unmapping a memory object
	*/
	class UnmapBufferEvent : public BufferEvent
	{
	public:
		UnmapBufferEvent(CommandQueue *parent,
			MemObject *buffer,
			void *mapped_addr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const;     /*!< \brief Say the event is a \c Coal::Event::UnmapBuffer one */

		void *mapping() const; /*!< \brief Mapped address to unmap */

	private:
		void *p_mapping;
	};

	/**
	* \brief Copying between two buffers
	*/
	class CopyBufferEvent : public BufferEvent
	{
	public:
		CopyBufferEvent(CommandQueue *parent,
			MemObject *source,
			MemObject *destination,
			size_t src_offset,
			size_t dst_offset,
			size_t cb,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::CopyBuffer one */

		MemObject *source() const;      /*!< \brief Source buffer, equivalent to \c Coal::BufferEvent::buffer() */
		MemObject *destination() const; /*!< \brief Destination buffer */
		size_t src_offset() const;      /*!< \brief Offset in the source buffer, in bytes */
		size_t dst_offset() const;      /*!< \brief Offset in the destination buffer, in bytes */
		size_t cb() const;              /*!< \brief Number of bytes to copy */

	private:
		MemObject *p_destination;
		size_t p_src_offset, p_dst_offset, p_cb;
	};

	/**
	* \brief Events related to rectangular (or cubic) memory regions
	*
	* This event is the base for all the *BufferRect events, and the Image ones.
	*/
	class ReadWriteCopyBufferRectEvent : public BufferEvent
	{
	public:
		ReadWriteCopyBufferRectEvent(CommandQueue *parent,
			MemObject *source,
			const size_t src_origin[3],
			const size_t dst_origin[3],
			const size_t region[3],
			size_t src_row_pitch,
			size_t src_slice_pitch,
			size_t dst_row_pitch,
			size_t dst_slice_pitch,
			unsigned int bytes_per_element,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		size_t src_origin(unsigned int index) const; /*!< \brief Source origin for the \p index dimension */
		size_t dst_origin(unsigned int index) const; /*!< \brief Destination origin for the \p index dimension */
		size_t region(unsigned int index) const;     /*!< \brief Region to copy for the \p index dimension */
		size_t src_row_pitch() const;                /*!< \brief Source row pitch */
		size_t src_slice_pitch() const;              /*!< \brief Source slice pitch */
		size_t dst_row_pitch() const;                /*!< \brief Destination row pitch */
		size_t dst_slice_pitch() const;              /*!< \brief Destination slice pitch */
		MemObject *source() const;                   /*!< \brief Source of the copy, for readability. Calls \c Coal::BufferEvent::buffer(). */

	protected:
		size_t p_src_origin[3], p_dst_origin[3], p_region[3];
		size_t p_src_row_pitch, p_src_slice_pitch;
		size_t p_dst_row_pitch, p_dst_slice_pitch;
	};

	/**
	* \brief Copying between two buffers
	*/
	class CopyBufferRectEvent : public ReadWriteCopyBufferRectEvent
	{
	public:
		CopyBufferRectEvent(CommandQueue *parent,
			MemObject *source,
			MemObject *destination,
			const size_t src_origin[3],
			const size_t dst_origin[3],
			const size_t region[3],
			size_t src_row_pitch,
			size_t src_slice_pitch,
			size_t dst_row_pitch,
			size_t dst_slice_pitch,
			unsigned int bytes_per_element,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		virtual Type type() const;      /*!< \brief Say the event is a \c Coal::Event::CopyBufferRect one */
		MemObject *destination() const; /*!< \brief Destination buffer */

	private:
		MemObject *p_destination;
	};

	/**
	* \brief Reading or writing to a buffer
	*/
	class ReadWriteBufferRectEvent : public ReadWriteCopyBufferRectEvent
	{
	public:
		ReadWriteBufferRectEvent(CommandQueue *parent,
			MemObject *buffer,
			const size_t buffer_origin[3],
			const size_t host_origin[3],
			const size_t region[3],
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			void *ptr,
			unsigned int bytes_per_element,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		void *ptr() const; /*!< \brief Pointer in host memory in which to put the data */

	private:
		void *p_ptr;
	};

	/**
	* \brief Reading a buffer
	*/
	class ReadBufferRectEvent : public ReadWriteBufferRectEvent
	{
	public:
		ReadBufferRectEvent(CommandQueue *parent,
			MemObject *buffer,
			const size_t buffer_origin[3],
			const size_t host_origin[3],
			const size_t region[3],
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::ReadBufferRect one */
	};

	/**
	* \brief Writing a buffer
	*/
	class WriteBufferRectEvent : public ReadWriteBufferRectEvent
	{
	public:
		WriteBufferRectEvent(CommandQueue *parent,
			MemObject *buffer,
			const size_t buffer_origin[3],
			const size_t host_origin[3],
			const size_t region[3],
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::WriteBufferRect one */
	};

	/**
	* \brief Reading or writing images
	*
	* This class only converts some of the arguments given to its constructor
	* to the one of \c Coal::ReadWriteBufferRectEvent. For example, the source row
	* and slice pitches are read from the \c Coal::Image2D object.
	*/
	class ReadWriteImageEvent : public ReadWriteBufferRectEvent
	{
	public:
		ReadWriteImageEvent(CommandQueue *parent,
			Image2D *image,
			const size_t origin[3],
			const size_t region[3],
			size_t row_pitch,
			size_t slice_pitch,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);
	};

	/**
	* \brief Reading an image
	*/
	class ReadImageEvent : public ReadWriteImageEvent
	{
	public:
		ReadImageEvent(CommandQueue *parent,
			Image2D *image,
			const size_t origin[3],
			const size_t region[3],
			size_t row_pitch,
			size_t slice_pitch,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::ReadImage one */
	};

	/**
	* \brief Writing to an image
	*/
	class WriteImageEvent : public ReadWriteImageEvent
	{
	public:
		WriteImageEvent(CommandQueue *parent,
			Image2D *image,
			const size_t origin[3],
			const size_t region[3],
			size_t row_pitch,
			size_t slice_pitch,
			void *ptr,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::WriteImage one */
	};

	/**
	* \brief Copying between two images
	*/
	class CopyImageEvent : public CopyBufferRectEvent
	{
	public:
		CopyImageEvent(CommandQueue *parent,
			Image2D *source,
			Image2D *destination,
			const size_t src_origin[3],
			const size_t dst_origin[3],
			const size_t region[3],
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::CopyImage one */
	};

	/**
	* \brief Copying an image to a buffer
	*/
	class CopyImageToBufferEvent : public CopyBufferRectEvent
	{
	public:
		CopyImageToBufferEvent(CommandQueue *parent,
			Image2D *source,
			MemObject *destination,
			const size_t src_origin[3],
			const size_t region[3],
			size_t dst_offset,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		size_t offset() const; /*!< \brief Offset in the buffer at which writing the image */
		Type type() const;     /*!< \brief Say the event is a \c Coal::Event::CopyImageToBuffer one */

	private:
		size_t p_offset;
	};

	/**
	* \brief Copying a buffer to an image
	*/
	class CopyBufferToImageEvent : public CopyBufferRectEvent
	{
	public:
		CopyBufferToImageEvent(CommandQueue *parent,
			MemObject *source,
			Image2D *destination,
			size_t src_offset,
			const size_t dst_origin[3],
			const size_t region[3],
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		size_t offset() const; /*!< \brief Offset in the buffer at which the copy starts */
		Type type() const;     /*!< \brief Say the event is a \c Coal::Event::CopyBufferToImage one */

	private:
		size_t p_offset;
	};

	/**
	* \brief Executing a native function as a kernel
	*
	* This event builds an argument list to give to the native function. It needs
	* for example to replace all occurence of a \c Coal::MemObject by a pointer
	* to data the host CPU can actually access, using
	* \c Coal::DeviceBuffer::nativeGlobalPointer().
	*/
	class NativeKernelEvent : public Event
	{
	public:
		NativeKernelEvent(CommandQueue *parent,
			void(*user_func)(void *),
			void *args,
			size_t cb_args,
			cl_uint num_mem_objects,
			const MemObject **mem_list,
			const void **args_mem_loc,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);
		~NativeKernelEvent();

		Type type() const;      /*!< \brief Say the event is a \c Coal::Event::NativeKernel one */

		void *function() const; /*!< \brief Host function to call */
		void *args() const;     /*!< \brief Args to give to the host function */

	private:
		void *p_user_func;
		void *p_args;
	};

	/**
	* \brief Executing a compiled kernel
	*/
	class KernelEvent : public Event
	{
	public:
		KernelEvent(CommandQueue *parent,
			Kernel *kernel,
			cl_uint work_dim,
			const size_t *global_work_offset,
			const size_t *global_work_size,
			const size_t *local_work_size,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);
		~KernelEvent();

		cl_uint work_dim() const;                     /*!< \brief Number of working dimensions */
		size_t global_work_offset(cl_uint dim) const; /*!< \brief Global work offset for the \p dim dimension */
		size_t global_work_size(cl_uint dim) const;   /*!< \brief Global work size for the \p dim dimension */
		size_t local_work_size(cl_uint dim) const;    /*!< \brief Number of work-items per work-group for the \p dim dimension */
		Kernel *kernel() const;                       /*!< \brief \c Coal::Kernel object to run */
		DeviceKernel *deviceKernel() const;           /*!< \brief \c Coal::DeviceKernel for the kernel and device of this event */

		virtual Type type() const;                    /*!< \brief Say the event is a \c Coal::Event::NDRangeKernel one */

	private:
		cl_uint p_work_dim;
		size_t p_global_work_offset[MAX_WORK_DIMS],
			p_global_work_size[MAX_WORK_DIMS],
			p_local_work_size[MAX_WORK_DIMS],
			p_max_work_item_sizes[MAX_WORK_DIMS];
		Kernel *p_kernel;
		DeviceKernel *p_dev_kernel;
	};

	/**
	* \brief Executing a task kernel
	*
	* This event is simple a \c Coal::KernelEvent with:
	*
	* - \c work_dim() set to 1
	* - \c global_work_offset() set to {0}
	* - \c global_work_size() set to {1}
	* - \c local_work_size() set to {1}
	*
	* It's in fact a \c Coal::KernelEvent containing only one single work-item.
	*/
	class TaskEvent : public KernelEvent
	{
	public:
		TaskEvent(CommandQueue *parent,
			Kernel *kernel,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::TaskKernel one */
	};

	/**
	* \brief User event
	*
	* This event is a bit special as it is created by a call to
	* \c clCreateUserEvent() and doesn't belong to an event queue. Thus, a mean had
	* to be found for all to work.
	*
	* The solution is the \c addDependentCommandQueue() function, called every time
	* the user event is added to a command queue. When this event becomes completed,
	* \c flushQueues() is called to allow all the \c Coal::CommandQueue objects
	* containing this event to push more events on their device.
	*
	* This way, command queues are not blocked by user events.
	*/
	class UserEvent : public Event
	{
	public:
		UserEvent(Context *context, cl_int *errcode_ret);

		Type type() const;        /*!< \brief Say the event is a \c Coal::Event::User one */
		Context *context() const; /*!< \brief Context of this event */
		void flushQueues();       /*!< \brief Call \c Coal::CommandQueue::pushEventsOnDevice() for each command queue in which this event is queued */

		/**
		* \brief Add a \c Coal::CommandQueue that will have to be flushed when this event becomes completed
		*
		* See the long description of this class for a complete explanation
		*
		* \param queue \c Coal::CommandQueue to add in the list of queues to flush
		*/
		void addDependentCommandQueue(CommandQueue *queue);

	private:
		Context *p_context;
		std::vector<CommandQueue *> p_dependent_queues;
	};

	/**
	* \brief Barrier event
	*/
	class BarrierEvent : public Event
	{
	public:
		BarrierEvent(CommandQueue *parent,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::Barrier one */
	};

	/**
	* \brief Event waiting for others to complete before being completed
	*/
	class WaitForEventsEvent : public Event
	{
	public:
		WaitForEventsEvent(CommandQueue *parent,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		virtual Type type() const; /*!< \brief Say the event is a \c Coal::Event::WaitForEvents one */
	};

	/**
	* \brief Marker event
	*/
	class MarkerEvent : public WaitForEventsEvent
	{
	public:
		MarkerEvent(CommandQueue *parent,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		Type type() const; /*!< \brief Say the event is a \c Coal::Event::Marker one */
	};

}

#endif
