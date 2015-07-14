//TODO: copyrights

#ifndef __COMMANDQUEUE_H__
#define __COMMANDQUEUE_H__

#include "object.h"
#include "opencl.h"
#include "Resource.hpp"

#include <map>
#include <list>

namespace Devices
{

	class Context;
	class DeviceInterface;
	class Event;

	/**
	* \brief Command queue
	*
	* This class holds a list of events that will be pushed on a given device.
	*
	* More details are given on the \ref events page.
	*/
	class CommandQueue : public Object
	{
	public:
		CommandQueue(Context *ctx,
			DeviceInterface *device,
			cl_command_queue_properties properties,
			cl_int *errcode_ret);
		~CommandQueue();

		/**
		* \brief Queue an event
		* \param event event to be queued
		* \return \c CL_SUCCESS if success, otherwise an error code
		*/
		cl_int queueEvent(Event *event);

		/**
		* \brief Information about the command queue
		* \copydetails Coal::DeviceInterface::info
		*/
		cl_int info(cl_command_queue_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret) const;

		/**
		* \brief Set properties of the command queue
		* \note This function is deprecated and only there for OpenCL 1.0
		*       compatibility
		* \param properties property to enable or disable
		* \param enable true to enable the property, false to disable it
		* \param old_properties old value of the properties, ignored if NULL
		* \return \c CL_SUCCESS if all is good, an error code if \p properties is
		*         invalid
		*/
		cl_int setProperty(cl_command_queue_properties properties,
			cl_bool enable,
			cl_command_queue_properties *old_properties);

		/**
		* \brief Check the properties given
		* \return \c CL_SUCCESS if they are valid, an error code otherwise
		*/
		cl_int checkProperties() const;

		/**
		* \brief Push events on the device
		*
		* This function implements a big part of what is described in
		* \ref events .
		*
		* It is called by \c Coal::Event::setStatus() when an event is
		* completed, or by \c queueEvent(). Its purpose is to explore the list
		* of queued events (\c p_events) and to call
		* \c Coal::DeviceInterface::pushEvent() for each event meeting its push
		* conditions.
		*
		* \section conditions Conditions
		*
		* If the command queue has the \c CL_OUT_OF_ORDER_EXEC_MODE_ENABLE
		* property disabled, an event can be pushed only if all the previous
		* ones in the list are completed with success. This way, an event
		* must be completed before any other can be pushed. This ensures
		* in-order execution.
		*
		* If this property is enable, more complex heuristics are used.
		*
		* The event list \c p_events is explored from top to bottom. At each
		* loop iteration, checks are performed to see if the event can be pushed.
		*
		* - When a \c Coal::BarrierEvent is encountered, no more events can be
		*   pushed, except if the \c Coal::BarrierEvent is the first in the list,
		*   as that means there are no other events that can be pushed, so the
		*   barrier can go away
		* - All events that are already pushed or finished are skipped
		* - The wait list of the event is then explored to ensure that all its
		*   dependencies are met.
		* - Finally, if the events passes all the tests, it is either pushed on
		*   the device, or simply set to \c Coal::Event::Complete if it's a
		*   dummy event (see \c Coal::Event::isDummy()).
		*/
		void pushEventsOnDevice();

		/**
		* \brief Remove from the event list completed events
		*
		* This function is called periodically to clean the event list from
		* completed events.
		*
		* It is needed to do that out of \c pushEventsOnDevice() as deleting
		* event may \c dereference() this command queue, and also delete it. It
		* would produce crashes.
		*/
		void cleanEvents();

		/**
		* \brief Flush the command queue
		*
		* Pushes all the events on the device, and then return. The event
		* don't need to be completed after this call.
		*/
		void flush();

		/**
		* \brief Finish the command queue
		*
		* Pushes the events like \c flush() but also wait for them to be
		* completed before returning.
		*/
		void finish();

		/**
		* \brief Return all the events in the command queue
		* \note Retains all the events
		* \param count number of events in the event queue
		* \return events currently in the event queue
		*/
		Event **events(unsigned int &count);

	private:
		DeviceInterface *p_device;
		cl_command_queue_properties p_properties;

		std::list<Event *> p_events;
		sw::Resource *p_event_list_mutex;
		sw::Event *p_event_list_cond;
		bool p_flushed;
	};

	/**
	* \brief Base class for all events
	*
	* This class contains logic common to all the events.
	*
	* Beside handling OpenCL-specific stuff, \c Coal::Event objects do nothing
	* implementation-wise. They do not compile kernels, copy data around, etc.
	* They only contain static and immutable data that is then used by the devices
	* to actually implement the event.
	*/
	class Event : public Object
	{
	public:
		/**
		* \brief Event type
		*
		* The allows objects using \c Coal::Event to know which event it is,
		* and to cast it to the correct sub-class.
		*/
		enum Type
		{
			NDRangeKernel = CL_COMMAND_NDRANGE_KERNEL,
			TaskKernel = CL_COMMAND_TASK,
			NativeKernel = CL_COMMAND_NATIVE_KERNEL,
			ReadBuffer = CL_COMMAND_READ_BUFFER,
			WriteBuffer = CL_COMMAND_WRITE_BUFFER,
			CopyBuffer = CL_COMMAND_COPY_BUFFER,
			ReadImage = CL_COMMAND_READ_IMAGE,
			WriteImage = CL_COMMAND_WRITE_IMAGE,
			CopyImage = CL_COMMAND_COPY_IMAGE,
			CopyImageToBuffer = CL_COMMAND_COPY_IMAGE_TO_BUFFER,
			CopyBufferToImage = CL_COMMAND_COPY_BUFFER_TO_IMAGE,
			MapBuffer = CL_COMMAND_MAP_BUFFER,
			MapImage = CL_COMMAND_MAP_IMAGE,
			UnmapMemObject = CL_COMMAND_UNMAP_MEM_OBJECT,
			Marker = CL_COMMAND_MARKER,
			AcquireGLObjects = CL_COMMAND_ACQUIRE_GL_OBJECTS,
			ReleaseGLObjects = CL_COMMAND_RELEASE_GL_OBJECTS,
			ReadBufferRect = CL_COMMAND_READ_BUFFER_RECT,
			WriteBufferRect = CL_COMMAND_WRITE_BUFFER_RECT,
			CopyBufferRect = CL_COMMAND_COPY_BUFFER_RECT,
			User = CL_COMMAND_USER,
			Barrier,
			WaitForEvents
		};

		/**
		* \brief Event status
		*/
		enum Status
		{
			Queued = CL_QUEUED,       /*!< \brief Simply queued in a command queue */
			Submitted = CL_SUBMITTED, /*!< \brief Submitted to a device */
			Running = CL_RUNNING,     /*!< \brief Running on the device */
			Complete = CL_COMPLETE    /*!< \brief Completed */
		};

		/**
		* \brief Function that can be called when an event change status
		*/
		typedef void (CL_CALLBACK *event_callback)(cl_event, cl_int, void *);

		/**
		* Structure used internally by \c Coal::Event to store for each event
		* status the callbacks to call with the corresponding \c user_data.
		*/
		struct CallbackData
		{
			event_callback callback; /*!< Function to call */
			void *user_data;         /*!< Pointer to pass as its third argument */
		};

		/**
		* \brief Timing counters of an event
		*/
		enum Timing
		{
			Queue,                   /*!< Time when the event was queued */
			Submit,                  /*!< Time when the event was submitted to the device */
			Start,                   /*!< Time when its execution began on the device */
			End,                     /*!< Time when its execution finished */
			Max                      /*!< Number of items in this enum */
		};

	public:
		/**
		* \brief Constructor
		* \param parent parent \c Coal::CommandQueue
		* \param status \c Status the event has when it is created
		* \param num_events_in_wait_list number of events to wait on
		* \param event_wait_list list of events to wait on
		* \param errcode_ret return value
		*/
		Event(CommandQueue *parent,
			Status status,
			cl_uint num_events_in_wait_list,
			const Event **event_wait_list,
			cl_int *errcode_ret);

		void freeDeviceData();      /*!< \brief Call \c Coal::DeviceInterface::freeEventDeviceData() */
		virtual ~Event();           /*!< \brief Destructor */

		/**
		* \brief Type of the event
		* \return type of the event
		*/
		virtual Type type() const = 0;

		/**
		* \brief Dummy event
		*
		* A dummy event is an event that doesn't have to be pushed on a device,
		* it is only a hint for \c Coal::CommandQueue
		*
		* \return true if the event is dummy
		*/
		bool isDummy() const;

		/**
		* \brief Set the event status
		*
		* This function calls the event callbacks, and
		* \c Coal::CommandQueue::pushEventsOnDevice() if \p status is
		* \c Complete .
		*
		* \param status new status of the event
		*/
		void setStatus(Status status);

		/**
		* \brief Set device-specific data
		* \param data device-specific data
		*/
		void setDeviceData(void *data);

		/**
		* \brief Update timing info
		*
		* This function reads current system time and puts it in \c p_timing
		*
		* \param timing timing event having just finished
		*/
		void updateTiming(Timing timing);

		/**
		* \brief Status
		* \return status of the event
		*/
		Status status() const;

		/**
		* \brief Wait for a specified status
		*
		* This function blocks until the event's status is set to \p status
		* by another thread.
		*
		* \param status the status the event must have for the function to return
		*/
		void waitForStatus(Status status);

		/**
		* \brief Device-specific data
		* \return data set using \c setDeviceData()
		*/
		void *deviceData();

		/**
		* \brief List of events on which this event depends on
		* \param count number of events in the list
		* \return list of the events
		* \warning the data is not copied, it's a simple pointer to internal data
		*/
		const Event **waitEvents(cl_uint &count) const;

		/**
		* \brief Add a callback for this event
		* \param command_exec_callback_type status the event must have in order
		*        to have the callback called
		* \param callback callback function
		* \param user_data user data given to the callback
		*/
		void setCallback(cl_int command_exec_callback_type,
			event_callback callback,
			void *user_data);

		/**
		* \brief Info about the event
		* \copydetails Coal::DeviceInterface::info
		*/
		cl_int info(cl_event_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret) const;

		/**
		* \brief Profiling info
		* \copydetails Coal::DeviceInterface::info
		*/
		cl_int profilingInfo(cl_profiling_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret) const;
	private:
		cl_uint p_num_events_in_wait_list;
		const Event **p_event_wait_list;

		sw::Event *p_state_change_cond;
		sw::Resource *p_state_mutex;

		Status p_status;
		void *p_device_data;
		std::multimap<Status, CallbackData> p_callbacks;

		cl_uint p_timing[Max];
	};

}

struct _cl_command_queue : public Devices::CommandQueue
{};

struct _cl_event : public Devices::Event
{};

#endif
